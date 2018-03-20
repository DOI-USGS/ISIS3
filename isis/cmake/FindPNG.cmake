# Finds include directory and all applicable libraries
#
# Sets the following:
#   PNG_INCLUDE_DIR
#   PNG_LIBRARY

find_path(PNG_INCLUDE_DIR
    NAMES png.h
    PATH_SUFFIXES png
)

find_library(PNG_LIBRARY
  NAMES png
)

message( "-- PNG INCLUDE DIR: ${PNG_INCLUDE_DIR}")
message( "-- PNG LIB: ${PNG_LIBRARY}")
