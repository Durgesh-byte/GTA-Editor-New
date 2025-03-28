#ifndef GTAMATH_H_
#define GTAMATH_H_

#pragma warning(push, 0)
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTAMath_LIBRARY)
#  define GTAMath_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTAMath_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GTADATAMODEL_H_