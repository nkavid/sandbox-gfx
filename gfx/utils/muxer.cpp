/*
 * Copyright (c) 2003 Fabrice Bellard
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

#include "muxer.hpp"

#include "utils/logger.hpp"
#include "vocabulary/time.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavcodec/codec_id.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/dict.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/mathematics.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
#include <libavutil/timestamp.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include "utils/libav_string_fix.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdio>

namespace gfx::utils::video
{
namespace
{
struct Size
{
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    Size(int widthParam, int heightParam)
        : width{widthParam},
          height{heightParam}
    {}

    explicit Size(const gfx::Size& size)
        : width{static_cast<int>(size.width)},
          height{static_cast<int>(size.height)}
    {}

    int width{};
    int height{};
};

struct OutputStream
{
    AVStream* st{nullptr};
    AVCodecContext* enc{nullptr};

    int64_t next_pts{};
    int samples_count{};

    AVFrame* frame{nullptr};
    AVFrame* tmp_frame{nullptr};

    AVPacket* tmp_pkt{nullptr};

    float t{};
    float tincr{};
    float tincr2{};

    SwsContext* sws_ctx{nullptr};
    SwrContext* swr_ctx{nullptr};
};

int write_frame(AVFormatContext* fmt_ctx,
                AVCodecContext* codecContext,
                AVStream* stream,
                AVFrame* frame,
                AVPacket* pkt)
{
  int ret{};
  ret = avcodec_send_frame(codecContext, frame);
  if (ret < 0)
  {
    logger::fatal("Error sending a frame to the encoder: ", av_err2str(ret));
  }

  while (ret >= 0)
  {
    ret = avcodec_receive_packet(codecContext, pkt);

    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
    {
      break;
    }

    if (ret < 0) [[unlikely]]
    {
      logger::fatal("Error encoding a frame: ", av_err2str(ret));
    }

    av_packet_rescale_ts(pkt, codecContext->time_base, stream->time_base);
    pkt->stream_index = stream->index;

    ret = av_interleaved_write_frame(fmt_ctx, pkt);
    if (ret < 0)
    {
      logger::fatal("Error while writing output packet: ", av_err2str(ret));
    }
  }

  return ret == AVERROR_EOF ? 1 : 0;
}

// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
void add_stream(OutputStream* ost,
                AVFormatContext* formatContext,
                const AVCodec** codec,
                AVCodecID codec_id,
                const Size& size,
                uint32_t frameRate)
{
  AVCodecContext* codecContext{nullptr};
  *codec = avcodec_find_encoder(codec_id);

  if (*codec == nullptr)
  {
    logger::fatal("Could not find encoder for ", avcodec_get_name(codec_id));
  }

  ost->tmp_pkt = av_packet_alloc();
  if (ost->tmp_pkt == nullptr)
  {
    logger::fatal("Could not allocate AVPacket");
  }

  ost->st = avformat_new_stream(formatContext, nullptr);

  if (ost->st == nullptr)
  {
    logger::fatal("Could not allocate stream");
  }

  ost->st->id  = static_cast<int>(formatContext->nb_streams - 1);
  codecContext = avcodec_alloc_context3(*codec);

  if (codecContext == nullptr)
  {
    logger::fatal("Could not alloc an encoding context");
  }

  ost->enc = codecContext;

  switch ((*codec)->type)
  {
  case AVMEDIA_TYPE_VIDEO:
    codecContext->codec_id = codec_id;

    codecContext->bit_rate  = 400000; // NOLINT(readability-magic-numbers)
    codecContext->width     = size.width;
    codecContext->height    = size.height;
    ost->st->time_base      = AVRational{1, static_cast<int>(frameRate)};
    codecContext->time_base = ost->st->time_base;

    codecContext->gop_size = 12; // NOLINT(readability-magic-numbers)
    codecContext->pix_fmt  = AV_PIX_FMT_YUV420P;
    if (codecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO)
    {
      codecContext->max_b_frames = 2; // NOLINT(readability-magic-numbers)
    }
    if (codecContext->codec_id == AV_CODEC_ID_MPEG1VIDEO)
    {
      codecContext->mb_decision = 2; // NOLINT(readability-magic-numbers)
    }
    break;

  default:
    break;
  }

  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  if ((formatContext->oformat->flags & AVFMT_GLOBALHEADER) != 0)
  {
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }
}

AVFrame* alloc_picture(AVPixelFormat pix_fmt, const Size& size)
{
  AVFrame* picture{nullptr};
  int ret{};

  picture = av_frame_alloc();
  if (picture == nullptr) [[unlikely]]
  {
    return nullptr;
  }

  picture->format = pix_fmt;
  picture->width  = size.width;
  picture->height = size.height;

  ret = av_frame_get_buffer(picture, 0);
  if (ret < 0) [[unlikely]]
  {
    logger::fatal("Could not allocate frame data");
  }

  return picture;
}

void open_video(AVFormatContext* /*oc*/,
                const AVCodec* codec,
                OutputStream* ost,
                AVDictionary* opt_arg)
{
  int ret{};
  AVCodecContext* codecContext{ost->enc};
  AVDictionary* opt{nullptr};

  av_dict_copy(&opt, opt_arg, 0);

  ret = avcodec_open2(codecContext, codec, &opt);
  av_dict_free(&opt);
  if (ret < 0) [[unlikely]]
  {
    logger::fatal("Could not open video codec: ", av_err2str(ret));
  }

  ost->frame = alloc_picture(codecContext->pix_fmt,
                             Size{codecContext->width, codecContext->height});
  if (ost->frame == nullptr) [[unlikely]]
  {
    logger::fatal("Could not allocate video frame");
  }

  ost->tmp_frame = nullptr;
  if (codecContext->pix_fmt != AV_PIX_FMT_YUV420P)
  {
    ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P,
                                   Size{codecContext->width, codecContext->height});
    if (ost->tmp_frame == nullptr) [[unlikely]]
    {
      logger::fatal("Could not allocate temporary picture");
    }
  }

  ret = avcodec_parameters_from_context(ost->st->codecpar, codecContext);
  if (ret < 0) [[unlikely]]
  {
    logger::fatal("Could not copy the stream parameters");
  }
}

void fill_yuv_image(AVFrame* pict, int64_t frameIndex, const Size& size)
{
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic,
  // readability-identifier-length)
  for (int y = 0; y < size.height; y++)
  {
    for (int x = 0; x < size.width; x++)
    {
      const auto lum = static_cast<uint8_t>(x + y + frameIndex * 3);
      pict->data[0][y * pict->linesize[0] + x] = lum;
    }
  }

  for (int y = 0; y < size.height / 2; y++)
  {
    for (int x = 0; x < size.width / 2; x++)
    {
      const auto cb = static_cast<uint8_t>(128 + y + frameIndex * 2);
      pict->data[1][y * pict->linesize[1] + x] = cb;
      const auto cr = static_cast<uint8_t>(64 + x + frameIndex * 5);
      pict->data[2][y * pict->linesize[2] + x] = cr;
    }
  }
  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic,
  // readability-identifier-length)
}

AVFrame* get_video_frame(OutputStream* ost, int64_t duration)
{
  AVCodecContext* codecContext{ost->enc};

  if (av_compare_ts(ost->next_pts, codecContext->time_base, duration, AVRational{1, 1})
      > 0)
  {
    return nullptr;
  }

  if (av_frame_make_writable(ost->frame) < 0) [[unlikely]]
  {
    logger::fatal("Frame not writable");
  }

  fill_yuv_image(ost->frame,
                 ost->next_pts,
                 Size{codecContext->width, codecContext->height});

  ost->frame->pts = ost->next_pts++;

  return ost->frame;
}

int write_video_frame(AVFormatContext* formatContext,
                      OutputStream* ost,
                      int64_t duration)
{
  return write_frame(formatContext,
                     ost->enc,
                     ost->st,
                     get_video_frame(ost, duration),
                     ost->tmp_pkt);
}

void close_stream(AVFormatContext* /*oc*/, OutputStream* ost)
{
  avcodec_free_context(&ost->enc);
  av_frame_free(&ost->frame);
  av_frame_free(&ost->tmp_frame);
  av_packet_free(&ost->tmp_pkt);
  sws_freeContext(ost->sws_ctx);
  swr_free(&ost->swr_ctx);
}
}

// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
Muxer::Muxer(gfx::URI uri,
             gfx::Size size,
             gfx::time::ms duration,
             gfx::time::fps frameRate)
    : _uri{uri},
      _size{size},
      _duration{duration},
      _frameRate{frameRate}
{
  OutputStream video_st{nullptr};
  const AVOutputFormat* fmt{nullptr};
  const char* filename{nullptr};
  AVFormatContext* outputFormatContext{nullptr};
  const AVCodec* video_codec{nullptr};
  int ret{};
  int have_video{0};
  int encode_video{0};
  AVDictionary* opt{nullptr};

  filename = _uri.c_str();

  avformat_alloc_output_context2(&outputFormatContext, nullptr, nullptr, filename);
  if (outputFormatContext == nullptr) [[unlikely]]
  {
    puts("Could not deduce output format from file extension: using mpegts.");
    avformat_alloc_output_context2(&outputFormatContext, nullptr, "mpegts", filename);
  }

  if (outputFormatContext == nullptr) [[unlikely]]
  {
    logger::fatal("outputFormatContext == nullptr");
  }

  fmt = outputFormatContext->oformat;

  if (fmt->video_codec != AV_CODEC_ID_NONE) [[likely]]
  {
    logger::info("Not deducing codec from format context, using: ",
                 avcodec_get_name(AV_CODEC_ID_H264));

    add_stream(&video_st,
               outputFormatContext,
               &video_codec,
               AV_CODEC_ID_H264,
               Size(_size),
               _frameRate);

    have_video   = 1;
    encode_video = 1;
  }

  if (have_video != 0) [[likely]]
  {
    open_video(outputFormatContext, video_codec, &video_st, opt);
  }

  av_dump_format(outputFormatContext, 0, filename, 1);

  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  if ((fmt->flags & AVFMT_NOFILE) == 0)
  {
    ret = avio_open(&outputFormatContext->pb, filename, AVIO_FLAG_WRITE);
    if (ret < 0) [[unlikely]]
    {
      logger::fatal("Could not open: ", filename);
    }
  }

  ret = avformat_write_header(outputFormatContext, &opt);
  if (ret < 0) [[unlikely]]
  {
    logger::fatal("Error occurred when opening output file: ", av_err2str(ret));
  }

  while (encode_video != 0)
  {
    constexpr uint32_t ms_to_sec{1000};
    encode_video = static_cast<int>(
        !static_cast<bool>(write_video_frame(outputFormatContext,
                                             &video_st,
                                             _duration.count() / ms_to_sec)));
  }
  av_write_trailer(outputFormatContext);

  if (have_video != 0)
  {
    close_stream(outputFormatContext, &video_st);
  }
  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  if ((fmt->flags & AVFMT_NOFILE) == 0)
  {
    avio_closep(&outputFormatContext->pb);
  }

  avformat_free_context(outputFormatContext);
}
}
