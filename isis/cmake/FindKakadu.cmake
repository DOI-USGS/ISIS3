# CMake module for find_package(Kakadu)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   KAKADU_INCLUDE_DIR
#   KAKADU_LIBRARY

find_path(KAKADU_INCLUDE_DIR
  NAME kdu_kernels.h
  PATH_SUFFIXES
  kakadu/v7_9_1-01762L/
)

find_library(KAKADU_LIBRARY
  NAMES kdu_v79R
)

# message("KAKADU_LIBRARY = ${KAKADU_INCLUDE_DIR}")
get_filename_component(VERSION_FILE ${KAKADU_LIBRARY} NAME_WE)
# message("VERSION_FILE = ${VERSION_FILE}")
string(REGEX MATCH "[0-9][0-9]" VERSION_NUM ${VERSION_FILE})
message("KAKADU FOUND VERSION = ${VERSION_NUM}")

get_filename_component(KAKADU_ROOT_INCLUDE_DIR "${KAKADU_INCLUDE_DIR}" DIRECTORY)

if(VERSION_NUM VERSION_LESS Kakadu_FIND_VERSION)
  message(FATAL_ERROR "Kakadu version is too old (${VERSION_NUM}). Use ${Kakadu_FIND_VERSION} or higher.")
endif(VERSION_NUM VERSION_LESS Kakadu_FIND_VERSION)
