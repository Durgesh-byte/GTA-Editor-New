#ifndef AINGTAFSIICOMMANDBASE_H
#define AINGTAFSIICOMMANDBASE_H
#include "AINGTAFlatScriptParser.h"
#include <QString>

typedef struct AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIICondition {

    QString _LHS;
    QString _RHS;
    QString _Operator;
    bool    _IsSetOnlyFS;
    bool _IsCheckOnlyVal;

    bool getIsSetOnlyFS() const;
    void setIsSetOnlyFS(const bool iIsSetOnlyFS = false);

    bool getIsSetOnlyValue() const;
    void setIsSetOnlyValue(const bool iIsSetOnlyFS = false);


    void setLHS(const QString &iLHS);
    QString getLHS()const;

    void setRHS(const QString &iRHS);
    QString getRHS()const;

    void setOperator(const QString &iOperator);
    QString getOperator()const;


}AINGTAFSIICondition;


class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIICommandBase
{
public:
    AINGTAFSIICommandBase();

    AINGTAFSIICommandBase(const QString &iCommandName);
    virtual ~AINGTAFSIICommandBase();

    virtual void setDeactivate(const QString &iVal);
    virtual bool isCommandDeactivated()const;

    virtual bool isLoggingOn();
    virtual void setLog(const QString &iVal);

    virtual void setStopOnFail(const QString &iVal);
    virtual QString isStopOnFail();

    virtual void setCommandName(const QString &iName);
    virtual QString getCommandName()const;

    virtual void setIsCommandValid(const bool &iVal);
    virtual bool IsCommandValid();

private :

    bool _Deactivate;
    bool _Log;
    QString _StopOnFail;
    QString _CommandName;
    bool _CommandValidity;

};

#endif // AINGTAFSIICOMMANDBASE_H
