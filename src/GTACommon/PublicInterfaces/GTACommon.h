#ifndef GTACOMMON_H_
#define GTACOMMON_H_

#pragma warning(push, 0)
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTACommon_LIBRARY)
#  define GTACommon_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTACommon_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
