find_path(PROTOBUF_INCLUDE_DIR
  NAMES google/
  PATH_SUFFIXES google-protobuf/protobuf2.6.1/
)

find_library(PROTOBUF_LIBRARY NAMES protobuf)

get_filename_component(PROTOBUF_ROOT_INCLUDE_DIR "${PROTOBUF_INCLUDE_DIR}" DIRECTORY)
get_version(${PROTOBUF_LIBRARY} VERSION_NUM)
message("PROTOBUF FOUND VERSION = ${VERSION_NUM}")
