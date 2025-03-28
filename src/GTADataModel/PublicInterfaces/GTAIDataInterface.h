#ifndef GTADATAINTERFACE_H_
#define GTADATAINTERFACE_H_

#pragma warning(push,0)
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTADataModel_LIBRARY)
#  define GTADataInterface_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTADataInterface_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GTADATAINTERFACE_H_
