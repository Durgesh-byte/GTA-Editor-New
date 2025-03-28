#include "GTAEquationBase.h"

GTAEquationBase::GTAEquationBase(EquationType iEquationType) : GTACommandBase(GTACommandBase::EQUATION)
{
    _EquationType = iEquationType;
}

//GTAEquationBase::GTAEquationBase(const GTAEquationBase &Rhs)
//{
//    _EquationType = Rhs.getEquationType();
//    sIsSolid = Rhs.getSolidState();
//    sIsContinous = Rhs.getContinousState();
//}

GTAEquationBase::EquationType  GTAEquationBase::getEquationType() const
{
    return _EquationType;
}
GTAEquationBase::~GTAEquationBase()
{

}

void GTAEquationBase::setParent(GTACommandBase* iParent)
{
    _parent = iParent;
}

GTACommandBase* GTAEquationBase::getParent(void) const
{
    return _parent;
}

QList<GTACommandBase*>& GTAEquationBase::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAEquationBase::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
    return false;
}
QColor GTAEquationBase::getForegroundColor() const
{
    return QColor(0,0,0);
}
QColor GTAEquationBase::getBackgroundColor() const
{
     return QColor();
}
QFont GTAEquationBase::getFont() const
{
    return QFont();
}
bool GTAEquationBase::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAEquationBase::getVariableList() const
{
    return QStringList();
}
void GTAEquationBase:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{

    iTagValueMap;//to suppress warning
}

void GTAEquationBase::stringReplace(const QRegExp&, const QString&)
{
}

bool GTAEquationBase::equipmentReplace(const QRegExp&, const QString&)
{
    return false;
}

bool GTAEquationBase::searchString(const QRegExp&,bool) const
{
    bool rc = false;

    return rc;
}
