# CMake module for find_package(CSM) aka Community Sensor Model
# Finds include directory and all applicable libraries
#
# Sets the following:
#   CSM_INCLUDE_DIR
#   CSM_LIBRARY

find_path(CSM_INCLUDE_DIR
  NAME csm.h
  PATH_SUFFIXES csm
)

find_library(CSM_LIBRARY
  NAMES csmapi
)

message(STATUS "CSM INCLUDE: " ${CSM_INCLUDE_DIR} )
message(STATUS "CSM LIB: "  ${CSM_LIBRARY} )
