# Build this project to generate a shared library (*.dll or *.so).

TARGET = QtWebApp
TEMPLATE = lib
QT -= gui
VERSION = 1.7.3

mac {
   QMAKE_MAC_SDK = macosx10.10
   QMAKE_CXXFLAGS += -std=c++11
   CONFIG += c++11
   QMAKE_LFLAGS_SONAME  = -Wl,-install_name,/usr/local/lib/
}

win32 {
   DEFINES += QTWEBAPPLIB_EXPORT
}

# Windows and Unix get the suffix "d" to indicate a debug version of the library.
# Mac OS gets the suffix "_debug".
CONFIG(debug, debug|release) {
    win32:      TARGET = $$join(TARGET,,,d)
    mac:        TARGET = $$join(TARGET,,,_debug)
    unix:!mac:  TARGET = $$join(TARGET,,,d)
}

DISTFILES += doc/* mainpage.dox Doxyfile
OTHER_FILES += ../readme.txt

include(qtservice/qtservice.pri)
include(logging/logging.pri)
include(httpserver/httpserver.pri)
include(templateengine/templateengine.pri)
