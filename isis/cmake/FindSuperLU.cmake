# CMake module for find_package(SuperLU)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   SUPERLU_INCLUDE_DIR
#   SUPERLU_LIBRARY

find_path(SUPERLU_INCLUDE_DIR
  NAME supermatrix.h
  PATH_SUFFIXES "superlu/superlu${SuperLU_FIND_VERSION}/superlu/" "superlu"
)

find_library(SUPERLU_LIBRARY
  NAMES "superlu_${SuperLU_FIND_VERSION}" "superlu"
)

get_filename_component(SUPERLU_ROOT_INCLUDE_DIR "${SUPERLU_INCLUDE_DIR}" DIRECTORY)


message( "-- SUPERLU INCLUDE DIR: ${SUPERLU_INCLUDE_DIR}")
message( "-- SUPERLU LIB: ${SUPERLU_LIBRARY}")
