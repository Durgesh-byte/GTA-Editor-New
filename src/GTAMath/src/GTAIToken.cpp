#include "GTAIToken.h"
#include "GTAUtil.h"

GTAIToken::GTAIToken(TokenType iType,QString iExpression)
{
    _Exp=iExpression;
    _TokenType = iType;
    
}
GTAIToken::~GTAIToken()
{
}
void GTAIToken::updateGenToolRetrunParams(const QHash<QString,QString>& iGenToolReturn)
{
    if(!_Exp.contains("$"))
        return;
    QString expression = _Exp;
    QStringList iTokens = expression.split("$");
    QStringList keys = iGenToolReturn.keys();
    for(int i=0;i<keys.count();i++)
    {
        QString key = keys.at(i);

        QStringList tokens = key.split("$");

        if(tokens.at(0) == iTokens.at(0))
        {
            if(iTokens.at(1).contains(tokens.at(1)))
            {
                expression = iGenToolReturn.value(key);
                _Exp = expression;
                break;
            }
        }


    }
}

void GTAIToken::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{

    //iTagValueMap;//to suppress warning
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    QString expression = _Exp;
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        expression.replace(tag,iterTag.value());
    }
    _Exp = expression;
}
