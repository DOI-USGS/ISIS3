# CMake module for find_package(GeoTIFF)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   GEOTIFF_INCLUDE_DIR
#   GEOTIFF_LIBRARY

find_path(GEOTIFF_INCLUDE_DIR
  NAMES geotiff.h
  PATH_SUFFIXES geotiff
)

find_library(GEOTIFF_LIBRARY
  NAMES geotiff
)

get_filename_component(GEOTIFF_ROOT_INCLUDE_DIR "${GEOTIFF_INCLUDE_DIR}" DIRECTORY)

message( "-- GEOTIFF INCLUDE DIR: ${GEOTIFF_INCLUDE_DIR}")
message( "-- GEOTIFF LIB: ${GEOTIFF_LIBRARY}")
