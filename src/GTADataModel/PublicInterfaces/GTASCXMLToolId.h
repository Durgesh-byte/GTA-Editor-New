#ifndef GTASCXMLTOOLID_H
#define GTASCXMLTOOLID_H

#include "GTADataModel.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTADataModel_SHARED_EXPORT GTASCXMLToolId
{
public:
    GTASCXMLToolId();

    inline void setId(const QString &iId){_ID = iId;}
    inline QString getId()const{return _ID;}

    inline void setToolType(const QString &iToolType){_ToolType = iToolType;}
    inline QString getToolType()const{return _ToolType;}

    inline void setListEquipName(const QString& iEquipName) { _EquipName = iEquipName; }
    inline QString getListEquipName()const { return _EquipName; }

    inline void setDefaultVal(bool iDefaultVal){_isDefault = iDefaultVal;}
    inline bool isDefault()const{return _isDefault;}

    inline void setActivateStatus(bool iActivateStatus) { _isActivated = iActivateStatus; }
    inline bool isActivated()const { return _isActivated; }

    bool operator==(const GTASCXMLToolId &iRhs);

private:
    QString _ID;
    QString _ToolType;
    QString _EquipName;
    bool _isDefault;
    bool _isActivated;
};

#endif // GTASCXMLTOOLID_H
