#ifndef AV_CPP__MACROS_H
#define AV_CPP__MACROS_H

extern "C" {
#include <libavutil/error.h>
}

#include <iostream>

#define AV_CPP_ERRORS                                                          \
  AV_CPP_HANDLE(-20, CodecNotOpen, "Codec is not open for encoding/decoding.") \
  AV_CPP_HANDLE(-21, InvalidPixelFormat,                                       \
                "Codec cannot process the given pixel format")                 \
  AV_CPP_HANDLE(-22, InvalidDimensions,                                        \
                "Codec cannot process frame with given dimensions")            \
  AV_CPP_HANDLE(-23, WrongDecoder,                                             \
                "Packet was encoded with different encoder")                   \
  AV_CPP_HANDLE(-24, SendFrameFailed, "Error sending frame to encoder")        \
  AV_CPP_HANDLE(-25, SendPacketFailed, "Error sending packet to decoder")      \
  AV_CPP_HANDLE(-26, ReceiveFrameFailed, "Error receiving frame from decoder") \
  AV_CPP_HANDLE(-27, ReceivePacketFailed, "Error receiving packet from encoder")
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
#define AV_CPP_ERROR(code)                                              \
  do {                                                                  \
    std::cerr << __FUNCTION__ << ": " << getErrorMessage(code) << "\n"; \
    return code;                                                        \
  } while (0)

/// Check if the code is negative and return the error if that's the case
#define AV_CPP_CHECK(code) \
  if (code < 0) AV_CPP_ERROR(code);

#undef AV_CPP_ERRORS

#endif  // AV_CPP__MACROS_H