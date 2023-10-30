extern "C" {
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/avcodec.h>
}

#include <string>

#include "internal/handlers.hpp"
#include "internal/frame.hpp"
#include "internal/packet.hpp"
#include "avcpp/utils.hpp"

using namespace avcpp;

#define CLEANUP_AND_ADIOS(format, decoder, code)                        \
  do {                                                                  \
    avformat_close_input(format);                                       \
    avcodec_close(decoder);                                             \
    std::cerr << __FUNCTION__ << ": " << getErrorMessage(code) << '\n'; \
    return Frame();                                                     \
  } while (0)

Frame avcpp::imread(const char* path) {
  AVFormatContext* ctx = nullptr;
  int ret = 0;
  int foundVideo = -1;
  Packet compressed;
  Frame original;

  ret = avformat_open_input(&ctx, path, nullptr, nullptr);
  if (ret < 0) CLEANUP_AND_ADIOS(&ctx, nullptr, ret);

  // * -------- Find the video source in file -------- *

  for (int i = 0; i < ctx->nb_streams && foundVideo < 0; ++i)
    if (ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
      foundVideo = i;

  if (foundVideo < 0) CLEANUP_AND_ADIOS(&ctx, nullptr, VideoStreamNotFound);

  // * -------- Get decoder --------- *

  const AVCodec* decoder =
      avcodec_find_decoder(ctx->streams[foundVideo]->codecpar->codec_id);
  if (!decoder) CLEANUP_AND_ADIOS(&ctx, nullptr, CodecNotFound);

  AVCodecContext* decoderCtx = avcodec_alloc_context3(decoder);
  avcodec_parameters_to_context(decoderCtx, ctx->streams[foundVideo]->codecpar);
  ret = avcodec_open2(decoderCtx, decoder, nullptr);
  if (ret < 0) CLEANUP_AND_ADIOS(&ctx, nullptr, ret);

  // * -------- Get the image --------- *

  ret = av_read_frame(ctx, compressed.raw());
  if (ret < 0) CLEANUP_AND_ADIOS(&ctx, decoderCtx, ret);

  ret = avcodec_send_packet(decoderCtx, compressed.raw());
  if (ret < 0) CLEANUP_AND_ADIOS(&ctx, decoderCtx, SendPacketFailed);

  Frame result(decoderCtx->width, decoderCtx->height, decoderCtx->pix_fmt);
  ret = avcodec_receive_frame(decoderCtx, original.raw());
  if (ret < 0) CLEANUP_AND_ADIOS(&ctx, decoderCtx, ReceiveFrameFailed);

  // * -------- Cleanup -------- *

  avcodec_close(decoderCtx);
  avcodec_free_context(&decoderCtx);
  avformat_close_input(&ctx);

  return original;
}