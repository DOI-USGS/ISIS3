find_path(PROTOBUF_INCLUDE_DIR
  NAMES google/
  PATH_SUFFIXES "google-protobuf/protobuf${Protobuf_FIND_VERSION}/"
)

find_library(PROTOBUF_LIBRARY NAMES protobuf)

get_filename_component(PROTOBUF_ROOT_INCLUDE_DIR "${PROTOBUF_INCLUDE_DIR}" DIRECTORY)

message( "-- PROTOBUF INCLUDE DIR: ${PROTOBUF_INCLUDE_DIR}")
message( "-- PROTOBUF LIB: ${PROTOBUF_LIBRARY}")
