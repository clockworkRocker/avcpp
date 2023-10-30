extern "C" {
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

#include <iostream>
#include <string>

#include "internal/handlers.hpp"
#include "internal/codec.hpp"
#include "internal/frame.hpp"
#include "internal/packet.hpp"

#include "internal/video_decoder.hpp"

namespace avcpp {

VideoDecoder::VideoDecoder(const std::string& name)
    : CodecBase(avcodec_find_decoder_by_name(name.c_str())) {
  if (!m_ctx->codec)
    std::cerr << __FUNCTION__ << ": No decoders found with given name!\n";
}

VideoDecoder::VideoDecoder(AVCodecID id) : CodecBase(avcodec_find_decoder(id)) {
  if (!m_ctx->codec)
    std::cerr << __FUNCTION__ << ": No encoders found with given ID!\n";
}

int VideoDecoder::decode(const Packet& packet) {
  /// Return code for functions
  int ret = 0;
  
  if (!avcodec_is_open(m_ctx))  //
    AV_CPP_ERROR(CodecNotOpen);

  if (packet.codecID() != m_ctx->codec_id)
    AV_CPP_ERROR(WrongDecoder);

  ret = avcodec_send_packet(m_ctx, packet.raw());
  AV_CPP_CHECK(ret);

  ret = avcodec_receive_frame(m_ctx, m_frame.raw());
  if (ret == AVERROR(EAGAIN)) return 1;

  AV_CPP_CHECK(ret);

  return 0;
}
}  // namespace avcpp