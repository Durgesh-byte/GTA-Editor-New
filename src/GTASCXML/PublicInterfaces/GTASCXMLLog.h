#ifndef GTASCXMLLOG_H
#define GTASCXMLLOG_H

#include "GTASCXMLUtils.h"
#include "GTASCXML.h"

class GTASCXML_SHARED_EXPORT GTASCXMLLog
{
    QString _expr;
    QString _label;
public:
    GTASCXMLLog();
    void setExpression(const QString &iExpr);
    void setLabel(const QString &iLabel);
    QString getExpression() const;
    QString getLabel() const;
    bool translateXMLToStruct(const QDomElement &iLogElem);
    QStringList getVariableList() const;
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;

};

#endif // GTASCXMLLOG_H
