#ifndef GTATOKENFUNCTION_H
#define GTATOKENFUNCTION_H
#include "GTAMath.h" 
#include "GTAIToken.h"

#pragma warning(push, 0)
#include <QString>
#include <QList>
#include <QStack>
#pragma warning(pop)

class GTAMathExpression;
class GTAMath_SHARED_EXPORT GTATokenFunction : public GTAIToken
{
public:
   
    GTATokenFunction(const QString& iExpression,const int& iNoOfArguments);
    virtual ~GTATokenFunction();

     bool isValid()         const;

     bool isOperator()      const{return false;}
     bool isConstant()      const{return false;}
     bool isVariable()      const{return false;}
     bool isFunction()      const{return true;}
     bool isEndBracket()    const{return false;}
     bool isStartBracket()  const{return false;}

     bool isFunctionSeperator()const{return false;}
	 //CommaSeparator to support multiple functions with comma separated operands i.e. pow, min, max function
     bool isCommaSeparator() const{return false;}

     bool popExpression();
    // void pushExpression(GTAMathExpression* iToken);
     bool pushToken(GTAIToken* iToken);
     QString getSCXMLExpr()const;
     QString getExpression();
     GTAIToken* getLastItem();
     bool isClosed(){return _isClosed;}
     void setClosed(bool iClosedStatus){_isClosed=iClosedStatus;}
     GTAIToken* getClone()const;
     virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
     bool serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const;
     void setExpression(GTAMathExpression* iExpression);
     void getVariableList(QStringList& oList);
     void setFunctionBracesStack( QStack<GTAIToken*>& oStack);
     void getFlatExpression(QList<GTAIToken*>& oList);
     bool isBracesBalanced();

     GTAMathExpression* _FuncExpression;

private:


    int _argumentSize;
    bool _isClosed;
    
};

#endif // GTAIToken_H
