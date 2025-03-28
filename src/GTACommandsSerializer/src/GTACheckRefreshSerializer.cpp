#include "GTACheckRefreshSerializer.h"
#include "GTACommentSerializer.h"
#include "GTAUtil.h"
#include "GTACheckRefresh.h"
#include "GTARequirementSerializer.h"
GTACheckRefreshSerializer::GTACheckRefreshSerializer():GTACmdSerializerInterface()
{
    _pChkAudioCmd = NULL;
}
GTACheckRefreshSerializer::~GTACheckRefreshSerializer()
{
    _pChkAudioCmd = NULL;
}
void GTACheckRefreshSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pChkAudioCmd = dynamic_cast<GTACheckRefresh*>((GTACommandBase*) iPCmd);
}
bool GTACheckRefreshSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pChkAudioCmd != NULL)
    {




        QString sParameter = _pChkAudioCmd->getParameter();
        QString sRefreshRate = _pChkAudioCmd->getRefreshTime();

        QStringList sDumpList = _pChkAudioCmd->getDumpList();


        oElement  = iDomDoc.createElement(XNODE_CHECK);
        oElement.setAttribute(XNODE_NAME,CHK_REFRESH);

        setActionAttributes(_pChkAudioCmd,oElement);




        QDomElement domMsgElement = iDomDoc.createElement(XNODE_PARAMETER);
        domMsgElement.setAttribute(XATTR_REFRESH_RATE,sRefreshRate);
        domMsgElement.setAttribute(XNODE_NAME,sParameter);
        oElement.appendChild(domMsgElement);



        if (!sDumpList.isEmpty())
        {
            QDomElement doDumpList = iDomDoc.createElement(XNODE_DUMPLIST);
            oElement.appendChild(doDumpList);
            foreach(QString dumpParameter, sDumpList)
            {
                QDomElement doDumpParamNode = iDomDoc.createElement(XNODE_DUMP_PARAM);
                QDomText domParamValTxt = iDomDoc.createTextNode(dumpParameter);
                doDumpList.appendChild(doDumpParamNode);
                doDumpParamNode.appendChild(domParamValTxt);

            }

        }


        //comment node
        GTACommentSerializer commentSerializer(_pChkAudioCmd);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }

        GTARequirementSerializer requirementSerializer(_pChkAudioCmd);
        QDomElement domRequirement;
        rc = requirementSerializer.serialize(iDomDoc,domRequirement);
        if( rc && (!domRequirement.isNull()))
        {
            oElement.appendChild(domRequirement);
        }

        rc = true;
    }
    return rc;
}


bool GTACheckRefreshSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString actionName = iElement.attribute(XNODE_CHECK);
        
        QStringList dumpList;

        

        GTACheckRefresh* pChkRefresh = new GTACheckRefresh;

        getActionAttributes(iElement,pChkRefresh);
        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            QDomElement childElem = childNode.toElement();
            QString sTagName = childElem.tagName();

            if (sTagName==XNODE_PARAMETER)
            {
                QString sRefreshRate = childElem.attribute(XATTR_REFRESH_RATE);
                QString sParameterName = childElem.attribute(XNODE_NAME);
                pChkRefresh->setRefreshTime(sRefreshRate);
                pChkRefresh->setParameter(sParameterName);

            }
            else if (sTagName == XNODE_DUMPLIST)
            {

                QDomNodeList dumpParamNodes = childNode.childNodes();
                int dumpNodeSize = dumpParamNodes.count();

                for (int j =0 ;j<dumpNodeSize;j++)
                {
                    QDomNode dumpNode = dumpParamNodes.at(j);
                    QString sDumpParameter = dumpNode.toElement().text();
                    if (!sDumpParameter.isEmpty())
                    {
                        dumpList.append(sDumpParameter);
                    }
                }

                if (!dumpList.isEmpty())
                    pChkRefresh->setDumpList(dumpList);
            }
        }



        opCommand=pChkRefresh;

        //comment node
        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);

        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);
        rc = true;
    }
    return rc;

}
#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1").arg(CHK_REFRESH),GTACheckRefreshSerializer)
