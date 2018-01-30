# CMake module for find_package(GSL)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   GSL_INCLUDE_DIR
#   GSL_LIBLIST

find_path(GSL_INCLUDE_DIR
  NAMES gsl_math.h
  PATH_SUFFIXES gsl
)

find_library(GSL_LIBRARY
  NAMES gsl
)

find_library(GSL_CBLAS_LIBRARY
  NAMES gslcblas
)

if(APPLE)
  string(REGEX MATCH "^(/([a-z or A-Z or 0-9])*)*" MY_PATH ${GSL_LIBRARY}})
  file(GLOB FOUND_FILES
        "${MY_PATH}.*[0-9]**.dylib"
  )
  get_filename_component(VERSION_FILE ${FOUND_FILES} NAME)
  string(REGEX MATCH "[0-9][0-9]" VERSION_NUM ${VERSION_FILE})
else(APPLE)
  get_version(${GSL_LIBRARY} VERSION_NUM)
endif(APPLE)
message("GSL FOUND VERSION = ${VERSION_NUM}")
set(GSL LIBLIST ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY})

get_filename_component(GSL_ROOT_INCLUDE_DIR "${GSL_INCLUDE_DIR}" DIRECTORY)
