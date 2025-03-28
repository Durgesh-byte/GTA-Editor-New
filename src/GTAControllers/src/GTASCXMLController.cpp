#include "GTASCXMLController.h"
#include "GTAScxmlLogMessage.h"
#include "GTAUtil.h"
#include "GTAActionParentTitleEnd.h"
#include "GTAOneClickLaunchApplication.h"

#pragma warning(push, 0)
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#pragma warning(pop)

GTASCXMLController::GTASCXMLController(const QString &iTemplatePath)
{
    _pElement = NULL;
    _isPirInit = false;
    _ScxmlTemplatePath = iTemplatePath;
    StateIdCntr = 0;
    _isGenericSCXMLCofig =false;
    _pAppCtrl = GTAAppController::getGTAAppController();
    _isGenericSCXMLCofig = TestConfig::getInstance()->getGenericSCXML();
}

void GTASCXMLController::GetParamList( QStringList &iVariableList)
{

    if(_pAppCtrl != NULL)
    {
        GTADataController DataCtrl;
        QStringList InvalidParamList;
        iVariableList.removeDuplicates();

        QStringList tempItems;
        QList<QStringList> complexQuery;
        int cntr = 0;
        foreach(QString item , iVariableList)
        {
            tempItems.append(item);
            if(cntr == 100)
            {
                complexQuery.append(tempItems);
                tempItems.clear();
                cntr=0;
            }
            cntr++;
        }
        complexQuery.append(tempItems);
        foreach(QStringList items, complexQuery)
        {
            DataCtrl.getParametersDetail(items,_pAppCtrl->getGTADbFilePath(),_icdParamList,InvalidParamList);
        }
    }
}

bool GTASCXMLController::exportToScxml(const GTAElement * ipElement,const QString &iFilePath, bool, bool)
{
    bool rc = false;
    if(ipElement == NULL)
    {
        _LastError = "Error: Element is NULL";
        return rc;
    }

    _isPirInit = TestConfig::getInstance()->getPIRInvokeForSCXML();

    QStringList variableList;
    _pElement = (GTAElement *)ipElement;
    setElementName(_pElement,iFilePath);
    _pElement->updateCommandInstanceName(QString());

    GTAElement* pTitleBasedElement = new GTAElement(_pElement->getElementType());



    GroupElementByTitleComplete(_pElement,variableList, pTitleBasedElement);



    _IntermediateElemList.append(pTitleBasedElement);




    pTitleBasedElement->setName(ipElement->getName());


    QString mainStateId  = "main_state";
    QString initParamListId = "InitParamList";
    QString initUnSubscribe = "InitUnSubscribeAll";
    QString unsubscribeEnd  = "UnSubscribeAllEnd";



    GTASCXMLState mainState;
    mainState.setId(mainStateId);

    GTASCXMLFinal mainStatefinal;
    mainStatefinal.setId(QString("%1_final").arg(mainState.getId()));
    mainState.setFinal(mainStatefinal);

    GTASCXMLTransition mainStateTx;
    if(!_isGenericSCXMLCofig)
        mainStateTx.setTarget(unsubscribeEnd);
    else
        mainStateTx.setTarget(SCXML_FINAL_STATE_ID);

    mainState.insertTransitions(mainStateTx);

    // update the list of parameters from the database.
    GetParamList(variableList);


    rc = getStateChart(pTitleBasedElement,mainState);

    GTASCXMLSCXML scxml;

    //insert Datamodel in scxmlNode
    createSCXMLDataModel(scxml);



    GTASCXMLState unInitState1;
    getUnInitState(unInitState1,initUnSubscribe,initParamListId);

    GTASCXMLState unInitState2;
    getUnInitState(unInitState2,unsubscribeEnd,SCXML_FINAL_STATE_ID);


    GTASCXMLState initState;
    getInitStateForApplications(initState,initParamListId,mainStateId);

    if(!_isGenericSCXMLCofig)
    {
        scxml.insertState(unInitState1);
        scxml.insertState(initState);
        scxml.insertState(mainState);
        scxml.insertState(unInitState2);
    }
    else
    {
        scxml.insertState(mainState);
    }

    GTASCXMLFinal scxmlFinal;
    scxmlFinal.setId(SCXML_FINAL_STATE_ID);

    scxml.setFinalState(scxmlFinal);

    QDomDocument iDomDoc;
    QDomElement scxmlElem = scxml.getSCXML(iDomDoc);
    iDomDoc.appendChild(scxmlElem);

    QFile outFile(iFilePath);
    rc = outFile.open(QFile::WriteOnly|QFile::Text);
	if (rc)
	{
		QTextStream writer(&outFile);
		QString xmlContents = iDomDoc.toString();
		// replacing all standard xml representation of linefeed and carriage return by \n and \r
		xmlContents.replace("&#xa;", "");
		//xmlContents.replace("&#xa;","&lt;br>&lt;/br>");
		xmlContents.replace("&#xd;", "");
		//xmlContents.replace("")
		foreach(QString stateIDs, _stateIdMap.keys())
		{
			//xmlContents.replace(QString("\"%1\"").arg(stateIDs),QString("\"%1\"").arg(_stateIdMap[stateIDs]));
		}
		writer << xmlContents;
		outFile.close();
	}
	return rc;
}

bool GTASCXMLController::getStateChart(const GTAElement *pElem,GTASCXMLState &ioState)
{
    bool rc =false;
    int directChildrenCnt = pElem->getDirectChildrenCount();
    QString parentStateId = ioState.getId();
    QString parentStateFinalId = QString("%1_final").arg(parentStateId);

    for(int i=0;i<directChildrenCnt;i++)
    {
        QString stateId = QString("%1_s%2").arg(parentStateId,QString::number(i));
        QString targetId = QString("%1_s%2").arg(parentStateId,QString::number(i+1));
        if(i == (directChildrenCnt - 1))
        {
            targetId = parentStateFinalId;
        }
        GTACommandBase *pCmd = NULL;
        pElem->getDirectChildren(i,pCmd);
        if(pCmd != NULL)
        {
            createStateNodeForCommand(pCmd,ioState,stateId,targetId);
        }
    }
    return rc;
}
bool GTASCXMLController::getStateChart(const GTACommandBase *pCmd,GTASCXMLState &ioState)
{
    bool rc =false;
    QList<GTACommandBase *> childrens = pCmd->getChildren();
    QString parentStateId = ioState.getId();
    QString parentStateFinalId = QString("%1_final").arg(parentStateId);

    for(int i=0;i<childrens.count();i++)
    {
        QString stateId = QString("%1_s%2").arg(parentStateId,QString::number(i));
        QString targetId = QString("%1_s%2").arg(parentStateId,QString::number(i+1));
        if(i == childrens.count() - 1)
        {
            targetId = parentStateFinalId;
        }
        GTACommandBase *pSubCmd = childrens.at(i);
        if(pCmd != NULL)
        {
            createStateNodeForCommand(pSubCmd,ioState,stateId,targetId);
        }
    }
    return rc;
}
void GTASCXMLController::createStateNodeForCommand(GTACommandBase *pCmd,GTASCXMLState &ioState, const QString &iStateId, const QString &iTargetId)
{
    if(pCmd != NULL)
    {
        // to define the name for the state id based on the command.
        setStateIDName(iStateId,pCmd);

        cmdParameterList.append(pCmd->getVariableList());

        if(pCmd->getCommandType() == GTACommandBase::ACTION)
        {
            GTAActionBase *pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
            QString act = pActCmd->getAction();
            QString com = pActCmd->getComplement();
            if(act == ACT_PRINT)
            {
                if(com == COM_PRINT_MSG)
                {
                    GTAActionPrint *pPrintCmd = dynamic_cast<GTAActionPrint *>(pActCmd);
                    createSCXMLPrintMessage(pPrintCmd,ioState,iStateId,iTargetId,QStringList());
                }
                else if(com == COM_PRINT_PARAM)
                {
                    GTAActionPrint *pPrintCmd = dynamic_cast<GTAActionPrint *>(pActCmd);
                    createSCXMLPrintParameter(pPrintCmd,ioState,iStateId,iTargetId,QStringList());
                }
                else if(com == COM_PRINT_TABLE)
                {
                    GTAActionPrintTable * pPrintTableCmd = dynamic_cast<GTAActionPrintTable *>(pActCmd);
                    createSCXMLPrintTable(pPrintTableCmd,ioState,iStateId,iTargetId,QStringList());
                }
                else if(com == COM_PRINT_TIME)
                {
                    GTAActionPrintTime * pPrintTime = dynamic_cast<GTAActionPrintTime *>(pActCmd);
                    createSCXMLPrintTime(pPrintTime,ioState,iStateId,iTargetId,QStringList());
                }
            }
            else if(act == ACT_SUBSCRIBE)
            {
                GTAActionSubscribe *pSubscribeCmd = dynamic_cast<GTAActionSubscribe *>(pActCmd);
                createSCXMLSubscribe(pSubscribeCmd,ioState,iStateId,iTargetId,QStringList());
            }
            else if(act == ACT_UNSUBSCRIBE)
            {
                GTAActionUnSubscribe *pUnSubscribeCmd = dynamic_cast<GTAActionUnSubscribe *>(pActCmd);
                createSCXMLUnSubscribe(pUnSubscribeCmd,ioState,iStateId,iTargetId,QStringList());
            }
            else if(act == ACT_GEN_TOOL)
            {
                GTAGenericToolCommand *pGenToolCmd = dynamic_cast<GTAGenericToolCommand *>(pActCmd);
                createSCXMLGenericTool(pGenToolCmd,ioState,iStateId,iTargetId,QStringList());

            }
            else if(act == ACT_TITLE)
            {
                GTAActionParentTitle *pTitleCmd = dynamic_cast<GTAActionParentTitle *>(pActCmd);
                createSCXMLTitle(pTitleCmd,ioState,iStateId,iTargetId,QStringList()); 
            }
            else if(act == ACT_WAIT)
            {

                if(com == COM_WAIT_FOR)
                {
                    GTAActionWait *pWaitCmd = dynamic_cast<GTAActionWait *>(pActCmd);
                    createSCXMLWaitFor(pWaitCmd,ioState,iStateId,iTargetId,QStringList());
                }
                else if(com == COM_WAIT_UNTIL)
                {
                    GTAActionWait *pWaitCmd = dynamic_cast<GTAActionWait *>(pActCmd);
                    createSCXMLWaitUntil(pWaitCmd,ioState,iStateId,iTargetId);
                }
            }
            else if(act == ACT_REL)
            {
                GTAActionRelease *pReleaseCmd = dynamic_cast<GTAActionRelease *>(pActCmd);
                createSCXMLReleaseParam(pReleaseCmd,ioState,iStateId,iTargetId,QStringList());
            }
            else if(act == ACT_CALL_EXT)
            {
                GTAActionExpandedCall *pCallCmd = dynamic_cast<GTAActionExpandedCall *>(pActCmd);
                createSCXMLCall(pCallCmd,ioState,iStateId,iTargetId,QStringList());
            }
            else if(act == ACT_CONDITION)
            {
                if(com == COM_CONDITION_IF)
                {
                    GTAActionIF *pIfCmd = dynamic_cast<GTAActionIF *>(pActCmd);
                    createSCXMLConditionIf(pIfCmd,ioState,iStateId,iTargetId,QStringList());
                }
                else if(com == COM_CONDITION_WHILE)
                {
                    GTAActionWhile *pWhile = dynamic_cast<GTAActionWhile *>(pActCmd);
                    createSCXMLConditionWhile(pWhile,ioState,iStateId,iTargetId,QStringList());
                }
                else if(com == COM_CONDITION_DO_WHILE)
                {
                    GTAActionDoWhile *pDoWhile = dynamic_cast<GTAActionDoWhile *>(pActCmd);
                    createSCXMLConditionDoWhile(pDoWhile,ioState,iStateId,iTargetId,QStringList());
                }
            }
            else if(act == ACT_SET)
            {
                GTAActionSetList *pSetList = dynamic_cast<GTAActionSetList*>(pActCmd);
                createSCXMLSetList(pSetList,ioState,iStateId,iTargetId,QStringList());
            }
            else if(act == ACT_MANUAL)
            {
                GTAActionManual *pManualActCmd = dynamic_cast<GTAActionManual *>(pActCmd);
                createSCXMLManualAction(pManualActCmd,ioState,iStateId,iTargetId,QStringList());
            }
            else if(act == ACT_GEN_FUNC)
            {
                GTAActionGenericFunction *pGenFunc = dynamic_cast<GTAActionGenericFunction * >(pActCmd);
                createSCXMLGenericFunction(pGenFunc,ioState,iStateId,iTargetId,QStringList());
            }
            else if(act == ACT_MATHS)
            {
                GTAActionMaths *pMaths = dynamic_cast<GTAActionMaths * >(pActCmd);
                createSCXMLMaths(pMaths,ioState,iStateId,iTargetId,QStringList());
            }
            else if(act == ACT_ONECLICK)
            {
                if(com == COM_ONECLICK_TITLE)
                {

                }
                else if(com == COM_ONECLICK_PINPROG)
                {
                    GTAActionOneClickPPC *pPPCCmd = dynamic_cast<GTAActionOneClickPPC * >(pActCmd);
                    createSCXMLOneClickPPC(pPPCCmd,ioState,iStateId,iTargetId,QStringList());
                }
                else if(com == COM_ONECLICK_ENV)
                {
                    GTAOneClick *pOneClickEnvCmd = dynamic_cast<GTAOneClick * >(pActCmd);
                    createSCXMLOneClickEnv(pOneClickEnvCmd,ioState,iStateId,iTargetId,QStringList());
                }
                else if(com == COM_ONECLICK_APP)
                {
                    GTAOneClickLaunchApplication *pOneClickAppCmd = dynamic_cast<GTAOneClickLaunchApplication * >(pActCmd);
                    createSCXMLOneClickApplication(pOneClickAppCmd,ioState,iStateId,iTargetId,QStringList());

                }
            }
        }
        else if(pCmd->getCommandType() == GTACommandBase::CHECK)
        {
            GTACheckBase * pCheckCmd = dynamic_cast<GTACheckBase * >(pCmd);
            GTACheckBase::CheckType chkType = pCheckCmd->getCheckType();
            if(chkType == GTACheckBase::VALUE)
            {
                GTACheckValue *pChkValCmd = dynamic_cast<GTACheckValue * >(pCmd);
                createSCXMLCheckValue(pChkValCmd,ioState,iStateId,iTargetId,QList<GTASCXMLTransition>(),QString(""));
            }
            else if(chkType == GTACheckBase::FWC_WARNING)
            {
                GTACheckFwcWarning *pFWCCmd = dynamic_cast<GTACheckFwcWarning * >(pCmd);
                createSCXMLCheckFWCWarning(pFWCCmd,ioState,iStateId,iTargetId,QStringList());
            }
            else if(chkType == GTACheckBase::BITE_MESSAGE)
            {
                GTACheckBiteMessage *pBiteCmd = dynamic_cast<GTACheckBiteMessage * >(pCmd);
                createSCXMLCheckBITEMessage(pBiteCmd,ioState,iStateId,iTargetId,QStringList());
            }
            else if(chkType == GTACheckBase::AUDIO_ALARM)
            {
                GTACheckAudioAlarm *pAudioAlarm = dynamic_cast<GTACheckAudioAlarm *>(pCmd);
                createSCXMLCheckAudioAlarm(pAudioAlarm,ioState,iStateId,iTargetId,QStringList());
            }
        }
        else
        {

        }
    }
}




void GTASCXMLController::GroupElementByTitleComplete(const GTAElement *pElem, QStringList &oVariableList, GTAElement *oGroupedElem)
{
    int cmdCnt = pElem->getDirectChildrenCount();
    // int GloblPosition = 0;
    GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
    pTitle->setTitle("StartOfTestProcedure");
    GTAActionParentTitle *pTitleCmd = new GTAActionParentTitle(pTitle,"");
    int titleChildCnt = 0;
    for(int i=0;i<cmdCnt;i++)
    {
        GTACommandBase *pCmd1 = NULL;
        pElem->getDirectChildren(i,pCmd1);
        if(pCmd1 != NULL)
        {
            GTACommandBase *pCmd = pCmd1->getClone();
            if(pCmd == NULL)
                continue;
            if(pCmd->isIgnoredInSCXML())
                continue;
            oVariableList.append(pCmd->getVariableList());
            if(pCmd->isTitle())
            {
                if(pTitleCmd != NULL)
                {
                    //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
                    oGroupedElem->appendCommand(pTitleCmd);
                }
                GTAActionTitle *pActTitleCmd = dynamic_cast<GTAActionTitle*>(pCmd);
                pTitleCmd = new GTAActionParentTitle(pActTitleCmd,"");
                titleChildCnt = 0;
            }
            else if(pTitleCmd && pCmd->hasReference())
            {
                GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                if(pCallCmd != NULL)
                {
                    GTAActionExpandedCall *pExtCall = new GTAActionExpandedCall(*pCallCmd);
                    QString callElemName = pCallCmd->getElement();
                    QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
                    pExtCall->setTagValue(pCallCmd->getTagValue());

                    //Load the call document in the element;
                    GTAElement* pElemOnCall=NULL;
                    GTAAppController* pController = GTAAppController::getGTAAppController();
                    bool rc = pController->getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pElemOnCall,true);

                    if(rc && pElemOnCall != NULL )
                    {
                        pElemOnCall->updateCommandInstanceName(pCallCmd->getInstanceName());
                        pElemOnCall->replaceTag(pCallCmd->getTagValue());

                        GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);
                        GroupElementByTitleComplete(pElemOnCall,oVariableList,groupedCallElem);
                        if(groupedCallElem != NULL)
                        {

                            groupedCallElem->replaceTag(pCallCmd->getTagValue());
                            //groupedCallElem->updateCommandInstanceName(pCallCmd->getInstanceName());
                            int size =groupedCallElem->getDirectChildrenCount();
                            int callChildCntr = 0;
                            for (int j=0;j<size;j++)
                            {
                                GTACommandBase* pCurCmd = NULL;
                                groupedCallElem->getDirectChildren(j,pCurCmd);

                                if (pCurCmd != NULL)
                                {
                                    pExtCall->insertChildren(pCurCmd,callChildCntr++);
                                }
                            }
                            if(pTitleCmd != NULL)
                            {
                                pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                            }
                        }
                    }
                    else
                    {
                        pExtCall->setBrokenLink(true);
                    }
                }

            }
            else if(pTitleCmd)// && pCmd->canBeCopied())
            {

                if(pCmd->canHaveChildren())
                {
                    GTAActionParentTitle *childrenTitle = NULL;//new GTAActionParentTitle(NULL,"");
                    createCommandGroupedByTitle(pCmd,childrenTitle,oVariableList);
                    if(childrenTitle != NULL)
                    {
                        QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                        if(childChildren.count()> 0)
                        {

                            pCmd->clearChildrenList();
                            for(int j=0;j<childChildren.count();j++)
                            {
                                GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                if(pchildTitleCmd != NULL)
                                    pCmd->insertChildren(pchildTitleCmd,j);
                            }
                        }
                    }
                }
                pTitleCmd->insertChildren(pCmd,titleChildCnt++);

            }
        }
    }
    if(pTitleCmd != NULL)
    {
        //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
        oGroupedElem->appendCommand(pTitleCmd);
    }
}
void GTASCXMLController::createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle *&ipTitleCmd,QStringList &oVariableList)
{
    int GlobalTitleCnt = 0;//ipTitleCmd->getChildrens().size();
    if(ipCmd != NULL)
    {
        QList<GTACommandBase*> children = ipCmd->getChildren();
        GTAActionTitle *pTitle = new GTAActionTitle(ACT_TITLE);
        pTitle->setTitle("StartOfTestProcedure");
        GTAActionParentTitle *pTitleCmd = new GTAActionParentTitle(pTitle,"");
        int titleChildCnt = 0;
        for(int i=0;i<children.count();i++)
        {
            GTACommandBase * pCmd1 = children.at(i);
            if(pCmd1 != NULL)
            {
                GTACommandBase *pCmd = pCmd1->getClone();
                if(pCmd == NULL)
                    continue;
                if(pCmd->isIgnoredInSCXML())
                    continue;
                oVariableList.append(pCmd->getVariableList());
                if(pCmd->isTitle())
                {
                    if(pTitleCmd != NULL)
                    {
                        if(ipTitleCmd != NULL)
                        {
                            GlobalTitleCnt = ipTitleCmd->getChildren().size();
                            //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
                            ipTitleCmd->insertChildren(pTitleCmd,GlobalTitleCnt++);
                        }
                        else
                        {
                            ipTitleCmd = new GTAActionParentTitle(*pTitleCmd);
                        }

                    }
                    GTAActionTitle *pActTitleCmd = dynamic_cast<GTAActionTitle*>(pCmd);
                    pTitleCmd = new GTAActionParentTitle(pActTitleCmd,"");
                    titleChildCnt = 0;

                }
                else if(pTitleCmd && pCmd->hasReference())
                {
                    GTAActionCall *pCallCmd = dynamic_cast<GTAActionCall *>(pCmd);
                    if(pCallCmd != NULL)
                    {
                        GTAActionExpandedCall *pExtCall = new GTAActionExpandedCall(*pCallCmd);
                        QString callElemName = pCallCmd->getElement();
                        QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
                        pExtCall->setTagValue(pCallCmd->getTagValue());

                        //Load the call document in the element;
                        GTAElement* pElemOnCall=NULL;
                        GTAAppController* pController = GTAAppController::getGTAAppController();
                        bool rc = pController->getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pElemOnCall,true);

                        if(rc && pElemOnCall != NULL )
                        {
                            GTAElement *groupedCallElem = new GTAElement(GTAElement::CUSTOM);
                            GroupElementByTitleComplete(pElemOnCall,oVariableList,groupedCallElem);
                            if(groupedCallElem != NULL)
                            {

                                groupedCallElem->replaceTag(pCallCmd->getTagValue());
                                //groupedCallElem->updateCommandInstanceName(pCallCmd->getInstanceName());
                                int size =groupedCallElem->getDirectChildrenCount();
                                int callChildCntr = 0;
                                for (int j=0;j<size;j++)
                                {
                                    GTACommandBase* pCurCmd = NULL;
                                    groupedCallElem->getDirectChildren(i,pCurCmd);

                                    if (pCurCmd != NULL)
                                    {
                                        pExtCall->insertChildren(pCurCmd,callChildCntr++);
                                    }
                                }
                                if(pTitleCmd != NULL)
                                {
                                    pTitleCmd->insertChildren(pExtCall,titleChildCnt++);
                                }
                            }
                        }
                        else
                        {
                            pExtCall->setBrokenLink(true);
                        }
                    }
                }
                else// if(pCmd->canBeCopied())
                {
                    if(pTitleCmd)
                    {
                        if(pCmd->canHaveChildren())
                        {
                            //TODO: Does this subtile do anything?
                            GTAActionTitle *pSubTitle = new GTAActionTitle(ACT_TITLE);
                            pSubTitle->setTitle("");
                            GTAActionParentTitle *childrenTitle = NULL;//new GTAActionParentTitle(NULL,"");
                            createCommandGroupedByTitle(pCmd,childrenTitle,oVariableList);
                            if(childrenTitle != NULL)
                            {
                                QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                                if(childChildren.count()> 0)
                                {

                                    pCmd->clearChildrenList();
                                    for(int j=0;j<childChildren.count();j++)
                                    {
                                        GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                        if(pchildTitleCmd != NULL)
                                            pCmd->insertChildren(pchildTitleCmd,j);
                                    }
                                }
                            }
                        }
                        pTitleCmd->insertChildren(pCmd,titleChildCnt++);
                    }
                    else
                    {
                        if(pCmd->canHaveChildren())
                        {
                            //TODO: Does this subtile do anything?
                            GTAActionTitle* pSubTitle = new GTAActionTitle(ACT_TITLE);
                            pSubTitle->setTitle("");
                            GTAActionParentTitle *childrenTitle = new GTAActionParentTitle(NULL,"");
                            createCommandGroupedByTitle(pCmd,childrenTitle,oVariableList);
                            //childrenTitle->insertChildren(new GTAActionParentTitleEnd,childrenTitle->getChildrens().size());
                            if(childrenTitle != NULL)
                            {
                                QList<GTACommandBase *> childChildren = childrenTitle->getChildren();
                                if(childChildren.count()> 0)
                                {

                                    pCmd->clearChildrenList();
                                    for(int j=0;j<childChildren.count();j++)
                                    {
                                        GTACommandBase *pchildTitleCmd =childChildren.at(j);
                                        if(pchildTitleCmd != NULL)
                                            pCmd->insertChildren(pchildTitleCmd,j);
                                    }
                                }
                            }
                        }
                        ipTitleCmd->insertChildren(pCmd,ipTitleCmd->getChildren().size());
                    }
                }


            }

        }
        if(pTitleCmd != NULL)
        {
            if(ipTitleCmd != NULL)
            {
                GlobalTitleCnt = ipTitleCmd->getChildren().size();
                //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
                ipTitleCmd->insertChildren(pTitleCmd,GlobalTitleCnt++);
            }
            else
            {
                ipTitleCmd = new GTAActionParentTitle(*pTitleCmd);

            }
            //pTitleCmd->insertChildren(new GTAActionParentTitleEnd,pTitleCmd->getChildrens().size());
        }
    }

}


void GTASCXMLController::setElementName(GTAElement* pElement,QString iSaveFilePath)
{
    _elementName = pElement->getName();
    if (_elementName.isEmpty())
    {
        QFileInfo fileInfo(iSaveFilePath);
        _elementName = fileInfo.completeBaseName();
    }


    QString sExtensionName = ".pro";
    GTAElement::ElemType  elemType = pElement->getElementType();
    switch (elemType)
    {
    case GTAElement::PROCEDURE:
        sExtensionName = ".pro";
        break;
    case GTAElement::SEQUENCE:
        sExtensionName = ".seq";
        break;
    case GTAElement::FUNCTION:
        sExtensionName = ".fun";
        break;
    case GTAElement::OBJECT:
        sExtensionName = ".obj";
        break;

    }
    _elementName.append(sExtensionName);
}


bool GTASCXMLController::getStateForParamGeneric(GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId,const QString &iParam)
{
    bool rc = false;
    if(!_icdParamList.contains(iParam))
        return rc;

    iState.setId(iStateId);
    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");
    GTASCXMLInvoke paramInvoke;
    rc = getInvokeForParamGeneric(paramInvoke,invokeId);
    if(rc)
    {
        GTASCXMLGenericFunction func = paramInvoke.getGenericFunction();
        func.updateAttributeByName("name",iParam);
        func.setFunctionReturn(iParam);
        paramInvoke.setInvokeFunctionGeneric(func);

        GTASCXMLAssign assign;
        assign.setExpression(QString("_event.data.%1").arg(iParam));
        assign.setLocation(iParam+".value");
        paramInvoke.insertAssign(assign);

        iState.insertInvokes(paramInvoke);
        GTASCXMLTransition tx1;
        tx1.setTarget(iTargetId);

        tx1.setEvent("done.invoke._all");
        iState.insertTransitions(tx1);
    }
    return rc;
}

bool GTASCXMLController::getStateForParam(GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId,const QString &iParam, const QString &iCmdName, bool iAddHiddenWaitTime)
{

    bool rc = false;

    if(_isGenericSCXMLCofig)
    {
        return getStateForParamGeneric(iState,iStateId,iTargetId,iParam);
    }

    QString delay;
    bool isPirWaitTimeSet = TestConfig::getInstance()->getPIRWaitTime();

    if(!_icdParamList.contains(iParam))
        return rc;

    iState.setId(iStateId);
    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");
    GTASCXMLInvoke paramInvoke;
    rc = getInvokeForParam(paramInvoke,iParam,invokeId,iCmdName);

    if(rc)
    {
        bool isHiddenWaitTime = false;
        GTASCXMLSend hiddenWaitTimeSend;
        QString hiddenWaitTimeEvent= QString("%1_hiddenWaitTime").arg(iStateId);
        QString invokeDelay;
        bool rc1 = getInvokeDealy(iCmdName,invokeDelay);
        if(rc1 && iAddHiddenWaitTime)
        {
            hiddenWaitTimeSend.setDelay(invokeDelay);
            hiddenWaitTimeSend.setEvent(hiddenWaitTimeEvent);
            //paramInvoke.insertSend(hiddenWaitTimeSend);
            GTASCXMLOnEntry onEntry = iState.getOnEntry();
            onEntry.insertSend(hiddenWaitTimeSend);
            iState.setOnEntry(onEntry);
            isHiddenWaitTime = true;
        }

        GTAICDParameter paramInfo = _icdParamList.value(iParam);
        if(!_isGenericSCXMLCofig)
        {
            if((paramInfo.getMedia().toLower() == "pir") && (_isPirInit == true))
            {
                QString initStateId = QString("%1_init").arg(iStateId);
                QString unInitStateId = QString("%1_unInit").arg(iStateId);
                QString paramStateId = QString("%1_getParam").arg(iStateId);
                QString finalStateId = QString("%1_finalParam").arg(iStateId);
                QString waitStateId = QString("%1_waitState").arg(iStateId);

                GTASCXMLState initState;
                getInitStateForParameter(initState,paramInfo,initStateId, waitStateId, finalStateId);

                GTASCXMLState waitState;
                QString waitTime = "";
                if(isPirWaitTimeSet)
                {
                    waitTime = delay+"ms";
                }
                else
                {
                    waitTime = "0";
                }
                getWaitState(NULL, waitTime,paramStateId,waitStateId,waitState);

                GTASCXMLState paramState;
                paramState.setId(paramStateId);
                paramState.insertInvokes(paramInvoke);

                GTASCXMLTransition tx1;
                tx1.setTarget(unInitStateId);

                if(!isHiddenWaitTime)
                    tx1.setEvent("done.invoke._all");
                else
                    tx1.setEvent(hiddenWaitTimeEvent);

                paramState.insertTransitions(tx1);

                GTASCXMLState unInitState;
                getUnInitStateForParameter(unInitState,paramInfo,unInitStateId, finalStateId, finalStateId);




                iState.insertStates(initState);
                iState.insertStates(waitState);
                iState.insertStates(paramState);
                iState.insertStates(unInitState);

                GTASCXMLFinal finalState;
                finalState.setId(finalStateId);

                iState.setFinal(finalState);

                GTASCXMLTransition tx2;
                tx2.setTarget(iTargetId);

                iState.insertTransitions(tx2);
            }
            else
            {
                iState.insertInvokes(paramInvoke);
                GTASCXMLTransition tx1;
                tx1.setTarget(iTargetId);
                if(!isHiddenWaitTime)
                    tx1.setEvent("done.invoke._all");
                else
                    tx1.setEvent(hiddenWaitTimeEvent);
                iState.insertTransitions(tx1);
            }
        }
        else
        {
            iState.insertInvokes(paramInvoke);
            GTASCXMLTransition tx1;
            tx1.setTarget(iTargetId);
            if(!isHiddenWaitTime)
                tx1.setEvent("done.invoke._all");
            else
                tx1.setEvent(hiddenWaitTimeEvent);
            iState.insertTransitions(tx1);
        }
    }
    return rc;
}
bool GTASCXMLController::getWaitState(const GTACommandBase *pCmd, const QString &iDelay,const QString iTarget,const QString iStateId, GTASCXMLState &ioWaitState)
{
    bool rc = true;
    ioWaitState.setId(iStateId);
    QString delayEventName = QString("%1_waitEvent").arg(iStateId);
    bool isDoubleOK = false;
    QString delayTest = iDelay;
    delayTest = delayTest.replace("ms","");
    double dDelay = delayTest.toDouble(&isDoubleOK);

    GTASCXMLTransition tx;
    tx.setTarget(iTarget);

    if(dDelay > 0)
    {
        GTASCXMLSend waitSend;
        waitSend.setDelay(iDelay);
        waitSend.setEvent(delayEventName);

        GTASCXMLOnEntry onEntry;
        onEntry.insertSend(waitSend);
        ioWaitState.setOnEntry(onEntry);
        tx.setEvent(delayEventName);
        if(pCmd != NULL)
            insertSuccessTrasition(pCmd,QStringList(),iTarget,delayEventName,ioWaitState,QString(),QString());
    }
    else
    {
        if(pCmd != NULL)
            insertSuccessTrasition(pCmd,QStringList(),iTarget,"",ioWaitState,QString(),QString());
    }

    if(pCmd == NULL)
        ioWaitState.insertTransitions(tx);


    return rc;
}
bool GTASCXMLController::getInitStateForParameter(GTASCXMLState &ioState,const  GTAICDParameter &iParam,const  QString &iStateId, const QString &iTargetId, const QString &iFinalId, const GTACommandBase *pCmd)
{
    bool rc= false;
    QString templateFileName = "invoke_subscribe.scxml";

    GTASCXMLInvoke subscribeParamInvoke;

    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");


    rc = getCloneInvoke(templateFileName,subscribeParamInvoke);
    if(rc)
    {
        subscribeParamInvoke.setInvokeId(invokeId);

        QString paramName = iParam.getSignalName();
        QString AppName = iParam.getApplicationName();
        QString Media = iParam.getMedia();
        QString ParamType = iParam.getValueType();
        QString busName = iParam.getBusName();

        QString invokeRet = QString("_invoke_%1_Return").arg(paramName);

        QString paramJSONStruct = QString("{ 'Name':'%1','%2':{'Media':'%3', '%4': {'ParameterType':'%5'}}}").arg(AppName,busName,Media,paramName,ParamType);
        QString paramLoc = QString("_%1_local").arg(paramName);

        ioState.setId(iStateId);

        GTASCXMLAssign assign;
        assign.setLocation(paramLoc);
        assign.setExpression(paramJSONStruct);

        GTASCXMLOnEntry onEntry;
        onEntry.insertAssign(assign);


        GTASCXMLTransition tx1;
        tx1.setTarget(iTargetId);
        tx1.setEvent("done.invoke._all");
        tx1.setCondition(QString("%1.Status == 'OK'").arg(invokeRet));

        GTASCXMLTransition tx2;
        tx2.setTarget(iFinalId);
        tx2.setEvent("done.invoke._all");
        tx2.setCondition(QString("%1.Status != 'OK'").arg(invokeRet));

        if(pCmd != NULL)
        {
            GTAScxmlLogMessage passLogMsg;
            passLogMsg.Identifier = pCmd->getInstanceName();
            passLogMsg.CurrentValue = invokeRet;
            passLogMsg.ExpectedValue = invokeRet;
            passLogMsg.LOD = GTAScxmlLogMessage::Main;
            passLogMsg.Result = GTAScxmlLogMessage::OK;
            passLogMsg.Status = GTAScxmlLogMessage::Success;
            passLogMsg.isCurrentValParam = false;
            QString log1 = passLogMsg.getLogMessage(_isGenericSCXMLCofig);
            log1 = log1.remove("'");
            GTASCXMLLog successLog;
            successLog.setExpression(QString("'%1'").arg(log1));
            successLog.setLabel("'Info'");
            tx1.insertLog(successLog);

            GTAScxmlLogMessage failLogMsg;
            failLogMsg.Identifier = pCmd->getInstanceName();
            failLogMsg.CurrentValue = invokeRet;
            failLogMsg.ExpectedValue = invokeRet;
            failLogMsg.LOD = GTAScxmlLogMessage::Main;
            failLogMsg.Result = GTAScxmlLogMessage::OK;
            failLogMsg.Status = GTAScxmlLogMessage::Success;
            failLogMsg.isCurrentValParam = false;
            QString log2 = failLogMsg.getLogMessage(_isGenericSCXMLCofig);
            log2 = log2.remove("'");
            GTASCXMLLog failLog;
            failLog.setExpression(QString("'%1'").arg(log2));
            failLog.setLabel("'Info'");
            tx2.insertLog(failLog);

        }

        GTASCXMLXMLRPC func = subscribeParamInvoke.getXMLRPCFunction();
        GTASCXMLXMLRPC::funcArg arg;
        rc = func.getArgumentByName("ParameterList",arg);
        if(rc)
        {
            arg.value = paramLoc;
            rc = func.updateArgumentByName("ParameterList",arg);
        }
        else
        {
            rc = false;
            return rc;
        }

        GTASCXMLXMLRPC::funcReturn ret;
        rc = func.getReturnArgument(ret);
        if(rc)
        {
            ret.name = invokeRet;
            func.updateReturnArgument(ret);
        }
        else
        {
            rc = false;
            return rc;
        }

        subscribeParamInvoke.setInvokeFunction(func);
        GTASCXMLAssign invokeAssing;
        invokeAssing.setExpression(QString("_event.data.%1").arg(invokeRet));
        invokeAssing.setLocation(invokeRet);
        subscribeParamInvoke.insertAssign(invokeAssing);

        ioState.insertInvokes(subscribeParamInvoke);
        ioState.setOnEntry(onEntry);
        ioState.insertTransitions(tx1);
        ioState.insertTransitions(tx2);
    }


    return rc;
}
bool GTASCXMLController::getUnInitStateForParameter(GTASCXMLState &ioState,const  GTAICDParameter &iParam,const  QString &iStateId, const QString &iTargetId, const QString &iFinalId, const GTACommandBase *pCmd)
{
    bool rc= false;
    QString templateFileName = "invoke_unsubscribe.scxml";

    GTASCXMLInvoke unsubscribeParamInvoke;

    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");


    rc = getCloneInvoke(templateFileName,unsubscribeParamInvoke);

    if(rc)
    {
        unsubscribeParamInvoke.setInvokeId(invokeId);


        QString paramName = iParam.getSignalName();
        QString AppName = iParam.getApplicationName();
        QString Media = iParam.getMedia();
        QString ParamType = iParam.getValueType();
        QString busName = iParam.getBusName();


        QString invokeRet = QString("_invoke_%1_return").arg(paramName);

        QString paramJSONStruct = QString("{ 'Name':'%1','%2':{'Media':'%3', '%4': {'ParameterType':'%5'}}}").arg(AppName,busName,Media,paramName,ParamType);
        QString paramLoc = QString("_%1_local").arg(paramName);

        ioState.setId(iStateId);

        GTASCXMLAssign assign;
        assign.setLocation(paramLoc);
        assign.setExpression(paramJSONStruct);

        GTASCXMLOnEntry onEntry;
        onEntry.insertAssign(assign);


        GTASCXMLTransition tx1;
        tx1.setTarget(iTargetId);
        tx1.setEvent("done.invoke._all");
        tx1.setCondition(QString("%1.Status == 'OK'").arg(invokeRet));

        GTASCXMLTransition tx2;
        tx2.setTarget(iFinalId);
        tx2.setEvent("done.invoke._all");
        tx2.setCondition(QString("%1.Status != 'OK'").arg(invokeRet));


        if(pCmd != NULL)
        {
            GTAScxmlLogMessage passLogMsg;
            passLogMsg.Identifier = pCmd->getInstanceName();
            passLogMsg.CurrentValue = invokeRet;
            passLogMsg.ExpectedValue = invokeRet;
            passLogMsg.LOD = GTAScxmlLogMessage::Main;
            passLogMsg.Result = GTAScxmlLogMessage::OK;
            passLogMsg.Status = GTAScxmlLogMessage::Success;
            passLogMsg.isCurrentValParam = false;
            QString log1 = passLogMsg.getLogMessage(_isGenericSCXMLCofig);
            //log1 = log1.remove("+");
            log1 = log1.remove("'");
            GTASCXMLLog successLog;
            successLog.setExpression(QString("'%1'").arg(log1));
            successLog.setLabel("'Info'");

            tx1.insertLog(successLog);

            GTAScxmlLogMessage failLogMsg;
            failLogMsg.Identifier = pCmd->getInstanceName();
            failLogMsg.CurrentValue = invokeRet;
            failLogMsg.ExpectedValue = invokeRet;
            failLogMsg.LOD = GTAScxmlLogMessage::Main;
            failLogMsg.Result = GTAScxmlLogMessage::OK;
            failLogMsg.Status = GTAScxmlLogMessage::Success;
            failLogMsg.isCurrentValParam = false;
            QString log2 = failLogMsg.getLogMessage(_isGenericSCXMLCofig);
            //log1 = log1.remove("+");
            log2 = log2.remove("'");
            GTASCXMLLog failLog;
            failLog.setExpression(QString("'%1'").arg(log2));
            failLog.setLabel("'Info'");

            tx2.insertLog(failLog);

        }

        GTASCXMLXMLRPC func = unsubscribeParamInvoke.getXMLRPCFunction();
        GTASCXMLXMLRPC::funcArg arg;
        rc = func.getArgumentByName("ParameterList",arg);
        if(rc)
        {
            arg.value = paramLoc;
            arg.name = "";
            rc = func.updateArgumentByName("ParameterList",arg);
        }
        else
        {
            rc = false;
            return rc;
        }

        GTASCXMLXMLRPC::funcReturn ret;
        rc = func.getReturnArgument(ret);
        if(rc)
        {
            ret.name = invokeRet;
            func.updateReturnArgument(ret);
        }
        else
        {
            rc = false;
            return rc;
        }

        unsubscribeParamInvoke.setInvokeFunction(func);
        GTASCXMLAssign invokeAssing;
        invokeAssing.setExpression(QString("_event.data.%1").arg(invokeRet));
        invokeAssing.setLocation(invokeRet);
        unsubscribeParamInvoke.insertAssign(invokeAssing);

        ioState.insertInvokes(unsubscribeParamInvoke);
        ioState.setOnEntry(onEntry);
        ioState.insertTransitions(tx1);
        ioState.insertTransitions(tx2);
    }


    return rc;
}
bool GTASCXMLController::getInvokeForParamGeneric(GTASCXMLInvoke &ioInvoke, const QString &iInvokId)
{
    QString templateName ="get_generic.scxml";
    bool rc = getCloneInvoke(templateName,ioInvoke);
    if(rc)
    {
        ioInvoke.setInvokeId(iInvokId);
    }
    return rc;

}

bool GTASCXMLController::getInvokeForParam(GTASCXMLInvoke &ioInvoke, const QString &iParam, const QString &iInvokId, const QString &iCmdName)
{
    bool rc= false;
    QString templateName = iCmdName+ ".scxml";
    templateName = templateName.replace(" ","_");
    rc = getCloneInvoke(templateName,ioInvoke);
    ioInvoke.setInvokeId(iInvokId);
    QString retName = QString("_invoke_%1_return").arg(iInvokId);
    if(rc)
    {
        GTASCXMLXMLRPC func = ioInvoke.getXMLRPCFunction();
        GTASCXMLXMLRPC::funcArg arg;
        rc = func.getArgumentByName("ParameterID",arg);
        if(rc)
        {
            if(arg.type == "array")
                arg.value = QString("['%1']").arg(iParam);
            else
                arg.value = QString("%1").arg(iParam);
            func.updateArgumentByName("ParameterID",arg);
            GTASCXMLXMLRPC::funcReturn ret;
            rc = func.getReturnArgument(ret);
            if(rc)
            {
                ret.name = retName;
                func.updateReturnArgument(ret);
            }
        }
        ioInvoke.setInvokeFunction(func);
        GTASCXMLAssign assign;
        assign.setExpression(QString("_event.data.%1").arg(retName));
        assign.setLocation(iParam);
        ioInvoke.insertAssign(assign);
    }
    return rc;
}
bool GTASCXMLController::getCloneInvoke(const QString & iTemplateFileName, GTASCXMLInvoke &iInvoke)
{
    QDomDocument oDomDoc;
    bool rc = openScxmlTemplateFile(iTemplateFileName,oDomDoc);
    if(rc && (!oDomDoc.isNull()))
    {
        QDomElement rootElem = oDomDoc.documentElement();
        if(! rootElem.isNull())
        {
            if(_isGenericSCXMLCofig)
                iInvoke.translateXMLToGenericStruct(rootElem);
            else
                iInvoke.translateXMLToStruct(rootElem);
        }
        else rc = false;
    }
    return rc;
}
bool GTASCXMLController::getInvokeDealy(const QString iCmdName, QString &oDelay)
{
    QString sValue =_pAppCtrl->getCommandProperty(iCmdName.trimmed(),XATTR_INVOKE_DELAY);
    QString sValueUnit=_pAppCtrl->getCommandProperty(iCmdName.trimmed(),XATTR_INVOKE_DELAY_UNIT);
    bool rc =false;

    if(!sValue.isEmpty())
    {
        bool isDoubleOk;
        double delay = sValue.toDouble(&isDoubleOk);
        if(isDoubleOk && (delay >0))
        {
            if(sValueUnit != "ms")
            {
                oDelay = QString::number(delay*1000)+"ms";
            }
            else
            {
                oDelay = QString::number(delay)+"ms";
            }
            rc = true;
        }
        else
        {
            sValue = "";
            oDelay = sValue;
            rc = false;
        }
    }
    else
    {
        sValue = "";
        rc = false;
    }
    return rc;
}
bool GTASCXMLController::openScxmlTemplateFile(const QString & iTemplateFileName, QDomDocument & oDomDoc)
{
    bool rc = false;
    QString scxmlTmplFilePath = QString("%1/%2").arg(_ScxmlTemplatePath,iTemplateFileName);
    QFile xmlFileObj(scxmlTmplFilePath);
    rc = xmlFileObj.open(QFile::ReadOnly | QFile::Text);
    if(rc)
    {

        rc = oDomDoc.setContent(&xmlFileObj);
        if(rc == false)
            _LastError = QString("Error: invalid scxml format of file %1").arg(scxmlTmplFilePath);

        xmlFileObj.close();
    }
    else _LastError = QString("Error: Unable to open file %1").arg(scxmlTmplFilePath);

    return rc;
}
void GTASCXMLController::setStateIDName(const QString& iStateID, const GTACommandBase* pCmd)
{
    if (pCmd!= NULL && !_stateIdMap.contains(iStateID))
    {
        QString stateName = pCmd->getSCXMLStateName();
        stateName.trimmed();
        QRegExp rx("([`~.:%!&;,*\\^?|/\\\\@#$:\\-\\+{}\\[\\]<>'\"\\s\\(\\)]+)");
        stateName.replace(rx,"_");
        stateName.replace("\n","_");
        while(stateName.endsWith("_"))
        {
            stateName.replace(stateName.size()-1,1,"");
        }
        _stateIdMap[iStateID] = QString("%1_%2").arg(iStateID,stateName);
    }
}
GTASCXMLController::paramType GTASCXMLController::getParamType(const QString &iParam)
{
    if(_icdParamList.contains(iParam))
        return GTASCXMLController::PARAM_ICD;
    else
    {
        bool isNum = false;
        iParam.toDouble(&isNum);
        if(isNum)
            return GTASCXMLController::PARAM_NUM;

        if(iParam.contains("\"") || iParam.contains("\'"))
            return GTASCXMLController::PARAM_STRING;

        return GTASCXMLController::PARAM_LOCAL;
    }
    //return GTASCXMLController::PARAM_UNKNOWN; unreachable code
}

bool GTASCXMLController::getIsActionOnFailForCmd(const GTACommandBase *pCmd)
{
    bool isActionOnFailContinue = false;

    if(pCmd->getCommandType() == GTACommandBase::ACTION)
    {
        GTAActionBase * pActCmd = (GTAActionBase *)pCmd;
        if(pActCmd)
        {
            if(!pActCmd->getActionOnFail().contains("stop"))
                isActionOnFailContinue = true;
        }
    }
    else if(pCmd->getCommandType() == GTACommandBase::CHECK)
    {
        GTACheckBase * pChkCmd = (GTACheckBase *)pCmd;
        if(pChkCmd)
        {
            if(!pChkCmd->getActionOnFail().contains("stop"))
                isActionOnFailContinue = true;
        }
    }
    else
        isActionOnFailContinue = false;

    return isActionOnFailContinue;
}

void GTASCXMLController::insertSuccessTrasition(const GTACommandBase *pCmd,const QStringList &iIcdParamList,const QString &iTargetId,const QString &iEventName, GTASCXMLState &ioState, QString &iExtCond, QString &iTimeoutCond, bool iAddLog)
{
    GTASCXMLTransition successTx;
    QString condStatement;
    QString cond;

    QString sEngine = QString::fromStdString(TestConfig::getInstance()->getEngineSelection());
    bool rc = pCmd->getSCXMLConditionStatement(iIcdParamList,condStatement,_genToolReturnMap,sEngine);
    if(rc)
    {
        if(condStatement.startsWith("(") && condStatement.endsWith(")"))
            cond =  (QString("%1 == true").arg(condStatement));
        else
            cond =  (QString("(%1) == true").arg(condStatement));

        if(!iTimeoutCond.isEmpty())
        {
            cond = QString("(%1) && (%2)").arg(cond,iTimeoutCond);
        }
        successTx.setCondition(cond);
    }
    if(!rc && !iExtCond.isEmpty())
    {
        cond = (QString("(%1) == true").arg(iExtCond));

        if(!iTimeoutCond.isEmpty())
        {
            cond = QString("(%1) && (%2)").arg(cond,iTimeoutCond);
        }

        successTx.setCondition(cond);
    }
    successTx.setTarget(iTargetId);
    successTx.setStatus(SCXML_STATUS_PASS);
    if(!iEventName.isEmpty())
        successTx.setEvent(iEventName);

    GTAScxmlLogMessage passLogMsg;
    passLogMsg.Identifier = pCmd->getInstanceName();
    passLogMsg.CurrentValue = "true";
    passLogMsg.ExpectedValue = "true";
    passLogMsg.LOD = GTAScxmlLogMessage::Main;
    passLogMsg.Result = GTAScxmlLogMessage::OK;
    passLogMsg.Status = GTAScxmlLogMessage::Success;
    passLogMsg.isCurrentValParam = false;
    QString log1 = passLogMsg.getLogMessage(_isGenericSCXMLCofig);
    //log1 = log1.remove("+");
    log1 = log1.remove("'");
    GTASCXMLLog successLog;
    successLog.setExpression(QString("'%1'").arg(log1));
    successLog.setLabel("'Info'");

    if(iAddLog)
        successTx.insertLog(successLog);

    ioState.insertTransitions(successTx);
}

void GTASCXMLController::insertFailTrasition(const GTACommandBase *pCmd, const QStringList &iIcdParamList, const QString &iTargetId, const QString &iEventName, GTASCXMLState &ioState, QString &iExtCond, bool iForceSuccess, QString &iTimeoutCond, bool iAddLog)
{
    GTASCXMLTransition failTx;
    QString condStatement;
    QString cond;
    QString sEngine = QString::fromStdString(TestConfig::getInstance()->getEngineSelection());
    bool rc = pCmd->getSCXMLConditionStatement(iIcdParamList,condStatement,_genToolReturnMap,sEngine);
    if(rc)
    {
        if(condStatement.startsWith("(") && condStatement.endsWith(")"))
            cond =  (QString("%1 == false").arg(condStatement));
        else
            cond =  (QString("(%1) == false").arg(condStatement));

        if(!iTimeoutCond.isEmpty())
        {
            cond = QString("(%1) && (%2)").arg(cond,iTimeoutCond);
        }
        failTx.setCondition(cond);
    }
    if(!rc && !iExtCond.isEmpty())
    {
        cond = (QString("(%1) == false").arg(iExtCond));

        if(!iTimeoutCond.isEmpty())
        {
            cond = QString("(%1) && (%2)").arg(cond,iTimeoutCond);
        }

        failTx.setCondition(cond);
    }



    bool isActionOnFailContinue = getIsActionOnFailForCmd(pCmd);

    if(isActionOnFailContinue)
        failTx.setTarget(iTargetId);
    else
        failTx.setTarget(SCXML_FINAL_STATE_ID);

    failTx.setStatus(SCXML_STATUS_FAIL);
    if(!iEventName.isEmpty())
        failTx.setEvent(iEventName);

    GTAScxmlLogMessage passLogMsg;
    passLogMsg.Identifier = pCmd->getInstanceName();
    passLogMsg.CurrentValue = "false";
    passLogMsg.ExpectedValue = "true";
    passLogMsg.LOD = GTAScxmlLogMessage::Main;
    passLogMsg.isCurrentValParam = false;
    if(!iForceSuccess)
    {
        passLogMsg.Result = GTAScxmlLogMessage::KO;
        passLogMsg.Status = GTAScxmlLogMessage::Fail;
    }
    else
    {
        passLogMsg.Result = GTAScxmlLogMessage::OK;
        passLogMsg.Status = GTAScxmlLogMessage::Success;
    }


    QString log1 = passLogMsg.getLogMessage(_isGenericSCXMLCofig);
    //log1 = log1.remove("+");
    log1 = log1.remove("'");

    GTASCXMLLog successLog;
    successLog.setExpression(QString("'%1'").arg(log1));
    successLog.setLabel("'Info'");

    if(iAddLog)
        failTx.insertLog(successLog);

    ioState.insertTransitions(failTx);


}

void GTASCXMLController::insertTimeOutTrasitionWoEvent(const GTACommandBase *pCmd, const QString &iTargetId, const QString &iEventName, GTASCXMLState &ioState, QString &iTimeoutCond)
{
    GTASCXMLTransition oTimeoutTx;
    oTimeoutTx.setEvent(iEventName);

    oTimeoutTx.setCondition(iTimeoutCond);
    oTimeoutTx.setStatus(SCXML_STATUS_FAIL);
    bool isActionOnFailContinue = getIsActionOnFailForCmd(pCmd);

    if(isActionOnFailContinue)
        oTimeoutTx.setTarget(iTargetId);
    else
        oTimeoutTx.setTarget(SCXML_FINAL_STATE_ID);


    GTAScxmlLogMessage timeoutLogMsg;

    timeoutLogMsg.Identifier = pCmd->getInstanceName();
    timeoutLogMsg.CurrentValue = QString("'TimeOut'");
    timeoutLogMsg.Status = GTAScxmlLogMessage::TimeOut;
    timeoutLogMsg.LOD = GTAScxmlLogMessage::Main;
    timeoutLogMsg.Result = GTAScxmlLogMessage::KO;
    timeoutLogMsg.isCurrentValParam = false;
    QString log = timeoutLogMsg.getLogMessage(_isGenericSCXMLCofig);
    //log = log.remove("+");
    log = log.remove("'");

    GTASCXMLLog timeoutLog;
    timeoutLog.setExpression(QString("'%1'").arg(log));
    timeoutLog.setLabel("'Info'");
    oTimeoutTx.insertLog(timeoutLog);
    ioState.insertTransitions(oTimeoutTx,true);

}


void GTASCXMLController::insertTimeoutTransition(const GTACommandBase *pCmd,const QString &iEventName, const QString &iTargetId, GTASCXMLState &ioState)
{
    GTASCXMLTransition timeoutTx;
    getTimeoutTranstion(pCmd,iEventName,iTargetId,timeoutTx);
    ioState.insertTransitions(timeoutTx,true);
}
void GTASCXMLController::getTimeoutTranstion(const GTACommandBase *pCmd,const QString &iEventName, const QString &iTargetId, GTASCXMLTransition &oTimeoutTx,const QString &iTimeoutCond)
{

    if(!iTimeoutCond.isEmpty())
        oTimeoutTx.setCondition(iTimeoutCond);

    oTimeoutTx.setEvent(iEventName);

    oTimeoutTx.setStatus(SCXML_STATUS_FAIL);
    bool isActionOnFailContinue = getIsActionOnFailForCmd(pCmd);

    if(isActionOnFailContinue)
        oTimeoutTx.setTarget(iTargetId);
    else
        oTimeoutTx.setTarget(SCXML_FINAL_STATE_ID);


    GTAScxmlLogMessage timeoutLogMsg;

    timeoutLogMsg.Identifier = pCmd->getInstanceName();
    timeoutLogMsg.CurrentValue = QString("'TimeOut'");
    timeoutLogMsg.Status = GTAScxmlLogMessage::TimeOut;
    timeoutLogMsg.LOD = GTAScxmlLogMessage::Main;
    timeoutLogMsg.Result = GTAScxmlLogMessage::KO;
    timeoutLogMsg.isCurrentValParam = false;
    QString log = timeoutLogMsg.getLogMessage(_isGenericSCXMLCofig);
    //log = log.remove("+");
    log = log.remove("'");

    GTASCXMLLog timeoutLog;
    timeoutLog.setExpression(QString("'%1'").arg(log));
    timeoutLog.setLabel("'Info'");
    oTimeoutTx.insertLog(timeoutLog);
}

void GTASCXMLController::getConditionStatement(const GTACommandBase *,const QStringList &, QString &, const GTASCXMLController::condType &)
{

}
void GTASCXMLController::insertTimeoutInState(const GTACommandBase *,const QStringList &, GTASCXMLState &)
{


}

void GTASCXMLController::logAllParametersOnExit(const GTACommandBase *pCmd, const QStringList &, GTASCXMLState &ioState)
{
    GTASCXMLOnExit onExit = ioState.getOnExit();

    QStringList paramList = pCmd->getVariableList();
    QStringList dumpList = pCmd->getDumpList();

    paramList.append(dumpList);

    foreach(QString item,paramList)
    {
        updateWithGenToolParam(item);
        GTAScxmlLogMessage objLogOnExit;
        objLogOnExit.LOD = GTAScxmlLogMessage::Detail;
        objLogOnExit.Identifier    = pCmd->getInstanceName();
        objLogOnExit.ExpectedValue = item;
        objLogOnExit.CurrentValue = item;
        objLogOnExit.Status = GTAScxmlLogMessage::None;
        objLogOnExit.Result = GTAScxmlLogMessage::kNa;
        objLogOnExit.isCurrentValParam = true;
        GTASCXMLLog onExitLog;
        onExitLog.setExpression(objLogOnExit.getLogMessage(_isGenericSCXMLCofig));
        onExitLog.setLabel("'Info'");

        onExit.insertLog(onExitLog);
    }
    ioState.setOnExit(onExit);
}

void GTASCXMLController::createSCXMLPrintMessage(const GTAActionPrint *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &iExtTimeouts)
{
    if(iPrintCmd == NULL)
        return;

    QStringList values = iPrintCmd->getValues();
    QString instanceName = iPrintCmd->getInstanceName();
    if(!values.isEmpty())
    {
        QString message = values.first();

        message.replace("\n","<br></br>");
        message.replace("`","_");

        GTASCXMLState printMessageState;
        printMessageState.setId(iStateId);
        QString defaultDelayEvent = QString("%1_delay_event").arg(iStateId);

        bool isActionOnFailContinue = true;

        GTASCXMLSend send1;
        send1.setDelay("10ms");
        send1.setEvent(defaultDelayEvent);

        GTASCXMLOnEntry onentry;
        onentry.insertSend(send1);

        GTASCXMLTransition tx1;
        tx1.setStatus(SCXML_STATUS_PASS);
        tx1.setTarget(iTargetId);
        tx1.setEvent(defaultDelayEvent);


        GTAScxmlLogMessage logMessage1;
        logMessage1.CurrentValue = message;
        logMessage1.ExpectedValue = "Message";
        logMessage1.Identifier = instanceName;
        logMessage1.FunctionStatus = "NA";

        logMessage1.Result = GTAScxmlLogMessage::OK;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
        logMessage1.isCurrentValParam = false;
        QString log1 = logMessage1.getLogMessage(_isGenericSCXMLCofig);
        log1 = log1.remove("+");
        log1 = log1.remove("'");

        GTASCXMLLog log;
        log.setLabel("Info");
        log.setExpression(QString("'%1'").arg(log1));

        tx1.insertLog(log);


        if(!iExtTimeouts.isEmpty())
        {
            for(int i=0;i<iExtTimeouts.count();i++)
            {
                QString externalTimeoutEvent = iExtTimeouts.at(i);
                GTASCXMLTransition tx2;
                tx2.setStatus(SCXML_STATUS_FAIL);
                tx2.setEvent(externalTimeoutEvent);
                if(isActionOnFailContinue)
                    tx2.setTarget(iTargetId);
                else
                    tx2.setTarget(SCXML_FINAL_STATE_ID);

                GTAScxmlLogMessage logMessage2;
                logMessage2.CurrentValue = message;
                logMessage2.ExpectedValue = "Message";
                logMessage2.Identifier = instanceName;
                logMessage2.FunctionStatus = "NA";

                logMessage2.Result = GTAScxmlLogMessage::KO;
                logMessage2.Status = GTAScxmlLogMessage::TimeOut;
                logMessage2.LOD = GTAScxmlLogMessage::Main;
                logMessage2.isCurrentValParam = false;
                QString log2 = logMessage2.getLogMessage(_isGenericSCXMLCofig);
                log2 = log2.remove("+");
                log2 = log2.remove("'");

                GTASCXMLLog log3;
                log3.setLabel("Info");
                log3.setExpression(QString("'%1'").arg(log2));

                tx2.insertLog(log3);

                printMessageState.insertTransitions(tx2);
            }
        }




        GTAScxmlLogMessage logMessage3;
        logMessage3.CurrentValue = message;
        logMessage3.ExpectedValue = "Message";
        logMessage3.Identifier = instanceName;
        logMessage3.FunctionStatus = "NA";

        logMessage3.Result = GTAScxmlLogMessage::kNa;
        logMessage3.Status = GTAScxmlLogMessage::None;
        logMessage3.LOD = GTAScxmlLogMessage::Detail;
        logMessage3.isCurrentValParam = false;
        QString log4 = logMessage3.getLogMessage(_isGenericSCXMLCofig);
        log4 = log4.remove("+");
        log4 = log4.remove("'");

        GTASCXMLLog log5;
        log5.setLabel("Info");
        log5.setExpression(QString("'%1'").arg(log4));

        GTASCXMLOnExit onExit;
        onExit.insertLog(log5);

        printMessageState.setOnEntry(onentry);
        printMessageState.setOnExit(onExit);
        printMessageState.insertTransitions(tx1);
        iState.insertStates(printMessageState);
    }
}
void GTASCXMLController::createSCXMLPrintParameter(const GTAActionPrint *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iPrintCmd == NULL)
        return;
    QString cmdName = QString("%1 %2").arg(iPrintCmd->getAction(),iPrintCmd->getComplement()).trimmed();
    QStringList items = iPrintCmd->getVariableList();
    items.append(iPrintCmd->getDumpList());
    QStringList icdParamList;
    QString finalStateId = QString("%1_final").arg(iStateId);
    QString dummyTransitionStateId = QString("%1_transtionState").arg(iStateId);
    QString dumpParamStateId = QString("%1_dumpList").arg(iStateId);


    for(int i=0;i<items.count();i++)
    {
        if(_icdParamList.contains(items.at(i)))
            icdParamList.append(items.at(i));
    }
    GTASCXMLState printParamState;
    for(int i=0;i<icdParamList.count();i++)
    {
        GTASCXMLState paramState;
        QString paramStateId = QString("%1_param_%2").arg(iStateId,QString::number(i));
        QString paramTargetId = QString("%1_param_%2").arg(iStateId,QString::number(i+1));
        if(i == icdParamList.count() - 1)
        {
            paramTargetId  = dummyTransitionStateId;
        }
        bool rc = getStateForParam(paramState,paramStateId,paramTargetId,icdParamList.at(i),cmdName);
        if(rc)
        {
            printParamState.insertStates(paramState);

        }
    }

    QString timeoutName = QString("%1_timeoutEvent").arg(iStateId);
    GTASCXMLSend timeoutSend;
    timeoutSend.setDelay("3000ms");
    timeoutSend.setEvent(timeoutName);

    GTASCXMLOnEntry onEntry;
    onEntry.insertSend(timeoutSend);

    //Onexit to print all the parameters;
    logAllParametersOnExit(iPrintCmd,items,printParamState);

    GTASCXMLFinal finalState;
    finalState.setId(finalStateId);

    GTASCXMLState dummyTransitionState;
    dummyTransitionState.setId(dummyTransitionStateId);
    GTASCXMLOnEntry dummOnEntry;
    GTASCXMLSend dummySend;
    QString dummyEventName = QString("%1_dummyEvent").arg(iStateId);
    dummySend.setEvent(dummyEventName);
    dummySend.setDelay("10ms");
    dummOnEntry.insertSend(dummySend);
    dummyTransitionState.setOnEntry(dummOnEntry);

    insertSuccessTrasition(iPrintCmd,icdParamList,dumpParamStateId,dummyEventName,dummyTransitionState);
    insertTimeoutTransition(iPrintCmd,timeoutName,finalStateId,dummyTransitionState);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    tx.setStatus(SCXML_STATUS_PASS);


    printParamState.setId(iStateId);
    printParamState.setOnEntry(onEntry);
    printParamState.insertStates(dummyTransitionState);
    printParamState.setFinal(finalState);
    printParamState.insertTransitions(tx);

    getStateForDumpListParameters(iPrintCmd,printParamState,dumpParamStateId,finalStateId);

    iState.insertStates(printParamState);
}
void GTASCXMLController::createSCXMLPrintTable(const GTAActionPrintTable	 *iPrintTableCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iPrintTableCmd == NULL)
        return;
    QString cmdName = QString("%1 %2").arg(iPrintTableCmd->getAction(),iPrintTableCmd->getComplement()).trimmed();
    QStringList items = iPrintTableCmd->getValues();
    QStringList icdParamList;
    QString finalStateId = QString("%1_final").arg(iStateId);
    QString dummyTransitionStateId = QString("%1_transtionState").arg(iStateId);
    QString dumpParamStateId = QString("%1_dumpList").arg(iStateId);

    for(int i=0;i<items.count();i++)
    {
        if(_icdParamList.contains(items.at(i)))
            icdParamList.append(items.at(i));
    }
    GTASCXMLState printParamState;
    for(int i=0;i<icdParamList.count();i++)
    {
        GTASCXMLState paramState;
        QString paramStateId = QString("%1_param_%2").arg(iStateId,QString::number(i));
        QString paramTargetId = QString("%1_param_%2").arg(iStateId,QString::number(i+1));
        if(i == icdParamList.count() - 1)
        {
            paramTargetId  = dummyTransitionStateId;
        }
        bool rc = getStateForParam(paramState,paramStateId,paramTargetId,icdParamList.at(i),cmdName);
        if(rc)
        {
            printParamState.insertStates(paramState);

        }
    }

    QString timeoutName = QString("%1_timeoutEvent").arg(iStateId);
    GTASCXMLSend timeoutSend;
    timeoutSend.setDelay("3000ms");
    timeoutSend.setEvent(timeoutName);

    GTASCXMLOnEntry onEntry;
    onEntry.insertSend(timeoutSend);

    //Onexit to print all the parameters;
    logAllParametersOnExit(iPrintTableCmd,items,printParamState);

    GTASCXMLFinal finalState;
    finalState.setId(finalStateId);

    GTASCXMLState dummyTransitionState;
    dummyTransitionState.setId(dummyTransitionStateId);
    GTASCXMLOnEntry dummOnEntry;
    GTASCXMLSend dummySend;
    QString dummyEventName = QString("%1_dummyEvent").arg(iStateId);
    dummySend.setEvent(dummyEventName);
    dummySend.setDelay("10ms");
    dummOnEntry.insertSend(dummySend);
    dummyTransitionState.setOnEntry(dummOnEntry);

    insertSuccessTrasition(iPrintTableCmd,icdParamList,dumpParamStateId,dummyEventName,dummyTransitionState);
    insertTimeoutTransition(iPrintTableCmd,timeoutName,finalStateId,dummyTransitionState);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    tx.setStatus(SCXML_STATUS_PASS);


    printParamState.setId(iStateId);
    printParamState.setOnEntry(onEntry);
    printParamState.insertStates(dummyTransitionState);
    printParamState.setFinal(finalState);
    printParamState.insertTransitions(tx);

    getStateForDumpListParameters(iPrintTableCmd,printParamState,dumpParamStateId,finalStateId);

    iState.insertStates(printParamState);
}
void GTASCXMLController::createSCXMLPrintTime(const GTAActionPrintTime *pPrintTime, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &)
{
    GTAActionPrintTime::TimeType eTimetime = pPrintTime->getTimeType();
    QString timeExpr= "";
    if(eTimetime == GTAActionPrintTime::Formatted)
        timeExpr = "Date()";
    else
        timeExpr = "Date.now()";

    GTASCXMLAssign dateTimeParam;
    dateTimeParam.setExpression(timeExpr);
    dateTimeParam.setLocation("_dateTime");

    GTASCXMLOnEntry onEntry;
    onEntry.insertAssign(dateTimeParam);

    GTASCXMLState printTimeState;
    printTimeState.setOnEntry(onEntry);

    printTimeState.setId(iStateId);

    GTAScxmlLogMessage logMsg;
    logMsg.Identifier = pPrintTime->getInstanceName();
    logMsg.Result = GTAScxmlLogMessage::OK;
    logMsg.Status = GTAScxmlLogMessage::Success;
    logMsg.ExpectedValue = "Time";
    logMsg.CurrentValue = "_dateTime";
    logMsg.LOD = GTAScxmlLogMessage::Main;
    logMsg.isCurrentValParam = true;
    QString sLogMsg = logMsg.getLogMessage(_isGenericSCXMLCofig);
    GTASCXMLLog log;
    log.setExpression(sLogMsg);
    log.setLabel("'Info'");

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    tx.insertLog(log);

    printTimeState.insertTransitions(tx);

    iState.insertStates(printTimeState);

}
void GTASCXMLController::createSCXMLTitle(const GTAActionParentTitle *iTitleCmd,GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iTitleCmd == NULL)
        return;
    GTASCXMLState titleState;
    titleState.setId(iStateId);
    getStateChart(iTitleCmd,titleState);


    GTASCXMLFinal final;

    QString finalStateId = QString("%1_final").arg(iStateId);
    final.setId(finalStateId);

    titleState.setFinal(final);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    titleState.insertTransitions(tx);

    iState.insertStates(titleState);
}
void GTASCXMLController::createSCXMLSubscribe(const GTAActionSubscribe *iSubscribeCmd,GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iSubscribeCmd == NULL)
        return;

    QStringList icdParamList;
    GTASCXMLState subscribeState;
    subscribeState.setId(iStateId);

    GTASCXMLFinal final;

    QString finalStateId = QString("%1_final").arg(iStateId);
    final.setId(finalStateId);
    subscribeState.setFinal(final);



    QStringList paramsToSubscribe = iSubscribeCmd->getValues();


    for(int i=0;i<paramsToSubscribe.count();i++)
    {
        if(_icdParamList.contains(paramsToSubscribe.at(i)))
        {
            icdParamList.append(paramsToSubscribe.at(i));
            
        }
    }
    ExternalSubscribeParams.append(icdParamList);
    for(int i=0;i<icdParamList.count();i++)
    {
        GTASCXMLState paramState;
        QString paramStateId = QString("%1_param_%2").arg(iStateId,QString::number(i));
        QString paramTargetId = QString("%1_param_%2").arg(iStateId,QString::number(i+1));
        if(i == icdParamList.count() - 1)
        {
            paramTargetId  = finalStateId;
        }
        bool rc = getInitStateForParameter(paramState,_icdParamList.value(icdParamList.at(i)),paramStateId,paramTargetId,finalStateId,iSubscribeCmd);
        if(rc)
        {
            subscribeState.insertStates(paramState);
        }
    }

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    subscribeState.insertTransitions(tx);

    iState.insertStates(subscribeState);
}
void GTASCXMLController::createSCXMLUnSubscribe(const GTAActionUnSubscribe *iUnSubscribeCmd,GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iUnSubscribeCmd == NULL)
        return;

    QStringList icdParamList;
    GTASCXMLState unSubscribeState;
    unSubscribeState.setId(iStateId);

    GTASCXMLFinal final;

    QString finalStateId = QString("%1_final").arg(iStateId);
    final.setId(finalStateId);
    unSubscribeState.setFinal(final);



    QStringList paramsToUnSubscribe = iUnSubscribeCmd->getValues();


    for(int i=0;i<paramsToUnSubscribe.count();i++)
    {
        if(_icdParamList.contains(paramsToUnSubscribe.at(i)))
            icdParamList.append(paramsToUnSubscribe.at(i));
    }

    for(int i=0;i<icdParamList.count();i++)
    {
        GTASCXMLState paramState;
        QString paramStateId = QString("%1_param_%2").arg(iStateId,QString::number(i));
        QString paramTargetId = QString("%1_param_%2").arg(iStateId,QString::number(i+1));
        if(i == icdParamList.count() - 1)
        {
            paramTargetId  = finalStateId;
        }
        bool rc = getUnInitStateForParameter(paramState,_icdParamList.value(icdParamList.at(i)),paramStateId,paramTargetId,finalStateId,iUnSubscribeCmd);
        if(rc)
        {
            unSubscribeState.insertStates(paramState);
        }
    }

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    unSubscribeState.insertTransitions(tx);

    iState.insertStates(unSubscribeState);
}
void GTASCXMLController::createSCXMLWaitFor(const GTAActionWait *iWaitCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iWaitCmd == NULL)
        return;

    QString waitDealy = iWaitCmd->getCounter();

    if(waitDealy.trimmed().isEmpty())
        return;

    bool bWaitDelay = false;
    double dWaitDelay = waitDealy.toDouble(&bWaitDelay);

    if(bWaitDelay)
    {
        QString sWaitDelay = QString::number(dWaitDelay*1000) + "ms";

        GTASCXMLState waitState;
        bool rc= getWaitState(iWaitCmd, sWaitDelay,iTargetId,iStateId,waitState);

        if(rc)
        {
            iState.insertStates(waitState);
        }
    }
}
void GTASCXMLController::createSCXMLWaitUntil(const GTAActionWait *iWaitCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QList<GTASCXMLTransition> &iExtTransitions, const QString &iConfEventName)
{

    if(iWaitCmd == NULL)
        return;

    QStringList icdParamList;
    QString cmdName = QString("%1 %2").arg(iWaitCmd->getAction(),iWaitCmd->getComplement()).trimmed();
    QString lhs = iWaitCmd->getParameter();
    QString rhs = iWaitCmd->getValue();
    updateWithGenToolParam(lhs);
    updateWithGenToolParam(rhs);
    QString condOperator = iWaitCmd->getCondition();
    double dTimeout = iWaitCmd->getTimeOutInMs();
    QString actOnFail = iWaitCmd->getActionOnFail();
    QString precision, precisionUnit;	iWaitCmd->getPrecision(precision, precisionUnit);
    QString confTime,confTimeUnit; iWaitCmd->getConfCheckTime(confTime,confTimeUnit);
    QStringList dumpList = iWaitCmd->getDumpList();
    bool hasConfEvent = !iConfEventName.isEmpty();

    GTASCXMLController::paramType type1 = getParamType(lhs);
    GTASCXMLController::paramType type2 = getParamType(rhs);


    bool isNum = false;
    bool isConfTimeExist = false;
    double dConfTime = confTime.toDouble(&isNum);
    if((confTimeUnit == ACT_TIMEOUT_UNIT_SEC) && isNum)
    {
        dConfTime = dConfTime*1000;
        if(dConfTime > 0)
            isConfTimeExist = true;
    }


    QString waitUntilMainFinalId = QString("%1_final").arg(iStateId);
    QString dummyTransitionStateId = QString("%1_transtionState").arg(iStateId);
    QString waitUntilState1Id = QString ("%1_waitUntil").arg(iStateId);
    QString waitUntilState2Id = QString ("%1_waitUntil_Conf").arg(iStateId);
    QString waitUntilFinalStateId  = QString ("%1_final").arg(waitUntilState1Id);
    QString parametersStateId  = QString("%1_paramStates").arg(iStateId);
    QString parameterStatesFinalId = QString("%1_final").arg(parametersStateId);
    QString timeoutName = QString("%1_timeoutEvent").arg(iStateId);
    QString confTimeStateId = QString("%1_conftime").arg(iStateId);
    QString confTimeEvent = QString("%1_conftime_event").arg(iStateId);
    QString dumplistStateId = QString("%1_dumpList").arg(iStateId);
    QString waitUntilState1TargetId = dumplistStateId;
    QString confStartTime = QString("%1_confStartTime").arg(waitUntilState2Id);
    QString confElapsedTime = QString("%1_confElapsedTime").arg(waitUntilState2Id);
    QString timeoutStartTime = QString("%1_timeoutStartTime").arg(iStateId);
    QString timeoutElapsedTime = QString("%1_timeoutElapsedTime").arg(iStateId);

    if(isConfTimeExist)
        waitUntilState1TargetId = confTimeStateId;

    GTASCXMLOnEntry onEntry;
    if(dTimeout >0)
    {
        if(_isGenericSCXMLCofig)
        {

            GTASCXMLSend timeoutSend;
            timeoutSend.setDelay(QString::number(dTimeout)+"ms");
            timeoutSend.setEvent(timeoutName);
            onEntry.insertSend(timeoutSend);

        }
        else
        {
            GTASCXMLAssign assign;
            assign.setExpression("Date.now()");
            assign.setLocation(timeoutStartTime);
            onEntry.insertAssign(assign);

        }
    }


    GTASCXMLState paramStates;
    if(type1 == GTASCXMLController::PARAM_ICD)
    {
        GTASCXMLState paramState;
        QString paramStateId = QString("%1_param_lhs").arg(iStateId);
        QString paramTargetId = QString("%1_param_rhs").arg(iStateId);
        if(type2 != GTASCXMLController::PARAM_ICD)
        {
            paramTargetId  = parameterStatesFinalId;
        }
        bool rc = getStateForParam(paramState,paramStateId,paramTargetId,lhs,cmdName);
        if(rc)
        {
            paramStates.insertStates(paramState);
            icdParamList.append(lhs);
        }
    }
    if(type2 == GTASCXMLController::PARAM_ICD)
    {
        GTASCXMLState paramState;
        QString paramStateId = QString("%1_param_rhs").arg(iStateId);
        QString paramTargetId = parameterStatesFinalId;
        bool rc = getStateForParam(paramState,paramStateId,paramTargetId,rhs,cmdName);
        if(rc)
        {
            paramStates.insertStates(paramState);
            icdParamList.append(rhs);
        }
    }

    if(!paramStates.isNull())
    {
        GTASCXMLFinal paramStatesFinal;
        paramStatesFinal.setId(parameterStatesFinalId);
        paramStates.setFinal(paramStatesFinal);
    }

    paramStates.setId(parametersStateId);
    GTASCXMLTransition paramStatesTx;
    paramStatesTx.setTarget(dummyTransitionStateId);
    paramStates.insertTransitions(paramStatesTx);



    QString timeoutCondTrue;
    QString timeoutCondFalse;
    GTASCXMLState dummyTransitionState;
    dummyTransitionState.setId(dummyTransitionStateId);
    GTASCXMLOnEntry dummOnEntry;
    GTASCXMLSend dummySend;
    QString dummyEventName = QString("%1_dummyEvent").arg(iStateId);
    dummySend.setEvent(dummyEventName);
    dummySend.setDelay("10ms");
    dummOnEntry.insertSend(dummySend);

    if(dTimeout>0)
    {
        if(!_isGenericSCXMLCofig)
        {
            GTASCXMLAssign assign;
            assign.setExpression(QString("Date.now() - %1").arg(timeoutStartTime));
            assign.setLocation(timeoutElapsedTime);
            dummOnEntry.insertAssign(assign);
            timeoutCondTrue = QString("(%1 >= %2) == true").arg(timeoutElapsedTime,QString::number(int(dTimeout)));
            timeoutCondFalse = QString("(%1 >= %2) == false").arg(timeoutElapsedTime,QString::number(int(dTimeout)));
        }

    }
    if(hasConfEvent && !_isGenericSCXMLCofig)
    {
        QString confElapsedTime1= QString("%1_confElapsedTime").arg(iStateId);
        GTASCXMLAssign assign;

        QString subConfStartTime = QString("%1_confStartTime").arg(iStateId);
        assign.setExpression(QString("Date.now() - %1").arg(subConfStartTime));
        assign.setLocation(confElapsedTime1);
        dummOnEntry.insertAssign(assign);
        waitUntilState1TargetId = QString("%1_dumpList").arg(iState.getId());

    }
    dummyTransitionState.setOnEntry(dummOnEntry);
    if(_isGenericSCXMLCofig)
    {
        if(!hasConfEvent)
        {
            insertSuccessTrasition(iWaitCmd,icdParamList,waitUntilFinalStateId,dummyEventName,dummyTransitionState,QString());
            insertFailTrasition(iWaitCmd,icdParamList,parametersStateId,dummyEventName,dummyTransitionState,QString(),false);
        }
        else
        {
            insertSuccessTrasition(iWaitCmd,icdParamList,waitUntilFinalStateId,dummyEventName,dummyTransitionState,QString());
            insertFailTrasition(iWaitCmd,icdParamList,waitUntilFinalStateId,dummyEventName,dummyTransitionState,QString(),false);
            insertSuccessTrasition(iWaitCmd,icdParamList,parametersStateId,dummyEventName,dummyTransitionState,QString(),QString(),false);
            insertFailTrasition(iWaitCmd,icdParamList,waitUntilFinalStateId,dummyEventName,dummyTransitionState);
        }
    }
    else
    {
        if(!hasConfEvent)
        {
            insertSuccessTrasition(iWaitCmd,icdParamList,waitUntilFinalStateId,dummyEventName,dummyTransitionState,QString(),timeoutCondFalse);
            insertFailTrasition(iWaitCmd,icdParamList,parametersStateId,dummyEventName,dummyTransitionState,QString(),false,timeoutCondFalse,false);
        }
        else
        {
            insertSuccessTrasition(iWaitCmd,icdParamList,waitUntilFinalStateId,dummyEventName,dummyTransitionState,QString(),QString(iConfEventName));
            insertFailTrasition(iWaitCmd,icdParamList,waitUntilFinalStateId,dummyEventName,dummyTransitionState,QString(),false,QString(iConfEventName));
            insertSuccessTrasition(iWaitCmd,icdParamList,parametersStateId,dummyEventName,dummyTransitionState,QString(),QString(),false);
            insertFailTrasition(iWaitCmd,icdParamList,waitUntilFinalStateId,dummyEventName,dummyTransitionState);
        }
    }

    if(dTimeout>0)
    {
        if(_isGenericSCXMLCofig)
            insertTimeoutTransition(iWaitCmd,timeoutName,waitUntilFinalStateId,dummyTransitionState);
        else
        {
            if(isConfTimeExist)
                insertTimeOutTrasitionWoEvent(iWaitCmd,dumplistStateId,dummyEventName,dummyTransitionState,timeoutCondTrue);
            else
                insertTimeOutTrasitionWoEvent(iWaitCmd,waitUntilFinalStateId,dummyEventName,dummyTransitionState,timeoutCondTrue);
        }

    }
    if(!iExtTransitions.isEmpty())
    {
        for(int i=0;i<iExtTransitions.count();i++)
            dummyTransitionState.insertTransitions(iExtTransitions.at(i));
    }


    GTASCXMLFinal waitUntilFinal;
    waitUntilFinal.setId(waitUntilFinalStateId);

    GTASCXMLState waitUntilState1;
    waitUntilState1.setId(waitUntilState1Id);
    waitUntilState1.insertStates(paramStates);
    waitUntilState1.insertStates(dummyTransitionState);
    waitUntilState1.setFinal(waitUntilFinal);

    GTASCXMLTransition WaitUntilState1Tx;
    WaitUntilState1Tx.setTarget(waitUntilState1TargetId);
    waitUntilState1.insertTransitions(WaitUntilState1Tx);


    GTASCXMLState waitUntilStateMain;
    waitUntilStateMain.setId(iStateId);

    waitUntilStateMain.insertStates(waitUntilState1);

    GTASCXMLFinal mainWaitUntilFinal;
    mainWaitUntilFinal.setId(waitUntilMainFinalId);
    waitUntilStateMain.setFinal(mainWaitUntilFinal);
    waitUntilStateMain.setOnEntry(onEntry);

    GTASCXMLTransition waitUntilMainStateTx;
    waitUntilMainStateTx.setTarget(iTargetId);
    waitUntilStateMain.insertTransitions(waitUntilMainStateTx);

    if(isConfTimeExist)
    {
        GTASCXMLState confTimeState;
        confTimeState.setId(confTimeStateId);
        GTASCXMLOnEntry onEntryConfTime;

        if(_isGenericSCXMLCofig)
        {
            GTASCXMLSend confTimeSend;
            confTimeSend.setDelay(QString::number(dConfTime)+"ms");
            confTimeSend.setEvent(confTimeEvent);
            onEntryConfTime.insertSend(confTimeSend);

        }
        else
        {
            GTASCXMLAssign confTimeAssign;
            confTimeAssign.setExpression("Date.now()");
            confTimeAssign.setLocation(confStartTime);
            onEntryConfTime.insertAssign(confTimeAssign);
        }
        confTimeState.setOnEntry(onEntryConfTime);

        GTASCXMLTransition confTimeTx;
        confTimeTx.setTarget(waitUntilState2Id);
        confTimeState.insertTransitions(confTimeTx);
        waitUntilStateMain.insertStates(confTimeState);

        GTASCXMLState waitUntilState2;
        waitUntilState2.setId(waitUntilState2Id);

        GTAActionWait *pConfWaitCmd = new GTAActionWait(GTAActionWait::UNTIL);
        pConfWaitCmd->setInstanceName(iWaitCmd->getInstanceName());
        pConfWaitCmd->setCondition(iWaitCmd->getCondition());
        pConfWaitCmd->setParameter(iWaitCmd->getParameter());
        pConfWaitCmd->setValue(iWaitCmd->getValue());
        pConfWaitCmd->setActionOnFail(iWaitCmd->getActionOnFail());
        pConfWaitCmd->setAction(iWaitCmd->getAction());
        pConfWaitCmd->setComplement(iWaitCmd->getComplement());
        pConfWaitCmd->setDumpList(dumpList);

        QList<GTASCXMLTransition> externalTxList;


        if(!_isGenericSCXMLCofig)
        {
            QString confDummyTxStateEventName = QString("%1_dummyEvent").arg(waitUntilState2Id);
            QString confTimeCond = QString("(%1 >= %2) == true").arg(confElapsedTime,QString::number(int(dConfTime)));


            if(dTimeout>0)
            {
                GTASCXMLTransition timeoutTx;
                getTimeoutTranstion(iWaitCmd,confDummyTxStateEventName,dumplistStateId,timeoutTx,timeoutCondTrue);
                externalTxList<<timeoutTx;
            }
            createSCXMLWaitUntil(pConfWaitCmd, waitUntilStateMain , waitUntilState2Id, waitUntilMainFinalId, externalTxList,confTimeCond);
        }
        else
        {
            if(dTimeout>0)
            {
                GTASCXMLTransition timeoutTx;
                getTimeoutTranstion(iWaitCmd,timeoutName,dumplistStateId,timeoutTx);
                externalTxList<<timeoutTx;
            }
            createSCXMLWaitUntil(pConfWaitCmd, waitUntilStateMain , waitUntilState2Id, waitUntilMainFinalId, externalTxList,confTimeEvent);
        }
    }
    if(!hasConfEvent)
        getStateForDumpListParameters(iWaitCmd,waitUntilStateMain,dumplistStateId,waitUntilMainFinalId);

    logAllParametersOnExit(iWaitCmd,iWaitCmd->getVariableList(),waitUntilStateMain);

    iState.insertStates(waitUntilStateMain);
}
void GTASCXMLController::createSCXMLReleaseParam(const GTAActionRelease *pReleaseParam, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &)
{
    QStringList params = pReleaseParam->getParameterList();

    if(pReleaseParam == NULL)
        return;
    QString cmdName = QString("%1 %2").arg(pReleaseParam->getAction(),pReleaseParam->getComplement()).trimmed();
    QStringList icdParamList;
    QString finalStateId = QString("%1_final").arg(iStateId);
    QString dummyTransitionStateId = QString("%1_transtionState").arg(iStateId);

    for(int i=0;i<params.count();i++)
    {
        if(_icdParamList.contains(params.at(i)))
            icdParamList.append(params.at(i));
    }
    GTASCXMLState releaseParamState;
    for(int i=0;i<icdParamList.count();i++)
    {
        GTASCXMLState paramState;
        QString paramStateId = QString("%1_param_%2").arg(iStateId,QString::number(i));
        QString paramTargetId = QString("%1_param_%2").arg(iStateId,QString::number(i+1));
        if(i == icdParamList.count() - 1)
        {
            paramTargetId  = dummyTransitionStateId;
        }
        bool rc = getStateForParam(paramState,paramStateId,paramTargetId,icdParamList.at(i),cmdName);
        if(rc)
        {
            releaseParamState.insertStates(paramState);

        }
    }



    GTASCXMLFinal finalState;
    finalState.setId(finalStateId);

    GTASCXMLState dummyTransitionState;
    dummyTransitionState.setId(dummyTransitionStateId);
    GTASCXMLOnEntry dummOnEntry;
    GTASCXMLSend dummySend;
    QString dummyEventName = QString("%1_dummyEvent").arg(iStateId);
    dummySend.setEvent(dummyEventName);
    dummySend.setDelay("10ms");
    dummOnEntry.insertSend(dummySend);
    dummyTransitionState.setOnEntry(dummOnEntry);

    insertSuccessTrasition(pReleaseParam,icdParamList,finalStateId,dummyEventName,dummyTransitionState);
    insertFailTrasition(pReleaseParam,icdParamList,finalStateId,dummyEventName,dummyTransitionState);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    tx.setStatus(SCXML_STATUS_PASS);


    releaseParamState.setId(iStateId);
    releaseParamState.insertStates(dummyTransitionState);
    releaseParamState.setFinal(finalState);
    releaseParamState.insertTransitions(tx);


    iState.insertStates(releaseParamState);

}
void GTASCXMLController::createSCXMLCall(const GTAActionExpandedCall *pCallCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &)
{
    if(pCallCmd == NULL)
        return;
    GTASCXMLState callState;
    callState.setId(iStateId);

    getStateChart(pCallCmd,callState);


    GTASCXMLFinal final;

    QString finalStateId = QString("%1_final").arg(iStateId);
    final.setId(finalStateId);

    callState.setFinal(final);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    callState.insertTransitions(tx);

    iState.insertStates(callState);
}
void GTASCXMLController::createSCXMLConditionIf(const GTAActionIF *iIfCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &)
{

    // Pending Items:
    //Timeout

    if(iIfCmd == NULL)
        return;
    GTAActionElse *pElseCmd = NULL;
    GTAActionIF *pIfCmd = new GTAActionIF(*iIfCmd);

    QStringList dumpList = iIfCmd->getDumpList();

    pIfCmd->clearChildrenList();

    // float dTimeout = iIfCmd->getTimeOutInMs();

    QStringList icdParamList;
    QString cmdName = QString("%1 %2").arg(iIfCmd->getAction(),iIfCmd->getComplement()).trimmed();


    QList<GTACommandBase *> children = iIfCmd->getChildren();
    for(int i=0;i<children.count();i++)
    {
        GTACommandBase *pCmd = children.at(i);
        if(pCmd != NULL)
        {
            GTAActionBase *pActBase = dynamic_cast<GTAActionBase *>(pCmd);
            if(pActBase != NULL)
            {
                QString com = pActBase->getComplement();
                if(com == COM_CONDITION_ELSE)
                {
                    pElseCmd = (GTAActionElse *)(pCmd);
                    break;
                }
                else if(com == COM_CONDITION_ENDIF)
                {
                    break;
                }
                else
                {
                    pIfCmd->insertChildren(pCmd,i);

                }
            }
            else if(pCmd->getCommandType() == GTACommandBase::CHECK)
            {
                pIfCmd->insertChildren(pCmd,i);
            }
        }
    }

    QString ifStateId = QString("%1_ifCond").arg(iStateId);
    QString elseStateId = QString("%1_elseCond").arg(iStateId);
    QString ifFinalStateId = QString("%1_final").arg(ifStateId);
    QString elseFinalStateId = QString("%1_final").arg(elseStateId);
    QString condStateId = QString("%1_cond").arg(iStateId);
    QString paramStateId = QString ("%1_params").arg(condStateId);
    QString parameterStatesFinalId = QString("%1_final").arg(paramStateId);
    QString dummyTransitionStateId = QString("%1_transtionState").arg(iStateId);
    QString cmdFinalId = QString("%1_final").arg(iStateId);
    QString dumplistStateId = QString("%1_dumpList").arg(iStateId);

    bool isCheckAudioAlarm = iIfCmd->getChkAudioAlarm();
    QString statusAssId = "";


    QString lhs  = iIfCmd->getParameter();
    QString rhs  = iIfCmd->getValue();

    updateWithGenToolParam(lhs);
    updateWithGenToolParam(rhs);
    GTASCXMLController::paramType type1 = getParamType(lhs);
    GTASCXMLController::paramType type2 = getParamType(rhs);


    GTASCXMLState paramStates;
    if(!isCheckAudioAlarm)
    {
        if(type1 == GTASCXMLController::PARAM_ICD)
        {
            GTASCXMLState paramState;
            QString subParamStateId = QString("%1_param_lhs").arg(iStateId);
            QString subParamTargetId = QString("%1_param_rhs").arg(iStateId);
            if(type2 != GTASCXMLController::PARAM_ICD)
            {
                subParamTargetId = parameterStatesFinalId;
            }
            bool rc = getStateForParam(paramState,paramStateId, subParamStateId,lhs,cmdName);
            if(rc)
            {
                paramStates.insertStates(paramState);
                icdParamList.append(lhs);
            }
        }
        if(type2 == GTASCXMLController::PARAM_ICD)
        {
            GTASCXMLState paramState;
            QString subParamStateId = QString("%1_param_rhs").arg(iStateId);
            //QString subParamTargetId = parameterStatesFinalId;
            bool rc = getStateForParam(paramState,paramStateId, subParamStateId,rhs,cmdName);
            if(rc)
            {
                paramStates.insertStates(paramState);
                icdParamList.append(rhs);
            }
        }
    }
    else
    {
        GTACheckAudioAlarm sAudioAlarm;
        sAudioAlarm.setDumpList(iIfCmd->getDumpList());
        sAudioAlarm.setInstanceName(iIfCmd->getInstanceName());
        sAudioAlarm.setValue(iIfCmd->getValue());
        sAudioAlarm.setTime(iIfCmd->getHeardAfter());
        QString audioStateId= QString("%1_audio").arg(iStateId);
        createSCXMLCheckAudioAlarm(&sAudioAlarm,paramStates,audioStateId,parameterStatesFinalId,QStringList(),iIfCmd->getCondition());
        statusAssId = QString("_%1_status").arg(audioStateId);
    }

    if(!paramStates.isNull())
    {
        GTASCXMLFinal paramStatesFinal;
        paramStatesFinal.setId(parameterStatesFinalId);
        paramStates.setFinal(paramStatesFinal);
    }

    paramStates.setId(paramStateId);
    GTASCXMLTransition paramStatesTx;
    paramStatesTx.setTarget(dummyTransitionStateId);
    paramStates.insertTransitions(paramStatesTx);


    GTASCXMLState dummyTransitionState;
    dummyTransitionState.setId(dummyTransitionStateId);
    GTASCXMLOnEntry dummOnEntry;
    GTASCXMLSend dummySend;
    QString dummyEventName = QString("%1_dummyEvent").arg(iStateId);
    dummySend.setEvent(dummyEventName);
    dummySend.setDelay("10ms");
    dummOnEntry.insertSend(dummySend);
    dummyTransitionState.setOnEntry(dummOnEntry);


    insertSuccessTrasition(iIfCmd,icdParamList,ifStateId,dummyEventName,dummyTransitionState,statusAssId);
    insertFailTrasition(iIfCmd,icdParamList,elseStateId,dummyEventName,dummyTransitionState,statusAssId);

    GTASCXMLState ifState;
    ifState.setId(ifStateId);
    getStateChart(pIfCmd,ifState);


    GTASCXMLState elseState;
    elseState.setId(elseStateId);
    if(pElseCmd != NULL)
    {
        getStateChart(pElseCmd,elseState);
    }



    GTASCXMLState cmdState;
    cmdState.setId(iStateId);
    GTASCXMLFinal cmdFinal;
    cmdFinal.setId(cmdFinalId);
    cmdState.setFinal(cmdFinal);

    GTASCXMLTransition cmdTx;
    cmdTx.setTarget(iTargetId);
    cmdState.insertTransitions(cmdTx);

    GTASCXMLFinal ifFinal;
    ifFinal.setId(ifFinalStateId);
    ifState.setFinal(ifFinal);
    GTASCXMLFinal elseFinal;
    elseFinal.setId(elseFinalStateId);
    elseState.setFinal(elseFinal);

    GTASCXMLTransition ifElseTx;
    ifElseTx.setTarget(dumplistStateId);
    ifState.insertTransitions(ifElseTx);
    elseState.insertTransitions(ifElseTx);




    cmdState.insertStates(paramStates);
    cmdState.insertStates(dummyTransitionState);
    cmdState.insertStates(ifState);
    cmdState.insertStates(elseState);

    getStateForDumpListParameters(iIfCmd,cmdState,dumplistStateId,cmdFinalId);

    logAllParametersOnExit(iIfCmd,iIfCmd->getVariableList(),cmdState);

    iState.insertStates(cmdState);
}
void GTASCXMLController::createSCXMLCheckAudioAlarm(const GTACheckAudioAlarm *iAudioAlarm, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &, const QString &iOperator)
{
    // Pending Items:
    //Dumplist
    //Timeout


    if(iAudioAlarm == NULL)
        return;

    QString cmdName = QString("%1 %2").arg(iAudioAlarm->getCheckName()).trimmed();
    QString actionOnFail = iAudioAlarm->getActionOnFail();
    QStringList dumpListParams = iAudioAlarm->getDumpList();
    QString heardAfter = iAudioAlarm->getTime();
    QString paramValue = iAudioAlarm->getAlarmName();
    QString templateName = "check_Audio_Alarm.scxml";
    QHash<QString,QString> opMap;
    QString invokeDelay = "";


    opMap.insert(ARITH_EQ,"'EQ'");
    opMap.insert(ARITH_NOTEQ,"'NE'");
    opMap.insert(ARITH_GT,"'GT'");
    opMap.insert(ARITH_LT,"'LT'");
    opMap.insert(ARITH_GTEQ,"'GoE'");
    opMap.insert(ARITH_LTEQ,"'LoE'");


    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");
    QString retName = QString("_invoke_%1_return").arg(invokeId);
    QString invokeDelaySendEvent = QString("%1_hiddenWaitTime").arg(iStateId);



    GTASCXMLInvoke invoke;
    bool rc = getCloneInvoke(templateName,invoke);

    if(rc)
    {
        invoke.setInvokeId(invokeId);

        GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();

        GTASCXMLXMLRPC::funcArg arg2 ;
        func.getArgumentByName("content",arg2);
        GTASCXMLXMLRPC::funcArg arg4 ;
        func.getArgumentByName("condition",arg4);
        GTASCXMLXMLRPC::funcArg arg6 ;
        func.getArgumentByName("delay",arg6);

        arg2.value = QString("'%1'").arg(paramValue);

        if(!iOperator.trimmed().isEmpty())
        {
            arg4.value = opMap.value(iOperator);
        }
        arg6.value = heardAfter;

        func.updateArgumentByName("content",arg2);
        func.updateArgumentByName("condition",arg4);
        func.updateArgumentByName("delay",arg6);

        GTASCXMLXMLRPC::funcReturn ret;
        rc = func.getReturnArgument(ret);
        if(rc)
        {
            ret.name = retName;
            func.updateReturnArgument(ret);
        }
	
        invoke.setInvokeFunction(func);
        GTASCXMLAssign assign;
        assign.setExpression(QString("_event.data.%1").arg(retName));
        assign.setLocation(retName);
        invoke.insertAssign(assign);
    }


    GTASCXMLTransition successTx;
    GTASCXMLTransition failTx;
    if(rc)
    {
        successTx.setCondition(QString("%1.Status == 'OK'").arg(retName));
        failTx.setCondition(QString("%1.Status != 'OK'").arg(retName));
    }

    successTx.setTarget(iTargetId);
    successTx.setStatus(SCXML_STATUS_PASS);
    successTx.setEvent("done.invoke._all");

    failTx.setTarget(iTargetId);
    failTx.setStatus(SCXML_STATUS_FAIL);
    failTx.setEvent("done.invoke._all");

    if(actionOnFail.trimmed().contains("stop"))
        failTx.setTarget(SCXML_FINAL_ID);


    GTAScxmlLogMessage passLogMsg;
    passLogMsg.Identifier = iAudioAlarm->getInstanceName();
    passLogMsg.CurrentValue = "true";
    passLogMsg.ExpectedValue = "true";
    passLogMsg.LOD = GTAScxmlLogMessage::Main;
    passLogMsg.Result = GTAScxmlLogMessage::OK;
    passLogMsg.Status = GTAScxmlLogMessage::Success;
    passLogMsg.isCurrentValParam = false;
    QString log1 = passLogMsg.getLogMessage(_isGenericSCXMLCofig);
    log1 = log1.remove("+");
    log1 = log1.remove("'");

    GTASCXMLLog successLog;
    successLog.setExpression(QString("'%1'").arg(log1));
    successLog.setLabel("'Info'");
    successTx.insertLog(successLog);


    GTAScxmlLogMessage failLogMsg;
    failLogMsg.Identifier = iAudioAlarm->getInstanceName();
    failLogMsg.CurrentValue = "true";
    failLogMsg.ExpectedValue = "true";
    failLogMsg.LOD = GTAScxmlLogMessage::Main;
    failLogMsg.Result = GTAScxmlLogMessage::OK;
    failLogMsg.Status = GTAScxmlLogMessage::Success;
    failLogMsg.isCurrentValParam = false;
    QString log2 = failLogMsg.getLogMessage(_isGenericSCXMLCofig);
    log2 = log2.remove("+");
    log2 = log2.remove("'");

    GTASCXMLLog failLog;
    failLog.setExpression(QString("'%1'").arg(log2));
    failLog.setLabel("'Info'");
    failTx.insertLog(failLog);

    GTASCXMLState CheckAudioState;
    CheckAudioState.setId(iStateId);
    CheckAudioState.insertInvokes(invoke);

    QString statusAssId = QString("_%1_status").arg(iStateId);

    GTASCXMLAssign statusAss;

    statusAss.setLocation(statusAssId);
    statusAss.setExpression("true");
    successTx.insertAssign(statusAss);
    statusAss.setExpression("fail");
    failTx.insertAssign(statusAss);

    rc = getInvokeDealy(cmdName,invokeDelay);
    if(rc)
    {
        GTASCXMLSend invokeDelaySend;
        invokeDelaySend.setDelay(invokeDelay);
        invokeDelaySend.setEvent(invokeDelaySendEvent);

        GTASCXMLOnEntry onEntry;
        onEntry.insertSend(invokeDelaySend);

        successTx.setEvent(invokeDelaySendEvent);
        failTx.setEvent(invokeDelaySendEvent);
        CheckAudioState.setOnEntry(onEntry);
    }
    CheckAudioState.insertTransitions(successTx);
    CheckAudioState.insertTransitions(failTx);

    iState.insertStates(CheckAudioState);
}
void GTASCXMLController::createSCXMLConditionWhile(const GTAActionWhile *iWhileCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &)
{

    if(iWhileCmd == NULL)
        return;
    GTAActionWhile *pWhileCmd = new GTAActionWhile(*iWhileCmd);

    QStringList dumpList = iWhileCmd->getDumpList();

    pWhileCmd->clearChildrenList();

    double dTimeout = iWhileCmd->getTimeOutInMs();

    QStringList icdParamList;
    QString cmdName = QString("%1 %2").arg(iWhileCmd->getAction(),iWhileCmd->getComplement()).trimmed();


    QList<GTACommandBase *> children = iWhileCmd->getChildren();
    for(int i=0;i<children.count();i++)
    {
        GTACommandBase *pCmd = children.at(i);
        if(pCmd != NULL)
        {
            GTAActionBase *pActBase = dynamic_cast<GTAActionBase *>(pCmd);
            if(pActBase != NULL)
            {
                QString com = pActBase->getComplement();
                if(com == COM_CONDITION_ENDWHILE)
                {
                    break;
                }
                else
                {
                    pWhileCmd->insertChildren(pCmd,i);
                }
            }
        }
    }

    QString whileStateId = QString("%1_whileCond").arg(iStateId);
    QString whileFinalStateId = QString("%1_final").arg(whileStateId);
    QString condStateId = QString("%1_cond").arg(iStateId);
    QString paramStateId = QString ("%1_params").arg(condStateId);
    QString parameterStatesFinalId = QString("%1_final").arg(paramStateId);
    QString dummyTransitionStateId = QString("%1_transtionState").arg(iStateId);
    QString cmdFinalId = QString("%1_final").arg(iStateId);
    QString dumpListStateId = QString("%1_dumpList").arg(iStateId);
    QString timeoutEventName = QString("%1_timeout_event").arg(iStateId);
    QString timeoutStartTime = QString("%1_timeoutStartTime").arg(iStateId);
    QString timeoutElapsedTime = QString("%1_timeoutElapsedTime").arg(iStateId);
    bool isCheckAudioAlarm = iWhileCmd->getChkAudioAlarm();
    QString statusAssId = "";


    QString lhs  = iWhileCmd->getParameter();
    QString rhs  = iWhileCmd->getValue();

    updateWithGenToolParam(lhs);
    updateWithGenToolParam(rhs);

    GTASCXMLController::paramType type1 = getParamType(lhs);
    GTASCXMLController::paramType type2 = getParamType(rhs);



    GTASCXMLState paramStates;
    if(!isCheckAudioAlarm)
    {
        if(type1 == GTASCXMLController::PARAM_ICD)
        {
            GTASCXMLState paramState;
            QString subParamStateId = QString("%1_param_lhs").arg(iStateId);
            QString subParamTargetId = QString("%1_param_rhs").arg(iStateId);
            if(type2 != GTASCXMLController::PARAM_ICD)
            {
                subParamTargetId = parameterStatesFinalId;
            }
            bool rc = getStateForParam(paramState, subParamStateId , subParamTargetId, lhs, cmdName);
            if(rc)
            {
                paramStates.insertStates(paramState);
                icdParamList.append(lhs);
            }
        }
        if(type2 == GTASCXMLController::PARAM_ICD)
        {
            GTASCXMLState paramState;
            QString subParamStateId = QString("%1_param_rhs").arg(iStateId);
            QString subParamTargetId = parameterStatesFinalId;
            bool rc = getStateForParam(paramState, subParamStateId, subParamTargetId, rhs, cmdName);
            if(rc)
            {
                paramStates.insertStates(paramState);
                icdParamList.append(rhs);
            }
        }
    }
    else
    {
        GTACheckAudioAlarm sAudioAlarm;
        sAudioAlarm.setDumpList(iWhileCmd->getDumpList());
        sAudioAlarm.setInstanceName(iWhileCmd->getInstanceName());
        sAudioAlarm.setValue(iWhileCmd->getValue());
        sAudioAlarm.setTime(iWhileCmd->getHeardAfter());
        QString audioStateId= QString("%1_audio").arg(iStateId);
        createSCXMLCheckAudioAlarm(&sAudioAlarm,paramStates,audioStateId,parameterStatesFinalId,QStringList(),iWhileCmd->getCondition());
        statusAssId = QString("_%1_status").arg(audioStateId);
    }

    if(!paramStates.isNull())
    {
        GTASCXMLFinal paramStatesFinal;
        paramStatesFinal.setId(parameterStatesFinalId);
        paramStates.setFinal(paramStatesFinal);
    }

    paramStates.setId(paramStateId);
    GTASCXMLTransition paramStatesTx;
    paramStatesTx.setTarget(dummyTransitionStateId);
    paramStates.insertTransitions(paramStatesTx);


    GTASCXMLState dummyTransitionState;
    dummyTransitionState.setId(dummyTransitionStateId);
    GTASCXMLOnEntry dummOnEntry;
    GTASCXMLSend dummySend;
    QString dummyEventName = QString("%1_dummyEvent").arg(iStateId);
    dummySend.setEvent(dummyEventName);
    dummySend.setDelay("10ms");
    dummOnEntry.insertSend(dummySend);
    dummyTransitionState.setOnEntry(dummOnEntry);


    if(_isGenericSCXMLCofig)
    {
        insertSuccessTrasition(iWhileCmd,icdParamList,whileStateId,dummyEventName,dummyTransitionState,statusAssId);
        insertFailTrasition(iWhileCmd,icdParamList,dumpListStateId,dummyEventName,dummyTransitionState,statusAssId,true);
        insertTimeoutTransition(iWhileCmd,timeoutEventName,dumpListStateId,dummyTransitionState);
    }
    else
    {
        QString timeoutCondTrue = QString("%1 >= %2").arg(timeoutElapsedTime,QString::number(dTimeout));
        QString timeoutCondFalse = QString("%1 < %2").arg(timeoutElapsedTime,QString::number(dTimeout));
        insertSuccessTrasition(iWhileCmd,icdParamList,whileStateId,dummyEventName,dummyTransitionState,statusAssId,timeoutCondFalse);
        insertFailTrasition(iWhileCmd,icdParamList,dumpListStateId,dummyEventName,dummyTransitionState,statusAssId,true,timeoutCondFalse);
        insertTimeOutTrasitionWoEvent(iWhileCmd,dumpListStateId,dummyEventName,dummyTransitionState,timeoutCondTrue);
    }


    GTASCXMLState whileState;
    whileState.setId(whileStateId);
    getStateChart(pWhileCmd,whileState);



    GTASCXMLState cmdState;
    cmdState.setId(iStateId);
    GTASCXMLFinal cmdFinal;
    cmdFinal.setId(cmdFinalId);
    cmdState.setFinal(cmdFinal);

    GTASCXMLTransition cmdTx;
    cmdTx.setTarget(paramStateId);
    cmdState.insertTransitions(cmdTx);

    GTASCXMLFinal whileFinal;
    whileFinal.setId(whileFinalStateId);
    whileState.setFinal(whileFinal);


    GTASCXMLTransition whileTx;
    whileTx.setTarget(cmdFinalId);
    whileState.insertTransitions(whileTx);

    cmdState.insertStates(paramStates);
    cmdState.insertStates(dummyTransitionState);
    cmdState.insertStates(whileState);

    getStateForDumpListParameters(iWhileCmd,cmdState,dumpListStateId,iTargetId);

    GTASCXMLOnEntry onEntry;
    if(_isGenericSCXMLCofig)
    {
        GTASCXMLSend timeOutSend;
        timeOutSend.setDelay(QString::number(dTimeout)+"ms");
        timeOutSend.setEvent(timeoutEventName);
        onEntry.insertSend(timeOutSend);
    }
    else
    {
        GTASCXMLAssign timeoutAssign;
        timeoutAssign.setExpression("Date.now()");
        timeoutAssign.setExpression(timeoutStartTime);
    }

    cmdState.setOnEntry(onEntry);


    logAllParametersOnExit(iWhileCmd,iWhileCmd->getVariableList(),cmdState);


    iState.insertStates(cmdState);
}
void GTASCXMLController::createSCXMLConditionDoWhile(const GTAActionDoWhile *iDoWhileCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &)
{

    if(iDoWhileCmd == NULL)
        return;
    GTAActionDoWhile *pWhileCmd = new GTAActionDoWhile(*iDoWhileCmd);

    QStringList dumpList = iDoWhileCmd->getDumpList();

    pWhileCmd->clearChildrenList();

    double dTimeout = iDoWhileCmd->getTimeOutInMs();

    QStringList icdParamList;
    QString cmdName = QString("%1 %2").arg(iDoWhileCmd->getAction(),iDoWhileCmd->getComplement()).trimmed();


    QList<GTACommandBase *> children = iDoWhileCmd->getChildren();
    for(int i=0;i<children.count();i++)
    {
        GTACommandBase *pCmd = children.at(i);
        if(pCmd != NULL)
        {
            GTAActionBase *pActBase = dynamic_cast<GTAActionBase *>(pCmd);
            if(pActBase != NULL)
            {
                QString com = pActBase->getComplement();
                if(com == COM_CONDITION_DO_WHILE_END)
                {
                    break;
                }
                else
                {
                    pWhileCmd->insertChildren(pCmd,i);
                }
            }
        }
    }

    QString whileStateId = QString("%1_doWhileCond").arg(iStateId);
    QString whileFinalStateId = QString("%1_final").arg(whileStateId);
    QString condStateId = QString("%1_cond").arg(iStateId);
    QString paramStateId = QString ("%1_params").arg(condStateId);
    QString parameterStatesFinalId = QString("%1_final").arg(paramStateId);
    QString dummyTransitionStateId = QString("%1_transtionState").arg(iStateId);
    QString cmdFinalId = QString("%1_final").arg(iStateId);
    QString dumpListStateId = QString("%1_dumpList").arg(iStateId);
    QString timeoutEventName = QString("%1_timeout_event").arg(iStateId);

    QString timeoutStartTime = QString("%1_timeoutStartTime").arg(iStateId);
    QString timeoutElapsedTime = QString("%1_timeoutElapsedTime").arg(iStateId);


    bool isCheckAudioAlarm = iDoWhileCmd->getChkAudioAlarm();
    QString statusAssId = "";


    QString lhs  = iDoWhileCmd->getParameter();
    QString rhs  = iDoWhileCmd->getValue();
    updateWithGenToolParam(lhs);
    updateWithGenToolParam(rhs);
    GTASCXMLController::paramType type1 = getParamType(lhs);
    GTASCXMLController::paramType type2 = getParamType(rhs);


    GTASCXMLState paramStates;
    if(!isCheckAudioAlarm)
    {
        if(type1 == GTASCXMLController::PARAM_ICD)
        {
            GTASCXMLState paramState;
            QString subParamStateId = QString("%1_param_lhs").arg(iStateId);
            QString subParamTargetId = QString("%1_param_rhs").arg(iStateId);
            if(type2 != GTASCXMLController::PARAM_ICD)
            {
                subParamTargetId = parameterStatesFinalId;
            }
            bool rc = getStateForParam(paramState, subParamStateId, subParamTargetId, lhs, cmdName);
            if(rc)
            {
                paramStates.insertStates(paramState);
                icdParamList.append(lhs);
            }
        }
        //TODO: This setting of parameter ID is copied like a thousand times, separate it into a method
        if(type2 == GTASCXMLController::PARAM_ICD)
        {
            GTASCXMLState paramState;
            QString subParamStateId = QString("%1_param_rhs").arg(iStateId);
            //QString subParamTargetId = parameterStatesFinalId;
            bool rc = getStateForParam(paramState,paramStateId, subParamStateId,rhs,cmdName);
            if(rc)
            {
                insertTimeoutTransition(iDoWhileCmd,timeoutEventName,dumpListStateId,paramState);
                paramStates.insertStates(paramState);
                icdParamList.append(rhs);
            }
        }
    }
    else
    {
        GTACheckAudioAlarm sAudioAlarm;
        sAudioAlarm.setDumpList(iDoWhileCmd->getDumpList());
        sAudioAlarm.setInstanceName(iDoWhileCmd->getInstanceName());
        sAudioAlarm.setValue(iDoWhileCmd->getValue());
        sAudioAlarm.setTime(iDoWhileCmd->getHeardAfter());
        QString audioStateId= QString("%1_audio").arg(iStateId);
        createSCXMLCheckAudioAlarm(&sAudioAlarm,paramStates,audioStateId,parameterStatesFinalId,QStringList(),iDoWhileCmd->getCondition());
        statusAssId = QString("_%1_status").arg(audioStateId);
    }

    if(!paramStates.isNull())
    {
        GTASCXMLFinal paramStatesFinal;
        paramStatesFinal.setId(parameterStatesFinalId);
        paramStates.setFinal(paramStatesFinal);
    }

    paramStates.setId(paramStateId);
    GTASCXMLTransition paramStatesTx;
    paramStatesTx.setTarget(dummyTransitionStateId);
    paramStates.insertTransitions(paramStatesTx);


    GTASCXMLState dummyTransitionState;
    dummyTransitionState.setId(dummyTransitionStateId);
    GTASCXMLOnEntry dummOnEntry;
    GTASCXMLSend dummySend;
    QString dummyEventName = QString("%1_dummyEvent").arg(iStateId);
    dummySend.setEvent(dummyEventName);
    dummySend.setDelay("10ms");
    dummOnEntry.insertSend(dummySend);
    dummyTransitionState.setOnEntry(dummOnEntry);



    if(_isGenericSCXMLCofig)
    {
        insertSuccessTrasition(iDoWhileCmd,icdParamList,cmdFinalId,dummyEventName,dummyTransitionState,statusAssId);
        insertFailTrasition(iDoWhileCmd,icdParamList,dumpListStateId,dummyEventName,dummyTransitionState,statusAssId);
        insertTimeoutTransition(iDoWhileCmd,timeoutEventName,dumpListStateId,dummyTransitionState);
    }
    else
    {
        QString timeoutCondTrue = QString("%1 >= %2").arg(timeoutElapsedTime,QString::number(dTimeout));
        QString timeoutCondFalse = QString("%1 < %2").arg(timeoutElapsedTime,QString::number(dTimeout));
        insertSuccessTrasition(iDoWhileCmd,icdParamList,cmdFinalId,dummyEventName,dummyTransitionState,statusAssId,timeoutCondFalse);
        insertFailTrasition(iDoWhileCmd,icdParamList,dumpListStateId,dummyEventName,dummyTransitionState,statusAssId,true,timeoutCondFalse);
        insertTimeOutTrasitionWoEvent(iDoWhileCmd,dumpListStateId,dummyEventName,dummyTransitionState,timeoutCondTrue);
    }

    GTASCXMLState whileState;
    whileState.setId(whileStateId);
    getStateChart(pWhileCmd,whileState);

    GTASCXMLState cmdState;
    cmdState.setId(iStateId);
    GTASCXMLFinal cmdFinal;
    cmdFinal.setId(cmdFinalId);
    cmdState.setFinal(cmdFinal);

    GTASCXMLTransition cmdTx;
    cmdTx.setTarget(whileStateId);
    cmdState.insertTransitions(cmdTx);

    GTASCXMLFinal whileFinal;
    whileFinal.setId(whileFinalStateId);
    whileState.setFinal(whileFinal);


    GTASCXMLTransition whileTx;
    whileTx.setTarget(paramStateId);
    whileState.insertTransitions(whileTx);


    cmdState.insertStates(whileState);
    cmdState.insertStates(paramStates);
    cmdState.insertStates(dummyTransitionState);

    getStateForDumpListParameters(iDoWhileCmd,cmdState,dumpListStateId,iTargetId);

    GTASCXMLOnEntry onEntry;

    if(_isGenericSCXMLCofig)
    {
        GTASCXMLSend timeOutSend;
        timeOutSend.setDelay(QString::number(dTimeout)+"ms");
        timeOutSend.setEvent(timeoutEventName);
        onEntry.insertSend(timeOutSend);
    }
    else
    {
        GTASCXMLAssign timeoutAssign;
        timeoutAssign.setExpression("Date.now()");
        timeoutAssign.setExpression(timeoutStartTime);
    }

    logAllParametersOnExit(iDoWhileCmd,icdParamList,cmdState);

    iState.insertStates(cmdState);
}
void GTASCXMLController::createSCXMLSetGeneric(const GTAActionSet *iSetCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts)
{
    QString param = iSetCmd->getParameter();
    updateWithGenToolParam(param);
    GTAEquationBase *pEq = iSetCmd->getEquation();
    QString fsVal = iSetCmd->getFunctionStatus();
    bool isSetOnlyFS = iSetCmd->getIsSetOnlyFSFixed();
    QString actOnFail = iSetCmd->getActionOnFail();
    QString dummyEventName = QString("%1_event").arg(iStateId);
    QString functionValue= "";
    QString functionType = "";
    GTAICDParameter icdParam1 = _icdParamList.value(param);
    QString rhsValue;
    QString rhs;
    GTASCXMLController::paramType type1 = getParamType(param);
    GTASCXMLController::paramType type2 = PARAM_UNKNOWN;


    QString multiSetListName = QString("_%1_MultiSetList").arg(iStateId);
    QString setRetName = QString("_%1_return").arg(iStateId);

    if(!isSetOnlyFS)
    {

        pEq->getFunctionStatement(functionType, functionValue);
        functionType = pEq->getSimulationName();

        if(functionType == EQ_CONST)
        {
            GTAEquationConst *pConstEq = dynamic_cast<GTAEquationConst*>(pEq);
            rhsValue = pConstEq->getConstant();
            rhs = rhsValue;
            type2 = getParamType(rhsValue);
        }
    }

    GTASCXMLState SetState;
    QStringList icdParamList;
    QString cmdName = QString("%1 %2").arg(iSetCmd->getAction(),iSetCmd->getComplement()).trimmed();

    QString getParamStateId = QString("%1_getParamValue").arg(iStateId);
    QString setParamStateId = QString("%1_setParamValue").arg(iStateId);
    SetState.setId(setParamStateId);

    if(type2 == GTASCXMLController::PARAM_ICD)
    {
        GTASCXMLState getParamState;
        getParamState.setId(getParamStateId);
        bool rc = getStateForParam(getParamState,getParamStateId,setParamStateId,rhsValue,"getParamValueSingleSample");

        if(rc)
        {
            rhsValue = rhsValue.append(".Value");

            icdParamList.append(rhsValue);
            iState.insertStates(getParamState);
        }
    }


    GTASCXMLOnEntry onEntry;


    if(type1 == GTASCXMLController::PARAM_ICD)
    {

        if(functionType == EQ_CONST)
        {
            GTASCXMLInvoke setCmdInvoke;
            getCloneInvoke("set_generic.scxml",setCmdInvoke);
            GTASCXMLGenericFunction func =  setCmdInvoke.getGenericFunction();

            func.updateAttributeByName("name",param);
            func.updateAttributeByName("value",rhsValue);
            func.setFunctionReturn(param);

            setCmdInvoke.setInvokeFunctionGeneric(func);


            QString invokeId = setParamStateId;
            invokeId = invokeId.replace("s","i");
            setCmdInvoke.setInvokeId(invokeId);

            GTASCXMLAssign invokeAssign;
            invokeAssign.setExpression(QString("_event.data.%1").arg(param));
            invokeAssign.setLocation(param+".value");
            setCmdInvoke.insertAssign(invokeAssign);

            SetState.insertInvokes(setCmdInvoke);
            insertSuccessTrasition(iSetCmd,icdParamList,iTargetId,"done.invoke._all",SetState,QString("%1.Status == 'OK'").arg(rhs));
            insertFailTrasition(iSetCmd,icdParamList,iTargetId,"done.invoke._all",SetState,QString("%1.Status != 'OK'").arg(rhs));
        }
    }
    else
    {
        GTASCXMLAssign multiSetListAssign2;
        multiSetListAssign2.setLocation(QString("%1").arg(param));
        multiSetListAssign2.setExpression(rhsValue);
        onEntry.insertAssign(multiSetListAssign2);

        insertSuccessTrasition(iSetCmd,icdParamList,iTargetId,"",SetState);
    }


    for(int i=0;i<iExtTimeouts.count();i++)
        insertTimeoutTransition(iSetCmd,iExtTimeouts.at(i),iTargetId,SetState);

    SetState.setOnEntry(onEntry);
    iState.insertStates(SetState);
}
void GTASCXMLController::createSCXMLSet(const GTAActionSet *iSetCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &iExtTimeouts, const QString &iInvokeDealy)
{
    if(iSetCmd == NULL)
        return;
    if(_isGenericSCXMLCofig)
    {
        createSCXMLSetGeneric(iSetCmd, iState,iStateId, iTargetId,iExtTimeouts);
        return;
    }


    QString param = iSetCmd->getParameter();
    updateWithGenToolParam(param);
    GTAEquationBase *pEq = iSetCmd->getEquation();
    QString fsVal = iSetCmd->getFunctionStatus();
    bool isSetOnlyFSFixed = iSetCmd->getIsSetOnlyFSFixed();
    bool isSetOnlyFSVariable = iSetCmd->getIsSetOnlyFSVariable();
    QString actOnFail = iSetCmd->getActionOnFail();
    QString funcType = "";
    QString dummyEventName = QString("%1_event").arg(iStateId);
    QString functionValue= "";
    QString functionType = "";
    GTAICDParameter icdParam1 = _icdParamList.value(param);
    QString rhsValue;
    QString rhs;
    GTASCXMLController::paramType type1 = getParamType(param);
    GTASCXMLController::paramType type2 = PARAM_UNKNOWN;


    QString multiSetListName = QString("%1_MultiSetList").arg(iStateId);
    QString setRetName = QString("_%1_return").arg(iStateId);

    if(!isSetOnlyFSFixed && !isSetOnlyFSFixed)
    {

        pEq->getFunctionStatement(functionType, functionValue);
        functionType = pEq->getSimulationName();

        if(functionType == EQ_CONST)
        {
            GTAEquationConst *pConstEq = dynamic_cast<GTAEquationConst*>(pEq);
            rhsValue = pConstEq->getConstant();
            rhs = rhsValue;
            updateWithGenToolParam(rhsValue);
            type2 = getParamType(rhsValue);
            funcType = icdParam1.getValueType();
        }
        else
        {
            funcType = functionType;
        }
    }

    GTASCXMLState SetState;
    QStringList icdParamList;
    QString cmdName = QString("%1 %2").arg(iSetCmd->getAction(),iSetCmd->getComplement()).trimmed();

    QString getParamStateId = QString("%1_getParamValue").arg(iStateId);
    QString setParamStateId = QString("%1_setParamValue").arg(iStateId);
    SetState.setId(setParamStateId);

    if(type2 == GTASCXMLController::PARAM_ICD)
    {
        GTASCXMLState getParamState;
        getParamState.setId(getParamStateId);
        bool rc = getStateForParam(getParamState,getParamStateId,setParamStateId,rhsValue,"getParamValueSingleSample");

        if(rc)
        {
            rhsValue = rhsValue.append(".Value");

            icdParamList.append(rhsValue);
            iState.insertStates(getParamState);
        }
    }

    GTASCXMLOnEntry onEntry;


    if(type1 == GTASCXMLController::PARAM_ICD)
    {
        GTASCXMLAssign multiSetListAssign;
        multiSetListAssign.setLocation(QString("%1[0]").arg(multiSetListName));
        multiSetListAssign.setExpression(QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3',%4}").arg(icdParam1.getTempParamName(),funcType,fsVal,functionValue));
        QString invokeDelayEvent = QString("%1_invoke_dealy").arg(iStateId);

        cmdParameterList.append(multiSetListName);

        onEntry.insertAssign(multiSetListAssign);

        if(!iInvokeDealy.isEmpty())
        {
            GTASCXMLSend invokeDealySend;
            invokeDealySend.setDelay(iInvokeDealy);
            invokeDealySend.setEvent(invokeDelayEvent);
            onEntry.insertSend(invokeDealySend);
        }
        if(functionType == EQ_CONST)
        {
            GTASCXMLAssign multiSetListAssign1;
            multiSetListAssign1.setLocation(QString("%1[0].FunctionValue.Value").arg(multiSetListName));
            multiSetListAssign1.setExpression(rhsValue);
            onEntry.insertAssign(multiSetListAssign1);
        }


        GTASCXMLInvoke setCmdInvoke;
        getCloneInvoke(cmdName+".scxml",setCmdInvoke);
        GTASCXMLXMLRPC func =  setCmdInvoke.getXMLRPCFunction();
        GTASCXMLXMLRPC::funcArg args;
        func.getArgumentByName("ParameterList",args);
        args.value = multiSetListName;
        GTASCXMLXMLRPC::funcReturn ret;
        func.getReturnArgument(ret);
        ret.name =setRetName;

        QString invokeId = setParamStateId;
        invokeId = invokeId.replace("s","i");
        setCmdInvoke.setInvokeId(invokeId);

        func.updateArgumentByName("ParameterList",args);
        func.updateReturnArgument(ret);

        setCmdInvoke.setInvokeFunction(func);

        GTASCXMLAssign invokeAssign;
        invokeAssign.setExpression(QString("_event.data.%1").arg(setRetName));
        invokeAssign.setLocation(setRetName);
        setCmdInvoke.insertAssign(invokeAssign);

        SetState.insertInvokes(setCmdInvoke);

        if(!iInvokeDealy.isEmpty())
        {
            insertSuccessTrasition(iSetCmd,icdParamList,iTargetId,invokeDelayEvent,SetState,QString("%1.Status == 'OK'").arg(setRetName));
            insertFailTrasition(iSetCmd,icdParamList,iTargetId,invokeDelayEvent,SetState,QString("%1.Status != 'OK'").arg(setRetName));

        }
        else
        {
            insertSuccessTrasition(iSetCmd,icdParamList,iTargetId,"done.invoke._all",SetState,QString("%1.Status == 'OK'").arg(setRetName));
            insertFailTrasition(iSetCmd,icdParamList,iTargetId,"done.invoke._all",SetState,QString("%1.Status != 'OK'").arg(setRetName));
        }
    }
    else
    {
        GTASCXMLAssign multiSetListAssign2;
        multiSetListAssign2.setLocation(QString("%1").arg(param));
        multiSetListAssign2.setExpression(rhsValue);
        onEntry.insertAssign(multiSetListAssign2);
        insertSuccessTrasition(iSetCmd,icdParamList,iTargetId,"",SetState);
    }
    for(int i=0;i<iExtTimeouts.count();i++)
        insertTimeoutTransition(iSetCmd,iExtTimeouts.at(i),iTargetId,SetState);

    SetState.setOnEntry(onEntry);

    logAllParametersOnExit(iSetCmd,iSetCmd->getVariableList(),SetState);

    iState.insertStates(SetState);
}
void GTASCXMLController::createSCXMLSetList(const GTAActionSetList *iSetCmdList, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId,const QStringList &)
{
    double dTimeout = iSetCmdList->getTimeOutInMs();
    QString actOnFail = iSetCmdList->getActionOnFail();
    QStringList varList = iSetCmdList->getVariableList();
    QString invokeDelay = "";
    QString cmdName = QString("%1 %2").arg(iSetCmdList->getAction(), iSetCmdList->getComplement());
    getInvokeDealy(cmdName,invokeDelay);

    GTASCXMLState setListState;
    setListState.setId(iStateId);

    int setCnt = iSetCmdList->getSetActionCount();
    QString finalStateId = QString("%1_final").arg(iStateId);
    QString dummyTransitionStateId = QString("%1_transtionState").arg(iStateId);
    QString timeoutName = QString("%1_timeoutEvent").arg(iStateId);
    QString dumpListStateId = QString("%1_dumpList").arg(iStateId);
    QStringList icdParamList;

    for(int i=0;i<varList.count();i++)
    {
        if(_icdParamList.contains(varList.at(i)))
            icdParamList.append(varList.at(i));
    }

    for(int i =0;i<setCnt;i++)
    {
        GTAActionSet *setCmd = NULL;
        bool rc = iSetCmdList->getSetAction(i,setCmd);
        if(rc)
        {
            setCmd->setTimeOut(QString::number(dTimeout),"ms");
            setCmd->setActionOnFail(actOnFail);
            setCmd->setInstanceName(iSetCmdList->getInstanceName());
            QString setStateId = QString("%1_setCmd_%2").arg(iStateId,QString::number(i));
            QString setTargetId = QString("%1_setCmd_%2").arg(iStateId,QString::number(i+1));
            QString setFinalId = QString("%1_setCmd_%2_final").arg(iStateId,QString::number(i));
            if(i >= (setCnt - 1))
                setTargetId =dumpListStateId;
            GTASCXMLState setState;
            setState.setId(setStateId);
            createSCXMLSet(setCmd,setState,setStateId,setFinalId,QStringList()<<timeoutName,invokeDelay);


            GTASCXMLFinal setStateFinal;
            setStateFinal.setId(setFinalId);
            setState.setFinal(setStateFinal);

            GTASCXMLTransition setStateTx;
            setStateTx.setTarget(setTargetId);
            setState.insertTransitions(setStateTx);

            setListState.insertStates(setState);
        }
    }
    GTASCXMLFinal finalState;
    finalState.setId(finalStateId);
    setListState.setFinal(finalState);


    GTASCXMLState dummyTransitionState;
    dummyTransitionState.setId(dummyTransitionStateId);
    GTASCXMLOnEntry dummOnEntry;
    GTASCXMLSend dummySend;
    QString dummyEventName = QString("%1_dummyEvent").arg(iStateId);
    dummySend.setEvent(dummyEventName);
    dummySend.setDelay("10ms");
    dummOnEntry.insertSend(dummySend);
    dummyTransitionState.setOnEntry(dummOnEntry);


    insertSuccessTrasition(iSetCmdList,icdParamList,dumpListStateId,dummyEventName,dummyTransitionState);


    if(dTimeout>0)
    {
        insertTimeoutTransition(iSetCmdList,timeoutName,finalStateId,dummyTransitionState);
        GTASCXMLOnEntry onEntry;
        GTASCXMLSend send;
        send.setDelay(QString::number(dTimeout).trimmed() + "ms");
        send.setEvent(timeoutName);
        onEntry.insertSend(send);
        setListState.setOnEntry(onEntry);
    }

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    setListState.insertTransitions(tx);
    //setListState.insertStates(dummyTransitionState);

    getStateForDumpListParameters(iSetCmdList,setListState,dumpListStateId,finalStateId);
    iState.insertStates(setListState);
}
void GTASCXMLController::createSCXMLManualAction(const GTAActionManual *iManActCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iManActCmd == NULL)
        return;

    QString message = iManActCmd->getMessage();
    bool waitForAck = iManActCmd->getAcknowledgetment();
    bool waitForFeedback = iManActCmd->getUserFeedback();
    QString userFeedBack = iManActCmd->getParameter();
    updateWithGenToolParam(userFeedBack);
    QString returnType = iManActCmd->getReturnType();
    QString dummyFeedBack = userFeedBack;
    QString returnName = QString("%1_return").arg(iStateId);
    GTASCXMLState cmdState;
    cmdState.setId(iStateId);

    // bool hasChannel = iManActCmd->hasChannelInControl();
    GTASCXMLInvoke manActInvoke;

    QString cmdName = QString("%1 %2").arg(iManActCmd->getAction(),iManActCmd->getComplement());
    QString invokeDelay="";
    bool isInvokeAvail = getInvokeDealy(cmdName,invokeDelay);

    if(!isInvokeAvail)
        invokeDelay="";


    QStringList resolvedParams = GTAUtil::getProcessedParameterForChannel(userFeedBack,true,true);
    if(waitForFeedback)
    {
        GTASCXMLController::paramType type = GTASCXMLController::PARAM_UNKNOWN;
        if(resolvedParams.count()>0)
            type = getParamType(resolvedParams.at(0));
        else
            type = getParamType(userFeedBack);

        if((type == GTASCXMLController::PARAM_LOCAL) || (type == GTASCXMLController::PARAM_ICD))
        {

            bool rc = getCloneInvoke("manual_action_feedback.scxml",manActInvoke);
            if(rc)
            {
                if(type == PARAM_ICD)
                {
                    dummyFeedBack = QString("_%1_feedback").arg(iStateId);
                }
                GTASCXMLXMLRPC func= manActInvoke.getXMLRPCFunction();
                GTASCXMLXMLRPC::funcArg arg;
                func.getArgumentByName("message",arg);
                arg.value = QString("%1").arg(message);

                GTASCXMLXMLRPC::funcArg arg1;
                func.getArgumentByName("ExpectedReturnType",arg1);
                arg1.value = QString("%1").arg(returnType);

                GTASCXMLXMLRPC::funcReturn ret;
                func.getReturnArgument(ret);
                ret.name =returnName;

                func.updateArgumentByName("message",arg);
                func.updateArgumentByName("ExpectedReturnType",arg1);
                func.updateReturnArgument(ret);
                manActInvoke.setInvokeFunction(func);

                GTASCXMLAssign assign;
                assign.setExpression(QString("_event.data.%1").arg(returnName));
                assign.setLocation(dummyFeedBack);

                manActInvoke.insertAssign(assign);

                if(type != PARAM_ICD)
                {
                    cmdState.insertInvokes(manActInvoke);
                    logAllParametersOnExit(iManActCmd,QStringList()<<userFeedBack,cmdState);
                    logAllParametersOnExit(iManActCmd,QStringList()<<dummyFeedBack,cmdState);
                }
            }
        }

        if(type == PARAM_ICD)
        {
            QString manActStateId = QString("%1_manAct").arg(iStateId);
            QString manActTargetId = QString("%1_setCmd").arg(iStateId);
            QString finalStateId = QString("%1_final").arg(iStateId);
            QString setCmdFinalId = QString("%1_final").arg(manActTargetId);
            GTASCXMLState manActState;
            manActState.setId(manActStateId);
            manActState.insertInvokes(manActInvoke);
            insertSuccessTrasition(iManActCmd,QStringList(),manActTargetId,"done.invoke._all",manActState,QString());



            GTAActionSetList setListCmd;
            setListCmd.setAction(ACT_SET);
            setListCmd.setTimeOut(ACT_DEFAULT_TIMEOUT,ACT_TIMEOUT_UNIT_SEC);

            if(resolvedParams.count()>0)
            {
                for(int i=0;i<resolvedParams.count();i++)
                {
                    GTAActionSet setCmd;
                    setCmd.setInstanceName(iManActCmd->getInstanceName());
                    setCmd.setParameter(resolvedParams.at(i));
                    setCmd.setIsSetOnlyFSFixed(false);
                    setCmd.setIsSetOnlyFSVariable(false);

                    GTAEquationConst *pEq = new GTAEquationConst();
                    pEq->setConstant(dummyFeedBack);
                    GTAEquationBase *pEqBase = pEq;
                    setCmd.setEquation(pEqBase);
                    setCmd.setAction(ACT_SET_INSTANCE);

                    setListCmd.addSetAction(&setCmd);
                }
            }
            else
            {
                GTAActionSet setCmd;
                setCmd.setInstanceName(iManActCmd->getInstanceName());
                setCmd.setParameter(userFeedBack);
                setCmd.setIsSetOnlyFSFixed(false);
                setCmd.setIsSetOnlyFSVariable(false);

                GTAEquationConst *pEq = new GTAEquationConst();
                pEq->setConstant(dummyFeedBack);
                GTAEquationBase *pEqBase = pEq;
                setCmd.setEquation(pEqBase);
                setCmd.setAction(ACT_SET_INSTANCE);

                setListCmd.addSetAction(&setCmd);
            }
            GTASCXMLState setCmdState;
            setCmdState.setId(manActTargetId);
            insertSuccessTrasition(iManActCmd,QStringList(),finalStateId,"",setCmdState,QString());

            GTASCXMLFinal setCmdFinal;
            setCmdFinal.setId(setCmdFinalId);
            setCmdState.setFinal(setCmdFinal);
            createSCXMLSetList(&setListCmd,setCmdState,manActTargetId,setCmdFinalId,QStringList());

            cmdState.insertStates(manActState);
            cmdState.insertStates(setCmdState);
            GTASCXMLFinal final;
            final.setId(finalStateId);
            cmdState.setFinal(final);
            insertSuccessTrasition(iManActCmd,QStringList(),iTargetId,QString(""),cmdState,QString(""));
        }
        else
            insertSuccessTrasition(iManActCmd,QStringList(),iTargetId,QString("done.invoke._all"),cmdState,QString(""));
    }
    else
    {
        bool rc = getCloneInvoke("manual_action_acknowledgement.scxml",manActInvoke);
        if(rc)
        {
            GTASCXMLXMLRPC func= manActInvoke.getXMLRPCFunction();
            GTASCXMLXMLRPC::funcArg arg;
            func.getArgumentByName("message",arg);
            arg.value = QString("%1").arg(message);

            GTASCXMLXMLRPC::funcArg arg1;
            func.getArgumentByName("wait_ack",arg1);
            if(waitForAck)
                arg1.value = QString("'true'");
            else
                arg1.value = QString("'false'");


            func.updateArgumentByName("message",arg);
            func.updateArgumentByName("wait_ack",arg1);

            manActInvoke.setInvokeFunction(func);
            cmdState.insertInvokes(manActInvoke);
        }
        insertSuccessTrasition(iManActCmd,QStringList(),iTargetId,"done.invoke._all",cmdState,QString(""));
    }

    iState.insertStates(cmdState);
}
void GTASCXMLController::createSCXMLGenericFunction(const GTAActionGenericFunction *iGenFunc,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    QString scxmlText = iGenFunc->getSCXMLText();
    double dTimeOut = iGenFunc->getTimeOutInMs();
    bool isCustomcond = iGenFunc->hasUsercondiiton();
    QString returnName = iGenFunc->getFuncReturnName();

    QDomDocument domGenericFuncElem;
    domGenericFuncElem.setContent(scxmlText);

    QString timeOutName = QString("%1_timeout_event").arg(iStateId);

    if(!domGenericFuncElem.isNull())
    {
        GTASCXMLState genFuncState;
        genFuncState.setId(iStateId);

        GTASCXMLSend timeoutSend;
        timeoutSend.setDelay(QString::number(dTimeOut)+"ms");
        timeoutSend.setEvent(timeOutName);
        GTASCXMLOnEntry onEntry;
        onEntry.insertSend(timeoutSend);

        QDomElement rootElem = domGenericFuncElem.documentElement();
        QDomNodeList invokeElemList = rootElem.elementsByTagName(SXN_INVOKE);
        QDomNodeList txElemList = rootElem.elementsByTagName(SXN_TRANSITION);
        GTASCXMLInvoke invoke;
        if(!invokeElemList.isEmpty())
        {
            invoke.translateXMLToStruct(invokeElemList.at(0).toElement());
        }
        genFuncState.insertInvokes(invoke);

        if(!isCustomcond)
        {
            insertSuccessTrasition(iGenFunc,QStringList(),iTargetId,"done.invoke._all",genFuncState,QString("%1.Status == 'OK'").arg(returnName));
            insertFailTrasition(iGenFunc,QStringList(),iTargetId,"done.invoke._all",genFuncState,QString("%1.Status != 'OK'").arg(returnName));
        }
        else
        {
            for(int i=0;i<txElemList.count();i++)
            {
                QDomNode txElem = txElemList.at(i);
                if(!txElem.isNull())
                {
                    GTASCXMLTransition tx;
                    tx.translateXMLToStruct(txElem.toElement());
                    genFuncState.insertTransitions(tx);
                }
            }
        }
        insertTimeoutTransition(iGenFunc,timeOutName,iTargetId,genFuncState);

        iState.insertStates(genFuncState);
    }
}
void GTASCXMLController::createSCXMLMaths(const GTAActionMaths *iMaths,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iMaths == NULL)
        return;

    GTAActionMaths *pMathsCommand = (GTAActionMaths *)iMaths;
    if(pMathsCommand == NULL)
        return;
    QString lhs= pMathsCommand->getFirstVariable();
    updateWithGenToolParam(lhs);
    pMathsCommand->updateGenToolRetrunParams(_genToolReturnMap);
    QString rhs= pMathsCommand->getSCXMLExpr();
    QString cmdName = pMathsCommand->getAction();

    QStringList varList = pMathsCommand->getVariableList();
    QStringList icdParamList;
    paramType lhsType = getParamType(lhs);

    QString location = QString("_%1_mathsLoc").arg(iStateId);
    QString paramStateId = QString("%1_getParams").arg(iStateId);
    QString mathsStateId = QString("%1_MathsState").arg(iStateId);
    QString mathsStateFinalId = QString("%1_MathsState_final").arg(iStateId);
    QString paramStateFinalId = QString("%1_getParam_final").arg(iStateId);
    QString StateFinalId = QString("%1_final").arg(iStateId);
    QString setParameterStateId = QString("%1_setParamValue").arg(iStateId);

    QString invokeDelay;
    // bool isInvokeDelayAvail = 
	getInvokeDealy(cmdName,invokeDelay);

    GTASCXMLState MathsCommandState;
    MathsCommandState.setId(iStateId);



    if((lhsType == GTASCXMLController::PARAM_LOCAL))
    {
        location = lhs;
    }
    else if((lhsType != GTASCXMLController::PARAM_ICD))
    {
        GTASCXMLTransition tx;
        tx.setTarget(iTargetId);
        MathsCommandState.insertTransitions(tx);
        iState.insertStates(MathsCommandState);
        return;
    }


    if(!rhs.contains(lhs))
    {
        int idx = varList.indexOf(lhs);
        if(idx != -1)
            varList.removeAt(idx);
    }
    varList.removeDuplicates();

    for(int i=0;i<varList.count();i++)
    {
        if(_icdParamList.contains(varList.at(i)))
            icdParamList.append(varList.at(i));
    }
    GTASCXMLState getParamState;
    bool hasGetParamState = false;
    for(int i=0;i<icdParamList.count();i++)
    {
        GTASCXMLState paramState;
        QString subParamStateId = QString("%1_param_%2").arg(iStateId,QString::number(i));
        QString paramTargetId = QString("%1_param_%2").arg(iStateId,QString::number(i+1));
        if(i == icdParamList.count() - 1)
        {
            paramTargetId  = paramStateFinalId;
        }
        bool rc = getStateForParam(paramState,paramStateId, subParamStateId,icdParamList.at(i),cmdName);
        if(rc)
        {
            getParamState.insertStates(paramState);
            hasGetParamState = true;
        }
    }



    for(int i=0;i<varList.count();i++)
    {
        if(rhs.contains(varList.at(i)))
        {
            QString param = varList.at(i);
            if(getParamType(param) == GTASCXMLController::PARAM_ICD)
                rhs.replace(param,QString("%1.Value").arg(param));
        }
    }

    GTASCXMLState mathsState;
    mathsState.setId(mathsStateId);

    GTASCXMLAssign assign;

    assign.setLocation(location);
    assign.setExpression(rhs);

    GTASCXMLOnExit onExit;
    onExit.insertAssign(assign);

    mathsState.setOnExit(onExit);


    if(hasGetParamState)
    {
        getParamState.setId(paramStateId);
        GTASCXMLFinal paramFinal;
        paramFinal.setId(paramStateFinalId);
        getParamState.setFinal(paramFinal);
        GTASCXMLTransition getParamTx;
        getParamTx.setTarget(mathsStateFinalId);
        getParamState.insertTransitions(getParamTx);

        mathsState.insertStates(getParamState);

        GTASCXMLFinal mathsStateFinal;
        mathsStateFinal.setId(mathsStateFinalId);
        mathsState.setFinal(mathsStateFinal);
    }

    if(lhsType == GTASCXMLController::PARAM_ICD)
    {

        GTASCXMLTransition mathsStateTx;
        mathsStateTx.setTarget(setParameterStateId);
        mathsState.insertTransitions(mathsStateTx);

        GTAActionSet setCmd;
        setCmd.setInstanceName(pMathsCommand->getInstanceName());
        setCmd.setParameter(lhs);
        setCmd.setIsSetOnlyFSFixed(false);
        setCmd.setIsSetOnlyFSVariable(false);

        GTAEquationConst *pEq = new GTAEquationConst();
        pEq->setConstant(location);
        GTAEquationBase *pEqBase = pEq;
        setCmd.setEquation(pEqBase);
        setCmd.setAction(ACT_SET_INSTANCE);


        MathsCommandState.insertStates(mathsState);

        createSCXMLSet(&setCmd,MathsCommandState,iStateId,StateFinalId,QStringList(),invokeDelay);
        insertSuccessTrasition(pMathsCommand,QStringList(),iTargetId,"",MathsCommandState,QString(""));
    }
    else
    {
        GTASCXMLTransition mathsStateTx;
        mathsStateTx.setTarget(StateFinalId);
        mathsState.insertTransitions(mathsStateTx);
        MathsCommandState.insertStates(mathsState);
        insertSuccessTrasition(pMathsCommand,icdParamList,iTargetId,"",MathsCommandState,QString(""));
    }

    GTASCXMLFinal final;
    final.setId(StateFinalId);
    MathsCommandState.setFinal(final);


    logAllParametersOnExit(pMathsCommand,pMathsCommand->getVariableList(),MathsCommandState);

    iState.insertStates(MathsCommandState);
}
void GTASCXMLController::createSCXMLCheckFWCWarning(const GTACheckFwcWarning *iChkFwcWarning,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iChkFwcWarning == NULL)
        return;

    QString message = iChkFwcWarning->getMessage();
    QString heading = iChkFwcWarning->getHeader();
    QString color = iChkFwcWarning->getFWCWarningColor();
    QString delay = iChkFwcWarning->getWaitForBuffer();

    if(message.contains("<"))
        message.replace("<","&lt;");

    QString sIsDisplayed = iChkFwcWarning->getCondition();

    QString retName = QString("_%1_return").arg(iStateId);
    QString timeoutEventName = QString("%1_timeout_event").arg(iStateId);
    QString cmdName = "check_FWC_Warning";
    GTASCXMLInvoke invoke;
    getCloneInvoke(cmdName+".scxml",invoke);

    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");

    invoke.setInvokeId(invokeId);

    GTASCXMLXMLRPC func=  invoke.getXMLRPCFunction();

    GTASCXMLXMLRPC::funcArg args;
    func.getArgumentByName("content",args);

    args.value = (QString("%1 %2").arg(heading,message)).trimmed();

    func.updateArgumentByName("content",args);

    func.getArgumentByName("condition_value",args);

    args.value = QString("%1").arg(color);

    func.updateArgumentByName("condition_value",args);

    func.getArgumentByName("delay",args);

    args.value = QString("%1").arg(delay);

    func.updateArgumentByName("delay",args);

    GTASCXMLXMLRPC::funcReturn ret;
    func.getReturnArgument(ret);

    ret.name =retName;

    func.updateReturnArgument(ret);

    invoke.setInvokeFunction(func);

    GTASCXMLAssign assign;

    assign.setExpression(QString("_event.data.%1").arg(retName));
    assign.setLocation(retName);

    invoke.insertAssign(assign);

    GTASCXMLState checkFwcWarningState;
    checkFwcWarningState.setId(iStateId);

    checkFwcWarningState.insertInvokes(invoke);

    if(sIsDisplayed.trimmed().isEmpty())
    {
        insertSuccessTrasition(iChkFwcWarning,QStringList(),iTargetId,"done.invoke._all",checkFwcWarningState,QString("%1.ResultStatus == 'OK'").arg(retName));
        insertFailTrasition(iChkFwcWarning,QStringList(),iTargetId,"done.invoke._all",checkFwcWarningState,QString("%1.ResultStatus == 'OK'").arg(retName));
    }
    else
    {
        insertSuccessTrasition(iChkFwcWarning,QStringList(),iTargetId,"done.invoke._all",checkFwcWarningState,QString("%1.ResultStatus != 'OK'").arg(retName));
        insertFailTrasition(iChkFwcWarning,QStringList(),iTargetId,"done.invoke._all",checkFwcWarningState,QString("%1.ResultStatus != 'OK'").arg(retName));
    }

    GTASCXMLSend send;
    send.setEvent(timeoutEventName);

    bool isNum = false;
    int idealy = delay.toInt(&isNum);
    if(idealy)
        send.setDelay(QString("%1ms").arg(QString::number(idealy*1000 + 3000)));
    else
        send.setDelay(QString("%1ms").arg(QString::number(3000)));

    GTASCXMLOnEntry onEntry;
    onEntry.insertSend(send);


    checkFwcWarningState.setOnEntry(onEntry);

    insertTimeoutTransition(iChkFwcWarning,timeoutEventName,iTargetId,checkFwcWarningState);

    iState.insertStates(checkFwcWarningState);
}
void GTASCXMLController::createSCXMLCheckBITEMessage(const GTACheckBiteMessage *iChkBiteMessage,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iChkBiteMessage == NULL)
        return;

    QString message = iChkBiteMessage->getMessage();
    QString delay = iChkBiteMessage->getBuffDepth();
    if(message.contains("<"))
        message.replace("<","&lt;");

    QString sIsDisplayed = iChkBiteMessage->getCondition();

    QString retName = QString("_%1_return").arg(iStateId);
    QString timeoutEventName = QString("%1_timeout_event").arg(iStateId);
    QString cmdName = "check_BITE_Message";
    GTASCXMLInvoke invoke;
    getCloneInvoke(cmdName+".scxml",invoke);

    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");

    invoke.setInvokeId(invokeId);

    GTASCXMLXMLRPC func=  invoke.getXMLRPCFunction();

    GTASCXMLXMLRPC::funcArg args;
    func.getArgumentByName("content",args);

    args.value = (QString("%1").arg(message)).trimmed();

    func.updateArgumentByName("content",args);


    func.getArgumentByName("delay",args);

    args.value = QString("%1").arg(delay);

    func.updateArgumentByName("delay",args);

    GTASCXMLXMLRPC::funcReturn ret;
    func.getReturnArgument(ret);

    ret.name =retName;

    func.updateReturnArgument(ret);

    invoke.setInvokeFunction(func);

    GTASCXMLAssign assign;

    assign.setExpression(QString("_event.data.%1").arg(retName));
    assign.setLocation(retName);

    invoke.insertAssign(assign);

    GTASCXMLState checkBiteMessageState;
    checkBiteMessageState.setId(iStateId);

    checkBiteMessageState.insertInvokes(invoke);

    if(sIsDisplayed.trimmed().isEmpty())
    {
        insertSuccessTrasition(iChkBiteMessage,QStringList(),iTargetId,"done.invoke._all",checkBiteMessageState,QString("%1.Status == 'OK'").arg(retName));
        insertFailTrasition(iChkBiteMessage,QStringList(),iTargetId,"done.invoke._all",checkBiteMessageState,QString("%1.Status == 'OK'").arg(retName));
    }
    else
    {
        insertSuccessTrasition(iChkBiteMessage,QStringList(),iTargetId,"done.invoke._all",checkBiteMessageState,QString("%1.Status != 'OK'").arg(retName));
        insertFailTrasition(iChkBiteMessage,QStringList(),iTargetId,"done.invoke._all",checkBiteMessageState,QString("%1.Status != 'OK'").arg(retName));
    }

    GTASCXMLSend send;
    send.setEvent(timeoutEventName);
    bool isNum = false;
    int idealy = delay.toInt(&isNum);
    if(idealy)
        send.setDelay(QString("%1ms").arg(QString::number(idealy*1000 + 3000)));
    else
        send.setDelay(QString("%1ms").arg(QString::number(3000)));

    GTASCXMLOnEntry onEntry;
    onEntry.insertSend(send);


    checkBiteMessageState.setOnEntry(onEntry);

    insertTimeoutTransition(iChkBiteMessage,timeoutEventName,iTargetId,checkBiteMessageState);

    iState.insertStates(checkBiteMessageState);
}
void GTASCXMLController::createSCXMLCheckValue(const GTACheckValue *iChkValue,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QList<GTASCXMLTransition> &iExtTransitions, const QString &iConfEventName)
{
    if(iChkValue == NULL)
        return;


    QStringList varList = iChkValue->getVariableList();
    bool isWaitUntil = iChkValue->getWaitUntil();
    QString confTime,confTimeUnit;
    double dTimeout = iChkValue->getTimeOutInMs();
    iChkValue->getConfCheckTime(confTime,confTimeUnit);
    QString cmdName = "check_value";
    bool hasConfEvent = !iConfEventName.isEmpty();

    bool isConfTimeOk;

    double dConfTime = confTime.toDouble(&isConfTimeOk);

    if(confTimeUnit != "ms")
    {
        dConfTime = dConfTime *1000;
    }

    QString timeoutStartTime = QString("%1_TimeoutStart").arg(iStateId);
    QString timeoutElapsedTime = QString("%1_TimeoutElapsed").arg(iStateId);
    QString timeoutCondTrue = QString("%1 >= %2").arg(timeoutElapsedTime,QString::number(dTimeout));
    QString timeoutCondFalse = QString("%1 < %2").arg(timeoutElapsedTime,QString::number(dTimeout));


    QStringList icdParamList;
    for(int i=0;i<varList.count();i++)
        if(_icdParamList.contains(varList.at(i)))
            icdParamList.append(varList.at(i));

    QString cond = "";

    GTASCXMLState timeoutState;
    GTASCXMLState checkValueState1;
    GTASCXMLState confTimeState;
    GTASCXMLState checkValueState2;
    GTASCXMLState getParamState;
    GTASCXMLState dummyTranstionState;

    QString timeoutStateId = QString("%1_timeout").arg(iStateId);
    QString confTimeStateId = QString("%1_confTime").arg(iStateId);
    QString checkValueState1Id = QString("%1_checkVal1").arg(iStateId);
    QString checkValueState2Id = QString("%1_checkVal2").arg(iStateId);
    QString dummyTranstionStateId =QString("%1_transtionState").arg(iStateId);
    QString getParamStateId = QString("%1_getParamState").arg(iStateId);
    QString dumpListStateId = QString("%1_dumpList").arg(iStateId);

    QString timeoutEventName = QString("%1_timeout_event").arg(iStateId);
    QString ConfTimeEventName = QString("%1_confTime_event").arg(iStateId);
    QString dummyTranstionStateEventName = QString("%1_transtion_event").arg(iStateId);


    QString chkValFinalStateId = QString("%1_final").arg(iStateId);
    QString getParamFinalStateId = QString("%1_final").arg(getParamStateId);
    QString chkVal1FinalStateId = QString("%1_final").arg(checkValueState1Id);
    QString chkVal2FinalStateId = QString("%1_final").arg(checkValueState2Id);


    QString confTimeStartTime = QString("%1_ConfStartTime").arg(checkValueState2Id);
    QString confTimeElapsedTime = QString("%1_ConfElapsedTime").arg(checkValueState2Id);
    QString confCondTrue = QString("%1 >= %2").arg(confTimeElapsedTime,QString::number(dConfTime));
    QString confCondFalse = QString("%1 < %2").arg(confTimeElapsedTime,QString::number(dConfTime));



    dummyTranstionState.setId(dummyTranstionStateId);

    GTASCXMLOnEntry dummyOnEntry;

    GTASCXMLSend dummySend;
    dummySend.setEvent(dummyTranstionStateEventName);
    dummySend.setDelay("10ms");

    dummyOnEntry.insertSend(dummySend);


    if(!_isGenericSCXMLCofig)
    {
        if(dTimeout>0)
        {
            GTASCXMLAssign timeoutElpAss;
            timeoutElpAss.setExpression(QString("Date.now() - %1").arg(timeoutStartTime));
            timeoutElpAss.setLocation(timeoutElapsedTime);
            dummyOnEntry.insertAssign(timeoutElpAss);
        }
        if(hasConfEvent)
        {
            QString confStartTime = QString("%1_ConfStartTime").arg(iStateId);
            QString confElapsedTime = QString("%1_ConfElapsedTime").arg(iStateId);

            GTASCXMLAssign confElpAss;
            confElpAss.setExpression(QString("Date.now() - %1").arg(confStartTime));
            confElpAss.setLocation(confElapsedTime);
            dummyOnEntry.insertAssign(confElpAss);
        }
    }
    dummyTranstionState.setOnEntry(dummyOnEntry);

    getParamState.setId(getParamStateId);

    GTASCXMLFinal getParamStateFinal;
    getParamStateFinal.setId(getParamFinalStateId);
    getParamState.setFinal(getParamStateFinal);

    GTASCXMLState checkValueState;
    checkValueState.setId(iStateId);

    checkValueState1.setId(checkValueState1Id);

    GTASCXMLFinal checkVal1Final;
    checkVal1Final.setId(chkVal1FinalStateId);

    checkValueState1.setFinal(checkVal1Final);



    if((dTimeout>0) && (!hasConfEvent))
    {
        GTASCXMLOnEntry onEntry;
        if(_isGenericSCXMLCofig)
        {
            GTASCXMLSend timeoutSend;
            timeoutSend.setEvent(timeoutEventName);
            timeoutSend.setDelay(QString("%1ms").arg(QString::number(dTimeout)));
            onEntry.insertSend(timeoutSend);
        }
        else
        {
            GTASCXMLAssign timeoutAss;
            timeoutAss.setExpression("Date.now()");
            timeoutAss.setLocation(timeoutStartTime);
            onEntry.insertAssign(timeoutAss);
        }
        timeoutState.setOnEntry(onEntry);
        timeoutState.setId(timeoutStateId);
        GTASCXMLTransition timeoutTx;
        timeoutTx.setTarget(checkValueState1Id);
        timeoutState.insertTransitions(timeoutTx);
        checkValueState.insertStates(timeoutState);
    }

    if(!iExtTransitions.isEmpty())
    {
        for(int i=0;i<iExtTransitions.count();i++)
            dummyTranstionState.insertTransitions(iExtTransitions.at(i));
    }
    if(_isGenericSCXMLCofig)
    {

    }
    else
    {
        for(int i=0;i<icdParamList.count();i++)
        {
            GTASCXMLState paramState;
            QString paramStateId = QString("%1_param_%2").arg(checkValueState1Id,QString::number(i));
            QString paramTargetId = QString("%1_param_%2").arg(checkValueState1Id,QString::number(i+1));
            if(i == icdParamList.count() - 1)
            {
                paramTargetId  = dummyTranstionStateId;
            }
            bool rc = getStateForParam(paramState,paramStateId,paramTargetId,icdParamList.at(i),cmdName);
            if(rc)
            {
                getParamState.insertStates(paramState);
            }
        }
    }
    if(isWaitUntil)
    {
        if(!hasConfEvent)
        {
            insertSuccessTrasition(iChkValue,icdParamList,getParamFinalStateId,dummyTranstionStateEventName,dummyTranstionState,QString(),timeoutCondFalse);
            insertFailTrasition(iChkValue,icdParamList,getParamStateId,dummyTranstionStateEventName,dummyTranstionState,QString(),false,timeoutCondFalse,false);
            if(_isGenericSCXMLCofig)
            {
                if(dTimeout>0)
                    insertTimeoutTransition(iChkValue,timeoutEventName,dumpListStateId,dummyTranstionState);
            }
            else
            {
                insertTimeOutTrasitionWoEvent(iChkValue,dumpListStateId,dummyTranstionStateEventName,dummyTranstionState,timeoutCondTrue);
            }
        }
        else
        {
            if(_isGenericSCXMLCofig)
            {
                insertSuccessTrasition(iChkValue,icdParamList,getParamFinalStateId,iConfEventName,dummyTranstionState);
                insertFailTrasition(iChkValue,icdParamList,getParamFinalStateId,iConfEventName,dummyTranstionState);
                insertSuccessTrasition(iChkValue,icdParamList,getParamStateId,dummyTranstionStateEventName,dummyTranstionState,QString(),QString(),false);
                insertFailTrasition(iChkValue,icdParamList,getParamFinalStateId,dummyTranstionStateEventName,dummyTranstionState);
            }
            else
            {
                insertSuccessTrasition(iChkValue,icdParamList,getParamFinalStateId,dummyTranstionStateEventName,dummyTranstionState,QString(),QString(iConfEventName));
                insertFailTrasition(iChkValue,icdParamList,getParamFinalStateId,dummyTranstionStateEventName,dummyTranstionState,QString(),false,QString(iConfEventName));
                insertSuccessTrasition(iChkValue,icdParamList,getParamStateId,dummyTranstionStateEventName,dummyTranstionState,QString(),QString(),false);
                insertFailTrasition(iChkValue,icdParamList,getParamFinalStateId,dummyTranstionStateEventName,dummyTranstionState);
            }
        }
    }
    else
    {
        insertSuccessTrasition(iChkValue,icdParamList,dumpListStateId,dummyTranstionStateEventName,dummyTranstionState,QString());
        insertFailTrasition(iChkValue,icdParamList,dumpListStateId,dummyTranstionStateEventName,dummyTranstionState,QString());
    }

    getParamState.insertStates(dummyTranstionState);
    GTASCXMLTransition getParamTx;
    getParamTx.setTarget(chkVal1FinalStateId);
    getParamState.insertTransitions(getParamTx);
    checkValueState1.insertStates(getParamState);


    if(dConfTime > 0)
    {
        GTASCXMLTransition chkVal1Tx;
        chkVal1Tx.setTarget(confTimeStateId);
        checkValueState1.insertTransitions(chkVal1Tx);
        checkValueState.insertStates(checkValueState1);


        confTimeState.setId(confTimeStateId);
        GTASCXMLOnEntry confOnEntry;

        if(_isGenericSCXMLCofig)
        {
            GTASCXMLSend confSend;
            confSend.setEvent(ConfTimeEventName);
            confSend.setDelay(QString("%1ms").arg(QString::number(dConfTime)));
            confOnEntry.insertSend(confSend);
        }
        else
        {
            GTASCXMLAssign confAss;
            confAss.setExpression("Date.now()");
            confAss.setLocation(confTimeStartTime);
            confOnEntry.insertAssign(confAss);
        }

        confTimeState.setOnEntry(confOnEntry);



        GTASCXMLTransition confTimeTx;
        confTimeTx.setTarget(checkValueState2Id);
        confTimeState.insertTransitions(confTimeTx);

        checkValueState.insertStates(confTimeState);

        GTACheckValue *pConfChkCmd = new GTACheckValue(*iChkValue);
        pConfChkCmd->setConfCheckTime(ACT_DEFAULT_CONFTIME,ACT_TIMEOUT_UNIT_MSEC);
        pConfChkCmd->setTimeOut("0",ACT_TIMEOUT_UNIT_MSEC);

        QList<GTASCXMLTransition> externalTxList;
        GTASCXMLTransition timeoutTx;

        if(_isGenericSCXMLCofig)
        {
            getTimeoutTranstion(iChkValue,timeoutEventName,getParamFinalStateId,timeoutTx);
        }
        else
        {
            QString confDummyTranstionStateEventName = QString("%1_transtion_event").arg(checkValueState2Id);
            getTimeoutTranstion(iChkValue,confDummyTranstionStateEventName,getParamFinalStateId,timeoutTx,timeoutCondTrue);
            ConfTimeEventName = QString(confCondTrue);

        }
        externalTxList<<timeoutTx;
        createSCXMLCheckValue(pConfChkCmd, checkValueState , checkValueState2Id, chkValFinalStateId, externalTxList,ConfTimeEventName);
    }
    else
    {
        GTASCXMLTransition chkVal1Tx;
        chkVal1Tx.setTarget(chkValFinalStateId);
        checkValueState1.insertTransitions(chkVal1Tx);
        checkValueState.insertStates(checkValueState1);
    }



    GTASCXMLFinal chkValStateFinal;
    chkValStateFinal.setId(chkValFinalStateId);
    checkValueState.setFinal(chkValStateFinal);


    GTASCXMLTransition chkValStateTx;
    chkValStateTx.setTarget(iTargetId);
    checkValueState.insertTransitions(chkValStateTx);

    getStateForDumpListParameters(iChkValue,checkValueState,dumpListStateId,chkValFinalStateId);

    logAllParametersOnExit(iChkValue,iChkValue->getVariableList(),checkValueState);
    iState.insertStates(checkValueState);

}
void GTASCXMLController::createSCXMLOneClickPPC(const GTAActionOneClickPPC *iPpcCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(iPpcCmd == NULL)
        return;
    QString cmdName = QString("%1 %2").arg(iPpcCmd->getAction(),iPpcCmd->getComplement()).trimmed();
    QString ppcPath = iPpcCmd->getPPCPath();
    QString invokeId = iStateId;
    bool isStart = iPpcCmd->getIsStart();
    QString retName = QString("_%1_return").arg(iStateId);
    cmdName.replace(" ","_");

    GTASCXMLInvoke oneClickPPCInvoke;
    bool rc = getCloneInvoke(cmdName+".scxml",oneClickPPCInvoke);

    if(rc)
    {
        GTASCXMLXMLRPC func = oneClickPPCInvoke.getXMLRPCFunction();
        GTASCXMLXMLRPC::funcArg args;
        func.getArgumentByName("args_list",args);
        if(isStart)
            args.value = QString("['%1']").arg(ppcPath);
        else
            args.value = QString("[]");

        func.updateArgumentByName("args_lsit",args);

        GTASCXMLXMLRPC::funcReturn ret;
        func.getReturnArgument(ret);
        ret.name = retName;
        func.updateReturnArgument(ret);

        oneClickPPCInvoke.setInvokeFunction(func);

        invokeId  = invokeId.replace("s","i");

        oneClickPPCInvoke.setInvokeId(invokeId);

        GTASCXMLAssign assign;
        assign.setExpression(QString("_event.data.%1").arg(retName));
        assign.setLocation(retName);

        GTASCXMLState oneClickState;
        oneClickState.insertInvokes(oneClickPPCInvoke);

        oneClickState.setId(iStateId);

        insertSuccessTrasition(iPpcCmd,QStringList(),iTargetId,"done.invoke._all",oneClickState,QString("%1 == 'OK'").arg(retName));
        insertFailTrasition(iPpcCmd,QStringList(),iTargetId,"done.invoke._all",oneClickState,QString("%1 == 'OK'").arg(retName));

        iState.insertStates(oneClickState);

    }

}
void GTASCXMLController::createSCXMLOneClickStartEnv(const GTAOneClick *iOneClickEnvCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    QString configName = iOneClickEnvCmd->getConfigName();
    QString testRigName = iOneClickEnvCmd->getTestRigName();
    QString testRigType = iOneClickEnvCmd->getTestRigType();
    QString appInfo = iOneClickEnvCmd->getApplicationName();
    QStringList options = iOneClickEnvCmd->getOptionNames();
    QString cmdName = QString("%1 %2").arg(iOneClickEnvCmd->getAction(),iOneClickEnvCmd->getComplement());
    cmdName.replace(" ","_");
    QString version = "";
    if(appInfo.contains(":"))
    {
        QStringList items = appInfo.trimmed().split(":");
        if(items.count()>1)
        {
            version = items.at(1);
            version = version.trimmed();
        }
    }
    if(version == "nil")
    {
        version = "";
    }

    QStringList optionList;
    foreach(QString option,options)
    {
        QStringList optionVal = iOneClickEnvCmd->getValueForOption(option);
        QString optionType = iOneClickEnvCmd->getTypeofOption(option);
        QStringList dirNames = iOneClickEnvCmd->getSelectedDirNames(option);
        if(optionType == "selectfile")
            optionVal = dirNames;
        if(!optionVal.isEmpty() && !optionVal.at(0).isEmpty())
        {
            if(optionType.toLower() == "yesno")
            {
                if(optionVal.at(0).toLower() == "yes")
                {
                    optionList.append("'"+option+"'"+":"+"true");
                }
                else
                {
                    optionList.append("'"+option+"'"+":"+"false");
                }
            }
            else
            {
                if(optionType == "freetext")
                {
                    optionList.append("'"+option+"'"+":'"+optionVal.at(0)+"'");
                }
                else if(optionType.toLower() == "selectfile")
                {
                    optionList.append("'"+option+"'"+":'"+optionVal.join(";")+"'");
                }
                else
                {
                    optionList.append("'"+option+"'"+":'"+optionVal.at(0)+"'");
                }

            }
        }
    }
    QString optionStr = optionList.join(",");

    QString jsonStruct = QString("{'ConfigurationName':'%1', 'version':'%2', 'options' :{%3}}").arg(configName,version,optionStr);

    QString configVarName = QString("%1_%2").arg(iStateId,configName);


    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");

    QString retName = QString("_%1_return").arg(iStateId);



    GTASCXMLState oneClickState;
    oneClickState.setId(iStateId);


    GTASCXMLAssign assign;
    assign.setExpression(jsonStruct);
    assign.setLocation(configVarName);

    GTASCXMLOnEntry onEntry;
    onEntry.insertAssign(assign);
    oneClickState.setOnEntry(onEntry);


    GTASCXMLOnExit onExit;

    assign.setExpression("[]");
    onExit.insertAssign(assign);
    oneClickState.setOnExit(onExit);

    GTASCXMLInvoke invoke;
    invoke.setInvokeId(invokeId);

    getCloneInvoke(cmdName+"_Start_Env.scxml",invoke);

    GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();
    GTASCXMLXMLRPC::funcArg args;
    func.getArgumentByName("ConfigurationName",args);
    args.value = configVarName;
    func.updateArgumentByName("ConfigurationName",args);
    GTASCXMLXMLRPC::funcReturn ret;
    func.getReturnArgument(ret);

    ret.name = retName;
    func.updateReturnArgument(ret);

    func.setToolId(testRigName);
    func.setToolType(testRigType);


    invoke.setInvokeFunction(func);






    GTASCXMLAssign assign1;
    assign1.setExpression(QString("_event.data.%1").arg(retName));
    assign1.setLocation(retName+".LaunchStatus");
    invoke.insertAssign(assign1);
    oneClickState.insertInvokes(invoke);

    insertSuccessTrasition(iOneClickEnvCmd,QStringList(),iTargetId,"done.invoke._all",oneClickState,QString("%1.LaunchStatus == 'OK'").arg(retName));
    insertFailTrasition(iOneClickEnvCmd,QStringList(),iTargetId,"done.invoke._all",oneClickState,QString("%1.LaunchStatus == 'OK'").arg(retName));

    iState.insertStates(oneClickState);

}
void GTASCXMLController::createSCXMLOneClickStopTestConfig(const GTAOneClick *iOneClickEnvCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    QString cmdName = QString("%1 %2").arg(iOneClickEnvCmd->getAction(),iOneClickEnvCmd->getComplement());
    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");
    cmdName.replace(" ","_");
    QString retName = QString("_%1_return").arg(iStateId);



    GTASCXMLState oneClickState;
    oneClickState.setId(iStateId);
    GTASCXMLInvoke invoke;
    invoke.setInvokeId(invokeId);

    getCloneInvoke(cmdName+"_Stop_TestConfig.scxml",invoke);

    GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();


    GTASCXMLXMLRPC::funcReturn ret;
    func.getReturnArgument(ret);

    ret.name = retName;
    func.updateReturnArgument(ret);


    invoke.setInvokeFunction(func);



    GTASCXMLAssign assign1;
    assign1.setExpression(QString("_event.data.%1").arg(retName));
    assign1.setLocation(QString("%1.LaunchStatus").arg(retName));
    invoke.insertAssign(assign1);

    oneClickState.insertInvokes(invoke);

    insertSuccessTrasition(iOneClickEnvCmd,QStringList(),iTargetId,"done.invoke._all",oneClickState,QString("%1.LaunchStatus == 'OK'").arg(retName));
    insertFailTrasition(iOneClickEnvCmd,QStringList(),iTargetId,"done.invoke._all",oneClickState,QString("%1.LaunchStatus == 'OK'").arg(retName));

    iState.insertStates(oneClickState);

}
void GTASCXMLController::createSCXMLOneClickStopAlto(const GTAOneClick *iOneClickEnvCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    QString configName = iOneClickEnvCmd->getConfigName();
    QString testRigName = iOneClickEnvCmd->getTestRigName();
    QString testRigType = iOneClickEnvCmd->getTestRigType();
    QString cmdName = QString("%1 %2").arg(iOneClickEnvCmd->getAction(),iOneClickEnvCmd->getComplement());
    cmdName.replace(" ","_");
    QString invokeId = iStateId;
    invokeId = invokeId.replace("s","i");

    QString retName = QString("_%1_return").arg(iStateId);



    GTASCXMLState oneClickState;
    oneClickState.setId(iStateId);

    GTASCXMLInvoke invoke;
    invoke.setInvokeId(invokeId);
    getCloneInvoke(cmdName+"_Stop_Alto.scxml",invoke);

    GTASCXMLXMLRPC func = invoke.getXMLRPCFunction();

    GTASCXMLXMLRPC::funcArg args;

    func.getArgumentByName("ConfigurationName",args);

    args.value = "ATM";
    cmdParameterList.append("ATM");
    func.updateArgumentByName("ConfigurationName",args);

    GTASCXMLXMLRPC::funcReturn ret;
    func.getReturnArgument(ret);

    ret.name = retName;
    func.updateReturnArgument(ret);

    func.setToolId(testRigName);
    func.setToolType(testRigType);


    invoke.setInvokeFunction(func);


    GTASCXMLAssign assign1;
    assign1.setExpression(QString("_event.data.%1").arg(retName));
    assign1.setLocation("ATM.LaunchStatus");
    invoke.insertAssign(assign1);

    oneClickState.insertInvokes(invoke);

    insertSuccessTrasition(iOneClickEnvCmd,QStringList(),iTargetId,"done.invoke._all",oneClickState,QString("%1.LaunchStatus == 'OK'").arg("ATM"));
    insertFailTrasition(iOneClickEnvCmd,QStringList(),iTargetId,"done.invoke._all",oneClickState,QString("%1.LaunchStatus == 'OK'").arg("ATM"));

    iState.insertStates(oneClickState);

}
void GTASCXMLController::createSCXMLOneClickEnv(const GTAOneClick *iOneClickEnvCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    bool isStartEnv = iOneClickEnvCmd->getIsStartEnv();
    bool isStopAlto = iOneClickEnvCmd->getIsStopAlto();
    bool isStopTestConfig = iOneClickEnvCmd->getIsStopTestConfig();

    GTASCXMLState oneClickState;
    oneClickState.setId(iStateId);

    QString startEnvStateId = QString("%1_StartEnv").arg(iStateId);
    QString stopAltoStateId = QString("%1_StopAlto").arg(iStateId);
    QString stopTestConfigStateId = QString("%1_StopTestConfig").arg(iStateId);
    QString finalStateId = QString("%1_final").arg(iStateId);


    QString targetId = finalStateId;
    if(isStartEnv)
    {
        if(isStopAlto)
            targetId = stopAltoStateId;
        else if(isStopTestConfig)
            targetId = stopTestConfigStateId;

        createSCXMLOneClickStartEnv(iOneClickEnvCmd,oneClickState,startEnvStateId,targetId,QStringList());
    }
    if(isStopAlto)
    {
        if(isStopTestConfig)
            targetId = stopTestConfigStateId;

        createSCXMLOneClickStopAlto(iOneClickEnvCmd,oneClickState,stopAltoStateId,targetId,QStringList());
    }
    if(isStopTestConfig)
    {

        createSCXMLOneClickStopTestConfig(iOneClickEnvCmd,oneClickState,stopTestConfigStateId,iTargetId,QStringList());
    }

    GTASCXMLFinal final;
    final.setId(finalStateId);
    oneClickState.setFinal(final);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    oneClickState.insertTransitions(tx);

    iState.insertStates(oneClickState);
}
void GTASCXMLController::createSCXMLOneClickLaunchApp(const GTAOneClickLaunchApplication *,  GTASCXMLState &, const QString &, const QString &, const QStringList &)
{

}
void GTASCXMLController::createSCXMLOneClickApplication(const GTAOneClickLaunchApplication *iOneClickAppCmd,  GTASCXMLState &, const QString &iStateId, const QString &, const QStringList &)
{
    
    QList<GTAOneClickLaunchApplicationItemStruct *>itemStructList = iOneClickAppCmd->getLaStruct();

    GTASCXMLState oneClickAppState;
    QString finalStateId = QString("%1_final").arg(iStateId);
    for(int i=0;i<itemStructList.count();i++)
    {
        QString stateId = QString("%1_OneClickApp_%2").arg(iStateId,QString::number(i));
        QString targetId = QString("%1_OneClickApp_%2").arg(iStateId,QString::number(i+ 1));

        if(i>= itemStructList.count() - 1)
        {
            targetId = finalStateId;
        }


        GTAOneClickLaunchApplicationItemStruct * itemStruct = itemStructList.at(i);
        bool isKillApp = itemStruct->getKillMode();
        if(isKillApp)
        {

        }
        else
        {

        }
    }

}
void GTASCXMLController::createSCXMLParallelCall(const GTAActionCallProcedures *pParallelCallCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{

    if(pParallelCallCmd == NULL)
        return;

    QList<GTACallProcItem> callItems = pParallelCallCmd->getCallProcedures();
    bool isParallelCall = pParallelCallCmd->getPrallelExecution();
    GTAAppController* pController = GTAAppController::getGTAAppController();
    QStringList varList;

    GTASCXMLState parallelCallState;
    parallelCallState.setId(iStateId);

    int callCnt = callItems.count();

    QString finalStateId = QString("%1_final").arg(iStateId);

    for(int i=0;i<callCnt;i++)
    {
        GTACallProcItem callItem = callItems.at(i);
        GTACallProcItem::call_type type = callItem._type;
        QMap<QString,QString> tagValue = callItem._tagValue;

        bool isUUID = true;
        QString elemName = callItem._UUID;
        if(elemName.isEmpty())
        {
            elemName = callItem._elementName;
            isUUID = false;
        }
        GTAElement *pCallElem = NULL;
        GTAElement *pCallElemGrp = NULL;
        bool rc = pController->getElementFromDocument(elemName,pCallElem,isUUID);
        if((rc) && (pCallElem != NULL))
        {
            if(type == GTACallProcItem::FUNCTION)
                pCallElem->replaceTag(tagValue);

            GroupElementByTitleComplete(pCallElem,varList,pCallElemGrp);
            if(pCallElemGrp != NULL)
            {
                GTASCXMLState callState;
                QString stateID = QString("%1_call_%2").arg(iStateId,QString::number(i));

                QString finalCallStateId = QString("%1_call_%2_final").arg(iStateId,QString::number(i));

                if(!isParallelCall)
                {
                    QString targetId = QString("%1_call_%2").arg(iStateId,QString::number(i+1));
                    if(i >= callCnt - 1)
                    {
                        targetId = finalStateId;
                    }

                    GTASCXMLTransition callTx;
                    callTx.setTarget(targetId);

                }

                callState.setId(stateID);
                GTASCXMLFinal callFinal;
                callFinal.setId(finalCallStateId);
                callState.setFinal(callFinal);


                getStateChart(pCallElemGrp,callState);

                parallelCallState.insertStates(callState);
            }
        }


    }


    GTASCXMLFinal parallelCallFinal;
    parallelCallFinal.setId(finalStateId);

    GTASCXMLTransition parallelCallTx;
    parallelCallTx.setTarget(iTargetId);
    parallelCallState.insertTransitions(parallelCallTx);

    if(isParallelCall)
        parallelCallState.setType(GTASCXMLState::PARALLEL);

    iState.insertStates(parallelCallState);
}
void GTASCXMLController::createSCXMLGenericTool(const GTAGenericToolCommand *pGenToolCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId, const QStringList &)
{
    if(pGenToolCmd == NULL)
        return;
    QList<GTAGenericFunction> funcList = pGenToolCmd->getCommand();
    double dTimeout = pGenToolCmd->getTimeOutInMs();

    GTASCXMLState genericToolState;
    genericToolState.setId(iStateId);

    QString finalId = QString("%1_final").arg(iStateId);

    GTASCXMLFinal final;
    final.setId(finalId);

    genericToolState.setFinal(final);

    QString toolName = pGenToolCmd->getToolName();

//    QString cmdPath = pGenToolCmd->getCommandPath();
//    cmdPath.replace("call function - ","");
//    cmdPath.replace("(","");
//    cmdPath.replace(")","");
//    QStringList items = cmdPath.split("#");

//    if(items.count()>0)
//        items.removeAt(items.count() - 1);
//    cmdPath = items.join("#");
//    cmdPath.replace("/","#");
    QString cmdPath = pGenToolCmd->getObjId();

    QString timeoutEventName = QString("%1_timeout_event").arg(iStateId);

    GTASCXMLOnEntry onEntry;
    if(dTimeout>0)
    {
        GTASCXMLSend timeoutSend;
        timeoutSend.setEvent(timeoutEventName);
        timeoutSend.setDelay(QString::number(dTimeout)+"ms");
        onEntry.insertSend(timeoutSend);
    }

    genericToolState.setOnEntry(onEntry);

    for(int i=0;i<funcList.count();i++)
    {
        GTAGenericFunction func = funcList.at(i);
        QString funcStateId = QString("%1_genFunc_%2").arg(iStateId,QString::number(i));
        QString funcTargetId = QString("%1_genFunc_%2").arg(iStateId,QString::number(i+1));
        QString funcInvokeId = QString("%1_genFunc_%2").arg(iStateId,QString::number(i));
        funcInvokeId = funcInvokeId.replace("s","i");

        if(i >= funcList.count() - 1)
        {
            funcTargetId = finalId;
        }
        GTASCXMLXMLRPC xmlFunc;


        //QList<GTAGenericArgument> args = func.getFunctionArguments();
        QList<GTAGenericArgument> args = func.getAllArguments();
        QString funcName = func.getFunctionName();
        QString funcRetNameOrg = func.getReturnParamName();
        QString toolId = func.getToolID();
        QString toolType = func.getToolType();
        bool isWaitUntilTrue = func.getWaitUntilStatus();

        QString funcReturnType = func.getReturnParamType();
        QString location = QString("%1:%2:%3:%4$%5").arg("GEN_TOOL",toolName.trimmed(),funcName.trimmed(),cmdPath.trimmed(),funcRetNameOrg.trimmed());
        location.replace(" ","_");
        QString funcRetName = QString("%1_%2").arg(funcStateId, funcRetNameOrg);
        QString returnCond = "";
        if(!_genToolReturnMap.contains(location))
            _genToolReturnMap.insert(location,funcRetName);

        QString trueCond;
        QString falseCond;
        bool isTrueCond = pGenToolCmd->getSCXMLConditionStatement(funcRetName,funcRetNameOrg,true,trueCond,QString());
        bool isFalseCond = pGenToolCmd->getSCXMLConditionStatement(funcRetName,funcRetNameOrg,false,falseCond,QString());

        xmlFunc.setFunctionName(funcName);
        xmlFunc.setToolId(toolId);
        xmlFunc.setToolType(toolType);


        //ask to make func ret as list instead of one argument.
        GTASCXMLXMLRPC::funcReturn funcRet;

        funcRet.name = funcRetName;
        funcRet.type = funcReturnType;

        xmlFunc.insertFunctionReturn(funcRet.name,funcRet.type);

        for(int j=0;j<args.count();j++)
        {
            GTASCXMLXMLRPC::funcArg funcArg;
            funcArg.name = args[j].getArgumentName();
            funcArg.type = args[j].getArgumentType();
            funcArg.value = args[j].getUserSelectedValue();
            xmlFunc.insertFunctionArgument(funcArg.type, funcArg.name,funcArg.value);
        }

        GTASCXMLInvoke funcInvoke;
        funcInvoke.setInvokeId(funcInvokeId);
        funcInvoke.setInvokeFunction(xmlFunc);
        funcInvoke.setInvokeType("xml-rpc");

        GTASCXMLAssign returnAssing;
        returnAssing.setExpression(QString("_event.data.%1").arg(funcRetName));
        returnAssing.setLocation(funcRetName);

        funcInvoke.insertAssign(returnAssing);

        GTASCXMLState funcState;
        funcState.setId(funcStateId);
        funcState.insertInvokes(funcInvoke);

        //        GTASCXMLTransition funcTx;
        //        funcTx.setTarget(funcTargetId);
        //        funcTx.setEvent("done.invoke._all");
        //        QString condition = "";//func.getSCXMLCondition(funcRetName); --> need to add in genericfunc class.
        //        funcTx.setCondition(QString("%1").arg(condition));

        //funcState.insertTransitions(funcTx);

        if(isTrueCond)
            insertSuccessTrasition(pGenToolCmd,QStringList(),funcTargetId,"done.invoke._all",funcState,trueCond);
        else
            insertSuccessTrasition(pGenToolCmd,QStringList(),funcTargetId,"done.invoke._all",funcState,QString("%1 == 'OK'").arg(trueCond));

        if(isWaitUntilTrue)
        {
            if(isFalseCond)
                insertFailTrasition(pGenToolCmd,QStringList(),funcStateId,"done.invoke._all",funcState,falseCond);
            else
            {
                insertFailTrasition(pGenToolCmd,QStringList(),funcStateId,"done.invoke._all",funcState,QString("%1 != 'OK'").arg(falseCond));
            }
        }
        else
        {
            if(isFalseCond)
                insertFailTrasition(pGenToolCmd,QStringList(),funcTargetId,"done.invoke._all",funcState,falseCond);
            else
            {
                insertFailTrasition(pGenToolCmd,QStringList(),funcTargetId,"done.invoke._all",funcState,QString("%1 != 'OK'").arg(falseCond));
            }
        }
        if(dTimeout>0)
            insertTimeoutTransition(pGenToolCmd,timeoutEventName,funcTargetId,funcState);
        genericToolState.insertStates(funcState);
    }
    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    genericToolState.insertTransitions(tx);

    iState.insertStates(genericToolState);
}
QString GTASCXMLController::getParameterExpression(const QString &) const
{
    return QString();
}
void GTASCXMLController::createSCXMLDataModel(GTASCXMLSCXML &iSCXMLNode)
{
    QStringList scxmlVarList;
    scxmlVarList.append(cmdParameterList);
    scxmlVarList.append(iSCXMLNode.getVariableList());
    scxmlVarList.removeDuplicates();

    GTASCXMLDataModel dataModel;

    QList<GTASCXLMParamInfo> _initData;
    QList<GTASCXMLData> _paramData;
    QList<GTASCXMLData> _localVars;

    for(int i=0;i<scxmlVarList.count();i++)
    {
        QString id =scxmlVarList.at(i);
        if(_icdParamList.contains(id))
        {
            GTAICDParameter param = _icdParamList.value(id);
            QString appName = param.getApplicationName();
            QString busName = param.getBusName();
            QString media = param.getMedia();
            QString paramType = param.getValueType();
            QString paramUnit = param.getUnit();
            QString paramdefValue = param.getMinValue();

            if(id.contains("-"))
                id.replace("-","_");

            QString jsonexprParamIdData = "";

            if(_isGenericSCXMLCofig)
                jsonexprParamIdData = QString("{\"name\":\"%1\",\"unit\":\"%2\",\"value\":\"%3\"}").arg(id,paramUnit,paramdefValue);
            else
                jsonexprParamIdData = QString("{'ParameterType':'%1'}").arg(paramType);

            GTASCXMLData data;
            data.setDataId(id);
            data.setDataExpr(jsonexprParamIdData);
            _paramData.append(data);

            GTASCXLMParamInfo paramInfo;


            if(ExternalSubscribeParams.contains(id))
            {
                paramInfo.insertParamInfo(appName,busName,media,false);
            }
            else
                paramInfo.insertParamInfo(appName,busName,media,true);
            if(_initData.contains(paramInfo))
            {

                int idx = _initData.indexOf(paramInfo);
                if(idx >-1)
                {
                    GTASCXLMParamInfo prevParamInfo= _initData.at(idx);
                    if(ExternalSubscribeParams.contains(id))
                    {
                        prevParamInfo.insertParamInfo(appName,busName,media,false);
                    }
                    else
                        prevParamInfo.insertParamInfo(appName,busName,media,true);

                    
                    _initData[idx] = prevParamInfo;
                }
            }
            else
            {

                _initData.append(paramInfo);
            }

        }
        else
        {
            GTASCXMLData data;
            updateWithGenToolParam(id);
            if(_genToolParams.contains(id))
                id =_genToolParams.value(id);
            data.setDataId(id);
            if(id.contains("_MultiSetList"))
                data.setDataExpr("[]");
            else
                data.setDataExpr("");
            _localVars.append(data);
        }
    }
    if(!_isGenericSCXMLCofig)
    {
        for(int i=0;i<_initData.count();i++)
        {
            GTASCXLMParamInfo paramInfo = _initData.at(i);
            QString id = paramInfo.getId();
            QString expr = paramInfo.getJsonExpr();

            if(id.contains("-"))
                id.replace("-","_");

            GTASCXMLData data;
            data.setDataId(id);
            data.setDataExpr(expr);
            dataModel.insertDataItem(data);
            if(paramInfo.getIsInitNeeded())
            {
                _subscribeParamList.append(id);
            }
        }
    }

    for(int i=0;i<_paramData.count();i++)
    {
        dataModel.insertDataItem(_paramData[i]);
    }
    for(int i=0;i<_localVars.count();i++)
    {

        dataModel.insertDataItem(_localVars[i]);
    }

    iSCXMLNode.setDataModel(dataModel);
}
void GTASCXMLController::getInitStateForApplications(GTASCXMLState &oState, const QString &iStateId, const QString &iTarget)
{
    GTASCXMLInvoke Invoke;
    bool rc = getCloneInvoke("subscribe.scxml",Invoke);
    oState.setId(iStateId);
    if(rc)
    {
        if(!_subscribeParamList.isEmpty())
        {
            QStringList retNames;
            for(int i=0;i<_subscribeParamList.count();i++)
            {
                GTASCXMLInvoke subInvoke = Invoke;
                QString invokeId = QString("%1_invoke_%2").arg(iStateId,QString::number(i));
                QString funcRet = QString("_%1_return_%2").arg(iStateId,QString::number(i));
                GTASCXMLXMLRPC func;

                func = subInvoke.getXMLRPCFunction();

                GTASCXMLXMLRPC::funcArg arg;
                func.getArgumentByName("ParameterList",arg);
                arg.value = _subscribeParamList.at(i);

                func.updateArgumentByName("ParameterList",arg);

                GTASCXMLXMLRPC::funcReturn ret;
                ret.name = funcRet;
                func.updateReturnArgument(ret);
                subInvoke.setInvokeId(invokeId);
                subInvoke.setInvokeFunction(func);

                GTASCXMLAssign assign;
                assign.setExpression(QString("_event.data.%1").arg(funcRet));
                assign.setLocation(funcRet);

                subInvoke.insertAssign(assign);

                oState.insertInvokes(subInvoke);
                retNames.append(funcRet);
            }

            GTASCXMLOnExit onExit;
            QStringList successCondList;
            QStringList failCondList;
            for(int i=0;i<retNames.count();i++)
            {
                QString cond1 = QString("(%1.Status == 'OK')").arg(retNames.at(i));
                successCondList.append(cond1);
                QString cond2 = QString("(%1.Status != 'OK')").arg(retNames.at(i));
                failCondList.append(cond2);

                GTASCXMLLog log;

                QString logExpr = "'Following signals were not subscribed:' +"+retNames.at(i);
                log.setExpression(logExpr);
                log.setLabel("error");

                onExit.insertLog(log);

            }
            GTASCXMLTransition successTx;
            GTASCXMLTransition failTx;

            successTx.setStatus("success");
            failTx.setStatus("fail");

            successTx.setTarget(iTarget);
            failTx.setTarget(SCXML_FINAL_STATE_ID);

            successTx.setEvent("done.invoke._all");
            failTx.setEvent("done.invoke._all");

            successTx.setCondition(QString("(%1)").arg(successCondList.join(" && ")));
            failTx.setCondition(QString("(%1)").arg(failCondList.join(" && ")));

            oState.insertTransitions(successTx);
            oState.insertTransitions(failTx);
            oState.setOnExit(onExit);
        }
        else
        {
            GTASCXMLTransition tx;
            tx.setTarget(iTarget);
            oState.insertTransitions(tx);
        }
    }
    else
    {
        GTASCXMLTransition tx;
        tx.setTarget(iTarget);
        oState.insertTransitions(tx);
    }
}
void GTASCXMLController::getUnInitState(GTASCXMLState &oState, const QString &iStateId, const QString &iTarget)
{
    GTASCXMLInvoke unsubInvoke;
    bool rc = getCloneInvoke("unsubscribe.scxml",unsubInvoke);
    oState.setId(iStateId);
    if(rc)
    {
        QString invokeId = QString("%1_invoke").arg(iStateId);
        QString funcRet = QString("_%1_return").arg(iStateId);

        GTASCXMLXMLRPC func = unsubInvoke.getXMLRPCFunction();

        GTASCXMLXMLRPC::funcReturn ret;
        ret.name = funcRet;
        func.updateReturnArgument(ret);

        unsubInvoke.setInvokeId(invokeId);
        unsubInvoke.setInvokeFunction(func);

        GTASCXMLAssign assign;
        assign.setExpression(QString("_event.data.%1").arg(funcRet));
        assign.setLocation(funcRet);

        unsubInvoke.insertAssign(assign);


        oState.insertInvokes(unsubInvoke);

        GTASCXMLTransition successTx;
        successTx.setEvent("done.invoke._all");
        successTx.setStatus("success");
        successTx.setTarget(iTarget);
        successTx.setCondition(QString("%1.Status == 'OK'").arg(funcRet));

        GTASCXMLTransition failTx;
        failTx.setEvent("done.invoke._all");
        failTx.setStatus("fail");
        failTx.setTarget(iTarget);
        failTx.setCondition(QString("%1.Status != 'OK'").arg(funcRet));

        oState.insertTransitions(successTx);
        oState.insertTransitions(failTx);

    }
    else
    {
        GTASCXMLTransition tx;
        tx.setTarget(iTarget);
        oState.insertTransitions(tx);
    }

}
void GTASCXMLController::updateWithGenToolParam(QString &ioParam)
{
    QStringList iTokens = ioParam.split("$");
    QStringList keys = _genToolReturnMap.keys();
    for(int i=0;i<keys.count();i++)
    {
        QString key = keys.at(i);

        QStringList tokens = key.split("$");
        QString loc = tokens.at(0);
        loc.replace("[","");
        loc.replace("]","");
        if(loc == iTokens.at(0))
        {
            if(iTokens.at(1).contains(tokens.at(1)))
            {
                QString returnParam = _genToolReturnMap.value(key);

                ioParam = iTokens.at(1);
                ioParam = ioParam.replace(tokens.at(1),returnParam);
                _genToolParams.insert(ioParam,returnParam);
                break;
            }
        }
    }
}
void GTASCXMLController::getStateForDumpListParameters(const GTACommandBase *pCmd,GTASCXMLState &iState, const QString iStateId, const QString iTargetId)
{
    GTASCXMLState dumpListState;
    dumpListState.setId(iStateId);

    QString actOnFail = "";
    if(pCmd->getCommandType() == GTACommandBase::ACTION)
    {
        GTAActionBase *pActCmd = (GTAActionBase *)(pCmd);
        if(pActCmd != NULL)
            actOnFail = pActCmd->getActionOnFail();
    }
    else if(pCmd->getCommandType() == GTACommandBase::CHECK)
    {
        GTACheckBase *pChkCmd = (GTACheckBase *)(pCmd);
        if(pChkCmd != NULL)
            actOnFail = pChkCmd->getActionOnFail();
    }
    if(actOnFail.contains("stop"))
    {
        QStringList iDumpParamList = pCmd->getVariableList();

        QString finalStateId = QString("%1_final").arg(iStateId);
        int subStateIdCntr = 0;
        for(int i=0;i<iDumpParamList.count();i++)
        {
            QString param = iDumpParamList.at(i);
            if(_icdParamList.contains(param))
            {

                QString dumpParamStateId = QString("%1_dumpParam_%2").arg(iStateId,QString::number(subStateIdCntr));
                QString dumpParamTargetId = QString("%1_dumpParam_%2").arg(iStateId,QString::number(subStateIdCntr +1));
                if(i >= iDumpParamList.count() - 1)
                {
                    dumpParamTargetId = finalStateId;
                }
                GTASCXMLState paramState;
                bool rc = getStateForParam(paramState,dumpParamStateId,dumpParamTargetId,param,"getParamValueSingleSample");
                if(rc)
                {
                    dumpListState.insertStates(paramState);
                }
                subStateIdCntr++;
            }
        }
        GTASCXMLFinal final;
        final.setId(finalStateId);
        dumpListState.setFinal(final);
        logAllParametersOnExit(pCmd,iDumpParamList,dumpListState);
    }

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    dumpListState.insertTransitions(tx);
    iState.insertStates(dumpListState);
}


