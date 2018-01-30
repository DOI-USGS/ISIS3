find_path(XERCESC_INCLUDE_DIR
  NAMES xercesc/
  PATH_SUFFIXES xercesc/xercesc-3.1.2/
)
# message("XERCESC_INCLUDE_DIR = ${XERCESC_INCLUDE_DIR}")

find_library(XercesC_LIBRARY NAMES xerces-c)
# message("XercesC lib = ${XercesC_LIBRARY}")
get_version(${XercesC_LIBRARY} VERSION_NUM)
message("XercesC FOUND VERSION = ${VERSION_NUM}")
