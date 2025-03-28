#include "GTAActionManual.h"
#include "GTAUtil.h"
GTAActionManual::GTAActionManual()
{
    _Message = "";
    _AcknowledgeMent = false;
    _UserFeedback = false;
    _ReturnType = "";
    _TargetMachine = "";
    _Parameter = "";
    _IsLocalParam = true;
}
GTAActionManual::~GTAActionManual()
{

}
GTAActionManual::GTAActionManual(const GTAActionManual& rhs):GTAActionBase(rhs)
{
    //from GTACommandBase
  /* setCommandType(rhs.getCommandType());*/
    //this->setParent(NULL);//This should be set based on paste

    //from GTAActionBase

   /*setAction(rhs.getAction());
   setComplement(rhs.getComplement());
	
	QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

	rhs.getPrecision(sPrecision,sPrecisionUnit);
	setPrecision(sPrecision,sPrecisionUnit);

	rhs.getTimeOut( sTimeOut,sTimeOutUnit);
	setTimeOut( sTimeOut,sTimeOutUnit);

   setActionOnFail(rhs.getActionOnFail());
 */   //from GTAActionIF
   setParameter(rhs.getParameter());
   setMessage(rhs.getMessage());
   setAcknowledgement(rhs.getAcknowledgetment());
   setUserFeedback(rhs.getUserFeedback());
   setReturnType(rhs.getReturnType());
   setTargetMachine(rhs.getTargetMachine());
   setParameter(rhs.getParameter());
   //setComment(rhs.getComment());
   setImageName(rhs.getImageName());
   setHTMLMessage(rhs.getHtmlMessage());
    //QList<GTACommandBase*> lstChildrens = rhs.getChildrens();
    //for (int i=0;i<lstChildrens.size();++i)
    //{
    //	GTACommandBase* pCurrentChild = lstChildrens.at(i);
    //	GTACommandBase* pClone = NULL;
    //	if (pCurrentChild!=NULL)
    //		pClone=pCurrentChild->getClone();
    //	this->insertChildren(pClone,i);
    //}


}
GTACommandBase* GTAActionManual::getClone() const
{
    GTACommandBase* pBase = new GTAActionManual(*this);
    return pBase;
}
GTAActionManual::GTAActionManual(const QString &iAction):GTAActionBase(iAction,QString())
{
    _Message = "";
    _AcknowledgeMent = false;
    _UserFeedback = false;
    _ReturnType = "";
    _TargetMachine = "";
    _Parameter = "";
    _IsLocalParam = true;
}

void GTAActionManual::setHTMLMessage(const QString & iHTMLMessage)
{
    QString HTMLMessage = iHTMLMessage;
	//removing DTD from generated html
	//allowed only by uscxml scheduler
    int idx = HTMLMessage.indexOf("<html>");
    if (idx >=0)
    {
        HTMLMessage.remove(0,idx);
        HTMLMessage.replace("'","");
    }
    _htmlMessage = HTMLMessage;
}

QString GTAActionManual::getHtmlMessage()const
{
    return _htmlMessage;
}

void GTAActionManual::setMessage(const QString & iMessage)
{
    _Message = iMessage;
}

QString GTAActionManual::getMessage() const
{
    return _Message;
}

void GTAActionManual::setAcknowledgement(bool iVal)
{
    _AcknowledgeMent = iVal;
}

bool GTAActionManual::getAcknowledgetment() const
{
    return _AcknowledgeMent;
}

void GTAActionManual::setUserFeedback(bool iVal)
{
    _UserFeedback = iVal;
}

bool GTAActionManual::getUserFeedback() const
{
    return _UserFeedback;
}

void GTAActionManual::setReturnType(const QString & iVal)
{
    _ReturnType = iVal;
}

QString GTAActionManual::getReturnType() const
{
    return _ReturnType;
}

void GTAActionManual::setTargetMachine(const QString & iMachine)
{
    _TargetMachine = iMachine;
}

QString GTAActionManual::getTargetMachine() const
{
    return _TargetMachine;
}

void GTAActionManual::setParameter(const QString & iParam)
{
    _Parameter = iParam;
}

QString GTAActionManual::getParameter() const
{
    return _Parameter;
}
void GTAActionManual::setImageName(const QString & iImgName)
{
    _Image = iImgName;
}
QString GTAActionManual::getImageName() const
{
    return _Image;
}
QString GTAActionManual::getStatement() const
{
    return QString("%1 - %2").arg("ManAct",getMessage());
}
QString GTAActionManual::getSCXMLStateName()const
{
   //return QString("%1_%2_%3").arg("ManAct",_Message,_Parameter);
   QString msg = _Message;
   QStringList words = msg.split(" ",QString::SkipEmptyParts);
   if (words.size()>=5)
       msg = QString("%1_%2_%3_%4_%5").arg(words.at(0),words.at(1),words.at(2),words.at(3),words.at(4));
   
   return QString("%1_%2").arg("ManAct",msg);

}

QString GTAActionManual::getLTRAStatement()  const
{
    QString msg = getMessage();

    QStringList items = msg.split("");
    QStringList items1;
    for(int i=0;i<items.count();i++)
    {
        if(!QString(items.at(i).trimmed()).isEmpty())
            items1.append(items.at(i).trimmed());
        else
            items1.append("|");
    }
    QString newMsg = items1.join("");
    newMsg.replace("||","|");
    newMsg.replace("|"," ");
	newMsg.replace("   ","\n");
    return QString("#c#%1#c# - %2").arg("MANUAL ACTION",newMsg);
}

QList<GTACommandBase*>& GTAActionManual::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionManual::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

void  GTAActionManual::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionManual::getParent(void) const
{
    return _parent;
}
bool GTAActionManual::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionManual::getVariableList() const
{
    QStringList lstOfVars;
    if (!_Parameter.isEmpty() && GTACommandBase::isVariable(_Parameter))
    {
        lstOfVars.append(_Parameter);
    }
    //GTACommandBase::collectVariableNames(_Parameter,lstOfVars);
    return lstOfVars;

}

void GTAActionManual:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _Parameter.replace(tag,iterTag.value());
    }
}

void GTAActionManual:: stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
   /* _Title.replace(iStrToFind,iStringToReplace,iCaseSensitive);
    _Message.replace(iStrToFind,iStringToReplace,iCaseSensitive) ;*/
    _Parameter.replace(iStrToFind,iStringToReplace);
    
}
bool GTAActionManual::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QString temp = _Parameter;
    bool rc = false;
    if (temp.count('.') == 2) // triplet detection
    {
        temp.chop(temp.size() - temp.indexOf('.'));
        if (temp.contains(iStrToFind))
        {
            rc = true;
            temp.replace(iStrToFind, iStringToReplace);
            _Parameter.remove(0, _Parameter.indexOf('.'));
            _Parameter.prepend(temp);
        }
    }
    return rc;
}
bool GTAActionManual::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;
    if( _Parameter.contains(iRegExp) )
      rc = true;
    else if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;
    return rc;
}
