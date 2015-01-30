find_path(V4L2_INCLUDE_DIRS NAMES linux/videodev2.h)
mark_as_advanced(V4L2_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(V4L2 DEFAULT_MSG V4L2_INCLUDE_DIRS)

if(V4L2_FOUND)
include_directories(SYSTEM ${V4L2_INCLUDE_DIRS})
endif()
