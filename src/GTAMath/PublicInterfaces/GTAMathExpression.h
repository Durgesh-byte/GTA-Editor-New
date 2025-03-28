#ifndef GTAMATHEXPRESSION_H
#define GTAMATHEXPRESSION_H
#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#include <QList>
#include <QStack>
#include <QDomDocument>
#pragma warning(pop)

#include "GTAMath.h"

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTAIToken;
class GTAMath_SHARED_EXPORT GTAMathExpression
{
public:
    
    GTAMathExpression(bool isFunc=false);
    virtual ~GTAMathExpression();
    GTAMathExpression(GTAMathExpression* iRhs);

    bool isValid()const;
    bool pushToken(GTAIToken* iToken);
    QString getSCXMLExpr()const;
    void popToken();
    QString getExpression();
    bool isEmpty(){return _TokenList.isEmpty();}
    GTAIToken* getLastItem();
    bool isFunctionExpression(){return _isFunctionArgument;}
    QList<GTAIToken*>     getTokenListClones();

    bool serializeExpression(QDomElement& ioRootElement, QDomDocument& iDomDocument);
    void  getVariableList(QStringList& iList);
    void setFunctionBracesStack( QStack<GTAIToken*>& oStack);
    void getFlatExpression(QList<GTAIToken*>& oList);
    bool isBracesBalanced();
    bool appendTokenToCurrentList(GTAIToken* iToken);
    void clearAll();
    
    void ReplaceTag(const QMap<QString,QString>& iTagValueMap);
	void updateGenToolRetrunParams(const QHash<QString,QString>& iGenToolReturn);

    bool _isPowFunction, isMultipleCommaEnabled;
    QString paramInfo;

private:
    QList<GTAIToken*>     _TokenList;
    GTAMathExpression(const GTAMathExpression&){};
    bool _isFunctionArgument;

};

#endif // GTAIToken_H
