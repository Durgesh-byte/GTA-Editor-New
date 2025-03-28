#-------------------------------------------------
#
# Project created by QtCreator 2019-04-04T11:00:33
#
#-------------------------------------------------

QT       += core network xml
CONFIG   += dll

TARGET = GTAXmlRpcServerClient
TEMPLATE = lib
DEFINES += GTAXMLRPCSERVERCLIENT_LIBRARY

PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}

SOURCES += $$files(src/*.cpp*)\

HEADERS += $$files(PublicInterfaces/*.h*)\
           #$$files(XmlRpcLib/inc/*.h*)\

contains(TARGET_ARCH, x86_64) {
    message("GTAXmlRpcServerClient 64-bit")
    64_BIT="_x64"
    LIBS    +=  -L"$$LIBDIR" \
    		"$$PWD/XMLRPCLib/lib/$$QT_MAJOR_VERSION$$QT_MINOR_VERSION$$QT_PATCH_VERSION$$64_BIT/$$PROJECT_BUILD_CONFIG/*.lib" \
} else {
    message("GTAXmlRpcServerClient 32-bit")
	LIBS    +=  -L"$$LIBDIR" \
        		"$$PWD/XMLRPCLib/lib/$$QT_MAJOR_VERSION$$QT_MINOR_VERSION$$QT_PATCH_VERSION/$$PROJECT_BUILD_CONFIG/*.lib" \
}

INCLUDEPATH +=  $$PWD/XmlRpcLib/inc \
                $$PWD/PublicInterfaces \
