/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */

#include <errno.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/macros.h>
#include <libavutil/mem.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
  NUM_CHANNELS = 4U
};

static AVFormatContext* g_fmt_ctx      = NULL;
static AVCodecContext* g_video_dec_ctx = NULL;
static AVCodecContext* g_audio_dec_ctx = NULL;
static int g_width                     = 0;
static int g_height                    = 0;
static enum AVPixelFormat g_pix_fmt;
static AVStream* g_video_stream         = NULL;
static AVStream* g_audio_stream         = NULL;
static const char* g_src_filename       = NULL;
static const char* g_video_dst_filename = NULL;
static const char* g_audio_dst_filename = NULL;
static FILE* g_video_dst_file           = NULL;
static FILE* g_audio_dst_file           = NULL;

static uint8_t* g_video_dst_data[NUM_CHANNELS] = {NULL};
static int g_video_dst_linesize[NUM_CHANNELS];
static int g_video_dst_bufsize = 0;

static int g_video_stream_idx  = -1;
static int g_audio_stream_idx  = -1;
static AVFrame* g_frame        = NULL;
static AVPacket* g_pkt         = NULL;
static int g_video_frame_count = 0;
static int g_audio_frame_count = 0;

static int output_video_frame(AVFrame* av_frame)
{
  if (av_frame->width != g_width || av_frame->height != g_height
      || av_frame->format != g_pix_fmt)
  {
    /* To handle this change, one could call av_image_alloc again and
     * decode the following frames into another rawvideo file. */
    fprintf(stderr,
            "Error: Width, height and pixel format have to be "
            "constant in a rawvideo file, but the width, height or "
            "pixel format of the input video changed:\n"
            "old: width = %d, height = %d, format = %s\n"
            "new: width = %d, height = %d, format = %s\n",
            g_width,
            g_height,
            av_get_pix_fmt_name(g_pix_fmt),
            av_frame->width,
            av_frame->height,
            av_get_pix_fmt_name(av_frame->format));
    return -1;
  }

  printf("video_frame n:%d\n", g_video_frame_count++);

  /* copy decoded frame to destination buffer:
   * this is required since rawvideo expects non aligned data */
  av_image_copy(g_video_dst_data,
                g_video_dst_linesize,
                (const uint8_t**)(av_frame->data),
                av_frame->linesize,
                g_pix_fmt,
                g_width,
                g_height);

  /* write to rawvideo file */
  if (fwrite(g_video_dst_data[0], 1, g_video_dst_bufsize, g_video_dst_file) == EOF)
  {
    puts("fwrite error");
  }
  return 0;
}

static int output_audio_frame(AVFrame* av_frame)
{
  size_t unpadded_linesize = av_frame->nb_samples
                           * av_get_bytes_per_sample(av_frame->format);
  printf("audio_frame n:%d nb_samples:%d pts:%s\n",
         g_audio_frame_count++,
         av_frame->nb_samples,
         av_ts2timestr(av_frame->pts, &g_audio_dec_ctx->time_base));

  /* Write the raw audio data samples of the first plane. This works
   * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
   * most audio decoders output planar audio, which uses a separate
   * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
   * In other words, this code will write only the first audio channel
   * in these cases.
   * You should use libswresample or libavfilter to convert the frame
   * to packed data. */
  if (fwrite(av_frame->extended_data[0], 1, unpadded_linesize, g_audio_dst_file) == EOF)
  {
    puts("fwrite error");
  }

  return 0;
}

// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
static int decode_packet(AVCodecContext* dec, const AVPacket* pkt)
{
  int ret = 0;

  // submit the packet to the decoder
  ret = avcodec_send_packet(dec, pkt);
  if (ret < 0)
  {
    fprintf(stderr, "Error submitting a packet for decoding (%s)\n", av_err2str(ret));
    return ret;
  }

  // get all the available frames from the decoder
  while (ret >= 0)
  {
    ret = avcodec_receive_frame(dec, g_frame);
    if (ret < 0)
    {
      // those two return values are special and mean there is no output
      // frame available, but there were no errors during decoding
      if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
      {
        return 0;
      }

      fprintf(stderr, "Error during decoding (%s)\n", av_err2str(ret));
      return ret;
    }

    // write the frame data to output file
    if (dec->codec->type == AVMEDIA_TYPE_VIDEO)
    {
      ret = output_video_frame(g_frame);
    }
    else
    {
      ret = output_audio_frame(g_frame);
    }

    av_frame_unref(g_frame);
    if (ret < 0)
    {
      return ret;
    }
  }

  return 0;
}

static int open_codec_context(int* stream_idx,
                              AVCodecContext** dec_ctx,
                              AVFormatContext* fmt_ctx,
                              enum AVMediaType type)
{
  int ret            = 0;
  int stream_index   = 0;
  AVStream* st       = NULL;
  const AVCodec* dec = NULL;

  ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
  if (ret < 0)
  {
    fprintf(stderr,
            "Could not find %s stream in input file '%s'\n",
            av_get_media_type_string(type),
            g_src_filename);
    return ret;
  }

  stream_index = ret;
  st           = fmt_ctx->streams[stream_index];

  /* find decoder for the stream */
  dec = avcodec_find_decoder(st->codecpar->codec_id);
  if (!dec)
  {
    fprintf(stderr, "Failed to find %s codec\n", av_get_media_type_string(type));
    return AVERROR(EINVAL);
  }

  /* Allocate a codec context for the decoder */
  *dec_ctx = avcodec_alloc_context3(dec);
  if (!*dec_ctx)
  {
    fprintf(stderr,
            "Failed to allocate the %s codec context\n",
            av_get_media_type_string(type));
    return AVERROR(ENOMEM);
  }

  /* Copy codec parameters from input stream to output codec context */
  if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0)
  {
    fprintf(stderr,
            "Failed to copy %s codec parameters to decoder context\n",
            av_get_media_type_string(type));
    return ret;
  }

  /* Init the decoders */
  if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0)
  {
    fprintf(stderr, "Failed to open %s codec\n", av_get_media_type_string(type));
    return ret;
  }
  *stream_idx = stream_index;

  return 0;
}

static int get_format_from_sample_fmt(const char** fmt, enum AVSampleFormat sample_fmt)
{
  struct sample_fmt_entry
  {
      enum AVSampleFormat sample_fmt;
      const char *fmt_be, *fmt_le;
  } sample_fmt_entries[] = {
      { AV_SAMPLE_FMT_U8,    "u8",    "u8"},
      {AV_SAMPLE_FMT_S16, "s16be", "s16le"},
      {AV_SAMPLE_FMT_S32, "s32be", "s32le"},
      {AV_SAMPLE_FMT_FLT, "f32be", "f32le"},
      {AV_SAMPLE_FMT_DBL, "f64be", "f64le"},
  };

  *fmt = NULL;

  for (int i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++)
  {
    struct sample_fmt_entry* entry = &sample_fmt_entries[i];
    if (sample_fmt == entry->sample_fmt)
    {
      *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
      return 0;
    }
  }

  fprintf(stderr,
          "sample format %s is not supported as output format\n",
          av_get_sample_fmt_name(sample_fmt));
  return -1;
}

enum CLIArg_Enum
{
  CLIArg_Executable = 0,
  CLIArg_SourceFile = 1,
  CLIArg_VideoOut   = 2,
  CLIArg_AudioOut   = 3,
  CLIArg_NumArgs    = 4
};

// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
int main(int argc, char** argv)
{
  int ret = 0;

  if (argc != CLIArg_NumArgs)
  {
    fprintf(stderr,
            "usage: %s  input_file video_output_file audio_output_file\n"
            "API example program to show how to read frames from an input file.\n"
            "This program reads frames from a file, decodes them, and writes decoded\n"
            "video frames to a rawvideo file named video_output_file, and decoded\n"
            "audio frames to a rawaudio file named audio_output_file.\n",
            argv[CLIArg_Executable]);
    exit(1);
  }
  g_src_filename       = argv[CLIArg_SourceFile];
  g_video_dst_filename = argv[CLIArg_VideoOut];
  g_audio_dst_filename = argv[CLIArg_AudioOut];

  /* open input file, and allocate format context */
  if (avformat_open_input(&g_fmt_ctx, g_src_filename, NULL, NULL) < 0)
  {
    fprintf(stderr, "Could not open source file %s\n", g_src_filename);
    exit(1);
  }

  /* retrieve stream information */
  if (avformat_find_stream_info(g_fmt_ctx, NULL) < 0)
  {
    fprintf(stderr, "Could not find stream information\n");
    exit(1);
  }

  if (open_codec_context(&g_video_stream_idx,
                         &g_video_dec_ctx,
                         g_fmt_ctx,
                         AVMEDIA_TYPE_VIDEO)
      >= 0)
  {
    g_video_stream = g_fmt_ctx->streams[g_video_stream_idx];

    g_video_dst_file = fopen(g_video_dst_filename, "wb");
    if (!g_video_dst_file)
    {
      fprintf(stderr, "Could not open destination file %s\n", g_video_dst_filename);
      ret = 1;
      goto end;
    }

    /* allocate image where the decoded image will be put */
    g_width   = g_video_dec_ctx->width;
    g_height  = g_video_dec_ctx->height;
    g_pix_fmt = g_video_dec_ctx->pix_fmt;
    ret       = av_image_alloc(g_video_dst_data,
                         g_video_dst_linesize,
                         g_width,
                         g_height,
                         g_pix_fmt,
                         1);
    if (ret < 0)
    {
      fprintf(stderr, "Could not allocate raw video buffer\n");
      goto end;
    }
    g_video_dst_bufsize = ret;
  }

  if (open_codec_context(&g_audio_stream_idx,
                         &g_audio_dec_ctx,
                         g_fmt_ctx,
                         AVMEDIA_TYPE_AUDIO)
      >= 0)
  {
    g_audio_stream   = g_fmt_ctx->streams[g_audio_stream_idx];
    g_audio_dst_file = fopen(g_audio_dst_filename, "wb");
    if (!g_audio_dst_file)
    {
      fprintf(stderr, "Could not open destination file %s\n", g_audio_dst_filename);
      ret = 1;
      goto end;
    }
  }

  /* dump input information to stderr */
  av_dump_format(g_fmt_ctx, 0, g_src_filename, 0);

  if (!g_audio_stream && !g_video_stream)
  {
    fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
    ret = 1;
    goto end;
  }

  g_frame = av_frame_alloc();
  if (!g_frame)
  {
    fprintf(stderr, "Could not allocate frame\n");
    ret = AVERROR(ENOMEM);
    goto end;
  }

  g_pkt = av_packet_alloc();
  if (!g_pkt)
  {
    fprintf(stderr, "Could not allocate packet\n");
    ret = AVERROR(ENOMEM);
    goto end;
  }

  if (g_video_stream)
  {
    printf("Demuxing video from file '%s' into '%s'\n",
           g_src_filename,
           g_video_dst_filename);
  }
  if (g_audio_stream)
  {
    printf("Demuxing audio from file '%s' into '%s'\n",
           g_src_filename,
           g_audio_dst_filename);
  }

  /* read frames from the file */
  while (av_read_frame(g_fmt_ctx, g_pkt) >= 0)
  {
    // check if the packet belongs to a stream we are interested in, otherwise
    // skip it
    if (g_pkt->stream_index == g_video_stream_idx)
    {
      ret = decode_packet(g_video_dec_ctx, g_pkt);
    }
    else if (g_pkt->stream_index == g_audio_stream_idx)
    {
      ret = decode_packet(g_audio_dec_ctx, g_pkt);
    }
    av_packet_unref(g_pkt);
    if (ret < 0)
    {
      break;
    }
  }

  /* flush the decoders */
  if (g_video_dec_ctx)
  {
    decode_packet(g_video_dec_ctx, NULL);
  }
  if (g_audio_dec_ctx)
  {
    decode_packet(g_audio_dec_ctx, NULL);
  }

  printf("Demuxing succeeded.\n");

  if (g_video_stream)
  {
    printf("Play the output video file with the command:\n"
           "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
           av_get_pix_fmt_name(g_pix_fmt),
           g_width,
           g_height,
           g_video_dst_filename);
  }

  if (g_audio_stream)
  {
    enum AVSampleFormat sfmt = g_audio_dec_ctx->sample_fmt;
    int n_channels           = g_audio_dec_ctx->ch_layout.nb_channels;
    const char* fmt          = NULL;

    if (av_sample_fmt_is_planar(sfmt))
    {
      const char* packed = av_get_sample_fmt_name(sfmt);
      printf("Warning: the sample format the decoder produced is planar "
             "(%s). This example will output the first channel only.\n",
             packed ? packed : "?");
      sfmt       = av_get_packed_sample_fmt(sfmt);
      n_channels = 1;
    }

    ret = get_format_from_sample_fmt(&fmt, sfmt);
    if (ret < 0)
    {
      goto end;
    }

    if (g_audio_dec_ctx)
    {
      printf("Play the output audio file with the command:\n"
             "ffplay -f %s -ac %d -ar %d %s\n",
             fmt,
             n_channels,
             g_audio_dec_ctx->sample_rate,
             g_audio_dst_filename);
    }
  }

end:
  avcodec_free_context(&g_video_dec_ctx);
  avcodec_free_context(&g_audio_dec_ctx);
  avformat_close_input(&g_fmt_ctx);
  if (g_video_dst_file)
  {
    if (fclose(g_video_dst_file) == EOF)
    {
      puts("fclose(video_dst_file) error");
    }
  }
  if (g_audio_dst_file)
  {
    if (fclose(g_audio_dst_file) == EOF)
    {
      puts("fclose(audio_dst_file) error");
    }
  }
  av_packet_free(&g_pkt);
  av_frame_free(&g_frame);
  av_free(g_video_dst_data[0]);

  return ret < 0;
}
