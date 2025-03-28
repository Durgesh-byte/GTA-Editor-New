#include "GTACMDSCXMLLog.h"

#pragma warning(push, 0)
#include <qdom.h>
#pragma warning(pop)

GTACMDSCXMLLog::GTACMDSCXMLLog()
{

}
void GTACMDSCXMLLog::setReportType(GTACMDSCXMLLog::REPORT_TYPE iRepType, GTACMDSCXMLLog::REPORT_SUB_TYPE iSubRepType)
{
    _RepType = iRepType;
    _SubRepType = iSubRepType;
}
void GTACMDSCXMLLog::setCommandName(const QString &iCommandName)
{
    _Name = iCommandName;
}
void GTACMDSCXMLLog::setCommandDescription(const QString &iDescription)
{
    _Description = iDescription;
}
void GTACMDSCXMLLog::setCommandStatement(const QString &iStatement)
{
    _EnglishStatement = iStatement;
}
void GTACMDSCXMLLog::insertRequirements(const QStringList &iRequirements)
{
    _Requirements.append(iRequirements);
}
void GTACMDSCXMLLog::insertVariables(const LogVariableInfo &iVarInfo)
{
    _LogVarList.append(iVarInfo);
}
void GTACMDSCXMLLog::setCommandInstanceId(const QString &iInstanceId)
{
    _InstanceId = iInstanceId;
}
void GTACMDSCXMLLog::insertExtraInfo(const QString &iAttrName,const QString &iAttrValue)
{
    _ExtraInfo.insert(iAttrName,iAttrValue);
}

QDomElement GTACMDSCXMLLog::getLogElem() const
{
    QDomDocument domDoc;
    QDomElement reportElem = domDoc.createElement("report");
    QDomElement nameElem = domDoc.createElement("name");
    QDomElement descriptionElem = domDoc.createElement("description");
    QDomElement variablesElem = domDoc.createElement("variables");
    QDomElement requirementsElem = domDoc.createElement("requirements");
    QDomElement typeElem = domDoc.createElement("type");
    QDomElement subTypeElem = domDoc.createElement("sub_type");
    QDomElement EnglishStmtElem = domDoc.createElement("english_statement");
    QDomElement InstanceIdElem = domDoc.createElement("cmd_instance");


    QDomText nameText = domDoc.createTextNode(_Name);
    QDomText desText = domDoc.createTextNode(_Description);
    QDomText engStmtText = domDoc.createTextNode(_EnglishStatement);
    QDomText InstanceIdText = domDoc.createTextNode(_InstanceId);

    nameElem.appendChild(nameText);
    descriptionElem.appendChild(desText);
    EnglishStmtElem.appendChild(engStmtText);
    InstanceIdElem.appendChild(InstanceIdText);

    for(int i=0;i<_LogVarList.count();i++)
    {
        LogVariableInfo varInfo = _LogVarList.at(i);
        QDomElement varElem = domDoc.createElement("variable");
        varElem.setAttribute("name",varInfo._VarName);
        varElem.setAttribute("expected_value",varInfo._expectedValue);
        varElem.setAttribute("type",varInfo._VarType);
        variablesElem.appendChild(varElem);
    }

    for(int i=0;i<_Requirements.count();i++)
    {
        QString req = _Requirements.at(i);
        QDomElement reqElem = domDoc.createElement("requirement");
        QDomText reqText = domDoc.createTextNode(req);
        reqElem.appendChild(reqText);
        requirementsElem.appendChild(reqElem);
    }

    if(_RepType == GTACMDSCXMLLog::REP_ACTION)
    {
        QDomText typeText = domDoc.createTextNode("REPORT_ACTION");
        typeElem.appendChild(typeText);
        QDomText subTypeText  = domDoc.createTextNode("REP_ACTION");
        subTypeElem.appendChild(subTypeText);
    }
    else if(_RepType == GTACMDSCXMLLog::REP_CALL_CMD)
    {
        QDomText typeText = domDoc.createTextNode("REPORT_CALL_CMD");
        typeElem.appendChild(typeText);
        if(_SubRepType == GTACMDSCXMLLog::REP_CALL_CMD_FUN)
        {
            QDomText subTypeText  = domDoc.createTextNode("REP_CALL_CMD_FUN");
            subTypeElem.appendChild(subTypeText);
        }
        else if(_SubRepType == GTACMDSCXMLLog::REP_CALL_CMD_OBJ)
        {
            QDomText subTypeText  = domDoc.createTextNode("REP_CALL_CMD_OBJ");
            subTypeElem.appendChild(subTypeText);
        }
        else if(_SubRepType == GTACMDSCXMLLog::REP_CALL_CMD_PRO)
        {
            QDomText subTypeText  = domDoc.createTextNode("REP_CALL_CMD_PRO");
            subTypeElem.appendChild(subTypeText);
        }
    }
    else if(_RepType == GTACMDSCXMLLog::REP_CHECK)
    {
        QDomText typeText = domDoc.createTextNode("REP_CHECK");
        typeElem.appendChild(typeText);
        QDomText subTypeText  = domDoc.createTextNode("REP_CHECK");
        subTypeElem.appendChild(subTypeText);
    }
    else if(_RepType == GTACMDSCXMLLog::REP_TITLE)
    {
        QDomText typeText = domDoc.createTextNode("REP_TITLE");
        typeElem.appendChild(typeText);
        QDomText subTypeText  = domDoc.createTextNode("REP_TITLE");
        subTypeElem.appendChild(subTypeText);
    }
    else if(_RepType == GTACMDSCXMLLog::REP_HEADER)
    {
        QDomText typeText = domDoc.createTextNode("REP_HEADER");
        typeElem.appendChild(typeText);

        if(_SubRepType == GTACMDSCXMLLog::REP_HEADER_CALL)
        {
            QDomText subTypeText  = domDoc.createTextNode("REP_HEADER_CALL");
            subTypeElem.appendChild(subTypeText);
        }
        else if(_SubRepType == GTACMDSCXMLLog::REP_HEADER_MAIN)
        {
            QDomText subTypeText  = domDoc.createTextNode("REP_HEADER_MAIN");
            subTypeElem.appendChild(subTypeText);
        }
    }

    QStringList keys = _ExtraInfo.keys();
    QDomElement ExtraInfoNode = domDoc.createElement("extraInfo");
    for(int i=0;i<keys.count();i++)
    {
        QString key = keys.at(i);
        if(!key.trimmed().isEmpty())
        {
            QString value = _ExtraInfo.value(key);
            QDomElement Info = domDoc.createElement("info");
            QDomText subNameText = domDoc.createTextNode(value);
            Info.appendChild(subNameText);
            Info.setAttribute("name",key);
            ExtraInfoNode.appendChild(Info);
        }
    }

    reportElem.appendChild(typeElem);
    reportElem.appendChild(subTypeElem);
    reportElem.appendChild(nameElem);
    reportElem.appendChild(descriptionElem);
    reportElem.appendChild(variablesElem);
    reportElem.appendChild(requirementsElem);
    reportElem.appendChild(EnglishStmtElem);
    reportElem.appendChild(ExtraInfoNode);

    return reportElem;
}
