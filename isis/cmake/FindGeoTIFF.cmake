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

# DO SOMETHING DIFFERENT FOR APPLE
if(APPLE)
  string(REGEX MATCH "^(/([a-z or A-Z or 0-9])*)*" MY_PATH ${GEOTIFF_LIBRARY}})
  file(GLOB FOUND_FILES
        "${MY_PATH}.*[0-9]**.dylib"
  )
  get_filename_component(VERSION_FILE ${FOUND_FILES} NAME)
  string(REGEX MATCH "[0-9]" VERSION_NUM ${VERSION_FILE})
else(APPLE)
  get_version(${GEOTIFF_LIBRARY} VERSION_NUM)
endif(APPLE)
message("GEOTIFF FOUND VERSION = ${VERSION_NUM}")

get_filename_component(GEOTIFF_ROOT_INCLUDE_DIR "${GEOTIFF_INCLUDE_DIR}" DIRECTORY)

if(VERSION_NUM VERSION_LESS GeoTIFF_FIND_VERSION)
  message(FATAL_ERROR "GeoTIFF version is too old (${VERSION_NUM}). Use ${GeoTIFF_FIND_VERSION} or higher.")
endif(VERSION_NUM VERSION_LESS GeoTIFF_FIND_VERSION)
