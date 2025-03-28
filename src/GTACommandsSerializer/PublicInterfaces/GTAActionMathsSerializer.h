#ifndef GTAActionMathssSERIALIZER_H
#define GTAActionMathssSERIALIZER_H
#include "GTACommandsSerializer.h"
#include "GTACmdSerializerInterface.h"

class GTAMathExpression;
class GTAActionMaths;

class GTACommandsSerializer_SHARED_EXPORT GTAActionMathsSerializer : public GTACmdSerializerInterface
{
public:
    GTAActionMathsSerializer();
    virtual ~GTAActionMathsSerializer(){}
    bool serialize(QDomDocument &iDomDoc, QDomElement &oElement);
    bool deserialize(const QDomElement & iElement, GTACommandBase *& opCommand);
    void setCommand(const GTACommandBase* ipCmd);
   
private:
    GTAMathExpression*  getExpressionFromDom(const QDomElement& iExpressionElement,bool isFunctionArgument=false);
    GTAActionMaths * _pActionCmd;
};

#endif // GTAActionMathssSERIALIZER_H
