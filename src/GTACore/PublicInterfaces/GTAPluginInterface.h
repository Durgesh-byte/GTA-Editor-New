#ifndef GTAPLUGININTERFACE_H
#define GTAPLUGININTERFACE_H
#include "GTACore.h"
#include "GTAElement.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QMainWindow>
#pragma warning(pop)

struct GTACore_SHARED_EXPORT PluginOutput
{
    PluginOutput():errors(QStringList()),newFolders(QStringList()),retVal(false),importedFilesCount(-1){}
    QStringList errors;
    QStringList newFolders;
    bool retVal;
    int importedFilesCount;
};

class GTACore_SHARED_EXPORT GTAPluginInterface
{

public:

    GTAPluginInterface();
    virtual ~GTAPluginInterface();
	virtual QString aboutPlugin() = 0;
    virtual PluginOutput importExternalFile(const QString &iPluginType,const QString &iImportPath,const QString &iExportPath,const QString &iErrorLogPath,bool iOverwriteFiles) = 0;
    virtual QStringList getParserList()const = 0;
    virtual bool checkForWidget(const QString &iPluginType,const QString &iImportPath) = 0;

signals:
    virtual void saveImportElement(GTAElement * pElem, const QString &iDataDir,bool iOverwriteFiles = true) = 0;
    virtual void widgetClosed(bool iwidgetClosed) = 0;
};

#define GTAPluginInterface_iid "com.AIE.GTA.GTAPluginInterface/4.0"
Q_DECLARE_INTERFACE(GTAPluginInterface, GTAPluginInterface_iid)
#endif // GTAPLUGININTERFACE_H

