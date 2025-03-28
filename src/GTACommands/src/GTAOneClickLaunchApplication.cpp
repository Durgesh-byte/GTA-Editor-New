#include "GTAOneClickLaunchApplication.h"
#include "GTAUtil.h"






GTAOneClickLaunchApplication::GTAOneClickLaunchApplication() :
        GTAActionBase(ACT_ONECLICK,COM_ONECLICK_APP)
{

}

GTAOneClickLaunchApplication::GTAOneClickLaunchApplication(const GTAOneClickLaunchApplication & iCmd)
{
    setAction(iCmd.getAction());
    setComment(iCmd.getComment());
    setComplement(iCmd.getComplement());
    setActionOnFail(iCmd.getActionOnFail());
    setDumpList(iCmd.getDumpList());
    setCommandType(iCmd.getCommandType());

    QList<GTAOneClickLaunchApplicationItemStruct *> icmdItems = iCmd.getLaStruct();
    foreach (GTAOneClickLaunchApplicationItemStruct * iAppItem,icmdItems)
    {
      _LAItemStructs.append(new GTAOneClickLaunchApplicationItemStruct(*iAppItem));
    }
}
GTAOneClickLaunchApplicationItemStruct::GTAOneClickLaunchApplicationItemStruct()
{

}
GTAOneClickLaunchApplicationItemStruct::GTAOneClickLaunchApplicationItemStruct(const GTAOneClickLaunchApplicationItemStruct& rhs)
{

    _TestRig=rhs.getTestRig();
    _ToolId=rhs.getToolId() ;
    _ToolType=rhs.getToolType();
    _AppName=rhs.getAppName() ;
    _ConfFile=rhs.getConfigFile();
    _AppItem=rhs.getAppItem();
    _Options=rhs.getOptions();
    _KillMode=rhs.getKillMode();
    foreach(QString sOption, _Options)
    {
        _OptionValMap[sOption]=rhs.getValue(sOption);
        _OptionTypeMap[sOption]=rhs.getType(sOption);
        _OptionArgMap[sOption]=rhs.getArgs(sOption);
    }
   
}

GTAOneClickLaunchApplication::~GTAOneClickLaunchApplication()
{

}

void GTAOneClickLaunchApplicationItemStruct::setKillMode(bool iKillMode)
{

_KillMode = iKillMode;
}

bool GTAOneClickLaunchApplicationItemStruct::getKillMode() const
{
return _KillMode;

}
void GTAOneClickLaunchApplicationItemStruct::setAppName(const QString &iAppName)
{
    _AppName = iAppName;
}

void GTAOneClickLaunchApplicationItemStruct::setToolId(const QString &iToolId)
{
    _ToolId = iToolId;
}


void GTAOneClickLaunchApplicationItemStruct::setToolType(const QString &iToolType)
{
    _ToolType = iToolType;
}


void GTAOneClickLaunchApplicationItemStruct::setTestRig(const QString &iTestRig)
{
    _TestRig = iTestRig;
}

void GTAOneClickLaunchApplicationItemStruct::insertOption(const QString &iOptionName, const QString &iOptionValue)
{
    if(!_Options.contains(iOptionName))
    {
        _Options<<iOptionName;
        _OptionValMap.insert(iOptionName,iOptionValue);
    }
    else
    {
        _OptionValMap[iOptionName] = iOptionValue;
    }
}
void GTAOneClickLaunchApplicationItemStruct::insertValArgs(const QString &iOptionName, const QString &iArgs)
{
    if(!_OptionArgMap.contains(iOptionName))
        _OptionArgMap.insert(iOptionName,iArgs);
    else
        _OptionArgMap[iOptionName] = iArgs;
}
void GTAOneClickLaunchApplicationItemStruct::insertOptionType(const QString &iOptionName, const QString &iType)
{
    if(!_OptionTypeMap.contains(iOptionName))
        _OptionTypeMap.insert(iOptionName,iType);
    else
        _OptionTypeMap[iOptionName] = iType;
}
void GTAOneClickLaunchApplicationItemStruct::setConfigFile(const QString &iConfigFile)
{
    _ConfFile = iConfigFile;
}

QString GTAOneClickLaunchApplicationItemStruct::getConfigFile() const
{
    return _ConfFile;
}
QString GTAOneClickLaunchApplicationItemStruct::getAppName() const
{
    return _AppName;
}

QString GTAOneClickLaunchApplicationItemStruct::getToolId() const
{
    return _ToolId;
}


QString GTAOneClickLaunchApplicationItemStruct::getToolType()const
{
    return _ToolType;
}


QString GTAOneClickLaunchApplicationItemStruct::getTestRig()const
{
    return _TestRig;
}

QStringList GTAOneClickLaunchApplicationItemStruct::getOptions()const
{
    return _Options;
}

QString GTAOneClickLaunchApplicationItemStruct::getArgs(const QString &iOptionName) const
{
    return _OptionArgMap.value(iOptionName);
}

QString GTAOneClickLaunchApplicationItemStruct::getValue(const QString &iOptionName) const
{
    return _OptionValMap.value(iOptionName);
}
QString GTAOneClickLaunchApplicationItemStruct::getType(const QString &iOptionName) const
{
    return _OptionTypeMap.value(iOptionName);
}

QString GTAOneClickLaunchApplication::getStatement() const
{

    QString oStatment = getAction();
    for(int i=0;i<_LAItemStructs.size();i++)
    {
        GTAOneClickLaunchApplicationItemStruct* pAppItem = _LAItemStructs.at(i);
        bool sKillMode = pAppItem->getKillMode();
        if (pAppItem)
        {
            QStringList optionList = pAppItem->getOptions();
            QString sOptions;
            if (!optionList.isEmpty())
            {
                QStringList optionValLst;
                foreach(QString sOption, optionList)
                {
                    QString sValue = pAppItem->getValue(sOption); 
                    optionValLst.append( QString("%1[%2]").arg(sOption,sValue));
                }

                sOptions = optionValLst.join(",");
            }
            if(!sKillMode)
                oStatment.append(QString("\n%1/%2(%3)").arg(pAppItem->getConfigFile(),pAppItem->getAppName(),sOptions));
            else
                oStatment.append(QString("\n%1/%2_%3(%4)").arg(pAppItem->getConfigFile(),pAppItem->getAppName(),"KillApp",sOptions));
        }
    }
    /*/QString oStatment = QString("%1 %2 %3 %4").arg(getAction(),getComplement(),getConfigFile(), getAppName());
    return oStatment;*/
    return oStatment;
}
void GTAOneClickLaunchApplication::setParent(GTACommandBase* iParent)
{
    _parent = iParent;
}
GTACommandBase* GTAOneClickLaunchApplication::getParent(void) const
{
    return _parent;
}
QList<GTACommandBase*>& GTAOneClickLaunchApplication::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAOneClickLaunchApplication::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    /************************************************************************
     to supress wrning                                                      */               
    
    pBase = NULL;
    idPos;
    /************************************************************************/
    return false;
}
GTACommandBase * GTAOneClickLaunchApplication::getClone()const
{
    return (new  GTAOneClickLaunchApplication(*this));
}
void GTAOneClickLaunchApplicationItemStruct::setAppItem(const QString &)
{

}
QString GTAOneClickLaunchApplicationItemStruct::getAppItem() const
{
    return QString();
}



bool GTAOneClickLaunchApplication::canHaveChildren() const
{
    return false;
}

QStringList GTAOneClickLaunchApplication::getVariableList() const
{
    return QStringList();

}

void GTAOneClickLaunchApplication:: ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    iTagValueMap;//to suppress warning
    //do nothing
}
bool GTAOneClickLaunchApplication::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
bool GTAOneClickLaunchApplication::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTAOneClickLaunchApplication::getDumpList()const
{
    return _dumpList;

}

void GTAOneClickLaunchApplication::insertLAStruct( GTAOneClickLaunchApplicationItemStruct * iLAStruct)
{
    _LAItemStructs.append(iLAStruct);
}
void GTAOneClickLaunchApplication::insertLAStruct( QList<GTAOneClickLaunchApplicationItemStruct *> iLstLAStruct)
{
    _LAItemStructs.append(iLstLAStruct);
}
QList<GTAOneClickLaunchApplicationItemStruct *> GTAOneClickLaunchApplication::getLaStruct() const
{
    return _LAItemStructs;
}
QString GTAOneClickLaunchApplication::getSCXMLStateName()const
{
    return getStatement();

}

QString GTAOneClickLaunchApplication::getApplicationName()
{
	GTAOneClickLaunchApplicationItemStruct * pOneClickLaunchApplicationItemStructObj = _LAItemStructs.at(0);
	QString appName = pOneClickLaunchApplicationItemStructObj->getConfigFile();
	return appName;
}