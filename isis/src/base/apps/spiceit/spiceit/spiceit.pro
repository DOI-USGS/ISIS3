# This project demonstrates how to use QtWebAppLib by linking to the shared library (*.dll or *.so).

TARGET = spiceit
TEMPLATE = app
QT = core network
CONFIG += console depend_includepath

HEADERS += \
           src/requesthandler.h

SOURCES += src/main.cpp \
           src/requesthandler.cpp

OTHER_FILES += etc/*  logs/* ../readme.txt


include(isis1.pri)
LIBS += -L../../../../../lib -lisis3.5.2
LIBS += -L/usgs/pkgs/local/v007/lib -pthread  -lQt5Core -lQt5Concurrent -lQt5XmlPatterns -lQt5Xml -lQt5Network -lQt5Sql -lQt5Gui -lQt5PrintSupport -lQt5Positioning -lQt5Qml -lQt5Quick -lQt5Sensors -lQt5Svg -lQt5Test -lQt5OpenGL -lQt5Widgets -lQt5Multimedia -lQt5MultimediaWidgets -lQt5WebChannel -lQt5WebEngine -lQt5WebEngineWidgets -lQt5DBus -lqwt -lxerces-c -lgeotiff -ltiff -lcspice   -lgeos-3.5.1 -lgeos_c -lgsl -lgslcblas -lX11  -lprotobuf -lboost_date_time -lboost_filesystem -lboost_graph -lboost_math_c99f -lboost_math_c99l -lboost_math_c99 -lboost_math_tr1f -lboost_math_tr1l -lboost_math_tr1 -lboost_prg_exec_monitor -lboost_program_options -lboost_regex -lboost_serialization -lboost_signals -lboost_system -lboost_thread -lboost_unit_test_framework -lboost_wave -lboost_wserialization -lboost_timer -lboost_chrono -lkdu_a79R -lkdu_v79R -lcholmod -lamd -lcamd -lccolamd -lcolamd -llapack -lsuitesparseconfig -lhdf5 -lhdf5_hl -lhdf5_cpp -lhdf5_hl_cpp -lpcl_common -lpcl_octree -lpcl_io -lvtksys-7.1 -lsuperlu_4.3 -l:libblas.so.3 -l:libgfortran.so.3 -lBullet3Collision  -lBullet3Common -lBullet3Dynamics   -lBullet3Geometry  -lBullet3OpenCL_clew -lBulletCollision   -lBulletDynamics   -lBulletInverseDynamics -lBulletSoftBody    -lLinearMath  -lembree


#---------------------------------------------------------------------------------------
# The following lines import the shared QtWebApp library.
# You may need to modify the path names to match your computer.
#---------------------------------------------------------------------------------------

CONFIG += depend_includepath

win32 {
   DEFINES += QTWEBAPPLIB_IMPORT
}

# Directories, where the *.h files are stored
INCLUDEPATH += $$PWD/../QtWebApp

# Directory where the release version of the shared library (*.dll or *.so) is stored, and base name of the file.
CONFIG(release, debug|release) {
    win32:      LIBS += -L$$PWD/../build-QtWebApp-Desktop_Qt_5_7_1_MinGW_32bit-Release/release/  -lQtWebApp1
    mac:        LIBS += -L$$PWD/../build-QtWebApp-Desktop_Qt_5_7_1_clang_64bit-Release/          -lQtWebApp
    unix:!mac:  LIBS += -L$$PWD/../build-QtWebApp-Desktop_Qt_5_7_1_GCC_64bit-Debug/              -lQtWebApp
}

# Directory where the debug version of the shared library (*.dll or *.so) is stored, and base name of the file.
CONFIG(debug, debug|release) {
    win32:      LIBS += -L$$PWD/../build-QtWebApp-Desktop_Qt_5_7_1_MinGW_32bit-Debug/debug/      -lQtWebAppd1
    mac:        LIBS += -L$$PWD/../build-QtWebApp-Desktop_Qt_5_7_1_clang_64bit-Debug/            -lQtWebApp_debug
    unix:!mac:  LIBS += -L$$PWD/../build-QtWebApp-Desktop_Qt_5_7_1_GCC_64bit-Debug/              -lQtWebAppd
}

win32 {
   DEFINES += QTWEBAPPLIB_IMPORT
}

