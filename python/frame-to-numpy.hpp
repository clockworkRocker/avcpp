py::buffer_info getInfoForFrame(Frame& f, AVPixelFormat fmt = AV_PIX_FMT_NONE) {
  int dims = 0;
  int channels = 0;
  int itemsize = sizeof(uint8_t);
  std::string format = py::format_descriptor<uint8_t>::format();
  py::array::ShapeContainer shape;
  py::array::ShapeContainer strides;
  if (fmt < 0) fmt = static_cast<AVPixelFormat>(f.format());

  switch (static_cast<AVPixelFormat>(fmt)) {
    // * -------- 4-channel formats -------- *
    case AV_PIX_FMT_RGBAF32:  // * Adjust size for float32 format
      itemsize = sizeof(_Float32);
      format = py::format_descriptor<_Float32>::format();
    case AV_PIX_FMT_ABGR:
    case AV_PIX_FMT_ARGB:
    case AV_PIX_FMT_RGBA:
      dims = 3;
      channels = 4;
      break;

      // * -------- 3-channel formats -------- *

    case AV_PIX_FMT_RGBF32:  // * Adjust size for float32 format
      itemsize = sizeof(_Float32);
      py::format_descriptor<_Float32>::format();
    case AV_PIX_FMT_RGB24:
    case AV_PIX_FMT_BGR24:
      dims = 3;
      channels = 3;
      break;

    // * -------- Single-channel formats -------- *
    case AV_PIX_FMT_GRAYF32:  // * Adjust size for float32 format
      itemsize = sizeof(_Float32);
      format = py::format_descriptor<_Float32>::format();
    case AV_PIX_FMT_GRAY8:
      dims = 2;
      channels = 1;
      break;

    default:
      throw std::invalid_argument(
          "This pixel format is not supported. Use to_numpy() with one of the "
          "supported formats to perform conversion.");
  }

  switch (dims) {
    case 2:
      shape = {f.height(), f.width()};
      strides = {f.width() * channels * itemsize, channels * itemsize};
    case 3:
      shape = {f.height(), f.width(), channels};
      strides = {f.width() * channels * itemsize, channels * itemsize,
                 itemsize};
  }

  return {
      static_cast<void*>(f.buffer()), itemsize, format, dims, shape, strides};
}

/**
 * @brief Convert a frame into a numpy.array
 */
py::array_t<uint8_t> frameToNumpy(Frame& f,
                                  AVPixelFormat format = AV_PIX_FMT_RGB24) {
  auto desc = getInfoForFrame(f, format);
  int cStride = static_cast<int>(desc.strides[0]);
  int ret = 0;

  if (static_cast<AVPixelFormat>(f.format()) == format)
    return py::array_t<uint8_t>(desc);

  desc.ptr = nullptr;
  py::array_t<uint8_t> result(desc);
  uint8_t* ptr = result.mutable_data();

  SWScale::setDstProps(f.width(), f.height(), format);
  SWScale::setFlags(SWS_LANCZOS);
  SWScale::transform(f, &ptr, &cStride);

  return result;
}
