find_path(RAW1394_INCLUDE_DIRS NAMES libraw1394/raw1394.h)
mark_as_advanced(RAW1394_INCLUDE_DIRS)

find_library(RAW1394_LIBRARIES raw1394)
mark_as_advanced(RAW1394_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(raw1394 DEFAULT_MSG RAW1394_INCLUDE_DIRS RAW1394_LIBRARIES)

if(RAW1394_FOUND)
include_directories(SYSTEM ${RAW1394_INCLUDE_DIRS})
endif()
