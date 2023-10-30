#ifndef AV_CPP__MACROS_H
#define AV_CPP__MACROS_H

extern "C" {
#include <libavutil/error.h>
}

#include <iostream>

#define AV_CPP_ERRORS                                                      \
  AV_CPP_HANDLE(-101, CodecNotOpen,                                        \
                "Codec is not open for encoding/decoding.")                \
  AV_CPP_HANDLE(-111, CodecNotFound, "No codec found with given id/name")  \
  AV_CPP_HANDLE(-102, InvalidPixelFormat,                                  \
                "Codec cannot process the given pixel format")             \
  AV_CPP_HANDLE(-103, InvalidDimensions,                                   \
                "Codec cannot process frame with given dimensions")        \
  AV_CPP_HANDLE(-104, WrongDecoder,                                        \
                "Packet was encoded with different encoder")               \
  AV_CPP_HANDLE(-105, SendFrameFailed, "Error sending frame to encoder")   \
  AV_CPP_HANDLE(-106, SendPacketFailed, "Error sending packet to decoder") \
  AV_CPP_HANDLE(-107, ReceiveFrameFailed,                                  \
                "Error receiving frame from decoder")                      \
  AV_CPP_HANDLE(-108, ReceivePacketFailed,                                 \
                "Error receiving packet from encoder")                     \
  AV_CPP_HANDLE(-109, VideoStreamNotFound, "File has no video streams")    \
  AV_CPP_HANDLE(-110, AudioStreamNotFound, "File has no audio streams")    \
  AV_CPP_HANDLE(-112, SWScaleFailed, "Error allocating context for sws_scale")

/* ========================================================================== */

#define AV_CPP_HANDLE(num, name, message) name = num,

namespace avcpp {

/// @brief Codec errors enum
enum AVCppError { AV_CPP_ERRORS };

}  // namespace avcpp

#undef AV_CPP_HANDLE

/* ======================= ERROR MESSAGE FUNCTION =========================== */

#define AV_CPP_HANDLE(num, name, message) \
  case name:                              \
    return message;

namespace avcpp {

enum { MaxErrorBufSize = 64 };
/// @brief A string buffer for error messages
static char ErrorBuf[MaxErrorBufSize];

/// Get a corresponding error message from error code
inline const char* getErrorMessage(int code) {
  switch (code) {
    AV_CPP_ERRORS

    default:
      return av_make_error_string(ErrorBuf, MaxErrorBufSize, code);
  }
}
}  // namespace avcpp

#undef AV_CPP_HANDLE

/* ========================================================================== */
/// Compact error report macro
#define AV_CPP_RETURN_ON_ERROR(value, code)                              \
  do {                                                                  \
    std::cerr << __FUNCTION__ << ": " << getErrorMessage(code) << "\n"; \
    return (value);                                                     \
  } while (0)

/// Another error report macro
#define AV_CPP_ERROR(code) AV_CPP_RETURN_ON_ERROR(code, code)

/// Check if the code is negative and return the error if that's the case
#define AV_CPP_CHECK(code) \
  if (code < 0) AV_CPP_ERROR(code)

#undef AV_CPP_ERRORS

#endif  // AV_CPP__MACROS_H