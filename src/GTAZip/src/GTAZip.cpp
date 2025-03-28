#include "GTAZip.h"
#include "CLRZIP.h"

GTAZip::GTAZip()
{
}
void GTAZip::zipData(const QString &iZipFile, const QString iInputDir)
{
    ZipData(iZipFile.toLatin1().data(),iInputDir.toLatin1().data());
}
