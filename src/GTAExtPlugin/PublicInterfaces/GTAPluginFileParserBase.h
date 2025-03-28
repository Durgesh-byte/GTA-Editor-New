#ifndef AINGTAPLUGINFILEPARSERBASE_H
#define AINGTAPLUGINFILEPARSERBASE_H

#include <QStringList>
#include "AINGTAExtPlugin.h"
#include <QFileInfo>
#include <AINGTAElement.h>



class AINGTAEXTPLUGIN_SHARED_EXPORT AINGTAPluginFileParserBase : public QObject
{
    Q_OBJECT
public:
    AINGTAPluginFileParserBase(){}
    void setExportPath(const QString &iExportPath){_ExportPath = iExportPath;}
    virtual bool parseAndSaveFile(const QFileInfo &iAVATARFile){return false;}
    virtual QStringList getErrors(){return _ErrorMessages;}
    virtual QString getElementDirPath()const {return _ElementDirPath;}

signals:
    virtual void saveImportElement(AINGTAElement *pElem, const QString &iDataDir);
protected:
        QString _ExportPath;
        QStringList _ErrorMessages;
        QString _ElementDirPath;
};

#endif // AINGTAPLUGINFILEPARSERBASE_H
