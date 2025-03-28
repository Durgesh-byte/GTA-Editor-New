#ifndef GTAACTIONSUBSCRIBE_H
#define GTAACTIONSUBSCRIBE_H

#include "GTAActionBase.h"

class GTACommands_SHARED_EXPORT GTAActionSubscribe : public GTAActionBase
{
    QStringList _ParamList;
public:
    GTAActionSubscribe();
    GTAActionSubscribe(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    GTAActionSubscribe(const GTAActionSubscribe& rhs);
    void setValues(const QStringList & ipValueList);
    QStringList getValues() const;

    virtual QString getStatement() const;

    virtual QString getLTRAStatement()  const;

    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;
    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    QString getSCXMLStateName()const;
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const;
};

#endif // GTAACTIONSUBSCRIBE_H
