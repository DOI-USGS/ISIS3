# CMake module for find_package(TNT)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   TNT_INCLUDE_DIR

find_path(NN_INCLUDE_DIR
    NAMES nn.h
    PATH_SUFFIXES nn
)

get_filename_component(NN_ROOT_INCLUDE_DIR "${NN_INCLUDE_DIR}" DIRECTORY)

find_library(NN_LIBRARY  NAMES nn)
message("NN FOUND VERSION = UNABLE TO VERSION")
