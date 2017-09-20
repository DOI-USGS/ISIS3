# CMake module for find_package(Jama)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   JAMA_INCLUDE_DIR

find_path(JAMA_INCLUDE_DIR
    NAMES jama_cholesky.h
    PATH_SUFFIXES
    jama/jama125/jama
    /jama
)

get_filename_component(JAMA_ROOT_INCLUDE_DIR "${JAMA_INCLUDE_DIR}" DIRECTORY)

message("JAMA FOUND")
