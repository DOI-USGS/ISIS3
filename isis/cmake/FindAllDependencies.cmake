#==============================================================================
# High level script to handle all required 3rd party dependencies
# - All of them are expected to be in the 3rdParty folder, this script does not
#   go looking for them if they are not?
#===============================================================================

message("CONDA PREFIX: $ENV{CONDA_PREFIX}")
list(APPEND CMAKE_FIND_ROOT_PATH $ENV{CONDA_PREFIX} $ENV{CONDA_PREFIX}/lib/cmake/Qt5)

list(APPEND CMAKE_INCLUDE_PATH
  /usgs/pkgs/local/v007/include/
  /usgs/pkgs/local/v007/bin/
  /usgs/pkgs/local/v007/lib/
  /usgs/pkgs/local/v007/objects/
  /usgs/pkgs/local/v007/include/googleprotobuf/protobuf2.6.1/
  /usgs/pkgs/local/v007/include/xercesc/xercesc3.1.2/
  /usgs/pkgs/local/v007/include/tiff/tiff4.0.5/
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


# Add thirdPartyCppFlags
set(thirdPartyCppFlags ${thirdPartyCppFlags} -DGMM_USES_SUPERLU)
set(thirdPartyCppFlags ${thirdPartyCppFlags} "-DENABLEJP2K=${JP2KFLAG}")

# Flag to fix numeric literals problem with boost on linux
if(NOT APPLE)
  set(thirdPartyCppFlags ${thirdPartyCppFlags} -fext-numeric-literals )
endif()

# Paths to required executables
find_program(XALAN Xalan REQUIRED)
find_program(LATEX latex)
find_program(DOXYGEN NAME doxygen PATH_SUFFIXES doxygen REQUIRED)
find_program(UIC uic REQUIRED)
find_program(MOC moc REQUIRED)
find_program(RCC rcc REQUIRED)
find_program(PROTOC protoc REQUIRED)

include(FindProtobuf)

find_package(Qt5 COMPONENTS
                Core
                Concurrent
                # DBus
                Gui
                Multimedia
                MultimediaWidgets
                Network
                OpenGL # Needed to install mesa-common-dev for this!
                # Positioning
                PrintSupport
                Qml
                Quick
                Script
                ScriptTools
                # Sensors
                Sql
                Svg
                Test
		WebChannel
		#WebKit
		#WebKitWidgets
                Widgets
                Xml
                XmlPatterns REQUIRED)

# Some of these will have non-traditional installs with version numbers in the paths in v007
# For these, we pass in a version number, and use it in the path suffix
# This only applies to v007, and outside of the building, we should only expect standard installs
# The v007-specific installs are listed beside their find_package calls below:
find_package(Boost     1.59.0  REQUIRED) # "boost/boost${Boost_FIND_VERSION}/boost/"
find_package(Bullet    2.86    REQUIRED)
find_package(Cholmod   4.4.5   REQUIRED) # "SuiteSparse/SuiteSparse${Cholmod_FIND_VERSION}/SuiteSparse/"
find_package(CSPICE    65      REQUIRED)
find_package(Eigen             REQUIRED)
find_package(Embree    2.15.0  REQUIRED)
find_package(GeoTIFF   2       REQUIRED)
find_package(GMM       5.0     REQUIRED) # "/gmm/gmm-${GMM_FIND_VERSION}/gmm/"
find_package(GSL       19      REQUIRED)
find_package(HDF5      1.8.15  REQUIRED)
find_package(Jama      125     REQUIRED) # Jama version is 1.2.5, but v007 directory is "jama/jama125/"
find_package(NN                REQUIRED)
find_package(OpenCV    3.1.0   REQUIRED)
find_package(PCL       1.8     REQUIRED) # "pcl-${PCL_FIND_VERSION}"
find_package(Protobuf  2.6.1   REQUIRED) # "google-protobuf/protobuf${Protobuf_FIND_VERSION}/"
find_package(Qwt       6       REQUIRED) # "qwt${Qwt_FIND_VERSION}"
find_package(SuperLU   4.3     REQUIRED) # "superlu/superlu${SuperLU_FIND_VERSION}/superlu/"
find_package(TIFF      4.0.5   REQUIRED) # "tiff/tiff-${TIFF_FIND_VERSION}"
find_package(TNT       126     REQUIRED) # TNT version is 1.2.6, but v007 directory is "tnt/tnt126/"
find_package(XercesC   3.1.2   REQUIRED) # "xercesc/xercesc-${XercesC_FIND_VERSION}/"
find_package(X11       6       REQUIRED)
find_package(nanoflann         REQUIRED)
find_package(PNG               REQUIRED)
find_package(Kakadu)

# v007 might have different versions installed for our mac and linux systems.
# Im this case, we specify the version numbers being searched for in the non-traditional installs.
if(APPLE)
  find_package(Geos    3.5.0   REQUIRED)
  find_package(OpenGL            REQUIRED)
else(APPLE)
  find_package(Geos    3.5.1   REQUIRED)
endif(APPLE)

get_cmake_property(_variableNames VARIABLES) # Get All VARIABLES
foreach (_variableName ${_variableNames})
#message("VAR=${_variableName}")
    if (_variableName MATCHES ".+_INCLUDE_DIR$")
      list(APPEND ALLINCDIRS "${${_variableName}}")
    elseif (_variableName MATCHES ".+_INCLUDE_PATH$")
      list(APPEND ALLINCDIRS "${${_variableName}}")
    endif(_variableName MATCHES ".+_INCLUDE_DIR$")
endforeach()

foreach (_variableName ${_variableNames})
    if (_variableName MATCHES "^CMAKE+")
    elseif (_variableName MATCHES ".+_LIB$")
      list(APPEND ALLLIBS "${${_variableName}}")
    elseif (_variableName MATCHES ".+_LIBRARY$")
      list(APPEND ALLLIBS "${${_variableName}}")
    elseif (_variableName MATCHES ".+_LIBRARIES$")
      list(APPEND ALLLIBS "${${_variableName}}")
    endif(_variableName MATCHES "^CMAKE+")
endforeach()

foreach (_variableName ${_variableNames})
    get_filename_component(LIBDIR "${${_variableName}}" DIRECTORY)
    if (_variableName MATCHES "^CMAKE+")
    elseif (_variableName MATCHES ".+_LIB$")
      list(APPEND ALLLIBDIRS "${LIBDIR}")
    elseif (_variableName MATCHES ".+_LIBRARY$")
      list(APPEND ALLLIBDIRS "${LIBDIR}")
    elseif (_variableName MATCHES ".+_LIBRARIES$")
      list(APPEND ALLLIBDIRS "${LIBDIR}")
    endif(_variableName MATCHES "^CMAKE+")
endforeach()

list(REMOVE_DUPLICATES ALLLIBDIRS)
list(REMOVE_DUPLICATES ALLLIBS)
list(REMOVE_DUPLICATES ALLINCDIRS)
