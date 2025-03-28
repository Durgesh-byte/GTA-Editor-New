QT       +=core gui xml
QT       += script
CONFIG   += dll

TARGET = GTADataModel
TEMPLATE = lib

DEFINES += GTADataModel_LIBRARY
PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}

SOURCES +=  $$files(src/*.cpp) \

HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*)\

INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTACommon/PublicInterfaces \
               $$PWD/../GTACommon/PrivateInterfaces \
               $$PWD/../GTACommands/PrivateInterfaces \
               $$PWD/../GTACommands/PublicInterfaces \


LIBS    +=  -L"$$LIBDIR" \
            -lGTACommands\
			-lGTACommon\
