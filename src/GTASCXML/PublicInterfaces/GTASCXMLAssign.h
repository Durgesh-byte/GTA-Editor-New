#ifndef GTASCXMLASSIGN_H
#define GTASCXMLASSIGN_H

#include "GTASCXMLUtils.h"
#include "GTASCXML.h"

class GTASCXML_SHARED_EXPORT GTASCXMLAssign
{
    QString _expr;
    QString _loc;

public:
    GTASCXMLAssign();
    void setExpression(const QString &iExpr);
    void setLocation(const QString &iLocation);
    bool translateXMLToStruct(const QDomElement &iAssignElem);

    QString getExpression() const;
    QString getLocation() const;

    QStringList getVariableList() const;
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;

};

#endif // GTASCXMLASSIGN_H
