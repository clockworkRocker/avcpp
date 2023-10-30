#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
using namespace pybind11::literals;
namespace py = pybind11;

#include <avcpp/AV.hpp>
#include <avcpp/utils.hpp>
using namespace avcpp;

#include <sstream>

#include "ratios-py.hpp"
#include "pixelformats-py.hpp"
#include "codecs-py.hpp"
#include "frame-to-numpy.hpp"

PYBIND11_MODULE(libavpy, m) {
  m.doc() = "Python bindings for some of libav functions";

  add_ratios(m);
  add_pixelFormats(m);
  add_codec_ids(m);

  py::class_<Frame>(m, "Frame", py::buffer_protocol())
      .def(py::init<int, int, AVPixelFormat>(), "width"_a = 0, "height"_a = 0,
           "pixel_format"_a = AV_PIX_FMT_NONE)
      .def("datasize", &Frame::datasize,
           "Get buffer size in bytes required for this frame")
      .def("format", &Frame::format)
      .def("width", &Frame::width)
      .def("height", &Frame::height)
      .def("row_step", &Frame::rowStep)
      .def_property("pts", &Frame::pts, &Frame::setPts)
      .def("is_keyframe", &Frame::isKeyFrame)
      .def("to_format", &Frame::toFormat)
      .def("to_numpy", &frameToNumpy, "format"_a = AV_PIX_FMT_RGB24,
           "Convert the frame into RGB24 format and place the result into a "
           "numpy array")

      .def("__repr__",
           [](const Frame& frame) {
             std::ostringstream str;
             str << '<' << frame.width() << 'x' << frame.height()
                 << " AVFrame of format "
                 << std::to_string(static_cast<AVPixelFormat>(frame.format()))
                 << '>';
             return str.str();
           })

      .def_buffer([](Frame& frame) { return getInfoForFrame(frame); })

      .doc() = "Class that represents libav frames";

  py::class_<Packet>(m, "Packet")
      .def(py::init())
      .def_property(
          "pts", &Packet::pts,
          [](Packet& packet, int64_t pts) { packet.raw()->pts = pts; },
          "The presentation timestamp of the frame that was encoded into the "
          "packet")
      .def_property("codec_id", &Packet::codecID, &Packet::setCodecID)
      .def("has_keyframe", &Packet::hasKeyframe,
           "Returns true if the packet contains a keyframe")
      .def("size", &Packet::size, "Get the size of packet buffer in bytes")
      .def(
          "buffer",
          [](const Packet& p) {
            return py::bytes((const char*)p.buffer(), p.size());
          },
          "")

      .def("__repr__",
           [](const Packet& p) {
             std::ostringstream out;
             out << '<' << avcodec_get_name(p.codecID()) << " AVPacket of size "
                 << p.size() << '>';

             return out.str();
           })

      .doc() = "A Python wrapper around a libav AVPacket struct pointer";

  py::class_<CodecBase>(m, "Codec")
      .def_property("bitrate", &CodecBase::bitrate, &CodecBase::setBitrate)
      .def_property("fps", &CodecBase::framerate, &CodecBase::setFramerate)
      .def_property("time_base", &CodecBase::timeBase, &CodecBase::setTimeBase)
      .def_property("gop_size", &CodecBase::gopSize, &CodecBase::setGOPSize)
      .def_property("frame_width", &CodecBase::frameWidth,
                    &CodecBase::setFrameWidth)
      .def_property("frame_height", &CodecBase::frameHeight,
                    &CodecBase::setFrameHeight)
      .def_property("pixel_format", &CodecBase::pixelFormat,
                    &CodecBase::setFramePixelFormat)
      .def_property_readonly("id", &CodecBase::id)
      .def_property_readonly("name", &CodecBase::name)
      .def("set_option",
           static_cast<int (CodecBase::*)(
               const std::string&, const std::string&)>(&CodecBase::setOption),
           "name"_a, "value"_a, "Set an internal codec option")
      .def("set_option",
           static_cast<int (CodecBase::*)(const std::string&, int)>(
               &CodecBase::setOption),
           "name"_a, "value"_a, "Set an internal codec option")
      .def("set_option",
           static_cast<int (CodecBase::*)(const std::string&, double)>(
               &CodecBase::setOption),
           "name"_a, "value"_a, "Set an internal codec option")
      .def("open", &CodecBase::open,
           "Initialize the codec for encoding/decoding\n"
           "\n"
           "It is important to set all the necessary parameters using "
           "corresponding setters BEFORE calling this method because it is "
           "impossible to make changes to those parameters after "
           "initialization.\n"
           "\n"
           "Returns:\n"
           "--------\n"
           "int\n"
           "\t0 on success or a negative error code on fail")
      .def("reset", &CodecBase::reset,
           "Close and reset the codec\n"
           "\n"
           "Warning:\n"
           "--------\n"
           "Doing this will make the codec unusable until the next open() call")
      .doc() = "The base class for encoders and decoders";

  py::class_<VideoEncoder, CodecBase>(m, "VideoEncoder")
      .def(py::init<const std::string&>(), "name"_a)
      .def(py::init<AVCodecID>(), "id"_a)
      .def("encode", &VideoEncoder::encode,
           "Encode a frame and save the resulting packet into an internal "
           "buffer. To retrieve it, use getPacket().\n"
           "\n"
           "Returns:\n"
           "--------\n"
           "0 on success; a negative code on error, 1 on waiting for packet "
           "(if you get 1, send more frames before you can get output).")
      .def_property_readonly(
          "packet", &VideoEncoder::getPacket,
          "Retrieve the read-only packet data\n"
          "\n"
          "Warning:\n"
          "--------\n"
          "This should only be used after calling encode() at least once. "
          "Calling it beforehand will give a packet with no data and all "
          "default values.")
      .doc() = "Class used for video encoding";

  py::class_<VideoDecoder, CodecBase>(m, "VideoDecoder")
      .def(py::init<const std::string&>(), "name"_a)
      .def(py::init<AVCodecID>(), "id"_a)
      .def("decode", &VideoDecoder::decode, "packet"_a,
           "Decode the packet and save the resulting frame into an internal "
           "buffer. To retrieve it, use getFrame().\n"
           "\n"
           "Returns:\n"
           "--------\n"
           "0 on success, 1 if the frame result isn't yet available for "
           "retrieval, a negative code on error")
      .def_property_readonly(
          "frame", &VideoDecoder::getFrame,
          "Retrieve the read-only reference to last decoded frame\n"
          "\n"
          "Warning:\n"
          "--------\n"
          "This will only yield a desired result after at least one decode() "
          "call. Otherwise a frame with default values and empty buffer will "
          "be returned")
      .doc() = "Class for video decoding";

  m.def("imread", &imread, "path"_a, "Read an image from given path");
}