# Include python
include(FindPythonAnaconda.cmake)
set(ISIS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
include_directories(${PYTHON_INCLUDE_DIR} ${ISIS_INCLUDE_DIR})

set(CMAKE_SWIG_FLAGS "")
set(CMAKE_SWIG_OUTDIR isiscore)
set_source_files_properties(../isiscore.i
                            PROPERTIES CPLUSPLUS ON SWIG_FLAGS "-py3")

# Add and link
swig_add_library(isiscore
                 LANGUAGE python
                 SOURCES ../isiscore.i)

set_target_properties(_isiscore PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SWIG_OUTDIR})

if (APPLE)
     set_target_properties(_isiscore PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
     # On OSX, do not link python - this causes segfaults
     swig_link_libraries(isiscore core)
else()
     swig_link_libraries(isiscore core ${PYTHON_LIBRARIES})
endif()

# Build out a standard directory structure
# file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)

# Move the static files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
               ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
              ${CMAKE_CURRENT_BINARY_DIR}/isiscore/__init__.py
              COPYONLY)

## Move the tests
#file(COPY ${CMAKE_SOURCE_DIR}/python/tests
#     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install . --prefix=${CMAKE_INSTALL_PREFIX} --no-deps --no-build-isolation
                              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")
                              