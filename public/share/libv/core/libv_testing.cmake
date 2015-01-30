enable_testing()



#
# Compile tous les tests unitaires, exécute ceux qui ont bien été compilés, et vérifie leur code d'erreur.
#

file(GLOB_RECURSE _files tests/*.cpp)

#
# DEPRECATED SINCE 2014-03-20

file(GLOB_RECURSE _other_files unit/*.cpp)
if(_other_files)
  message(WARNING "You have files in ${PROJECT_SOURCE_DIR}/unit. Please move them in ${PROJECT_SOURCE_DIR}/tests.")
  set(_files ${_files} ${_other_files})
endif()

# END OF DEPRECATED SECTION
#

if(NOT TARGET tests)
  add_custom_target(tests)
endif()

if(_files)
  add_custom_target(${PROJECT_NAME}-tests)
  add_dependencies(tests ${PROJECT_NAME}-tests)
endif()

foreach(_file ${_files}) # _file = /home/me/libv-core/tests/some-dir/my-test.cpp
  get_filename_component(_name ${_file} NAME_WE) # _name = my-test
  get_filename_component(_path ${_file} PATH) # _path = /home/me/libv-core/tests/some-dir
  file(RELATIVE_PATH _name ${PROJECT_SOURCE_DIR} ${_path}/${_name}) # _name = tests/some-dir/my-test
  set(_name ${path_prefix}/${_name}) # _name = libv/core/tests/some-dir/my-test
  string(REPLACE "/" "-" _target ${_name}) # _target = libv-core-tests-some-dir-my-test
  add_executable(${_target} EXCLUDE_FROM_ALL ${_file})
  add_dependencies(${PROJECT_NAME}-tests ${_target})
  add_test(NAME ${_name}/build COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${_target})
  add_test(NAME ${_name} COMMAND $<TARGET_FILE:${_target}> ${_path})
  set_tests_properties(${_name} PROPERTIES DEPENDS ${_name}/build)
  target_link_libraries(${_target} ${PROJECT_NAME} ${${variable_prefix}_tests_libraries})
endforeach()



#
# Compile les programmes fournis comme exemples.
# On ne vérifie pas qu'ils fonctionnent, mais au moins qu'ils peuvent être compilés.
# La difficulté, c'est qu'il faut exporter une partie de nos variables pour que les exemples puissent configurer les bibliothèques dont ils dépendent.
# On les range dans un fichier cache.cmake.
#

file(GLOB_RECURSE _files ${PROJECT_SOURCE_DIR}/examples/*/CMakeLists.txt)

if(NOT TARGET examples)
  add_custom_target(examples)
endif()

if(_files)
  add_custom_target(${PROJECT_NAME}-examples)
  add_dependencies(examples ${PROJECT_NAME}-examples)
endif()

foreach(_file ${_files}) # _file = /home/me/libv-core/examples/my-example/CMakeLists.txt
  get_filename_component(_path ${_file} PATH) # _path = /home/me/libv-core/examples/my-example
  file(RELATIVE_PATH _name ${PROJECT_SOURCE_DIR} ${_path}) # _name = examples/my-example
  set(_name ${path_prefix}/${_name}) # _name = libv/core/examples/my-example
  string(REPLACE "/" "-" _target ${_name}) # _target = libv-core-examples-my-example
  set(_dir ${PROJECT_BINARY_DIR}/${_target}) # _dir = /tmp/build/libv-core-examples-my-example
  set(_cache ${_dir}/cache.cmake)
  file(WRITE ${_cache})

  # Export *_DIR variables.
  # They tell us where *Config.cmake files are.
  get_cmake_property(_vars VARIABLES)
  foreach(_var ${_vars})
    string(REGEX MATCH "_DIR$" _match ${_var})
    if("${_var}" AND _match)
      file(APPEND ${_cache} "set(${_var} \"${${_var}}\" CACHE INTERNAL \"\" FORCE)\n")
    endif()
  endforeach()

  # Export all variables from cache.
  get_cmake_property(_vars CACHE_VARIABLES)
  foreach(_var ${_vars})
    get_property(_type CACHE ${_var} PROPERTY TYPE)
    if("${_var}"
    AND NOT "${_var}" STREQUAL CMAKE_HOME_DIRECTORY # fixes « CMake Error: The source "..." does not match the source "..." used to generate cache.  Re-run cmake with a different source directory. »
    AND NOT "${_type}" STREQUAL STATIC
    )
      string(REGEX REPLACE "([\"\\])" "\\\\\\1" _escaped "${${_var}}")
      file(APPEND ${_cache} "set(${_var} \"${_escaped}\" CACHE ${_type} \"\" FORCE)\n")
    endif()
  endforeach()

  # Export some other variables.
  file(APPEND ${_cache} "set(CMAKE_MODULE_PATH \"${CMAKE_MODULE_PATH}\" CACHE INTERNAL \"\" FORCE)\n")

  add_custom_target(${_target} COMMAND ${CMAKE_COMMAND} -C${_cache} ${_path} COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${_dir})
  add_dependencies(${PROJECT_NAME}-examples ${_target})
  add_test(NAME ${_name} COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${_target})
endforeach()



#
# Compile chaque .hpp indépendamment de tout autre.
# Permet de vérifier que les .hpp peuvent être inclus dans n'importe quel ordre, et que toutes leurs dépendances sont bien satisfaites.
#

set(_cpp_files)

foreach(_file ${hpp_files}) # _file = /home/me/libv-core/src/libv/core/test.hpp
  file(RELATIVE_PATH _file ${PROJECT_SOURCE_DIR}/src ${_file}) # _file = libv/core/test.hpp
  set(_cpp_file ${PROJECT_BINARY_DIR}/${_file}.cpp) # _cpp_file = /tmp/build/libv/core/test.hpp.cpp
  file(WRITE ${_cpp_file} "#include <${_file}>")
  list(APPEND _cpp_files ${_cpp_file})
endforeach()

if(_cpp_files)
  set(_target ${PROJECT_NAME}-include)
  add_library(${_target} STATIC EXCLUDE_FROM_ALL ${_cpp_files})
  set_target_properties(${_target} PROPERTIES COMPILE_DEFINITIONS LIBV_IGNORE_OPTIONAL_DEPENDENCIES)
  add_test(NAME ${path_prefix}/include COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${_target})
  add_dependencies(tests ${_target})
endif()
