#include "GTAActionGenericFunction.h"
#include "GTAActionElse.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

GTAActionGenericFunction::GTAActionGenericFunction():GTAActionBase()
{
    
}
GTAActionGenericFunction::GTAActionGenericFunction(const GTAActionGenericFunction& rhs):GTAActionBase(rhs)
{
  
    //from GTACommandBase
   /* setCommandType(rhs.getCommandType());*/
    setParent(NULL);//This should be set based on paste

    //from GTAActionBase
    /*setAction(rhs.getAction());
    setComplement(rhs.getComplement());
    
	QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit,confTime,confTimeUnit;

	rhs.getPrecision(sPrecision,sPrecisionUnit);
	setPrecision(sPrecision,sPrecisionUnit);

	rhs.getTimeOut( sTimeOut,sTimeOutUnit);
	setTimeOut( sTimeOut,sTimeOutUnit);

    rhs.getConfCheckTime(confTime,confTimeUnit);
    setConfCheckTime(confTime,confTimeUnit);*/

    
    QStringList dumpList = rhs.getDumpList();
    setDumpList(dumpList);
    
   /* setActionOnFail(rhs.getActionOnFail());
        
    setComment(rhs.getComment());*/
    setFunctionName(rhs.getFunctionName());

    setSCXMLText(rhs.getSCXMLText());
    setUsercondition(rhs.hasUsercondiiton());

    
    
}
GTACommandBase* GTAActionGenericFunction::getClone() const
{
    GTACommandBase* pBase = new GTAActionGenericFunction(*this);
    return pBase;
}
GTAActionGenericFunction::~GTAActionGenericFunction()
{
    
}

QString GTAActionGenericFunction::getStatement() const
{
    QString oAction;
    oAction = QString("Generic command:%1").arg(_functionName);
    return oAction;
}
QString GTAActionGenericFunction::getSCXMLStateName()const
{

    return getStatement();

}


//QStringList GTAActionGenericFunction::getVariableList() const
//{
//   //TODO:CONFIRM THE BEHAVIOUR AND IMPLEMENT
//    QStringList var;
//    /*QStringList splittedItems = _infoText.split(TAG_IDENTIFIER);
//    for (int i=0;i<splittedItems.size();i++)
//    {
//        if(i%2)
//            var.append(QString("@%1@").arg(splittedItems.at(i)));
//    }
//    return var;*/
//
//    bool startAppending = false;
//    bool stopAppending =false;
//    QString varStr;
//    for (int i=0;i<_infoText.size()-1;i++)
//    {
//        QString currentChar = _infoText.at(i);
//        QString nextChar = _infoText.at(i+1);
//        if(currentChar == "\"" || currentChar =="'")
//        {
//            if (nextChar=="@" )
//            {
//                if(startAppending == false)
//                    startAppending = true;
//                else
//                    varStr.clear();
//            }
//            
//        }
//        
//        if(currentChar =="@" )
//        {
//            if ( (nextChar=="\"" || nextChar =="'") && stopAppending ==false)
//            {
//                stopAppending = true;
//            }
//        }
//
//        if (startAppending == true && stopAppending == false)
//            varStr.append(currentChar);
//
//        if (stopAppending == true)
//        {
//            if(!varStr.isEmpty())
//            {
//                varStr.remove("\"");
//                varStr.append("@");
//                var.append(varStr);
//                varStr.clear();
//            }
//            startAppending = false;
//            stopAppending = false;
//        }
//
//       
//
//    }
//
//    var.removeDuplicates();
//    return var;
//
//}

QStringList GTAActionGenericFunction::getVariableList() const
{
    QDomDocument domDoc;
    bool rc = domDoc.setContent(_infoText); 
    QStringList vars;
    if (rc)
    {
        QDomElement rootNode = domDoc.documentElement();
        getAllAttributeValues(rootNode,vars);

    }
   
    

    return vars;

}
void GTAActionGenericFunction::getAllAttributeValues(QDomElement& iElement, QStringList& oVars)const
{
    if (!iElement.isNull())
    {
        QDomNodeList childNodes = iElement.childNodes();
    
        for ( int i=0;i<childNodes.size();i++)
        {
            QDomElement childelement = childNodes.at(i).toElement();
            QDomNamedNodeMap childAttributes= childelement.attributes();
            if (!childAttributes.isEmpty())
            {
                for (int j=0; j<childAttributes.size(); j++)
                {
                    QDomNode nodeItem = childAttributes.item(j);
                    QDomAttr attributeNode = nodeItem.toAttr();
                    if (!attributeNode.isNull())
                    {
                        QString attrVal = attributeNode.value();
                        if ( attrVal.startsWith(QString("'@")) )
                        {
                            if (attrVal.endsWith(QString("@'")))
                            {
                                attrVal.remove("'");
                                //attrVal.remove("@");
                                oVars.append(attrVal);
                            }
                        }
                        else if ( attrVal.startsWith(QString("@")) )
                        {
                            if (attrVal.endsWith(QString("@")))
                            {
                                attrVal.remove("'");
                                //attrVal.remove("@");
                                oVars.append(attrVal);
                            }
                        }
                    }
                }
            }

            getAllAttributeValues(childelement,oVars);

        }
    
    }

}
void GTAActionGenericFunction:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _infoText.replace(tag,iterTag.value());
       
    }
//    for(int i=0;i<_lstChildren->size();++i)
//    {
//        GTACommandBase* pCommand = _lstChildren->at(i);
//        if (NULL!=pCommand)
//            pCommand->ReplaceTag(iTagValueMap);
//    }
}

void GTAActionGenericFunction::stringReplace(const QRegExp&, const QString&)
{
//     _Param.replace(iStrToFind,iStringToReplace);
}
bool GTAActionGenericFunction::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
  
   if (ibTextSearch && this->getComment().contains(iRegExp))
       rc = true;
   else if (ibTextSearch && this->getStatement().contains(iRegExp))
       rc = true;
   else
       rc=false;
   return rc;
}

bool GTAActionGenericFunction::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTAActionGenericFunction::getDumpList()const
{
    return _dumpList;
    
}

bool GTAActionGenericFunction::insertChildren(GTACommandBase* pBase,const int& idPos)

{
    pBase;idPos;//for supressing warning.
    return false;
}