#include "GTASCXMLUtils.h"

#pragma warning(push, 0)
#include <QStack>
#pragma warning(pop)

void GTASCXMLUtils::validateExpression(QString &oExpression)
{
    QByteArray r = oExpression.toLatin1();
    QByteArray s = oExpression.toLatin1();

    QStack<int> indexStack;

    int i = 0;
    while (i < s.length())
    {

        if (s[i] == '(')
        {
            

            if ((s[i + 1] == '(') || (s[i+1] == ' ' && s[i + 2] == '('))
            {
                indexStack.push(-i);
            }
            else
            {
                indexStack.push(i);
            }
        }
        //        else if (s[i] != ')' && s[i] != '(')
        //        {
        //            i++;
        //        }
        else if (s[i] == ')')
        {
            int top = indexStack.top();
            if (s[i - 1] == ')' && top <= 0)
            {
                r[-top] = '$';
                r[i] = '$';
                indexStack.pop();
            }
            else if (s[i - 1] == ')' && top > 0)
            {
                //("Something is wrong!!");
            }
            else if (s[i - 1] != ')' && top > 0)
                indexStack.pop();
            //            i++;
        }
        i++;
    }

    QString result(r);
    result.remove("$");
    oExpression.clear();
    oExpression = result;

}
