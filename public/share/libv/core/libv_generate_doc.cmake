## Generate the documentation.

find_package(Doxygen)

if(DOXYGEN_FOUND)

# doxygen will parse those directories
file(GLOB doc_dirs src tests examples)
string(REPLACE ";" " " doc_dirs "${doc_dirs}")

#
# DEPRECATED SINCE 2014-03-20

file(GLOB _dir doc)
if(_dir)
  message(WARNING "You have files in ${_dir}. Please move them in ${PROJECT_SOURCE_DIR}/src.")
  set(doc_dirs "${doc_dirs} ${_dir}")
endif()

file(GLOB _dir unit)
if(_dir)
  message(WARNING "You have files in ${_dir}. Please move them in ${PROJECT_SOURCE_DIR}/tests.")
  set(doc_dirs "${doc_dirs} ${_dir}")
endif()

# END OF DEPRECATED SECTION
#

# doxygen need to know the include directories to expand macros
get_property(_dirs DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(include_dirs "")
foreach(_dir ${_dirs})
  if(EXISTS ${_dir})
    set(include_dirs "${include_dirs} ${_dir}")
  endif()
endforeach()

file(GLOB_RECURSE bib_files src/*.bib)

# configure paths and other global options
set(local_exports "")
set(_exports doxyfile.ini.in)
if(EXISTS ${_exports})
  file(READ ${_exports} _exports)
  string(CONFIGURE "${_exports}" local_exports @ONLY)
endif()
configure_file(${CMAKE_CURRENT_LIST_DIR}/doxyfile.ini.in Doxyfile)

# actually build the doc
set(_file ${${macro_prefix}_TAG_FILE})
file(GLOB_RECURSE _files *.cpp *.hpp *.dox doxyfile.ini.in)
add_custom_command(OUTPUT ${_file} COMMAND ${DOXYGEN_BIN} DEPENDS ${_files} ${LIBV_TAG_FILES})
add_custom_target(${PROJECT_NAME}-doc ALL DEPENDS ${_file})
install(FILES ${_file} DESTINATION share/doc/${path_prefix})
install(DIRECTORY ${${macro_prefix}_DOC_DIR} DESTINATION share/doc/${path_prefix})

# add dependencies on other modules
if(LIBV_DOC_DEPENDS)
  add_dependencies(${PROJECT_NAME}-doc ${LIBV_DOC_DEPENDS})
endif()

endif()
