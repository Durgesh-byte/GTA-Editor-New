#ifndef GTATOKENCONST_H
#define GTATOKENCONST_H
#include "GTAMath.h" 
#include "GTAIToken.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTAMath_SHARED_EXPORT GTATokenConst : public GTAIToken
{
public:
   
    GTATokenConst(const QString& iExp);
    virtual ~GTATokenConst();

     bool isValid()   const;
     
     bool isOperator()const{return false;}
     bool isConstant()const{return true;}
     bool isVariable()const{return false;}
     bool isFunction()const{return false;}
     bool isEndBracket()     const{return false;}
     bool isStartBracket()   const{return false;}
	 //CommaSeparator to support multiple functions with comma separated operands i.e. pow, min, max function
     bool isCommaSeparator() const{return false;}
     bool isFunctionSeperator()   const{return false;}
     GTAIToken* getClone()const;
     bool serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const;

private:
    
};

#endif // GTAIToken_H
