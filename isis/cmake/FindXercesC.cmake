find_path(XERCESC_INCLUDE_DIR
  NAMES xercesc/
  PATH_SUFFIXES xercesc/xercesc-3.1.2/
)
# message("XERCESC_INCLUDE_DIR = ${XERCESC_INCLUDE_DIR}")

find_library(XercesC_LIBRARY NAMES xerces-c)
