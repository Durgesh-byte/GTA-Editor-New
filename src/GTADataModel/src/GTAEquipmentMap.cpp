#include "GTAEquipmentMap.h"


bool GTAEquipmentMap::operator==(const GTAEquipmentMap &iRhs)
{
    bool rc = false;
    QString rhsEquipment = iRhs.getEquipment();
    QString rhsApplication = iRhs.getApplication();
    QString rhsFileName = iRhs.getFileName();
    QString rhsFileType = iRhs.getFileType();
    QString rhsMedia = iRhs.getMedia();
    if(_Equipment == rhsEquipment &&
            _Application == rhsApplication &&
            _FileName == rhsFileName &&
            _FileType == rhsFileType &&
            _Media == rhsMedia)
    {
        rc = true;
    }
    return rc;
}
