#ifndef AV_CPP__AV_H
#define AV_CPP__AV_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include <iostream>
#include <string>
#include <memory>

#include "internal/handlers.hpp"
#include "internal/frame.hpp"
#include "internal/packet.hpp"
#include "internal/swscale.hpp"
#include <internal/codec.hpp>
#include "internal/video_encoder.hpp"
#include "internal/video_decoder.hpp"

#endif  // AV_CPP__AV_H