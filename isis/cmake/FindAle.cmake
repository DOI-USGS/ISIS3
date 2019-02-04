# CMake module for find_package(ale)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   ALE_INCLUDE_DIR
#   ALE_LIBRARY

find_path(ALE_INCLUDE_DIR
  NAMES ale.h
  PATH_SUFFIXES ale
)

find_library(ALE_LIBRARY
  NAMES ale
)

get_filename_component(ALE_ROOT_INCLUDE_DIR "${ALE_INCLUDE_DIR}" DIRECTORY)

message(STATUS "ALE INCLUDE DIR: ${ALE_INCLUDE_DIR}")
message(STATUS "ALE LIB: ${ALE_LIBRARY}")
