# CMake module for find_package(CSPICE)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   CSPICE_INCLUDE_DIR
#   CSPICE_LIBRARY

find_path(CSPICE_INCLUDE_DIR
  NAME SpiceUsr.h
  PATH_SUFFIXES naif
)

find_library(CSPICE_LIBRARY
  NAMES cspice
)

string(REGEX MATCH "^(/([a-z or A-Z or 0-9])*)*" MY_PATH ${CSPICE_LIBRARY}})
file(GLOB FOUND_FILES
      "${MY_PATH}.*[0-9]**.a"
)
get_filename_component(VERSION_FILE "${FOUND_FILES}" NAME)

get_filename_component(CSPICE_ROOT_INCLUDE_DIR "${CSPICE_INCLUDE_DIR}" DIRECTORY)
