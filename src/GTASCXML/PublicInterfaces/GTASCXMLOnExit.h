#ifndef GTASCXMLONEXIT_H
#define GTASCXMLONEXIT_H

#include "GTASCXMLLog.h"
#include "GTASCXMLAssign.h"
#include "GTASCXMLSend.h"
#include "GTASCXML.h"
#include "GTASCXMLCancel.h"

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTASCXML_SHARED_EXPORT GTASCXMLOnExit
{
    QList<GTASCXMLLog> _logs;
    QList<GTASCXMLAssign> _assigns;
    QList<GTASCXMLSend> _sends;
    QList<GTASCXMLCancel> _cancels;
    QList<QDomElement> _extraInfoList;

public:
    GTASCXMLOnExit();
    void insertExtraInfo(const QDomElement &iExtraInfo);
    void insertLog(const GTASCXMLLog &  iLog);
    void insertAssign(const GTASCXMLAssign & iAssign);
    void insertSend(const GTASCXMLSend & iSend);
    void insertCancel(const GTASCXMLCancel &iCancel);
    QStringList getVariableList() const;
    bool translateXMLToStruct(const QDomElement &iOnExitElem);

    bool isNull() const;
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;
};

#endif // GTASCXMLONEXIT_H
