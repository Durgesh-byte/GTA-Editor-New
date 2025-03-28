QT       +=core xml
CONFIG   += dll

TARGET = GTAZip
TEMPLATE = lib

DEFINES += GTAZip_LIBRARY
PROJECT_DIR = $$PWD

!include ( $$PWD/../GTAApplication.pri ) {
  error ( "GTAApplication.pri not included" )
}

SOURCES +=  $$files(src/*.cpp) \


HEADERS  += $$files(PrivateInterfaces/*.h*)\
            $$files(PublicInterfaces/*.h*)


INCLUDEPATH += $$PWD/PrivateInterfaces \
               $$PWD/PublicInterfaces \
              
greaterThan(QT_MAJOR_VERSION, 4) {
    TARGET_ARCH=$${QT_ARCH}
} else {
    TARGET_ARCH=$${QMAKE_HOST.arch}
}

contains(TARGET_ARCH, x86_64) {
    message("GTAZip 64-bit")
                64_BIT="x64"
                LIBS    +=  -L"$$LIBDIR" \
                            "$$PWD/../../ExternalLib/CLRZIP/$$64_BIT/Release/*.lib" \
} else {
     message("GTAZip 32-bit")
		LIBS    +=  -L"$$LIBDIR" \
                      "$$PWD/../../ExternalLib/CLRZIP/Release/*.lib" \
}			  


 











