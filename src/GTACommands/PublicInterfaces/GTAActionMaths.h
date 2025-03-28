#ifndef GTAActionMathsS_H
#define GTAActionMathsS_H
#include "GTAActionBase.h"

class GTAMathExpression;
class GTACommands_SHARED_EXPORT GTAActionMaths : public GTAActionBase
{
public:

    GTAActionMaths();

    GTAActionMaths(const GTAActionMaths& rhs);
    virtual ~GTAActionMaths();
    
    
    void setFirstVariable(const QString & ipVar);
   
   
    QString getFirstVariable()const;
    
    
    QString getStatement() const;


    QStringList getRHSVariableList() const;


    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;
    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    void updateGenToolRetrunParams(const QHash<QString,QString>& iGenToolReturn);

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    QString getSCXMLStateName()const;
    void setExpression(GTAMathExpression* iExp);
    GTAMathExpression* getExpression()const;
    QString getSCXMLExpr() const;
    void setExpressionTxt(const QString& iExpression);
    QString getExpressionTxt()const;
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const;
    QString processExpression(const QString &ioExpression) const;
private:
    QString _var1;
    
    GTAMathExpression* _pExpression;
    QString _Expression;
};

#endif // GTAActionMathss_H
