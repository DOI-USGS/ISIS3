# CMake module for find_package(Cholmod)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   CHOLMOD_INCLUDE_DIR
#   CHOLMOD_LIBLIST

find_path(CHOLMOD_INCLUDE_DIR
  NAME cholmod.h
  PATH_SUFFIXES "SuiteSparse/SuiteSparse${Cholmod_FIND_VERSION}/SuiteSparse/" "SuiteSparse"
)

find_library(CHOLMOD_LIBRARY      NAMES cholmod)
find_library(CCOLAMD_LIBRARY      NAMES ccolamd)
find_library(COLAMD_LIBRARY       NAMES colamd)
find_library(CAMD_LIBRARY         NAMES camd)
find_library(AMD_LIBRARY          NAMES amd)
find_library(SUITESPARSE_LIBRARY  NAMES suitesparseconfig)
find_library(BLAS_LIBRARY NAMES blas)

# OSX does not link against lapack
if(NOT APPLE)
  find_library(LAPACK_LIBRARY       NAMES lapack)
endif()

# Dependencies for lapack
get_filename_component(CHOLMOD_ROOT_INCLUDE_DIR "${CHOLMOD_INCLUDE_DIR}" DIRECTORY)

message(STATUS "CHOLMOD INCLUDE: "  ${CHOLMOD_INCLUDE_DIR} )
message(STATUS "CHOLMOD LIB: "  ${CHOLMOD_LIBRARY} )
message(STATUS "CCOLMOD LIB: "  ${CCOLAMD_LIBRARY} )
message(STATUS "CAMD LIB: "  ${CAMD_LIBRARY} )
message(STATUS "AMD LIB: "  ${AMD_LIBRARY} )
message(STATUS "SUITESPARSE LIB: "  ${SUITESPARSE_LIBRARY} )
message(STATUS "BLAS LIB: "  ${BLAS_LIBRARY} )

if(NOT APPLE)
  message(STATUS "LAPACK LIB: " ${LAPACK_LIBRARY})
endif()
