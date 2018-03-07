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

message( "-- CSPICE INCLUDE: " ${CSPICE_INCLUDE_DIR} )
message( "-- CSPICE LIB: "  ${CSPICE_LIBRARY} )
