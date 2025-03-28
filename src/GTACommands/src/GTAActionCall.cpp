#include "GTAActionCall.h"
#include "GTAActionBase.h"
#include "GTAUtil.h"
#include "GTAActionParentTitle.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionCallProcedures.h"

GTAActionCall::GTAActionCall()
{
    _Parameter = "";
    _isParallelInSCXML=false;
}

GTAActionCall::GTAActionCall(const QString &iAction,
                                   const QString & iComplement,
                                   const QString & iElement ): GTAActionBase(iAction,iComplement)
{
    _Parameter = iElement;
    _isParallelInSCXML=false;
}
GTAActionCall::GTAActionCall(const GTAActionCall &iObj):GTAActionBase(iObj)
{
    this->setElement(iObj.getElement());
    _MapOfVarTags=iObj.getTagValue();
    _MapOfTagValues = iObj.getTagParameterValue();
    _isParallelInSCXML=iObj.isParallelInSCXML();
    setInstanceName(iObj.getInstanceName());
    _MonitorName = iObj.getMonitorName();
    _continuousLoopTime = iObj.getContinuousLoopTime();
    _isContinuousLoop = iObj.isContinuousLoop();
    _continuousLoopTimeUnit = iObj.getContinuousLoopTimeUnit();
}

void GTAActionCall::setElement(const QString & iElement)
{
    _Parameter = iElement;
}

QString GTAActionCall::getElement() const
{
    return _Parameter;
}

QString GTAActionCall::getStatement() const
{
    QString complement = getComplement();
    QStringList args;


    if(_MapOfVarTags.keys().isEmpty())
        return QString("%1 %2 - %3").arg(getAction(),complement,_Parameter);
    else
    {
        foreach (QString key, _MapOfVarTags.keys())
        {
            if(!key.isEmpty())
            {
                QString sVal =  _MapOfVarTags.value(key);
                if(!sVal.isEmpty())
                {
                    args.append(key+" : "+sVal);
                }
                else
                {
                    return QString("Error : %1 %2 - %3(%4)").arg(getAction(),complement,_Parameter,args.join(", "));
                }
            }
        }
        
        return QString("%1 %2 - %3(%4)").arg(getAction(),complement,_Parameter,args.join(", "));
    }
}

QList<GTACommandBase*>& GTAActionCall::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionCall::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
    return false;
}
void  GTAActionCall::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionCall::getParent(void) const
{
    return _parent;
}
GTACommandBase * GTAActionCall::getClone() const
{
    return (new GTAActionCall(*this));
}
bool GTAActionCall:: hasReference()
{
    return true;
}
QString GTAActionCall::getLTRAStatement()  const
{
    QStringList args;
    QString complement = getComplement();
    if (_MapOfVarTags.keys().isEmpty())
        return QString("#c#%1#c# - #b#%2#b#").arg("CALL",_Parameter);
    else
    {
        foreach (QString key, _MapOfVarTags.keys())
        {
            if(!key.isEmpty())
            {
                QString sVal =  _MapOfVarTags.value(key);
                if(!sVal.isEmpty())
                {
                    args.append(key+" : "+sVal);
                }
                else
                {
                    return QString("Error : #c#%1#c# %2 - #b#%3#b#(%4)").arg("CALL","",_Parameter,args.join(", "));
                }
            }
        }

        return QString("#c#%1#c# %2 - #b#%3#b#(%4)").arg("CALL","",_Parameter,args.join(", "));
    }
}
bool GTAActionCall::canHaveChildren() const
{
    return false;//true
}
void GTAActionCall::setTagValue(QMap <QString,QString> iTagValueMap) 
{
    _MapOfVarTags = iTagValueMap;
}
void GTAActionCall::setTagParameterValue(QMap <QString, QString> iTagParamValueMap)
{
    _MapOfTagValues = iTagParamValueMap;
}
void GTAActionCall::setTagValue(const QHash <QString,QString>& iTagValueHash) 
{
    QMap <QString,QString> tagValuemap;
    foreach(QString tag, iTagValueHash.keys())
    {
        tagValuemap.insert(tag, iTagValueHash.value(tag));

    }
    _MapOfVarTags = tagValuemap;
}
QMap <QString,QString>  GTAActionCall::getTagValue() const
{
    return _MapOfVarTags;
}
QMap <QString, QString>  GTAActionCall::getTagParameterValue() const
{
    return _MapOfTagValues;
}
QString GTAActionCall::getFunctionWithArgument() const
{
    QStringList args;
    foreach (QString key, _MapOfVarTags.keys())
    {
        if(!key.isEmpty())
        {
            QString sVal =  _MapOfVarTags.value(key);
            if(!sVal.isEmpty())
            {
                args.append(key+" : "+sVal);
            }
        }
    }
    return QString("%1(%2)").arg(_Parameter,args.join(","));
}
void GTAActionCall::updateTagValue()
{
    QMap<QString, QString> map;
    if (!_MapOfTagValues.isEmpty())
    {
        foreach(QString tag, _MapOfVarTags.values())
        {
            QString key = _MapOfVarTags.key(tag);
            if (_MapOfTagValues.contains(tag))
            {
                // Change if value is known
                tag = _MapOfTagValues.value(tag);
            }
            map.insert(key, tag);
        }
        setTagValue(map);
    }
}
QStringList GTAActionCall::getVariableList() const
{
    return QStringList();

}

void GTAActionCall:: ReplaceTag(const QMap<QString,QString>&) 
{
    /*QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _Parameter.replace(tag,iterTag.value());
        
    }*/
}

GTACommandBase* GTAActionCall::  getFirstNonCallChild( GTACommandBase* ipCall)
{

    if(ipCall != NULL)
    {
        QList <GTACommandBase*> lstChildrens = ipCall->getChildren();
        if (!lstChildrens.isEmpty())
        {
            GTACommandBase* pFirstCmd = NULL;
            GTAActionCall* pCall = NULL;
            for(int childIdx=0;childIdx<lstChildrens.count();childIdx++)
            {
                pFirstCmd = lstChildrens.at(childIdx);
                pCall = dynamic_cast<GTAActionCall*>(pFirstCmd);
                if(pCall!= NULL)
                {
                    break;
                }
                if(!pFirstCmd->isTitle())
                {
                    break;
                }
            }
            pCall = dynamic_cast<GTAActionCall*>(pFirstCmd);
            if (pCall!=NULL)
            {
                return getFirstNonCallChild(pFirstCmd);
            }
            else
                return pFirstCmd;

        }

    }

    return NULL;

}

GTACommandBase* GTAActionCall::getFirstNonCallChild()
{
    return getFirstNonCallChild(this);
}

QString GTAActionCall::  getGlobalResultStatus( GTACommandBase* ipCmd)
{

    if(ipCmd != NULL)
    {
        QString result = "NA";
        bool isResultAvail =false;
        QList<GTAScxmlLogMessage> logMsgList = ipCmd->getLogMessageList();
        GTAActionExpandedCall* pCallcast = dynamic_cast<GTAActionExpandedCall*>(ipCmd);
        if(pCallcast!=NULL)
        {
            if (pCallcast->hasBrokenLink())
                return "NA";
        }
        if(!logMsgList.isEmpty())
        {
            GTAScxmlLogMessage msg = logMsgList.first();
            if(!msg.Identifier.isEmpty() && (msg.Result == GTAScxmlLogMessage::kNa))
                return "NA";
        }
        QList <GTACommandBase*> lstChildrens = ipCmd->getChildren();
        if (!lstChildrens.isEmpty())
        {
            GTACommandBase* pFirstCmd = NULL;
            GTAActionCall* pCall = NULL;
            GTAActionParentTitle * pTitle = NULL;
            for(int childIdx=0;childIdx<lstChildrens.count();childIdx++)
            {
                pFirstCmd = lstChildrens.at(childIdx);
                pCall = dynamic_cast<GTAActionCall*>(pFirstCmd);
                if (pCall!=NULL)
                {
                    result = getGlobalResultStatus(pFirstCmd);

                    if(result!="NA")
                        isResultAvail = true;
                }
                else if(pFirstCmd->isTitle())
                {
                    pTitle = dynamic_cast<GTAActionParentTitle*>(pFirstCmd);
                    if (pTitle != NULL)
                    {
                        result = pTitle->getGlobalResultStatus();
                    }
                    if(result != "NA")
                        isResultAvail = true;
                }
                else
                {

                    QList<GTAScxmlLogMessage> msgList =  pFirstCmd->getLogMessageList();
                    //                    if (msgList.isEmpty())
                    //                        result="";
                    for(int msgIdx=0;msgIdx<msgList.count();msgIdx++)
                    {
                        GTAScxmlLogMessage msg = msgList.at(msgIdx);
                        if(!msg.Identifier.isEmpty() && (msg.LOD == GTAScxmlLogMessage::Main))
                        {
                            if(msg.Result == GTAScxmlLogMessage::KO)
                                result = "KO";
                            isResultAvail = true;
                        }
                    }
                }
                if(result == "KO")
                    break;
            }
        }
        if((isResultAvail) && (result == "NA"))
            result="OK";
        return result;
    }

    return QString("KO");

}

QString GTAActionCall::getGlobalResultStatus()
{
    return getGlobalResultStatus(this);
    //    QString result = "NA";
    //    QString data = "";
    //    if((_lstChildren != NULL) && (!_lstChildren->isEmpty()))
    //    {
    //        for(int i=0;i<_lstChildren->count();i++)
    //        {
    //            GTACommandBase *pChildCmd=  _lstChildren->at(i);
    //            if(pChildCmd != NULL)
    //            {
    //                if(pChildCmd->isTitle())
    //                {
    //                    GTAActionParentTitle *pTitle = dynamic_cast<GTAActionParentTitle*>(pChildCmd);
    //                    if(pTitle != NULL)
    //                    {
    //                        data = pTitle->getGlobalResultStatus();
    //                    }
    //                }
    //                else if(pChildCmd->hasReference())
    //                {
    //                    GTAActionExpandedCall *pCallCmd = dynamic_cast<GTAActionExpandedCall*>(pChildCmd);
    //                    if(pCallCmd != NULL)
    //                    {
    //                       data = pCallCmd->getGlobalResultStatus();
    //                    }
    //                }
    //                else
    //                {
    //                    data = pChildCmd->getExecutionResult();
    //                }
    //                if(data == "KO")
    //                    break;
    //                else
    //                {
    //                    if((data == "OK")|| (data=="KO"))
    //                        result = data;
    //                }
    //            }
    //        }
    //    }
    //    if((data != "N/A") && (data != ""))
    //        result = data;
    //    return result;
}
bool GTAActionCall::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{

    bool rc;

    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;
    return rc;
}
double GTAActionCall::getTimeoutInMilliseconds()
{
    QString callTimeout,callTimeoutUint;
    getTimeOut(callTimeout,callTimeoutUint);
    double TimeInMS = ((callTimeoutUint == ACT_TIMEOUT_UNIT_MSEC) ? callTimeout.toDouble():callTimeout.toDouble() * 1000);
    return TimeInMS;

}
QString GTAActionCall::getSCXMLStateName()const
{
    return QString("CALL_%1").arg(getElement());

}
void GTAActionCall::setMonitorName(const QString &iMonitorName){
    _MonitorName = iMonitorName;
}
QString GTAActionCall::getMonitorName() const
{
    return _MonitorName;
}

void GTAActionCall::setContinuousLoopTime(const QString &iContinuousLoopTime){
    _continuousLoopTime = iContinuousLoopTime;
}
QString GTAActionCall::getContinuousLoopTime() const
{
    return _continuousLoopTime;
}
void GTAActionCall::setContinuousLoopTimeUnit(const QString &iContinuousLoopTimeUnit){
    _continuousLoopTimeUnit = iContinuousLoopTimeUnit;
}
QString GTAActionCall::getContinuousLoopTimeUnit() const
{
    return _continuousLoopTimeUnit;
}
void GTAActionCall::setIsContinuousLoop(bool isContinuousLoop){
    _isContinuousLoop = isContinuousLoop;
}
bool GTAActionCall::isContinuousLoop() const
{
    return _isContinuousLoop;
}

//void GTAActionCall::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
//{
//    _Parameter.replace(iStrToFind,iStringToReplace);
//}
//bool GTAActionCall::replacableStrFound(const QRegExp& iRegExp)const
//{
//    bool rc = false;
//    rc = _Parameter.contains(iRegExp);
//    return rc;
//}
