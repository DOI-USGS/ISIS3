# CMake module for find_package(Geos)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   GEOS_INCLUDE_DIR
#   GEOS_LIBRARY

#changing to 3.5.1 for v007
find_path(GEOS_INCLUDE_DIR
  NAME geos
  PATH_SUFFIXES geos/geos3.5.1/
)
#tjw:  Changing to 3.5.1 for v007
find_library(GEOS_LIBRARY
  NAMES geos-3.5.1
)

find_library(GEOS_C_LIBRARY
  NAMES geos_c
)

get_filename_component(GEOS_ROOT_INCLUDE_DIR "${GEOS_INCLUDE_DIR}" DIRECTORY)
