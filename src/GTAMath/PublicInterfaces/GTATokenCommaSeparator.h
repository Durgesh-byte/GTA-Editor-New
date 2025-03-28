#ifndef GTATOKENCOMMASEPARATOR_H
#define GTATOKENCOMMASEPARATOR_H
#include "GTAMath.h"
#include "GTAIToken.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTAMath_SHARED_EXPORT GTATokenCommaSeparator : public GTAIToken
{
public:

    GTATokenCommaSeparator(const QString& iExp);
    virtual ~GTATokenCommaSeparator();

     bool isValid()         const;
     bool isOperator()      const{return false;}
     bool isConstant()      const{return false;}
     bool isVariable()      const{return false;}
     bool isFunction()      const{return false;}
     bool isEndBracket()    const{return false;}
     bool isStartBracket()  const{return false;}
     bool isFunctionSeperator()const{return false;}

     bool isCommaSeparator() const{return true;}



     GTAIToken* getClone()const;
     bool serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const;

private:

};

#endif // GTAIToken_H
