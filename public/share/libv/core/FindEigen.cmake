find_path(EIGEN_INCLUDE_DIRS NAMES signature_of_eigen3_matrix_library PATH_SUFFIXES eigen3)
mark_as_advanced(EIGEN_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Eigen DEFAULT_MSG EIGEN_INCLUDE_DIRS)

if(EIGEN_FOUND)
include_directories(SYSTEM ${EIGEN_INCLUDE_DIRS})
endif()
