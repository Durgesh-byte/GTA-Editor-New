#ifndef GTAZIP_H
#define GTAZIP_H

#pragma warning(push, 0)
// Some include(s) with unfixable warnings

#include "QString"
#include <QtCore/qglobal.h>
#pragma warning(pop)

#if defined(GTAZip_LIBRARY)
#  define GTAZip_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define GTAZip_SHARED_EXPORT Q_DECL_IMPORT
#endif


class GTAZip_SHARED_EXPORT GTAZip
{
    QString zipFile;
    QString inputDir;

public:
    GTAZip();
    static void zipData(const QString &iZipFile, const QString iInputDir);
};

#endif // GTAZIP_H
