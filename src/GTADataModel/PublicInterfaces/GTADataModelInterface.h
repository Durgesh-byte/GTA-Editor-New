#ifndef GTADATAMODEL_H_
#define GTADATAMODEL_H_

#pragma warning(push,0)
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTADataModel_LIBRARY)
#  define GTADataModel_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTADataModel_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GTADATAMODEL_H_