QT       +=core xml gui xmlpatterns network

greaterThan(QT_MAJOR_VERSION, 4){
 QT += widgets concurrent
}

CONFIG   += dll

TARGET = GTASequencer
TEMPLATE = lib

DEFINES += GTASequencer_LIBRARY
#PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}

SOURCES +=  $$files(src/*.cpp) \

HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*)


FORMS	+= $$files(forms/*.ui) \


INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/PublicInterfaces \
               $$PWD/../GTAXmlRpcServerClient/XmlRpcLib/inc \
               $$PWD/../GTACommon/PublicInterfaces \

LIBS += -L"$$LIBDIR" \
    -lGTAXmlRpcServerClient\

RESOURCES += $$PWD/*.qrc \
