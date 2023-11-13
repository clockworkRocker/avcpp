namespace frame_py{

py::buffer_info getBufferInfo(Frame &f, AVPixelFormat fmt = AV_PIX_FMT_NONE) {
  int dims = 0;
  int channels = 0;
  int itemsize = sizeof(uint8_t);
  std::string format = py::format_descriptor<uint8_t>::format();
  py::array::ShapeContainer shape;
  py::array::ShapeContainer strides;
  if (fmt < 0) fmt = static_cast<AVPixelFormat>(f.format());

  switch (static_cast<AVPixelFormat>(fmt)) {
    // * -------- 4-channel formats -------- *
    case AV_PIX_FMT_RGBAF32LE:  // * Adjust size for float32 format
    case AV_PIX_FMT_RGBAF32BE:
      itemsize = sizeof(_Float32);
      format = py::format_descriptor<_Float32>::format();
      dims = 3;
      channels = 4;
      break;

    case AV_PIX_FMT_RGBA64LE:
    case AV_PIX_FMT_RGBA64BE:
    case AV_PIX_FMT_BGRA64LE:
    case AV_PIX_FMT_BGRA64BE:
      itemsize = sizeof(int16_t);
      format = py::format_descriptor<int16_t>::format();
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
    case AV_PIX_FMT_GRAYF32LE:  // * Adjust size for float32 format
    case AV_PIX_FMT_GRAYF32BE:
      itemsize = sizeof(_Float32);
      format = py::format_descriptor<_Float32>::format();
      dims = 2;
      channels = 1;
      break;

    case AV_PIX_FMT_GRAY16LE:
    case AV_PIX_FMT_GRAY16BE:
      itemsize = sizeof(int16_t);
      format = py::format_descriptor<int16_t>::format();
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
      break;
    case 3:
      shape = {f.height(), f.width(), channels};
      strides = {f.width() * channels * itemsize, channels * itemsize,
                 itemsize};
      break;
  }

  return {
      static_cast<void *>(f.buffer()), itemsize, format, dims, shape, strides};
}

/**
 * @brief Convert a frame into a numpy.array
 */
py::array_t<uint8_t> toNumpy(Frame &f,
                                  AVPixelFormat format = AV_PIX_FMT_RGB24) {
  auto desc = getBufferInfo(f, format);
  int cStride = static_cast<int>(desc.strides[0]);
  int ret = 0;

  if (static_cast<AVPixelFormat>(f.format()) == format)
    return py::array_t<uint8_t>(desc);

  desc.ptr = nullptr;
  py::array_t<uint8_t> result(desc);
  uint8_t *ptr = result.mutable_data();

  SWScale::setDstProps(f.width(), f.height(), format);
  SWScale::setFlags(SWS_LANCZOS);
  SWScale::transform(f, &ptr, &cStride);

  return result;
}

/**
 * @brief Make a frame out of numpy.array
 */
Frame fromNumpy(py::array_t<uint8_t> &arr,
                   AVPixelFormat format = AV_PIX_FMT_RGB24, bool copy = true) {
  auto desc = arr.request();

  AVFrame *frame = av_frame_alloc();
  frame->height = arr.shape(0);
  frame->width = arr.shape(1);
  frame->format = format;
  frame->pict_type = AV_PICTURE_TYPE_I;

  if (copy || !arr.owndata()) {
    av_frame_get_buffer(frame, Frame::DefaultAlign);
    av_image_fill_arrays(frame->data, frame->linesize,
                         static_cast<uint8_t *>(desc.ptr), format, frame->width,
                         frame->height, Frame::DefaultAlign);
  } else {
    frame->buf[0] = av_buffer_create(
        static_cast<uint8_t *>(desc.ptr), arr.size(),
        [](void *opaque, uint8_t *data) {
          static_cast<py::array_t<uint8_t> *>(opaque)->dec_ref();
        },
        &arr, 0);
    arr.inc_ref();
    av_image_fill_linesizes(frame->linesize, format, frame->width);
    av_image_fill_pointers(frame->data, format, frame->height,
                           frame->buf[0]->data, frame->linesize);
  }

  return cpp__wrap(frame);
}
}