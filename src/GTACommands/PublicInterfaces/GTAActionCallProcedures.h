#ifndef GTAACTIONCALLPROCEDURES_H
#define GTAACTIONCALLPROCEDURES_H
#include "GTAActionBase.h"

#pragma warning(push, 0)
#include <QMap>
#pragma warning(pop)

struct GTACommands_SHARED_EXPORT GTACallProcItem 
{
    enum call_type{PROCEDURE,FUNCTION};
    call_type               _type;
    QString                 _elementName;
    QMap<QString,QString>  _tagValue;
    QString _UUID;

    GTACallProcItem(call_type iItemType, const QString& iElemName, const QMap<QString,QString>& itagValue,const QString &iUUID);
    GTACallProcItem(const GTACallProcItem& iRhs);

};
class GTACommands_SHARED_EXPORT GTAActionCallProcedures : public GTAActionBase
{
public:
    enum PrintType{PARAMETER,MESSAGE,FWC_BUFFER,BITE_BUFFER};
    GTAActionCallProcedures();
    GTAActionCallProcedures(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    GTAActionCallProcedures(const GTAActionCallProcedures& rhs);
    void setCallProcedures(const QList<GTACallProcItem>& icallProcedures);
    QList<GTACallProcItem> getCallProcedures() const;

    inline void setParallelExecution(bool iPrallelExecution){_parallelExecution = iPrallelExecution;}
    bool getPrallelExecution()const {return _parallelExecution;}


    bool getCallProcedure(const int& index, GTACallProcItem::call_type& oType, QString& oElemName, QMap<QString,QString>&  oTtagValue,QString &oUUID)const;
    void appendCallProcedure(const GTACallProcItem::call_type& iType, const QString& iElemName,const QMap<QString,QString>&  oTtagValue,const QString &iUUID);
    int procedureCount(){return _callProcedures.size();}

    virtual QString getStatement() const;

    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;
    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    bool hasPrecision() const{return  false ;}
    bool hasTimeOut()const {return false;}

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    QString getSCXMLStateName()const;
    bool hasChannelInControl()const {return false;}
    QStringList getProcedureNames()const;
    void updateVariableList(QStringList& iVarList);

    /**
      * 
      * The function returns statement for LTRA
      * @return: processed statement with all call items with their tag variables
      */
    QString getLTRAStatement() const;

    /**
      * 
      * The provides the status of the command based on status of all call items
      * @return: OK/KO/NA based on result of children
      */
    QString getGlobalResultStatus();

private:
    //QStringList _callProcedures;
    QList<GTACallProcItem> _callProcedures;
    bool _parallelExecution;
    QStringList _lstOfVars;

  
};

#endif // GTAActionCallProcedures_H
