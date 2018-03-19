#===============================================================================
# High level script to handle all required 3rd party dependencies
# - All of them are expected to be in the 3rdParty folder, this script does not
#   go looking for them if they are not?
#===============================================================================

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

# Specify top level directories
set(thirdPartyDir "/usgs/pkgs/local/v007")
set(INCLUDE_DIR "${thirdPartyDir}/include")
set(LIB_DIR     "${thirdPartyDir}/lib")
set(PLUGIN_DIR  "${thirdPartyDir}/plugins")
set(BIN_DIR     "${thirdPartyDir}/bin")
set(LIC_DIR     "${thirdPartyDir}/license")

# Set up plugin dir for OSX to correctly find qt5 plugins for installing
if(APPLE)
  set(thirdPartyDir "/opt/usgs/v007")
  set(PLUGIN_DIR "${thirdPartyDir}/ports/libexec/qt5/plugins")
endif(APPLE)


# Add thirdPartyCppFlags
set(thirdPartyCppFlags ${thirdPartyCppFlags} -DGMM_USES_SUPERLU)
set(thirdPartyCppFlags ${thirdPartyCppFlags} "-DENABLEJP2K=${JP2KFLAG}")

# Paths to required executables
find_program(XALAN Xalan REQUIRED)
find_program(LATEX latex)
find_program(DOXYGEN NAME doxygen PATH_SUFFIXES doxygen REQUIRED)
find_program(UIC uic REQUIRED)
find_program(MOC moc REQUIRED)
find_program(RCC rcc REQUIRED)
find_program(PROTOC protoc REQUIRED)

# message(STATUS "${CMAKE_PREFIX_PATH}")

include(FindProtobuf)

if(APPLE)
  find_package(OpenGL            REQUIRED)
  find_package(Qt5 COMPONENTS
                  Core
                  Concurrent
                  DBus
                  Gui
                  Multimedia
                  MultimediaWidgets
                  Network
                  OpenGL # Needed to install mesa-common-dev for this!
                  Positioning
                  PrintSupport
                  Qml
                  Quick
                  Script
                  ScriptTools
                  Sensors
                  Sql
                  Svg
                  Test
                  WebChannel
                  WebEngine
                  WebEngineWidgets
                  Widgets
                  Xml
                  XmlPatterns REQUIRED)
else() # oh god why
  find_path(QT5_CORE_INCLUDE_DIR                 NAMES qchar.h                PATH_SUFFIXES qt/qt5.7.1/QtCore/)
  find_path(QT5_CONCURRENT_INCLUDE_DIR           NAMES qtconcurrentmap.h      PATH_SUFFIXES qt/qt5.7.1/QtConcurrent)
  find_path(QT5_DBUS_INCLUDE_DIR                 NAMES qdbusmacros.h          PATH_SUFFIXES qt/qt5.7.1/QtDBus)
  find_path(QT5_GUI_INCLUDE_DIR                  NAMES qpainter.h             PATH_SUFFIXES qt/qt5.7.1/QtGui)
  find_path(QT5_MULTIMEDIA_INCLUDE_DIR           NAMES qmediacontent.h        PATH_SUFFIXES qt/qt5.7.1/QtMultimedia)
  find_path(QT5_MULTIMEDIAWIDGETS_INCLUDE_DIR    NAMES qvideowidget.h         PATH_SUFFIXES qt/qt5.7.1/QtMultimediaWidgets)
  find_path(QT5_NETWORK_INCLUDE_DIR              NAMES qsslsocket.h           PATH_SUFFIXES qt/qt5.7.1/QtNetwork)
  find_path(QT5_OPENGL_INCLUDE_DIR               NAMES qtopenglglobal.h       PATH_SUFFIXES qt/qt5.7.1/QtOpenGL)
  find_path(QT5_POSITIONING_INCLUDE_DIR          NAMES qgeocoordinate.h       PATH_SUFFIXES qt/qt5.7.1/QtPositioning)
  find_path(QT5_PRINTSUPPORT_INCLUDE_DIR         NAMES qprinter.h             PATH_SUFFIXES qt/qt5.7.1/QtPrintSupport)
  find_path(QT5_QML_INCLUDE_DIR                  NAMES qqmlinfo.h             PATH_SUFFIXES qt/qt5.7.1/QtQml)
  find_path(QT5_QUICK_INCLUDE_DIR                NAMES qquickview.h           PATH_SUFFIXES qt/qt5.7.1/QtQuick)
#  find_path(QT5_SCRIPT_INCLUDE_DIR               NAMES qscriptengine.h        PATH_SUFFIXES qt/qt5.7.1/QtScript)
#find_path(QT5_SCRIPTTOOLS_INCLUDE_DIR          NAMES qtscripttoolsversion.h PATH_SUFFIXES qt/qt5.7.1/QtScriptTools)
  find_path(QT5_SENSORS_INCLUDE_DIR              NAMES qgyroscope.h           PATH_SUFFIXES qt/qt5.7.1/QtSensors)
  find_path(QT5_SQL_INCLUDE_DIR                  NAMES qsql.h                 PATH_SUFFIXES qt/qt5.7.1/QtSql)
  find_path(QT5_SVG_INCLUDE_DIR                  NAMES qsvgwidget.h           PATH_SUFFIXES qt/qt5.7.1/QtSvg)
  find_path(QT5_TEST_INCLUDE_DIR                 NAMES qtest.h                PATH_SUFFIXES qt/qt5.7.1/QtTest)
  find_path(QT5_WEBCHANNEL_INCLUDE_DIR           NAMES qwebchannel.h          PATH_SUFFIXES qt/qt5.7.1/QtWebChannel)
  find_path(QT5_WEBENGINE_INCLUDE_DIR            NAMES qtwebengineglobal.h    PATH_SUFFIXES qt/qt5.7.1/QtWebEngine)
  find_path(QT5_WEBENGINEWIDGETS_INCLUDE_DIR     NAMES qwebenginescript.h     PATH_SUFFIXES qt/qt5.7.1/QtWebEngineWidgets)
  find_path(QT5_WIDGETS_INCLUDE_DIR              NAMES qwidget.h              PATH_SUFFIXES qt/qt5.7.1/QtWidgets)
  find_path(QT5_XML_INCLUDE_DIR                  NAMES qxml.h                 PATH_SUFFIXES qt/qt5.7.1/QtXml)
  find_path(QT5_XMLPATTERNS_INCLUDE_DIR          NAMES qtxmlpatternsglobal.h  PATH_SUFFIXES qt/qt5.7.1/QtXmlPatterns)

  get_filename_component(QT5_ROOT_INCLUDE_DIR "${QT5_CORE_INCLUDE_DIR}" DIRECTORY)

  find_library(QT5_CORE_LIBRARY                  NAMES Qt5Core)
  find_library(QT5_CONCURRENT_LIBRARY            NAMES Qt5Concurrent)
  find_library(QT5_DBUS_LIBRARY                  NAMES Qt5DBus)
  find_library(QT5_GUI_LIBRARY                   NAMES Qt5Gui)
  find_library(QT5_MULTIMEDIA_LIBRARY            NAMES Qt5Multimedia)
  find_library(QT5_MULTIMEDIAWIDGETS_LIBRARY     NAMES Qt5MultimediaWidgets)
  find_library(QT5_NETWORK_LIBRARY               NAMES Qt5Network)
  find_library(QT5_OPENGL_LIBRARY                NAMES Qt5OpenGL)
  find_library(QT5_POSITIONING_LIBRARY           NAMES Qt5Positioning)
  find_library(QT5_PRINTSUPPORT_LIBRARY          NAMES Qt5PrintSupport)
  find_library(QT5_QML_LIBRARY                   NAMES Qt5Qml)
  find_library(QT5_QUICK_LIBRARY                 NAMES Qt5Quick)
  find_library(QT5_SCRIPT_LIBRARY                NAMES Qt5Script)
  find_library(QT5_SCRIPTTOOLS_LIBRARY           NAMES Qt5ScriptTools)
  find_library(QT5_SENSORS_LIBRARY               NAMES Qt5Sensors)
  find_library(QT5_SQL_LIBRARY                   NAMES Qt5Sql)
  find_library(QT5_SVG_LIBRARY                   NAMES Qt5Svg)
  find_library(QT5_TEST_LIBRARY                  NAMES Qt5Test)
  find_library(QT5_WEBCHANNEL_LIBRARY            NAMES Qt5WebChannel)
  find_library(QT5_WEBENGINE_LIBRARY             NAMES Qt5WebEngine)
  find_library(QT5_WEBENGINECORE_LIBRARY         NAMES Qt5WebEngineCore)
  find_library(QT5_WEBENGINEWIDGETS_LIBRARY      NAMES Qt5WebEngineWidgets)
  find_library(QT5_WIDGETS_LIBRARY               NAMES Qt5Widgets)
  find_library(QT5_XML_LIBRARY                   NAMES Qt5Xml)
  find_library(QT5_XMLPATTERNS_LIBRARY           NAMES Qt5XmlPatterns)
endif(APPLE)


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
find_package(OpenGL            REQUIRED)
find_package(PCL       1.8     REQUIRED) # "pcl-${PCL_FIND_VERSION}"
find_package(Protobuf  2.6.1   REQUIRED) # "google-protobuf/protobuf${Protobuf_FIND_VERSION}/"
find_package(Qwt       6       REQUIRED) # "qwt${Qwt_FIND_VERSION}"
find_package(SuperLU   4.3     REQUIRED) # "superlu/superlu${SuperLU_FIND_VERSION}/superlu/"
find_package(TIFF      4.0.5   REQUIRED) # "tiff/tiff-${TIFF_FIND_VERSION}"
find_package(TNT       126     REQUIRED) # TNT version is 1.2.6, but v007 directory is "tnt/tnt126/"
find_package(XercesC   3.1.2   REQUIRED) # "xercesc/xercesc-${XercesC_FIND_VERSION}/"
find_package(X11       6       REQUIRED)
find_package(Kakadu)


# v007 might have different versions installed for our mac and linux systems.
# Im this case, we specify the version numbers being searched for in the non-traditional installs.
if(APPLE)
  find_package(Geos    3.5.0   REQUIRED)
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
list(APPEND ALLINCDIRS "/opt/usgs/v007/ports/include/" "/opt/usgs/v007/3rdParty/include/")

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

# message(STATUS "ALL LIBS DIRS: ${ALLLIBDIRS}")
# message(STATUS "ALL LIBS: ${ALLLIBS}")
# message(STATUS "ALL INCS: ${ALLINCDIRS}")
#---------------------------------------------------------------------------
#  Define the third party distribution libraries (patterns)
#---------------------------------------------------------------------------

# On OSX we need to include a LOT of extra libraries!
set(EXTRA_DYNAMIC_LIBS)
if(APPLE)
  set(extras
    # QT dependencies
    libpcre16*.dylib
    libgthread-*.dylib
    libpcre.*dylib
    libharfbuzz*.dylib
    libgraphite2.*dylib
    libleveldb*.dylib*
    libsnappy.*dylib
    libwebp*.dylib
    libdbus*.dylib
    libiconv*.dylib
    liblzma*.dylib
    libz*.dylib
    libssl*.dylib
    libcrypto*.dylib
    libpng*.dylib
    libjpeg.*dylib
    libmng.*dylib
    liblcms2.*dylib
    libsqlite3.*dylib
    postgresql*/libpq.*dylib
    mysql56/mysql/libmysqlclient*.dylib
    libiodbc*.dylib
    # OpenCV dependancies
    libtbb*.dylib
    libjasper*.dylib
    libImath*.dylib
    libIlmImf*.dylib
    libIex*.dylib
    libHalf*.dylib
    libIlmThread*.dylib
    libswscale*.dylib
    # Secondary requirements to all OpenCV dependancies
    libSDL-1*.dylib
    libnettle*.dylib
    libhogweed*.dylib
    libgmp*.dylib
    libxvidcore*.dylib
    libx264*.dylib
    libvorbisenc*.dylib
    libvorbis*.dylib
    libogg*.dylib
    libtheoraenc*.dylib
    libtheoradec*.dylib
    libspeex*.dylib
    libschroedinger-1*.dylib
    libopus*.dylib
    libopenjpeg*.dylib
    libmp3lame*.dylib
    libmodplug*.dylib
    libfreetype*.dylib
    libbluray*.dylib
    libass*.dylib
    libgnutls*.dylib
    libbz2*.dylib
    libXrandr*.dylib
    libXext*.dylib
    libXrender*.dylib
    libX11*.dylib
    libxcb*.dylib
    libXau*.dylib
    libXdmcp*.dylib
    liborc-0*.dylib
    libxml2*.dylib
    libfribidi*.dylib
    libfontconfig*.dylib
    libexpat*.dylib
    libintl*.dylib
    libglib-*.dylib
    libp11-kit*.dylib
    libffi*.dylib
    # OpenCV3 dependencies
    libavresample*.dylib
    libxcb-shm*.dylib
    libsoxr*.dylib
    libopenjp2*.dylib
    libOpenNI*.dylib
    libswresample*.dylib
    libidn*.dylib
    libtasn1*.dylib
    libusb*.dylib
    # libxerces-c depends on these libraries
    libicui18n*.dylib
    libicuuc*.dylib
    libicudata*.dylib
    # libgeotiff depends on these libraries
    libproj*.dylib)

else() # Linux
  set(extras libtbb.so*)
endif()

set(EXTRALIBDIR ${LIB_DIR})
foreach(lib ${extras})
  set(EXTRA_DYNAMIC_LIBS ${EXTRA_DYNAMIC_LIBS} ${EXTRALIBDIR}/${lib})
endforeach()

# message("EXTRA_DYNAMIC_LIBS = ${EXTRA_DYNAMIC_LIBS}")

#  Libraries
set(THIRDPARTYLIBS)

set(RAW_DYNAMIC_LIBS ${QT_DYNAMIC_LIBS}
                     ${QWT_DYNAMIC_LIBS}
                     ${XERCES_DYNAMIC_LIBS}
                     ${GEOTIFF_DYNAMIC_LIBS}
                     ${HDF5_DYNAMIC_LIBS}
                     ${TIFF_DYNAMIC_LIBS}
                     ${NAIF_DYNAMIC_LIBS}
                     ${GEOS_DYNAMIC_LIBS}
                     ${GSL_DYNAMIC_LIBS}
                     ${SUPERLU_DYNAMIC_LIBS}
                     ${PROTOBUF_DYNAMIC_LIBS}
                     ${KAKADU_DYNAMIC_LIBS} # Empty if not available
                     ${CHOLMOD_DYNAMIC_LIBS}
                     ${OPENCV_DYNAMIC_LIBS}
                     ${EXTRA_DYNAMIC_LIBS})

# message("THIRDPARTYLIBS = ${RAW_DYNAMIC_LIBS}")

# For each item in this list, expand the wildcard to get the actual library list.
foreach(lib ${RAW_DYNAMIC_LIBS})
  string(FIND "${lib}" "*" position)
  if(${position} EQUAL -1)
    # No wildcard, just add it.
    set(THIRDPARTYLIBS ${THIRDPARTYLIBS} ${lib})
  else()
    # Expand wildcard, then add.
    file(GLOB expandedLibs ${lib})
    set(THIRDPARTYLIBS ${THIRDPARTYLIBS} ${expandedLibs})
  endif()
endforeach()

# message("THIRDPARTYLIBS = ${THIRDPARTYLIBS}")

# Plugins
file(GLOB_RECURSE THIRDPARTYPLUGINS "${PLUGIN_DIR}/*${SO}")
file(GLOB THIRDPARTYPLUGINFOLDERS "${PLUGIN_DIR}/*")

# message("third party libs = ${THIRDPARTYLIBS}")
# message("third party plugins = ${THIRDPARTYPLUGINS}")
# message("third party plugins folders = ${THIRDPARTYPLUGINFOLDERS}")
