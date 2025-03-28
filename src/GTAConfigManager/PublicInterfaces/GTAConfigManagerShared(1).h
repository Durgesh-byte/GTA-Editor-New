#ifndef GTACONFIGMANAGERSHARED_H
#define GTACONFIGMANAGERSHARED_H

#include <QtCore/qglobal.h>

#if defined(GTAConfigManager_LIBRARY)
#  define GTAConfigManager_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTAConfigManager_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif