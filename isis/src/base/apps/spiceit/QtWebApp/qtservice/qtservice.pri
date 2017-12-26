INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!win32:QT += network
win32:LIBS += -luser32

HEADERS += $$PWD/qtservice.h $$PWD/qtservice_p.h
unix:HEADERS += $$PWD/qtunixsocket.h $$PWD/qtunixserversocket.h

SOURCES += $$PWD/qtservice.cpp
win32:SOURCES += $$PWD/qtservice_win.cpp
unix:SOURCES += $$PWD/qtservice_unix.cpp $$PWD/qtunixsocket.cpp $$PWD/qtunixserversocket.cpp

