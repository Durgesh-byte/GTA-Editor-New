#include "GTAActionWhile.h"
#include "GTAActionWhileEnd.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

GTAActionWhile::GTAActionWhile()
{
    _ChkAudioAlarm = false;
    _IsFSOnly = false;
    _IsValueOnly = false;
//    _IsRepeatedLogging = false;

    setAction(ACT_CONDITION);
    setComplement(COM_CONDITION_WHILE);
}
GTAActionWhile::GTAActionWhile(const GTAActionWhile& rhs):GTAActionBase(rhs)
{
    _ChkAudioAlarm = false;
    _IsFSOnly = false;
    _IsValueOnly = false;
//    _IsRepeatedLogging = false;
    //from GTACommandBase
    /* this->setCommandType(rhs.getCommandType());*/
    this->setParent(NULL);//This should be set based on paste

    //from GTAActionBase
    /* this->setAction(rhs.getAction());
    this->setComplement(rhs.getComplement());

    QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

    rhs.getPrecision(sPrecision,sPrecisionUnit);
    setPrecision(sPrecision,sPrecisionUnit);

    rhs.getTimeOut( sTimeOut,sTimeOutUnit);
    setTimeOut( sTimeOut,sTimeOutUnit);

    QString sConfTime,sConfTimeUnit;
    rhs.getConfCheckTime(sConfTime,sConfTimeUnit);
    setConfCheckTime(sConfTime,sConfTimeUnit);

    this->setActionOnFail(rhs.getActionOnFail());*/
    this->setHeardAfter(rhs.getHeardAfter());
    
    //from GTAActionWhile
    this->setParameter(rhs.getParameter());
    this->setCondition(rhs.getCondition());
    this->setValue(rhs.getValue());
    this->setComment(rhs.getComment());
    setDumpList(rhs.getDumpList());

    this->setChkAudioAlarm(rhs.getChkAudioAlarm());
    this->setIsFsOnly(rhs.getIsFsOnly());
    this->setIsValueOnly(rhs.getIsValueOnly());
    this->setFunctionalStatus(rhs.getFunctionalStatus());
//    this->setIsRepeatedLogging(rhs.getIsRepeatedLogging());
    
    QList<GTACommandBase*> lstChildrens = rhs.getChildren();
    for (int i=0;i<lstChildrens.size();++i)
    {
        GTACommandBase* pCurrentChild = lstChildrens.at(i);
        GTACommandBase* pClone = NULL;
        if (pCurrentChild!=NULL)
        {
            pClone=pCurrentChild->getClone();
            if (pClone)
                pClone->setParent(this);
        }
        this->insertChildren(pClone,i);
    }

}
GTACommandBase* GTAActionWhile::getClone() const
{
    GTACommandBase* pBase = new GTAActionWhile(*this);
    return pBase;
}

QString GTAActionWhile::getGlobalResultStatus()
{
    QString data = "NA";

    QList<GTACommandBase *> childrens = getChildren();

    for(int i=0;i<childrens.count();i++)
    {
        GTACommandBase *pCmd = childrens.at(i);
        QString data1 = pCmd->getGlobalResultStatus();
        if(!data1.trimmed().isEmpty())
        {
            if(data1 == "KO")
            {
                data = data1;
                break;
            }
            else if(data1 != "NA")
            {
                data = data1;
            }
        }
    }
    return data;
}


GTAActionWhile::~GTAActionWhile()
{
    if (!_lstChildren->isEmpty())
    {
        for(int i=0;i<_lstChildren->size();++i)
        {
            GTACommandBase* pCommand = _lstChildren->at(i);
            if (NULL!=pCommand)
            {
                delete pCommand;
                pCommand=NULL;
            }
        }
        _lstChildren->clear();
    }
}
void GTAActionWhile::setParameter(const QString & iParam)
{
    _Param =iParam;
}

void GTAActionWhile::setValue(const QString & iParam)
{
    _ParamValue = iParam;
}

void GTAActionWhile::setCondition(const QString & iOperator)
{
    _Operator = iOperator;
}
void GTAActionWhile::setChkAudioAlarm(const bool iChkAudioAlarm)
{
    _ChkAudioAlarm = iChkAudioAlarm;
}
bool GTAActionWhile::getChkAudioAlarm()const
{
    return _ChkAudioAlarm;
}


QString GTAActionWhile::getParameter() const
{
    return _Param;
}
QString GTAActionWhile::getValue()const
{
    return _ParamValue;
}
QString GTAActionWhile::getCondition() const
{
    return _Operator;
}

QString GTAActionWhile::getStatement() const
{
    QString oAction;
    QString paramName = getParameter();
    getTrimmedStatement(paramName);
    if(getIsFsOnly())
    {
        oAction = QString("%1 (Function Status of %2 %4 %3)").arg(getComplement(),paramName,getFunctionalStatus(),getCondition());
    }
    else if(getIsSDIOnly())
    {
        oAction = QString("%1 (SDI value of %2 %4 %3)").arg(getComplement(),paramName,getSDI(),getCondition());
    }
    else if(getIsParityOnly())
    {
        oAction = QString("%1 (Parity of %2 %4 %3)").arg(getComplement(),paramName,getParity(),getCondition());
    }
    else if(getIsRefreshRateOnly())
    {
        oAction = QString("%1 (Refresh rate is set for %2)").arg(getComplement(),paramName);
    }
    else
        oAction = QString("%1 %2 %3 %4").arg(getComplement(),paramName,getCondition(),getValue());
    return oAction;
}

QString GTAActionWhile::getLTRAStatement() const
{
    QString oAction;
    oAction = QString("#c#%1#c# %2 %3 %4").arg(getComplement().toUpper(),getParameter(),getCondition(),getValue());
    QString paramter = getParameter();
    QString value = getValue();
    //    if(paramter.contains("."))
    //    {
    //        QStringList items = paramter.split(".");
    //        if(!items.isEmpty())
    //            paramter = items.last();
    //    }
    //    if(value.contains("."))
    //    {
    //        QStringList items = value.split(".");
    //        if(!items.isEmpty())
    //            value = items.last();
    //    }
    return QString("#c#%1#c# #b#%2#b# %3 #b#%4#b#").arg(getComplement().toUpper(),paramter,getCondition(),value);
}
QString GTAActionWhile::getSCXMLStateName()const
{
    QHash<QString,QString> mapEqnStr = GTAUtil::getNomenclatureOfArithSymbols();
    return QString("%1 %2 %3 %4").arg(getComplement(),getParameter(),mapEqnStr[getCondition()],getValue()) ;
}

QList<GTACommandBase*>& GTAActionWhile::getChildren(void) const
{
    return *_lstChildren;
}

bool GTAActionWhile::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    bool rc = false;
    int currSize=_lstChildren->size();
    if (idPos<=currSize && currSize>=0)
    {
        _lstChildren->insert(idPos,pBase);
        if (pBase!=NULL)
        {
            pBase->setParent(this);
            if(this->isIgnoredInSCXML() == true)
            {
                pBase->setIgnoreInSCXML(this->isIgnoredInSCXML());
            }
            if(this->getReadOnlyState() == true)
            {
                pBase->setReadOnlyState(this->getReadOnlyState());
            }
        }
        rc = true;
    }
    return rc;
}

void  GTAActionWhile::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionWhile::getParent(void) const
{
    return _parent;
}

bool GTAActionWhile:: hasChildren()
{
    return !(_lstChildren->isEmpty());
}
bool GTAActionWhile::canHaveChildren() const
{
    return true;
}
//QStringList GTAActionWhile::getVariableList() const
//{
//    QStringList lstOfVars;
//    //GTACommandBase::collectVariableNames(_Param,lstOfVars);
//     if (GTACommandBase::isVariable(_Param) && (_ChkAudioAlarm != true))
//         lstOfVars.append(_Param);
//     if (GTACommandBase::isVariable(_ParamValue) && (_ChkAudioAlarm != true))
//        lstOfVars.append(_ParamValue);
//
////    for(int i=0;i<_lstChildren->size();++i)
////    {
////        GTACommandBase* pCommand = _lstChildren->at(i);
////        if (NULL!=pCommand)
////        {
////            QStringList lsofVarsForChild = pCommand->getVariableList();
////            if (!lsofVarsForChild.isEmpty())
////                lstOfVars.append(lsofVarsForChild);
////        }
////    }
//    return lstOfVars;
//
//}


QStringList GTAActionWhile::resolveEngineParam()const
{
    QStringList lstOfVars;
    QString paramName = getParameter();
    QString paramVal = getValue();

    QString param1ChA = paramName;
    QString param1ChB = paramName;
    QString param2ChA = paramVal;
    QString param2ChB = paramVal;

    QString cicAIdentifier = QString("A:CIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cicBIdentifier = QString("B:CIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cnicAIdentifier = QString("A:CNIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cnicBIdentifier = QString("B:CNIC%1").arg(GTA_CIC_IDENTIFIER);

    param1ChA.replace(cicAIdentifier,"A");
    param1ChB.replace(cicAIdentifier,"B");
    param1ChA.replace(cnicAIdentifier,"A");
    param1ChB.replace(cnicAIdentifier,"B");


    param1ChA.replace(cicBIdentifier,"A");
    param1ChB.replace(cicBIdentifier,"B");
    param1ChA.replace(cnicBIdentifier,"A");
    param1ChB.replace(cnicBIdentifier,"B");

    param1ChB.replace(GTA_CIC_IDENTIFIER,"");
    param1ChA.replace(GTA_CIC_IDENTIFIER,"");
    param1ChB.replace(GTA_CIC_IDENTIFIER,"");
    param1ChA.replace(GTA_CIC_IDENTIFIER,"");

    param2ChA.replace(cicAIdentifier,"A");
    param2ChB.replace(cicAIdentifier,"B");
    param2ChA.replace(cnicAIdentifier,"A");
    param2ChB.replace(cnicAIdentifier,"B");

    param2ChA.replace(cicBIdentifier,"A");
    param2ChB.replace(cicBIdentifier,"B");
    param2ChA.replace(cnicBIdentifier,"A");
    param2ChB.replace(cnicBIdentifier,"B");


    param2ChA.replace(GTA_CIC_IDENTIFIER,"");
    param2ChB.replace(GTA_CIC_IDENTIFIER,"");
    param2ChA.replace(GTA_CIC_IDENTIFIER,"");
    param2ChB.replace(GTA_CIC_IDENTIFIER,"");


    if (! param1ChA.isEmpty() && GTACommandBase::isVariable(param1ChA) && (_ChkAudioAlarm != true))
        lstOfVars.append(param1ChA);


    if (! param1ChB.isEmpty() && GTACommandBase::isVariable(param1ChB) && (_ChkAudioAlarm != true))
        lstOfVars.append(param1ChB);

    if (! param2ChA.isEmpty() && GTACommandBase::isVariable(param2ChA) && (_ChkAudioAlarm != true))
        lstOfVars.append(param2ChA);

    if (! param2ChB.isEmpty() && GTACommandBase::isVariable(param2ChB) && (_ChkAudioAlarm != true))
        lstOfVars.append(param2ChB);

    return lstOfVars;
}

/*New implementaition 09/03/2018*/
QStringList GTAActionWhile::resolveEngineParam(const QString &iParam)const
{
    QStringList lstOfVars;
    QStringList resolved = GTAUtil::getProcessedParameterForChannel(iParam,true,true);
    resolved.removeDuplicates();
    if(resolved.count() > 0)
    {
        for(int i = 0; i < resolved.count();i++)
        {
            QString resolvedParam = resolved.at(i);
            if (GTACommandBase::isVariable(resolvedParam))
            {
                lstOfVars.append(resolvedParam);
            }
        }
    }
    return lstOfVars;

}

QStringList GTAActionWhile::getVariableList() const
{

    QStringList lstOfVars;
    QString paramName = getParameter();                // Following code has been refactored. implemenation according to new logic 09/03/2018
    QString paramVal = getValue();

    QStringList resolved = resolveEngineParam(paramName);
    if(!resolved.isEmpty() && (_ChkAudioAlarm != true))
        lstOfVars.append(resolved);

    resolved.clear();
    resolved = resolveEngineParam(paramVal);
    if(!resolved.isEmpty() && (_ChkAudioAlarm != true))
        lstOfVars.append(resolved);

    return lstOfVars;
}

bool GTAActionWhile::getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QString &iEngine, bool isGenericSCXML, const QStringList iLocalVarList) const
{

    bool rc = true;
    QString lhs = _Param;
    QString rhs = _ParamValue;

    //GTAUtil::updateWithGenToolParam(lhs,iGenToolReturnMap);
    //GTAUtil::updateWithGenToolParam(rhs,iGenToolReturnMap);

    if(getChkAudioAlarm())
    {
        oCondtionStatement =  QString();
        return false;
    }


    //check for channel info...
    QStringList resolvedLhsParams;

    bool hasLhsChannelInfo = GTAUtil::resolveChannelParam(lhs,resolvedLhsParams);
    QStringList resolvedRhsParams;
    bool hasRhsChannelInfo = GTAUtil::resolveChannelParam(rhs,resolvedRhsParams);


    QString cicAIdentifier = QString("A:CIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cicBIdentifier = QString("B:CIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cnicAIdentifier = QString("A:CNIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cnicBIdentifier = QString("B:CNIC%1").arg(GTA_CIC_IDENTIFIER);


    if(hasLhsChannelInfo || hasRhsChannelInfo)
    {

        QHash<QString,QString> engineVarMAp;
        QStringList engineChannelSignals = GTAUtil::getChannelSelectionSignals(iEngine,engineVarMAp);
        QString channelSignal;
        QString channelSignalCondStatement_A;
        QString channelSignalCondStatement_B;
		//do not generate the condition statement if engine channel is not specified
		if (!engineChannelSignals.isEmpty() && engineChannelSignals.count()==2)
		{
			if(resolvedLhsParams.at(0).contains("EEC1_A") || resolvedLhsParams.at(0).contains("EEC1_B") || resolvedRhsParams.at(0).contains("EEC1_A") || resolvedRhsParams.at(0).contains("EEC1_B"))
			{
				channelSignal = engineChannelSignals.at(0);

			}
			else if(resolvedLhsParams.at(0).contains("EEC2_A") || resolvedLhsParams.at(0).contains("EEC2_B") || resolvedRhsParams.at(0).contains("EEC2_A") || resolvedRhsParams.at(0).contains("EEC2_B"))
			{
				channelSignal = engineChannelSignals.at(1);
			}
		}

        if(lhs.contains(cicAIdentifier) || rhs.contains(cicBIdentifier))
        {
            channelSignalCondStatement_A = QString("(%1.Value == 1)").arg(channelSignal);
            channelSignalCondStatement_B = QString("(%1.Value == 0)").arg(channelSignal);
        }
        else if(lhs.contains(cnicAIdentifier) || rhs.contains(cnicBIdentifier))
        {
            channelSignalCondStatement_A = QString("(%1.Value != 1)").arg(channelSignal);
            channelSignalCondStatement_B = QString("(%1.Value != 0)").arg(channelSignal);
        }

        int lhsCnt = resolvedLhsParams.count();
        int rhsCnt = resolvedRhsParams.count();

        if((lhsCnt == 2) && (rhsCnt == 2))
        {

            QString param_1List = getSCXMLCondition(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,isGenericSCXML,iLocalVarList);
            QString param_2List = getSCXMLCondition(resolvedLhsParams.at(1),resolvedRhsParams.at(1),iIcdParamList,isGenericSCXML,iLocalVarList);
            QString paramCond = QString("(%1 && %2)||(%3 && %4)").arg(channelSignalCondStatement_A, param_1List,channelSignalCondStatement_B, param_2List);
            oCondtionStatement = "(" + paramCond + ")";

        }
		else if ((lhsCnt == 2) && (rhsCnt == 1))
		{
			QString param_1List = getSCXMLCondition(resolvedLhsParams.at(0), resolvedRhsParams.at(0), iIcdParamList, isGenericSCXML, iLocalVarList);
			QString param_2List = getSCXMLCondition(resolvedLhsParams.at(1), resolvedRhsParams.at(0), iIcdParamList, isGenericSCXML, iLocalVarList);
			QString paramCond = QString("(%1 && %2)||(%3 && %4)").arg(channelSignalCondStatement_A, param_1List, channelSignalCondStatement_B, param_2List);
			oCondtionStatement = "(" + paramCond + ")";
		}
		else if ((lhsCnt == 1) && (rhsCnt == 1))
		{
			QString param_1List = getSCXMLCondition(resolvedLhsParams.at(0), resolvedRhsParams.at(0), iIcdParamList, isGenericSCXML, iLocalVarList);
			QString paramCond = QString("(%1 && %2)").arg(channelSignalCondStatement_A, param_1List);
			oCondtionStatement = "(" + paramCond + ")";
		}
		else if ((lhsCnt == 1) && (rhsCnt == 2))
		{
			QString param_1List = getSCXMLCondition(resolvedLhsParams.at(0), resolvedRhsParams.at(0), iIcdParamList, isGenericSCXML, iLocalVarList);
			QString param_2List = getSCXMLCondition(resolvedLhsParams.at(0), resolvedRhsParams.at(1), iIcdParamList, isGenericSCXML, iLocalVarList);
			QString paramCond = QString("(%1 && %2)||(%3 && %4)").arg(channelSignalCondStatement_A, param_1List, channelSignalCondStatement_B, param_2List);
			oCondtionStatement = "(" + paramCond + ")";
		}

    }
    else
    {
        oCondtionStatement = getSCXMLCondition(lhs,rhs,iIcdParamList,isGenericSCXML,iLocalVarList);
    }


    return rc;
}


QString GTAActionWhile::getSCXMLCondition(const QString &iLhs, const QString &iRhs, const QStringList &iIcdParamList, bool isGenericSCXML, const QStringList iLocalVarList) const
{
    QString lhs = iLhs;
    QString rhs = iRhs;
    QString CondtionStatement;
    bool isLhsParam = iIcdParamList.contains(lhs);
    bool isRhsParam = iIcdParamList.contains(rhs);
    bool isLhsLocal = iLocalVarList.contains(lhs);
    bool isRhsLocal = iLocalVarList.contains(rhs);
    QString lshVal = lhs;
    QString rhsVal = rhs;
	QString lhsStatusCond,rhsStatusCond;
	QString fs = getFunctionalStatus();
    bool checkVal = getIsValueOnly();

	rhsVal = resolveInternalParams(iRhs,iLocalVarList);
	lshVal = resolveInternalParams(iLhs,iLocalVarList);

    if(isLhsParam)
    {
        lshVal = lshVal+".Value";
        if(!isGenericSCXML)
        {
            //lhsStatusCond = QString("%1.Status == 'NO'").arg(lhs);
            if(fs != ACT_FSSSM_NOT_LOST && !fs.isEmpty())
                lhsStatusCond = QString("%1.Status == '%2'").arg(lshVal,fs);
            else
                lhsStatusCond = QString("%1.Status != '%2'").arg(lshVal,ACT_FSSSM_LOST);
        }
        else
        {
            //lhsStatusCond = QString("FS_%1.Value == FS_%1.validityVal.FS_NO").arg(lhs);
            if(fs != ACT_FSSSM_NOT_LOST && !fs.isEmpty())
                lhsStatusCond = QString("FS_%1.Value == FS_%1.validityVal.FS_%2").arg(lshVal,fs);
            else
                lhsStatusCond = QString("FS_%1.Value != FS_%1.validityVal.FS_NO").arg(lshVal);
        }
    }
    if(isRhsParam)
    {
        rhsVal = rhsVal+".Value";
    }
    if(isLhsLocal && isGenericSCXML)
    {
        lshVal = lshVal+".Value";
    }
    if(isRhsLocal && isGenericSCXML)
    {
        rhsVal = rhsVal+".Value";
    }

    QString CondStatement = "";

    QString op = getCondition();
    QString precision, precisionUnit;

    getPrecision(precision,precisionUnit);
    
    QString ValCond;

    double dPrec = 0.0;
    if(isGenericSCXML)
    {
        bool isValidNum = false;
        dPrec = precision.toDouble(&isValidNum);
        if(precisionUnit == ACT_PRECISION_UNIT_PERCENT)
        {
            if((isValidNum) && dPrec<=0)
            {
                dPrec = 0.0;
            }
            else
            {
                dPrec = dPrec/100;
            }
        }
    }


    if(lhs.contains("\"") || (rhs.contains("\"")))
    {
        if(lshVal.contains("\""))
            lshVal = lshVal.replace("\"","'");
        if(rhsVal.contains("\""))
            rhsVal = rhsVal.replace("\"","'");
        if(op == ARITH_EQ)
        {
            ValCond = QString("(%1 == %2)").arg(lshVal,rhsVal);
        }
        else if(op == ARITH_NOTEQ)
        {
            ValCond = QString("(%1 != %2)").arg(lshVal,rhsVal);
        }
        else
        {
            ValCond = QString("(%1 %3 %2)").arg(lshVal,rhsVal,op);
        }

        CondStatement = QString("(%1)").arg(ValCond);
        if(!lhsStatusCond.trimmed().isEmpty() && !checkVal)
            CondStatement.append(QString (" && (%1) == true").arg(lhsStatusCond));

        if(_IsFSOnly)
            CondStatement = lhsStatusCond;
        else if(_IsOnlySDI && isLhsParam)
        {
            bool ok;
            int sdiValue = _SDI.toInt(&ok,2);
            if(!ok)
                sdiValue = 0;
            CondStatement = QString("%1.Sdi == %2").arg(lhs,QString::number(sdiValue));
        }
        else if(_IsOnlyParity && isLhsParam)
        {
            bool ok;
            int parityValue = _Parity.toInt(&ok,2);
            if(!ok)
                parityValue = 0;
            CondStatement = QString("%1.Parity == %2").arg(lhs,QString::number(parityValue));
        }
        else if(_IsOnlyRefreshRate && isLhsParam)
        {
            // fill refresh rate here later
            CondStatement = precisionCondition(QString("%1.RefreshRate").arg(lhs),"250","=",precision,precisionUnit,dPrec,isGenericSCXML);
        }
        CondtionStatement = "(" + CondStatement +")";
    }
    else
    {

        ValCond = precisionCondition(lshVal,rhsVal,op,precision,precisionUnit,dPrec,isGenericSCXML);
        CondStatement = QString("(%1)").arg(ValCond);
        if(!lhsStatusCond.trimmed().isEmpty() && !checkVal)
            CondStatement.append(QString (" && (%1) == true").arg(lhsStatusCond));

        if(_IsFSOnly)
        {
            CondStatement = lhsStatusCond;
        }
        else if(_IsOnlySDI && isLhsParam)
        {
            bool ok;
            int sdiValue = _SDI.toInt(&ok,2);
            if(!ok)
                sdiValue = 0;
            CondStatement = QString("%1.Sdi == %2").arg(lhs,QString::number(sdiValue));
        }
        else if(_IsOnlyParity && isLhsParam)
        {
            bool ok;
            int parityValue = _Parity.toInt(&ok,2);
            if(!ok)
                parityValue = 0;
            CondStatement = QString("%1.Parity == %2").arg(lhs,QString::number(parityValue));
        }
        else if(_IsOnlyRefreshRate && isLhsParam)
        {
            // fill refresh rate here later
            CondStatement = precisionCondition(QString("%1.RefreshRate").arg(lhs),"250","=",precision,precisionUnit,dPrec,isGenericSCXML);
        }

        CondtionStatement = "(" + CondStatement +")";
    }
    return CondtionStatement;
}

QString GTAActionWhile::precisionCondition(const QString &iLhsVal, const QString &iRhsVal,const QString &op,const QString &iPrecision,const QString &iPrecisionUnit,double dPrec, bool isGenericSCXML)const
{
    QString ValCond;
    if(!iPrecision.trimmed().isEmpty() && (iPrecision != "0"))
    {
        if(op == ARITH_EQ)
        {
            if(iPrecisionUnit == ACT_PRECISION_UNIT_VALUE)
            {
                if(!isGenericSCXML)
                    ValCond = QString("almostEqualAbsolute(%1,%2,%3)").arg(iLhsVal,iRhsVal,iPrecision);
                else
                {
                    if(dPrec > 0)
                        ValCond = QString("((%1+%2)<=%3) && ((%1-%2)>=%3)").arg(iLhsVal,QString::number(dPrec),iRhsVal);
                    else
                        ValCond = QString("%1 == %3").arg(iLhsVal,QString::number(dPrec),iRhsVal);
                }
            }
            else if(iPrecisionUnit == ACT_PRECISION_UNIT_PERCENT)
            {
                if(!isGenericSCXML)
                {
                    bool isDoubleOk = false;
                    double dPrecisionVal = iPrecision.toDouble(&isDoubleOk);
                    if(isDoubleOk)
                        dPrecisionVal = dPrecisionVal*0.01;
                    else
                        dPrecisionVal  = 0.0;

                    ValCond = QString("almostEqualPercent(%1,%2,%3)").arg(iLhsVal,iRhsVal,QString::number(dPrecisionVal));
                }
                else
                {
                    if(dPrec > 0)
                        ValCond = QString("((%1+(%1*%2))<=%3) && ((%1-(%1*%2))>=%3)").arg(iLhsVal,QString::number(dPrec),iRhsVal);
                    else
                        ValCond = QString("%1 == %3").arg(iLhsVal,QString::number(dPrec),iRhsVal);
                }
            }
        }
        else if(op == ARITH_NOTEQ)
        {
            if(iPrecisionUnit == ACT_PRECISION_UNIT_VALUE)
            {
                if(!isGenericSCXML)
                    ValCond = QString("notAlmostEqualAbsolute(%1,%2,%3)").arg(iLhsVal,iRhsVal,iPrecision);
                else
                {
                    if(dPrec >0)
                        ValCond = QString("((%1+%2)>%3) || ((%1-%2)<%3)").arg(iLhsVal,QString::number(dPrec),iRhsVal);
                    else
                        ValCond = QString("%1 != %3").arg(iLhsVal,QString::number(dPrec),iRhsVal);
                }
            }
            else if(iPrecisionUnit == ACT_PRECISION_UNIT_PERCENT)
            {
                if(!isGenericSCXML)
                {
                    bool isDoubleOk = false;
                    double dPrecisionVal = iPrecision.toDouble(&isDoubleOk);
                    if(isDoubleOk)
                        dPrecisionVal = dPrecisionVal*0.01;
                    else
                        dPrecisionVal  = 0.0;

                    ValCond = QString("notAlmostEqualPercent(%1,%2,%3)").arg(iLhsVal,iRhsVal,QString::number(dPrecisionVal));
                }
                else
                {
                    if(dPrec >0)
                        ValCond = QString("((%1+(%1*%2))>%3) || ((%1-(%1*%2))<%3)").arg(iLhsVal,QString::number(dPrec),iRhsVal);
                    else
                        ValCond = QString("%1 != %3").arg(iLhsVal,QString::number(dPrec),iRhsVal);
                }
            }
        }
        else
        {
            if(op == ARITH_EQ)
            {
                ValCond = QString("(%1 == %2)").arg(iLhsVal,iRhsVal);
            }
            if(op == ARITH_NOTEQ)
            {
                ValCond = QString("(%1 != %2)").arg(iLhsVal,iRhsVal);
            }
            else
            {
                ValCond = QString("(%1 %3 %2)").arg(iLhsVal,iRhsVal,op);
            }

        }
    }
    else
    {
        if(op == ARITH_EQ)
        {
            ValCond = QString("(%1 == %2)").arg(iLhsVal,iRhsVal);
        }
        else if(op == ARITH_NOTEQ)
        {
            ValCond = QString("(%1 != %2)").arg(iLhsVal,iRhsVal);
        }
        else
        {
            ValCond = QString("(%1 %3 %2)").arg(iLhsVal,iRhsVal,op);
        }
    }

    return ValCond;
}
void GTAActionWhile::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _Param.replace(tag,iterTag.value());
        _ParamValue.replace(tag,iterTag.value());
        
    }

    //    for(int i=0;i<_lstChildren->size();++i)
    //    {
    //        GTACommandBase* pCommand = _lstChildren->at(i);
    //        if (NULL!=pCommand)
    //            pCommand->ReplaceTag(iTagValueMap);
    //    }
}
void GTAActionWhile::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    _Param.replace(iStrToFind,iStringToReplace);
    _ParamValue.replace(iStrToFind,iStringToReplace);

}
bool GTAActionWhile::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QString temp = _Param;
    bool rc = false;
    if (temp.count('.') == 2) // triplet detection
    {
        temp.chop(temp.size() - temp.indexOf('.'));
        if (temp.contains(iStrToFind))
        {
            rc = true;
            temp.replace(iStrToFind, iStringToReplace);
            _Param.remove(0, _Param.indexOf('.'));
            _Param.prepend(temp);
        }
    }
    return rc;
}
bool GTAActionWhile::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;

    if (_Param.contains(iRegExp))
        rc = true;
    else if(_ParamValue.contains(iRegExp))
        rc =  true;
    else if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;

    return rc;
}
bool GTAActionWhile::expandReferences()
{
    int currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-2;i>=0;i--)
    {
        GTACommandBase* pCurrentChild = _lstChildren->at(i);
        if (pCurrentChild->hasReference())
        {
            _lstChildren->removeAt(i);
            QList<GTACommandBase*>& referenceChildrens = pCurrentChild->getChildren();
            for (int j=0;j<referenceChildrens.size();j++)
            {
                _lstChildren->insert(i+j,referenceChildrens.at(j)->getClone());
            }

        }
    }
    currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-2;i>=0;i--)
    { 
        GTACommandBase* pCurrentChild = _lstChildren->at(i);
        pCurrentChild->expandReferences();
    }
    return true;
}
bool GTAActionWhile::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTAActionWhile::getDumpList()const
{
    return _dumpList;
}

void GTAActionWhile::setHeardAfter(const QString & iHeardAfter)
{
    _HeardAfter = iHeardAfter;
}
QString GTAActionWhile::getHeardAfter()const
{
    return _HeardAfter;
}
bool GTAActionWhile::hasLoop() const
{
    return true;

}

void GTAActionWhile::replaceUntagged()
{
    int currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-2;i>=0;i--)
    {
        GTACommandBase* pCurrentChild = _lstChildren->at(i);

        if (pCurrentChild!=NULL)
        {
            //if (pCurrentChild->canHaveChildren())
            //{
            pCurrentChild->replaceUntagged();
            //}

            QStringList varList = pCurrentChild->getVariableList();
            bool contains = false;
            foreach(QString varName,varList)
            {
                if(varName.contains(TAG_IDENTIFIER))
                {
                    contains = true;
                }
            }
            if(contains )
            {
                delete pCurrentChild ;
                _lstChildren->removeAt(i);
            }
        }
        
    }
}
void GTAActionWhile::setLogMessageList(const QList<GTAScxmlLogMessage> &iList)
{
    GTAScxmlLogMessage mainLogMsg;
    foreach(GTAScxmlLogMessage logMsg,iList)
    {
        if((logMsg.LOD == GTAScxmlLogMessage::Main)&& (mainLogMsg.Result!=GTAScxmlLogMessage::KO))
        {
            mainLogMsg=logMsg;
        }
        else if(logMsg.LOD == GTAScxmlLogMessage::Detail)
        {
            _LogMessageList.append(logMsg);
        }
    }
    _LogMessageList.append(mainLogMsg);
}
bool GTAActionWhile::hasChannelInControl()const
{
    if (GTACommandBase::hasChannelInCtrl(_Param))
    {
        return true;
    }

    if (GTACommandBase::hasChannelInCtrl(_ParamValue))
    {
        return true;
    }

    return false;
}

bool GTAActionWhile::createEndCommand(GTACommandBase* & opCmd)const
{
    opCmd = new GTAActionWhileEnd;
    return true;
}

void GTAActionWhile::setIsFsOnly(const bool &iVal)
{
    _IsFSOnly = iVal;
}

bool GTAActionWhile::getIsFsOnly()const
{
    return _IsFSOnly;
}

void GTAActionWhile::setIsValueOnly(const bool &iVal)
{
    _IsValueOnly = iVal;
}

bool GTAActionWhile::getIsValueOnly()const
{
    return _IsValueOnly;
}

QString GTAActionWhile::getFunctionalStatus()const
{
    return _FunctionalStatus;
}

void GTAActionWhile::setFunctionalStatus(const QString &iFuncStatus)
{
    _FunctionalStatus = iFuncStatus;
}

//void GTAActionWhile::setIsRepeatedLogging(const bool &iVal)
//{
//    _IsRepeatedLogging = iVal;
//}

//bool GTAActionWhile::getIsRepeatedLogging()const
//{
//    return _IsRepeatedLogging;
//}
