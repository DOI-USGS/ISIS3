# CMake module for find_package(GMM)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   GMM_INCLUDE_DIR

find_path(GMM_INCLUDE_DIR
  NAMES gmm.h
  PATH_SUFFIXES /gmm/gmm-5.0/gmm/ gmm
)

get_filename_component(GMM_ROOT_INCLUDE_DIR "${GMM_INCLUDE_DIR}" DIRECTORY)

message("GMM FOUND VERSION = UNABLE TO VERSION")
