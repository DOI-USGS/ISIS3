# CMake module for find_package(Ale)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   ALE_INCLUDE_DIR
#   ALE_LIBRARY

find_path(ALE_INCLUDE_DIR
    NAMES Isd.h
    PATH_SUFFIXES ale
)

find_library(ALE_LIBRARY
  NAMES ale
)

message(STATUS "ALE INCLUDE DIR: "  ${ALE_INCLUDE_DIR} )
message(STATUS "ALE LIB: "  ${ALE_LIBRARY} )

get_filename_component(ALE_ROOT_INCLUDE_DIR "${ALE_INCLUDE_DIR}" DIRECTORY)

# Find ale version from conda and add to ale_version.txt
execute_process(
    COMMAND conda list ale --json
    OUTPUT_VARIABLE ALE_VERSION_JSON
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Extract version and build string
string(REGEX MATCH "\"version\": \"([^\"]+)\"" _ ${ALE_VERSION_JSON})
set(ALE_VERSION ${CMAKE_MATCH_1})

string(REGEX MATCH "\"build_string\": \"([^\"]+)\"" _ ${ALE_VERSION_JSON})
set(ALE_BUILD ${CMAKE_MATCH_1})

set(ALE_VERSION_FULL "${ALE_VERSION} | ${ALE_BUILD}")

# Write to a version file in the build directory
set(ALE_VERSION_FILE "${CMAKE_BINARY_DIR}/ale_version.txt")
if(ALE_VERSION_FULL)
    file(WRITE "${CMAKE_BINARY_DIR}/ale_version.txt" "${ALE_VERSION_FULL}\n")
else()
    file(WRITE "${CMAKE_BINARY_DIR}/ale_version.txt" "ALE version information unavailable\n")
endif()
