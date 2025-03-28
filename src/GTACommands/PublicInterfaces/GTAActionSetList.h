#ifndef GTAACTIONSETLIST_H
#define GTAACTIONSETLIST_H
#include "GTAActionBase.h"
class GTAActionSet;
class  GTACommands_SHARED_EXPORT GTAActionSetList : public GTAActionBase
{
public:
    GTAActionSetList();
    GTAActionSetList(const GTAActionSetList& rhs);
    GTAActionSetList(const QString &iAction, const QString & iComplement);
   
    virtual QString getStatement() const;
    virtual QString getLTRAStatement() const;

    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;

    bool addSetAction(GTAActionSet* ipSetAction);
    bool deleteSetAction(const int& iIndex);
    bool getSetAction(const int& iIndex,GTAActionSet*& opSetAction) const;
    int  getSetActionCount() const;

    void setSetCommandlist(  QList <GTAActionSet*> ipActionSetList){_setActionList = ipActionSetList;}
    void getSetCommandlist(QList <GTAActionSet*>& opActionSetList)const{opActionSetList=_setActionList;}

    void setVmacForceType(const QString & iVmacForceType);
    QString getVmacForceType() const;
    virtual bool canHaveChildren() const;
   
    virtual QStringList getVariableList() const;
    virtual void ReplaceTag(const QMap<QString,QString> & iTagValueMap);
    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;


    QString getSCXMLStateName() const;
    bool hasTimeOut() const{return true;}
    bool hasPrecision() const{return false;}
    bool hasChannelInControl() const;
    virtual bool isCompoundState(){return true;}
    virtual QString getCompounsStateId(){return QString("UserDef_SetList");}

    void setLastEditedRow(const int iRow) { _lastEditedRow = iRow; };
    int getLastEditedRow() const { return _lastEditedRow; };

private:
    QList <GTAActionSet*>  _setActionList;
    QString _VmacForceType;
    int   _lastEditedRow = 0;
};

#endif // GTAActionSetList_H
