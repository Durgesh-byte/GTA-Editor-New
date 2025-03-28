QT       += core sql gui xml network

greaterThan(QT_MAJOR_VERSION, 4){
 QT += widgets concurrent testlib
}
else {
    CONFIG   += qtestlib
}

TARGET = GTAAppUnitTest
TEMPLATE = app
DEFINES += GTAAppUnitTest_LIBRARY

PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}
SOURCES += $$files(src/*.cpp) \


HEADERS += $$files(PublicInterfaces/*.h*)\
           $$files(PrivateInterfaces/*.h*) \

INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTAControllers/PublicInterfaces \
               $$PWD/../GTADataModel/PublicInterfaces \
               $$PWD/../GTADataModel/PrivateInterfaces \               
               $$PWD/../GTACommon/PublicInterfaces \
               $$PWD/../GTACommands/PublicInterfaces \
               $$PWD/../GTAParsers/PublicInterfaces \
               $$PWD/../GTAParsers/PrivateInterfaces \
               $$PWD/../GTAViewModel/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/XmlRpcLib/inc \
               $$PWD/../GTACore/PublicInterfaces \
               $$PWD/../GTASequencer/PublicInterfaces \

LIBS += -L"$$LIBDIR" \
        -lGTAControllers\
        -lGTADataModel \
        -lGTACommon\
        -lGTACommands\
        -lGTAParsers\
        -lGTAViewModel\


