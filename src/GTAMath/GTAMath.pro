QT       +=core xml
CONFIG   += dll

TARGET = GTAMath
TEMPLATE = lib

DEFINES += GTAMath_LIBRARY
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
               


LIBS    +=  -L"$$LIBDIR" \
            -lGTACommon
