# CMake module for find_package(X11)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   X11_LIBRARY

find_library(X11_LIBRARY
  NAMES X11
)

message( "-- X11 LIB: "  ${X11_LIBRARY} ) 
