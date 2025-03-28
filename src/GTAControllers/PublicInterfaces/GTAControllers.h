#ifndef GTACONTROLLERS_H_
#define GTACONTROLLERS_H_

#pragma warning(push, 0)
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTAControllers_LIBRARY)
#  define GTAControllers_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTAControllers_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
