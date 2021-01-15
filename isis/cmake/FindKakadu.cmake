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
    PATHS /usgs/apps/kakadu/v7_9_1-01762L/managed/all_includes/ /isisData/kakadu/

  )

  find_library(KAKADU_A_LIBRARY
    NAMES kdu_a79R
  )

  find_library(KAKADU_V_LIBRARY
    NAMES kdu_v79R
  )

  get_filename_component(KAKADU_ROOT_INCLUDE_DIR "${KAKADU_INCLUDE_DIR}" DIRECTORY)

  message(STATUS "KAKADU INC DIR: ${KAKADU_INCLUDE_DIR}")
  message(STATUS "KAKADU A LIB: ${KAKADU_A_LIBRARY}")
  message(STATUS "KAKADU V LIB: ${KAKADU_V_LIBRARY}")
else()
  message(STATUS "KAKADU DISABLED")
endif()
