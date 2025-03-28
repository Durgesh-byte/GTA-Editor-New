#ifndef GTAITOKEN_H
#define GTAITOKEN_H
#include "GTAMath.h" 

#pragma warning(push, 0)
#include <QString>
#include <QDomElement>
#include <QDomDocument>
#include <QMap>
#include <QHash>
#pragma warning(pop)

class GTAMath_SHARED_EXPORT GTAIToken
{
public:
    //Added new COMMASEPARATOR to support multiple functions with comma separated operands i.e. pow, min, max function
    enum TokenType{STARTBRACKET,ENDBRACKET,OPERATOR,CONSTANT,VARIABLE,FUNCTION,FUNCTIONSEPERATOR,COMMASEPARATOR};
    GTAIToken(TokenType iType,QString iExpression);
    virtual ~GTAIToken();

    virtual bool isValid()          const=0;
    virtual bool isOperator()       const=0;
    virtual bool isConstant()       const=0;
    virtual bool isVariable()       const=0;
    virtual bool isFunction()       const=0;
    virtual bool isEndBracket()     const=0;
    virtual bool isStartBracket()   const=0;
    virtual bool isFunctionSeperator()       const=0;
	//CommaSeparator to support multiple functions with comma separated operands i.e. pow, min, max function
    virtual bool isCommaSeparator() const =0;

    //	virtual bool isRandomFunction() const=0;

    virtual bool serializeToken(QDomElement& ioRootElement, QDomDocument& iDomDocument)const=0;

    virtual GTAIToken* getClone()const=0;
    TokenType getTokenType()const{return _TokenType;}
    
    virtual QString getExpression(){return _Exp;}
    
    virtual bool popExpression(){return false;}
    virtual bool pushToken(GTAIToken*){return false;}
    virtual bool isLastTokenFunction(){return false;}
    virtual GTAIToken* getLastItem(){return NULL;}
    virtual void ReplaceTag(const QMap<QString,QString>& iTagValueMap);
    void updateGenToolRetrunParams(const QHash<QString,QString> &iGenToolReturn);

protected:
    QString     _Exp;
    TokenType _TokenType;

};

#endif // GTAIToken_H
