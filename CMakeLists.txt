project(avcpp C CXX)
cmake_minimum_required(VERSION 3.8)

option(AVCPP_BUILD_PYTHON OFF)
option(AVCPP_BUILD_TESTS OFF)

# ---------------------------- FIND FFMPEG LIBRARIES -------------------------- #
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

# ---------------------------- MAIN PROJECT STUFF ---------------------------- #
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${LIBAV_INCLUDE_DIRS}
)

file(GLOB ${PROJECT_NAME}_SOURCES src/*.cpp)
file(GLOB ${PROJECT_NAME}_PUBLIC_HEADERS 
    "include/avcpp/*.hpp"
)
file(GLOB ${PROJECT_NAME}_PRIVATE_HEADERS
    "include/internal/*.hpp"
)

add_library(${PROJECT_NAME}_shared SHARED ${${PROJECT_NAME}_SOURCES})
set_target_properties(${PROJECT_NAME}_shared PROPERTIES 
    OUTPUT_NAME ${PROJECT_NAME}
    PRIVATE_HEADER "${${PROJECT_NAME}_PRIVATE_HEADERS}"
    PUBLIC_HEADER "${${PROJECT_NAME}_PUBLIC_HEADERS}"
)
target_link_libraries(${PROJECT_NAME}_shared ${LIBAV_LIBRARIES})

add_library(${PROJECT_NAME}_static ${${PROJECT_NAME}_SOURCES})
set_target_properties(${PROJECT_NAME}_static PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
)
target_link_libraries(${PROJECT_NAME}_static ${LIBAV_LIBRARIES})

# ---------------------------------- TESTS ----------------------------------- #
if (AVCPP_BUILD_TESTS)

file(GLOB_RECURSE TESTS ${PROJECT_SOURCE_DIR}/test/*.cpp)
foreach(_test ${TESTS})
    get_filename_component(testName ${_test} NAME_WE)
    add_executable(${testName} ${_test})
    target_link_libraries(${testName} ${PROJECT_NAME}_shared ${LIBAV_LIBRARIES})
endforeach(_test ${TESTS})


endif(AVCPP_BUILD_TESTS)

# ----------------------------- PYTHON BINDINGS ------------------------------ #
if (AVCPP_BUILD_PYTHON)

find_package(Python3 COMPONENTS Development Interpreter REQUIRED)
find_package(pybind11 REQUIRED)

pybind11_add_module(libavpy ${PROJECT_SOURCE_DIR}/python/libavpy.cpp)
target_link_libraries(libavpy PRIVATE ${PROJECT_NAME}_shared)

install(
    TARGETS libavpy
    DESTINATION "lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages/"
)
endif(AVCPP_BUILD_PYTHON)


# ------------------------------- INSTALLATION ------------------------------- #
install(
    TARGETS ${PROJECT_NAME}_static 
    DESTINATION lib/ 
    OPTIONAL
)
install(
    TARGETS ${PROJECT_NAME}_shared 
    EXPORT ${PROJECT_NAME}_exp
    LIBRARY DESTINATION lib/ 
    PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}/${PROJECT_NAME}
    PRIVATE_HEADER DESTINATION include/${PROJECT_NAME}/internal
    OPTIONAL
)

install(FILES
        ${PROJECT_SOURCE_DIR}/cmake/avcppConfig.cmake
        ${PROJECT_SOURCE_DIR}/cmake/FindAvcpp.cmake
	DESTINATION "share/cmake/${PROJECT_NAME}"
)
