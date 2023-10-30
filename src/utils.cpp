extern "C" {
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/avcodec.h>
}

#include <string>

#include "internal/handlers.hpp"
#include "internal/frame.hpp"
#include "internal/utils.hpp"

using namespace avcpp;

Frame imread(const char* path) {
  AVFormatContext* ctx = nullptr;
  int ret = 0;
  int foundVideo = -1;

  ret = avformat_open_input(&ctx, path, nullptr, nullptr);
  AV_CPP_CHECK(ret);

  // * -------- Find the video source in file -------- *

  for (int i = 0; i < ctx->nb_streams && foundVideo < 0; ++i)
    if (ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
      foundVideo = i;

  if (foundVideo < 0) {
    std::cout << __FUNCTION__ << ": Video stream not found\n";
    avformat_close_input(&ctx);
    return Frame();
  }

  // * -------- Get decoder ---------

  AVCodecContext* codecCtx;

}