find_path(XERCESC_INCLUDE_DIR
  NAMES xercesc/
  PATH_SUFFIXES "xercesc/xercesc-${XercesC_FIND_VERSION}/"
)
# message("XERCESC_INCLUDE_DIR = ${XERCESC_INCLUDE_DIR}")

find_library(XercesC_LIBRARY NAMES xerces-c)

message( "-- XERCES LIB: "  ${XercesC_LIBRARY} )
message( "-- XERCES INCLUDE DIR: "  ${XERCESC_INCLUDE_DIR} )
