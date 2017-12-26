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

HEADERS += $$PWD/../../../../../inc/*.h

INCLUDEPATH += $$PWD/../../../../../inc /../../../../qisis/objs/ControlPointEditWidget /usgs/pkgs/local/v007/include/qwt  /usgs/pkgs/local/v007/include/SuiteSparse/SuiteSparse4.4.5/SuiteSparse/ /usgs/pkgs/local/v007/include/boost/boost1.59.0/ /usgs/pkgs/local/v007/include/naif /usgs/pkgs/local/v007/include/tnt/tnt126 /usgs/pkgs/local/v007/include/geos/geos3.5.1/
DEPENDPATH += $$PWD/../../../../../inc /../../../../qisis/objs/ControlPointEditWidget /usgs/pkgs/local/v007/include/qwt /usgs/pkgs/local/v007/include/SuiteSparse/SuiteSparse4.4.5/SuiteSparse/ /usgs/pkgs/local/v007/include/boost/boost1.59.0/ /usgs/pkgs/local/v007/include/naif /usgs/pkgs/local/v007/include/tnt/tnt126 /usgs/pkgs/local/v007/include/geos/geos3.5.1/

