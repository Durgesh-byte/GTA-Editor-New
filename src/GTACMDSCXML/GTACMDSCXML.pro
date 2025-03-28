QT       +=core gui xml
CONFIG   += dll

TARGET = GTACMDSCXML
TEMPLATE = lib

DEFINES += GTACMDSCXML_LIBRARY
PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}

SOURCES +=  $$files(src/*.cpp) \

    

HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*) \



INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTACommon/PublicInterfaces \
               $$PWD/../GTACommon/PrivateInterfaces \
               $$PWD/../GTACommands/PrivateInterfaces \
               $$PWD/../GTACommands/PublicInterfaces \
               $$PWD/../GTADataModel/PublicInterfaces \
               $$PWD/../GTASCXML/PublicInterfaces \


LIBS    +=  -L"$$LIBDIR" \
            -lGTACommands\
            -lGTACommon\
            -lGTASCXML\
            -lGTADataModel\




