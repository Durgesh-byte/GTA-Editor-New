#ifndef GTATOKENENDBRACKET_H
#define GTATOKENENDBRACKET_H
#include "GTAMath.h" 
#include "GTAIToken.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTAMath_SHARED_EXPORT GTATokenEndBracket : public GTAIToken
{
public:
   
    GTATokenEndBracket(const QString& iExp);
    virtual ~GTATokenEndBracket();

     bool isValid()         const;
     bool isOperator()      const{return false;}
     bool isConstant()      const{return false;}
     bool isVariable()      const{return false;}
     bool isFunction()      const{return false;}
     bool isEndBracket()    const{return true;}
     bool isStartBracket()  const{return false;}
     bool isFunctionSeperator()  const{return false;}
	 //CommaSeparator to support multiple functions with comma separated operands i.e. pow, min, max function
     bool isCommaSeparator() const{return false;}
     bool serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const;

     GTAIToken* getClone()const;

private:
    
};

#endif // GTAIToken_H
