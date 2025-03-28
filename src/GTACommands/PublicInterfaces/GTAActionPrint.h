#ifndef GTAACTIONPRINT_H
#define GTAACTIONPRINT_H
#include "GTAActionBase.h"
class GTACommands_SHARED_EXPORT GTAActionPrint : public GTAActionBase
{
public:
    enum PrintType{PARAMETER,MESSAGE,FWC_BUFFER,BITE_BUFFER};
    GTAActionPrint();
    GTAActionPrint(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    GTAActionPrint(const GTAActionPrint& rhs);
    void setValues(const QStringList & ipValueList);
    QStringList getValues() const;

    inline void setMessageType(const QString & iMessage){_MessageType = iMessage;}
    QString getMessageType()const {return _MessageType;}

    void setPrintType(PrintType  iType);
    PrintType getPrintType() const;

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
//    void getTrimmedStatement(QString &val);

private:
    QStringList _PrintValues;
    PrintType _PrintType;
    QString _MessageType;
};

#endif // GTAACTIONPRINT_H
