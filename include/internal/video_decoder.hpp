#ifndef AV_CPP__VIDEO_DECODER_H
#define AV_CPP__VIDEO_DECODER_H

namespace avcpp {
/**
 * Video decoder class
 */
class VideoDecoder : public CodecBase {
 public:  // * Constructors
  VideoDecoder(const std::string& name);
  VideoDecoder(AVCodecID id);

 public:
  /**
   * @brief Decode the packet and save the resulting frame into an internal
   *        buffer. To retrieve it, use getFrame().
   *
   * @return 0 on success, 1 if the frame result isn't yet available for
   *         retrieval, a negative code on error
   */
  int decode(const Packet& packet);

  /**
   * @brief Retrieve the read-only reference to last decoded frame
   *
   * @warning This will only yield a desired result after at least one decode()
   *          call. Otherwise a frame with default values and empty buffer will
   *          be returned
   */
  inline const Frame& getFrame() const { return m_frame; }

  /**
   * @return The number in sequence of last successfully decoded frame
  */
  inline int getFrameID() const { return m_framecount - 1; }

 private:
  Frame m_frame;
  int m_framecount = 0;
};

}  // namespace avcpp

#endif  // AV_CPP__VIDEO_DECODER_H