#include "GTASCXMLToolId.h"


GTASCXMLToolId::GTASCXMLToolId()
{
    _ID = QString("");
    _ToolType = QString("");
    _EquipName = QString("");
    _isDefault = false;
    _isActivated = false;
}

bool GTASCXMLToolId::operator==(const GTASCXMLToolId &iRhs)
{
    bool rc = false;
    QString rhsToolId = iRhs.getId();
    QString rhsToolType = iRhs.getToolType();
    bool rhsDefVal = iRhs.isDefault();
    if(_ID == rhsToolId && _ToolType == rhsToolType && _isDefault == rhsDefVal)
    {
        rc = true;
    }
    return rc;
}
