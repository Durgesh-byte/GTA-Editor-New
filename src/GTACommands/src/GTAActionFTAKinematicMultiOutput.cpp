#include "GTAActionFTAKinematicMultiOutput.h"
#include "GTAUtil.h"
#include "GTAActionCall.h"

GTAActionFTAKinematicMultiOutput::GTAActionFTAKinematicMultiOutput()
{
    setAction(ACT_FCTL);
    setComplement(COM_FCTL_KINEMATIC_MULTI_OUTPUT);
    _globalResult = "NA";
    _PrintTableCmdList.clear();
}

GTAActionFTAKinematicMultiOutput::GTAActionFTAKinematicMultiOutput(const GTAActionFTAKinematicMultiOutput& rhs):GTAActionBase(rhs)
{
    _PrintTableCmdList.clear();
  /*  setAction(rhs.getAction());
    setComplement(rhs.getComplement());
    setComment(rhs.getComment());*/
    setEpsilon1(rhs.getEpsilon1());
    setEpsilon2(rhs.getEpsilon2());
    setNoOfLines(rhs.getNoOfLines());
    setWaitPilOrder(rhs.getWaitPilOrder());
    setRefFilePath(rhs.getRefFilePath());
    setRefFileData(rhs.getRefFileData());
    setCurrentName(rhs.getCurrentName());
    setGlobalResultStatus(rhs.getGlobalResultStatus());
    QStringList sInputItems = rhs.getInputVariableList();
    for(int i=0;i<sInputItems.count();i++)
    {
        QString sVar = rhs.getInputVariable(i);
        insertInputVariable(i,sVar);
    }

    QStringList sOutputItems = rhs.getOutputVariableList();
    for(int i=0;i<sOutputItems.count();i++)
    {
        QString sVar = rhs.getOutputVariable(i);
        insertMonitoredVariable(i,sVar);
    }
    QList<GTAActionPrintTable*> sPrintTableList = rhs.getPrintTableCmdList();
    for(int i=0;i<sPrintTableList.count();i++)
    {
        GTAActionPrintTable *pCmd = sPrintTableList.at(i);
        insertPrintTableCmd(pCmd);
    }

    QMap<QString, QString> tagValuePair = rhs.getTagValuePairForTemapltes();
    for(int i=0;i<tagValuePair.keys().count();i++)
    {
        QString tag = tagValuePair.keys().at(i);
        QString sVar = tagValuePair.value(tag);
        insertTagValuepairForTemplates(tag,sVar);
    }

}

QString GTAActionFTAKinematicMultiOutput::getRefFilePath() const
{
    return _RefFilePath;
}
void GTAActionFTAKinematicMultiOutput::setRefFilePath(const QString &iRefFilePath)
{
    _RefFilePath = iRefFilePath;
}
void GTAActionFTAKinematicMultiOutput::setRefFileData(const QStringList &iRefFileData)
{
    _RefFileData = iRefFileData;
}
QStringList GTAActionFTAKinematicMultiOutput::getRefFileData() const
{
    return _RefFileData;
}

void GTAActionFTAKinematicMultiOutput::setEpsilon1(const QString & ipVar)
{

    _Epsilon1=ipVar; ;

}
void GTAActionFTAKinematicMultiOutput::setEpsilon2(const QString & ipVar)
{
    _Epsilon2 =ipVar;
    
}
void GTAActionFTAKinematicMultiOutput::setNoOfLines(const QString & ipVar)
{
    _NoOfLines = ipVar ;

    
}
void GTAActionFTAKinematicMultiOutput::setWaitPilOrder(const QString & ipVar)
{   
    _WaitPILOrder =ipVar;
}
void GTAActionFTAKinematicMultiOutput::insertInputVariable(const int& order, const QString& iInpt)
{
    if(!iInpt.isEmpty())
        _mapInputVars.insert(order,iInpt);

}
void GTAActionFTAKinematicMultiOutput::insertMonitoredVariable(const int& order, const QString& iInpt)
{
    if(!iInpt.isEmpty())
        _mapMonitoredVars.insert(order,iInpt);

}
QString  GTAActionFTAKinematicMultiOutput::getEpsilon1( )const
{
    return _Epsilon1;

}
QString  GTAActionFTAKinematicMultiOutput::getEpsilon2( )const
{
    return _Epsilon2;
}
QString  GTAActionFTAKinematicMultiOutput::getNoOfLines( )const
{
    return _NoOfLines;
}
QString  GTAActionFTAKinematicMultiOutput::getWaitPilOrder( )const
{
    return _WaitPILOrder;
}

QStringList  GTAActionFTAKinematicMultiOutput::getInputVariableList() const
{
    return QStringList(_mapInputVars.values());

}
QString  GTAActionFTAKinematicMultiOutput::getInputVariable(const int& iIndex) const
{
    return _mapInputVars.value(iIndex);
}

QStringList  GTAActionFTAKinematicMultiOutput::getOutputVariableList() const
{
    return QStringList(_mapMonitoredVars.values());
}
QString  GTAActionFTAKinematicMultiOutput::getOutputVariable(const int& iIndex) const
{
    return _mapMonitoredVars.value(iIndex);

}

QString GTAActionFTAKinematicMultiOutput::getStatement() const
{
    QString sComplement= getComplement() ;
    if(sComplement == COM_FCTL_KINEMATIC_MULTI_OUTPUT)
        return  QString("FCTL Kinematic MultiOutput[%1]").arg(getCurrentName());

    return QString();
}

QList<GTACommandBase*>& GTAActionFTAKinematicMultiOutput::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionFTAKinematicMultiOutput::insertChildren(GTACommandBase* pBase,const int& idPos)
{
     pBase;idPos;//for supressing warning.
    return false;
}

void  GTAActionFTAKinematicMultiOutput::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionFTAKinematicMultiOutput::getParent(void) const
{
    return _parent;
}
GTACommandBase* GTAActionFTAKinematicMultiOutput::getClone() const
{
    return  new GTAActionFTAKinematicMultiOutput(*this);
}
bool GTAActionFTAKinematicMultiOutput::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionFTAKinematicMultiOutput::getVariableList() const
{
    QStringList lstOfVars;

    lstOfVars.append(getInputVariableList());
    lstOfVars.append(getOutputVariableList());
    int count = lstOfVars.count();

    for(int  i=count-1;i>=0;i--)
    {
        if( !GTACommandBase::isVariable(lstOfVars.at(i)))
        {
            lstOfVars.takeAt(i);
        }
    }
    return lstOfVars;
}

void GTAActionFTAKinematicMultiOutput:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));

        for (int i=0;i<_mapInputVars.size();i++)
        {
            _mapInputVars[i] = _mapInputVars[i].replace(tag,iterTag.value());
        }
        for (int i=0;i<_mapMonitoredVars.size();i++)
        {
            _mapMonitoredVars[i] = _mapMonitoredVars[i].replace(tag,iterTag.value());
        }

        _Epsilon1.replace(tag,iterTag.value());
        _Epsilon2.replace(tag,iterTag.value());
        _NoOfLines.replace(tag,iterTag.value());
        _WaitPILOrder.replace(tag,iterTag.value());

    }
}

bool GTAActionFTAKinematicMultiOutput::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;


    foreach(QString var, getVariableList())
    {
        if(var.contains(iRegExp))
            return true;
    }
    /*  if(getVariableList().contains(iRegExp))
        return true;*/


    if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;

    return rc;
}
QString GTAActionFTAKinematicMultiOutput::getSCXMLStateName()const
{
    return getStatement();

}
void GTAActionFTAKinematicMultiOutput::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    for (int i=0;i<_mapInputVars.size();i++)
    {
        _mapInputVars[i] = _mapInputVars[i].replace(iStrToFind,iStringToReplace);
    }
    for (int i=0;i<_mapMonitoredVars.size();i++)
    {
        _mapMonitoredVars[i] = _mapMonitoredVars[i].replace(iStrToFind,iStringToReplace);
    }
}

bool GTAActionFTAKinematicMultiOutput::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QMap temp = _mapInputVars;
    bool rc = false;
    for (int i = 0; i < _mapInputVars.size(); i++)
    {
        if (temp[i].count('.') == 2) // triplet detection
        {
            temp[i].chop(temp[i].size() - temp[i].indexOf('.'));
            if (temp[i].contains(iStrToFind))
            {
                rc = true;
                temp[i].replace(iStrToFind, iStringToReplace);
                _mapInputVars[i].remove(0, _mapInputVars[i].indexOf('.'));
                _mapInputVars[i].prepend(temp[i]);
            }
        }
    }
    temp = _mapMonitoredVars;
    for (int i = 0; i < _mapMonitoredVars.size(); i++)
    {
        if (temp[i].count('.') == 2) // triplet detection
        {
            temp[i].chop(temp[i].size() - temp[i].indexOf('.'));
            if (temp[i].contains(iStrToFind))
            {
                rc = true;
                temp[i].replace(iStrToFind, iStringToReplace);
                _mapMonitoredVars[i].remove(0, _mapMonitoredVars[i].indexOf('.'));
                _mapMonitoredVars[i].prepend(temp[i]);
            }
        }
    }
    return rc;
}

void GTAActionFTAKinematicMultiOutput::setCurrentName(const QString &iName)
{
    _Name = iName;
}

QString GTAActionFTAKinematicMultiOutput::getCurrentName() const
{
    return _Name;
}

void GTAActionFTAKinematicMultiOutput::insertTagValuepairForTemplates(const QString &iTag,QString &iVal)
{
    _tagValuePair.insert(iTag,iVal);
}


QMap<QString, QString > GTAActionFTAKinematicMultiOutput::getTagValuePairForTemapltes() const
{

    return _tagValuePair;
}
bool GTAActionFTAKinematicMultiOutput::createsCommadsFromTemplate()const
{
    return true;
}

void GTAActionFTAKinematicMultiOutput::setGlobalResultStatus(const QString &iResult)
{
    _globalResult = iResult;
}

QString GTAActionFTAKinematicMultiOutput::getGlobalResultStatus()  const
{
    return _globalResult;
}
void GTAActionFTAKinematicMultiOutput::insertPrintTableCmd( GTAActionPrintTable *iPrintTable)
{
    if(iPrintTable!= NULL)
    {
        _PrintTableCmdList.append(iPrintTable);
    }
}

QList<GTAActionPrintTable *> GTAActionFTAKinematicMultiOutput::getPrintTableCmdList() const
{
    return _PrintTableCmdList;
}
