#ifndef GTASCXMLCANCEL_H
#define GTASCXMLCANCEL_H

#include "GTASCXML.h"
#include "GTASCXMLUtils.h"


class GTASCXML_SHARED_EXPORT GTASCXMLCancel
{
    QString _SendEventId;

public:
    GTASCXMLCancel();
    void setSendEventId(const QString &iSendEventId);
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;
};

#endif // GTASCXMLCANCEL_H
