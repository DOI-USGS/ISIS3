cmake_minimum_required(VERSION 3.14)

# Setup for SWIG
set(CMAKE_SWIG_FLAGS)
find_package(SWIG REQUIRED)
include(UseSWIG)
list(APPEND CMAKE_SWIG_FLAGS "-py3;-DPY3")

message(STATUS "SWIG flags: " ${CMAKE_SWIG_FLAGS} )

if(EXISTS $ENV{CONDA_PREFIX})
  message("Searching Anaconda environment at $ENV{CONDA_PREFIX} for Python libraries")
  set(Python_FIND_VIRTUALENV FIRST)
  set(Python_FIND_REGISTRY LAST)
  set(Python_FIND_FRAMEWORK LAST)
  set(Python_FIND_STRATEGY LOCATION)
endif()

# Setup for Python linking
find_package(Python3 REQUIRED COMPONENTS Development)

# Setup for wrapper library
set(ASTROSET_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/astroset")
set(ASTROSET_SOURCES astroset.i
                   UserInterface.i 
                   )

set(ASTROSET_APPS apps/)

set_source_files_properties(${ASTROSET_SOURCES} PROPERTIES CPLUSPLUS ON)
swig_add_library(astroset
                 LANGUAGE python
                 SOURCES ${ASTROSET_SOURCES}
                 OUTPUT_DIR ${ASTROSET_OUTPUT_DIR})
swig_link_libraries(astroset isis Python3::Module)
set_target_properties(astroset PROPERTIES SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE)
set_target_properties(astroset PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ASTROSET_OUTPUT_DIR})

# Create the files to install the Python wrapper
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
               ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
               ${ASTROSET_OUTPUT_DIR}/__init__.py
               COPYONLY)

file(COPY ${ASTROSET_APPS}
          DESTINATION
          ${ASTROSET_OUTPUT_DIR}/apps/)

set(ASTROSET_APP_XML_FILES "apps/findFeaturesSegment/findFeaturesSegment.xml")
file(COPY ${ASTROSET_APP_XML_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin/xml/ )
install(FILES ${ASTROSET_APP_XML_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/xml/)

# Setup to run setup tools on install 
install(CODE "execute_process(COMMAND $ENV{CONDA_PREFIX}/bin/pip install ${CMAKE_CURRENT_BINARY_DIR}/ --prefix=${CMAKE_INSTALL_PREFIX}
                              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")

