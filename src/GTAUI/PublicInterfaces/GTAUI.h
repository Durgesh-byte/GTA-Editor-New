#ifndef GTAUI_H
#define GTAUI_H


#pragma warning (push, 0)
#include <QtCore/qglobal.h>
#pragma warning (pop)


#if defined(GTAUI_LIBRARY)
#  define GTAUI_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTAUI_SHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // GTACORE_H


