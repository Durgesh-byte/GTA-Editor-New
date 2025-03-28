#ifndef GTASCXMLSCXML_H
#define GTASCXMLSCXML_H

#include "GTASCXMLUtils.h"

#include "GTASCXMLState.h"
#include "GTASCXMLFinal.h"
#include "GTASCXMLDataModel.h"
#include "GTASCXML.h"

class GTASCXML_SHARED_EXPORT GTASCXMLSCXML
{
    QString _initStateId;
    QString _elemName;
    QString _toolVersion;
    QList<GTASCXMLState> _states;
    GTASCXMLDataModel _datamodel;
    GTASCXMLFinal _final;
    bool _insertParamInit;
    bool _insertParamUnInit;
    bool _insertParamUnInitFirst;
    QDomElement _ReportInfo;
public:
    GTASCXMLSCXML();
    void insertReportInfo(QDomElement &iReportInfo);
    void setElemName(const QString &iElemName);
    void setToolVersion(const QString &iToolVersion);
    void insertState(const GTASCXMLState &iState);
    void setDataModel(const GTASCXMLDataModel &iDataModel);
    void setFinalState(const GTASCXMLFinal &iFinal);
    void setInitUnInitConfig(bool isInit,bool isUnInit, bool isUnInitFirst);
    QStringList getVariableList() const;
    bool translateXMLToStruct(QDomElement &iSCXMLElem);

    QDomElement getSCXML(QDomDocument &iDomDoc, bool iEnUSCXML = false) const;
    QString getSCXMLString(bool iEnUSCXML = false) const;
};

#endif // GTASCXMLSCXML_H
