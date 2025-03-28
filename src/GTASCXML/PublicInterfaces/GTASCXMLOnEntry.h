#ifndef GTASCXMLONENTRY_H
#define GTASCXMLONENTRY_H


#include "GTASCXMLLog.h"
#include "GTASCXMLAssign.h"
#include "GTASCXMLSend.h"
#include "GTASCXML.h"
#include "GTASCXMLCancel.h"

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTASCXML_SHARED_EXPORT GTASCXMLOnEntry
{
    QList<GTASCXMLLog> _logs;
    QList<GTASCXMLAssign> _assigns;
    QList<GTASCXMLSend> _sends;
    QList<GTASCXMLCancel> _cancels;
    QList<QDomElement> _extraInfoList;
public:
    GTASCXMLOnEntry();
    void insertLog(const GTASCXMLLog &  iLog);
	QList<GTASCXMLLog> getLogs() const;
    void insertAssign(const GTASCXMLAssign & iAssign);
	QList<GTASCXMLAssign> getAssigns() const;
    void insertSend(const GTASCXMLSend & iSend);
	QList<GTASCXMLSend> getSends() const;
    void insertCancel(const GTASCXMLCancel &iCancel);
	QList<GTASCXMLCancel> getCancels() const;
    void insertExtraInfo(QDomElement &iExtraInfo);
	QList<QDomElement> getExtraInfo() const;
    bool translateXMLToStruct(const QDomElement &iOnEntryElem);
    bool isNull() const;
    QStringList getVariableList() const;
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;
   
};

#endif // GTASCXMLONENTRY_H
