#ifndef GTATOKENVARIABLE_H
#define GTATOKENVARIABLE_H
#include "GTAMath.h" 
#include "GTAIToken.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTAMath_SHARED_EXPORT GTATokenVariable : public GTAIToken
{
public:
   
    GTATokenVariable(const QString& iExpression);
    virtual ~GTATokenVariable();

     bool isValid()         const;
     bool isOperator()      const{return false;}
     bool isConstant()      const{return false;}
     bool isVariable()      const{return true;}
     bool isFunction()      const{return false;}

     bool isEndBracket()    const{return false;}
     bool isStartBracket()  const{return false;}
     bool isFunctionSeperator()  const{return false;}
	 //CommaSeparator to support multiple functions with comma separated operands i.e. pow, min, max function
     bool isCommaSeparator() const{return false;}

     bool serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const;
     GTAIToken* getClone()const;

private:
    
};

#endif // GTAIToken_H
