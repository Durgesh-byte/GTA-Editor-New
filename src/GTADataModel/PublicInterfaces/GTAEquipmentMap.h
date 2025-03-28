#ifndef GTAEQUIPMENTMAP_H
#define GTAEQUIPMENTMAP_H
#include "GTADataModelInterface.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTADataModel_SHARED_EXPORT GTAEquipmentMap
{
public:
    GTAEquipmentMap():_Equipment(QString()),
        _Application(QString()), _FileType(QString()),
        _FileName(QString()), _Media(QString()){}

    inline void setEquipment(const QString &iEquipment){_Equipment = iEquipment;}
    inline QString getEquipment()const{return _Equipment;}


    inline void setApplication(const QString &iApplication){_Application = iApplication;}
    inline QString getApplication()const{return _Application;}

    inline void setFileName(const QString &iFileName){_FileName = iFileName;}
    inline QString getFileName()const{return _FileName;}

    inline void setFileType(const QString &iFileType){_FileType = iFileType;}
    inline QString getFileType()const{return _FileType;}

    inline void setMedia(const QString &iMedia){_Media = iMedia;}
    inline QString getMedia()const{return _Media;}

    inline void setToolName(const QString &iToolName){_ToolName = iToolName;}
    inline QString getToolName()const{return _ToolName;}

    inline void setToolType(const QString &iToolType){_ToolType = iToolType;}
    inline QString getToolType()const{return _ToolType;}

//    inline void setIsUpdated(bool iVal){_isUpdated = iVal;}
//    inline bool IsUpdated()const{return _isUpdated;}

    bool operator==(const GTAEquipmentMap &iRhs);



private:
    QString _Equipment;
    QString _Application;
    QString _FileType;
    QString _FileName;
    QString _Media;
    QString _ToolName;
    QString _ToolType;

//    bool _isUpdated;
};

#endif // GTAEQUIPMENTMAP_H
