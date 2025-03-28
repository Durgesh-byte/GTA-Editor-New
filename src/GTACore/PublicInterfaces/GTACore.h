#ifndef GTACORE_H
#define GTACORE_H

#pragma warning(push, 0)
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTACore_LIBRARY)
#  define GTACore_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTACore_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GTACORE_H


