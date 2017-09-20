# CMake module for find_package(PCL)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   PCL_INCLUDE_DIR
#   PCL_LIBRARY

find_path(PCL_ROOT_INCLUDE_DIR
  NAME pcl
  PATH_SUFFIXES pcl-1.8
)

find_path(PCL_INCLUDE_DIR
  NAME pcl_base.h
  PATH_SUFFIXES pcl-1.8/pcl
)

find_library(PCL_COMMON_LIBRARY NAMES pcl_common)
find_library(PCL_OCTREE_LIBRARY NAMES pcl_octree)
find_library(PCL_IO_LIBRARY     NAMES pcl_io)

get_filename_component(PCL_ROOT_INCLUDE_DIR "${PCL_INCLUDE_DIR}" DIRECTORY)
message("PCL FOUND VERSION = ${VERSION_NUM}")
