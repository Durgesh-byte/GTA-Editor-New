#ifndef GTASCXMLSEND_H
#define GTASCXMLSEND_H

#include "GTASCXMLUtils.h"
#include "GTASCXML.h"

class GTASCXML_SHARED_EXPORT GTASCXMLSend
{
    QString _id;
    QString _delay;
    QString _event;
	QString _delayexpr;
public:
    GTASCXMLSend();
    void setId(const QString &iId);
    void setDelay(const QString &iDelay);
    void setEvent(const QString &iEvent);
    bool translateXMLToStruct(const QDomElement &iSendElem);
    QStringList getVariableList() const;
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;
};

#endif // GTASCXMLSEND_H
