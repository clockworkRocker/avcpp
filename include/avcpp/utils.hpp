#ifndef AV_CPP__UTILS_H
#define AV_CPP__UTILS_H

namespace avcpp {

/// @brief Read an image from file, decode it and put the raw data into a frame
/// @return The filled frame on success, an empty one on failure
Frame imread(const char* path);

}

#endif // AV_CPP__UTILS_H