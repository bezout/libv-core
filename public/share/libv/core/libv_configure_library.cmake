include(libv_preamble)
include(libv_generate_config.cmake)
include(libv_use_qt)

string(REGEX REPLACE "[^;]*/private/[^;]*;?" "" hpp_files "${hpp_files}") # exclude private headers
include_directories(BEFORE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)
foreach(_file ${hpp_files})
  get_filename_component(_dir ${_file} PATH)
  file(RELATIVE_PATH _dir ${PROJECT_SOURCE_DIR}/src ${_dir})
  install(FILES ${_file} DESTINATION include/${_dir})
endforeach()

include(libv_generate_all.hpp)
include(libv_generate_version.hpp)
include(libv_generate_doc)

if(cpp_files)
  add_library(${PROJECT_NAME} SHARED ${cpp_files})
  target_link_libraries(${PROJECT_NAME} ${LIBV_LIBRARIES})
  set(${macro_prefix}_PRIVATE_CXX_FLAGS "${libv_private_CXX_FLAGS_${CMAKE_CXX_COMPILER_ID}}" CACHE STRING "recommended compiler flags when building ${PROJECT_NAME}")
  set(CMAKE_CXX_FLAGS "${LIBV_CXX_FLAGS} ${${macro_prefix}_PRIVATE_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
  install(TARGETS ${PROJECT_NAME} DESTINATION lib)
endif()

include(libv_testing)
