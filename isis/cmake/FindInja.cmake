# CMake module for find_package(Inja)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   INJA_INCLUDE_DIR
#   INJA_LIBRARY

find_path(INJA_INCLUDE_DIR
    NAME inja.hpp
    PATH_SUFFIXES "inja"
)

message(STATUS "INJA INCLUDE DIR: "  ${INJA_INCLUDE_DIR} )

