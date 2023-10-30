find_package(avcpp ${avcpp_FIND_VERSION} QUIET NO_MODULE PATHS $ENV{HOME} /opt/avcpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(avcpp CONFIG_MODE REQUIRED_VARS avcpp_LIBRARIES)