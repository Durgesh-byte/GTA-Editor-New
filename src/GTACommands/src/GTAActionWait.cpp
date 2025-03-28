#include "GTAActionWait.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

GTAActionWait::GTAActionWait(WaitType iType)
{
    setWaitType(iType);

	setAction(ACT_WAIT);
	if (iType)
		setComplement(COM_WAIT_FOR);
	else
		setComplement(COM_WAIT_UNTIL);

    _Param = QString();
    _ParamValue = QString();
    _Operator = QString();
    _Counter = QString();
    _FunctionalStatus = QString();
    _IsFSOnly = false;
    _IsValueOnly = false;
    _LoopSampling = QString();
    _IsLoopSampling = false;
    _UnitLoopSampling = QString();
}
GTAActionWait::GTAActionWait(const QString &iAction, const QString &iComplement,
                                   WaitType iType) : GTAActionBase(iAction,iComplement)
{
    setWaitType(iType);
    _Param = QString();
    _ParamValue = QString();
    _Operator = QString();
    _Counter = QString();
    _FunctionalStatus = QString();
    _IsFSOnly = false;
    _IsValueOnly = false;
    _LoopSampling = QString();
    _IsLoopSampling = false;
    _UnitLoopSampling = QString();
}
GTAActionWait::~GTAActionWait()
{

}
GTAActionWait::GTAActionWait(const GTAActionWait& iCmd):GTAActionBase(iCmd)
{
    //QString precisionVal, precisionType,ConfTime,ConfUnit;
    //iCmd.getPrecision(precisionVal,precisionType);
    //QString timeOut, unit;
    //iCmd.getTimeOut(timeOut,unit);

    //this->setAction(iCmd.getAction());
    //this->setComplement(iCmd.getComplement());
    //this->setCommandType(iCmd.getCommandType());
    //this->setActionOnFail(iCmd.getActionOnFail());
    //this->setPrecision(precisionVal,precisionType);
    //this->setTimeOut(timeOut,unit);
    //this->setComment(iCmd.getComment());
    //iCmd.getConfCheckTime(ConfTime,ConfUnit);
    //this->setConfCheckTime(ConfTime,ConfUnit);
    this->setWaitType(iCmd.getWaitType());
    this->setParameter(iCmd.getParameter());
    this->setValue(iCmd.getValue());
    this->setCondition(iCmd.getCondition());
    this->setCounter(iCmd.getCounter());
    this->setIsFsOnly(iCmd.getIsFsOnly());
    this->setIsValueOnly(iCmd.getIsValueOnly());
    this->setFunctionalStatus(iCmd.getFunctionalStatus());
    this->setLoopSampling(iCmd.getLoopSampling());
    this->setIsLoopSampling(iCmd.getIsLoopSampling());
    this->setUnitLoopSampling(iCmd.getUnitLoopSampling());
    setDumpList(iCmd.getDumpList());
}
QString GTAActionWait::getStatement() const
{
    QString oAction;
    QString paramName = getParameter();
    getTrimmedStatement(paramName);
    if(! getCounter().isEmpty())
    {
        oAction = QString("%1 %2 %3 S").arg(getAction(),getComplement(),getCounter());
    }
    else if(getIsFsOnly())
    {
        oAction = QString("%1 %2 (Function Status of %3 %5 %4)").arg(getAction(),getComplement(),paramName,getFunctionalStatus(),getCondition());
    }
    else if(getIsSDIOnly())
    {
        oAction = QString("%1 %5 (SDI value of %2 %4 %3)").arg(getAction(),paramName,getSDI(),getCondition(),getComplement());
    }
    else if(getIsParityOnly())
    {
        oAction = QString("%1 %5 (Parity of %2 %4 %3)").arg(getAction(),paramName,getParity(),getCondition(),getComplement());
    }
    else if(getIsRefreshRateOnly())
    {
        oAction = QString("%1 %5 (Refresh rate is set for %2)").arg(getAction(),paramName,getComplement());
    }
    else
        oAction = QString("%1 %2 %3 %4 %5").arg(getAction(),getComplement(),
                                                paramName,getCondition(),getValue());

    return oAction;
}
QString GTAActionWait::getLTRAStatement() const
{
    QString oAction;
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

    if(! getCounter().isEmpty())
    {
        oAction = QString("#c#%1#c# %2 %3 S").arg(getAction().toUpper(),getComplement(),getCounter());
    }
    else
        oAction = QString("#c#%1#c# %2 #b#%3#b# %4 #b#%5#b#").arg(getAction().toUpper(),getComplement(),
                                                                  paramter,getCondition(),value);

    return oAction;
}

QList<GTACommandBase*>& GTAActionWait::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionWait::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

void GTAActionWait ::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionWait::getParent(void) const
{
    return _parent;
}
void GTAActionWait::setWaitType(WaitType iType)
{
    _WaitType = iType;
}

GTAActionWait::WaitType GTAActionWait::getWaitType() const
{
    return _WaitType;
}
GTACommandBase* GTAActionWait::getClone() const
{
    return  (new GTAActionWait(*this));
}
bool GTAActionWait::canHaveChildren() const
{
    return false;//true;
}

QStringList GTAActionWait::resolveEngineParam(const QString &iParam)const
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

QStringList GTAActionWait::getVariableList() const
{
    QStringList lstOfVars;
    if (_WaitType==UNTIL)
    {
        //        lstOfVars.append(resolveEngineParam(_Param,_ParamValue));

        QStringList resolved = resolveEngineParam(_Param);
        if(!resolved.isEmpty())
            lstOfVars.append(resolved);

        resolved.clear();
        resolved = resolveEngineParam(_ParamValue);
        if(!resolved.isEmpty())
            lstOfVars.append(resolved);

    }
    if (_WaitType==FOR)
    {
        bool isNum= false;
        _Counter.toDouble(&isNum);
        if(!isNum && _Counter.startsWith("@") && _Counter.endsWith("@"))
            lstOfVars.append(_Counter);
		// Manage the case where a local parameter is used
		if(!isNum && _Counter.count(TAG_IDENTIFIER) == 0)
			lstOfVars.append(_Counter);
    }
    // GTACommandBase::collectVariableNames(_Parameter,lstOfVars);
    return lstOfVars;

}

//QStringList GTAActionWait::resolveEngineParam(const QString &iParam, const QString &iValue)const
//{
//    QStringList _lstParam;
//    QStringList lstOfVars;
//    _lstParam << iParam;
//    QStringList _lstValue;
//    _lstValue << iValue;
//    QString cicAIdentifier = QString("A:CIC%1").arg(GTA_CIC_IDENTIFIER);
//    QString cicBIdentifier = QString("B:CIC%1").arg(GTA_CIC_IDENTIFIER);
//    QString cnicAIdentifier = QString("A:CNIC%1").arg(GTA_CIC_IDENTIFIER);
//    QString cnicBIdentifier = QString("B:CNIC%1").arg(GTA_CIC_IDENTIFIER);

//    for(int i=0;i<_lstParam.size();i++)
//    {
//        QStringList lstOfVarsInParam;
//        QString paramName = _lstParam.at(i);

//        QString param1ChA = paramName;
//        QString param1ChB = paramName;

//        param1ChA.replace(cicAIdentifier,"A");
//        param1ChB.replace(cicAIdentifier,"B");
//        param1ChA.replace(cnicAIdentifier,"A");
//        param1ChB.replace(cnicAIdentifier,"B");


//        param1ChA.replace(cicBIdentifier,"A");
//        param1ChB.replace(cicBIdentifier,"B");
//        param1ChA.replace(cnicBIdentifier,"A");
//        param1ChB.replace(cnicBIdentifier,"B");

//        param1ChB.replace(GTA_CIC_IDENTIFIER,"");
//        param1ChA.replace(GTA_CIC_IDENTIFIER,"");
//        param1ChB.replace(GTA_CIC_IDENTIFIER,"");
//        param1ChA.replace(GTA_CIC_IDENTIFIER,"");



//        if (GTACommandBase::isVariable(param1ChA))
//        {
//            lstOfVarsInParam.append(param1ChA);
//        }
//        if (GTACommandBase::isVariable(param1ChB))
//        {
//            lstOfVarsInParam.append(param1ChB);
//        }
//        //GTACommandBase::collectVariableNames(_lstParam.at(i),lstOfVarsInParam);
//        if (!lstOfVarsInParam.isEmpty())
//            lstOfVars.append(lstOfVarsInParam);
//    }
//    for(int i=0;i<_lstValue.size();i++)
//    {
//        QStringList lstOfVarsInParam;
//        QString paramVal = _lstValue.at(i);
//        QString param2ChA = paramVal;
//        QString param2ChB = paramVal;

//        param2ChA.replace(cicAIdentifier,"A");
//        param2ChB.replace(cicAIdentifier,"B");
//        param2ChA.replace(cnicAIdentifier,"A");
//        param2ChB.replace(cnicAIdentifier,"B");

//        param2ChA.replace(cicBIdentifier,"A");
//        param2ChB.replace(cicBIdentifier,"B");
//        param2ChA.replace(cnicBIdentifier,"A");
//        param2ChB.replace(cnicBIdentifier,"B");


//        param2ChA.replace(GTA_CIC_IDENTIFIER,"");
//        param2ChB.replace(GTA_CIC_IDENTIFIER,"");
//        param2ChA.replace(GTA_CIC_IDENTIFIER,"");
//        param2ChB.replace(GTA_CIC_IDENTIFIER,"");

//        if (GTACommandBase::isVariable(param2ChA))
//        {
//            lstOfVarsInParam.append(param2ChA);
//        }
//        if (GTACommandBase::isVariable(param2ChB))
//        {
//            lstOfVarsInParam.append(param2ChB);
//        }
//        //GTACommandBase::collectVariableNames(_lstValue.at(i),lstOfVarsInParam);
//        if (!lstOfVarsInParam.isEmpty())
//            lstOfVars.append(lstOfVarsInParam);
//    }
//    lstOfVars.removeDuplicates();
//    return lstOfVars;

//}


bool GTAActionWait::hasChannelInControl()const
{
    bool hasChannelInCtrl = false;
    if (_WaitType == UNTIL)
    {
        hasChannelInCtrl = GTACommandBase::hasChannelInCtrl(_Param);

        if(!hasChannelInCtrl)
            hasChannelInCtrl = GTACommandBase::hasChannelInCtrl(_ParamValue);
    }


    return hasChannelInCtrl;
}

void GTAActionWait:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    if (_WaitType==UNTIL)
    {
        QMapIterator<QString,QString> iterTag(iTagValueMap);
        while (iterTag.hasNext())
        {
            iterTag.next();
            QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
            _Param.replace(tag,iterTag.value());
            _ParamValue.replace(tag,iterTag.value());
        }
    }
    if (_WaitType==FOR)
    {
        QMapIterator<QString,QString> iterTag(iTagValueMap);
        while (iterTag.hasNext())
        {
            iterTag.next();
            QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
            _Counter.replace(tag,iterTag.value());
            
        }
    }
}
void GTAActionWait::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    _Param.replace(iStrToFind,iStringToReplace);
    _ParamValue.replace(iStrToFind,iStringToReplace);

}
bool GTAActionWait::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
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
bool GTAActionWait::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
bool GTAActionWait::setDumpList(const QStringList& iDumpList)
{
    if (getComplement()==COM_WAIT_UNTIL)
    {
        _dumpList.clear();
        _dumpList.append(iDumpList);
    }

    return true;
}
QStringList GTAActionWait::getDumpList()const
{
    return _dumpList;

}
QString GTAActionWait::getSCXMLStateName()const
{
    
    QString stateName = getStatement();
    QString condition = getCondition();

    if(!condition.isEmpty())
    { 
        QHash<QString,QString> hshConds = GTAUtil::getNomenclatureOfArithSymbols();
        stateName.replace(QString(condition),hshConds[condition]);
    }

    return stateName;
}

bool GTAActionWait::getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QString &iEngine, bool isGenericSCXML, const QStringList iLocalVarList) const
{

    if(getComplement() == COM_WAIT_FOR)
    {
        oCondtionStatement = "";
        return false;
    }


    bool rc = true;
    QString lhs = _Param;
    QString rhs = _ParamValue;

    //GTAUtil::updateWithGenToolParam(lhs,iGenToolReturnMap);
    //GTAUtil::updateWithGenToolParam(rhs,iGenToolReturnMap);

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
		else if((lhsCnt == 2) && (rhsCnt == 1))
		{
			QString param_1List = getSCXMLCondition(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,isGenericSCXML,iLocalVarList);
			QString param_2List = getSCXMLCondition(resolvedLhsParams.at(1),resolvedRhsParams.at(0),iIcdParamList,isGenericSCXML,iLocalVarList);
			QString paramCond = QString("(%1 && %2)||(%3 && %4)").arg(channelSignalCondStatement_A, param_1List,channelSignalCondStatement_B, param_2List);
			oCondtionStatement.append(paramCond);
		}
		else if((lhsCnt == 1) && (rhsCnt == 1))
		{
			QString param_1List = getSCXMLCondition(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,isGenericSCXML,iLocalVarList);
			QString paramCond = QString("(%1 && %2)").arg(channelSignalCondStatement_A, param_1List);
			oCondtionStatement.append(paramCond);
		}
		else if((lhsCnt == 1) && (rhsCnt == 2))
		{
			QString param_1List = getSCXMLCondition(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,isGenericSCXML,iLocalVarList);
			QString param_2List = getSCXMLCondition(resolvedLhsParams.at(0),resolvedRhsParams.at(1),iIcdParamList,isGenericSCXML,iLocalVarList);
			QString paramCond = QString("(%1 && %2)||(%3 && %4)").arg(channelSignalCondStatement_A, param_1List,channelSignalCondStatement_B, param_2List);
			oCondtionStatement.append(paramCond);
		}
    }
    else
    {
        oCondtionStatement = getSCXMLCondition(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,isGenericSCXML,iLocalVarList);
    }

    return rc;

}

QString GTAActionWait::getSCXMLCondition(const QString &iLhs, const QString &iRhs, const QStringList &iIcdParamList, bool isGenericSCXML, const QStringList iLocalVarList) const
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
    QString op = getCondition();

	rhsVal = resolveInternalParams(iRhs,iLocalVarList);
	lshVal = resolveInternalParams(iLhs,iLocalVarList);

    if(isLhsParam)
    {
        QString fsVal = getFunctionalStatus();
        lshVal = lshVal+".Value";


        if(!isGenericSCXML)
        {
            if(_IsFSOnly)
            {
                if(op == ARITH_EQ)
                {
                    if(fsVal == "NOT_LOST")
                    {
                        fsVal = "DEAD";
                        lhsStatusCond = QString("(%1.Status != '%2')").arg(lhs,fsVal);
                    }
                    else
                    {
                        lhsStatusCond = QString("(%1.Status == '%2')").arg(lhs,fsVal);
                    }
                }
                else if(op == ARITH_NOTEQ)
                {
                    if(fsVal == "NOT_LOST")
                    {
                        fsVal = "DEAD";
                        lhsStatusCond = QString("(%1.Status == '%2')").arg(lhs,fsVal);
                    }
                    else
                    {
                        lhsStatusCond = QString("(%1.Status != '%2')").arg(lhs,fsVal);
                    }
                }
                else
                {
                    lhsStatusCond = QString("(%1.Status == '%2')").arg(lhs,fsVal);
                }
            }
            else if(!_IsValueOnly)
            {
                if(fsVal != ACT_FSSSM_NOT_LOST)
					lhsStatusCond = QString("%1.Status == '%2'").arg(lhs,fsVal);
				else
					lhsStatusCond = QString("%1.Status != '%2'").arg(lhs,ACT_FSSSM_LOST);
                //lhsStatusCond = QString("(%1.Status == '%2')").arg(lhs,fsVal);
            }
        }
        else
        {
            if(!_IsValueOnly)
                lhsStatusCond = QString("FS_%1.Value == FS_%1.validityVal.FS_%2").arg(lhs,fsVal);
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
        if(!lhsStatusCond.trimmed().isEmpty() && !_IsValueOnly)
            CondStatement.append(QString (" && (%1 == true)").arg(lhsStatusCond));

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
        CondStatement = QString("%1").arg(ValCond);
        if(!lhsStatusCond.trimmed().isEmpty() && !_IsValueOnly)
        {
            CondStatement.append(QString (" && (%1)").arg(lhsStatusCond));
            //            CondtionStatement = "(" + CondStatement +")";
        }
        //        else
        //        {
        //            CondtionStatement = CondStatement;
        //        }

        if(_IsFSOnly && isLhsParam)
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
    return CondtionStatement;
}

QString GTAActionWait::precisionCondition(const QString &iLhsVal, const QString &iRhsVal,const QString &op,const QString &iPrecision,const QString &precisionUnit,double dPrec, bool isGenericSCXML)const
{
    QString ValCond;
    if(!iPrecision.trimmed().isEmpty() && (iPrecision != "0"))
    {
        if(op == ARITH_EQ)
        {
            if(precisionUnit == ACT_PRECISION_UNIT_VALUE)
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
            else if(precisionUnit == ACT_PRECISION_UNIT_PERCENT)
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
            if(precisionUnit == ACT_PRECISION_UNIT_VALUE)
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
            else if(precisionUnit == ACT_PRECISION_UNIT_PERCENT)
            {
                if(!isGenericSCXML)
                {
                    bool isDoubleOk = false;
                    double  dPrecisionVal = iPrecision.toDouble(&isDoubleOk);
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


bool GTAActionWait::hasLoop() const
{
    if (getComplement()==COM_WAIT_UNTIL)
        return true;
    return false;
}
bool GTAActionWait::hasTimeOut() const
{
    if (getComplement()==COM_WAIT_UNTIL)
    {
        return true;
    }
    return false;
}
bool GTAActionWait::hasPrecision() const
{
    if (getComplement()==COM_WAIT_UNTIL)
    {
        return true;
    }
    return false;
}


void GTAActionWait::setIsFsOnly(const bool &iVal)
{
    _IsFSOnly = iVal;
}

bool GTAActionWait::getIsFsOnly()const
{
    return _IsFSOnly;
}

void GTAActionWait::setIsValueOnly(const bool &iVal)
{
    _IsValueOnly = iVal;
}

bool GTAActionWait::getIsValueOnly()const
{
    return _IsValueOnly;
}

QString GTAActionWait::getFunctionalStatus()const
{
    return _FunctionalStatus;
}

void GTAActionWait::setFunctionalStatus(const QString &iFuncStatus)
{
    _FunctionalStatus = iFuncStatus;
}

void GTAActionWait::setLoopSampling(const QString &iLoopSampling)
{
    _LoopSampling = iLoopSampling;
}

QString GTAActionWait::getLoopSampling()const
{
    return _LoopSampling;
}

void GTAActionWait::setIsLoopSampling(const bool &iVal)
{
    _IsLoopSampling = iVal;
}

bool GTAActionWait::getIsLoopSampling()const
{
    return _IsLoopSampling;
}

void GTAActionWait::setUnitLoopSampling(const QString &iVal)
{
    _UnitLoopSampling = iVal;
}

QString GTAActionWait::getUnitLoopSampling()const
{
    return _UnitLoopSampling;
}
