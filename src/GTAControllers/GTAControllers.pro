QT       += core xml sql gui xmlpatterns network
greaterThan(QT_MAJOR_VERSION, 4){
 QT += widgets concurrent
}
CONFIG   += dll

TARGET = GTAControllers
TEMPLATE = lib
DEFINES += GTAControllers_LIBRARY
PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}
SOURCES +=  $$files(src/*.cpp) \

    
HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*) \
#            $$PWD/../GTACore/PublicInterfaces/GTAPluginInterface.h\

    
INCLUDEPATH +=  $$PWD/PrivateInterfaces \
                $$PWD/PublicInterfaces \
                $$PWD/../GTADataModel/PublicInterfaces \
                $$PWD/../GTADataModel/PrivateInterfaces \
                $$PWD/../GTAParsers/PublicInterfaces \
                $$PWD/../GTAViewModel/PublicInterfaces \
                $$PWD/../GTACommon/PublicInterfaces \
                $$PWD/../GTACommands/PublicInterfaces \
                $$PWD/../GTACore/PublicInterfaces \
                $$PWD/../GTACommandsSerializer/PublicInterfaces \
                $$PWD/../GTASCXML/PublicInterfaces \
                $$PWD/../GTACMDSCXML/PublicInterfaces \
                $$PWD/../GTASequencer/PublicInterfaces \
                $$PWD/../GTAXmlRpcServerClient/PublicInterfaces \
                $$PWD/../GTAXmlRpcServerClient/XmlRpcLib/inc \

				
LIBS += -L"$$LIBDIR" \
        -lGTACore\
        -lGTAParsers \
        -lGTADataModel \
        -lGTAViewModel \
        -lGTACommon\
        -lGTACommands\
        -lGTACommandsSerializer\
        -lGTASCXML\
        -lGTACMDSCXML\
        -lGTASequencer\
		-lGTAXmlRpcServerClient\
