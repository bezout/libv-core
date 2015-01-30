# Comme find_package(), mais l'utilisateur peut définir l'option WITH_${package} (en majuscules) à OFF pour ne pas utiliser ${package}, même s'il est présent.
# Permet de limiter les dépendances d'un projet qui doit être exporté sur d'autres machines, ou de ne pas utiliser une dépendance qui nécessite un environnement particulier à l'exécution.
macro(libv_find_package)
  string(TOUPPER ${ARGV0} _package)
  option(WITH_${_package} "Try using ${ARGV0} for this project" ON)
  if(WITH_${_package})
    find_package(${ARGV})
    if(NOT ${_package}_FOUND)
      set(${_package}_LIBRARIES "")
    endif()
  endif()
endmacro()
