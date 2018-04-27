INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += network
QT += widgets
QT += concurrent
QT += xml
# Enable very detailed debug messages when compiling the debug version
CONFIG(debug, debug|release) {
    DEFINES += SUPERVERBOSE
}

HEADERS += $$PWD/../../../../inc/Camera.h $$PWD/../../../../inc/CameraFactory.h $$PWD/../../../../inc/Cube.h $$PWD/../../../../inc/FileName.h $$PWD/../../../../inc/IString.h $$PWD/../../../../inc/Kernel.h $$PWD/../../../../inc/KernelDb.h $$PWD/../../../../inc/Longitude.h $$PWD/../../../../inc/Process.h $$PWD/../../../../inc/Pvl.h $$PWD/../../../../inc/PvlToPvlTranslationManager.h $$PWD/../../../../inc/Table.h $$PWD/../../../../inc/TextFile.h



INCLUDEPATH += $$PWD/ /usgs/pkgs/local/v007/include/qt/qt5.7.1/QtCore /usgs/pkgs/local/v007/include/qt/qt5.7.1/Qt /usgs/pkgs/local/v007/include/naif /usgs/pkgs/local/v007/include/qwt /usgs/pkgs/local/v007/include/xercesc/xercesc-3.1.2 /usgs/pkgs/local/v007/include/geotiff /usgs/pkgs/local/v007/include/tiff/tiff-4.0.5 /usgs/pkgs/local/v007/include/naif  /usgs/pkgs/local/v007/include/tnt/tnt126  /usgs/pkgs/local/v007/include/tnt/tnt126/tnt /usgs/pkgs/local/v007/include/jama/jama125 /usgs/pkgs/local/v007/include/geos/geos3.5.1 /usgs/pkgs/local/v007/include  /usgs/pkgs/local/v007/include/gmm/gmm-5.0  /usgs/pkgs/local/v007/include/gmm/gmm-5.0/gmm /usgs/pkgs/local/v007/include/google-protobuf/protobuf2.6.1  /usgs/pkgs/local/v007/include/boost/boost1.59.0 /usgs/pkgs/local/v007/include/kakadu/v7_9_1-01762L /usgs/pkgs/local/v007/include/SuiteSparse/SuiteSparse4.4.5/SuiteSparse /usgs/pkgs/local/v007/include/hdf5 /usgs/pkgs/local/v007/include/eigen /usgs/pkgs/local/v007/include/flann  /usgs/pkgs/local/v007/include/pcl-1.8 /usgs/pkgs/local/v007/include/vtk-7.1 /usgs/pkgs/local/v007/include/superlu/superlu4.3 /usgs/pkgs/local/v007/include /usgs/pkgs/local/v007/include/nn /usgs/pkgs/local/v007/include/bullet /usgs/pkgs/local/v007/include/embree2 /usgs/pkgs/local/v007/include


DEPENDPATH += $$PWD/ /usgs/pkgs/local/v007/include/qt/qt5.7.1/QtCore /usgs/pkgs/local/v007/include/qt/qt5.7.1/Qt /usgs/pkgs/local/v007/include/naif /usgs/pkgs/local/v007/include/qwt /usgs/pkgs/local/v007/include/xercesc/xercesc-3.1.2 /usgs/pkgs/local/v007/include/geotiff /usgs/pkgs/local/v007/include/tiff/tiff-4.0.5 /usgs/pkgs/local/v007/include/naif  /usgs/pkgs/local/v007/include/tnt/tnt126  /usgs/pkgs/local/v007/include/tnt/tnt126/tnt /usgs/pkgs/local/v007/include/jama/jama125 /usgs/pkgs/local/v007/include/geos/geos3.5.1 /usgs/pkgs/local/v007/include  /usgs/pkgs/local/v007/include/gmm/gmm-5.0  /usgs/pkgs/local/v007/include/gmm/gmm-5.0/gmm /usgs/pkgs/local/v007/include/google-protobuf/protobuf2.6.1  /usgs/pkgs/local/v007/include/boost/boost1.59.0 /usgs/pkgs/local/v007/include/kakadu/v7_9_1-01762L /usgs/pkgs/local/v007/include/SuiteSparse/SuiteSparse4.4.5/SuiteSparse /usgs/pkgs/local/v007/include/hdf5 /usgs/pkgs/local/v007/include/eigen /usgs/pkgs/local/v007/include/flann  /usgs/pkgs/local/v007/include/pcl-1.8 /usgs/pkgs/local/v007/include/vtk-7.1 /usgs/pkgs/local/v007/include/superlu/superlu4.3 /usgs/pkgs/local/v007/include /usgs/pkgs/local/v007/include/nn /usgs/pkgs/local/v007/include/bullet /usgs/pkgs/local/v007/include/embree2 /usgs/pkgs/local/v007/include
