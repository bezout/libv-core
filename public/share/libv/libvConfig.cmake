set(vars "")

foreach(component ${libv_FIND_COMPONENTS})
  set(_lower libv-${component})
  string(TOUPPER ${_lower} _upper)

  if(NOT ${_upper}_FOUND)
    find_package(${_lower} QUIET)

    if(libv_FIND_REQUIRED_${component})
      list(APPEND vars ${_upper}_FOUND)
    endif()
  endif()
endforeach()

if(vars)
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(libv DEFAULT_MSG libv_FIND_COMPONENTS ${vars})
endif()
