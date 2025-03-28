#ifndef GTAACTIONGENERICFUNCTION_H
#define GTAACTIONGENERICFUNCTION_H
#include <GTAActionBase.h>

#pragma warning(push, 0)
#include <QDomDocument>
#include <QDomNamedNodeMap>
#include <QDomElement>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAActionGenericFunction : public GTAActionBase
{
public:
    GTAActionGenericFunction();
    GTAActionGenericFunction(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    virtual ~GTAActionGenericFunction();

    GTAActionGenericFunction(const GTAActionGenericFunction& rhs);


    
    QString getStatement() const;

    GTACommandBase*getClone() const;

    //bool hasElse();

    
    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;

    bool insertChildren(GTACommandBase* pBase,const int& idPos);
    bool canHaveChildren() const { return false; }
    QList<GTACommandBase*>& getChildren(void) const { return *_lstChildren; }
    GTACommandBase* getParent(void) const { return _parent; }
    bool hasParent() const { return _parent == nullptr; }
    void setParent(GTACommandBase* iParent)                      {_parent = iParent;}

    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;
    QString getSCXMLStateName()const;

    QString getSCXMLText()const{return _infoText;}
    void setSCXMLText(const QString& iText){_infoText=iText;}

    QString getFunctionName()const{return _functionName;}
    void setFunctionName(const QString& iText){_functionName=iText;}

    bool hasUsercondiiton()const {return _hasUserCondition;}
    void setUsercondition(bool iHasUserconditoin){_hasUserCondition=iHasUserconditoin;}

    bool hasPrecision() const{return true;}
    bool hasTimeOut() const{return true;}
    bool hasChannelInControl()const {return false;}
    bool hasConfirmatiomTime() {return true;}

	void setFuncReturnName(const QString &iFuncRet) {_funcReturnName = iFuncRet;};
	QString getFuncReturnName() const {return _funcReturnName;}

    /*  void replaceUntagged();*/


private:

    QString     _infoText;
    QString     _functionName;
    bool        _hasUserCondition;
    QString     _funcReturnName;
    void getAllAttributeValues(QDomElement& iElement, QStringList& oVars)const;



};

#endif // GTAActionGenericFunction_H
