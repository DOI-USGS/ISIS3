#==============================================================================
# High level script to handle all required 3rd party dependencies
# - All of them are expected to be in the 3rdParty folder, this script does not
#   go looking for them if they are not?
#===============================================================================

message("USING CONDA PREFIX: $ENV{CONDA_PREFIX}")
list(APPEND CMAKE_FIND_ROOT_PATH $ENV{CONDA_PREFIX} $ENV{CONDA_PREFIX}/lib/cmake/Qt5)

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
                #WebKit
                #WebKitWidgets
                Widgets
                Xml
                XmlPatterns REQUIRED)

# Some of these will have non-traditional installs with version numbers in the paths in v007
# For these, we pass in a version number, and use it in the path suffix
# This only applies to v007, and outside of the building, we should only expect standard installs
# The v007-specific installs are listed beside their find_package calls below:
find_package(Boost     1.59.0  REQUIRED)
find_package(Bullet    2.86    REQUIRED)
find_package(Cholmod   4.4.5   REQUIRED)
find_package(CSPICE    65      REQUIRED)
find_package(Eigen             REQUIRED)
find_package(Embree    2.15.0  REQUIRED)
find_package(GeoTIFF   2       REQUIRED)
find_package(GMM       5.0     REQUIRED)
find_package(GSL       19      REQUIRED)
find_package(HDF5      1.8.15  REQUIRED)
find_package(Jama      125     REQUIRED)
find_package(NN                REQUIRED)
find_package(OpenCV    3.1.0   REQUIRED)
find_package(PCL       1.8     REQUIRED)
find_package(Protobuf  2.6.1   REQUIRED)
find_package(Qwt       6       REQUIRED)
find_package(SuperLU   4.3     REQUIRED)
find_package(TIFF      4.0.5   REQUIRED)
find_package(TNT       126     REQUIRED)
find_package(XercesC   3.1.2   REQUIRED) 
find_package(X11       6       REQUIRED)
find_package(nanoflann         REQUIRED)
find_package(PNG               REQUIRED)
find_package(Kakadu)
find_package(Geos    3.5.0   REQUIRED)


# Im this case, we specify the version numbers being searched for in the non-traditional installs.
if(APPLE)
  find_package(OpenGL            REQUIRED)
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
    endif()
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
