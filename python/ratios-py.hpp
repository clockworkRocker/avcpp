#include <pybind11/pybind11.h>
using namespace pybind11::literals;
namespace py = pybind11;

#include <string>

void add_ratios(py::module& m) {
  py::class_<AVRational>(m, "Ratio")
      .def(py::init<int, int>(), "num"_a, "den"_a)
      .def(py::init([](double value) {
             return std::make_unique<AVRational>(av_d2q(value, INT32_MAX));
           }),
           "value"_a)

      .def(py::init([](int value) {
        return std::make_unique<AVRational>(AVRational{value, 1});
      }))

      .def_readwrite("num", &AVRational::num)
      .def_readwrite("den", &AVRational::den)
      .def("__float__", &av_q2d)
      .def("__repr__",
           [](const AVRational& ratio) {
             return std::to_string(ratio.num) + "/" + std::to_string(ratio.den);
           })
      .def("__add__", &av_add_q, "other"_a)
      .def("__sub__", &av_sub_q, "other"_a)
      .def("__mul__", &av_mul_q, "other"_a)
      .def("__truediv__", &av_div_q, "other"_a)
      .def("reduce",
           [](AVRational& ratio) {
             av_reduce(&ratio.num, &ratio.den, ratio.num, ratio.den, INT32_MAX);
             return ratio;
           })
      .doc() = "Rational number (pair of numerator and denominator)";
}