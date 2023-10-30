set(avcpp_FOUND TRUE)

set(avcpp_VERSION 0.0.1)
set(avcpp_VERSION_MAJOR 0)
set(avcpp_VERSION_MINOR 0)
set(avcpp_VERSION_PATCH 1)


get_filename_component(_cmake ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
get_filename_component(_share ${_cmake} DIRECTORY)
get_filename_component(avcpp_ROOT ${_share} DIRECTORY)

set(avcpp_INCLUDE_DIRS "${avcpp_ROOT}/include/avcpp")
set(avcpp_LIB_DIR "${avcpp_ROOT}/lib")
set(avcpp_CMAKE_MODULES_DIR "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${avcpp_CMAKE_MODULES_DIR}")

message("?? Looking for libs: ${avcpp_LIB_DIR}")
find_library(avcpp_LIBRARY avcpp 
    PATHS ${avcpp_LIB_DIR}
    NO_DEFAULT_PATH
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavdevice
    libavfilter
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
)

set(avcpp_LIBRARIES ${avcpp_LIBRARY} ${LIBAV_LIBRARIES})
message("-- Found avcpp: ${avcpp_VERSION} (${avcpp_CMAKE_MODULES_DIR})")