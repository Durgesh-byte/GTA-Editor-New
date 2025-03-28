#include "GTAComment.h"

GTAComment::GTAComment(CommentType iType ): GTACommandBase(GTACommandBase::COMMENT)
{
	setCommentType(iType);
}
GTAComment::~GTAComment()
{

}
GTAComment::GTAComment(const GTAComment& rhs):GTACommandBase(GTACommandBase::COMMENT)
{
	this->setCommentType(rhs.getCommentType());
	this->setValue(rhs.getValue());
}
void GTAComment::setCommentType(CommentType iType)
{
	_CommentType = iType;
}

GTAComment::CommentType GTAComment::getCommentType()const
{
	return _CommentType;
}

void GTAComment::setValue(const QString & iVal)
{
	_Commment = iVal;
}

QString GTAComment::getValue() const
{
	return _Commment;
}
QString GTAComment::getStatement() const
{
	QString commTypeStr;
	switch(_CommentType)
	{
	case TITLE: commTypeStr = "Title"; break;
	case COMMENT: commTypeStr = "Comment"; break;
	case PICTURE: commTypeStr = "Picture"; break;
	}

	QString oComment = QString("add %1 - %2").arg(commTypeStr,getValue());
	return oComment;
}

 
 QList<GTACommandBase*>& GTAComment::getChildren(void) const
 {
 	return *_lstChildren;
 }
 bool GTAComment::insertChildren(GTACommandBase* pBase,const int& idPos)
 {
     pBase;idPos;//for supressing warning.
 	return false;
 }
 
 void  GTAComment::setParent(GTACommandBase*)
 {
 	_parent=NULL;
 }
 GTACommandBase* GTAComment::getParent(void) const
 {
 	return _parent;
 }
GTACommandBase* GTAComment::getClone()const
{
	return new GTAComment(*this);
}

QColor GTAComment::getForegroundColor() const
{
    return QColor(0,0,0);
}
QColor GTAComment::getBackgroundColor() const
{
     return QColor();
}
QFont GTAComment::getFont() const
{
    return QFont();
}
bool GTAComment::canHaveChildren() const
{
    return false;
}
QStringList GTAComment::getVariableList() const
{
    return QStringList();
}
void GTAComment:: ReplaceTag(const QMap<QString,QString>& iTagValueMap) 
{
    iTagValueMap;//to suppress warning

}