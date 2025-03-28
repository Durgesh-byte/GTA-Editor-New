#include "GTAScxmlLogMessage.h"

#pragma warning(push, 0)
#include <QStringList>
#pragma warning(pop)

GTAScxmlLogMessage::GTAScxmlLogMessage()
{
    initializeMember();

}
GTAScxmlLogMessage::GTAScxmlLogMessage(const QString &iLogMessage)
{
    initializeMember();
    QStringList itemList = iLogMessage.split(":;:");
    for(int i =0 ; i < itemList.count(); i++)
    {
        QString val = itemList.at(i);
        val.remove("'");
        switch(i)
        {
        case 0: Identifier = val;break;
        case 1: Result = StringToResultType(val);break;
        case 2: ExpectedValue = val;break;
        case 3: CurrentValue = val;break;
        case 4: FunctionStatus = val;break;
        case 5: Status = StringToStatusType(val);break;
        case 6: LOD = StringToLevelOfDetail(val);break;
        }
    }
}

QString GTAScxmlLogMessage::getLogMessage(bool isGenericLog)
{
    ResultType ResultTypeCurrVal = Result;
    StatusType logStatusCurrVal = Status;
    //int level = _LogLevel;
    QString ident = Identifier;
    ident = ident.remove("'");
    QString logMessage = "";
    if(isGenericLog)
    {
        if(isCurrentValParam)
        {
            const QString toReplc = ".value";
            const QString empty = "";
            if(CurrentValue.contains(toReplc))
            {
                CurrentValue.replace(toReplc,empty);
            }
            logMessage = QString("{\"Command Identification text\":\"%1\",\"Status\":\"%2\",\"Expected Value\":\"%3\",\"Current Value\":\"+%4.value+\",\"StatusDescription\":\"%5\",\"Requirement ID\":\"%6\",\"time_stamp\":\"+%7.time_stamp+\"}").arg(
                    ident,ResultTypeToString(ResultTypeCurrVal),ExpectedValue,CurrentValue,StatusTypeToString(logStatusCurrVal),QString(""),CurrentValue);
        }
        else
        {
            logMessage = QString("{\"Command Identification text\":\"%1\",\"Status\":\"%2\",\"Expected Value\":\"%3\",\"Current Value\":\"%4\",\"StatusDescription\":\"%5\",\"Requirement ID\":\"%6\",\"time_stamp\":\"+_CurrentTime+\"}").arg(
                    ident,ResultTypeToString(ResultTypeCurrVal),ExpectedValue,CurrentValue,StatusTypeToString(logStatusCurrVal),QString(""),CurrentValue);
        }
        return logMessage;
    }
    else
    {
        if(CurrentValue.isEmpty() || CurrentValue.contains("'")|| CurrentValue.contains("\"") || CurrentValue.toLower() == "true" || CurrentValue.toLower() == "false")
        {
            logMessage =  QString("'%1:;:%2:;:%3:;:%4:;:%5:;:%6:;:%7'").arg(ident,ResultTypeToString(ResultTypeCurrVal),ExpectedValue,
                                                                            CurrentValue,FunctionStatus,StatusTypeToString(logStatusCurrVal),
                                                                            LevelOfDetailToString(LOD));

        }
        else
        {
            logMessage =  QString("'%1:;:%2:;:%3:;:'+%4+':;:%5:;:%6:;:%7'").arg(ident,ResultTypeToString(ResultTypeCurrVal),ExpectedValue,
                                                                                CurrentValue,FunctionStatus,StatusTypeToString(logStatusCurrVal),
                                                                                LevelOfDetailToString(LOD));
        }
        return logMessage.replace("\"","");
    }


}
void GTAScxmlLogMessage::initializeMember()
{
    Identifier = "";
    Result = kNa;
    ExpectedValue = "";
    CurrentValue = "";
    Status = None;
    FunctionStatus = "";
    _LogLevel = 0;
    LOD = Main;
    TimeStamp = "";
    StatID = "";
}

QString GTAScxmlLogMessage::StatusTypeToString(StatusType iVal) const
{
    QString oVal;
    switch(iVal)
    {
    case Success: oVal= "Success"; break;
    case Fail: oVal= "Fail"; break;
    case TimeOut: oVal= "TimeOut"; break;
    default:
        oVal = "None";break;
    }
    return oVal;
}

GTAScxmlLogMessage::StatusType GTAScxmlLogMessage::StringToStatusType(const QString & iVal) const
{
    StatusType oStatus = None;
    QString value = iVal.toUpper().trimmed();
    if(value == "SUCCESS")
    {
        oStatus = Success;
    }
    else if(value == "FAIL")
    {
        oStatus = Fail;
    }
    else if(value == "TIMEOUT")
    {
        oStatus = TimeOut;
    }
    return oStatus;
}

QString GTAScxmlLogMessage::ResultTypeToString(ResultType iVal) const
{
    QString oVal;
    switch(iVal)
    {
    case OK: oVal= "OK"; break;
    case KO: oVal= "KO"; break;
    default:
        oVal = "NA";break;
    }
    return oVal;
}

GTAScxmlLogMessage::ResultType GTAScxmlLogMessage::StringToResultType(const QString & iVal) const
{
    ResultType oResult = kNa;
    QString value = iVal.toUpper().trimmed();
    if(value == "OK")
    {
        oResult = OK;
    }
    else if(value == "KO")
    {
        oResult = KO;
    }

    return oResult;
}

QString GTAScxmlLogMessage::LevelOfDetailToString(LevelOfDetail iVal) const
{
    QString oVal;
    switch(iVal)
    {
    case Main: oVal= "0"; break;
    case Detail: oVal= "1"; break;
    default:
        oVal = "0";break;
    }
    return oVal;
}
GTAScxmlLogMessage::LevelOfDetail GTAScxmlLogMessage::StringToLevelOfDetail(const QString & iVal) const
{
    LevelOfDetail oLevelOfDetail = Main;
    QString value = iVal.toUpper().trimmed();
    if(value == "0")
    {
        oLevelOfDetail = Main;
    }
    else if(value == "1")
    {
        oLevelOfDetail = Detail;
    }

    return oLevelOfDetail;
}

int GTAScxmlLogMessage::getLOD() const
{
    QString LevelOfDetail = LevelOfDetailToString(LOD);
    return LevelOfDetail.toInt();
}

QString GTAScxmlLogMessage::getIdentifier() const
{
    return Identifier;
}

//void GTAScxmlLogMessage::setLogDepth(const QString & iStatID, const QString & iInvokeID)
//{
//    _LogDepth = QString("%1/%2").arg(iStatID,iInvokeID);
//}

//QString GTAScxmlLogMessage::getLogDepthPath() const
//{
//    return _LogDepth;
//}
//void GTAScxmlLogMessage::getLogDepth(QString & oStatID, QString & oInvokeID)
//{
//    QStringList IdList = _LogDepth.split("/");
//    if(IdList.count() == 2)
//    {
//        oStatID = IdList.at(0);
//        oInvokeID = IdList.at(1);
//    }
//}
