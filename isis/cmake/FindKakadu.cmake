# CMake module for find_package(Kakadu)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   KAKADU_INCLUDE_DIR
#   KAKADU_A_LIBRARY
#   KAKADU_V_LIBRARY

if(JP2KFLAG)
  find_path(KAKADU_INCLUDE_DIR
    NAME kdu_kernels.h
    PATH_SUFFIXES
    kakadu/v7_9_1-01762L/
  )

  find_library(KAKADU_A_LIBRARY
    NAMES kdu_a79R
  )

  find_library(KAKADU_V_LIBRARY
    NAMES kdu_v79R
  )

  get_filename_component(KAKADU_ROOT_INCLUDE_DIR "${KAKADU_INCLUDE_DIR}" DIRECTORY)

  message( "-- KAKADU INC DIR: ${KAKADU_INCLUDE_DIR}")
  message( "-- KAKADU A LIB: ${KAKADU_A_LIBRARY}")
  message( "-- KAKADU V LIB: ${KAKADU_V_LIBRARY}")
else()
  message("-- KAKADU DISABLED")
endif()
