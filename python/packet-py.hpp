namespace packet_py {

py::buffer_info getBufferInfo(Packet& p) {
  return py::buffer_info(p.buffer(), sizeof(uint8_t),
                         py::format_descriptor<uint8_t>::format(), 1,
                         {p.size()}, {sizeof(uint8_t)}, true);
}

Packet fromBytes(py::buffer& bytes, bool copy = true,
                 const py::kwargs& kwargs = {}) {
  AVPacket* packet = av_packet_alloc();

  auto desc = bytes.request();

  if (copy) {
    auto data = static_cast<uint8_t*>(av_malloc(desc.size * desc.itemsize));
    memcpy(data, desc.ptr, desc.size * desc.itemsize);
    av_packet_from_data(packet, data, desc.size * desc.itemsize);
  } else {
    auto data = static_cast<uint8_t*>(desc.ptr);
    bytes.inc_ref();

    std::cout << __FUNCTION__ << ": No-copy view is potentially unsafe.";
    /**
     * Here we create a ref-counted AVBuffer, pass the bytes memory as its data
     * and set the bytes dec_ref function as a deleter function as this memory
     * is already ref-counted from python
     */
    packet->buf = av_buffer_create(
        data, desc.size * desc.itemsize,
        [](void* opaque, uint8_t* data) {
          static_cast<py::buffer*>(opaque)->dec_ref();
        },
        &bytes, 0);
    packet->data = packet->buf->data;
  }

  if (kwargs.contains("pts")) packet->pts = kwargs["pts"].cast<int64_t>();
  if (kwargs.contains("dts")) packet->dts = kwargs["dts"].cast<int64_t>();
  if (kwargs.contains("pos")) packet->pos = kwargs["pos"].cast<int64_t>();
  if (kwargs.contains("duration"))
    packet->duration = kwargs["duration"].cast<int64_t>();
  if (kwargs.contains("flags")) packet->flags = kwargs["flags"].cast<int64_t>();

  return cpp__wrap(packet);
}

std::string repr(const Packet& p) {
  std::ostringstream s;
  s << "<Packet object at " << std::showbase << std::hex << &p << std::dec
    << "of size " << p.size() << '>';
  
  return s.str();
}
}  // namespace packet_py