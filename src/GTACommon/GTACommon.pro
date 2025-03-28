QT       += core xml

greaterThan(QT_MAJOR_VERSION, 4){
 QT += widgets
}

CONFIG   += dll

TARGET = GTACommon
TEMPLATE = lib
DEFINES += GTACommon_LIBRARY
PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}
SOURCES +=  $$files(src/*.cpp) \

HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*)\

INCLUDEPATH +=  $$PWD/PrivateInterfaces \
                $$PWD/PublicInterfaces \ 
                $$PWD/../GTAZip/PublicInterfaces \
                $$PWD/../GTAZip/PrivateInterfaces \

			   
LIBS += -L"$$LIBDIR" \
        -lGTAZip \
