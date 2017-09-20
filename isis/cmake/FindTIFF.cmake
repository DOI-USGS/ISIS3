find_path(TIFF_INCLUDE_DIR
  NAMES tiff.h
  PATH_SUFFIXES tiff/tiff-4.0.5
)
# message("TIFF_INCLUDE_DIR = ${TIFF_INCLUDE_DIR}")

find_library(TIFF_LIBRARY NAMES tiff)
# message("TIFF_LIBRARY = ${TIFF_LIBRARY}")

get_version(${TIFF_LIBRARY} VERSION_NUM)
message("TIFF FOUND VERSION = ${VERSION_NUM}")
