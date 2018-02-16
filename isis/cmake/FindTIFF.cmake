find_path(TIFF_INCLUDE_DIR
  NAMES tiff.h
  PATH_SUFFIXES "tiff/tiff-${TIFF_FIND_VERSION}"
)
# message("TIFF_INCLUDE_DIR = ${TIFF_INCLUDE_DIR}")

find_library(TIFF_LIBRARY NAMES tiff)
# message("TIFF_LIBRARY = ${TIFF_LIBRARY}")
