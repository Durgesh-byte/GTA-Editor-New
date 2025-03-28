QT       += core gui xml sql network
greaterThan(QT_MAJOR_VERSION, 4){
 QT += widgets
}
CONFIG   += dll

TARGET = GTAViewModel
TEMPLATE = lib
DEFINES += GTAViewModel_LIBRARY
PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}
SOURCES +=  $$files(src/*.cpp) \

HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*)\

INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTADataModel/PublicInterfaces \
               $$PWD/../GTADataModel/PrivateInterfaces \
               $$PWD/../GTACommands/PublicInterfaces \
               $$PWD/../GTACommon/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/XmlRpcLib/inc \


LIBS += -L"$$LIBDIR" \
        -lGTADataModel \
        -lGTACommands \
        -lGTACommon\
        -lGTAXmlRpcServerClient\
