QT       +=core gui xml
greaterThan(QT_MAJOR_VERSION, 4){
 QT += widgets
}
CONFIG   += dll

TARGET = GTACommandsSerializer
TEMPLATE = lib

DEFINES += GTACommandsSerializer_LIBRARY
PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}

SOURCES +=  $$files(src/*.cpp) \


HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*)\


INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTACommands/PublicInterfaces \
               $$PWD/../GTACommon/PublicInterfaces \
               $$PWD/../GTACore/PublicInterfaces \
               $$PWD/../GTAMath/PublicInterfaces \
               $$PWD/../GTADataModel/PublicInterfaces \
               $$PWD/../GTADataModel/PrivateInterfaces \
			   

LIBS    	+= -L"$$LIBDIR" \
               -lGTACommands\
               -lGTACommon\
			   -lGTACore\
			   -lGTAMath\
			   
