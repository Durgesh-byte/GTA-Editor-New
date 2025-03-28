#ifndef GTASEQUENCER_GLOBAL_H
#define GTASEQUENCER_GLOBAL_H

#pragma warning(push, 0)
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTASequencer_LIBRARY)
#  define GTASequencer_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTASequencer_SHARED_EXPORT Q_DECL_IMPORT
#endif


#endif // GTASEQUENCER_GLOBAL_H


