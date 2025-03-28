#include "GTASCXMLSCXML.h"

GTASCXMLSCXML::GTASCXMLSCXML()
{
}

void GTASCXMLSCXML::setElemName(const QString &iElemName)
{
    _elemName = iElemName;
}

void GTASCXMLSCXML::setToolVersion(const QString &iToolVersion)
{
    _toolVersion = iToolVersion;
}

void GTASCXMLSCXML::insertState(const GTASCXMLState &iState)
{
    _states<<iState;
    if(_initStateId.isEmpty())
    {
        _initStateId = iState.getId();
    }
}
void GTASCXMLSCXML::insertReportInfo(QDomElement &iReportInfo)
{
    _ReportInfo = iReportInfo;
}

QStringList GTASCXMLSCXML::getVariableList() const
{
    QStringList varList;
    varList.append(_final.getVariableList());
    for(int i=0;i<_states.count();i++)
    {
        varList.append(_states.at(i).getVariableList());
    }
    return varList;
}
void GTASCXMLSCXML::setDataModel(const GTASCXMLDataModel &iDataModel)
{
    _datamodel = iDataModel;
}

void GTASCXMLSCXML::setFinalState(const GTASCXMLFinal &iFinal)
{
    _final = iFinal;
}

void GTASCXMLSCXML::setInitUnInitConfig(bool isInit,bool isUnInit, bool isUnInitFirst)
{
    _insertParamInit = isInit;
    _insertParamUnInit = isUnInit;
    _insertParamUnInitFirst = isUnInitFirst;
}

bool GTASCXMLSCXML::translateXMLToStruct(QDomElement &iSCXMLElem)
{
    bool rc = false;
    if(iSCXMLElem.nodeName() == SCXML_SCXML)
    {
        rc = true;
        _initStateId = iSCXMLElem.attribute(SCXML_SCXML_INITID);
        _toolVersion = iSCXMLElem.attribute(SCXML_SCMXL_TOOLVER);
        _elemName = iSCXMLElem.attribute(SCXML_SCMXL_ELEMNAME);

        QDomNodeList childNodes= iSCXMLElem.childNodes();
        for(int i=0;i<childNodes.count();i++)
        {
            QDomNode scxmlChild = childNodes.at(i);
            if(!scxmlChild.isNull())
            {
                if((scxmlChild.nodeName() == SCXML_STATE_STATE) || (scxmlChild.nodeName() == SCXML_STATE_PARALLEL))
                {
                    GTASCXMLState state;
                    state.translateXMLToStruct(scxmlChild.toElement());
                    _states <<state;
                }
                else if(scxmlChild.nodeName() == SCXML_FINAL)
                {
                    _final.translateXMLToStruct(scxmlChild.toElement());
                }
                else
                {

                }
            }
        }

    }
    return rc;
}

/**
 * @brief Returns the complete string corresponding to the state chart of the element
 * @param isEnUSCXML
 * @return string of the SCXML
 */
QString GTASCXMLSCXML::getSCXMLString(bool iEnUSCXML) const
{
    QDomDocument iDomDoc;
    QDomElement SCXMLElem = iDomDoc.createElement(SCXML_SCXML);
    if(!_initStateId.isEmpty())
        SCXMLElem.setAttribute(SCXML_SCXML_INITID,_initStateId);
    if(!_toolVersion.isEmpty())
        SCXMLElem.setAttribute(SCXML_SCMXL_TOOLVER,_toolVersion);
    if(!_elemName.isEmpty())
        SCXMLElem.setAttribute(SCXML_SCMXL_ELEMNAME,_elemName);

    SCXMLElem.appendChild(_ReportInfo);
    SCXMLElem.appendChild(_datamodel.getSCXML(iDomDoc));

    QString stateStr;
    for(int i=0;i<_states.count();i++)
    {
        stateStr.append(_states.at(i).getSCXMLString(iEnUSCXML));
    }
    QDomElement subStateElem = iDomDoc.createElement("SUBSTATES");
    SCXMLElem.appendChild(subStateElem);
    if(_final.isNull())
    {
        GTASCXMLFinal final;
        final.setId(SCXML_FINAL_STATE_ID);
        SCXMLElem.appendChild(final.getSCXML(iDomDoc));
    }
    else
        SCXMLElem.appendChild(_final.getSCXML(iDomDoc));

	iDomDoc.appendChild(SCXMLElem);
    QString scxmlString = iDomDoc.toString();
    scxmlString.replace("<SUBSTATES/>",stateStr);
    return scxmlString;
}

QDomElement GTASCXMLSCXML::getSCXML(QDomDocument &iDomDoc, bool iEnUSCXML) const
{
    QDomElement SCXMLElem = iDomDoc.createElement(SCXML_SCXML);
    if(!_initStateId.isEmpty())
        SCXMLElem.setAttribute(SCXML_SCXML_INITID,_initStateId);
    if(!_toolVersion.isEmpty())
        SCXMLElem.setAttribute(SCXML_SCMXL_TOOLVER,_toolVersion);
    if(!_elemName.isEmpty())
        SCXMLElem.setAttribute(SCXML_SCMXL_ELEMNAME,_elemName);

    SCXMLElem.appendChild(_ReportInfo);
    SCXMLElem.appendChild(_datamodel.getSCXML(iDomDoc));

    for(int i=0;i<_states.count();i++)
    {
        SCXMLElem.appendChild(_states.at(i).getSCXML(iDomDoc,iEnUSCXML));
    }
    if(_final.isNull())
    {
        GTASCXMLFinal final;
        final.setId(SCXML_FINAL_STATE_ID);
        SCXMLElem.appendChild(final.getSCXML(iDomDoc));
    }
    else
        SCXMLElem.appendChild(_final.getSCXML(iDomDoc));
    return SCXMLElem;
}
