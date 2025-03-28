#ifndef GTASCXMLFINAL_H
#define GTASCXMLFINAL_H

#include "GTASCXMLOnEntry.h"
#include "GTASCXMLOnExit.h"
#include "GTASCXMLUtils.h"
#include "GTASCXML.h"

class GTASCXML_SHARED_EXPORT GTASCXMLFinal
{

    QString _id;
    GTASCXMLOnEntry _onEntry;
    GTASCXMLOnExit _onExit;

public:
    GTASCXMLFinal();
    void setId(const QString &iId);
    QString getId()const;
    void setOnEntry(const GTASCXMLOnEntry &iOnEntry);
    void setOnExit(const GTASCXMLOnExit &iOnExit);
    bool translateXMLToStruct(const QDomElement &iFinalElem);
    bool isNull() const;
    QStringList getVariableList() const;
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;

};

#endif // GTASCXMLFINAL_H
