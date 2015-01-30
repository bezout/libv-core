find_path(CIMG_INCLUDE_DIRS NAMES CImg.h)
mark_as_advanced(CIMG_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CImg DEFAULT_MSG CIMG_INCLUDE_DIRS)

if(CIMG_FOUND)
include_directories(SYSTEM ${CIMG_INCLUDE_DIRS})
endif()
