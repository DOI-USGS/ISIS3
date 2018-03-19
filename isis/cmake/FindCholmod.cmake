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

# OSX does not link against lapack
if(NOT APPLE)
  find_library(LAPACK_LIBRARY       NAMES lapack)
endif()

# Dependencies for lapack

# add gcc location for MacOS
# if(APPLE)
  find_library(FORTRAN_LIBRARY      NAMES gfortran
    NAMES gfortran
    PATHS /opt/usgs/v007/ports/lib/gcc5/
  )

  find_library(BLAS_LIBRARY NAMES blas)
# endif(APPLE)

get_filename_component(CHOLMOD_ROOT_INCLUDE_DIR "${CHOLMOD_INCLUDE_DIR}" DIRECTORY)

message( "-- CHOLMOD INCLUDE: "  ${CHOLMOD_INCLUDE_DIR} )
message( "-- CHOLMOD LIB: "  ${CHOLMOD_LIBRARY} )
message( "-- CCOLMOD LIB: "  ${CCOLAMD_LIBRARY} )
message( "-- CAMD LIB: "  ${CAMD_LIBRARY} )
message( "-- AMD LIB: "  ${AMD_LIBRARY} )
message( "-- SUITESPARSE LIB: "  ${SUITESPARSE_LIBRARY} )
message( "-- FORTRAN LIB: "  ${FORTRAN_LIBRARY} )
message( "-- BLAS LIB: "  ${BLAS_LIBRARY} )

if(NOT APPLE)
  message("LAPACK LIB" ${LAPACK_LIBRARY})
endif()
