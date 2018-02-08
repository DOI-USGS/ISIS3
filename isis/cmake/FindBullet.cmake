# CMake module for find_package(Bullet)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   BULLET_INCLUDE_DIR
#   BULLET_LIBRARY

find_path(BULLET_INCLUDE_DIR
  NAME btBulletCollisionCommon.h
  PATH_SUFFIXES bullet
)

find_library(BULLET_OPENCL_LIBRARY NAMES Bullet3OpenCL_clew)
find_library(BULLET_SOFTBODY_LIBRARY NAMES BulletSoftBody)
find_library(BULLET_INVERSEDYNAMICS_LIBRARY NAMES BulletInverseDynamics)
find_library(BULLET_DYNAMICS_LIBRARY NAMES Bullet3Common)
find_library(BULLET_COLLISION_LIBRARY NAMES BulletDynamics)
find_library(BULLET3_GEOMETRY_LIBRARY NAMES BulletCollision)
find_library(BULLET3_DYNAMICS_LIBRARY NAMES Bullet3Geometry)
find_library(BULLET3_COLLISION_LIBRARY NAMES Bullet3Dynamics)
find_library(BULLET3_COMMON_LIBRARY NAMES Bullet3Collision)
find_library(BULLET3_LINEARMATH_LIBRARY NAMES LinearMath)

get_filename_component(BULLET_ROOT_INCLUDE_DIR "${BULLET_INCLUDE_DIR}" DIRECTORY)

message( status "BULLET OPENCL" ${BULLET_OPENCL_LIBRARY} )
message( status "BULLET SOFTBODY" ${BULLET_SOFTBODY_LIBRARY})
message( status "BULLET INVERSE DYNAMICS" ${BULLET_INVERSEDYNAMICS_LIBRARY} )
message( status ${BULLET_DYNAMICS_LIBRARY} )
message( status ${BULLET_COLLISION_LIBRARY} )
message( status ${BULLET3_GEOMETRY_LIBRARY} )
message( status ${BULLET3_DYNAMICS_LIBRARY} )
message( status ${BULLET3_COLLISION_LIBRARY} )
message( status ${BULLET3_COMMON_LIBRARY} )
message( status ${BULLET3_LINEARMATH_LIBRARY} )
