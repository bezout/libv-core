find_path(TINYXML_INCLUDE_DIRS NAMES tinyxml.h)
mark_as_advanced(TINYXML_INCLUDE_DIRS)

find_library(TINYXML_LIBRARIES tinyxml)
mark_as_advanced(TINYXML_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(tinyxml DEFAULT_MSG TINYXML_INCLUDE_DIRS TINYXML_LIBRARIES)

if(TINYXML_FOUND)
include_directories(SYSTEM ${TINYXML_INCLUDE_DIRS})
endif()
