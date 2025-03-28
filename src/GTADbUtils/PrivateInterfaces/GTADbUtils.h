#ifndef AINGTADBUTILS_H_
#define AINGTADBUTILS_H_

#include <QtCore/qglobal.h>

#if defined(AINGTADbUtils_LIBRARY)
#  define AINGTADbUtils_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define AINGTADbUtils_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
