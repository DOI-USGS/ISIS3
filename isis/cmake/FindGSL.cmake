# CMake module for find_package(GSL)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   GSL_INCLUDE_DIR
#   GSL_LIBLIST

find_path(GSL_INCLUDE_DIR
  NAMES gsl_math.h
  PATH_SUFFIXES gsl
)

find_library(GSL_LIBRARY
  NAMES gsl
)

find_library(GSL_CBLAS_LIBRARY
  NAMES gslcblas
)

get_filename_component(GSL_ROOT_INCLUDE_DIR "${GSL_INCLUDE_DIR}" DIRECTORY)

message( "-- GSL INCLUDE DIR: ${GSL_INCLUDE_DIR}")
message( "-- GSL LIB: ${GSL_LIBRARY}")
message( "-- GSL CBLAS LIB: ${GSL_CBLAS_LIBRARY}")
