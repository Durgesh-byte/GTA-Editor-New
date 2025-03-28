#include "GTAOneClick.h"

GTAOneClick::GTAOneClick(const QString & iActionName, const QString & iComplement) :
        GTAActionBase(iActionName,iComplement)
{

}

GTAOneClick::GTAOneClick(const GTAOneClick & iCmd)
{
    setApplicationName(iCmd.getApplicationName());
    setConfFile(iCmd.getConfFile());
    setConfigName(iCmd.getConfigName());
    setComment(iCmd.getComment());
    setComplement(iCmd.getComplement());
    setTestRigName(iCmd.getTestRigName());
    setTestRigType(iCmd.getTestRigType());
    setAction(iCmd.getAction());
    setCommandType(iCmd.getCommandType());
    setDescription(iCmd.getDescription());
    setActionOnFail(iCmd.getActionOnFail());
    setDumpList(iCmd.getDumpList());
    setIsStartEnv(iCmd.getIsStartEnv());
    setIsStopAlto(iCmd.getIsStopAlto());
    setIsStopTestConfig(iCmd.getIsStopTestConfig());
    setInstanceName(iCmd.getInstanceName());

    QStringList options = iCmd.getOptionNames();
    foreach(QString option,options)
    {
        QString optionType = iCmd.getTypeofOption(option);
        QStringList optionValue = iCmd.getValueForOption(option);
        setOptionValueAndType(option,QStringList(optionValue),optionType);
        setDirNamesofOption(option,iCmd.getDirNamesofOption(option));
        setDirChkStatusofOption(option,iCmd.getDirChkStatusofOption(option));
        setIniFileNameofOption(option,iCmd.getIniFileNameofOption(option));

    }
}
GTAOneClick::~GTAOneClick()
{

}
void GTAOneClick::setApplicationName(const QString &iApplicationName)
{
    _ApplicationName = iApplicationName;
}
void GTAOneClick::setConfFile(const QString & iConfFile)
{
    _ConfFile = iConfFile;
}

void GTAOneClick::setDescription(const QString &iDescription)
{
    _Description = iDescription;
}
QString GTAOneClick::getDescription() const
{
    return _Description;
}
QString GTAOneClick::getConfFile() const
{
    return _ConfFile;
}
void GTAOneClick::setConfigName(const QString & iConfigName)
{
    _ConfigName = iConfigName;
}
QString GTAOneClick::getConfigName() const
{
    return _ConfigName;
}
void GTAOneClick::setTestRigName(const QString & iTestRigName)
{
    _TestRigName = iTestRigName;
}
QString GTAOneClick::getTestRigName() const
{
    return _TestRigName;
}

void GTAOneClick::setTestRigType(const QString &iTestRigType)
{
    _TestRigType = iTestRigType;
}

QString GTAOneClick::getTestRigType() const
{
    return _TestRigType;
}


QString GTAOneClick::getApplicationName() const
{
    return _ApplicationName;
}
void GTAOneClick::setOptionValueAndType(const QString &iOptionName,const QStringList & iOptionValue,const QString & iOptionType)
{
    if(!iOptionName.isEmpty())
    {
        if(!(iOptionName.isEmpty()&& iOptionType.isEmpty()&& iOptionValue.isEmpty()))
        {
            _OptionTypeMap.insert(iOptionName,iOptionType);
            _OptionValueMap.insert(iOptionName,iOptionValue);
            _OptionNamesList.append(iOptionName);
        }
    }
}
QStringList GTAOneClick::getOptionNames() const
{
    if(!_OptionNamesList.isEmpty())
    {
        return _OptionNamesList;
    }
    return QStringList();
}
QStringList GTAOneClick::getValueForOption(const QString & iOptionName) const
{
    if(!_OptionValueMap.isEmpty() && _OptionValueMap.contains(iOptionName))
    {
        return _OptionValueMap.value(iOptionName);
    }
    return QStringList();
}
QString GTAOneClick::getTypeofOption(const QString & iOptionName) const
{
    if(!_OptionTypeMap.isEmpty() && _OptionTypeMap.contains(iOptionName))
    {
        return _OptionTypeMap.value(iOptionName);
    }
    return QString();
}
QString GTAOneClick::getStatement() const
{

    QString configName = getConfigName();
    QStringList items;
    if(!_IsStartEnv)
    {
        if(_IsStopTestConfig)
            items.append("Stop TestConfig");
        if(_IsStopAlto)
            items.append("Stop Alto");
        configName = items.join(",");
    }

    QString oStatment = QString("%1 %2 %3").arg(getAction(),getComplement(),configName);
    return oStatment;
}
void GTAOneClick::setParent(GTACommandBase* iParent)
{
    _parent = iParent;
}
GTACommandBase* GTAOneClick::getParent(void) const
{
    return _parent;
}
QList<GTACommandBase*>& GTAOneClick::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAOneClick::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    //to avoid warning
    pBase;idPos;
    return false;
}
GTACommandBase * GTAOneClick::getClone()const
{
    return (new  GTAOneClick(*this));
}
bool GTAOneClick::canHaveChildren() const
{
    return false;
}

QStringList GTAOneClick::getVariableList() const
{
    return QStringList();

}

void GTAOneClick:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
    //do nothing
}
bool GTAOneClick::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
bool GTAOneClick::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTAOneClick::getDumpList()const
{
    return _dumpList;

}

bool GTAOneClick::getIsStartEnv() const{return _IsStartEnv;}
bool GTAOneClick::getIsStopTestConfig() const{return _IsStopTestConfig;}
bool GTAOneClick::getIsStopAlto()const {return _IsStopAlto;}

void GTAOneClick::setIsStartEnv(bool iVal)
{
    _IsStartEnv = iVal;
}

void GTAOneClick::setIsStopTestConfig(bool iVal)
{
    _IsStopTestConfig = iVal;
}

void GTAOneClick::setIsStopAlto(bool iVal)
{
    _IsStopAlto = iVal;
}


QStringList GTAOneClick::getDirNamesofOption(const QString &iOptionName) const
{
    return _DirNames.value(iOptionName);
}
QString GTAOneClick::getIniFileNameofOption(const QString &iOptionName) const
{
    return _IniFileName.value(iOptionName);
}
QHash<QString,bool> GTAOneClick::getDirChkStatusofOption(const QString &iOptionName) const
{
    return _DirChkStatus.value(iOptionName);
}

void GTAOneClick::setDirNamesofOption(const QString &iOptionName,const QStringList &iDirNames)
{
    if(!_DirNames.contains(iOptionName))
        _DirNames.insert(iOptionName,iDirNames);
}
void GTAOneClick::setIniFileNameofOption(const QString &iOptionName, const QString &iIniFileName)
{
    if(!_IniFileName.contains(iOptionName))
        _IniFileName.insert(iOptionName,iIniFileName);

}
void GTAOneClick::setDirChkStatusofOption(const QString &iOptionName, const QHash<QString,bool> &iDirChkStatus)
{
  
    if(!_DirChkStatus.contains(iOptionName))
        _DirChkStatus.insert(iOptionName,iDirChkStatus);
   

}

QStringList GTAOneClick::getSelectedDirNames(const QString &iOptionName) const
{
    QStringList selectedDirs;
    QHash<QString,bool> dirChkStatus = _DirChkStatus.value(iOptionName);
    foreach(QString key,dirChkStatus.keys())
    {
        if(dirChkStatus.value(key) == true)
            selectedDirs.append(key);
    }
    selectedDirs.removeDuplicates();
    return selectedDirs;
}
QString GTAOneClick::getSCXMLStateName()const
{
    return getStatement();

}