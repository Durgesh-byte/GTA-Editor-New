#ifndef GTAACTIONTITLE_H
#define GTAACTIONTITLE_H
#include "GTAActionBase.h"
#include "GTACommands.h"
#include "GTAUtil.h"

class GTACommands_SHARED_EXPORT GTAActionTitle : public GTAActionBase
{
public:
    GTAActionTitle(const QString &iAction);
    GTAActionTitle(const GTAActionTitle& iObjTitle);
    virtual ~GTAActionTitle();
    inline void setTitle(const QString & iTitle){_Title = iTitle;}
    inline void setComment(const QString & iComment){_Comment = iComment;}
    inline void setImageName(const QString & iImageName){_ImageName = iImageName;}

    inline QString getTitle() const {return _Title;}
    inline QString getComment() const {return _Comment;}
    inline QString getImageName() const {return _ImageName;}

    virtual QString getStatement() const;

    //implemented methods of command base
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual QList<GTACommandBase*>& getChildren(void) const;

    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);

    virtual int getAllChildrenCount(void);

    virtual GTACommandBase*getClone() const;

    //visu properry
    QColor getForegroundColor() const;
    QColor getBackgroundColor() const;
    QFont getFont() const;
    virtual bool canHaveChildren() const;

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    virtual bool isTitle(){return true;}

    bool logCanExist() const{return false;}
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;

    bool hasTimeOut() const{return false;}
    bool hasPrecision() const{return false;}
     bool hasChannelInControl()const {return false;}


protected:
    QString _Title;
    QString _Comment;
    QString _ImageName;
};

#endif // GTAACTIONTITLE_H
