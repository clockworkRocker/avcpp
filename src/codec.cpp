extern "C" {
#include "libavcodec/avcodec.h"
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#include "internal/handlers.hpp"
#include "internal/codec.hpp"

namespace avcpp {

CodecBase::~CodecBase() {
  if (avcodec_is_open(m_ctx)) avcodec_close(m_ctx);

  avcodec_free_context(&m_ctx);
}

int CodecBase::open() {
  if (/* m_codec && */ m_ctx->codec) {
    int ret = avcodec_open2(m_ctx, /* m_codec */ nullptr, nullptr);
    AV_CPP_CHECK(ret);

    return ret;
  }
  return -1;
}

void CodecBase::reset() {
  const AVCodec* old_codec = m_ctx->codec;

  avcodec_free_context(&m_ctx);
  m_ctx = avcodec_alloc_context3(old_codec);
}
}  // namespace avcpp