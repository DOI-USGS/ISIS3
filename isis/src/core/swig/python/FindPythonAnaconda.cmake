cmake_minimum_required(VERSION 3.12)

option(ANACONDA_PYTHON_VERBOSE "Anaconda dependency info" OFF)

if(NOT CMAKE_FIND_ANACONDA_PYTHON_INCLUDED)
  set(CMAKE_FIND_ANACONDA_PYTHON_INCLUDED 1)

  find_package(Python REQUIRED COMPONENTS Interpreter Development)

  if(ANACONDA_PYTHON_VERBOSE)
    message(STATUS "Found Python:")
    message(STATUS "  Interpreter: ${Python_EXECUTABLE}")
    message(STATUS "  Version: ${Python_VERSION}")
    message(STATUS "  Include dirs: ${Python_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${Python_LIBRARIES}")
  endif()

  set(PYTHON_INCLUDE_DIRS "${Python_INCLUDE_DIRS}")
  set(PYTHON_INCLUDE_DIR  "${Python_INCLUDE_DIRS}")

  set(PYTHON_LIBRARIES    "${Python_LIBRARIES}")
  set(PYTHON_LIBRARY       "${Python_LIBRARIES}")

  set(PYTHON_EXECUTABLE    "${Python_EXECUTABLE}")

  set(FOUND_PYTHONLIBS TRUE)

endif()