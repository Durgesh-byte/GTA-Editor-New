#include "GTAActionIRT.h"

GTAActionIRT::GTAActionIRT(const QString & iActionName, const QString & iComplement, const QString & iFunctionName) :
    GTAActionBase(iActionName,iComplement)
{
	setFunction(iFunctionName);
     
}
GTAActionIRT::GTAActionIRT(const GTAActionIRT & iCmd):GTAActionBase(iCmd)
{
    this->setFunction(iCmd.getFunction());
   /* this->setAction(iCmd.getAction());
    this->setComplement(iCmd.getComplement());
    this->setCommandType(iCmd.getCommandType());*/
    this->setSequenceFile(iCmd.getSequenceFile());
    this->setFunction(iCmd.getFunction());
    this->setFunctionCode(iCmd.getFunctionCode());
   /* this->setActionOnFail(iCmd.getActionOnFail());
    QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

    iCmd.getPrecision(sPrecision,sPrecisionUnit);
    setPrecision(sPrecision,sPrecisionUnit);

    iCmd.getTimeOut( sTimeOut,sTimeOutUnit);
    setTimeOut( sTimeOut,sTimeOutUnit);
    this->setComment(iCmd.getComment());*/
}
GTAActionIRT::~GTAActionIRT()
{
    
}
void GTAActionIRT::setSequenceFile(const QString & iSequenceFile)
{
    _SequenceFile = iSequenceFile;
}

QString GTAActionIRT::getSequenceFile() const
{
    return _SequenceFile;
}
void GTAActionIRT::setFunction(const QString & iFunctionName)
{
    _FunctionName = iFunctionName;
}

QString GTAActionIRT::getFunction() const
{
    return _FunctionName;
}

QString GTAActionIRT::getStatement() const
{
    QString oStatment = QString("%1 %2 %3").arg(getAction(),getComplement(),getFunction());
    return oStatment;
}

void GTAActionIRT::setParent(GTACommandBase* iParent)
{
	_parent = iParent;
}
GTACommandBase* GTAActionIRT::getParent(void) const
{
	return _parent;
}
QList<GTACommandBase*>& GTAActionIRT::getChildren(void) const
{
	return *_lstChildren;
}
bool GTAActionIRT::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
	return false;
}
void GTAActionIRT::setFunctionCode(const QByteArray & iCode)
{
    _XmlRpcFunction = iCode;
}
QByteArray GTAActionIRT::getFunctionCode() const
{
    return _XmlRpcFunction;
}
//void GTAActionIRT::setFunctionCode(const QList<QDomNode> & iCode)
//{
//    _XmlRpcFunctionList = iCode;
//}


//QList<QDomNode> GTAActionIRT::getFunctionCode() const
//{
////    QList<QDomNode> oFunctionCode;
	
////    for(int i = 0 ; i< _XmlRpcFunctionList.count(); i++)
////    {
////		QDomNode domNode = _XmlRpcFunctionList.at(i);
////        if(! domNode.isNull())
////        {
////            QDomNode cloneNode = domNode.cloneNode(true);
////            oFunctionCode.append(cloneNode);
////        }
////    }
////    return oFunctionCode;
//    return _XmlRpcFunctionList;
//}
GTACommandBase * GTAActionIRT::getClone() const
{
    return (new  GTAActionIRT(*this));
}
bool GTAActionIRT::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionIRT::getVariableList() const
{
    return QStringList();

}

void GTAActionIRT:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
    //do nothing
}
bool GTAActionIRT::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
QString GTAActionIRT::getSCXMLStateName()const
{
    QString sAction = getAction();
    QString sFunctionName = _FunctionName;
    QDomDocument doc;
    doc.setContent(QString(_XmlRpcFunction));
    QDomNode rootNode = doc.firstChild();
    QString sFunction;
    if(!rootNode.isNull())
    {
        QDomNodeList funcNodes = rootNode.toElement().elementsByTagName("function");
        {
            if (!funcNodes.isEmpty())
                sFunction = funcNodes.at(0).toElement().attribute("name");

        }
    }



    return QString("%1_%2_%3").arg(sAction,sFunctionName,sFunction);

}
