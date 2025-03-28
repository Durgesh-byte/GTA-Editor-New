#ifndef AINCMNUTILS_H_
#define AINCMNUTILS_H_

#include <QtCore/qglobal.h>

#if defined(AINCMNUtils_LIBRARY)
#  define AINCMNUtils_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define AINCMNUtils_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // AINCMNUTILS_H_
