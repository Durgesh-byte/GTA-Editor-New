#-------------------------------------------------
#
# Project created by QtCreator 2013-02-23T15:23:39
#
#-------------------------------------------------

QT       += core xml sql

greaterThan(QT_MAJOR_VERSION, 4){
 QT += concurrent
}

CONFIG   += dll
TARGET = GTAParsers
TEMPLATE = lib
DEFINES += GTAParsers_LIBRARY

PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}


SOURCES +=  $$files(src/*.cpp) \
            $$files(src/*.c) \


HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*)\


INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTADataModel/PublicInterfaces \
               $$PWD/../GTADataModel/PrivateInterfaces \
               $$PWD/../GTACommands/PublicInterfaces \
               $$PWD/../GTACommon/PublicInterfaces \

LIBS    +=  -L"$$LIBDIR" \
            -lGTADataModel\
            -lGTACommands\
            -lGTACommon\
            
