extern "C" {
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
}

#include <iostream>
#include <string>

#include "internal/handlers.hpp"
#include "internal/codec.hpp"
#include "internal/frame.hpp"
#include "internal/packet.hpp"

#include "internal/video_encoder.hpp"

namespace avcpp {

VideoEncoder::VideoEncoder(const std::string& name)
    : CodecBase(avcodec_find_encoder_by_name(name.c_str())) {
  if (!m_ctx->codec)
    std::cerr << __FUNCTION__ << ": No encoders found with given name!\n";

  m_packet.setCodecID(m_ctx->codec_id);
}

VideoEncoder::VideoEncoder(AVCodecID id) : CodecBase(avcodec_find_encoder(id)) {
  if (!m_ctx->codec)
    std::cerr << __FUNCTION__ << ": No encoders found with given ID!\n";

  m_packet.setCodecID(m_ctx->codec_id);
}

int VideoEncoder::encode(const Frame& frame) {
  // * Check if the codec is open
  if (!avcodec_is_open(m_ctx))  //
    AV_CPP_ERROR(CodecNotOpen);

  // * Check encoding
  if (static_cast<AVPixelFormat>(frame.format()) != m_ctx->pix_fmt)
    AV_CPP_ERROR(InvalidPixelFormat);

  // * Check size
  if (frame.width() != m_ctx->width || frame.height() != m_ctx->height) {
    std::cerr << __FUNCTION__
              << ": Frame dimensions do not match encoder settings ("
              << frame.width() << 'x' << frame.height() << " vs "
              << m_ctx->width << 'x' << m_ctx->height << ")\n";
    return InvalidDimensions;
  }

  // * Clean up packet
  av_packet_unref(m_packet.raw());

  // * Try encoding
  int ret = avcodec_send_frame(m_ctx, frame.raw());
  AV_CPP_CHECK(ret);

  ret = avcodec_receive_packet(m_ctx, m_packet.raw());
  if (ret == AVERROR(EAGAIN)) return 1;

  AV_CPP_CHECK(ret);

  return 0;
}

}  // namespace avcpp