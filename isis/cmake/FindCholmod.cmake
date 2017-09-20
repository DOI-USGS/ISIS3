# CMake module for find_package(Cholmod)
# Finds include directory and all applicable libraries
#
# Sets the following:
#   CHOLMOD_INCLUDE_DIR
#   CHOLMOD_LIBLIST

find_path(CHOLMOD_INCLUDE_DIR
  NAME cholmod.h
  PATH_SUFFIXES SuiteSparse/SuiteSparse4.4.5/SuiteSparse/ SuiteSparse
)

find_library(CHOLMOD_LIBRARY      NAMES cholmod)
find_library(CCOLAMD_LIBRARY      NAMES ccolamd)
find_library(COLAMD_LIBRARY       NAMES colamd)
find_library(CAMD_LIBRARY         NAMES camd)
find_library(AMD_LIBRARY          NAMES amd)
find_library(SUITESPARSE_LIBRARY  NAMES suitesparseconfig)
find_library(LAPACK_LIBRARY       NAMES lapack)

# Dependencies for lapack

# add gcc location for MacOS
find_library(FORTRAN_LIBRARY      NAMES gfortran
  NAMES gfortran
  PATHS /opt/usgs/v006/ports/lib/gcc5/
)

find_library(BLAS_LIBRARY NAMES blas)

get_filename_component(CHOLMOD_ROOT_INCLUDE_DIR "${CHOLMOD_INCLUDE_DIR}" DIRECTORY)
