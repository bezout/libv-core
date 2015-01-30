find_package(raw1394)

find_path(DC1394_INCLUDE_DIRS NAMES dc1394/dc1394.h)
mark_as_advanced(DC1394_INCLUDE_DIRS)

find_library(DC1394_LIBRARIES dc1394)
mark_as_advanced(DC1394_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(dc1394 DEFAULT_MSG DC1394_INCLUDE_DIRS DC1394_LIBRARIES RAW1394_FOUND)

if(DC1394_FOUND)
include_directories(SYSTEM ${DC1394_INCLUDE_DIRS})
endif()
