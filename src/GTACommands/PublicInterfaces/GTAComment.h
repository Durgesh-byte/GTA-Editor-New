#ifndef GTACOMMENT_H
#define GTACOMMENT_H
#include "GTACommands.h"
#include "GTACommandBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAComment : public GTACommandBase
{
public:
    enum CommentType{TITLE,COMMENT,PICTURE};
    GTAComment(CommentType iType = TITLE);
	GTAComment(const GTAComment& rhs);
	virtual ~GTAComment();

    void setCommentType(CommentType iType);
    CommentType getCommentType()const;

    void setValue(const QString & iVal);
    QString getValue() const;

    virtual QString getStatement() const;


	virtual QList<GTACommandBase*>& getChildren(void) const;
	virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
	virtual void setParent(GTACommandBase* iParent);
	virtual GTACommandBase* getParent(void) const;

	GTACommandBase* getClone()const;
	
     //Visual Property
     virtual QColor getForegroundColor() const;
     virtual QColor getBackgroundColor() const;
     virtual bool isForegroundColor() const { return false; };
     virtual QFont getFont() const;
     bool canHaveChildren() const;
	
     virtual QStringList getVariableList() const;

     void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

     bool logCanExist() const{return false;}
      QString getCommandTypeForDisplay() const {return QString("Comment");}
      bool hasChannelInControl()const {return false;}
     
private:
    QString _Commment;
    CommentType _CommentType;

};

#endif // GTACOMMENT_H
