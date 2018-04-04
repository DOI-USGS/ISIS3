# CMake module for find_package(PCL)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   PCL_INCLUDE_DIR

#find_path(EIGEN_INCLUDE_DIR
#  NAME Core
#  PATH_SUFFIXES eigen/Eigen eigen3/Eigen
#)

find_path(EIGEN_ROOT_INCLUDE_DIR
  NAME Eigen
  PATH_SUFFIXES eigen eigen3
)

message( "-- EIGEN INCLUDE DIR: "  ${EIGEN_ROOT_INCLUDE_DIR} )
