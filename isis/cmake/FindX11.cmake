# CMake module for find_package(X11)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   X11_LIBRARY

find_library(X11_LIBRARY
  NAMES X11
)

# message("X11_LIBRARY = ${X11_LIBRARY}")
set(VERSION_NUM "")
get_version(${X11_LIBRARY} VERSION_NUM)
message("X11 FOUND VERSION = ${VERSION_NUM}")
