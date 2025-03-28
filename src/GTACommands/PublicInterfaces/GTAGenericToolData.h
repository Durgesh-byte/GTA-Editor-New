#ifndef GTAGENERICTOOLDATA_H
#define GTAGENERICTOOLDATA_H

#include "GTACommands.h"
//#include "GTADataModel.h"
#include "GTAGenericFunction.h"
#include "GTAGenericDB.h"


class GTACommands_SHARED_EXPORT GTAGenericToolData
{
public:
    GTAGenericToolData();

    bool isTimeOut();
    void setTimeOut(bool iTimeOut);

    bool isActionOnFail();
    void setActionOnFail(bool iActionOnfail);

    QString getFunctionType();
    void setFunctionType(const QString iFunctionType);

    void setFunctions(QList<GTAGenericFunction> iFunctionList);
    QList<GTAGenericFunction> getFunctions();

    void setDefinitions(QList<GTAGenericParamData> iDefinitions);
    QList<GTAGenericParamData> getDefinitions();

    void setToolName(const QString &iToolName);
    QString getToolName()const;

    void setHasComment(const bool &iVal);
    bool hasComment()const;

    void setHasDumpList(const bool &iVal);
    bool hasDumpList()const;

    void setToolDisplayName(const QString &iDispName);
    QString getToolDisplayName() const;

    void setUserDB(const QList<GTAGenericDB> &iDB);
    QList<GTAGenericDB> getUserDB();


private:


    QList<GTAGenericFunction> _Functions;
    QList<GTAGenericParamData> _Definitions;
    QList<GTAGenericDB> _UserDB;
    bool _HasTimeOut;
    bool  _HasActionOnFail;
    bool _HasComment;
    bool _HasDumpList;
    QString _FunctionsType;
    QString _ToolName;
    QString _ToolDisplayName;

};

#endif // GTAGENERICTOOLDATA_H
