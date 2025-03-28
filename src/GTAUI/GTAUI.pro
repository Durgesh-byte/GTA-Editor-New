QT       += core sql gui xml network

greaterThan(QT_MAJOR_VERSION, 4){
 QT += widgets concurrent
}

CONFIG   += dll c++11
TARGET = GTAUI
TEMPLATE = lib
DEFINES += GTAUI_LIBRARY

PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}
SOURCES += $$files(src/*.cpp) \



HEADERS += $$files(PublicInterfaces/*.h*)\
           $$files(PrivateInterfaces/*.h*) \

    
FORMS	+= $$files(forms/*.ui) \


INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTAControllers/PublicInterfaces \
               $$PWD/../GTADataModel/PublicInterfaces \
               $$PWD/../GTADataModel/PrivateInterfaces \               
               $$PWD/../GTACommon/PublicInterfaces \
               $$PWD/../GTACommands/PublicInterfaces \
               $$PWD/../GTAMath/PublicInterfaces \
               $$PWD/../GTACore/PublicInterfaces \
               $$PWD/../GTAViewModel/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/XmlRpcLib/inc \
               $$PWD/../GTASequencer/PublicInterfaces \
			   

LIBS += -L"$$LIBDIR" \
        -lGTAControllers\
        -lGTADataModel \
        -lGTACommon\
        -lGTACommands\
        -lGTAMath\
        -lGTACore\
        -lGTAViewModel \
		
        
RESOURCES += $$PWD/*.qrc \
    resource.qrc
