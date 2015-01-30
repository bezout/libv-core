set(_dir ${PROJECT_BINARY_DIR}/include-v4l/linux)
find_file(_file NAMES libv4l1-videodev.h)
file(MAKE_DIRECTORY ${_dir})
execute_process(COMMAND ln -s -f ${_file} ${_dir}/videodev.h)
find_path(V4L1_INCLUDE_DIRS NAMES linux/videodev.h PATHS ${_dir})
mark_as_advanced(V4L1_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(V4L1 DEFAULT_MSG V4L1_INCLUDE_DIRS)

if(V4L1_FOUND)
include_directories(SYSTEM ${V4L1_INCLUDE_DIRS})
endif()
