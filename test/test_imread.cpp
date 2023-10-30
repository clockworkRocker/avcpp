#include <avcpp/AV.hpp>
#include <avcpp/utils.hpp>

using namespace avcpp;

int main() {
  Frame f(std::move(avcpp::imread("/ros_ws/hallo.jpg")));

  std::cout << "Read image of size " << f.width() << 'x' << f.height() << '\n';

  return 0;
}