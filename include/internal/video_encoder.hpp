#ifndef AV_CPP__VIDEO_ENCODER_H
#define AV_CPP__VIDEO_ENCODER_H

namespace avcpp {
/**
 * Video encoder
 */
class VideoEncoder : public CodecBase {
 public:  // * Constructors
  VideoEncoder(const std::string& name);
  VideoEncoder(AVCodecID id);

 public:

  /***
   * @brief Encode a frame and save the resulting packet into an internal
   *        buffer. To retrieve it, use getPacket().
   * @return 0 on success; a negative code on error, 1 on waiting for packet (if
   *         you get 1, send more frames before you can get output).
   */
  int encode(const Frame& frame);

  /**
   * @brief Retrieve the read-only packet data
   *
   * @warning This should only be used after calling encode() at least once.
   *          Calling it beforehand will give a packet with no data and all
   *          default values.
   */
  inline const Packet& getPacket() const { return m_packet; };

 private:
  Packet m_packet;
};

}  // namespace avcpp
#endif  // AV_CPP__VIDEO_ENCODER_H
