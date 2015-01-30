find_path(FFMPEG_AVCODEC_INCLUDE_DIRS NAMES libavcodec/avcodec.h)
mark_as_advanced(FFMPEG_AVCODEC_INCLUDE_DIRS)

find_library(FFMPEG_AVCODEC_LIBRARIES avcodec)
mark_as_advanced(FFMPEG_AVCODEC_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ffmpeg_avcodec DEFAULT_MSG FFMPEG_AVCODEC_INCLUDE_DIRS FFMPEG_AVCODEC_LIBRARIES)

if(FFMPEG_AVCODEC_FOUND)
include_directories(SYSTEM ${FFMPEG_AVCODEC_INCLUDE_DIRS})
endif()