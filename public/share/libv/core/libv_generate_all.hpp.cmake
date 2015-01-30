## Generate an all.hpp file for each directory.

# first, we need a list of all directories and their contents
# we store all ancestor directories of each header in a 'dirs' variable and the contents of each directory in a '${PROJECT_NAME}_files_in_${dir}' variable
# basically this emulates an associative array
set(_dirs)
macro(get_parent_directories _file)
  get_filename_component(_path ${_file} PATH)
  list(APPEND _dirs ${path_prefix}/${_path})
  list(APPEND ${PROJECT_NAME}_files_in_${path_prefix}/${_path} ${path_prefix}/${_file})
  if(_path)
    get_parent_directories(${_path})
  endif()
endmacro()
foreach(_file ${hpp_files})
  file(RELATIVE_PATH _file ${PROJECT_SOURCE_DIR}/src/${path_prefix} ${_file})
  get_parent_directories(${_file})
endforeach()

# we now know all directories and their immediate children
# let us write an all.hpp file containing an #include statement for each file
if(_dirs)
  list(REMOVE_DUPLICATES _dirs)
endif()
foreach(_dir ${_dirs})
  # get this dir's immediate children
  set(_files ${${PROJECT_NAME}_files_in_${_dir}})
  list(REMOVE_DUPLICATES _files)
  # install the generated file
  set(_target ${PROJECT_BINARY_DIR}/${_dir}/all.hpp)
  install(FILES ${_target} DESTINATION include/${_dir})
  # create a temporary file
  set(_new_file ${PROJECT_BINARY_DIR}/tmp)
  file(WRITE ${_new_file})
  # append an #include statement for each file
  foreach(_file ${_files})
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/src/${_file})
      file(APPEND ${_new_file} "#include <${_file}/all.hpp>\n")
    else()
      file(APPEND ${_new_file} "#include <${_file}>\n")
    endif()
  endforeach()
  # replace the previous version with the new one if they are different
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_new_file} ${_target})
endforeach()
