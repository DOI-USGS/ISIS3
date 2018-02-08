# CMake module for find_package(Embree)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   EMBREE_INCLUDE_DIR
#   EMBREE_LIBRARY

find_path(EMBREE_INCLUDE_DIR
  NAME rtcore.h
  PATH_SUFFIXES embree2
)

find_library(EMBREE_LIBRARY
  NAMES embree
)

get_filename_component(EMBREE_ROOT_INCLUDE_DIR "${EMBREE_INCLUDE_DIR}" DIRECTORY)

message("EMBREE INCLUDE: " ${EMBREE_INCLUDE_DIR})
message("EMBREE LIB: " ${EMBREE_LIBRARY})
