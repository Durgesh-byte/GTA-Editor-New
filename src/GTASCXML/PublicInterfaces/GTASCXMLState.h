#ifndef GTASCXMLSTATE_H
#define GTASCXMLSTATE_H


#include "GTASCXMLUtils.h"
#include "GTASCXMLFinal.h"
#include "GTASCXMLOnEntry.h"
#include "GTASCXMLOnExit.h"
#include "GTASCXMLTransition.h"
#include "GTASCXMLInvoke.h"
#include "GTASCXML.h"
#include "GTASCXMLDataModel.h"

class GTASCXML_SHARED_EXPORT GTASCXMLState
{

    QString _id;
    QString _initial;
    
    bool _isNested;

    GTASCXMLOnEntry _onEntry;
    GTASCXMLOnExit _onExit;
    GTASCXMLFinal _final;
    QList<GTASCXMLTransition> _transitions;
    QList<GTASCXMLState> _states;
    QList<GTASCXMLInvoke> _invokes;
    GTASCXMLDataModel _datamodel;
    bool _embedLog;
    QString _InstanceName;
    QStringList _varList;
public:
    enum STATE_TYPE{STATE,PARALLEL};
    STATE_TYPE _type;

    GTASCXMLState();
    void setId(const QString &iId);
    void setInitId(const QString &iInitId);
    void setType(const  STATE_TYPE &iType);
    void setOnEntry(const GTASCXMLOnEntry &iOnEntry);
	void insertOnEntry(const GTASCXMLOnEntry &iOnEntry);
    void setOnExit(const GTASCXMLOnExit &iOnExit);
    void setFinal(const GTASCXMLFinal &iFinal);
    void insertTransitions(const GTASCXMLTransition &iTransitions, bool iSubStates = false);
    void insertTransitionAtIndex(const GTASCXMLTransition &iTransitions, int index);
    void removeTransition(const GTASCXMLTransition &iTransitions, bool iSubStates = false);
    void insertInvokes(const GTASCXMLInvoke &iInvoke);
	void clearInvokes();
    void insertStates(const GTASCXMLState &iState);
    QStringList getVariableList() const;
    GTASCXMLDataModel getDataModel() const;
    void setDataModel(const GTASCXMLDataModel &iDataModel);
    void insertVarList(const QStringList &iVarList);
    bool translateXMLToStruct(const QDomElement &iStateElem);
    bool isNull();

    QString getId() const;
    QString getInitId() const;
    QList<GTASCXMLState> getStates() const;
	void clearStates();
    GTASCXMLOnEntry getOnEntry() const;
    GTASCXMLOnExit getOnExit();
    QList<GTASCXMLInvoke> getInvokes();
    QList<GTASCXMLTransition> getTransitions() const;
	void clearTransitions();
    QDomElement getSCXML(QDomDocument &iDomDoc, bool iEnUSCXML = false)  const;
    QString getSCXMLString(bool iEnUSCXML) const;
    void embedLog(const bool &iEmbedLog, const QString &iInstanceName);
	void setIsParamFromCSV(const bool isParamFromCSV);
	bool getIsParamFromCSV() const;
    void setIsFunctionalStatusFromCSV(const bool isFunctionalStatusFromCSV);
    bool getIsFunctionalStatusFromCSV() const;
private:
    void getLogs();
    QDomElement getCurrentStateSCXML(QDomDocument &iDomDoc, bool iEnUSCXML) const;
	bool _isParamFromCSV = false;
    bool _isFunctionalStatusFromCSV = false;
};

#endif // GTASCXMLSTATE_H

