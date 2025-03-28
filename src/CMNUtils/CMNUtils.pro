#-------------------------------------------------
#
# Project created by QtCreator 2013-02-23T15:23:39
#
#-------------------------------------------------

QT       += core gui xml sql
CONFIG   += dll
TARGET = AINCMNUtils
TEMPLATE = lib
DEFINES += AINCMNUtils_LIBRARY

PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}


SOURCES +=  $$files(src/*.cpp) \

HEADERS  += $$files(PrivateInterfaces/*.h*)\

INCLUDEPATH += $$PWD/PrivateInterfaces \

LIBS    +=  -L"$$LIBDIR" \
            

