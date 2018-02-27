#===============================================================================
#      The main build file for building ISIS using CMake.
#===============================================================================
# CMake initialization

# Specify the required version of CMake.  If your machine does not
#  have this, it should be easy to build from https://cmake.org/download/
cmake_minimum_required(VERSION 3.4)

# Point cmake to our other CMake files.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

list(APPEND CMAKE_INCLUDE_PATH
  /usgs/pkgs/local/v007/include/
  /usgs/pkgs/local/v007/bin/
  /usgs/pkgs/local/v007/lib/
  /usgs/pkgs/local/v007/objects/
  /usgs/pkgs/local/v007/include/google-protobuf/protobuf2.6.1/
  /usgs/pkgs/local/v007/include/xercesc/xercesc-3.1.2/
  /usgs/pkgs/local/v007/include/tiff/tiff-4.0.5/
  /usr/lib64/
)

set(CMAKE_PREFIX_PATH
  /usgs/pkgs/local/v007/include/
  /usgs/pkgs/local/v007/bin/
  /usgs/pkgs/local/v007/lib/
  /usgs/pkgs/local/v007/libexec/
  /opt/usgs/v007/ports/Library/Frameworks/
  /opt/usgs/v007/ports/libexec/
  /opt/usgs/v007/ports/bin/
  /opt/usgs/v007/ports/lib/
  /opt/usgs/v007/ports/include/
  /opt/usgs/v007/ports/libexec/qt5
  /opt/usgs/v007/ports/libexec/qt5/bin/
  /opt/usgs/v007/ports/libexec/qt5/lib/
  /opt/usgs/v007/3rdparty/bin
  /opt/usgs/v007/3rdparty/include/
  /opt/usgs/v007/3rdparty/lib/
  /opt/usgs/v007/proprietary/
  /opt/usgs/v007/proprietary/include/
  /opt/usgs/v007/proprietary/lib/
  /usr/lib/
  /usr/lib64/
  /usr/local/lib/
)

set(CMAKE_FIND_FRAMEWORK LAST)

include(AddIsisModule)
include(Utilities)
include(TestSetup)
include(InstallThirdParty)

#===============================================================================
#===============================================================================
# Project information

project (USGS_ISIS)


# Short and long name of this package
set(PACKAGE            "ISIS")
set(PACKAGE_NAME       "USGS ISIS")

# Version number
set(VERSION            "3.5.00.0")
set(PACKAGE_VERSION    ${VERSION})

# Full name and version number
set(PACKAGE_STRING     "${PACKAGE_NAME} ${VERSION}")

# Other release information
set(VERSION_DATE              "2017-04-24")
set(THIRD_PARTY_LIBS_VERSION  "v007")
set(RELEASE_STAGE             "alpha") # (alpha, beta, stable)

# Define to the address where bug reports for this package should be sent.
set(PACKAGE_BUGREPORT  "https://isis.astrogeology.usgs.gov/fixit")

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

# Specify flags used
set(thirdPartyCppFlags -Wall -std=c++11 -DISIS_LITTLE_ENDIAN=1 -fPIC -Wno-unused-parameter -Wno-overloaded-virtual)
#set(thirdPartyCppFlags -Wall -ansi -DISIS_LITTLE_ENDIAN=1 -fPIC -Wno-unused-parameter -Wno-overloaded-virtual)

# Specify user options that can be passed in with the initial CMake command.
option(isis3Data       "Directory containing Isis3Data"              "NA")
option(isis3TestData   "Directory containing Isis3TestData"          "NA")
option(testOutputDir   "Directory to store app test output folders"  "NA")
option(buildCore       "Build the core ISIS modules"                 ON  )
option(buildMissions   "Build the mission specific modules"          ON  )
option(buildStaticCore "Build libisis3 static as well as dynamic"    OFF )
option(buildTests      "Set up unit, application, and module tests." ON  )
option(JP2KFLAG        "Whether or not to build using JPEG200 support" OFF )

if(EXISTS ${isis3Data})
  set(ENV{ISIS3DATA} "${isis3Data}")
  message("Using ISIS3DATA = $ENV{ISIS3DATA}")
else()
  message(WARNING "Isis3Data directory ${isis3Data} not found, unit tests will fail.")
endif()
if(EXISTS ${isis3TestData})
  set(ENV{ISIS3TESTDATA} "${isis3TestData}")
  message("Using ISIS3TESTDATA = $ENV{ISIS3TESTDATA}")
else()
  message(WARNING "Isis3TestData directory ${isis3TestData} not found, application and module tests will fail.")
endif()
#if(EXISTS ${CMAKE_INSTALL_PREFIX})
#  set(ENV{CMAKE_INSTALL_PREFIX} "${CMAKE_INSTALL_PREFIX}")
#  message("Using INSTALL PREFIX = $ENV{CMAKE_INSTALL_PREFIX}")
#else()
#  message(WARNING "Isis3TestData directory ${CMAKE_INSTALL_PREFIX} not found, application and module tests will fail.")
#endif()

if(${testOutputDir} STREQUAL "OFF")
  message("Writing test data folders to = ${CMAKE_BINARY_DIR}/testOutputDir")
  execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/testOutputDir)
else()
  # User specified a test output folder, make sure it exists.
  message("Writing test data folders to = ${testOutputDir}")
  execute_process(COMMAND mkdir -p ${testOutputDir})
endif()

# Set the default library extension based on the platform
if(APPLE)
  set(SO ".dylib")
else()
  set(SO ".so")
endif()


#===============================================================================
#===============================================================================
# Start setting up the build
# Add extension to find fortran until .so symlink can be added to /usr/lib64
list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES .so.3 .so.6 .so.5)

# Set up all the third party library dependencies.
include(FindAllDependencies)

# Allow everything to include the 3rd party libraries
include_directories(SYSTEM ${ALLINCDIRS})
link_directories(${ALLLIBDIRS})

# inject ISISROOT
add_definitions( -DISISROOT="${CMAKE_SOURCE_DIR}" )
add_definitions( -DISISBUILDDIR="${CMAKE_BINARY_DIR}" )

# add isis headers
file(GLOB ISIS_HEADERS ${CMAKE_SOURCE_DIR}/src/*/objs/*/*.h
${CMAKE_SOURCE_DIR}/src/*/objs/*/*.hpp)
file(COPY ${ISIS_HEADERS} DESTINATION ${CMAKE_BINARY_DIR}/inc)

include_directories(${CMAKE_BINARY_DIR}/inc)

set(CORE_LIB_NAME isis3)

# Specify relative library include paths which will be set up on
#  the installed files.
if(APPLE)
  set(CMAKE_INSTALL_RPATH "@loader_path/../lib;@loader_path/../3rdParty/lib")
else()
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib;$ORIGIN/../3rdParty/lib")
endif()
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

# We will set up some links with these files at the end of the install process so
#  make sure they are cleared at the start of the install process.
install(CODE "EXECUTE_PROCESS(COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/lib/libisis3.5.0${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/lib/libisis3.5${SO})")
install(CODE "EXECUTE_PROCESS(COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/lib/libisis3.${SO})")
EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_SOURCE_DIR}/src/base/objs/Preference/TestPreferences ${CMAKE_BINARY_DIR}/)
install(CODE "EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_SOURCE_DIR}/src/base/objs/Preference/TestPreferences ${CMAKE_INSTALL_PREFIX}/)")
install(CODE "EXECUTE_PROCESS(COMMAND cp -f ${CMAKE_SOURCE_DIR}/IsisPreferences ${CMAKE_INSTALL_PREFIX}/)")

# Append CPP flags set in the third party lib file to the string set in this file.
string(REPLACE ";" " " FLAGS_STR "${thirdPartyCppFlags}")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${FLAGS_STR}" )
# Set up the ctest tool which is used to run all of the tests.
enable_testing()
include(CTest)

# Delete any existing plugin files in the build folder so they
#  don't get filled with duplicate entries.
file(GLOB existingPlugins "${CMAKE_BINARY_DIR}/plugins/*.plugin")
if(existingPlugins)
  file(REMOVE ${existingPlugins})
endif()

# Add a config file to the install bin directory so that QT can find the plugin libraries.
file(WRITE "${CMAKE_BINARY_DIR}/qt.conf" "[Paths]\nPlugins=../3rdParty/plugins/\n")
install(FILES "${CMAKE_BINARY_DIR}/qt.conf" DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

#Create the inc directory
execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/inc)

# Create an xml folder in the source directory that we will need later
set(sourceXmlFolder ${CMAKE_BINARY_DIR}/bin/xml)
execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin/xml)

# Set up install of the templates folder.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/templates DESTINATION .)

# Set up install of the make folder.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/make DESTINATION ${CMAKE_INSTALL_PREFIX})

# Have CMake process all of the source code and tests.
add_subdirectory(src objects)

# Set up third party libraries for installation
# install_third_party()

# # Create a link from the build/objects directory to each folder in 3rdPartyLibs/plugins.
# # - This is required so that the plugins can be found during unit tests.
# # TIP: Set "export QT_DEBUG_PLUGINS=1" to help debug plugin errors.
# foreach(plugin ${THIRDPARTYPLUGINFOLDERS})
#   get_filename_component(pluginName ${plugin} NAME)
#   if(NOT EXISTS ${CMAKE_BINARY_DIR}/lib/${pluginName})
#     execute_process(COMMAND ln -s "${plugin}" ${pluginName}
#     WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
#   endif()
# endforeach()

# Set up documentation build target.
# - This script is called by running "make docs".
# - This long call passes all desired variables to the script.
add_custom_target(docs COMMAND ${CMAKE_COMMAND}
                  -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
                  -DDOXYGEN=${DOXYGEN}  -DXALAN=${XALAN}
                  -DLATEX=${LATEX}
                  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                  -P ${CMAKE_MODULE_PATH}/BuildDocs.cmake)

# Add a custom build target to clean out everything that gets added to the source
#  directory during the build process.
# - Only a few things are added in order to make the tests work properly so
#   this is very straightforward.
add_custom_target(clean_source COMMAND rm -rf "${CMAKE_BINARY_DIR}/*" "${CMAKE_INSTALL_PREFIX}/*")

# Set up a few top level files for installation.
EXECUTE_PROCESS(COMMAND cp -f  ${CMAKE_SOURCE_DIR}/IsisPreferences   ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND cp -rf ${CMAKE_SOURCE_DIR}/scripts           ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND cp -f  ${CMAKE_SOURCE_DIR}/license.txt       ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND cp -f  ${CMAKE_SOURCE_DIR}/version           ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND cp -rf ${CMAKE_SOURCE_DIR}/make              ${CMAKE_BINARY_DIR})

# Copy the files on make install as well
install(FILES     ${CMAKE_SOURCE_DIR}/IsisPreferences DESTINATION  ${CMAKE_INSTALL_PREFIX})
install(FILES     ${CMAKE_SOURCE_DIR}/license.txt     DESTINATION  ${CMAKE_INSTALL_PREFIX})
install(FILES     ${CMAKE_SOURCE_DIR}/version         DESTINATION  ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/scripts         DESTINATION  ${CMAKE_INSTALL_PREFIX})

# Trigger all post-install behavior.
# - The only way to run commands post-install in CMake is to add a subdirectory at
#   the end of this file containing a CMakeLists.txt file which includes all of
#   the desired post-install commands inside.
add_subdirectory(cmake)
