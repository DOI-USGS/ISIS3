# CMake module for find_package(TNT)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   TNT_INCLUDE_DIR

find_path(TNT_INCLUDE_DIR
    NAMES tnt.h
    PATH_SUFFIXES "tnt/tnt${TNT_FIND_VERSION}/tnt" "tnt/"
)

get_filename_component(TNT_ROOT_INCLUDE_DIR "${TNT_INCLUDE_DIR}" DIRECTORY)

message("-- TNT INCLUDE DIR: ${TNT_INCLUDE_DIR}" )
