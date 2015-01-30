## Generate a version.hpp file containing interesting CMake variables.

set(_file ${PROJECT_BINARY_DIR}/tmp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/version.hpp.in ${_file})
get_cmake_property(_vars VARIABLES)

foreach(_var ${_vars})
  string(REGEX MATCH "FOUND$" _match ${_var})
  if(_match AND ${_var})
    string(TOUPPER ${PROJECT_NAME}_${_var} _var)
    string(REGEX REPLACE "[^_A-Z0-9]" "_" _var "${_var}")
    file(APPEND ${_file} "#define ${_var}\n")
  endif()
endforeach()

set(_target ${PROJECT_BINARY_DIR}/${path_prefix}/version.hpp)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_file} ${_target})
install(FILES ${_target} DESTINATION include/${path_prefix})
