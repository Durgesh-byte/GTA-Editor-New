#include "GTAFailureProfileBase.h"

GTAFailureProfileBase::GTAFailureProfileBase(EquationType iEquationType) : GTACommandBase(GTACommandBase::FAILPROFILE)
{
    _EquationType = iEquationType;
}

GTAFailureProfileBase::EquationType  GTAFailureProfileBase::getEquationType() const
{
    return _EquationType;
}
GTAFailureProfileBase::~GTAFailureProfileBase()
{

}
void GTAFailureProfileBase::setParent(GTACommandBase* iParent)
{
    _parent = iParent;
}

GTACommandBase* GTAFailureProfileBase::getParent(void) const
{
    return _parent;
}

QList<GTACommandBase*>& GTAFailureProfileBase::getChildren(void) const
{
    return *_lstChildren;
}

bool GTAFailureProfileBase::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    pBase;idPos;//for supressing warning.
    return false;
}
QColor GTAFailureProfileBase::getForegroundColor() const
{
    return QColor(0,0,0);
}
QColor GTAFailureProfileBase::getBackgroundColor() const
{
     return QColor();
}
QFont GTAFailureProfileBase::getFont() const
{
    return QFont();
}
bool GTAFailureProfileBase::canHaveChildren() const
{
    return false;//true;
}
QStringList GTAFailureProfileBase::getVariableList() const
{
    return QStringList();
}
void GTAFailureProfileBase:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{

    iTagValueMap;//to suppress warning
}