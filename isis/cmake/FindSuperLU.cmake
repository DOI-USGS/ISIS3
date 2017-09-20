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
  NAMES superlu_4.3
)

# message("SUPERLU_LIBRARY = ${SUPERLU_INCLUDE_DIR}")
get_filename_component(VERSION_FILE ${SUPERLU_LIBRARY} NAME)
# message("VERSION_FILE = ${VERSION_FILE}")
string(REGEX MATCH "[0-9]\\.[0-9]" VERSION_NUM ${VERSION_FILE})
message("SUPERLU FOUND VERSION = ${VERSION_NUM}")

get_filename_component(SUPERLU_ROOT_INCLUDE_DIR "${SUPERLU_INCLUDE_DIR}" DIRECTORY)
