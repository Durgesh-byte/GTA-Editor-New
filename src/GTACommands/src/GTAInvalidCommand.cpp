#include "GTAInvalidCommand.h"
#include "GTAUtil.h"
GTAInvalidCommand::GTAInvalidCommand()
{
    setAction(ACT_INVALID);
    _parent = NULL;
       /* setTimeOut("0",ACT_TIMEOUT_UNIT_SEC);
        setPrecision("0",ACT_PRECISION_UNIT_VALUE);
        setActionOnFail(ACT_FAIL_STOP);*/
    _createState=false;
}
GTAInvalidCommand::~GTAInvalidCommand()
{

}
QString GTAInvalidCommand::getStatement() const
{
    return QString("ERROR-[INVALID CMD]:%1").arg(_Message);
}
QList<GTACommandBase*>& GTAInvalidCommand::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAInvalidCommand::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
    return false;
}
void GTAInvalidCommand::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAInvalidCommand::getParent(void) const
{
    return _parent;
}
GTAInvalidCommand::GTAInvalidCommand(const GTAInvalidCommand& rhs)
{
    //from GTACommandBase
    this->setCommandType(rhs.getCommandType());
    //this->setParent(NULL);//This should be set based on paste

    //from GTAActionBase
    this->setAction(rhs.getAction());
    this->setComplement(rhs.getComplement());
	QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

	rhs.getPrecision(sPrecision,sPrecisionUnit);
	setPrecision(sPrecision,sPrecisionUnit);

	rhs.getTimeOut( sTimeOut,sTimeOutUnit);
	setTimeOut( sTimeOut,sTimeOutUnit);
    setActionOnFail(rhs.getActionOnFail());
    setComment(rhs.getComment());
    setMessage(rhs.getMessage());
    _createState = rhs.getStateExists();

}
GTACommandBase* GTAInvalidCommand::getClone() const
{
    GTACommandBase* pBase = new GTAInvalidCommand(*this);
    return pBase;
}
bool GTAInvalidCommand::canBeCopied()
{
    return false;
}
bool GTAInvalidCommand::IsDeletable()
{
    return false;
}
bool GTAInvalidCommand:: IsUserEditable()
{
    return false;

}
bool GTAInvalidCommand::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAInvalidCommand::getVariableList() const
{
    return QStringList();

}

void GTAInvalidCommand:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
    //do nothing
}
bool GTAInvalidCommand::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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

bool GTAInvalidCommand::logCanExist()const
{
    return false;
}
void GTAInvalidCommand::setMessage(const QString& iMessage)
{
    _Message=iMessage;
}
QString GTAInvalidCommand::getMessage()const
{
    return _Message;
}
QColor GTAInvalidCommand::getForegroundColor() const
{
    return QColor(255,0,0,180);
}
QColor GTAInvalidCommand::getBackgroundColor() const
{
    return QColor(128,0,0,50);
}
QFont GTAInvalidCommand::getFont() const
{
    QFont oFont;
    oFont.setBold(true);
    oFont.setWeight(QFont::Bold);
    return oFont;
}
