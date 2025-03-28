#ifndef GTACMDSCXMLLOG_H
#define GTACMDSCXMLLOG_H

#include "GTACMDSCXML.h"
#include "GTAScxmlLogMessage.h"

#pragma warning(push, 0)
#include <qdom.h>
#include <QList>
#include <QStringList>
#include <QHash>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
struct LogVariableInfo
{
    QString _VarName;
    QString _VarType;
    QString _expectedValue;

};

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLLog
{
public:
    enum REPORT_TYPE{REP_TITLE,REP_CALL_CMD,REP_ACTION,REP_CHECK,REP_HEADER};
    enum REPORT_SUB_TYPE{REP_HEADER_MAIN,REP_HEADER_CALL,REP_CALL_CMD_OBJ,REP_CALL_CMD_FUN,REP_CALL_CMD_PRO, REP_ACTCHK};

    GTACMDSCXMLLog();

    void setReportType(GTACMDSCXMLLog::REPORT_TYPE iRepType,GTACMDSCXMLLog::REPORT_SUB_TYPE iSubRepType);
    void setCommandName(const QString &iCommandName);
    void setCommandDescription(const QString &iDescription);
    void setCommandStatement(const QString &iStatement);
    void insertRequirements(const QStringList &iRequirements);
    void insertVariables(const LogVariableInfo &iVarInfo);
    void setCommandInstanceId(const QString &iInstanceId);
    void insertExtraInfo(const QString &iAttrName,const QString &iAttrValue);

    QDomElement getLogElem() const;


private:

    REPORT_TYPE _RepType; // global Command Type
    REPORT_SUB_TYPE _SubRepType; //
    QString _Name; // complete GTA command name (action/check + complement)
    QString _EnglishStatement;//getStatement;
    QString _Description; // user comments
    QStringList _Requirements; // to be developed in GTA
    QList<LogVariableInfo> _LogVarList; // log all variables required
    QString _InstanceId;
    QHash<QString, QString> _ExtraInfo;
};

#endif // GTACMDSCXMLLOG_H

