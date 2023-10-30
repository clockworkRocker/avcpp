#include <avcpp/AV.hpp>

using namespace avcpp;

int main() {
  Frame f(1920, 1080, AV_PIX_FMT_RGB24);
  f.toFormat(AV_PIX_FMT_GRAY16);

  Frame f2(1920, 1080, AV_PIX_FMT_YUV420P);
  SWScale::setFlags(SWS_LANCZOS);
  SWScale::transform(f, f2);

  return 0;
}