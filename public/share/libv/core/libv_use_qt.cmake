if(QT4_FOUND)

include(${QT_USE_FILE})

foreach(_file ${hpp_files})
  file(STRINGS ${_file} _lines REGEX Q_OBJECT)
  if(_lines)
    qt4_wrap_cpp(_file ${_file})
    list(APPEND cpp_files ${_file})
  endif()
endforeach()

file(GLOB_RECURSE _files ${PROJECT_SOURCE_DIR}/src/*.qrc)
qt4_add_resources(_files ${_files})
list(APPEND cpp_files ${_files})

file(GLOB_RECURSE _files ${PROJECT_SOURCE_DIR}/src/*.ui)
qt4_wrap_ui(_files ${_files})
list(APPEND cpp_files ${_files})

endif()
