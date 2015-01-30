include(libv_preamble)

#
# DEPRECATED SINCE 2014-03-18

file(GLOB _files *.cpp)
if(_files)
  message(WARNING "You have source files in ${PROJECT_SOURCE_DIR}. Please move them in ${PROJECT_SOURCE_DIR}/src.")
  list(APPEND cpp_files ${_files})
endif()

# END OF DEPRECATED SECTION
#

include(libv_use_qt)

add_executable(${PROJECT_NAME} ${cpp_files})
target_link_libraries(${PROJECT_NAME} ${LIBV_LIBRARIES})
set(${macro_prefix}_PRIVATE_CXX_FLAGS "${libv_private_CXX_FLAGS_${CMAKE_CXX_COMPILER_ID}}" CACHE STRING "recommended compiler flags when building ${PROJECT_NAME}")
set(CMAKE_CXX_FLAGS "${LIBV_CXX_FLAGS} ${${macro_prefix}_PRIVATE_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
