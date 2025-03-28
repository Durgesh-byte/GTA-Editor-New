#ifndef GTACOMMANDS_H_
#define GTACOMMANDS_H_

#pragma warning(push, 0)
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTACommands_LIBRARY)
#  define GTACommands_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTACommands_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
