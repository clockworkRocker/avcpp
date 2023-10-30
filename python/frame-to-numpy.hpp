void unref(AVBufferRef** ptr) {
  av_buffer_unref(ptr);
  delete ptr;
}

/// @brief Create a new ref to AVBuffer and wrap it with a unique_ptr for
/// automatic unreffing
std::unique_ptr<AVBufferRef*, void (*)(AVBufferRef**)> make_ref(
    AVBufferRef* src) {
  return std::unique_ptr<AVBufferRef*, void (*)(AVBufferRef**)>(
      new AVBufferRef*(src), &unref);
};

/**
 * @brief Convert a frame into a numpy.array
 *
 * @bug Using plain memcpy from buffer to buffer can cause issues
 */
py::array_t<uint8_t> frameToNumpy(const Frame& f,
                                  AVPixelFormat format = AV_PIX_FMT_RGB24) {
  int dims = 0;
  int channels = 0;
  py::array::ShapeContainer shape;
  py::array::ShapeContainer strides;
  int cStride = 0;
  py::buffer_info desc;
  uint8_t* buf = nullptr;

  switch (format) {
    case AV_PIX_FMT_RGBA:
      channels = 4;
      dims = 3;
      break;
    case AV_PIX_FMT_RGB24:
      channels = 3;
      dims = 3;
      break;
    case AV_PIX_FMT_GRAY8:
      channels = 1;
      dims = 2;
      break;
    default:
      throw(std::runtime_error(
          "frameToNumpy: Selected pixel format is not supported."));
  }

  switch (dims) {
    case 3:
      shape = {f.height(), f.width(), channels};
      strides = {channels * f.width(), channels, 1};
      break;
    case 2:
      shape = {f.width(), f.height()};
      strides = {channels * f.width(), channels};
      break;
  }
  buf = new uint8_t[f.width() * f.height() * channels];
  desc = {buf, sizeof(uint8_t), py::format_descriptor<uint8_t>::format(),
          dims,    shape,           strides};
  cStride = static_cast<int>(strides->at(0));

  if (static_cast<AVPixelFormat>(f.format()) == format) {
  // * Sorry, lads, couldn't avoid copying
    memcpy(buf, f.buffer(), f.width() * f.height() * channels);
  } else {
    SWScale::setDstProps(f.width(), f.height(), format);
    SWScale::setFlags(SWS_LANCZOS);
    SWScale::transform(f, &buf, &cStride);
  }

  return py::array_t<uint8_t>(desc);
}
