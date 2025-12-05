# CMake module for find_package(BulletFloat64) double precision package
# Finds include directory and all applicable libraries
#
#[=[
This Bullet find module provides a target definition for the double precision
version of the Bullet library. The target is provided as an imported library
interface. It is derived from CMAKE varibles provided in the Bullet package
configuration. The Bullet package is assumed to be provided in the Conda 
environment but could be used by other compatible Bullet configurations that
provide a double precision version the Bullet package.

Note the only thing that differentiates the Conda package manager version is
the explicit find_package() call that is specific to Conda. In the Conda
Bullet package, the double precision package configuration is provided in 
a separate CMAKE file, namely BulletConfig-float64.cmake. Other package
managers use different techniques. However any package manager that provides
consistent definitions of the Bullet CMAKE variables can use the 
add_bullet_double_target() macro to create the Bullet::Bullet_double target.

This module provides the following elements:

add_bullet_double_target()   # Macro to generate the bullet Target
Bullet::Bullet_double        # INTERFACE IMPORTED library target
Bullet_double_FOUND          # Module found variable

To add this target to ISIS, just append the Bullet::Bullet_double target
to ALLLIBS variable in ISIS3/isis/CMakeLists.txt file. This package
config also produces ISIS compatible variables that can be used instead
of the Bullet double target if preferred. However, the target is required
to add the compile definitions in Bullet::Bullet_double target if not
added explicitly in ISIS cmake system.

BULLET_DEFINITIONS           # Bullet double compile flags
BULLET_INCLUDE_DIR           # Include directory for Bullet
BULLET_LIBRARY_DIRS          # Library directory for Bullet
BULLET_LIBRARIES             # Absolute paths of all Bullet libraries
]=]

function(prepend_root_dir ifile rootpath outvar)
  string(FIND "${ifile}" "${rootpath}" _root_pos)
  set(_outpath "${ifile}")
  if(_root_pos EQUAL -1)
    string(PREPEND _outpath "${rootpath}/")
  endif()
  set(${outvar} "${_outpath}" PARENT_SCOPE)
endfunction(prepend_root_dir)


function(get_full_library_path inlibs libpath outlibs)
  # WARNING: This loop is sensitive in that you must use NO_CACHE
  # AND unset the _lib_t variable in order to find all the libraries.
  # Otherwise, it only finds the first library! Repeatedly...
  foreach(_lib IN LISTS inlibs)
    find_library(_lib_t "${_lib}" PATH "${libpath}" NO_CACHE)
    list(APPEND _alllibs ${_lib_t})
    unset(_lib_t) # Must be done or it only finds the first library.
  endforeach()
  set(${outlibs} "${_alllibs}" PARENT_SCOPE)
endfunction(get_full_library_path)

macro(add_bullet_double_target)

  if ( NOT TARGET Bullet::Bullet_double )
    if (BULLET_FOUND OR Bullet_FOUND )
      if(NOT "${BULLET_DEFINITIONS}" MATCHES ".*-DBT_USE_DOUBLE_PRECISION.*")
        message(FATAL_ERROR "Bullet does not appear to be built with "
                   "double precision, current definitions: ${BULLET_DEFINITIONS}")
      endif()
      message(STATUS "Bullet Double Compile Definitions: ${BULLET_DEFINITIONS}")

      # This configuration ensures the Bullet variable definitions are also conformant.
      # It also set to work with the ISIS system.
      add_library( Bullet::Bullet_double INTERFACE IMPORTED )
      set(BULLET_INCLUDE_DIR ${BULLET_INCLUDE_DIRS})
      prepend_root_dir("${BULLET_INCLUDE_DIR}"    "${BULLET_ROOT_DIR}"     BULLET_INCLUDE_DIR)
      prepend_root_dir("${BULLET_LIBRARY_DIRS}"   "${BULLET_ROOT_DIR}"     BULLET_LIBRARY_DIRS)
      get_full_library_path("${BULLET_LIBRARIES}" "${BULLET_LIBRARY_DIRS}" BULLET_LIBRARIES)
      
      set_target_properties( Bullet::Bullet_double
        PROPERTIES
          INTERFACE_COMPILE_DEFINITIONS "${BULLET_DEFINITIONS}"
          INTERFACE_INCLUDE_DIRECTORIES "${BULLET_INCLUDE_DIR}"
          INTERFACE_LINK_DIRECTORIES "${BULLET_LIBRARY_DIRS}"
          INTERFACE_LINK_LIBRARIES "${BULLET_LIBRARIES}"
      )
      set(Bullet_double_FOUND TRUE)
      message(STATUS "Bullet Double Target Created/Confirmed: Bullet::Bullet_double")
      message(STATUS "Bullet Includes:  ${BULLET_INCLUDE_DIR}")
      message(STATUS "Bullet Libdirs:  ${BULLET_LIBRARY_DIRS}")
      message(STATUS "Bullet Libraries: ${BULLET_LIBRARIES}")
    endif()
  endif()

endmacro()

# Call the Conda specific Bullet double precision package
find_package(Bullet REQUIRED CONFIGS BulletConfig-float64.cmake)

# Create/confirm the Conda Bullet double precision interface target.
# Note this target needs to be added to the end of the ALLLIBS variable
# in ISIS3/isis/CMakeLists.txt file.
add_bullet_double_target()
