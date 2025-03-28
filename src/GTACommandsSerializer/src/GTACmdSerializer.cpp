#include "GTACmdSerializer.h"
#include "GTAUtil.h"

GTACmdSerializer::GTACmdSerializer()
{
}
bool GTACmdSerializer::setActionAttributes(GTAActionBase* pAct,  QDomElement& actionElementNode)
{
    bool rc = false;
    if (pAct!=NULL && actionElementNode.isNull()==false)
    {

        QString actionName = pAct->getAction();
        QString complmentName = pAct->getComplement();
        actionElementNode.setAttribute(XNODE_NAME,actionName);
        actionElementNode.setAttribute(XNODE_COMPLEMENT,complmentName);


        QString onFail = pAct->getActionOnFail();
        actionElementNode.setAttribute(XNODE_ONFAIL,onFail);

        if(pAct->hasTimeOut())
        {
            QString sTimeOut,sTimeOutUnit;
            pAct->getTimeOut( sTimeOut,sTimeOutUnit);
            actionElementNode.setAttribute(XNODE_TIMEOUT,sTimeOut);
            actionElementNode.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);
        }

        if (pAct->hasPrecision())
        {
            QString sPrecision,sPrecisionUnit;

            pAct->getPrecision(sPrecision,sPrecisionUnit);
            actionElementNode.setAttribute(XNODE_PRECISION,sPrecision);
            actionElementNode.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit);
        }

        QString userfeedback = pAct->getUserComment();
        actionElementNode.setAttribute(XNODE_USER_FEEDBACK,userfeedback);

        QString sResult = pAct->getExecutionResult();
        QString sExecutionTime = pAct->getExecutionTime();
        QString sReferences = pAct->getReferences();


        actionElementNode.setAttribute(XNODE_EXEC_RESULT,sResult);
        actionElementNode.setAttribute(XNODE_EXEC_TIME,sExecutionTime);
        actionElementNode.setAttribute(XNODE_RES_REF,sReferences);

        QString confTime,confTimeUnit;
        pAct->getConfCheckTime(confTime,confTimeUnit);
        actionElementNode.setAttribute(XATTR_CONF_TIME,confTime);
        actionElementNode.setAttribute(XATTR_CONF_UNIT,confTimeUnit);

        QString hiddenInSCXML = pAct->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        actionElementNode.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);

        QString isReadOnly = pAct->getReadOnlyState() ? XNODE_TRUE : XNODE_FALSE;
        actionElementNode.setAttribute(XATTR_READ_ONLY,isReadOnly);

        QString hasBreakpoint = pAct->hasBreakpoint() ? XNODE_TRUE : XNODE_FALSE;
        actionElementNode.setAttribute(XATTR_BREAKPOINT,hasBreakpoint);

        rc = true;
    }

    return rc;
}
bool GTACmdSerializer::setActionAttributes(GTACheckBase* pChk,  QDomElement& actionElementNode)
{
    bool rc = false;
    if (pChk!=NULL && actionElementNode.isNull()==false)
    {


        rc = true;
        QString onFail = pChk->getActionOnFail();
        actionElementNode.setAttribute(XNODE_ONFAIL,onFail);


        QString sTimeOut,sTimeOutUnit;
        pChk->getTimeOut( sTimeOut,sTimeOutUnit);
        actionElementNode.setAttribute(XNODE_TIMEOUT,sTimeOut);
        actionElementNode.setAttribute(XNODE_TIMEOUT_UNIT,sTimeOutUnit);



        QString sPrecision,sPrecisionUnit;

        pChk->getPrecision(sPrecision,sPrecisionUnit);
        actionElementNode.setAttribute(XNODE_PRECISION,sPrecision);
        actionElementNode.setAttribute(XNODE_PRECISION_UNIT,sPrecisionUnit); 



        QString userfeedback = pChk->getUserComment();
        actionElementNode.setAttribute(XNODE_USER_FEEDBACK,userfeedback);



        QString sResult = pChk->getExecutionResult();
        QString sExecutionTime = pChk->getExecutionTime();
        QString sReferences = pChk->getReferences();


        actionElementNode.setAttribute(XNODE_EXEC_RESULT,sResult);
        actionElementNode.setAttribute(XNODE_EXEC_TIME,sExecutionTime);
        actionElementNode.setAttribute(XNODE_RES_REF,sReferences);

        QString confTime,confTimeUnit;
        pChk->getConfCheckTime(confTime,confTimeUnit);
        actionElementNode.setAttribute(XATTR_CONF_TIME,confTime);
        actionElementNode.setAttribute(XATTR_CONF_UNIT,confTimeUnit);

        QString hiddenInSCXML = pChk->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        actionElementNode.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);

        QString isReadOnly = pChk->getReadOnlyState() ? XNODE_TRUE : XNODE_FALSE;
        actionElementNode.setAttribute(XATTR_READ_ONLY,isReadOnly);


        QString hasBreakpoint = pChk->hasBreakpoint() ? XNODE_TRUE : XNODE_FALSE;
        actionElementNode.setAttribute(XATTR_BREAKPOINT,hasBreakpoint);

        rc = true;
    }

    return rc;
}
bool GTACmdSerializer::getActionAttributes(const QDomElement& iElement,GTAActionBase* pAct)
{

    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION && pAct!=NULL)
    {

        rc =true;
        if(pAct->getAction().isEmpty())
        {
            QString actionName =        iElement.attribute(XNODE_NAME);
            pAct->setAction(actionName);
        }
        if (pAct->getComplement().isEmpty())
        { 
            QString complementName =    iElement.attribute(XNODE_COMPLEMENT);
            pAct->setComplement(complementName);
        }
        if (pAct != NULL)
        {
            QString uuid =    iElement.attribute(XNODE_UUID);
            if (uuid=="")
                pAct->setObjId();
        }

        QString sActionOnFail =     iElement.attribute(XNODE_ONFAIL);
        pAct->setActionOnFail(sActionOnFail);

        if(pAct->hasTimeOut())
        {
            QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
            QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
            pAct->setTimeOut(sTimeOut,sTimeOutUnit);
        }
        if (pAct->hasPrecision())
        {
            QString sPrecision = iElement.attribute(XNODE_PRECISION);
            QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
            pAct->setPrecision(sPrecision,sPrecisionUnit);
        }

        QString sUserFeedback = iElement.attribute(XNODE_USER_FEEDBACK);
        pAct->updateUserComments(sUserFeedback);

        QString sResult = iElement.attribute(XNODE_EXEC_RESULT);
        QString sExecutionTime = iElement.attribute(XNODE_EXEC_TIME);
        QString sReferences =iElement.attribute(XNODE_RES_REF);

        pAct->setExecutionTime(sExecutionTime);
        pAct->setExecutionResult(sResult);
        pAct->setReferences(sReferences);


        QString sConftime = iElement.attribute(XATTR_CONF_TIME);
        QString sConftimeUnit = iElement.attribute(XATTR_CONF_UNIT);
        pAct->setConfCheckTime(sConftime,sConftimeUnit);

        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        pAct->setIgnoreInSCXML(ignoreInSCXML);

        bool isReadOnly = iElement.attribute(XATTR_READ_ONLY).toUpper()=="TRUE"?true:false;
        pAct->setReadOnlyState(isReadOnly);

        bool hasBreakpoint = iElement.attribute(XATTR_BREAKPOINT).toUpper() == XNODE_TRUE ? true:false;
        pAct->setBreakpoint(hasBreakpoint);

    }

    return rc;
}
bool GTACmdSerializer::getActionAttributes(const QDomElement& iElement,GTACheckBase* pChk)
{

    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_CHECK&& pChk!=NULL)
    {

        rc =true;
        

        QString sActionOnFail =     iElement.attribute(XNODE_ONFAIL);
        pChk->setActionOnFail(sActionOnFail);


        QString sTimeOut = iElement.attribute(XNODE_TIMEOUT);
        QString sTimeOutUnit = iElement.attribute(XNODE_TIMEOUT_UNIT);
        pChk->setTimeOut(sTimeOut,sTimeOutUnit);


        QString sPrecision = iElement.attribute(XNODE_PRECISION);
        QString sPrecisionUnit = iElement.attribute(XNODE_PRECISION_UNIT);
        pChk->setPrecision(sPrecision,sPrecisionUnit);


        QString sConftime = iElement.attribute(XATTR_CONF_TIME);
        QString sConftimeUnit = iElement.attribute(XATTR_CONF_UNIT);
        pChk->setConfCheckTime(sConftime,sConftimeUnit);


        QString sUserFeedback = iElement.attribute(XNODE_USER_FEEDBACK);
        pChk->updateUserComments(sUserFeedback);


        QString sResult = iElement.attribute(XNODE_EXEC_RESULT);
        QString sExecutionTime = iElement.attribute(XNODE_EXEC_TIME);
        QString sReferences =iElement.attribute(XNODE_RES_REF);

        pChk->setExecutionTime(sExecutionTime);
        pChk->setExecutionResult(sResult);
        pChk->setReferences(sReferences);


        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        pChk->setIgnoreInSCXML(ignoreInSCXML);

        bool isReadOnly = iElement.attribute(XATTR_READ_ONLY).toUpper()=="TRUE"?true:false;
        pChk->setReadOnlyState(isReadOnly);


        bool hasBreakpoint = iElement.attribute(XATTR_BREAKPOINT).toUpper() == XNODE_TRUE ? true:false;
        pChk->setBreakpoint(hasBreakpoint);

    }

    return rc;
}
