# CMake module for find_package(SuperLU)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   SUPERLU_INCLUDE_DIR
#   SUPERLU_LIBRARY

find_path(SUPERLU_INCLUDE_DIR
  NAME supermatrix.h
  PATH_SUFFIXES superlu/superlu5.0/superlu/ superlu
)

find_library(SUPERLU_LIBRARY
  NAMES superlu_4.3 superlu 
)

get_filename_component(SUPERLU_ROOT_INCLUDE_DIR "${SUPERLU_INCLUDE_DIR}" DIRECTORY)
