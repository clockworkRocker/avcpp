extern "C" {
#include <libswscale/swscale.h>
}

#include <memory>

#include "internal/frame.hpp"
#include "internal/handlers.hpp"
#include "internal/swscale.hpp"

using namespace avcpp;

SWScale::ContextUPtr SWScale::m_ctx =
    std::unique_ptr<SwsContext*, void (*)(SwsContext**)>(
        new SwsContext*(nullptr), &SWScale::deleteContext);
int SWScale::m_srcWidth = 0;
int SWScale::m_srcHeight = 0;
AVPixelFormat SWScale::m_srcPixFmt = AV_PIX_FMT_NONE;
int SWScale::m_dstWidth = 0;
int SWScale::m_dstHeight = 0;
AVPixelFormat SWScale::m_dstPixFmt = AV_PIX_FMT_NONE;
int SWScale::m_flags = 0;

void SWScale::setSrcProps(int width, int height, AVPixelFormat format) {
  m_srcHeight = height;
  m_srcWidth = width;
  m_srcPixFmt = format;
}

void SWScale::setDstProps(int width, int height, AVPixelFormat format) {
  m_dstHeight = height;
  m_dstWidth = width;
  m_dstPixFmt = format;
}

void SWScale::setFlags(int flags) { m_flags |= flags; }

void SWScale::clearFlags() { m_flags = 0; }

int SWScale::transform(const uint8_t* src[], const int* srcStrides,
                       uint8_t* dst[], const int dstStrides[]) {
  *m_ctx = sws_getCachedContext(*m_ctx, m_srcWidth, m_srcHeight, m_srcPixFmt,
                                m_dstWidth, m_dstHeight, m_dstPixFmt, m_flags,
                                nullptr, nullptr, nullptr);

  return sws_scale(*m_ctx, src, srcStrides, 0, m_srcHeight, dst, dstStrides);
}

int SWScale::transform(const Frame& frame, uint8_t* dst[],
                       const int dstStrides[]) {
  *m_ctx = sws_getCachedContext(*m_ctx, frame.width(), frame.height(),
                                static_cast<AVPixelFormat>(frame.format()),
                                m_dstWidth, m_dstHeight, m_dstPixFmt, m_flags,
                                nullptr, nullptr, nullptr);
  if (!m_ctx)
    AV_CPP_ERROR(SWScaleFailed);

  return sws_scale(*m_ctx, frame.raw()->data, frame.raw()->linesize, 0,
                   frame.height(), dst, dstStrides);
}

int SWScale::transform(const Frame& src, Frame& dst) {
  *m_ctx = sws_getCachedContext(*m_ctx, src.width(), src.height(),
                                static_cast<AVPixelFormat>(src.format()),
                                dst.width(), dst.height(),
                                static_cast<AVPixelFormat>(dst.format()),
                                m_flags, nullptr, nullptr, nullptr);
  if (!*m_ctx)
    AV_CPP_ERROR(SWScaleFailed);

  return sws_scale_frame(*m_ctx, dst.raw(), src.raw());
}

void SWScale::deleteContext(SwsContext** ptr) {
  sws_freeContext(*ptr);
  *ptr = nullptr;
  delete ptr;
}