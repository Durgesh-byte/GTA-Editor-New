#include "GTAICDParameter.h"
#include "GTAUtil.h"

GTAICDParameter::GTAICDParameter()
{
    _ParamName= QString();
    _ParamUpperName = QString();
    _Bit= QString();
    _Identifier= QString();
    _Label= QString();
    _MaxValue= QString();
    _MinValue= QString();
    _SignalType= QString();
    _SourceType= QString();
    _ParamType= QString();
    _Unit= QString();
    _ValueType= QString();
    _SourceFile= QString();
    _ToolName = "NA";
    _ToolType = QString("NA");
    _UUID = QString();
    _Color = QString();
    _BackColor = QString();
}
void GTAICDParameter::setUUID(const QString &iUUID)
{
    _UUID= iUUID;
}

QString GTAICDParameter::getUUID() const
{
    return _UUID;
}
void GTAICDParameter::setName(const QString & iVal)
{ 
	_ParamName = iVal;
}
QString GTAICDParameter::getName() const{ return _ParamName;}

void GTAICDParameter::setUpperName(const QString& iVal)
{
    _ParamUpperName = iVal.toUpper();
}
QString GTAICDParameter::getUpperName() const { return _ParamUpperName; }

void GTAICDParameter::setBit(const QString & iVal){_Bit = iVal;}
QString GTAICDParameter::getBit() const{return _Bit;}

void GTAICDParameter::setIdentifier(const QString & iVal){ _Identifier = iVal;}
QString GTAICDParameter::getIdentifier() const{return _Identifier;}

void GTAICDParameter::setLabel(const QString & iVal){_Label = iVal;}
QString GTAICDParameter::getLabel() const{return _Label;}

void GTAICDParameter::setMaxValue(const QString & iVal){_MaxValue = iVal;}
QString GTAICDParameter::getMaxValue() const{return _MaxValue;}

void GTAICDParameter::setMinValue(const QString & iVal){_MinValue =iVal;}
QString GTAICDParameter::getMinValue() const{return _MinValue;}

void GTAICDParameter::setSignalType(const QString & iVal){_SignalType =iVal;}
QString GTAICDParameter::getSignalType() const{return _SignalType;}

void GTAICDParameter::setSourceFile(const QString & iVal)
{
    _SourceFile = iVal;
    if(_SourceFile.endsWith(".csv"))
        setSourceType("ICD");
    else if (_SourceFile.endsWith(".xml"))
        setSourceType("PMR");
    else if(_SourceFile.endsWith(".pir"))
        setSourceType(PARAM_TYPE_PIR);

    /*QFileInfo FileInfo(iVal);
	_SourceFileLastModifiedDate = FileInfo.lastModified().toString();*/
}
QString GTAICDParameter::getSourceFile() const{return _SourceFile;}

void GTAICDParameter::setSourceType(const QString & iVal){_SourceType = iVal; }
QString GTAICDParameter::getSourceType() const{return _SourceType;}

void GTAICDParameter::setParameterType(const QString & iVal){_ParamType = iVal;}
QString GTAICDParameter::getParameterType() const{return _ParamType;}

void GTAICDParameter::setUnit(const QString & iVal){_Unit = iVal;}
QString GTAICDParameter::getUnit() const{return _Unit;}

void GTAICDParameter::setValueType(const QString & iVal){_ValueType =iVal;}
QString GTAICDParameter::getValueType() const{return _ValueType;}
void GTAICDParameter::setEquipmentName(const QString &iVal){_EquipmentName = iVal/*.toUpper()*/;}
QString GTAICDParameter::getEquipmentName() const{return _EquipmentName;}

void GTAICDParameter::setApplicationName(const QString &iValue) 
{
	//if(iValue != QString(this->getName().split(".")[0]))
		//_ApplicationName = QString(this->getName().split(".")[0]);
        
    //IVYTE - 2113:Handled the crash scenario where application parameter doesnot have "."
    if (iValue.contains(".") && iValue.count(".") >= 1)

	    _ApplicationName = QString(iValue.split(".")[0]);
	else
		_ApplicationName= iValue;

}
QString GTAICDParameter::getApplicationName() const {return _ApplicationName;}

void GTAICDParameter::setBusName(const QString &iValue)
{
	//if (iValue != QString(this->getName().split(".")[1]))
		//_BusName = QString(this->getName().split(".")[1]);

    //I04DAIVYTE-2113:Handled the crash scenario where bus parameter doesnot have "."
    if (iValue.contains(".") && iValue.count(".") >= 1)

		_BusName = QString(iValue.split(".")[1]);
	else
		_BusName = iValue; 
}
QString GTAICDParameter::getBusName() const {return _BusName;}

void GTAICDParameter::setSignalName(const QString & iValue)
{ 
	//if (iValue != QString(this->getName().split(".")[2]))
		//_SignalName = QString(this->getName().split(".")[2]);
	if(iValue.contains(".") && iValue.count(".") == 2)
		_SignalName = QString(iValue.split(".")[2]);
	else
	//else
		_SignalName = iValue;
}

QString GTAICDParameter::getSignalName() const { return _SignalName; }
//QString GTAICDParameter::getSourceFileLastModifiedDate() const
//{
//	return _SourceFileLastModifiedDate;
//}

bool GTAICDParameter::operator ==(const QString & iVal)
{
    bool rc = false;
    if(! this->getName().isEmpty())
    {
        if(this->getName().toLower() == iVal.toLower())
            rc = true;
    }
    return rc;
}

bool GTAICDParameter::operator ==(const GTAICDParameter & iParam)
{
    QRegExp regExp("_uuid\\[\\{[a-zA-Z0-9]{8}-([a-zA-Z0-9]{4}-){3}[a-zA-Z0-9]{12}\\}\\]");
    bool rc = false;
    QString tempName = iParam.getTempParamName();
    if (tempName.contains(regExp))
    {
        //int pos = regExp.indexIn(tempName);
        QStringList list = regExp.capturedTexts();
		//explaining the ternary because Krutika asked me to (still sure this is self-explanatory)
		//if ((uuid of iParam == this->UUID) && (tempParamName of this contains tempParamName of iParam)
		//return true
        rc = ((list.first().remove("_uuid") == this->getUUID()) ? (this->getTempParamName().contains(tempName.split("_uuid").first())? true : false ) : false);
        return rc;
    }
    if(iParam.getTempParamName() == this->getTempParamName())
        rc = true;
    return rc;
}


QList<GTAICDParameterValues> GTAICDParameter::getPossibleValueList()
{
    return _PossibleValues;
}

QString GTAICDParameter::getPossibleValues()
{
    QString value;
    if(_PossibleValues.count() >0)
    {
        if(_ValueType == BOOLEAN_TYPE || _ValueType == FLOAT_TYPE
           || _ValueType == INTERGER_TYPE)
        {
            GTAICDParameterValues val1 = _PossibleValues.at(0);
            GTAICDParameterValues val2 = _PossibleValues.at(1);
            QString value1 = QString("[%1:%2]").arg(val1.key,val1.value);
            QString value2 = QString("[%1:%2]").arg(val2.key,val2.value);
            value = QString("{%1,%2}").arg(value1,value2);
        }
        else if(_ValueType == ENUMERATE_TYPE)
        {
            QStringList enumVals;
            for(int i = 0; i < _PossibleValues.count(); i++)
            {
                GTAICDParameterValues val = _PossibleValues.at(i);
                QString  enumVal = QString("[%1:%2]").arg(val.key,val.value);
                enumVals.append(enumVal);
            }
            value = enumVals.join(",");
            value = QString("{%1}").arg(value);
        }
    }
    return value;
}

void GTAICDParameter::setPossibleValues(QList<GTAICDParameterValues> &iPossibleValues)
{
    _PossibleValues = iPossibleValues;
}
QStringList GTAICDParameter::getAsList()
{
    QStringList items;
    items.append(getName());
    items.append(getUpperName());
    items.append(getParameterType());
    items.append(getSignalType());
    items.append(getUnit());
    items.append(getValueType());
    items.append(getMinValue());
    items.append(getMaxValue());
    items.append(getLabel());
    items.append(getIdentifier());
    items.append(getBit());
    items.append(getEquipmentName());
    items.append(getSourceFile());
    items.append(getLastModifiedDate());
    items.append(getSourceType());
    items.append(getApplicationName());
    items.append(getBusName());
    items.append(getSignalName());
    items.append(getMedia());
    items.append(getMessageName());
    items.append(getParameterLocalName());
    items.append(getFSName());
    items.append(getSuffix());
    items.append(getPrecision());
    items.append(getTempParamName());
    items.append(getDirection());
    items.append(getMediaType());
    items.append(getToolName());
    items.append(getToolType());
    items.append(getRefreshRate());
    items.append(getPossibleValues());
    items.append(getColor());
    items.append(getBackColor());

	return items;
}

void GTAICDParameter::setPossibleValues(const QString &iValues)
{
    //parse ivalues and put them in _possiblevalues
    QString values = iValues;
    _PossibleValues.clear();
    values.replace("{","");
    values.replace("}","");

    QStringList sepLst = values.split(",");

    for(int i = 0; i < sepLst.count(); i++)
    {
        QString value = sepLst.at(i);
        value.replace("[","");
        value.replace("]","");
        QStringList sepVals = value.split(":");
        if(sepVals.count() == 2)
        {
            GTAICDParameterValues val;
            val.key = sepVals.at(0);
            val.value = sepVals.at(1);
            _PossibleValues.append(val);
        }
    }
}

/* This function allow to know if the Parameter must be considered as isOnlyValue depending on its SignalType
* @input: iSignalType - QString containing the SignalType to search
* @return: bool
*/
bool GTAICDParameter::isOnlyValueDueToSignalType(QString & iSignalType)
{
	if (iSignalType == "DISCRETE" || iSignalType == "ANALOGUE" || iSignalType == "AFDX" || iSignalType == "ARINC" || iSignalType.isEmpty() || iSignalType.startsWith("MODEL", Qt::CaseInsensitive))
		return true;
	else
		return false;
}

/* This function allow to obtain the Media from the Signal Type of a Parameter
* @input: iSignalType - QString
* @output: oMedia - QString containing the Media
* @return: QString
*/
QString GTAICDParameter::getMediaFromSignalType(const QString& iSignalType)
{
	if(iSignalType == "NA")
		return QString("AFDX");
	if (iSignalType.contains("ARINC_"))
		return QString("A429");
	if (iSignalType.contains("CAN"))
		return QString("CAN");
	if (iSignalType == "ANALOGUE")
		return QString("ANA");
	if (iSignalType == "DISCRETE")
		return QString("DIS");
	if (iSignalType.isEmpty())
		return QString("VMAC");
	if (iSignalType == "AFDX" || iSignalType == "ARINC")
		return QString("PIR");
	if (iSignalType.contains("VCOM"))
		return QString("VCOM");
    if (iSignalType.contains("MODEL"))
        return QString("MODEL");
    else
        return QString("UNKNOWN");
}
