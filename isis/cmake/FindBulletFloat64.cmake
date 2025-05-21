# CMake module for find_package(BULLET) double precision package
# Finds include directory and all applicable libraries
#

find_package(
  Bullet
  REQUIRED
  CONFIGS
  BulletConfig-float64.cmake )

# Set up Bullet Float64 configuration
set( BULLETFLOAT64_ROOT_DIR  "${BULLET_ROOT_DIR}" )

# Finds the Bullet include directory.
string(FIND "${BULLET_INCLUDE_DIRS}" "${BULLETFLOAT64_ROOT_DIR}" has_bulletfloat64_dir )
if( has_bulletfloat64_dir EQUAL -1)
  cmake_path(APPEND BULLETFLOAT64_ROOT_DIR ${BULLET_INCLUDE_DIRS} 
             OUTPUT_VARIABLE BULLETFLOAT64_INCLUDE_DIR )
else()
  set( BULLETFLOAT64_INCLUDE_DIR ${BULLET_INCLUDE_DIRS} )
endif()
set( BULLETFLOAT64_INCLUDE_DIRS  ${BULLETFLOAT64_INCLUDE_DIR} )

# Find the Bullet library directories
string(FIND "${BULLET_LIBRARY_DIRS}" "${BULLETFLOAT64_ROOT_DIR}" has_bulletfloat64_lib )
if( has_bulletfloat64_lib EQUAL -1 )
  cmake_path(APPEND BULLETFLOAT64_ROOT_DIR ${BULLET_LIBRARY_DIRS}
             OUTPUT_VARIABLE BULLETFLOAT64_LIBRARY_DIRS )
else()
  set( BULLETFLOAT64_LIBRARY_DIRS ${BULLET_LIBRARY_DIRS} )
endif()
set( BULLETFLOAT64_LIBRARY_DIR  ${BULLETFLOAT64_LIBRARY_DIRS} )

# Add the Bullet definitions and float64
set( BULLETFLOAT64_DEFINITIONS "${BULLET_DEFINITIONS}" )
set( BULLETFLOAT64_LIBRARIES ${BULLET_LIBRARIES} )

if(NOT
   ${BULLETFLOAT64_DEFINITIONS}
   MATCHES
   ".*-DBT_USE_DOUBLE_PRECISION.*")
  message(
    ERROR "Bullet does not appear to be built with double precision, current definitions: ${BULLETFLOAT64_DEFINITIONS}")
endif()

add_definitions( ${BULLETFLOAT64_DEFINITIONS} )
include_directories( ${BULLETFLOAT64_INCLUDE_DIRS} )
link_directories( ${BULLETFLOAT64_LIBRARY_DIRS} )

message(STATUS "BULLETFLOAT64 DEFINITIONS:  "  "${BULLETFLOAT64_DEFINITIONS}" )
message(STATUS "BULLETFLOAT64 INCLUDE DIRS: "  "${BULLETFLOAT64_INCLUDE_DIRS}" )
message(STATUS "BULLETFLOAT64 LIBRARY DIRS: "  "${BULLETFLOAT64_LIBRARY_DIRS}" )
message(STATUS "BULLETFLOAT64 LIBRARIES:    "  "${BULLETFLOAT64_LIBRARIES}" )

# Uniquely add each library for linking
foreach (_bulletLib ${BULLETFLOAT64_LIBRARIES})
  find_library( "BULLETFLOAT64_${_bulletLib}_LIBRARY" NAMES ${_bulletLib} 
                PATHS ${BULLETFLOAT64_LIBRARY_DIRS} )
  message(STATUS "Bullet Library: " "${BULLETFLOAT64_${_bulletLib}_LIBRARY}")
endforeach()
