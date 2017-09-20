# CMake module for find_package(Qwt)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   QWT_INCLUDE_DIR
#   QWT_LIBRARY

FIND_PATH(QWT_INCLUDE_DIR
  NAMES qwt.h
  PATH_SUFFIXES qwt-qt5 qwt qwt6
)

find_library(QWT_LIBRARY
  NAMES qwt
)

if(APPLE)
  set(VERSION_PATH "${QWT_LIBRARY}/Versions/")
  file(GLOB FOUND_FILES
        "${VERSION_PATH}[0-9]"
  )
  string(SUBSTRING ${FOUND_FILES} 63 64 VERSION_NUM)
else(APPLE)
  get_version(${QWT_LIBRARY} VERSION_NUM)
endif(APPLE)
message("QWT FOUND VERSION = ${VERSION_NUM}")

get_filename_component(QWT_ROOT_INCLUDE_DIR "${QWT_INCLUDE_DIR}" DIRECTORY)
