#ifndef GTAActionOneClickSetList_H
#define GTAActionOneClickSetList_H
#include "GTAActionBase.h"
#include "GTAActionOneClickSet.h"

class  GTACommands_SHARED_EXPORT GTAActionOneClickSetList : public GTAActionBase
{
public:
    GTAActionOneClickSetList();
    GTAActionOneClickSetList(const GTAActionOneClickSetList& rhs);
    GTAActionOneClickSetList(const QString &iAction, const QString & iComplement);
   
   
   
    virtual QString getStatement() const;

    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;

    bool addSetAction(GTAActionOneClickSet* ipSetAction);
    bool deleteSetAction(const int& iIndex);
    bool getSetAction(const int& iIndex,GTAActionOneClickSet*& opSetAction) const;
    int  getSetActionCount() const;

    void setSetCommandlist(  QList <GTAActionOneClickSet*> ipActionSetList){_setActionList = ipActionSetList;}
    void getSetCommandlist(QList <GTAActionOneClickSet*>& ipActionSetList)const{ipActionSetList=_setActionList;}

    void setVmacForceType(const QString & iVmacForceType);
    QString getVmacForceType() const;
    virtual bool canHaveChildren() const;
   
    virtual QStringList getVariableList() const;
    virtual void ReplaceTag(const QMap<QString,QString> & iTagValueMap);
    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;

     bool isOneClick(){return true;}

    QString getSCXMLStateName() const;
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const{return false;}

private:
    QList <GTAActionOneClickSet*>  _setActionList;
    QString _VmacForceType;
};

#endif // GTAActionOneClickSetList_H
