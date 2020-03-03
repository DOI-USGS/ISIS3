#===============================================================================
#      The main build file for building ISIS using CMake.
#===============================================================================
# CMake initialization

# Specify the required version of CMake.  If your machine does not
#  have this, it should be easy to build from https://cmake.org/download/
cmake_minimum_required(VERSION 3.10)

# Point cmake to our other CMake files.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# set(CMAKE_FIND_FRAMEWORK LAST)
set(CMAKE_FRAMEWORK_PATH /System/Library/Frameworks/)

set(CMAKE_FIND_FRAMEWORK LAST)

include(AddIsisModule)
include(Utilities)
include(TestSetup)
include(InstallThirdParty)
include(cmake/gtest.cmake)
include(GoogleTest)

#===============================================================================
#===============================================================================
# Project information

project (USGS_ISIS)

# Short and long name of this package
set(PACKAGE            "ISIS")
set(PACKAGE_NAME       "USGS ISIS")

# Version number
set(VERSION            "4.1.0")
set(PACKAGE_VERSION    ${VERSION})

# Full name and version number
set(PACKAGE_STRING     "${PACKAGE_NAME} ${VERSION}")

# Other release information
string(TIMESTAMP VERSION_DATE "%Y-%m-%d" UTC)
set(RELEASE_STAGE             "beta") # (alpha, beta, stable)

# Define to the address where bug reports for this package should be sent.
set(PACKAGE_BUGREPORT  "https://github.com/USGS-Astrogeology/ISIS3/issues")

# Main website associated with the software package
set(PACKAGE_URL        "https://isis.astrogeology.usgs.gov/")

# Retrieve a string describing the OS this is built on.
get_os_version(osVersionString)
message("Detected Operating System: ${osVersionString}")

#===============================================================================
#===============================================================================
# Configuration options

# All libraries are build as shared.  The main library is also built
#  as a static library using some specialized code in Utilities.cmake.
set(BUILD_SHARED_LIBS ON)

# make sure to leave rpaths untouched on install
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Specify user options that can be passed in with the initial CMake command.
option(isis3Data       "Directory containing Isis3Data"                 OFF )
option(isis3TestData   "Directory containing Isis3TestData"             OFF )
option(testOutputDir   "Directory to store app test output folders"     OFF )
option(buildCore       "Build the core ISIS modules"                    ON  )
option(buildMissions   "Build the mission specific modules"             ON  )
option(buildStaticCore "Build libisis3 static as well as dynamic"       OFF )
option(buildTests      "Set up unit, application, and module tests."    ON  )
option(JP2KFLAG        "Whether or not to build using JPEG2000 support" OFF )
option(pybindings      "Turn on to build Python bindings"               OFF )

# if cmake install prefix is not set, and conda env is activated, use the
# conda env as the install directory
if(DEFINED ENV{CONDA_PREFIX} AND CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
 set(CMAKE_INSTALL_PREFIX $ENV{CONDA_PREFIX})
endif()

# Prioritize passed in variables over env vars, probably a better way to do this
if(DEFINED ENV{ISIS3DATA} AND NOT isis3Data)
 set(isis3Data $ENV{ISIS3DATA})
endif()
if(DEFINED ENV{ISIS3TESTDATA} AND NOT isis3TestData)
 set(isis3TestData $ENV{ISIS3TESTDATA})
endif()

if(EXISTS ${isis3Data})
 set(ENV{ISIS3DATA} "${isis3Data}")
else()
 message(WARNING "Isis3Data directory ${isis3Data} not found, unit tests will fail.")
 set(isis3Data OFF)
endif()

if(EXISTS ${isis3TestData})
 set(ENV{ISIS3TESTDATA} "${isis3TestData}")
else()
 message(WARNING "Isis3TestData directory ${isis3TestData} not found, application and module tests will fail.")
 set(isis3TestData OFF)
endif()

if(${testOutputDir} STREQUAL "OFF")
 message("Writing test data folders to = ${CMAKE_BINARY_DIR}/testOutputDir")
 set(testOutputDir "${CMAKE_BINARY_DIR}/testOutputDir")
 execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/testOutputDir)
else()
 # User specified a test output folder, make sure it exists.
 message("Writing test data folders to = ${testOutputDir}")
 execute_process(COMMAND mkdir -p ${testOutputDir})
endif()

# inject ISISROOT
add_definitions( -DISISROOT="${CMAKE_SOURCE_DIR}" )
add_definitions( -DISISBUILDDIR="${CMAKE_BINARY_DIR}" )

message("CONFIGURATION")
message("\tBUILD STATIC CORE: ${buildStaticCore}")
message("\tBUILD TESTS: ${buildTests}")
message("\tBUILD CORE: ${buildCore}")
message("\tBUILD MISSIONS: ${buildMissions}")
message("\tJP2K SUPPORT: ${JP2KFLAG}")
message("\tPYTHON BINDINGS: ${pybindings}")
message("\tISIS3DATA: ${isis3Data}")
message("\tISISTESTDATA: ${isis3TestData}")
message("\tTEST OUTPUT DIR: ${testOutputDir}")
message("\tINSTALL PREFIX: ${CMAKE_INSTALL_PREFIX}")

#===============================================================================
#===============================================================================

# Set up the ctest tool which is used to run all of the tests.
enable_testing()
include(CTest)

# Set up Anaconda prefix in the case with a non-default conda env is activated
if(EXISTS $ENV{CONDA_PREFIX})
  message("CONDA PREFIX: $ENV{CONDA_PREFIX}")
  list(APPEND CMAKE_FIND_ROOT_PATH $ENV{CONDA_PREFIX}
                                   $ENV{CONDA_PREFIX}/lib/cmake/Qt5)
endif()

# options only allow on/off but this flag is piped into ISIS as ENABLEJP2K
# needs to be either 1 or 0 for C style true false
if(JP2KFLAG)
 set(JP2KFLAG 1)
endif()

# Set up the ctest tool which is used to run all of the tests.
enable_testing()
include(CTest)

# Specify flags used
# on linux, add the conda prefix to handle possible issues with rpaths at link time
# sometimes third parties do not set their rpaths correctly
set(thirdPartyCppFlags -Wall
                       -fPIC
                       -std=c++11
                       -DISIS_LITTLE_ENDIAN=1
                       -Wno-unused-parameter
                       -Wno-overloaded-virtual
                       -Wno-strict-aliasing
                       -DUSE_UNSTABLE_GEOS_CPP_API=1
		               -Wno-strict-overflow
                       -DGMM_USES_SUPERLU
                       -DENABLEJP2K=${JP2KFLAG}
                     )

 # Append CPP flags set in the third party lib file to the string set in this file.
 string(REPLACE ";" " " FLAGS_STR "${thirdPartyCppFlags}")

 set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${FLAGS_STR}" )


# Flag to fix numeric literals problem with boost on linux
# Add gold linker (and therefore, phtread) to speed up linux (spec. Ubuntu18.04) builds
if(NOT APPLE)
  set(thirdPartyCppFlags ${thirdPartyCppFlags} -fuse-ld=gold
	                                       -pthread
					       -fext-numeric-literals
                                               -Wl,-rpath,$ENV{CONDA_PREFIX}/lib)
endif()

# Append CPP flags set in the third party lib file to the string set in this file.
string(REPLACE ";" " " FLAGS_STR "${thirdPartyCppFlags}")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${FLAGS_STR}" )

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "RELEASE")
endif()

if(APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og")
endif()

# Paths to required executables
find_program(XALAN Xalan REQUIRED)
find_program(LATEX latex)
find_program(DOXYGEN NAME doxygen PATH_SUFFIXES doxygen REQUIRED)
find_program(UIC uic REQUIRED)
find_program(MOC moc REQUIRED)
find_program(RCC rcc REQUIRED)
find_program(PROTOC protoc REQUIRED)

find_package(Qt5 COMPONENTS
                Core
                Concurrent
                Gui
                Multimedia
                MultimediaWidgets
                Network
                OpenGL # Needed to install mesa-common-dev for this!
                PrintSupport
                Qml
                Quick
                Script
                ScriptTools
                Sql
                Svg
                Test
                WebChannel
                Widgets
                Xml
                XmlPatterns
                # Search this path explicitly for MacOS OpenGL Framework
                PATHS /System/Library/Frameworks/ REQUIRED)

find_package(Boost 1.59.0 REQUIRED atomic
                                        log
                                        regex
                                        log_setup
                                        serialization
                                        chrono
                                        math_c99
                                        signals
                                        container
                                        math_c99f
                                        context
                                        math_c99l
                                        system
                                        math_tr1
                                        math_tr1f
                                        math_tr1l
                                        test_exec_monitor
                                        date_time
                                        thread
                                        exception
                                        timer
                                        filesystem
                                        prg_exec_monitor
                                        program_options
                                        unit_test_framework
                                        iostreams
                                        wave
                                        random
                                        wserialization)

find_package(ale               REQUIRED)
find_package(Json              REQUIRED)
find_package(Bullet    2.86    REQUIRED)
find_package(Cholmod   4.4.5   REQUIRED)
find_package(CSPICE    65      REQUIRED)
find_package(Eigen             REQUIRED)
find_package(Embree    2.15.0  REQUIRED)
find_package(GeoTIFF   2       REQUIRED)
find_package(GMM       5.0     REQUIRED)
SET(GSL_ROOT_DIR $ENV{CONDA_PREFIX})
find_package(GSL       2.2.1   REQUIRED)
find_package(HDF5      1.8.15  REQUIRED)
find_package(Jama      125     REQUIRED)
find_package(NN                REQUIRED)
find_package(OpenCV    3.1.0   REQUIRED)
find_package(PCL               REQUIRED)
find_package(Protobuf  2.6.1   REQUIRED)
find_package(Qwt       6       REQUIRED)
find_package(SuperLU   4.3     REQUIRED)
find_package(TIFF      4.0.0   REQUIRED)
find_package(TNT       126     REQUIRED)
find_package(XercesC   3.1.2   REQUIRED)
find_package(X11       6       REQUIRED)
find_package(nanoflann         REQUIRED)
find_package(PNG               REQUIRED)
find_package(Kakadu)
find_package(Geos    3.5.0     REQUIRED)
find_package(Armadillo         REQUIRED)
find_package(Threads)


# In this case, we specify the version numbers being searched for in the non-traditional installs.
if(APPLE)
  find_package(OpenGL REQUIRED)
endif(APPLE)

if(pybindings)
 find_package(Python REQUIRED)
 find_package(Sip    REQUIRED)
endif()

# Iterate through all variables and extract the libraries and include directories
get_cmake_property(_variableNames VARIABLES) # Get All VARIABLES

set(ALLLIBDIRS "")
set(ALLLIBS "")
set(ALLINCDIRS "")

# Get all include dir variables
foreach (_variableName ${_variableNames})
   if (_variableName MATCHES ".+_INCLUDE_DIR$")
     list(APPEND ALLINCDIRS "${${_variableName}}")
   elseif (_variableName MATCHES ".+_INCLUDE_PATH$")
     list(APPEND ALLINCDIRS "${${_variableName}}")
   endif(_variableName MATCHES ".+_INCLUDE_DIR$")
endforeach()

# get all Library variables
foreach (_variableName ${_variableNames})
   get_filename_component(LIBDIR "${${_variableName}}" DIRECTORY)
   if (_variableName MATCHES "^CMAKE+")
   elseif (_variableName MATCHES ".+_LIBRARY$")
     list(APPEND ALLLIBDIRS "${LIBDIR}")
     list(APPEND ALLLIBS "${${_variableName}}")
   elseif (_variableName MATCHES ".+_LIBRARIES$")
     list(APPEND ALLLIBDIRS "${LIBDIR}")
     list(APPEND ALLLIBS "${${_variableName}}")
   endif()
endforeach()

# Sometimes we add the same lib more than once (especially with LIBDIRS)
list(REMOVE_DUPLICATES ALLLIBDIRS)
list(REMOVE_DUPLICATES ALLLIBS)
list(REMOVE_DUPLICATES ALLINCDIRS)

#===============================================================================
#===============================================================================

# Set python bindings configuration and set target for generating C++ files
if(pybindings)
  message("Configuring Python Bindings")

  if (NOT DEFINED PYINSTALL_DIR)
    set(PYINSTALL_DIR ${PYTHON_SITE_PACKAGES_DIR})
  endif()
  message(STATUS "PYTHON BINDINGS INSTALL DIR: ${PYINSTALL_DIR}")

 # We need to get the locations for sip files, modules, etc.
 set(ISIS_SIP_DIR "${CMAKE_SOURCE_DIR}/sipfiles")
 set(ISIS_SIP_MODULE "${CMAKE_SOURCE_DIR}/sipfiles/master.sip")
 set(SIP_BUILD_FILE "isispy.sbf")
 set(ISIS_SIP_CODE_DIR ${CMAKE_BINARY_DIR}/sipsrc)

 # Create the output directory for the new .CPP files
 execute_process(COMMAND mkdir -p "${ISIS_SIP_CODE_DIR}")

 # get list of output files exepected from sip

 # get the PYQT configuration based flags from Python
 execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                 "from PyQt5.QtCore import PYQT_CONFIGURATION as qtconfigdict;print(qtconfigdict['sip_flags'])"
                 OUTPUT_VARIABLE PYQT_SIP_FLAGS)

 # CMAKE doesn't handle spaces from python well when piping that into the
 # command because of white space and a trailing new line,
 # so we turn it into a list
 message(STATUS "Getting SIP config...")
 message(STATUS "Attempting 'python -c \"from PyQt5.QtCore import PYQT_CONFIGURATION as qtconfigdict; print(qtconfigdict['sip_flags'])\"'")

 if (${PYQT_SIP_FLAGS} STREQUAL "")
    message(FATAL_ERROR "print(qtconfigdict['sip_flags']) returned empty string, is sip installed? Python binding can be disabled with pybindings=OFF")
 endif()
 string(STRIP ${PYQT_SIP_FLAGS} PYQT_SIP_FLAGS)
 string(REPLACE " " ";" PYQT_SIP_FLAGS ${PYQT_SIP_FLAGS})

 message(STATUS "FLAGS: ${PYQT_SIP_FLAGS}")
 message(STATUS "Generating C++ code from sip files")
 message(STATUS "SIP BUILD FILE: ${SIP_BUILD_FILE}")
 message(STATUS "SIP MODULE: ${ISIS_SIP_MODULE}")
 message(STATUS "SIP GENERATED CODE DIR: ${ISIS_SIP_CODE_DIR}")

 execute_process(COMMAND ${SIP_BINARY_PATH} -e -o -c ${ISIS_SIP_CODE_DIR} -I ${SIP_DEFAULT_SIP_DIR}/PyQt5 ${PYQT_SIP_FLAGS} ${ISIS_SIP_MODULE})

 # add target so users can run the command after initial configuration
 add_custom_target(sipfiles
                  COMMAND ${SIP_BINARY_PATH} -e -o -c ${ISIS_SIP_CODE_DIR} -I ${SIP_DEFAULT_SIP_DIR}/PyQt5 ${PYQT_SIP_FLAGS} ${ISIS_SIP_MODULE}
                  COMMENT "Generating C++ code from sip files")

 file(GLOB SIP_GENERATED_SOURCE_FILES ${ISIS_SIP_CODE_DIR}/*.cpp)
 add_library(isispy MODULE ${SIP_GENERATED_SOURCE_FILES})
 target_link_libraries(isispy ${ALLLIBS})
 target_link_libraries(isispy isis3)
 set_target_properties(isispy PROPERTIES LINK_DEPENDS isis3 INSTALL_RPATH ${CMAKE_INSTALL_PREFIX})
 add_dependencies(isispy sipfiles)

 install(TARGETS isispy DESTINATION ${PYINSTALL_DIR})
endif()


#===============================================================================
#===============================================================================

# Start setting up the build
# Add extension to find fortran until .so symlink can be added to /usr/lib64
list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES .so.3 .so.6 .so.5)

# Allow everything to include the 3rd party libraries
include_directories(SYSTEM ${ALLINCDIRS})
link_directories(${ALLLIBDIRS})

include_directories(${CMAKE_BINARY_DIR}/inc)
set(CORE_LIB_NAME isis3)

# Specify relative library include paths which will be set up on
#  the installed files.
if(APPLE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};@loader_path/../lib;@loader_path/../3rdParty/lib")
else()
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};$ORIGIN/../lib;$ORIGIN/../3rdParty/lib")
endif()

# We will set up some links with these files at the end of the install process so
#  make sure they are cleared at the start of the install process.
install(CODE "EXECUTE_PROCESS(COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/lib/libisis3.6.0${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/lib/libisis3.6${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/lib/libisis3.${SO})")
EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_SOURCE_DIR}/TestPreferences ${CMAKE_BINARY_DIR}/)
install(CODE "EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_SOURCE_DIR}/src/base/objs/Preference/TestPreferences ${CMAKE_INSTALL_PREFIX}/)")
install(CODE "EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_SOURCE_DIR}/IsisPreferences ${CMAKE_INSTALL_PREFIX}/)")

# Install the header files
install(CODE "EXECUTE_PROCESS(COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/include/isis3)")
file(GLOB ISIS_INCLUDE_FILES ${CMAKE_BINARY_DIR}/inc/*)
install(CODE "EXECUTE_PROCESS(COMMAND cp -fv ${ISIS_INCLUDE_FILES} ${CMAKE_INSTALL_PREFIX}/include/isis3)")

# Delete any existing plugin files in the build folder so they
#  don't get filled with duplicate entries.
file(GLOB existingPlugins "${CMAKE_BINARY_DIR}/plugins/*.plugin")
if(existingPlugins)
  file(REMOVE ${existingPlugins})
endif()

# Add a config file to the install bin directory so that QT can find the plugin libraries.
file(WRITE "${CMAKE_BINARY_DIR}/qt.conf" "[Paths]\nPlugins=../3rdParty/plugins/\n")
install(FILES "${CMAKE_BINARY_DIR}/qt.conf" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/xml)

#Create the inc directory
execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/inc)

# Create an xml folder in the source directory that we will need later
set(sourceXmlFolder ${CMAKE_BINARY_DIR}/bin/xml)
execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin/xml)

# Create the version file
configure_file(${CMAKE_SOURCE_DIR}/cmake/version.in ${CMAKE_BINARY_DIR}/version)

# Set up install of the templates folder.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/templates DESTINATION .)

# Set up install of the make folder.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/make DESTINATION ${CMAKE_INSTALL_PREFIX})

# Have CMake process all of the source code and tests.
add_subdirectory(src objects)

if(APPLE)
 set(SO ".dylib")
else()
 set(SO ".so")
endif()

# Set up documentation build target.
# - This script is called by running "ninja docs".
# - This long call passes all desired variables to the script.
add_custom_target(docs COMMAND ${CMAKE_COMMAND}
                  -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
                  -DDOXYGEN=${DOXYGEN}  -DXALAN=${XALAN}
                  -DLATEX=${LATEX}
                  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                  -P ${CMAKE_MODULE_PATH}/BuildDocs.cmake)

# Add custom build target to copy modified header files to the build/incs directory.
# ALL is specified so that the target is added to the default build target, i.e. the copy command
# will be executed when running "ninja install"
# On a clean build, all files will be copied over.
add_custom_target(incs ALL COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${CMAKE_SOURCE_DIR}/src/*/objs/*/*.h ${CMAKE_SOURCE_DIR}/src/*/objs/*/*.hpp
  ${CMAKE_SOURCE_DIR}/src/*/apps/*/*.h ${CMAKE_BINARY_DIR}/inc)
add_dependencies(isis3 incs)

# Add a custom build target to clean out everything that gets added to the source
#  directory during the build process.
# - Only a few things are added in order to make the tests work properly so
#   this is very straightforward.
add_custom_target(clean_source COMMAND rm -rf "${CMAKE_BINARY_DIR}/*" "${CMAKE_INSTALL_PREFIX}/*")

# Set up a few top level files for installation.
EXECUTE_PROCESS(COMMAND cp -f  ${CMAKE_SOURCE_DIR}/IsisPreferences   ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND cp -rf ${CMAKE_SOURCE_DIR}/scripts           ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND cp -f  ${CMAKE_SOURCE_DIR}/../LICENSE.txt    ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND cp -rf ${CMAKE_SOURCE_DIR}/make              ${CMAKE_BINARY_DIR})

# Copy the files on make install as well
install(FILES     ${CMAKE_SOURCE_DIR}/IsisPreferences DESTINATION  ${CMAKE_INSTALL_PREFIX})
install(FILES     ${CMAKE_SOURCE_DIR}/../LICENSE.txt  DESTINATION  ${CMAKE_INSTALL_PREFIX})
install(FILES     ${CMAKE_BINARY_DIR}/version         DESTINATION  ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts         DESTINATION  ${CMAKE_INSTALL_PREFIX})

# Trigger all post-install behavior.
# - The only way to run commands post-install in CMake is to add a subdirectory at
#   the end of this file containing a CMakeLists.txt file which includes all of
#   the desired post-install commands inside.
add_subdirectory(cmake)
option (BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
