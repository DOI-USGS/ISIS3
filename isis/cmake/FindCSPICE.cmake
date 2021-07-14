# CMake module for find_package(CSPICE)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   CSPICE_INCLUDE_DIR
#   CSPICE_LIBRARY

# Search the override dirs first.
# The second set of 'finds' won't overwrite the variables if these overrides are found.
if(OVERRIDE_CSPICE_INCLUDE_DIR AND OVERRIDE_CSPICE_LIB_DIR)
    find_path(CSPICE_INCLUDE_DIR
      NAME SpiceUsr.h
      PATHS "${OVERRIDE_CSPICE_INCLUDE_DIR}"
      PATH_SUFFIXES naif cspice
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH
      NO_CMAKE_FIND_ROOT_PATH
    )

    find_library(CSPICE_LIBRARY
      NAMES cspice
      PATHS "${OVERRIDE_CSPICE_LIB_DIR}"
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH
      NO_CMAKE_FIND_ROOT_PATH
    )
endif()

find_path(CSPICE_INCLUDE_DIR
  NAME SpiceUsr.h
  PATH_SUFFIXES naif cspice
)

find_library(CSPICE_LIBRARY
  NAMES cspice
)

message(STATUS "CSPICE INCLUDE: " ${CSPICE_INCLUDE_DIR} )
message(STATUS "CSPICE LIB: "  ${CSPICE_LIBRARY} )
