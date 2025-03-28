#include "GTACheckValue.h"
#include "GTACheckBase.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

GTACheckValue::GTACheckValue() :GTACheckBase(GTACheckBase::VALUE)
{
    _lstParam.clear();
    _lstValue.clear();
    _lstCondition.clear();
    _lstBinaryOperators.clear();
    _lstFunctionalStatus.clear();
    _lstPrecisionType.clear();
    _lstPrecisionValue.clear();
    _IsWaitUntil = false;
    _IsCheckFS.clear();
    _IsSDIOnly.clear();
    _IsParityOnly.clear();
    _IsRefreshRateOnly.clear();
    _lstSDIStatus.clear();
    _lstParityStatus.clear();
    _ConfirmationCheckTime=9999.999;
    _freeTextCondition = QString("");

    setActionOnFail(ACT_FAIL_CONTINUE);
}
GTACheckValue::GTACheckValue(const GTACheckValue& iObj):GTACheckBase(iObj)
{

    //QString sTimeOut, sTimeUnit, sPrecision, sPrecisionUnit;
    //iObj.getTimeOut(sTimeOut,sTimeUnit);
    //iObj.getPrecision(sPrecision,sPrecisionUnit);

    //setTimeOut(sTimeOut,sTimeUnit);
    //setPrecision(sPrecision,sPrecisionUnit);
    //setActionOnFail(iObj.getActionOnFail());
    //setCheckType(iObj.getCheckType());
    //setCommandType(iObj.getCommandType());

    setWaitUntil(iObj.getWaitUntil());
    setDumpList(iObj.getDumpList());
    ////setConfCheckTime(iObj.getConfCheckTime());
    //QString sConfTime,sConfTimeUnit;
    //iObj.getConfCheckTime(sConfTime,sConfTimeUnit);
    //setConfCheckTime(sConfTime,sConfTimeUnit);
    setLastEditedRow(iObj.getLastEditedRow());
    setLastEditedCol(iObj.getLastEditedCol());

    int noOfItems = iObj.getCount();
    for(int i=0;i<noOfItems;i++)
    {
        QString sParam = iObj.getParameter(i);
        QString sValue = iObj.getValue(i);
        QString sCondition = iObj.getCondition(i);
        QString sFuncStatus = iObj.getFunctionalStatus(i);
        double dParameterVal = iObj.getPrecisionValue(i);
        QString sParameterType = iObj.getPrecisionType(i);

        _lstParam<<sParam;
        _lstCondition<<sCondition;
        _lstValue<<sValue;
        _lstFunctionalStatus<<sFuncStatus;
        _lstPrecisionType<<sParameterType;
        _lstPrecisionValue<<dParameterVal;
        _IsCheckOnlyValue << iObj.getISOnlyCheckValue(i);
        _IsCheckFS<< iObj.getISCheckFS(i);
        _lstSDIStatus << iObj.getSDI(i);
        _lstParityStatus << iObj.getParity(i);
        _IsSDIOnly << iObj.getIsSDIOnly(i);
        _IsParityOnly << iObj.getIsParityOnly(i);
        _IsRefreshRateOnly << iObj.getIsRefreshRateOnly(i);
		_lstPrecisionValueFE << iObj.getPrecisionValueFE(i);
        if (i<noOfItems-1)
        {
            QString sOperator = iObj.getBinaryOperator(i);
            _lstBinaryOperators<<sOperator;
        }
    }
    setFreeTextCondition(iObj.getFreeTextCondition());
    //  setComment(iObj.getComment());
}
GTACommandBase* GTACheckValue::getClone()const
{
    return new GTACheckValue(*this);
}
GTACheckValue::~GTACheckValue()
{
}

void GTACheckValue::insertParamenter(const QString& iParam, 
                                        const QString& iCondition,
                                        const QString& iValue,
                                        const QString& iFuncStatus,
                                        const double& isPrecision,
										const QString& isPrecisionFE,
                                        const QString& isPrecisionType, 
                                        const bool isCheckOnlyValue,
                                        const bool isCheckFS,
                                        const bool isSDI,
                                        const bool isParity,
                                        const bool isRefreshRate,
                                        const QString &iSDI,
                                        const QString &iParity)
{

    _lstParam<<iParam;
    _lstCondition<<iCondition;
    _lstValue<<iValue;
    _lstFunctionalStatus<<iFuncStatus;
    _lstPrecisionValue<<isPrecision;
    _lstPrecisionType<<isPrecisionType;
    _IsCheckFS << isCheckFS;
    _IsCheckOnlyValue << isCheckOnlyValue;
    _lstSDIStatus << iSDI;
    _lstParityStatus << iParity;
    _IsSDIOnly << isSDI;
    _IsParityOnly << isParity;
    _IsRefreshRateOnly << isRefreshRate;
	_lstPrecisionValueFE << isPrecisionFE;
}

void GTACheckValue::setParameters(struct StructCheck parameters) 
{
	_lstParam << parameters.lstParam;
	_lstCondition << parameters.lstCondition;
	_lstValue << parameters.lstValue;
	_lstFunctionalStatus << parameters.lstFunctionalStatus;
	_lstPrecisionValue << parameters.lstPrecisionValue;
	_lstPrecisionType << parameters.lstPrecisionType;
	_IsCheckFS << parameters.isCheckFS;
	_IsCheckOnlyValue << parameters.isCheckOnlyValue;
	_lstSDIStatus << parameters.lstSDIStatus;
	_lstParityStatus << parameters.lstParityStatus;
	_IsSDIOnly << parameters.isSDIOnly;
	_IsParityOnly << parameters.isParityOnly;
	_IsRefreshRateOnly << parameters.isRefreshRateOnly;
	_lstPrecisionType << parameters.lstPrecisionType;
	_lstPrecisionValueFE << parameters.lstPrecisionValueFE;
}

void GTACheckValue::insertBinaryOperator(const QString& iBinaryOperator)
{
    _lstBinaryOperators<<iBinaryOperator;
}
void GTACheckValue::removeParameter(const int &iIndex)
{
    if(iIndex >= 0)
    {
        _lstParam.removeAt(iIndex);
        _lstCondition.removeAt(iIndex);
        _lstValue.removeAt(iIndex);
        if (iIndex<=_lstBinaryOperators.size()-1)
            _lstBinaryOperators.removeAt(iIndex);
        _lstFunctionalStatus.removeAt(iIndex);
        _lstPrecisionValue.removeAt(iIndex);
        _lstPrecisionType.removeAt(iIndex);
        _IsCheckOnlyValue.removeAt(iIndex);
		_lstPrecisionValueFE.removeAt(iIndex);

        if(iIndex < _lstSDIStatus.count())
            _lstSDIStatus.removeAt(iIndex);
        if(iIndex < _lstParityStatus.count())
            _lstParityStatus.removeAt(iIndex);
        if(iIndex < _IsSDIOnly.count())
            _IsSDIOnly.removeAt(iIndex);
        if(iIndex < _IsParityOnly.count())
            _IsParityOnly.removeAt(iIndex);
        if(iIndex < _IsRefreshRateOnly.count())
            _IsRefreshRateOnly.removeAt(iIndex);


    }

}


void GTACheckValue::clearAllParameters()
{
    _lstParam.clear();
    _lstCondition.clear();
    _lstValue.clear();
    _lstBinaryOperators.clear();
    _lstFunctionalStatus.clear();
    _lstPrecisionValue.clear();
    _lstPrecisionType.clear();
    _IsCheckOnlyValue.clear();
    _lstSDIStatus.clear();
    _lstParityStatus.clear();
    _IsSDIOnly.clear();
    _IsParityOnly.clear();
    _IsRefreshRateOnly.clear();
	_IsCheckFS.clear();
	_lstPrecisionValueFE.clear();
}

struct StructCheck GTACheckValue::getAllParameters()
{
	StructCheck toRet;
	toRet.lstParam = _lstParam;
	toRet.lstValue = _lstValue;
	toRet.lstCondition = _lstCondition;
	toRet.lstFunctionalStatus = _lstFunctionalStatus;
	toRet.lstPrecisionValue = _lstPrecisionValue;
	toRet.lstPrecisionType = _lstPrecisionType;
	toRet.lstBinaryOperators = _lstBinaryOperators;
	toRet.isCheckOnlyValue = _IsCheckOnlyValue;
	toRet.isSDIOnly = _IsSDIOnly;
	toRet.isParityOnly = _IsParityOnly;
	toRet.isRefreshRateOnly = _IsRefreshRateOnly;
	toRet.lstSDIStatus = _lstSDIStatus;
	toRet.lstParityStatus = _lstParityStatus;
	toRet.lstPrecisionValueFE = _lstPrecisionValueFE;
	return toRet;
}

QString GTACheckValue::getParameter(const int & iIndex) const
{
    return _lstParam.at(iIndex);
}
QStringList GTACheckValue::getListParameters() const
{
	return _lstParam;
}
QString GTACheckValue::getValue(const int & iIndex) const
{
    return _lstValue.at(iIndex);
}
QString GTACheckValue::getBinaryOperator(const int & iIndex) const
{
    if (!_lstBinaryOperators.isEmpty() && iIndex < _lstBinaryOperators.size())
        return _lstBinaryOperators.at(iIndex);
    else
        return "";
}

QString GTACheckValue::getSDI(const int & iIndex) const
{
    QString sSdi= QString("");
    if(!_lstSDIStatus.isEmpty())
    {
        if(iIndex >= 0 && iIndex < _lstSDIStatus.count())
            sSdi = _lstSDIStatus.at(iIndex);
    }
    return sSdi;
}
QString GTACheckValue::getParity(const int & iIndex) const
{
    QString parity = QString("");
    if(!_lstParityStatus.isEmpty())
    {
        if(iIndex >= 0 && iIndex < _lstParityStatus.count())
            parity = _lstParityStatus.at(iIndex);
    }
    return parity;
}


QString GTACheckValue::getFunctionalStatus(const int & iIndex) const
{
    return _lstFunctionalStatus.at(iIndex);
}
QString GTACheckValue::getCondition(const int & iIndex) const
{
    return _lstCondition.at(iIndex);
}
double GTACheckValue::getPrecisionValue(const int & iIndex) const
{
    return _lstPrecisionValue.at(iIndex);
}
QString GTACheckValue::getPrecisionValueFE(const int & iIndex) const
{
	if (_lstPrecisionValueFE.size() > iIndex)
		return _lstPrecisionValueFE.at(iIndex);
	else
		return QString("");
	//todo: errors handling
}
QStringList GTACheckValue::getAllPrecisionValueFE() const
{
	return _lstPrecisionValueFE;
}
QString GTACheckValue::getPrecisionType(const int & iIndex) const
{
    return _lstPrecisionType.at(iIndex);
}
QString GTACheckValue::getLTRAStatement() const
{
    QString oCheck ;
    oCheck = "#c#CHECK#c# ";
    for (int i=0;i<_lstParam.size();i++)
    {
        QString paramter = _lstParam.at(i);
        QString value = _lstValue.at(i);
        //        if(paramter.contains("."))
        //        {
        //            QStringList items = paramter.split(".");
        //            if(!items.isEmpty())
        //                paramter = items.last();
        //        }

        QString sPrecisonType = _lstPrecisionType.at(i)==ACT_PRECISION_UNIT_PERCENT?QString(ACT_PRECISION_UNIT_PERCENT):QString();
        if(_IsCheckFS.at(i) == true)
        {
            oCheck.append(QString ("#b#%1#b#(Function Status:#b#%2#b#)\n").arg( paramter,
                                                                                _lstFunctionalStatus.at(i)));

        }
        else if(_IsSDIOnly.at(i) == true)
        {
            oCheck.append(QString ("SDI value of #b#%1#b# %3 %2\n").arg( paramter,
                                                                         _lstSDIStatus.at(i),
                                                                         _lstCondition.at(i)));
        }
        else if(_IsParityOnly.at(i) == true)
        {
            oCheck.append(QString ("Parity of #b#%1#b# %3 %2\n").arg( paramter,
                                                                      _lstParityStatus.at(i),
                                                                      _lstCondition.at(i)));
        }
        //        else if(_IsRefreshRateOnly.at(i) == true)
        //        {
        //            oCheck.append(QString ("Refresh Rate of #b#%1#b# %3 %2\n").arg( paramter,
        //                                                                            CHK_REFRESH_RATE_STR,
        //                                                                            _lstCondition.at(i)));

        //        }
        else if(_IsCheckOnlyValue.at(i) == true)
        {
            oCheck.append(QString ("Only Value of #b#%1#b# %3 %2 with #b#Precision#b#:%4%5\n").arg( paramter,
                                                                                                    value,
                                                                                                    _lstCondition.at(i),
                                                                                                    QString::number(_lstPrecisionValue.at(i),'f',4),
                                                                                                    sPrecisonType));
        }
        else
        {
            //            if(value.contains("."))
            //            {
            //                QStringList items = value.split(".");
            //                if(!items.isEmpty())
            //                    value = items.last();
            //            }
            oCheck.append(QString("#b#%1#b#(Function Status:#b#%2#b#) %3 #b#%4#b# with #b#Precision#b#:%5%6\n").arg(  paramter,
                                                                                                                      _lstFunctionalStatus.at(i),
                                                                                                                      _lstCondition.at(i),
                                                                                                                      value,
                                                                                                                      QString::number(_lstPrecisionValue.at(i),'f',4),
                                                                                                                      sPrecisonType));

        }
        if (i<_lstBinaryOperators.size())
        {
            oCheck.append(QString("%1\n").arg(_lstBinaryOperators.at(i))) ;
        }
    }
    return oCheck;
}
QString GTACheckValue::getStatement() const
{
    QString oCheck ;
    oCheck = "Check ";
    if(_freeTextCondition.isEmpty())
    {
        for (int i=0;i<_lstParam.size();i++)
        {

            QString paramName =  _lstParam.at(i);
            getTrimmedStatement(paramName);

            QString paramVal = _lstValue.at(i);
            getTrimmedStatement(paramVal);

            QString sPrecisonType = _lstPrecisionType.at(i)==ACT_PRECISION_UNIT_PERCENT?QString(ACT_PRECISION_UNIT_PERCENT):QString();
            if(_IsCheckFS.at(i)==true)
            {
                oCheck.append(QString ("Function Status of %1 %4 %2 %3").arg(paramName,
                                                                             _lstFunctionalStatus.at(i),
                                                                             CHK_FS_STR,_lstCondition.at(i)));

            }
            else if(_IsSDIOnly.at(i) == true)
            {
                oCheck.append(QString ("SDI value of %1 %4 %2 %3").arg(paramName,
                                                                       _lstSDIStatus.at(i),
                                                                       CHK_SDI_STR,_lstCondition.at(i)));
            }
            else if(_IsParityOnly.at(i) == true)
            {
                oCheck.append(QString ("Parity of %1 %4 %2 %3").arg( paramName,
                                                                     _lstParityStatus.at(i),
                                                                     CHK_PARITY_STR,_lstCondition.at(i)));
            }
            else if(_IsRefreshRateOnly.at(i) == true)
            {
                oCheck.append(QString ("%2 %1").arg( paramName,CHK_REFRESH_RATE_STR));

            }
            else
            {
                QString expr;
				if (_lstPrecisionValueFE.size() > i && _lstPrecisionValueFE.at(i) != "") {
					expr = QString("Value %1(Function Status:%2) %3 %4(Precision:%5%6)\n").arg(paramName,
						_lstFunctionalStatus.at(i),
						_lstCondition.at(i),
						paramVal,
						_lstPrecisionValueFE.at(i),
						sPrecisonType);
				}
				else
				{
					expr = QString("Value %1(Function Status:%2) %3 %4(Precision:%5%6)\n").arg(paramName,
						_lstFunctionalStatus.at(i),
						_lstCondition.at(i),
						paramVal,
						QString::number(_lstPrecisionValue.at(i), 'f', 4),
						sPrecisonType);
				}
                if (_IsCheckOnlyValue.at(i))
                    expr.append(CHK_VALUE_ONLY_STR);

                oCheck.append(expr);

            }
            if (i<_lstBinaryOperators.size())
            {
                oCheck.append(QString("%1\n").arg(_lstBinaryOperators.at(i))) ;
            }
        }
    }
    else
    {
        oCheck.append(QString(" %1").arg(_freeTextCondition));
    }
    return oCheck;
}
QString GTACheckValue::getSCXMLStateName()const
{


    QString oCheck ;
    QHash <QString,QString> hshOfCondition= GTAUtil::getNomenclatureOfArithSymbols();
    if (_IsCheckOnlyValue.at(0))
        oCheck = QString("Check FS  %1_%2_%3").arg(_lstParam.at(0),hshOfCondition[ARITH_EQ],_lstFunctionalStatus.at(0));
    else    
    {
        oCheck = QString("Check Value %1 ").arg(_lstParam.at(0));
        if (!_lstFunctionalStatus.at(0).isEmpty())
            oCheck.append(QString("With FS %1 ").arg(_lstFunctionalStatus.at(0)));
        /*
            QString precisionText =  QString::number(_lstPrecisionValue.at(0),'f',4);
            QString sPrecisonType = _lstPrecisionType.at(0)==ACT_PRECISION_UNIT_PERCENT?QString("PERCENT"):QString();
            */

        oCheck.append(
                QString("%1 %2").arg( hshOfCondition.value(_lstCondition.at(0)),
                                      _lstValue.at(0))
                );

        /*      if(!sPrecisonType.isEmpty())
                oCheck.append(QString(" %1").arg(sPrecisonType));*/



    }
    
    



    return oCheck;

}
int GTACheckValue::getCount()const
{
    return _lstParam.size();
}
void GTACheckValue::setWaitUntil(bool iVal)
{
    _IsWaitUntil = iVal;
}
bool GTACheckValue::getWaitUntil() const
{
    return _IsWaitUntil;
}


QList<GTACommandBase*>& GTACheckValue ::getChildren(void) const
{
    return *_lstChildren;
}
bool GTACheckValue::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
    return false;
}

void  GTACheckValue::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTACheckValue::getParent(void) const
{
    return _parent;
}
//void GTACheckValue::setConfCheckTime(const double & iTime)
//{
//    _ConfirmationCheckTime = iTime;
//}
//double GTACheckValue::getConfCheckTime()const
//{
//    return _ConfirmationCheckTime;
//
//}

QStringList GTACheckValue::resolveEngineParam()const
{
    QStringList lstOfVars;
    QString cicAIdentifier = QString("A:CIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cicBIdentifier = QString("B:CIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cnicAIdentifier = QString("A:CNIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cnicBIdentifier = QString("B:CNIC%1").arg(GTA_CIC_IDENTIFIER);

    for(int i=0;i<_lstParam.size();i++)
    {
        QStringList lstOfVarsInParam;
        QString paramName = _lstParam.at(i);

        QString param1ChA = paramName;
        QString param1ChB = paramName;

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



        if (GTACommandBase::isVariable(param1ChA))
        {
            lstOfVarsInParam.append(param1ChA);
        }
        if (GTACommandBase::isVariable(param1ChB))
        {
            lstOfVarsInParam.append(param1ChB);
        }
        //GTACommandBase::collectVariableNames(_lstParam.at(i),lstOfVarsInParam);
        if (!lstOfVarsInParam.isEmpty())
            lstOfVars.append(lstOfVarsInParam);
    }
    for(int i=0;i<_lstValue.size();i++)
    {
        QStringList lstOfVarsInParam;
        QString paramVal = _lstValue.at(i);
        QString param2ChA = paramVal;
        QString param2ChB = paramVal;

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

        if (GTACommandBase::isVariable(param2ChA))
        {
            lstOfVarsInParam.append(param2ChA);
        }
        if (GTACommandBase::isVariable(param2ChB))
        {
            lstOfVarsInParam.append(param2ChB);
        }
        //GTACommandBase::collectVariableNames(_lstValue.at(i),lstOfVarsInParam);
        if (!lstOfVarsInParam.isEmpty())
            lstOfVars.append(lstOfVarsInParam);
    }
    lstOfVars.removeDuplicates();
    return lstOfVars;
}

QStringList GTACheckValue::resolveEngineParam(const QString &iParam)const
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

QStringList GTACheckValue::getVariableList() const
{

    QStringList lstOfVars;

    for(int i=0;i<_lstParam.size();i++)
    {
        QString paramName = _lstParam.at(i);
        QStringList resolved = resolveEngineParam(paramName);
        if(!resolved.isEmpty())
            lstOfVars.append(resolved);

    }

    for(int i=0;i<_lstValue.size();i++)
    {

        QString paramVal = _lstValue.at(i);
        QStringList resolved;
        resolved = resolveEngineParam(paramVal);
        if(!resolved.isEmpty())
            lstOfVars.append(resolved);
    }

    for (int i = 0;i < _lstCondition.size();i++)
    {
        QString paramName = _lstCondition.at(i);
        QStringList resolved = resolveEngineParam(paramName);
        if (!resolved.isEmpty() && (paramName.contains("line.")))
            lstOfVars.append(resolved);

    }


    return lstOfVars;

}

void GTACheckValue:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        for(int i=0;i<_lstParam.size();i++)
        {
            QString sParam =_lstParam.at(i);
            sParam.replace(tag,iterTag.value());
            _lstParam.replace(i,sParam);
        }

        for(int i=0;i<_lstValue.size();i++)
        {
            QString sParam =_lstValue.at(i);
            sParam.replace(tag,iterTag.value());
            _lstValue.replace(i,sParam);
        }
    }
}
void GTACheckValue::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    _lstParam.replaceInStrings(iStrToFind,iStringToReplace);
    _lstValue.replaceInStrings(iStrToFind,iStringToReplace);

}
bool GTACheckValue::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QStringList temp = _lstParam;
    bool rc = false;
    for (auto i = 0; i < temp.size(); ++i)
    {
        if (temp[i].count('.') == 2) // triplet detection
        {
            temp[i].chop(temp[i].size() - temp[i].indexOf('.'));
            if (temp[i].contains(iStrToFind))
            {
                rc = true;
                temp[i].replace(iStrToFind, iStringToReplace);
                _lstParam[i].remove(0, _lstParam[i].indexOf('.'));
                _lstParam[i].prepend(temp[i]);
            }
        }
    }
    return rc;
}
bool GTACheckValue::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;

    foreach(QString str, _lstParam)
    {
        if (str.contains(iRegExp))
            return true;
    }
    foreach(QString str, _lstValue)
    {
        if (str.contains(iRegExp))
            return true;
    }
    
    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;
    return rc;
}



bool GTACheckValue::getIsRefreshRateOnly(const int & iIndex) const
{
    if(!_IsRefreshRateOnly.isEmpty())
        return _IsRefreshRateOnly.at(iIndex);
    else
        return false;
}

bool GTACheckValue::getIsParityOnly(const int & iIndex) const
{
    if(!_IsParityOnly.isEmpty())
        return _IsParityOnly.at(iIndex);
    else
        return false;
}


bool GTACheckValue::getIsSDIOnly(const int & iIndex) const
{
    if(!_IsSDIOnly.isEmpty())
        return _IsSDIOnly.at(iIndex);
    else
        return false;
}


bool GTACheckValue::getISCheckFS(const int & iIndex) const
{
    if(!_IsCheckFS.isEmpty())
        return _IsCheckFS.at(iIndex);
    else
        return false;
}
bool GTACheckValue::getISOnlyCheckValue(const int & iIndex) const
{
    if(!_IsCheckOnlyValue.isEmpty())
        return _IsCheckOnlyValue.at(iIndex);
    else
        return false;
}
bool GTACheckValue::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTACheckValue::getDumpList()const
{
    return _dumpList;

}
bool GTACheckValue::hasLoop() const
{

    return _IsWaitUntil;
    
    
}

bool GTACheckValue::hasChannelInControl()const
{

    for(int i=0;i<_lstParam.size();i++)
    {

        QString paramName = _lstParam.at(i);
        if (GTACommandBase::hasChannelInCtrl(paramName))
        {
            return true;
        }

    }
    for(int i=0;i<_lstValue.size();i++)
    {
        QString paramName = _lstValue.at(i);
        if (GTACommandBase::hasChannelInCtrl(paramName))
        {
            return true;
        }
    }

    return false;
}
void GTACheckValue::updateWithGenToolParam(QString &ioParam, const QHash<QString, QString> &iGenToolReturnMap) const
{
    if(!ioParam.contains("$"))
        return;
    QStringList iTokens = ioParam.split("$");
    QStringList keys = iGenToolReturnMap.keys();
    for(int i=0;i<keys.count();i++)
    {
        QString key = keys.at(i);

        QStringList tokens = key.split("$");

        if(tokens.at(0) == iTokens.at(0))
        {
            if(iTokens.at(1).contains(tokens.at(1)))
            {
                ioParam = iGenToolReturnMap.value(key);
                break;
            }
        }


    }
}

void GTACheckValue::getConditionStatementForFreeText(const QStringList &, QString &, const QHash<QString, QString> &, const QString &, bool, const QStringList &) const
{

}

void GTACheckValue::getConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &iGenToolReturnMap, const QString &iEngine, bool isGenericSCXML, const QStringList &iLocalVarList) const
{
    int cnt = getCount();

    QStringList condItems;

    condItems.append("(");

    for(int i=0;i<cnt;i++)
    {
        QString param = getParameter(i);
        updateWithGenToolParam(param,iGenToolReturnMap);
        QString value = getValue(i);
        updateWithGenToolParam(value,iGenToolReturnMap);
        QString cond = getBinaryOperator(i);

        QString lhs = param;
        QString rhs = value;


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

//            if(lhs.contains(cicAIdentifier) || rhs.contains(cicBIdentifier))
            if(lhs.contains(cicAIdentifier) || lhs.contains(cicBIdentifier)|| rhs.contains(cicBIdentifier) || rhs.contains(cicAIdentifier))
            {
                channelSignalCondStatement_A = QString("(%1.Value == 1)").arg(channelSignal);
                channelSignalCondStatement_B = QString("(%1.Value == 0)").arg(channelSignal);
            }
//            else if(lhs.contains(cnicAIdentifier) || rhs.contains(cnicBIdentifier))
            else if(lhs.contains(cnicAIdentifier) || lhs.contains(cnicBIdentifier) || rhs.contains(cnicBIdentifier) || rhs.contains(cnicAIdentifier))
            {
                channelSignalCondStatement_A = QString("(%1.Value != 1)").arg(channelSignal);
                channelSignalCondStatement_B = QString("(%1.Value != 0)").arg(channelSignal);
            }
            else
            {
                channelSignalCondStatement_A = "true";
                channelSignalCondStatement_B = "true";
            }

            int lhsCnt = resolvedLhsParams.count();
            int rhsCnt = resolvedRhsParams.count();

            if((lhsCnt == 2) && (rhsCnt == 2))
            {

                QStringList param_1List = getCheckValueConditionForSCXML(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,i,isGenericSCXML,iLocalVarList);
                QStringList param_2List = getCheckValueConditionForSCXML(resolvedLhsParams.at(1),resolvedRhsParams.at(1),iIcdParamList,i,isGenericSCXML,iLocalVarList);
                QString paramCond = QString("(%1 && %2)||(%3 && %4)").arg(channelSignalCondStatement_A, param_1List.at(0),channelSignalCondStatement_B, param_2List.at(0));
                condItems.append(paramCond);
            }
            else if((lhsCnt == 2) && (rhsCnt == 1))
            {
                QStringList param_1List = getCheckValueConditionForSCXML(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,i,isGenericSCXML,iLocalVarList);
                QStringList param_2List = getCheckValueConditionForSCXML(resolvedLhsParams.at(1),resolvedRhsParams.at(0),iIcdParamList,i,isGenericSCXML,iLocalVarList);
                QString paramCond = QString("(%1 && %2)||(%3 && %4)").arg(channelSignalCondStatement_A, param_1List.at(0),channelSignalCondStatement_B, param_2List.at(0));
                condItems.append(paramCond);
            }
            else if((lhsCnt == 1) && (rhsCnt == 1))
            {
                QStringList param_1List = getCheckValueConditionForSCXML(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,i,isGenericSCXML,iLocalVarList);
                QString paramCond = QString("(%1 && %2)").arg(channelSignalCondStatement_A, param_1List.at(0));
                condItems.append(paramCond);
            }
            else if((lhsCnt == 1) && (rhsCnt == 2))
            {
                QStringList param_1List = getCheckValueConditionForSCXML(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,i,isGenericSCXML,iLocalVarList);
                QString paramCond = QString("(%1 && %2)").arg(channelSignalCondStatement_A, param_1List.at(0));
                condItems.append(paramCond);
            }



            if(cnt>1)
            {
                if (!cond.isEmpty())
                {
                    QString binOper;
                    if (cond=="AND")
                    {
                        binOper = "&&";
                        condItems.append(binOper);
                    }
                    else if (cond=="OR")
                    {
                        binOper = "||";
                        condItems.append(")");
                        condItems.append(binOper);
                        condItems.append("(");
                    }

                }
            }
        }
        else
        {
			condItems.append(getCheckValueConditionForSCXML(resolvedLhsParams.at(0),resolvedRhsParams.at(0),iIcdParamList,i,isGenericSCXML,iLocalVarList));
            if(cnt>1)
            {
                if (!cond.isEmpty())
                {
                    QString binOper;
                    if (cond=="AND")
                    {
                        binOper = "&&";
                        condItems.append(binOper);
                    }
                    else if (cond=="OR")
                    {
                        binOper = "||";
                        condItems.append(")");
                        condItems.append(binOper);
                        condItems.append("(");
                    }

                }
            }
        }
    }
    if(!condItems.isEmpty())
    {
        condItems.append(")");
        oCondtionStatement = condItems.join(" ");
    }
}


bool GTACheckValue::getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &iGenToolReturnMap, const QString &iEngine, bool isGenericSCXML, const QStringList &iLocalVarList) const
{
    bool rc = true;

    if(_freeTextCondition.isEmpty())
    {
        getConditionStatement(iIcdParamList,oCondtionStatement,iGenToolReturnMap,iEngine,isGenericSCXML,iLocalVarList);
    }
    else
    {
        getConditionStatementForFreeText(iIcdParamList,oCondtionStatement,iGenToolReturnMap,iEngine,isGenericSCXML,iLocalVarList);
    }
    return rc;
}

QString GTACheckValue::getEvalCondId() const {
	return _evalCondId;
}

void GTACheckValue::setEvalCondId(QString const evalCondId) {
	_evalCondId = evalCondId;
}


QStringList GTACheckValue::getCheckValueConditionForSCXML(const QString &iLhs, const QString &iRhs,const QStringList &iIcdParamList,const int &iIndex, bool isGenericSCXML,const QStringList &iLocalVarList)const
{

    QStringList condItems;
    QString op = getCondition(iIndex);
    QString cond = getBinaryOperator(iIndex);
    QString fs = getFunctionalStatus(iIndex);
	fs.replace(" ", "");
    QString sdi = getSDI(iIndex);
    QString parity = getParity(iIndex);
    QString precisionUnit = getPrecisionType(iIndex);
    QString precision = QString::number(getPrecisionValue(iIndex));
	QString precisionFE = getPrecisionValueFE(iIndex);
	precisionFE = precisionFE.simplified();
	precisionFE.replace(" ", "");

	/*Get Array and counter to use in foreach*/
	QStringList templist = _evalCondId.split('_');
	QString forEachArr;
	if (templist.contains("ForEachStates")) {
		QString cntr;
		QString arr;
		int i = templist.size() - 1;
		while (i >= 0) {
			if (templist.at(i) == "ForEachStates") {
				for (int j = 0; j < i; j++) {
					if (j != 0) {
						cntr += "_" + templist.at(j);
						arr += "_" + templist.at(j);
					}
					else {
						cntr += templist.at(j);
						arr += templist.at(j);
					}

				}
				cntr += "_arr_cntr";
				arr += "_line_arr";
				i = -1;
			}
			i--;
		}
		forEachArr = arr + '[' + cntr + ']';
	}

	if (fs.startsWith("line")) {
		QString columnName = fs.split("__").at(1);
		fs = forEachArr + '.' + columnName;
	}

    bool isChkVal = getISOnlyCheckValue(iIndex);
    bool isChkFS = getISCheckFS(iIndex);
    bool isSDI = getIsSDIOnly(iIndex);
    bool isParity = getIsParityOnly(iIndex);
    bool isRefreshRate = getIsRefreshRateOnly(iIndex);

    //generic scxml sends only signal value in parameter list
    //thus signal value should be identified for contains
    bool isLhsParam = isGenericSCXML? (iIcdParamList.contains(iLhs.split(".").last())) : (iIcdParamList.contains(iLhs));
    bool isRhsParam = isGenericSCXML? (iIcdParamList.contains(iRhs.split(".").last())) : (iIcdParamList.contains(iRhs));

    bool isLhsLocal = iLocalVarList.contains(iLhs);
    bool isRhsLocal = iLocalVarList.contains(iRhs);
    
    QString lshVal = iLhs;
    QString rhsVal = iRhs;

	rhsVal = resolveInternalParams(iRhs,iLocalVarList);
	lshVal = resolveInternalParams(iLhs,iLocalVarList);
        op = resolveInternalParams(op, iLocalVarList);
	
	QString lhsStatusCond, rhsStatusCond;

	

	if (_lstParam.at(0).startsWith("line")) {
		QString condParam = _evalCondId;
		int length = _evalCondId.length();
		QString returnCheck = condParam.remove(length - 8, 8);
		QString columnName = "Value";
		if (iRhs.startsWith("line")) {
			columnName = iRhs.split("__").at(1);
		}
		returnCheck += "getParameters_returnCheck.";
		returnCheck += "Value";
		lshVal = returnCheck;
		if (iRhs.startsWith("line") ){
			rhsVal = rhsVal.split('.').at(0) + '.' + columnName;
		}
		if (fs != ACT_FSSSM_NOT_LOST && !fs.contains("line"))
			lhsStatusCond = QString("%1.Status == '%2'").arg(lshVal.split('.').at(0), fs);
        else if (fs.contains("line"))
            lhsStatusCond = QString("%1.Status == %2").arg(lshVal.split('.').at(0), fs);
		else
			lhsStatusCond = QString("%1.Status != '%2'").arg(lshVal.split('.').at(0), ACT_FSSSM_LOST);
	}


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


    if(isLhsParam)
    {
        lshVal = lshVal+".Value";
        if(!isGenericSCXML)
        {
            if(fs != ACT_FSSSM_NOT_LOST)
                lhsStatusCond = QString("%1.Status == '%2'").arg(iLhs,fs);
            else
                lhsStatusCond = QString("%1.Status != '%2'").arg(iLhs,ACT_FSSSM_LOST);
        }
        else
        {
            if(fs != ACT_FSSSM_NOT_LOST)
                lhsStatusCond = QString("FS_%1.Value == FS_%2.value.FS.%3").arg(iLhs,iLhs.split(".").last(),fs);
            else
                lhsStatusCond = QString("FS_%1.Value != FS_%2.value.FS.NO").arg(iLhs,iLhs.split(".").last());
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

    QString ValCond;

    if(iLhs.contains("\"") || (iRhs.contains("\"")))
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
        if((!lhsStatusCond.trimmed().isEmpty()) && !isChkVal)
        {
            CondStatement.append(QString (" && (%1)").arg(lhsStatusCond));
        }

        if(isChkFS)
            CondStatement = lhsStatusCond;
        else if(isSDI && isLhsParam)
        {
            bool ok;
            int sdiValue = sdi.toInt(&ok,2);
            if(!ok)
                sdiValue = 0;
            CondStatement = QString("%1.Sdi == %2").arg(iLhs,QString::number(sdiValue));
        }
        else if(isParity && isLhsParam)
        {
            bool ok;
            int parityValue = parity.toInt(&ok,2);
            if(!ok)
                parityValue = 0;
            CondStatement = QString("%1.Parity == %2").arg(iLhs,QString::number(parityValue));
        }
        else if(isRefreshRate && isLhsParam)
        {
            // fill refresh rate here later
            CondStatement = precisionCondition(QString("%1.RefreshRate").arg(iLhs),"250","=",precision,precisionUnit,dPrec,isGenericSCXML);
        }
        condItems.append("(" + CondStatement +")");
    }
    else
    {
		if (precisionFE.startsWith("line")) 
		{
			

			precisionFE = forEachArr + '.' + precisionFE.split("__").at(1);
			ValCond = precisionCondition(lshVal, rhsVal, op, precisionFE, precisionUnit, dPrec, isGenericSCXML);
		}
		else
			ValCond = precisionCondition(lshVal, rhsVal, op, precision, precisionUnit, dPrec, isGenericSCXML);
        if(isChkVal)
        {
            CondStatement = QString("(%1)").arg(ValCond);
        }
        else if(isChkFS)
        {
            if((!lhsStatusCond.trimmed().isEmpty()))
            {
                CondStatement.append(QString ("(%1)").arg(lhsStatusCond));
            }
        }
        else if(isSDI && isLhsParam)
        {
            bool ok;
            int sdiValue = sdi.toInt(&ok,2);
            if(!ok)
                sdiValue = 0;
            CondStatement = QString("%1.Sdi == %2").arg(iLhs,QString::number(sdiValue));
        }
        else if(isParity && isLhsParam)
        {
            bool ok;
            int parityValue = parity.toInt(&ok,2);
            if(!ok)
                parityValue = 0;
            CondStatement = QString("%1.Parity == %2").arg(iLhs,QString::number(parityValue));
        }
        else if(isRefreshRate && isLhsParam)
        {
            // fill refresh rate here later
            CondStatement = precisionCondition(QString("%1.RefreshRate").arg(iLhs),"250","=",precision,precisionUnit,dPrec,isGenericSCXML);
        }
        else
        {
            CondStatement = QString("(%1)").arg(ValCond);
            if((!lhsStatusCond.trimmed().isEmpty()) && !isChkVal)
            {
                CondStatement.append(QString (" && (%1)").arg(lhsStatusCond));
            }
        }

        //        if(!isChkFS)
        //        {
        //            CondStatement = QString("(%1)").arg(ValCond);
        //            if((!lhsStatusCond.trimmed().isEmpty()) && !isChkVal)
        //            {
        //                CondStatement.append(QString (" && (%1)").arg(lhsStatusCond));
        //            }
        //            condItems.append("(" + CondStatement +")");
        //        }
        //        else
        //        {
        //            if((!lhsStatusCond.trimmed().isEmpty()))
        //            {
        //                CondStatement.append(QString ("(%1)").arg(lhsStatusCond));
        //                condItems.append("(" + CondStatement +")");
        //            }
        //        }

        condItems.append("(" + CondStatement +")");
    }
    return condItems;
}


QString GTACheckValue::precisionCondition(const QString &iLhsVal, const QString &iRhsVal,const QString &op,const QString &iPrecision,const QString &precisionUnit,double dPrec, bool isGenericSCXML)const
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
					if (iPrecision.contains("line") && !isDoubleOk)
						ValCond = QString("almostEqualPercent(%1,%2,%3)").arg(iLhsVal, iRhsVal, iPrecision);
					else
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
            else if (op == ARITH_LTEQ || op == ARITH_GTEQ || op == ARITH_GT || op == ARITH_LT)
            {
                ValCond = QString("%1 %3 %2").arg(iLhsVal, iRhsVal, op);
            }
            else
            {
                ValCond = QString("eval(%1 %3 %2)").arg(iLhsVal, iRhsVal, "+" + op + "+");
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
        else if (op == ARITH_LTEQ || op == ARITH_GTEQ || op == ARITH_GT || op == ARITH_LT)
        {
            ValCond = QString("%1 %3 %2").arg(iLhsVal, iRhsVal, op);
        }
        else
        {
            ValCond = QString("eval(%1 %3 %2)").arg(iLhsVal, iRhsVal, "+" + op + "+");
        }
    }
    return ValCond;
}
void GTACheckValue::setFreeTextCondition(const QString &iText)
{
    _freeTextCondition = iText;
}

QString GTACheckValue::getFreeTextCondition()const
{
    return _freeTextCondition;
}
int GTACheckValue::getIndex(QString &iParam,QString &oParam_Val)
{
    for(int i=0;i<_lstParam.size();i++)
    {
        QString paramName = _lstParam.at(i);
        if (iParam == paramName)
        {
            oParam_Val = "param";
            return i;
        }
    }

    for(int i=0;i<_lstValue.size();i++)
    {

        QString paramVal = _lstValue.at(i);
        if (iParam == paramVal)
        {
            oParam_Val = "val";
            return i;
        }
    }
    oParam_Val = "";
    return -1;
}
