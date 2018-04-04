# CMake module for find_package(GSL)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   GSL_INCLUDE_DIR
#   GSL_LIBLIST

find_path(NANOFLANN_INCLUDE_DIR
  NAMES nanoflann.hpp
  PATH_SUFFIXES nanoflann/
)

get_filename_component(NANOFLANN_ROOT_INCLUDE_DIR "${NANOFLANN_INCLUDE_DIR}" DIRECTORY)

message( "-- NANOFLANN INCLUDE DIR: ${NANOFLANN_INCLUDE_DIR}")
