QT       +=core gui xml
CONFIG   += dll

TARGET = GTACommands
TEMPLATE = lib

DEFINES += GTACommands_LIBRARY
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
               $$PWD/../GTAMath/PublicInterfaces \

LIBS    +=  -L"$$LIBDIR" \
            -lGTACommon\
            -lGTAMath\
