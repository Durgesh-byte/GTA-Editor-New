QT       += core sql gui xml network

greaterThan(QT_MAJOR_VERSION, 4){
 QT += widgets
}

TARGET = GTAApp
TEMPLATE = app
DEFINES += GTAApp_LIBRARY

#VERSION = 4.00.001
#DEFINES += TOOLVER=\\\"$$VERSION\\\"
PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}

SOURCES += $$files(src/*.cpp) \

#RESOURCES += qdarkstyle/style.qrc


INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTAControllers/PublicInterfaces \
               $$PWD/../GTAUI/PublicInterfaces \
               $$PWD/../GTAUI/PrivateInterfaces \
               $$PWD/../GTADataModel/PublicInterfaces \
               $$PWD/../GTADataModel/PrivateInterfaces \
               $$PWD/../GTACommon/PublicInterfaces \
               $$PWD/../GTACommands/PublicInterfaces \
               $$PWD/../GTAUserInterfaces/PrivateInterfaces \
               $$PWD/../GTAViewModel/PublicInterfaces \
               $$PWD/../GTACore/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/XmlRpcLib/inc \
               $$PWD/../GTASequencer/PublicInterfaces \

LIBS += -L"$$LIBDIR" \
        -lGTAControllers\
        -lGTAUI\
		-lGTADataModel \
		-lGTACommon \
		-lGTACommands \
		
