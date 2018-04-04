find_path(TIFF_INCLUDE_DIR
  NAMES tiff.h
  PATH_SUFFIXES "tiff/tiff-${TIFF_FIND_VERSION}"
)

find_library(TIFF_LIBRARY NAMES tiff)

message( "-- TIFF INCLUDE DIR: ${TIFF_INCLUDE_DIR}")
message( "-- TIFF LIB: ${TIFF_LIBRARY}")
