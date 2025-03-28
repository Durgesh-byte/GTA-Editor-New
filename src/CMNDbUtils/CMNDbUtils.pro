QT       += sql
CONFIG   += dll

TARGET = AINCMNDbUtils
TEMPLATE = lib

DEFINES += AINCMNDbUtils_LIBRARY

PROJECT_DIR = $$PWD
!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}
SOURCES += $$files(src/*.cpp) \

HEADERS += $$files(PublicInterfaces/*.h*)\
           $$files(PrivateInterfaces/*.h*) \
		   

INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
               $$PWD/../AINCMNUtils/PrivateInterfaces \
			   
LIBS    +=  -L"$$LIBDIR" \
            -lAINCMNUtils \
