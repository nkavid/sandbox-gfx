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

#include <fmt/core.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/codec.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/mem.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
#include <libavutil/timestamp.h>
}

#include "libav_string_fix.hpp"

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <optional>
#include <span>

namespace gfx
{
// NOLINTBEGIN
static AVFormatContext* g_fmt_ctx{nullptr};
static AVCodecContext* g_video_dec_ctx{nullptr};

static int g_width{};
static int g_height{};

static AVPixelFormat g_pix_fmt{};

static AVStream* g_video_stream{nullptr};

static const char* g_src_filename{nullptr};
static const char* g_video_dst_filename{nullptr};

static void close_file(FILE* filePtr)
{
  fclose(filePtr);
}

using UniqueFile = std::unique_ptr<FILE, decltype(&close_file)>;

static UniqueFile makeUniqueFile(const char* filename, const char* mode)
{
  return UniqueFile(fopen(filename, mode), &close_file);
}

// TODO clang-diagnostic-exit-time-destructors
// TODO clang-diagnostic-global-constructors
static std::optional<UniqueFile> g_video_dst_file{std::nullopt};

static uint8_t* g_video_dst_data[4] = {nullptr};
static int g_video_dst_linesize[4];
static size_t g_video_dst_bufsize;

static int g_video_stream_idx = -1;
static AVFrame* g_frame{nullptr};
static AVPacket* g_pkt{nullptr};
static int g_video_frame_count = 0;

// NOLINTEND
namespace
{
int output_video_frame(AVFrame* frame)
{
  if (frame->width != g_width || frame->height != g_height
      || frame->format != g_pix_fmt)
  {
    fmt::print(stderr,
               "Error: Width, height and pixel format have to be "
               "constant in a rawvideo file, but the width, height or "
               "pixel format of the input video changed:\n"
               "old: width = %d, height = %d, format = %s\n"
               "new: width = %d, height = %d, format = %s\n",
               g_width,
               g_height,
               av_get_pix_fmt_name(g_pix_fmt),
               frame->width,
               frame->height,
               av_get_pix_fmt_name(static_cast<AVPixelFormat>(frame->format)));
    return -1;
  }

  fmt::print("video_frame n:%d\n", g_video_frame_count++);

  av_image_copy(static_cast<uint8_t**>(g_video_dst_data),
                static_cast<int*>(g_video_dst_linesize),
                const_cast<const uint8_t**>(frame->data), // NOLINT
                static_cast<int*>(frame->linesize),
                g_pix_fmt,
                g_width,
                g_height);

  // NOLINTNEXTLINE
  fwrite(g_video_dst_data[0], 1, g_video_dst_bufsize, g_video_dst_file.value().get());
  return 0;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity,readability-function-size)
int decode_packet(AVCodecContext* dec, const AVPacket* pkt)
{
  int ret = 0;

  ret = avcodec_send_packet(dec, pkt);
  if (ret < 0)
  {
    fmt::print(stderr,
               "Error submitting a packet for decoding (%s)\n",
               av_err2str(ret));
    return ret;
  }

  while (ret >= 0)
  {
    ret = avcodec_receive_frame(dec, g_frame);
    if (ret < 0)
    {
      if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
      {
        return 0;
      }

      fmt::print(stderr, "Error during decoding (%s)\n", av_err2str(ret));
      return ret;
    }

    if (dec->codec->type == AVMEDIA_TYPE_VIDEO)
    {
      ret = output_video_frame(g_frame);
    }

    av_frame_unref(g_frame);
    if (ret < 0)
    {
      return ret;
    }
  }

  return 0;
}

// NOLINTNEXTLINE(readability-function-size)
int open_codec_context(int* stream_idx,
                       AVCodecContext** dec_ctx,
                       AVFormatContext* fmt_ctx,
                       AVMediaType type)
{
  int ret          = 0;
  int stream_index = 0;
  AVStream* stream{nullptr};
  const AVCodec* dec{nullptr};

  ret = av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
  if (ret < 0)
  {
    fmt::print(stderr,
               "Could not find %s stream in input file '%s'\n",
               av_get_media_type_string(type),
               g_src_filename);
    return ret;
  }

  stream_index = ret;
  stream       = *std::next(fmt_ctx->streams, stream_index);

  dec = avcodec_find_decoder(stream->codecpar->codec_id);
  if (dec == nullptr)
  {
    fmt::print(stderr, "Failed to find %s codec\n", av_get_media_type_string(type));
    return AVERROR(EINVAL);
  }

  *dec_ctx = avcodec_alloc_context3(dec);
  if (*dec_ctx == nullptr)
  {
    fmt::print(stderr,
               "Failed to allocate the %s codec context\n",
               av_get_media_type_string(type));
    return AVERROR(ENOMEM);
  }

  ret = avcodec_parameters_to_context(*dec_ctx, stream->codecpar);
  if (ret < 0)
  {
    fmt::print(stderr,
               "Failed to copy %s codec parameters to decoder context\n",
               av_get_media_type_string(type));
    return ret;
  }

  ret = avcodec_open2(*dec_ctx, dec, nullptr);
  if (ret < 0)
  {
    fmt::print(stderr, "Failed to open %s codec\n", av_get_media_type_string(type));
    return ret;
  }
  *stream_idx = stream_index;

  return 0;
}

// NOLINTNEXTLINE(readability-function-size, readability-function-cognitive-complexity)
bool create_resources()
{
  int ret = 0;
  if (open_codec_context(&g_video_stream_idx,
                         &g_video_dec_ctx,
                         g_fmt_ctx,
                         AVMEDIA_TYPE_VIDEO)
      >= 0)
  {
    g_video_stream = *std::next(g_fmt_ctx->streams, g_video_stream_idx);

    g_video_dst_file = makeUniqueFile(g_video_dst_filename, "wb");

    if (g_video_dst_file == nullptr)
    {
      fmt::print(stderr, "Could not open destination file %s\n", g_video_dst_filename);
      return false;
    }

    g_width   = g_video_dec_ctx->width;
    g_height  = g_video_dec_ctx->height;
    g_pix_fmt = g_video_dec_ctx->pix_fmt;

    ret = av_image_alloc(std::span<uint8_t*>(g_video_dst_data).data(),
                         std::span<int>(g_video_dst_linesize).data(),
                         g_width,
                         g_height,
                         g_pix_fmt,
                         1);
    if (ret < 0)
    {
      fmt::print(stderr, "Could not allocate raw video buffer\n");
      return false;
    }
    g_video_dst_bufsize = static_cast<size_t>(ret);
  }

  av_dump_format(g_fmt_ctx, 0, g_src_filename, 0);

  if (g_video_stream == nullptr)
  {
    fmt::print(stderr, "Could not find video stream in the input, aborting\n");
    return false;
  }

  g_frame = av_frame_alloc();
  if (g_frame == nullptr)
  {
    fmt::print(stderr, "Could not allocate frame\n");
    return false;
  }

  g_pkt = av_packet_alloc();
  if (g_pkt == nullptr)
  {
    fmt::print(stderr, "Could not allocate packet\n");
    return false;
  }

  if (g_video_stream != nullptr)
  {
    fmt::print("Demuxing video from file '%s' into '%s'\n",
               g_src_filename,
               g_video_dst_filename);
  }

  return true;
}

void destroy_resources()
{
  avcodec_free_context(&g_video_dec_ctx);
  avformat_close_input(&g_fmt_ctx);

  av_packet_free(&g_pkt);
  av_frame_free(&g_frame);
  av_free(g_video_dst_data[0]);
}

namespace cli_arg
{
enum CLIArg
{
  Executable = 0,
  SourceFile = 1,
  VideoOut   = 2,
  NumArgs    = 3
};
} // namespace cli_arg

// NOLINTNEXTLINE(clang-diagnostic-unsafe-buffer-usage)
bool parse_args(int argc, const char** argv)
{
  if (argc != cli_arg::NumArgs)
  {
    fmt::print(
        stderr,
        "usage: %s [input_file] [video_output_file]\n"
        "API example program to show how to read frames from an input file.\n"
        "This program reads frames from a file, decodes them, and writes decoded\n"
        "video frames to a rawvideo file named video_output_file.\n",
        *std::next(argv, cli_arg::Executable));
    return false;
  }
  g_src_filename       = *std::next(argv, cli_arg::SourceFile);
  g_video_dst_filename = *std::next(argv, cli_arg::VideoOut);

  return true;
}

} // namespace
} // namespace gfx

// NOLINTNEXTLINE(readability-function-size, readability-function-cognitive-complexity)
int main(int argc, const char** argv)
{
  using namespace gfx;

  if (!parse_args(argc, argv))
  {
    return EXIT_FAILURE;
  }

  if (avformat_open_input(&g_fmt_ctx, g_src_filename, nullptr, nullptr) < 0)
  {
    fmt::print(stderr, "Could not open source file %s\n", g_src_filename);
    return EXIT_FAILURE;
  }

  if (avformat_find_stream_info(g_fmt_ctx, nullptr) < 0)
  {
    fmt::print(stderr, "Could not find stream information\n");
    return EXIT_FAILURE;
  }

  int ret = 0;

  if (!create_resources())
  {
    ret = AVERROR(ENOMEM);
    goto end; // NOLINT(cppcoreguidelines-avoid-goto)
  }

  while (av_read_frame(g_fmt_ctx, g_pkt) >= 0)
  {
    if (g_pkt->stream_index == g_video_stream_idx)
    {
      ret = decode_packet(g_video_dec_ctx, g_pkt);
    }
    av_packet_unref(g_pkt);
    if (ret < 0)
    {
      break;
    }
  }

  if (g_video_dec_ctx != nullptr)
  {
    decode_packet(g_video_dec_ctx, nullptr);
  }

  fmt::print("Demuxing succeeded.\n");

  if (g_video_stream != nullptr)
  {
    fmt::print("Play the output video file with the command:\n"
               "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s\n",
               av_get_pix_fmt_name(g_pix_fmt),
               g_width,
               g_height,
               g_video_dst_filename);
  }

end:
  destroy_resources();

  return static_cast<int>(ret < 0);
}
