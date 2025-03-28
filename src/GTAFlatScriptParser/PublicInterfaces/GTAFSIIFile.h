#ifndef AINGTAFSIIFILE_H
#define AINGTAFSIIFILE_H
#include <QList>
#include <QStringList>
#include <QHash>
#include "AINGTAFSIIFunction.h"
#include "AINGTAFSIITestProcedure.h"
#include "AINGTAFlatScriptParser.h"


class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIFile
{

    QString _Name;
    QString _Description;
    QStringList _ICDDatabase;
    AINGTAFSIITestProcedure _TestProc;
    QList <AINGTAFSIIFunction> _Functions;
    QHash <QString,QString> _FunctionNameMap;
    QString _ExternalXmlFilePath;


public:
    AINGTAFSIIFile();

    void setICDDatabaseList(const QStringList &iICDList);
    QStringList getICDDatabaseList()const;

    void setTestProcedure(const AINGTAFSIITestProcedure &iTestProc);
    AINGTAFSIITestProcedure getTestProcedure();

    void setFunctionList(const QList<AINGTAFSIIFunction> &iFunctionList);
    QList<AINGTAFSIIFunction> getFunctionList();

    void addFunction(const AINGTAFSIIFunction &iFunction);

    void setName(const QString &iName);
    QString getName()const;

    void setDescription(const QString &iDesc);
    QString getDescription()const;

    void insertIntoFuncNameMap(const QString &iLongName, const QString &iShorName);
    QHash<QString,QString> getFuncNameMap()const;
    QString getValueFromFuncNameMap(const QString &iKey)const;
    bool isPresentInFuncNameMap(const QString &iKey);

    void setExternalXmlFilePath(const QString &iPath);
    QString getExternalXmlFilePath()const;

	void clear();
    bool operator == (const AINGTAFSIIFile &iFSIIFile) const;
};

#endif // AINGTAFSIIFILE_H
