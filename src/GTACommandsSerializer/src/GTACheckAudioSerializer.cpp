#include "GTACheckAudioSerializer.h"
#include "GTACommentSerializer.h"
#include "GTAUtil.h"
#include "GTARequirementSerializer.h"
GTACheckAudioSerializer::GTACheckAudioSerializer(GTACheckAudioAlarm *ipActionCmd)
{
    _pChkAudioCmd = ipActionCmd;
}

bool GTACheckAudioSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pChkAudioCmd != NULL)
    {

        QString sTimeOut , sTimeUnit;
        QString sPrecision,sPrecisionUnit;


        _pChkAudioCmd->getTimeOut(sTimeOut,sTimeUnit) ;
        _pChkAudioCmd->getPrecision(sPrecision,sPrecisionUnit) ;
        QString sActionOnFail =_pChkAudioCmd->getActionOnFail() ;


        QString sAudioMsg = _pChkAudioCmd->getAlarmName();
        QString sdelay = _pChkAudioCmd->getTime();

        QStringList sDumpList = _pChkAudioCmd->getDumpList();


        oElement  = iDomDoc.createElement(XNODE_CHECK);
        oElement.setAttribute(XNODE_NAME,CHK_AUDIO_ALRM);
//         oElement.setAttribute(XNODE_ONFAIL,sActionOnFail);
//         oElement.setAttribute(XNODE_TIMEOUT,sTimeOut);
//         oElement.setAttribute(XNODE_PRECISION,sPrecision);
//         oElement.setAttribute(XNODE_PRECISION_UNIT, sPrecisionUnit);
//         oElement.setAttribute(XNODE_TIMEOUT_UNIT,sTimeUnit);
// 
//         QString hiddenInSCXML = _pChkAudioCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
//         oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);

        setActionAttributes(_pChkAudioCmd,oElement);




        QDomElement domMsgElement = iDomDoc.createElement(XNODE_PARAMETER);
        domMsgElement.setAttribute(XNODE_NAME,"AUDIO_SOUND");
        domMsgElement.setAttribute(XNODE_VALUE,sAudioMsg);
        oElement.appendChild(domMsgElement);




        QDomElement domWaitBufferDepthElem = iDomDoc.createElement(XNODE_WAIT_BUFFER_DEPTH);
        QDomText buffDepthTxtN= iDomDoc.createTextNode(sdelay);
        domWaitBufferDepthElem.appendChild(buffDepthTxtN);
        oElement.appendChild(domWaitBufferDepthElem);

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

bool GTACheckAudioSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString actionName = iElement.attribute(XNODE_CHECK);
//         QString sOnFail = iElement.attribute(XNODE_ONFAIL);
//         QString sPrecisoin = iElement.attribute(XNODE_PRECISION);
//         QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
//         QString sTimeOutUnit= iElement.attribute(XNODE_TIMEOUT_UNIT);
//         QString sPrecisoinUnit= iElement.attribute(XNODE_PRECISION_UNIT);
//         
//         bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
       
        QStringList dumpList;
        GTACheckAudioAlarm* pAudioAlarm = new GTACheckAudioAlarm;
//         pAudioAlarm->setTimeOut(sTimeOut,sTimeOutUnit);
//         pAudioAlarm->setPrecision(sPrecisoin,sPrecisoinUnit);
//         pAudioAlarm->setActionOnFail(sOnFail);
//         pAudioAlarm->setIgnoreInSCXML(ignoreInSCXML); 
//         pAudioAlarm->setIgnoreInSCXML(ignoreInSCXML);
        getActionAttributes(iElement,pAudioAlarm);

        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            QDomElement childElem = childNode.toElement();
            QString sTagName = childElem.tagName();

            if (sTagName==XNODE_PARAMETER)
            {
                QString sMsg = childElem.text();
                QDomAttr domValAttr = childElem.attributeNode(XNODE_VALUE);
                pAudioAlarm->setValue(domValAttr.value());
            }
            else if (sTagName==XNODE_WAIT_BUFFER_DEPTH)
            {
                QString sMsg = childElem.text();
                pAudioAlarm->setTime(sMsg);
            }else if (sTagName == XNODE_DUMPLIST)
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
                    pAudioAlarm->setDumpList(dumpList);
            }
        }



        opCommand=pAudioAlarm;

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
