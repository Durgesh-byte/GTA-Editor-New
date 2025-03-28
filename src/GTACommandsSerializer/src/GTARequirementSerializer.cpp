#include "GTARequirementSerializer.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#include <QDomDocument>
#include <QDomNode>
#pragma warning(pop)

GTARequirementSerializer::GTARequirementSerializer(GTACommandBase * iCmd):GTACmdSerializer()
{
    _pCmd = iCmd;
}


bool GTARequirementSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pCmd != NULL)
    {
        QStringList requirements = _pCmd->getRequirements();
        oElement = iDomDoc.createElement(XNODE_REQUIREMENTS);
        foreach(QString requirement, requirements)
        {
            QDomElement reqNode = iDomDoc.createElement(XNODE_REQUIREMENT);
            reqNode.setAttribute("Val",requirement);
            oElement.appendChild(reqNode);

        }
        rc = true;
    }
    return rc;
}

bool GTARequirementSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull() && opCommand != NULL)
    {
        rc = true;
        bool found = false;
        QDomNodeList children = iElement.childNodes();
        QDomNode reqNode;
        for(int i = 0; i < children.count(); i++)
        {
            reqNode = children.at(i);
            if(reqNode.isElement() && reqNode.nodeName() == XNODE_REQUIREMENTS)
            {
                found = true;
                break;

            }
        }
        if(found)
        {
            QDomNodeList reqNodeList = reqNode.childNodes();
            QStringList cmdReqLst;
            for(int i = 0; i < reqNodeList.count();i++)
            {
                QDomNode subReqNode = reqNodeList.at(i);
                if(subReqNode.nodeName() == XNODE_REQUIREMENT)
                {
                    QString values = subReqNode.attributes().namedItem("Val").nodeValue();
                    cmdReqLst.append(values);
                }
            }
            opCommand->setRequirements(cmdReqLst);
        }
    }
    return rc;
}
