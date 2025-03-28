#ifndef GTAACTIONPRINTTABLE_H
#define GTAACTIONPRINTTABLE_H
#include "GTAActionBase.h"
class GTACommands_SHARED_EXPORT GTAActionPrintTable : public GTAActionBase
{
public:
   
    GTAActionPrintTable();
    GTAActionPrintTable(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    GTAActionPrintTable(const GTAActionPrintTable& rhs);
    void setValues(const QStringList & ipValueList);
    QStringList getValues() const;
    void setTableName(const QString& iName){_tableName = iName;}
    QString getTableName() const {return _tableName;}
    QString getTitleName() const {return _titleName;}
    void setTitleName(const QString &iTitleName){_titleName = iTitleName;}

    virtual QString getStatement() const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;
    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind, const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    QString getSCXMLStateName()const;
    void replaceUntagged();
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const;
      virtual QString getLTRAStatement() const;
private:
    QStringList _PrintValues;
    QString _tableName;
    QString _titleName;
   
};

#endif // GTAActionPrintTable_H
