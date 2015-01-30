# Compatibility with older CMake versions.
get_filename_component(CMAKE_CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

# Get the subdirectory and the variables/macros names for the current module.
# Example: if PROJECT_NAME = libv-core, then:
# → path_prefix = libv/core
# → variable_prefix = libv_core
# → macro_prefix = LIBV_CORE
string(REPLACE "-" "/" path_prefix ${PROJECT_NAME})
string(REPLACE "-" "_" variable_prefix ${PROJECT_NAME})
string(TOUPPER "${variable_prefix}" macro_prefix)

file(GLOB_RECURSE cpp_files src/*.cpp)
file(GLOB_RECURSE hpp_files src/*.hpp)

# Configure public files and development tools.
file(GLOB_RECURSE _files RELATIVE ${PROJECT_SOURCE_DIR} *.in/*)
foreach(_file ${_files})
  configure_file(${_file} ${_file} @ONLY)
endforeach()

# Install public files.
foreach(_dir ${PROJECT_SOURCE_DIR}/public ${PROJECT_BINARY_DIR}/public.in)
  if(EXISTS ${_dir})
    install(DIRECTORY ${_dir}/ DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)
  endif()
endforeach()

# Install development tools.
if(NOT TARGET tools)
  add_custom_target(tools)
endif()
if(EXISTS ${PROJECT_SOURCE_DIR}/tools OR EXISTS ${PROJECT_BINARY_DIR}/tools.in)
  add_custom_target(${PROJECT_NAME}-tools
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/tools ${CMAKE_INSTALL_PREFIX}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_BINARY_DIR}/tools.in ${CMAKE_INSTALL_PREFIX}
    )
  add_dependencies(tools ${PROJECT_NAME}-tools)
endif()
