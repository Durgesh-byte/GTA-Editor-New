#ifndef GTASCXMLINVOKE_H
#define GTASCXMLINVOKE_H

#include "GTASCXMLLog.h"
#include "GTASCXMLSend.h"
#include "GTASCXMLAssign.h"
#include "GTASCXMLXMLRPC.h"
#include "GTASCXMLUtils.h"
#include "GTASCXML.h"
#include "GTASCXMLGenericFunction.h"

class GTASCXML_SHARED_EXPORT GTASCXMLInvoke
{

    QString _invokeId;
    QString _invokeType;
    QString _invokeDelay;
    QList<GTASCXMLLog> _logs;
    QList<GTASCXMLAssign> _assigns;
    QList<GTASCXMLSend> _sends;
    GTASCXMLXMLRPC _func;
    GTASCXMLGenericFunction _genFunc;


public:
    GTASCXMLInvoke();
    void setInvokeId(const QString &iInvokeId);
    void setInvokeType(const QString &iInvokeType);
    void setInvokeFunction(const GTASCXMLXMLRPC &iXmlRpc);
    void insertLog(const GTASCXMLLog &iLog);
    void insertAssign(const GTASCXMLAssign &iAssign);
	void clearAssigns() { _assigns.clear(); };
    void insertSend(const GTASCXMLSend &iSend);
    void setInvokeDelay(const QString &iTimeInMs);
    bool translateXMLToStruct(const QDomElement &iInvokeElem);
    bool translateXMLToGenericStruct(QDomElement &iInvokeElem);
    void setInvokeFunctionGeneric(const GTASCXMLGenericFunction &iGenFunc);
	QList<GTASCXMLAssign> getAssigns() const { return _assigns; };
    QString getInvokeId(){return _invokeId;}

    GTASCXMLXMLRPC getXMLRPCFunction();
    GTASCXMLGenericFunction getGenericFunction();
    QStringList getVariableList() const;

    QDomElement getSCXML(QDomDocument &iDomDoc)  const;



};

#endif // GTASCXMLINVOKE_H
