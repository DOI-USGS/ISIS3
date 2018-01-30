# CMake module for find_package(TNT)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   TNT_INCLUDE_DIR

find_path(TNT_INCLUDE_DIR
    NAMES tnt.h
    PATH_SUFFIXES
    tnt/tnt126/tnt
    tnt/
)

get_filename_component(TNT_ROOT_INCLUDE_DIR "${TNT_INCLUDE_DIR}" DIRECTORY)
file(READ "${TNT_INCLUDE_DIR}/tnt_version.h" EXTRACT LIMIT 6 OFFSET 1006)
string(SUBSTRING ${EXTRACT} 0 5 VERSION_NUM)
message("TNT FOUND VERSION = ${VERSION_NUM}")
