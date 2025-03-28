#include "GTAActionTitle.h"

GTAActionTitle::GTAActionTitle(const QString &iAction) : GTAActionBase(iAction,QString())
{
    _Title = QString();
    _Comment = QString();
    _ImageName = QString();
}
GTAActionTitle::GTAActionTitle(const GTAActionTitle& iObjTitle):GTAActionBase(iObjTitle)
{
    _Title = QString();
    _Comment = QString();
    _ImageName = QString();

    /*this->setAction(iObjTitle.getAction());
    this->setComplement(iObjTitle.getComplement());
    this->setActionOnFail(iObjTitle.getActionOnFail());

	QString sTimeOut,sTimeOutUnit,sPrecision,sPrecisionUnit;

	iObjTitle.getPrecision(sPrecision,sPrecisionUnit);
    this->setPrecision(sPrecision,sPrecisionUnit);
	
	iObjTitle.getTimeOut( sTimeOut,sTimeOutUnit);
    this->setTimeOut( sTimeOut,sTimeOutUnit);
    this->setCommandType(this->getCommandType());*/

    setTitle(iObjTitle.getTitle());
    setComment(iObjTitle.getComment());
    setImageName(iObjTitle.getImageName());
}

GTAActionTitle::~GTAActionTitle()
{
    _Title = QString();
    _Comment = QString();
    _ImageName = QString();
}

QString GTAActionTitle::getStatement() const
{
    QString oStatement =  getTitle();//QString("[Title] %1").arg(getTitle());
    return oStatement;
}

void GTAActionTitle::setParent(GTACommandBase* iParent)
{
    _parent = iParent;
}

GTACommandBase* GTAActionTitle::getParent(void) const
{
    return _parent;
}

QList<GTACommandBase*>& GTAActionTitle::getChildren(void) const
{
    return *_lstChildren;
}

bool GTAActionTitle::insertChildren(GTACommandBase*, const int&)
{
    return false;
}

int GTAActionTitle::getAllChildrenCount(void)
{
    return 0;
}


GTACommandBase* GTAActionTitle::getClone()const
{
    return (new GTAActionTitle(*this));
}

QColor GTAActionTitle::getForegroundColor() const
{
    return TITLE_EDITOR_FORE_COLOR;
}
QColor GTAActionTitle::getBackgroundColor() const
{
    return TITLE_EDITOR_BACK_COLOR;
}
QFont GTAActionTitle::getFont() const
{
    QFont oFont;
    oFont.setBold(true);
    oFont.setWeight(QFont::Bold);
    return oFont;
}
bool GTAActionTitle::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAActionTitle::getVariableList() const
{
    return QStringList();

}

void GTAActionTitle:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning
    //do nothing
}


bool GTAActionTitle::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
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
