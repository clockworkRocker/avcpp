#ifndef AVCPP__SWSCALE_H
#define AVCPP__SWSCALE_H

namespace avcpp {
class SWScale {
 public:  // * Types
  typedef std::unique_ptr<SwsContext*, void (*)(SwsContext**)> ContextUPtr;

 public:  // * Main methods
  /// @brief Set the properties of source image for scaling
  static void setSrcProps(int width, int height, AVPixelFormat format);

  /// @brief Set the properties of resulting image for scaling
  static void setDstProps(int width, int height, AVPixelFormat format);

  /// @brief Set the sws_scale conversion flag
  static void setFlags(int flags);

  static void clearFlags();

  /// @brief Use current settings to transform an image
  /// @param src Source buffer
  /// @param srcStrides Layer (row) strides for source image
  /// @param dst Destination buffer
  /// @param dstStrides Layer (row) strides for resulting image
  /// @return 0 on success, a negative code on error
  static int transform(const uint8_t* src[], const int* srcStrides,
                       uint8_t* dst[], const int dstStrides[]);

  /// @brief Transform an image and store the result in dst
  /// @param[in] frame The original image. Its properties will be used as srcProps
  /// @param[out] dst Destination buffer
  /// @param[in] dstStrides Layer (row) strides for resulting image
  /// @return 0 on success, a negative code on error
  static int transform(const Frame& frame, uint8_t* dst[],
                       const int dstStrides[]);

  /// @brief Transform an image and write the result to another frame
  static int transform(const Frame& src, Frame& dst);

 private:
  static void deleteContext(SwsContext** ptr);

 private:
  /// Conversion context smart pointer
  static ContextUPtr m_ctx;
  /// Source image width
  static int m_srcWidth;
  /// Source image height
  static int m_srcHeight;
  /// Source image pixel format
  static AVPixelFormat m_srcPixFmt;
  /// Result image width
  static int m_dstWidth;
  /// Result image height
  static int m_dstHeight;
  /// Result image pixel format
  static AVPixelFormat m_dstPixFmt;
  /// Conversion flags
  static int m_flags;
};
}  // namespace avcpp

#endif