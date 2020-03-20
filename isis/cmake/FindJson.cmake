# CMake module for find_package(Json)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   JSON_INCLUDE_DIR
#   JSON_LIBRARY


find_path(JSON_INCLUDE_DIR
    NAME json.hpp
    PATH_SUFFIXES "nlohmann"
)

message(STATUS "JSON INCLUDE DIR: "  ${JSON_INCLUDE_DIR} )
