# Generate xxxConfig.cmake and Findxxx files to be used with find_package(xxx).

set(local_exports "")
set(_exports ${PROJECT_SOURCE_DIR}/public.cmake.in)
set(_config ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)

#
# DEPRECATED SINCE 2014-03-19

file(GLOB _file ${PROJECT_SOURCE_DIR}/src/CMakeLists.txt.in)
if(_file)
  message(WARNING "Please rename ${_file} as ${_exports}.")
  set(_exports ${_file})
endif()

# END OF DEPRECATED SECTION
#

if(EXISTS ${_exports})
  file(READ ${_exports} _exports)
  string(CONFIGURE "${_exports}" local_exports @ONLY)
endif()

if(cpp_files)
  set(has_library TRUE)
else()
  set(has_library FALSE)
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/libvConfig.cmake.in ${_config} @ONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/Findlibv.cmake.in ${CMAKE_BINARY_DIR}/Find${PROJECT_NAME}.cmake @ONLY)

install(FILES ${_config} DESTINATION share/${PROJECT_NAME})

find_package(${PROJECT_NAME} REQUIRED)
