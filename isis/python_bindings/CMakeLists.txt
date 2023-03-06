cmake_minimum_required(VERSION 3.14)

# Setup for SWIG
set(CMAKE_SWIG_FLAGS)
find_package(SWIG REQUIRED)
include(UseSWIG)
list(APPEND CMAKE_SWIG_FLAGS "-py3;-DPY3")

message(STATUS "SWIG flags: " ${CMAKE_SWIG_FLAGS} )

# Setup for Python linking
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Setup for wrapper library
set(ISISPY_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/astroset")
set(ISISPY_SOURCES astroset.i
                   UserInterface.i 
                   )
set_source_files_properties(${ISISPY_SOURCES} PROPERTIES CPLUSPLUS ON)
swig_add_library(astroset
                 LANGUAGE python
                 SOURCES ${ISISPY_SOURCES}
                 OUTPUT_DIR ${ISISPY_OUTPUT_DIR})
swig_link_libraries(astroset isis Python3::Module)
set_target_properties(astroset PROPERTIES SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE)
set_target_properties(astroset PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ISISPY_OUTPUT_DIR})

# Setup for wrapper tests
if(pybindings)
    add_test(NAME python-tests
        COMMAND python -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests
        WORKING_DIRECTORY ${ISISPY_OUTPUT_DIR})
else()
    message(STATUS "Skipping Python Bindings Tests")
endif()

# Create the files to install the Python wrapper
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
               ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
               ${ISISPY_OUTPUT_DIR}/__init__.py
               COPYONLY)

# Setup to run setup tools on install 
install(CODE "execute_process(COMMAND $ENV{CONDA_PREFIX}/bin/python ${CMAKE_CURRENT_BINARY_DIR}/setup.py install --single-version-externally-managed --prefix=${CMAKE_INSTALL_PREFIX} --record=record.txt
                              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")

