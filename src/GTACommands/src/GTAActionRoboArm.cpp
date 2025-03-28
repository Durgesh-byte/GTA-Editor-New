#include "GTAActionRoboArm.h"

GTAActionRoboArm::GTAActionRoboArm(const QString & iActionName, const QString & iComplement, const QString & iFunctionName) :
    GTAActionBase(iActionName,iComplement)
{
	setFunction(iFunctionName);
     
}
GTAActionRoboArm::GTAActionRoboArm(const GTAActionRoboArm & iCmd):GTAActionBase(iCmd)
{
    this->setFunction(iCmd.getFunction());
  /*  this->setAction(iCmd.getAction());
    this->setComplement(iCmd.getComplement());
    this->setCommandType(iCmd.getCommandType());*/
    this->setSequenceFile(iCmd.getSequenceFile());
    this->setFunction(iCmd.getFunction());
    this->setFunctionCode(iCmd.getFunctionCode());
    this->setActionOnFail(iCmd.getActionOnFail());
   /* QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

    iCmd.getPrecision(sPrecision,sPrecisionUnit);
    setPrecision(sPrecision,sPrecisionUnit);

    iCmd.getTimeOut( sTimeOut,sTimeOutUnit);
    setTimeOut( sTimeOut,sTimeOutUnit);
    this->setComment(iCmd.getComment());*/
}
GTAActionRoboArm::~GTAActionRoboArm()
{
    
}
void GTAActionRoboArm::setSequenceFile(const QString & iSequenceFile)
{
    _SequenceFile = iSequenceFile;
}

QString GTAActionRoboArm::getSequenceFile() const
{
    return _SequenceFile;
}
void GTAActionRoboArm::setFunction(const QString & iFunctionName)
{
    _FunctionName = iFunctionName;
}

QString GTAActionRoboArm::getFunction() const
{
    return _FunctionName;
}

QString GTAActionRoboArm::getStatement() const
{
    QString oStatment = QString("%1 %2 %3").arg(getAction(),getComplement(),getFunction());
    return oStatment;
}

void GTAActionRoboArm::setParent(GTACommandBase* iParent)
{
	_parent = iParent;
}
GTACommandBase* GTAActionRoboArm::getParent(void) const
{
	return _parent;
}
QList<GTACommandBase*>& GTAActionRoboArm::getChildren(void) const
{
	return *_lstChildren;
}
bool GTAActionRoboArm::insertChildren(GTACommandBase*, const int&)
{
	return false;
}
void GTAActionRoboArm::setFunctionCode(const QByteArray & iCode)
{
    _XmlRpcFunction = iCode;
}
QByteArray GTAActionRoboArm::getFunctionCode() const
{
    return _XmlRpcFunction;
}
//void GTAActionRoboArm::setFunctionCode(const QList<QDomNode> & iCode)
//{
//    _XmlRpcFunctionList = iCode;
//}


//QList<QDomNode> GTAActionRoboArm::getFunctionCode() const
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
GTACommandBase * GTAActionRoboArm::getClone() const
{
    return (new  GTAActionRoboArm(*this));
}
bool GTAActionRoboArm::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionRoboArm::getVariableList() const
{
    return QStringList();

}

void GTAActionRoboArm:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
    //do nothing
}
bool GTAActionRoboArm::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
QString GTAActionRoboArm::getSCXMLStateName()const
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
