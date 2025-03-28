#ifndef GTAGENERICTOOLCONTROLLER_H
#define GTAGENERICTOOLCONTROLLER_H

#include "GTAGenericToolData.h"
#include "GTAControllers.h"
#include "GTASystemServices.h"

#pragma warning(push, 0)
#include <QHash>
#include <QStringList>
#include <QDomDocument>
#pragma warning(pop)

class GTAControllers_SHARED_EXPORT GTAGenericToolController
{
    GTAGenericToolData _ToolConfigData;
    //Tool name, complete path
    QHash<QString,QString> _AllGenTools;           //key value pair of all the names of external tools in the Tool Config folder and their complete path
    GTAGenericToolData _ToolInputData;  //Input file parsed data
    QHash<QString,QString> _UserToolNameHash;
    QString _ToolConfigPathDir;
    QStringList _UserDBNames;

public slots:
    void onGetAllToolNamesSignal();

public:
    GTAGenericToolController();
 //   bool validateConfigXml(const QString &iconfigFileName);
    QHash<QString,QString> getAllToolConfigrations(QStringList &errList);
    QString getPathByToolName(const QString &iToolName);
    QList<GTAGenericDB> getUserDefinedDb();
    QList<GTAGenericFunction> getGenericToolFunctions(const QString &iToolName);
    //   QList<GTAGenericAttribute> getGenericToolFunctionReturn(const QString &iReturnName);
    QList<GTAGenericFunction> getInputFileFunctions(const QString &iInputFileName);
    QList<GTAGenericParamData> getGenericToolDefintions(const QString &iToolName);
    bool hasActionOnfail();
    bool hasTimeOut();
    bool hasDumpList();
    bool hasComment();

private:
    bool ParseToolConfigFile(const QString &iToolName);
    void parseConfigFile(const QString &iToolName, QDomDocument *pDomDoc);
    QList<GTAGenericFunction> parseFunctions(QDomDocument *pDomDoc);
    QList<GTAGenericParamData> parseDefinitions(QDomDocument *pDomDoc);
    bool ParseInputFile(const QString &iInputFile);
    void parseInputFile(const QString &iInputFile, QDomDocument *pDomDoc);
    QList<GTAGenericFunction> matchConfigAndInput();
    QList<GTAGenericFunction> parseInputFileFunctions(QDomDocument *pDomDoc);
    void parseGenericDBFile();

};

#endif // GTAGENERICTOOLCONTROLLER_H
