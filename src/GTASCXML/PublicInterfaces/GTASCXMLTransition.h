#ifndef GTASCXMLTRANSITION_H
#define GTASCXMLTRANSITION_H

#include "GTASCXMLLog.h"
#include "GTASCXMLAssign.h"
#include "GTASCXMLSend.h"
#include "GTASCXML.h"
#include "GTASCXMLUtils.h"
#include "GTASCXMLCancel.h"

#pragma warning(push, 0)
#include <QList>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTASCXML_SHARED_EXPORT GTASCXMLTransition
{
    QString _cond;
    QString _event;
    QString _target;
    QString _status;
    QList<GTASCXMLLog> _logs;
    QList<GTASCXMLAssign> _assigns;
    QList<GTASCXMLSend> _sends;
    QList<GTASCXMLCancel> _cancels;
    bool _hasDefEventCancel;
public:
    GTASCXMLTransition();
    void setCondition(const QString & iCond);
    void setEvent(const QString & iEvent);
    void setTarget(const QString & iTarget);
    void setStatus(const QString & iStatus);
    void setDefaultEventCancel(bool iDefEventCancel);
    void insertLog(const GTASCXMLLog &  iLog);
    void insertAssign(const GTASCXMLAssign & iAssign);
    void insertSend(const GTASCXMLSend & iSend);
    void insertCancel(const GTASCXMLCancel &iCancel);

    QString getEvent() const {return _event;}
    QString getTarget() const {return _target;}
    QString getCondition() const {return _cond;}
    QString getStatus() const {return _status;}
    QList<GTASCXMLLog> getLogs() const {return _logs;}
    QList<GTASCXMLCancel> getCancels() const {return _cancels;}
    bool getDefaultEventCancel() { return _hasDefEventCancel;}
    QStringList getVariableList() const;
    bool translateXMLToStruct(const QDomElement &iTransitionElem);

    QDomElement getSCXML(QDomDocument & iDomDoc)  const;

};

#endif // GTASCXMLTRANSITION_H
