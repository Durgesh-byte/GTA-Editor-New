#include "GTACheckValueSerializer.h"
#include "GTAUtil.h"
#include "GTACommands.h"
#include "GTACheckBase.h"
#include "GTACommentSerializer.h"
#include "GTACheckValue.h"
#include "GTARequirementSerializer.h"
GTACheckValueSerializer::GTACheckValueSerializer() : GTACmdSerializerInterface()
{
    _pCheckCmd = NULL;
}
GTACheckValueSerializer::~GTACheckValueSerializer() 
{
    _pCheckCmd = NULL;
}
void GTACheckValueSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pCheckCmd = dynamic_cast<GTACheckValue*>((GTACommandBase*) iPCmd);
}
bool GTACheckValueSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)//(QDomDocument &iDomDoc,QDomElement &oElement)
{
    bool rc = false;
    if(_pCheckCmd != NULL)
    {

        QString sTimeOut , sTimeUnit;
        _pCheckCmd->getTimeOut(sTimeOut,sTimeUnit) ;

        QString sPrecision,sPrecisionUnit;
        _pCheckCmd->getPrecision(sPrecision,sPrecisionUnit) ;

        QString sActionOnFail =_pCheckCmd->getActionOnFail() ;

        int totalParameters = _pCheckCmd->getCount();
        QString waitUntil = _pCheckCmd->getWaitUntil() ? "TRUE" : "FALSE";

        // double dConfirmationCheckTime ;//= _pCheckCmd->getConfCheckTime();

        QString confTime,confTimeUnit;
        
        _pCheckCmd->getConfCheckTime(confTime,confTimeUnit);

        oElement  = iDomDoc.createElement(XNODE_CHECK);
        oElement.setAttribute(XNODE_NAME,CHK_VALUE);
        
        setActionAttributes(_pCheckCmd,oElement);
       
        QString lastEditedRow = QString::number(_pCheckCmd->getLastEditedRow());
        QString lastEditedCol = QString::number(_pCheckCmd->getLastEditedCol());
        oElement.setAttribute(XNODE_LAST_EDITED_ROW, lastEditedRow);
        oElement.setAttribute(XNODE_LAST_EDITED_COL, lastEditedCol);

        QDomElement ElemWaitUntil = iDomDoc.createElement(XNODE_WAIT);
        ElemWaitUntil.setAttribute(XNODE_VALUE,waitUntil);
        oElement.appendChild(ElemWaitUntil);

        QString freeTextCheck = _pCheckCmd->getFreeTextCondition();
        if(freeTextCheck.isEmpty())
        {
            for (int i=0;i<totalParameters;i++)
            {
                QDomElement domParamElement = iDomDoc.createElement(XNODE_PARAMETER);
                QString sParameter = _pCheckCmd->getParameter(i) ;


                QString sValue = _pCheckCmd->getValue(i) ;
                QString sCondition = _pCheckCmd->getCondition(i) ;
                QString sBinaryOperator = _pCheckCmd->getBinaryOperator(i) ;
                QString sFstatus = _pCheckCmd->getFunctionalStatus(i);
                double dPrecisionVal = _pCheckCmd->getPrecisionValue(i);
                QString sPrecisionVal = QString::number(dPrecisionVal,'f',4);
				if (dPrecisionVal == 0 && _pCheckCmd->getPrecisionValueFE(i) != "")
					sPrecisionVal = _pCheckCmd->getPrecisionValueFE(i);
                QString sPrecisionType = _pCheckCmd->getPrecisionType(i);
                QString sIsChkOnlyValue = _pCheckCmd->getISOnlyCheckValue(i)?"true":"false";
                QString sIsChkFS =  _pCheckCmd->getISCheckFS(i)?"true":"false";
                QString isChkSDIOnly = _pCheckCmd->getIsSDIOnly(i)? "true":"false";
                QString isChkParityOnly = _pCheckCmd->getIsParityOnly(i)? "true":"false";
                QString isChkRefreshOnly = _pCheckCmd->getIsRefreshRateOnly(i)? "true":"false";
                QString sSDI = _pCheckCmd->getSDI(i);
                QString sParity = _pCheckCmd->getParity(i);

                domParamElement.setAttribute(XATTR_NAME,sParameter);
                domParamElement.setAttribute(XATTR_VAL,sValue);
                domParamElement.setAttribute(XATTR_OPERATOR,sCondition);
                domParamElement.setAttribute(XATTR_FS,sFstatus);
                domParamElement.setAttribute(XATTR_PRECISION_VAL, sPrecisionVal);
                domParamElement.setAttribute(XATTR_PRECISION_TYPE,sPrecisionType);
                domParamElement.setAttribute(XATTR_CHK_ONLY_VALUE,sIsChkOnlyValue);
                domParamElement.setAttribute(XATTR_CHK_FS,sIsChkFS);
                domParamElement.setAttribute(XNODE_CHK_SDI_ONLY,isChkSDIOnly);
                domParamElement.setAttribute(XNODE_CHK_PARITY_ONLY,isChkParityOnly);
                domParamElement.setAttribute(XNODE_CHK_REFRESH_RATE_ONLY,isChkRefreshOnly);
                domParamElement.setAttribute(XNODE_SDI_STATUS,sSDI);
                domParamElement.setAttribute(XNODE_PARITY_STATUS,sParity);

                oElement.appendChild(domParamElement);
                if (!sBinaryOperator.isEmpty())
                {
                    QDomElement operatorNode = iDomDoc.createElement(XNODE_OPERATOR);
                    QDomText msgTxtNode = iDomDoc.createTextNode(sBinaryOperator);
                    operatorNode.appendChild(msgTxtNode);
                    oElement.appendChild(operatorNode);

                }
            }
        }
        else
        {

            QDomElement ElemFreeText = iDomDoc.createElement(XNODE_FREE_TEXT);
            ElemFreeText.setAttribute(XNODE_FREE_TEXT,freeTextCheck);
            oElement.appendChild(ElemFreeText);
        }


        QStringList sDumpList;
        if (sActionOnFail == ACT_FAIL_STOP)
        {
            sDumpList = _pCheckCmd->getDumpList();
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
        }
        //comment node
        GTACommentSerializer commentSerializer(_pCheckCmd);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }


        GTARequirementSerializer requirementSerializer(_pCheckCmd);
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

bool GTACheckValueSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)//(const QDomElement & ioElement)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString actionName = iElement.attribute(XNODE_NAME);
        QString sOnFail = iElement.attribute(XNODE_ONFAIL);

        QStringList dumpList;


        GTACheckValue* pChk = new GTACheckValue;

        int lastEditedRow = iElement.hasAttribute(XNODE_LAST_EDITED_ROW) ? iElement.attribute(XNODE_LAST_EDITED_ROW).toInt() : 0;
        int lastEditedCol = iElement.hasAttribute(XNODE_LAST_EDITED_COL) ? iElement.attribute(XNODE_LAST_EDITED_COL).toInt() : 0;
        pChk->setLastEditedRow(lastEditedRow);
        pChk->setLastEditedCol(lastEditedCol);

        getActionAttributes(iElement,pChk);
        QDomNodeList childList = iElement.childNodes();
        for(int i = 0; i < childList.count(); i++)
        {
            QDomNode childNode = childList.at(i);
            QDomElement childElem = childNode.toElement();
            QString sTagName = childElem.tagName();

            if (sTagName==XNODE_PARAMETER)
            {
                QString sParameter = childElem.attribute(XNODE_NAME);
                QString sCondition=childElem.attribute(XNODE_OPERATOR);
                QString sValue=childElem.attribute(XATTR_VAL);
                QString sFunctionStatus = childElem.attribute(XATTR_FS);
                QString sPrecisionValue = childElem.attribute(XATTR_PRECISION_VAL);
                double dPrecisionValue = sPrecisionValue.toDouble();
                QString sPrecisionType = childElem.attribute(XATTR_PRECISION_TYPE);
                bool bIsCheckOnlyValue = childElem.attribute(XATTR_CHK_ONLY_VALUE)=="true"?true:false;
                bool bIsCheckFs = childElem.attribute(XATTR_CHK_FS)=="true"?true:false;
                bool bIsCheckSDI = childElem.attribute(XNODE_CHK_SDI_ONLY)== "true"? true:false;
                bool bIsCheckParity = childElem.attribute(XNODE_CHK_PARITY_ONLY)== "true"? true:false;
                bool bIsCheckRefresh = childElem.attribute(XNODE_CHK_REFRESH_RATE_ONLY)== "true"? true:false;
                QString sSDI = childElem.attribute(XNODE_SDI_STATUS);
                QString sParity = childElem.attribute(XNODE_PARITY_STATUS);

                pChk->insertParamenter(sParameter,sCondition,sValue,sFunctionStatus,
                                       dPrecisionValue,sPrecisionValue,sPrecisionType,bIsCheckOnlyValue,
                                       bIsCheckFs,bIsCheckSDI,bIsCheckParity,
                                       bIsCheckRefresh,sSDI,sParity);

                
            }
            else if (sTagName==XNODE_OPERATOR)
            {
                QString sBinaryOperator=childElem.text();
                if (!sBinaryOperator.isEmpty())
                    pChk->insertBinaryOperator(sBinaryOperator);
            }
            else if(sTagName == XNODE_WAIT)
            {
                bool waitUntil = childElem.attribute(XNODE_VALUE).toUpper() == "TRUE" ? true : false ;
                pChk->setWaitUntil(waitUntil);
            }
            else if(sTagName == XNODE_DUMPLIST && sOnFail == ACT_FAIL_STOP)
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
                    pChk->setDumpList(dumpList);
            }
            else if(sTagName == XNODE_FREE_TEXT)
            {
                QString freeTextCheck = childElem.attribute(XNODE_FREE_TEXT);
                pChk->setFreeTextCondition(freeTextCheck);
            }

        }


        rc = true;
        opCommand=pChk;

        //comment node
        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);


        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);
    }
    return rc;

}

#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1").arg(CHK_VALUE),GTACheckValueSerializer)

