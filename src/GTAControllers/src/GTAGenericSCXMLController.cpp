#include "GTAGenericSCXMLController.h"
#include "GTAScxmlLogMessage.h"

#pragma warning(push, 0)
#include <QTextStream>
#include <QFileInfo>
#pragma warning(pop)

GTAGenericSCXMLController::GTAGenericSCXMLController(const QString &iTemplatePath, bool iGenInvokeForLocal)
{
    _pElement = NULL;
    _EnGenerateInvokeForLocalVar = false;
    _ScxmlTemplatePath = iTemplatePath;
    _isGenericSCXMLCofig = true;
    _pAppCtrl = GTAAppController::getGTAAppController();
    _EnGenerateInvokeForLocalVar = iGenInvokeForLocal;
}
QString GTAGenericSCXMLController::getLastError()
{
    return _LastError;
}
void GTAGenericSCXMLController::setElementName(GTAElement* pElement,QString iSaveFilePath)
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

void GTAGenericSCXMLController::GetParamList( QStringList &iVariableList)
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
            DataCtrl.getParametersDetail(items,_pAppCtrl->getGTADbFilePath(),_dbParamList,InvalidParamList);
        }
    }
}

bool GTAGenericSCXMLController::exportToScxml(const GTAElement * ipElement,const QString &iFilePath)
{
    bool rc = false;
    if(ipElement == NULL)
    {
        _LastError = "Error: Element is NULL";
        return rc;
    }
    QStringList variableList;
    _pElement = (GTAElement *)ipElement;
    setElementName(_pElement,iFilePath);
    _pElement->updateCommandInstanceName(QString());

    GTAElement* pTitleBasedElement = new GTAElement(_pElement->getElementType());
    GroupElementByTitleComplete(_pElement,variableList, pTitleBasedElement);

    GTAHeader *pHdr = _pElement->getHeader();
    if(pHdr != NULL)
    {
        pHdr->setElemName(ipElement->getName());
        _headerLists.append(pHdr);
    }
    pTitleBasedElement->setName(ipElement->getName());

    QString mainStateId  = ipElement->getName();
    QString mainStateFinalId = QString("%1_final").arg(mainStateId);

    GTASCXMLState mainState;

    // update the list of parameters from the database.
    GetParamList(variableList);

    rc = getStateChart(pTitleBasedElement,mainState,false,mainStateFinalId);

    mainState.setId(mainStateId);

    GTASCXMLFinal mainStatefinal;
    mainStatefinal.setId(mainStateFinalId);
    mainState.setFinal(mainStatefinal);

    GTASCXMLTransition mainStateTx;
    mainStateTx.setTarget(SCXML_FINAL_STATE_ID);

    mainState.insertTransitions(mainStateTx);


    GTASCXMLState parallelState;
    parallelState.setId("p0");
    parallelState.insertStates(mainState);
    parallelState.insertTransitions(mainStateTx);
    parallelState.setType(GTASCXMLState::PARALLEL);

    GTASCXMLSCXML scxml;

    scxml.insertState(parallelState);

    //insert Datamodel in scxmlNode
    createSCXMLDataModel(scxml);



    GTASCXMLFinal scxmlFinal;
    scxmlFinal.setId(SCXML_FINAL_STATE_ID);

    scxml.setFinalState(scxmlFinal);

    QDomDocument iDomDoc;
    QDomElement scxmlElem = scxml.getSCXML(iDomDoc);
    iDomDoc.appendChild(scxmlElem);

    QFile outFile(iFilePath);
    outFile.open(QFile::WriteOnly|QFile::Text);
    QTextStream writer(&outFile);
    QString xmlContents = iDomDoc.toString();
    // replacing all standard xml representation of linefeed and carriage return by \n and \r
    xmlContents.replace("&#xa;","");
    //xmlContents.replace("&#xa;","&lt;br>&lt;/br>");
    xmlContents.replace("&#xd;","");
    //xmlContents.replace("")

    writer<<xmlContents;
    outFile.close();

    QFileInfo metaDataInfo(iFilePath);
    GenerateHeaderMetaData(metaDataInfo.absolutePath(),metaDataInfo.fileName());

	return rc;
}

void GTAGenericSCXMLController::GroupElementByTitleComplete(const GTAElement *pElem, QStringList &oVariableList, GTAElement *oGroupedElem)
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
                        GTAHeader *pHdr = NULL;
                        pController->getHeaderFromDocument(callElemName,pHdr);
                        if(pHdr!= NULL)
                        {
                            pHdr->setElemName(pElemOnCall->getName());
                            _headerLists.append(pHdr);
                        }

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
void GTAGenericSCXMLController::createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle *&ipTitleCmd,QStringList &oVariableList)
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
                            //TODO: is pSubTitle needed for anything?
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
                            //TODO: Is pSubTitle needed for anything?
                            GTAActionTitle *pSubTitle = new GTAActionTitle(ACT_TITLE);
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
/**
  This statechart function gnerates state chart for the Elem.
  Elem source could be either editor content or a call command.
  The state Id and Target Id are implemented as per Generic SCXML Spec.
  State/Target Id : <Parent_Id>_s<CurrentCmdIdx>_<CMD_NAME>
  Final Id: <Parent_Id>_final
 **/
bool GTAGenericSCXMLController::getStateChart(const GTAElement *pElem,GTASCXMLState &ioState, const bool, QString tpFinalStateId)
{
    bool rc =false;
    bool isFirst = true;
    int directChildrenCnt = pElem->getDirectChildrenCount();
    QString parentStateId = ioState.getId();
    QString parentStateFinalId ="";
    if(!parentStateId.isEmpty())
    {
        parentStateFinalId = QString("%1_final").arg(parentStateId);
    }
    int stateIdCntr = 0;
    for(int i=0;i<directChildrenCnt;i++)
    {
        GTACommandBase *pCmd = NULL;
        pElem->getDirectChildren(i,pCmd);

        if(pCmd != NULL)
        {

            if(isFirst && (pCmd->getAllChildrenCount() == 0))
            {
                isFirst = false;
                continue;
            }
            QString targetId = "";
            QString stateId  = "";

            if(!parentStateId.isEmpty())
            {
                if(i == (directChildrenCnt - 1))
                {
                    targetId = parentStateFinalId;
                }
                else
                {
                    GTACommandBase *pNextCmd = NULL;

                    pElem->getDirectChildren(i+1,pNextCmd);
                    if(pNextCmd != NULL)
                    {
                        targetId = QString("%1_TS%2").arg(parentStateId,QString::number(stateIdCntr+1));
                    }
                    else
                    {
                        targetId = QString("%1_TS%2").arg(parentStateId,QString::number(stateIdCntr+1));
                    }
                }
                stateId = QString("%1_TS%2").arg(parentStateId,QString::number(stateIdCntr));
            }
            else
            {
                if(i == (directChildrenCnt - 1))
                {
                    targetId = tpFinalStateId;
                }
                else
                {
                    GTACommandBase *pNextCmd = NULL;

                    pElem->getDirectChildren(i+1,pNextCmd);
                    if(pNextCmd != NULL)
                    {
                        targetId = QString("TS%2").arg(QString::number(stateIdCntr+1));
                    }
                    else
                    {
                        targetId = QString("TS%2").arg(QString::number(stateIdCntr+1));
                    }
                }
                stateId = QString("TS%2").arg(QString::number(stateIdCntr));
            }
            stateIdCntr++;

            createStateNodeForCommand(pCmd,ioState,stateId,targetId);
            if (!ioState.getStates().isEmpty())
                rc = true;
            else
                _LastError = QString("Error: Statechart creation failed");
        }
    }
    return rc;
}

/**
  This statechart function gnerates state chart for the Command with Children.
  This Command source could be either a Call command or Condition or Title.
  The state Id and Target Id are implemented as per Generic SCXML Spec.
  State/Target Id : <Parent_Id>_s<CurrentCmdIdx>_<CMD_NAME>
  Final Id: <Parent_Id>_final
 **/
bool GTAGenericSCXMLController::getStateChart(const GTACommandBase *pCmd,GTASCXMLState &ioState)
{
    bool rc =false;
    QList<GTACommandBase *> childrens = pCmd->getChildren();
    QString parentStateId = ioState.getId();
    QString parentStateFinalId = QString("%1_final").arg(parentStateId);

    for(int i=0;i<childrens.count();i++)
    {
        GTACommandBase *pSubCmd = childrens.at(i);
        if(pSubCmd != NULL)
        {
            QString targetId = "";

            if(i == (childrens.count() - 1))
            {
                targetId = parentStateFinalId;
            }
            else
            {
                GTACommandBase *pNextCmd = NULL;

                pNextCmd = childrens.at(i+1);
                if(pNextCmd != NULL)
                {
                    if(pNextCmd->isCompoundState())
                    {
                        QString nextStateid = pNextCmd->getCompounsStateId();
                        if (nextStateid.contains("UserDef_SetList"))
                            targetId = QString("%1_s%2_%3").arg(parentStateId,QString::number(i+1),nextStateid);
                        else
                            targetId = QString("%1_cs%2_%3").arg(parentStateId,QString::number(i+1),nextStateid);
                    }
                    else if(pNextCmd->isTitle())
                    {
                        targetId = QString("%1_TS%2").arg(parentStateId,QString::number(i+1));
                    }
                    else
                        targetId = QString("%1_s%2").arg(parentStateId,QString::number(i+1));
                }
                else
                {
                    targetId = QString("%1_s%2").arg(parentStateId,QString::number(i+1));
                }
            }
            QString stateId = QString("%1_s%2").arg(parentStateId,QString::number(i));
            if(pSubCmd->isTitle())
            {
                stateId = QString("%1_TS%2").arg(parentStateId,QString::number(i));
            }
            createStateNodeForCommand(pSubCmd,ioState,stateId,targetId);
            if (!ioState.getStates().isEmpty())
                rc = true;
            else
                _LastError = QString("Error: Statechart creation failed");
        }
    }
    return rc;
}
/**
  In this function SCXML state is created for each command.
  If the command is not part of the Generic SCXML Spec, a dummy State is created for that command.

  **/
void GTAGenericSCXMLController::createStateNodeForCommand(GTACommandBase *pCmd,GTASCXMLState &ioState, const QString &iStateId, const QString &iTargetId)
{
    if(pCmd != NULL)
    {
        // to define the name for the state id based on the command.
        //setStateIDName(iStateId,pCmd);

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
                    createGenericSCXMLPrintMessage(pPrintCmd,ioState,iStateId,iTargetId);
                }
                else if(com == COM_PRINT_PARAM)
                {
                    GTAActionPrint *pPrintCmd = dynamic_cast<GTAActionPrint *>(pActCmd);
                    createGenericSCXMLPrintParameter(pPrintCmd,ioState,iStateId,iTargetId);
                }
                else if(com == COM_PRINT_TABLE)
                {
                    GTAActionPrintTable * pPrintTableCmd = dynamic_cast<GTAActionPrintTable *>(pActCmd);
                    createGenericSCXMLPrintTable(pPrintTableCmd,ioState,iStateId,iTargetId);
                }
                else if(com == COM_PRINT_TIME)
                {
                    GTAActionPrintTime * pPrintTime = dynamic_cast<GTAActionPrintTime *>(pActCmd);
                    createGenericSCXMLPrintTime(pPrintTime,ioState,iStateId,iTargetId);

                    // To be discussed for this command.
                }
                else
                {

                }
            }
            else if(act == ACT_TITLE)
            {
                GTAActionParentTitle *pTitleCmd = dynamic_cast<GTAActionParentTitle *>(pActCmd);
                createGenericSCXMLTitle(pTitleCmd,ioState,iStateId,iTargetId);
            }
            else if(act == ACT_WAIT)
            {

                if(com == COM_WAIT_FOR)
                {
                    GTAActionWait *pWaitCmd = dynamic_cast<GTAActionWait *>(pActCmd);
                    createGenericSCXMLWaitFor(pWaitCmd,ioState,iStateId,iTargetId);
                }
                else if(com == COM_WAIT_UNTIL)
                {
                    GTAActionWait *pWaitCmd = dynamic_cast<GTAActionWait *>(pActCmd);
                    createGenericSCXMLWaitUntil(pWaitCmd,ioState,iStateId,iTargetId);
                }
                else
                {

                }
            }
            else if(act == ACT_CALL_EXT)
            {
                GTAActionExpandedCall *pCallCmd = dynamic_cast<GTAActionExpandedCall *>(pActCmd);
                createGenericSCXMLCall(pCallCmd,ioState,iStateId,iTargetId);
            }
            else if(act == ACT_CONDITION)
            {
                if(com == COM_CONDITION_IF)
                {
                    GTAActionIF *pIfCmd = dynamic_cast<GTAActionIF *>(pActCmd);
                    createGenericSCXMLConditionIf(pIfCmd,ioState,iStateId,iTargetId);
                }
                else if(com == COM_CONDITION_WHILE)
                {
                    GTAActionWhile *pWhile = dynamic_cast<GTAActionWhile *>(pActCmd);
                    createGenericSCXMLConditionWhile(pWhile,ioState,iStateId,iTargetId);
                }
                else if(com == COM_CONDITION_DO_WHILE)
                {
                    GTAActionDoWhile *pDoWhile = dynamic_cast<GTAActionDoWhile *>(pActCmd);
                    createGenericSCXMLConditionDoWhile(pDoWhile,ioState,iStateId,iTargetId);
                }
                else
                {

                }
            }
            else if(act == ACT_SET)
            {
                GTAActionSetList *pSetList = dynamic_cast<GTAActionSetList*>(pActCmd);
                createGenericSCXMLSet(pSetList,ioState,iStateId,iTargetId);
            }
            else if(act == ACT_MATHS)
            {
                GTAActionMaths *pMaths = dynamic_cast<GTAActionMaths * >(pActCmd);
                crateGenericSCXMLMaths(pMaths,ioState,iStateId,iTargetId);
            }
            else
            {
                //All the commands which are not part of the Generic SCXML a dummy state is created.

            }
        }
        else if(pCmd->getCommandType() == GTACommandBase::CHECK)
        {
            GTACheckBase * pCheckCmd = dynamic_cast<GTACheckBase * >(pCmd);
            GTACheckBase::CheckType chkType = pCheckCmd->getCheckType();
            if(chkType == GTACheckBase::VALUE)
            {
                GTACheckValue *pChkValCmd = dynamic_cast<GTACheckValue * >(pCmd);
                createGenericSCXMLCheckValue(pChkValCmd,ioState,iStateId,iTargetId);
            }
        }
        else
        {

            //All the commands which are not part of the Generic SCXML a dummy state is created.
        }
    }
}


void GTAGenericSCXMLController::createGenericSCXMLTitle(const GTAActionParentTitle *iTitleCmd,GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId)
{
    if(iTitleCmd == NULL)
        return;
    GTASCXMLState titleState;
    titleState.setId(iStateId);
    getStateChart(iTitleCmd,titleState);


    GTASCXMLFinal final;
    GTASCXMLTransition tx;

    QString finalStateId = QString("%1_final").arg(iStateId);
    final.setId(finalStateId);

    GTASCXMLOnEntry finalOnEntry;
    GTASCXMLSend finalSend;
    QString finalEventId = QString("%1_final_event").arg(iStateId);
    finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
    finalOnEntry.insertSend(finalSend);
    final.setOnEntry(finalOnEntry);
    tx.setEvent(finalEventId);

    titleState.setFinal(final);

    tx.setTarget(iTargetId);
    titleState.insertTransitions(tx);

    iState.insertStates(titleState);
}

void GTAGenericSCXMLController::createGenericSCXMLPrintMessage(const GTAActionPrint *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId)
{
    if(iPrintCmd == NULL)
        return;

    QStringList values = iPrintCmd->getValues();
    QString instanceName = iPrintCmd->getInstanceName();
    QString message = "";
    if(!values.isEmpty())
    {
        message = values.first();
        message.replace("\n","<br></br>");
        message.replace("`","_");
    }

    GTASCXMLState printState;
    printState.setId(iStateId);

//    GTASCXMLFinal final;

//    QString finalStateId = QString("%1_final").arg(iStateId);
//    final.setId(finalStateId);

//    printState.setFinal(final);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    printState.insertTransitions(tx);


    GTASCXMLOnEntry onEntry;

    GTAScxmlLogMessage logMessage1;
    logMessage1.CurrentValue = "message";
    logMessage1.ExpectedValue = message;
    logMessage1.Identifier = instanceName; 
    logMessage1.FunctionStatus = "NA";

    logMessage1.Result = GTAScxmlLogMessage::OK;
    logMessage1.Status = GTAScxmlLogMessage::Success;
    logMessage1.LOD = GTAScxmlLogMessage::Main;
    logMessage1.isCurrentValParam = false;
    QString log1 = logMessage1.getLogMessage(_isGenericSCXMLCofig);

    GTASCXMLLog log;
    log.setLabel("'info'");
    log.setExpression(QString("%1").arg(log1));

    onEntry.insertLog(log);
    printState.setOnEntry(onEntry);
    QStringList params = iPrintCmd->getVariableList();
    for(int i=0;i<params.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(params.at(i));
        printState.insertVarList(QStringList()<<p.getSignalName());
    }
    printState.embedLog(true,instanceName);

    iState.insertStates(printState);
}

void GTAGenericSCXMLController::createGenericSCXMLPrintTime(const GTAActionPrintTime *iPrintCmd, GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId)
{
    if(iPrintCmd == NULL)
        return;

    // GTAActionPrintTime::TimeType type = iPrintCmd->getTimeType();
    QString instanceName = iPrintCmd->getInstanceName();


    GTASCXMLState printState;
    printState.setId(iStateId);

//    GTASCXMLFinal final;

//    QString finalStateId = QString("%1_final").arg(iStateId);
//    final.setId(finalStateId);

//    printState.setFinal(final);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    printState.insertTransitions(tx);


    GTASCXMLOnEntry onEntry;

    GTAScxmlLogMessage logMessage1;
    logMessage1.CurrentValue = "_currentTime";
    logMessage1.ExpectedValue = "time";
    logMessage1.Identifier = instanceName;
    logMessage1.FunctionStatus = "NA";
    logMessage1.Result = GTAScxmlLogMessage::OK;
    logMessage1.Status = GTAScxmlLogMessage::Success;
    logMessage1.LOD = GTAScxmlLogMessage::Main;
    logMessage1.isCurrentValParam = true;
    QString log1 = logMessage1.getLogMessage(_isGenericSCXMLCofig);

    GTASCXMLLog log;
    log.setLabel("'info'");
    log.setExpression(QString("%1").arg(log1));

    onEntry.insertLog(log);
    printState.setOnEntry(onEntry);
    QStringList params = iPrintCmd->getVariableList();
    for(int i=0;i<params.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(params.at(i));
        printState.insertVarList(QStringList()<<p.getSignalName());
    }
    printState.embedLog(true,instanceName);

    iState.insertStates(printState);
}

void GTAGenericSCXMLController::createGenericSCXMLWaitFor(const GTAActionWait *iWaitCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId)
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
        QString waitEventName = QString("%1_wait_event").arg(iStateId);

        GTASCXMLState waitState;

        GTASCXMLSend waitEvent;
        waitEvent.setDelay(sWaitDelay);
        waitEvent.setEvent(waitEventName);

        GTASCXMLOnEntry onEntry;
        onEntry.insertSend(waitEvent);
        waitState.setOnEntry(onEntry);

        GTASCXMLTransition tx;
        tx.setTarget(iTargetId);
        tx.setEvent(waitEventName);
        waitState.insertTransitions(tx);

        waitState.setId(iStateId);
        QStringList params = iWaitCmd->getVariableList();
        for(int i=0;i<params.count();i++)
        {
            GTAICDParameter p = _dbParamList.value(params.at(i));
            waitState.insertVarList(QStringList()<<p.getSignalName());
        }
        waitState.embedLog(true,iWaitCmd->getInstanceName());

        iState.insertStates(waitState);
    }
}

GTAGenericSCXMLController::ParamType GTAGenericSCXMLController::getParamType(const QString &iParam)
{
    if(_dbParamList.contains(iParam))
        return GTAGenericSCXMLController::PARAM_DB;
    else
    {
        bool isNum = false;
        iParam.toDouble(&isNum);
        if(isNum)
            return GTAGenericSCXMLController::PARAM_NUM;

        if(iParam.contains("\"") || iParam.contains("\'"))
            return GTAGenericSCXMLController::PARAM_STRING;

        return GTAGenericSCXMLController::PARAM_LOCAL;
    }
    //return GTAGenericSCXMLController::PARAM_UNKNOWN; unreachable code
}

bool GTAGenericSCXMLController::getCloneInvoke(const QString & iTemplateFileName, GTASCXMLInvoke &iInvoke)
{
    QDomDocument oDomDoc;
    bool rc = openScxmlTemplateFile(iTemplateFileName,oDomDoc);
    if(rc && (!oDomDoc.isNull()))
    {
        QDomElement rootElem = oDomDoc.documentElement();
        if(! rootElem.isNull())
        {
            iInvoke.translateXMLToGenericStruct(rootElem);
        }
        else rc = false;
    }
    return rc;
}
bool GTAGenericSCXMLController::openScxmlTemplateFile(const QString & iTemplateFileName, QDomDocument & oDomDoc)
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


bool GTAGenericSCXMLController::getInvokeForParamGeneric(GTASCXMLInvoke &ioInvoke, const QString &iInvokId)
{
    QString templateName ="get_generic.scxml";
    bool rc = getCloneInvoke(templateName,ioInvoke);
    if(rc)
    {
        ioInvoke.setInvokeId(iInvokId);
    }
    return rc;
}
bool GTAGenericSCXMLController::getInvokeForSetParamGeneric(GTASCXMLInvoke &ioInvoke, const QString &iInvokId)
{
    QString templateName ="set_generic.scxml";
    bool rc = getCloneInvoke(templateName,ioInvoke);
    if(rc)
    {
        ioInvoke.setInvokeId(iInvokId);
    }
    return rc;
}
void GTAGenericSCXMLController::insertInvokeForSetParam(const QString &iParam, const QString &iVal, const QString &iFS, const bool &iSetOnlyFS,GTASCXMLState &oState, QStringList &oEventList, bool iSetOnlyValue, int id)
{
    QString invokeCntr = QString("_%1_invokeCntr").arg(oState.getId());
    int invokeIdCntr = 0;
    QString param = iParam;
    QString fs_name = "FS_"+iParam;
    
    

    GTASCXMLInvoke paramInvoke;
    

    if(!iSetOnlyFS)
    {
        QString invokeId = (id == -1)? QString("%1_i%2").arg(oState.getId(),QString::number(invokeIdCntr++)):QString("%1_i%2").arg(oState.getId(),QString::number(id++));
        getInvokeForSetParamGeneric(paramInvoke,invokeId);

        GTASCXMLGenericFunction func = paramInvoke.getGenericFunction();
        func.setFunctionReturn(param);
        func.updateAttributeByName("name",param);
        func.updateAttributeByName("value",iVal);
        paramInvoke.setInvokeFunctionGeneric(func);
        GTASCXMLAssign timeAssign;
        timeAssign.setLocation(QString("%1.time_stamp").arg(param));
        timeAssign.setExpression(QString("_event.data.%1.time_stamp").arg(param));
        GTASCXMLAssign valueAssign;
        valueAssign.setLocation(QString("%1.value").arg(param));
        valueAssign.setExpression(QString("_event.data.%1.value").arg(param));
        paramInvoke.insertAssign(timeAssign);
        paramInvoke.insertAssign(valueAssign);
        GTASCXMLAssign invokeCntrAssign;
        invokeCntrAssign.setExpression(QString("%1.value + 1").arg(invokeCntr));
        invokeCntrAssign.setLocation(QString("%1.value").arg(invokeCntr));
        paramInvoke.insertAssign(invokeCntrAssign);
        oEventList.append(QString("done.invoke.%1").arg(invokeId));
        oState.insertInvokes(paramInvoke);
    }

    if(!iSetOnlyValue)
    {
        QString fsInvokeId = (id == -1)? QString("%1_i%2").arg(oState.getId(),QString::number(invokeIdCntr++)):QString("%1_i%2").arg(oState.getId(),QString::number(id++));
        getInvokeForSetParamGeneric(paramInvoke,fsInvokeId);

        GTASCXMLInvoke fsParamInvoke;
        getInvokeForParamGeneric(fsParamInvoke,fsInvokeId);
        GTASCXMLGenericFunction func1 = paramInvoke.getGenericFunction();
        func1.setFunctionReturn(fs_name);
        func1.updateAttributeByName("name",fs_name);
        func1.updateAttributeByName("value",QString("%1.fs_name.FS_%2").arg(fs_name,iFS));
        fsParamInvoke.setInvokeFunctionGeneric(func1);
        GTASCXMLAssign timeAssign1;
        timeAssign1.setLocation(QString("%1.time_stamp").arg(fs_name));
        timeAssign1.setExpression(QString("_event.data.%1.time_stamp").arg(fs_name));
        GTASCXMLAssign valueAssign1;
        valueAssign1.setLocation(QString("%1.value").arg(fs_name));
        valueAssign1.setExpression(QString("_event.data.%1.value").arg(fs_name));
        fsParamInvoke.insertAssign(timeAssign1);
        fsParamInvoke.insertAssign(valueAssign1);
        GTASCXMLAssign invokeCntrAssign1;
        invokeCntrAssign1.setExpression(QString("%1.value + 1").arg(invokeCntr));
        invokeCntrAssign1.setLocation(QString("%1.value").arg(invokeCntr));
        fsParamInvoke.insertAssign(invokeCntrAssign1);
        oEventList.append(QString("done.invoke.%1").arg(fsInvokeId));
        oState.insertInvokes(fsParamInvoke);
    }
}

void GTAGenericSCXMLController::insertInvokeForParam(const QStringList &iParamList, GTASCXMLState &oState, QStringList &oEventList, int id, bool isFSonly, bool isValueOnly)
{
    QString invokeCntr = QString("_%1_invokeCntr").arg(oState.getId());
    int invokeIdCntr = 0;
    for(int i=0;i<iParamList.count();i++)
    {
        QString param = iParamList.at(i);
        QString fs_name = "FS_"+iParamList.at(i);

        //if only FS is required, Invoke for value is not generated
        if (!isFSonly)
        {
			QString invokeId = (id == -1)? QString("%1_i%2").arg(oState.getId(),QString::number(invokeIdCntr)) : QString("%1_i%2").arg(oState.getId(),QString::number(id++));
            GTASCXMLInvoke paramInvoke;
            getInvokeForParamGeneric(paramInvoke,invokeId);
            GTASCXMLGenericFunction func = paramInvoke.getGenericFunction();
            func.setFunctionReturn(param);
            func.updateAttributeByName("name",param);
            paramInvoke.setInvokeFunctionGeneric(func);
            GTASCXMLAssign timeAssign;
            timeAssign.setLocation(QString("%1.time_stamp").arg(param));
            timeAssign.setExpression(QString("_event.data.%1.time_stamp").arg(param));
            GTASCXMLAssign valueAssign;
            valueAssign.setLocation(QString("%1.value").arg(param));
            valueAssign.setExpression(QString("_event.data.%1.value").arg(param));
            paramInvoke.insertAssign(timeAssign);
            paramInvoke.insertAssign(valueAssign);
            GTASCXMLAssign invokeCntrAssign;
            invokeCntrAssign.setExpression(QString("%1.value + 1").arg(invokeCntr));
            invokeCntrAssign.setLocation(QString("%1.value").arg(invokeCntr));
            paramInvoke.insertAssign(invokeCntrAssign);
            oEventList.append(QString("done.invoke.%1").arg(invokeId));

            oState.insertInvokes(paramInvoke);
        }

        //if only Value is required, Invoke for FS is not generated
        if (!isValueOnly)
        {
			QString fsInvokeId = (id == -1)? QString("%1_i%2").arg(oState.getId(),QString::number(++invokeIdCntr)) : QString("%1_i%2").arg(oState.getId(),QString::number(id++));
            GTASCXMLInvoke fsParamInvoke;
            getInvokeForParamGeneric(fsParamInvoke,fsInvokeId);
            GTASCXMLGenericFunction func1 = fsParamInvoke.getGenericFunction();
            func1.setFunctionReturn(fs_name);
            func1.updateAttributeByName("name",fs_name);
            fsParamInvoke.setInvokeFunctionGeneric(func1);
            GTASCXMLAssign timeAssign1;
            timeAssign1.setLocation(QString("%1.time_stamp").arg(fs_name));
            timeAssign1.setExpression(QString("_event.data.%1.time_stamp").arg(fs_name));
            GTASCXMLAssign valueAssign1;
            valueAssign1.setLocation(QString("%1.value").arg(fs_name));
            valueAssign1.setExpression(QString("_event.data.%1.value").arg(fs_name));
            fsParamInvoke.insertAssign(timeAssign1);
            fsParamInvoke.insertAssign(valueAssign1);
            GTASCXMLAssign invokeCntrAssign1;
            invokeCntrAssign1.setExpression(QString("%1.value + 1").arg(invokeCntr));
            invokeCntrAssign1.setLocation(QString("%1.value").arg(invokeCntr));
            fsParamInvoke.insertAssign(invokeCntrAssign1);
            oEventList.append(QString("done.invoke.%1").arg(fsInvokeId));

            oState.insertInvokes(fsParamInvoke);
        }
        invokeIdCntr++;
    }
}

void GTAGenericSCXMLController::createGenericSCXMLPrintParameter(const GTAActionPrint *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId)
{
    if(iPrintCmd == NULL)
        return;

    QStringList values = iPrintCmd->getValues();
    QString instanceName = iPrintCmd->getInstanceName();

    QStringList invokeTypeParamList;

    GTASCXMLState printParamState;
    printParamState.setId(iStateId);

    GTASCXMLOnExit onExit;
    GTASCXMLOnEntry onEntry;

    QStringList localParams;
    for(int i=0;i<values.count();i++)
    {
        QString param = values.at(i);
        GTAGenericSCXMLController::ParamType type= getParamType(param);
        GTAScxmlLogMessage logMessage1;


        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(param);
            param = p.getSignalName();
            logMessage1.isCurrentValParam = true;
            invokeTypeParamList.append(param);
            cmdParameterList.append(p.getTempParamName());
            continue;
        }
        else if(type == PARAM_STRING)
        {
            param.replace("\"","");
            param.replace("\'","");
            logMessage1.isCurrentValParam = false;
        }
        else if(type == PARAM_NUM)
        {
            logMessage1.isCurrentValParam = false;
        }
        else if(type == PARAM_LOCAL)
        {
            logMessage1.isCurrentValParam = true;
            if(_EnGenerateInvokeForLocalVar)
                invokeTypeParamList.append(param);
            cmdParameterList.append(param);
        }

        logMessage1.CurrentValue = param;
        logMessage1.ExpectedValue = param;
        logMessage1.Identifier = instanceName;
        logMessage1.FunctionStatus = "NA";
        logMessage1.Result = GTAScxmlLogMessage::OK;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
        QString log1 = logMessage1.getLogMessage(_isGenericSCXMLCofig);
        GTASCXMLLog log;
        log.setLabel("'info'");
        log.setExpression(QString("%1").arg(log1));
        onExit.insertLog(log);
    }



    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);


    if(invokeTypeParamList.count()>0)
    {
        QStringList oEventList;
        insertInvokeForParam(invokeTypeParamList,printParamState,oEventList);

        QString invokeCntr = QString("_%1_invokeCntr").arg(iStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        printParamState.setDataModel(datamodel);

        tx.setCondition(QString("%1.value == %2").arg(invokeCntr,QString::number(oEventList.count())));
        tx.setEvent(oEventList.join(" "));
    }

    printParamState.insertTransitions(tx);
    printParamState.setOnEntry(onEntry);
    printParamState.setOnExit(onExit);
    QStringList params = iPrintCmd->getVariableList();
    for(int i=0;i<params.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(params.at(i));
        printParamState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            printParamState.insertVarList(QStringList()<<params.at(i));
        }
    }

    printParamState.embedLog(true,iPrintCmd->getInstanceName());
    iState.insertStates(printParamState);
}

void GTAGenericSCXMLController::createGenericSCXMLPrintTable(const GTAActionPrintTable *iPrintCmd,  GTASCXMLState &iState, const QString &iStateId, const QString &iTargetId)
{
    if(iPrintCmd == NULL)
        return;

    QStringList values = iPrintCmd->getValues();
    QString instanceName = iPrintCmd->getInstanceName();

    QStringList invokeTypeParamList;

    GTASCXMLState printParamState;
    printParamState.setId(iStateId);

    GTASCXMLOnExit onExit;
    GTASCXMLOnEntry onEntry;

    for(int i=0;i<values.count();i++)
    {
        QString param = values.at(i);
        GTAGenericSCXMLController::ParamType type= getParamType(param);

        GTAScxmlLogMessage logMessage1;


        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(param);
            param = p.getSignalName();
            logMessage1.isCurrentValParam = true;
            invokeTypeParamList.append(param);
            cmdParameterList.append(param);
        }
        else if(type == PARAM_STRING)
        {
            param.replace("\"","");
            param.replace("\'","");
            logMessage1.isCurrentValParam = false;
        }
        else if(type == PARAM_NUM)
        {
            logMessage1.isCurrentValParam = false;
        }
        else if(type == PARAM_LOCAL)
        {
            logMessage1.isCurrentValParam = true;
            if(_EnGenerateInvokeForLocalVar)
                invokeTypeParamList.append(param);
            cmdParameterList.append(param);
        }
        logMessage1.CurrentValue = param;
        logMessage1.ExpectedValue = param;
        logMessage1.Identifier = instanceName;
        logMessage1.FunctionStatus = "NA";
        logMessage1.Result = GTAScxmlLogMessage::OK;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
        QString log1 = logMessage1.getLogMessage(_isGenericSCXMLCofig);
        GTASCXMLLog log;
        log.setLabel("'info'");
        log.setExpression(QString("%1").arg(log1));
        onExit.insertLog(log);
    }



    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);


    if(invokeTypeParamList.count()>0)
    {
        QStringList oEventList;
        insertInvokeForParam(invokeTypeParamList,printParamState,oEventList);

        QString invokeCntr = QString("_%1_invokeCntr").arg(iStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        printParamState.setDataModel(datamodel);

        tx.setCondition(QString("%1.value == %2").arg(invokeCntr,QString::number(oEventList.count())));
        tx.setEvent(oEventList.join(" "));
    }

    printParamState.insertTransitions(tx);
    printParamState.setOnEntry(onEntry);
    printParamState.setOnExit(onExit);
    QStringList params = iPrintCmd->getVariableList();
    for(int i=0;i<params.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(params.at(i));
        printParamState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            printParamState.insertVarList(QStringList()<<params.at(i));
        }
    }

    printParamState.embedLog(true,iPrintCmd->getInstanceName());
    iState.insertStates(printParamState);
}

void GTAGenericSCXMLController::createSCXMLDataModel(GTASCXMLSCXML &iSCXMLNode)
{
    QStringList scxmlVarList;
    scxmlVarList.append(cmdParameterList);
    //scxmlVarList.append(iSCXMLNode.getVariableList());
    scxmlVarList.removeDuplicates();

    GTASCXMLDataModel dataModel;

    QList<GTASCXMLData> _paramData;
    QList<GTASCXMLData> _localVars;

    for(int i=0;i<scxmlVarList.count();i++)
    {
        QString id =scxmlVarList.at(i);
        GTAGenericSCXMLController::ParamType type = getParamType(id);
        if((type == GTAGenericSCXMLController::PARAM_DB) || (_EnGenerateInvokeForLocalVar && (type == GTAGenericSCXMLController::PARAM_LOCAL)))
        {
            GTAICDParameter param;
            if(type == GTAGenericSCXMLController::PARAM_DB)
                param = _dbParamList.value(id);
            else if((type == GTAGenericSCXMLController::PARAM_LOCAL) && _EnGenerateInvokeForLocalVar)
            {
                param.setTempParamName(id);
                param.setSignalName(id);
                param.setUnit("NA");
                param.setValueType("NA");
                param.setMinValue("NA");
            }

            QString paramUnit = param.getUnit();
            QString paramdefValue = param.getMinValue();
            QString paramType = param.getValueType();
            QString signalName = param.getSignalName();

            QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"%2\",\"unit\":\"%3\",\"value\":\"%4\",\"time_stamp\":\"\",\"validity_id\":\"FS_%1\"}").arg(signalName,paramType,paramUnit,paramdefValue);

            //QString jsonexprFSEnums = QString("\"validityVal\":{\"FS_ND\":\"0x00\",\"FS_NO\":\"0x03\",\"FS_NCD\":\"0x01\",\"FS_FT\":\"0x02\"}");
            //QString jsonexprFSEnums = QString("\"validityVal\":{\"FS_ND\":\"1\",\"SSM_BCD_PLUS\":\"2\",\"SSM_BCD_NORTH\":\"3\",\"SSM_BCD_EAST\":\"4\",\"SSM_BCD_RIGHT\":\"5\",\"SSM_BCD_To\":\"6\",\"SSM_BCD_Above\":\"7\",\"SSM_BNR_FW\":\"8\",\"SSM_DIS_NO\":\"9\",\"FS_NO\":\"10\",\"SSM_BCD_MINUS\":\"11\",\"SSM_BCD_SOUTH\":\"12\",\"SSM_BCD_WEST\":\"13\",\"SSM_BCD_LEFT\":\"14\",\"SSM_BCD_From\":\"15\",\"SSM_BCD_Below\":\"16\",\"SSM_BNR_NO\":\"17\",\"SSM_DIS_FW\":\"18\",\"FS_NCD\":\"19\",\"SSM_BCD_NCD\":\"20\",\"SSM_BNR_NCD\":\"21\",\"SSM_DIS_NCD\":\"22\",\"FS_FT\":\"23\",\"SSM_BCD_FT\":\"24\",\"SSM_BNR_FT\":\"25\",\"SSM_DIS_FT\":\"26\"}");
            QString jsonexprFSEnums = QString("{\"FS\":{\"ND\":\"1\",\"NO\":\"3\",\"NCD\":\"2\",\"FT\":\"0\"},\"SSM\":{\"BCD\":{\"ABOVE\":\"0\",\"TO\":\"0\",\"RIGHT\":\"0\",\"EAST\":\"0\",\"NORTH\":\"0\",\"PLUS\":\"0\",\"FT\":\"2\",\"NCD\":\"1\",\"BELOW\":\"3\",\"FROM\":\"3\",\"LEFT\":\"3\",\"WEST\":\"3\",\"SOUTH\":\"3\",\"MINUS\":\"3\"},\"BNR\":{\"FW\":\"0\",\"NO\":\"3\",\"FT\":\"2\",\"NCD\":\"1\"},\"DIS\":{\"FW\":\"3\",\"NO\":\"0\",\"FT\":\"2\",\"NCD\":\"1\"}}}");
            QString fsJSONStructId = QString("{\"name\":\"FS_%1\",\"type\":\"enumerate\",\"unit\":\"NA\",\"value\":%2,\"time_stamp\":\"\"}").arg(signalName,jsonexprFSEnums);

            GTASCXMLData data;
            data.setDataId(signalName);
            data.setDataExpr(jsonexprParamIdData);

            GTASCXMLData data1;
            data1.setDataId(QString("FS_%1").arg(signalName));
            data1.setDataExpr(fsJSONStructId);

            _paramData.append(data);
            _paramData.append(data1);
        }
        else
        {
            GTASCXMLData data;

            QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"NA\",\"unit\":\"NA\",\"value\":\"NA\",\"time_stamp\":\"\",\"validity_id\":\"NA\"}").arg(id);

            data.setDataId(id);
            data.setDataExpr(jsonexprParamIdData);
            _localVars.append(data);
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

void GTAGenericSCXMLController::createGenericSCXMLConditionIf(const GTAActionIF *iIfCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId)
{
    if(iIfCmd == NULL)
        return;
    GTAActionElse *pElseCmd = NULL;
    GTAActionIF *pIfCmd = new GTAActionIF(*iIfCmd);
    int eventId = 0;
    double dTimeoutVal = iIfCmd->getTimeOutInMs();
    QString actOnFail = iIfCmd->getActionOnFail();
    QStringList varList = iIfCmd->getVariableList();
    QStringList dumpList = iIfCmd->getDumpList();
    bool isChkOnlyFS = iIfCmd->getIsFsOnly();
    varList.append(dumpList);

    varList.removeDuplicates();
    cmdParameterList.append(varList);
    pIfCmd->clearChildrenList();

    QString cmdCond;

    QStringList dbParamList;
    QStringList localParamList;
    for(int i=0;i<varList.count();i++)
    {
        GTAGenericSCXMLController::ParamType type = getParamType(varList.at(i));
        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(varList.at(i));
            varList[i] = p.getSignalName();
            dbParamList.append(p.getSignalName());
        }
        else if(type == GTAGenericSCXMLController::PARAM_LOCAL)
            localParamList.append(varList.at(i));
    }

    if(dbParamList.isEmpty() && isChkOnlyFS)
        return;
    iIfCmd->getSCXMLConditionStatement(dbParamList,cmdCond,QString(),true,localParamList);

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
        }
    }

    QString parentId = iState.getId();
    QString currentId = iStateId;
    currentId.replace(parentId,"");
    if(currentId.contains("_s"))
    {
        currentId.replace("_s","");
    }
    else
    {
        currentId.replace("_cs","");
    }
    currentId.replace("_","");

    int stateIdCnt = currentId.toInt();
    GTASCXMLState cmdState;

    QString cmdStateId =  QString("%1_cs%2").arg(parentId,QString::number(stateIdCnt));
    QString cmdStateFinalStateId = QString("%1_final").arg(cmdStateId);
    QString ifStateId = QString("%1_cs%2").arg(cmdStateId,"0");
    QString getParamStateId = QString("%1_s0").arg(cmdStateId);
    QString elseStateId = QString("%1_cs%2").arg(cmdStateId,"1");


    GTASCXMLTransition getParamTxIf;
    getParamTxIf.setTarget(QString("%1_UserDef_If").arg(ifStateId));
    GTASCXMLTransition getParamTxElse;
    if(pElseCmd != NULL)
        getParamTxElse.setTarget(QString("%1_UserDef_Else").arg(elseStateId));
    else
    {
        //getParamTxElse.setTarget(cmdStateFinalStateId);
        getParamTxElse.setTarget(iTargetId);
    }

    GTASCXMLState getParamState;
    getParamState.setId(getParamStateId);


    GTASCXMLOnEntry onEntry;
    GTASCXMLOnExit onExit;

    if(_EnGenerateInvokeForLocalVar)
        dbParamList.append(localParamList);

    if(dbParamList.count()>0)
    {
        QStringList oEventList;
        insertInvokeForParam(dbParamList,getParamState,oEventList);

        QString invokeCntr = QString("_%1_invokeCntr").arg(iStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        getParamState.setDataModel(datamodel);

        getParamTxIf.setCondition(QString("((%3)== true) && (%1.value == %2)").arg(invokeCntr,QString::number(oEventList.count()),cmdCond));
        getParamTxIf.setEvent(oEventList.join(" "));
        getParamTxElse.setCondition(QString("((%3) == false) && (%1.value == %2)").arg(invokeCntr,QString::number(oEventList.count()),cmdCond));
        getParamTxElse.setEvent(oEventList.join(" "));

        getParamState.insertTransitions(getParamTxIf);
        getParamState.insertTransitions(getParamTxElse);
        getParamState.setOnEntry(onEntry);
        getParamState.setOnExit(onExit);

    }
    else
    {
        getParamTxIf.setCondition(QString("(%3)== true").arg(cmdCond));
        getParamTxElse.setCondition(QString("(%3)== false").arg(cmdCond));
        getParamState.insertTransitions(getParamTxIf);
        getParamState.insertTransitions(getParamTxElse);
    }

    cmdState.insertStates(getParamState);

    if(pIfCmd != NULL)
    {

        GTASCXMLState ifState;
        ifState.setId(QString("%1").arg(ifStateId));
        getStateChart(pIfCmd,ifState);
        ifState.setId(QString("%1_UserDef_If").arg(ifStateId));
        GTASCXMLTransition ifTx;
        ifTx.setTarget(cmdStateFinalStateId);


        GTASCXMLFinal ifFinal;
        ifFinal.setId(QString("%1_final").arg(ifStateId));

        GTASCXMLOnEntry finalOnEntry;
        GTASCXMLSend finalSend;
        QString finalEventId = QString("%1_final_event").arg(ifStateId);
        finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
        finalOnEntry.insertSend(finalSend);
        ifFinal.setOnEntry(finalOnEntry);
        ifTx.setEvent(finalEventId);

        ifState.setFinal(ifFinal);
        ifState.insertTransitions(ifTx);

        cmdState.insertStates(ifState);


    }

    if(pElseCmd != NULL)
    {

        GTASCXMLState elseState;
        elseState.setId(QString("%1").arg(elseStateId));
        getStateChart(pElseCmd,elseState);
        elseState.setId(QString("%1_UserDef_Else").arg(elseStateId));

        GTASCXMLTransition elseTx;
        elseTx.setTarget(cmdStateFinalStateId);

        GTASCXMLFinal elseFinal;
        elseFinal.setId(QString("%1_final").arg(elseStateId));

        GTASCXMLOnEntry finalOnEntry;
        GTASCXMLSend finalSend;
        QString finalEventId = QString("%1_final_event").arg(elseStateId);
        finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
        finalOnEntry.insertSend(finalSend);
        elseFinal.setOnEntry(finalOnEntry);
        elseTx.setEvent(finalEventId);

        elseState.setFinal(elseFinal);
        elseState.insertTransitions(elseTx);

        cmdState.insertStates(elseState);
    }

    
    GTASCXMLTransition cmdStateTx;
    cmdStateTx.setTarget(iTargetId);

    if(dTimeoutVal>0)
    {
        GTASCXMLSend timeoutEvent;
        QString timeoutEventName = QString("%1_timeout_%2").arg(cmdStateId,QString::number(eventId));
        timeoutEvent.setDelay(QString::number(dTimeoutVal)+"ms");
        timeoutEvent.setEvent(timeoutEventName);
        eventId++;
        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        if(!actOnFail.isEmpty())
        {
            if(actOnFail == "stop")
                timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
            else
                timeoutTx.setTarget(iTargetId);
        }
        onEntry.insertSend(timeoutEvent);
        cmdState.insertTransitions(timeoutTx,false);
    }
    cmdState.setOnEntry(onEntry);
    cmdState.setOnExit(onExit);

    GTASCXMLFinal cmdStateFinal;
    cmdStateFinal.setId(cmdStateFinalStateId);

    cmdState.setId(QString("%1_UserDef_IfElse").arg(cmdStateId));

    GTASCXMLOnEntry finalOnEntry;
    GTASCXMLSend finalSend;
    QString finalEventId = QString("%1_final_event").arg(cmdStateId);
    finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
    finalOnEntry.insertSend(finalSend);
    cmdStateFinal.setOnEntry(finalOnEntry);
    cmdStateTx.setEvent(finalEventId);

    cmdState.setFinal(cmdStateFinal);
    cmdState.insertTransitions(cmdStateTx);
    for(int i=0;i<varList.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(varList.at(i));
        cmdState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            cmdState.insertVarList(QStringList()<<varList.at(i));
        }
    }
    cmdState.embedLog(true,iIfCmd->getInstanceName());
    iState.insertStates(cmdState);
}
void GTAGenericSCXMLController::createGenericSCXMLConditionDoWhile(const GTAActionDoWhile *iDoWhileCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId)
{
    if(iDoWhileCmd == NULL)
        return;

    QStringList varList = iDoWhileCmd->getVariableList();
    QStringList dumpList = iDoWhileCmd->getDumpList();
    varList.append(dumpList);
    int eventId = 0;
    double dTimeoutVal = iDoWhileCmd->getTimeOutInMs();
    QString actOnFail = iDoWhileCmd->getActionOnFail();
    varList.removeDuplicates();
    cmdParameterList.append(varList);

    QString cmdCond;

    QStringList dbParamList;
    QStringList localParamList;
    for(int i=0;i<varList.count();i++)
    {
        GTAGenericSCXMLController::ParamType type = getParamType(varList.at(i));
        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(varList.at(i));
            varList[i] = p.getSignalName();
            dbParamList.append(varList.at(i));
        }
        else if(type == GTAGenericSCXMLController::PARAM_LOCAL)
            localParamList.append(varList.at(i));
    }

    iDoWhileCmd->getSCXMLConditionStatement(dbParamList,cmdCond,QString(),true,localParamList);
    //    validateExpression(cmdCond);

    GTAActionDoWhile *pDoWhileCmd = new GTAActionDoWhile(*iDoWhileCmd);
    QList<GTACommandBase *> children = iDoWhileCmd->getChildren();
    pDoWhileCmd->clearChildrenList();

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
                    pDoWhileCmd->insertChildren(pCmd,i);
                }
            }
        }
    }

    QString parentId = iState.getId();
    QString currentId = iStateId;
    currentId.replace(parentId,"");
    if(currentId.contains("_s"))
    {
        currentId.replace("_s","");
    }
    else
    {
        currentId.replace("_cs","");
    }
    currentId.replace("_","");

    int stateIdCnt = currentId.toInt();
    GTASCXMLState cmdState;

    QString cmdStateId =  QString("%1_cs%2").arg(parentId,QString::number(stateIdCnt));
    QString cmdStateFinalStateId = QString("%1_final").arg(cmdStateId);
    QString getParamStateId = QString("%1_s0").arg(cmdStateId);
    cmdState.setId(cmdStateId);




    GTASCXMLTransition getParamTxElse;
    getParamTxElse.setTarget(cmdStateFinalStateId);

    GTASCXMLState getParamState;
    getParamState.setId(getParamStateId);


    QList<GTASCXMLState> stateList;
    if(pDoWhileCmd != NULL)
    {
        getStateChart(pDoWhileCmd,cmdState);
        stateList = cmdState.getStates();
        if(stateList.count()>0)
        {
            GTASCXMLState lstState = stateList.last();
            QString lstStateId = lstState.getId();
            lstStateId.replace(cmdStateId,"");
            lstStateId.replace("_s","");
            int idx = lstStateId.toInt();
            if(idx>=0)
            {
                getParamState.setId(QString("%1_s%2").arg(cmdStateId,QString::number(idx+1)));
            }
        }

    }
    GTASCXMLOnEntry onEntry;
    GTASCXMLOnExit onExit;

    if(_EnGenerateInvokeForLocalVar)
        dbParamList.append(localParamList);
    if(dbParamList.count()>0)
    {

        QStringList oEventList;
        insertInvokeForParam(dbParamList,getParamState,oEventList);

        QString invokeCntr = QString("_%1_invokeCntr").arg(iStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);
        getParamState.setDataModel(datamodel);
        getParamTxElse.setEvent(oEventList.join(" "));
        getParamState.setOnEntry(onEntry);
        getParamState.setOnExit(onExit);

    }
    GTASCXMLTransition cmdStateTxIf;
    GTASCXMLTransition cmdStateTxElse;
    cmdStateTxIf.setCondition(QString("(%3)== true").arg(cmdCond));
    cmdStateTxElse.setCondition(QString("(%3)== false").arg(cmdCond));

    cmdStateTxIf.setTarget(QString("%1_UserDef_DoWhile").arg(cmdStateId));
    cmdStateTxElse.setTarget(iTargetId);

    cmdState.insertTransitions(cmdStateTxIf);
    //cmdState.insertTransitions(cmdStateTxElse);



    getParamState.insertTransitions(getParamTxElse);
    if(dTimeoutVal>0)
    {
        GTASCXMLSend timeoutEvent;
        QString timeoutEventName = QString("%1_timeout_%2").arg(cmdStateId,QString::number(eventId));
        timeoutEvent.setDelay(QString::number(dTimeoutVal)+"ms");
        timeoutEvent.setEvent(timeoutEventName);
        eventId++;
        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        if(!actOnFail.isEmpty())
        {
            if(actOnFail == "stop")
                timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
            else
                timeoutTx.setTarget(iTargetId);
        }
        onEntry.insertSend(timeoutEvent);
        cmdState.insertTransitions(timeoutTx,true);
    }
    cmdState.setOnEntry(onEntry);
    cmdState.setOnExit(onExit);


    GTASCXMLFinal cmdFinalNode;
    cmdFinalNode.setId(cmdStateFinalStateId);

    GTASCXMLOnEntry finalOnEntry;
    GTASCXMLSend finalSend;
    QString finalEventId = QString("%1_final_event").arg(cmdStateId);
    finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
    finalOnEntry.insertSend(finalSend);
    cmdFinalNode.setOnEntry(finalOnEntry);
    cmdStateTxElse.setEvent(finalEventId);

    cmdState.setFinal(cmdFinalNode);
    cmdState.insertTransitions(cmdStateTxElse);
    cmdState.setId(QString("%1_UserDef_DoWhile").arg(cmdStateId));
    cmdState.insertStates(getParamState);
    for(int i=0;i<varList.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(varList.at(i));
        cmdState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            cmdState.insertVarList(QStringList()<<varList.at(i));
        }
    }
    cmdState.embedLog(true,iDoWhileCmd->getInstanceName());
    iState.insertStates(cmdState);
}

void GTAGenericSCXMLController::createGenericSCXMLConditionWhile(const GTAActionWhile *iWhileCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId)
{
    if(iWhileCmd == NULL)
        return;

    QStringList varList = iWhileCmd->getVariableList();
    QStringList dumpList = iWhileCmd->getDumpList();
    varList.append(dumpList);
    int eventId = 0;
    double dTimeoutVal = iWhileCmd->getTimeOutInMs();
    QString actOnFail = iWhileCmd->getActionOnFail();
    varList.removeDuplicates();
    cmdParameterList.append(varList);

    QString cmdCond;
    QStringList dbParamList;
    QStringList localParamList;
    for(int i=0;i<varList.count();i++)
    {
        GTAGenericSCXMLController::ParamType type = getParamType(varList.at(i));
        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(varList.at(i));
            varList[i] = p.getSignalName();
            dbParamList.append(varList.at(i));
        }
        else if(type == GTAGenericSCXMLController::PARAM_LOCAL)
            localParamList.append(varList.at(i));
    }

    iWhileCmd->getSCXMLConditionStatement(dbParamList,cmdCond,QString(),true,localParamList);
    //    validateExpression(cmdCond);

    GTAActionWhile *pWhileCmd = new GTAActionWhile(*iWhileCmd);
    QList<GTACommandBase *> children = iWhileCmd->getChildren();
    pWhileCmd->clearChildrenList();

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

    QString parentId = iState.getId();
    QString currentId = iStateId;
    currentId.replace(parentId,"");
    if(currentId.contains("_s"))
    {
        currentId.replace("_s","");
    }
    else
    {
        currentId.replace("_cs","");
    }
    currentId.replace("_","");

    int stateIdCnt = currentId.toInt();
    GTASCXMLState cmdState;

    QString cmdStateId =  QString("%1_cs%2").arg(parentId,QString::number(stateIdCnt));
    QString cmdStateFinalStateId = QString("%1_final").arg(cmdStateId);
    QString getParamStateId = QString("%1_s0").arg(cmdStateId);





    GTASCXMLTransition getParamTxIf;
    getParamTxIf.setTarget("");
    GTASCXMLTransition getParamTxElse;
    getParamTxElse.setTarget(cmdStateFinalStateId);

    GTASCXMLState getParamState;
    getParamState.setId(getParamStateId);


    QList<GTASCXMLState> stateList;
    if(pWhileCmd != NULL)
    {
        GTASCXMLState WhileState;
        WhileState.setId(QString("%1").arg(cmdStateId));
        getStateChart(pWhileCmd,WhileState);
        QString initId = WhileState.getInitId();
        getParamTxIf.setTarget(initId);
        stateList = WhileState.getStates();
        if(stateList.count()>0)
        {
            GTASCXMLState lstState = stateList.last();
            QString lstStateId = lstState.getId();
            lstStateId.replace(cmdStateId,"");
            lstStateId.replace("_s","");
            int idx = lstStateId.toInt();
            if(idx>=0)
            {
                getParamState.setId(QString("%1_s%2").arg(cmdStateId,QString::number(idx+1)));
            }
        }
    }


    GTASCXMLOnEntry onEntry;
    GTASCXMLOnExit onExit;

    if(_EnGenerateInvokeForLocalVar)
        dbParamList.append(localParamList);

    if(dbParamList.count()>0)
    {

        QStringList oEventList;
        insertInvokeForParam(dbParamList,getParamState,oEventList);

        QString invokeCntr = QString("_%1_invokeCntr").arg(iStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        getParamState.setDataModel(datamodel);

        getParamTxIf.setCondition(QString("((%3)== true) && (%1.value == %2)").arg(invokeCntr,QString::number(oEventList.count()),cmdCond));
        getParamTxIf.setEvent(oEventList.join(" "));
        getParamTxElse.setCondition(QString("((%3) == false) && (%1.value == %2)").arg(invokeCntr,QString::number(oEventList.count()),cmdCond));
        getParamTxElse.setEvent(oEventList.join(" "));

        getParamState.setOnEntry(onEntry);
        getParamState.setOnExit(onExit);

    }
    else
    {
        getParamTxIf.setCondition(QString("(%3)== true").arg(cmdCond));
        getParamTxElse.setCondition(QString("(%3)== false").arg(cmdCond));

    }



    GTASCXMLTransition cmdStateTxIf;
    GTASCXMLTransition cmdStateTxElse;
    cmdStateTxIf.setCondition(QString("(%3)== true").arg(cmdCond));
    cmdStateTxElse.setCondition(QString("(%3)== false").arg(cmdCond));

    cmdStateTxIf.setTarget(QString("%1_UserDef_While").arg(cmdStateId));
    cmdStateTxElse.setTarget(iTargetId);

    cmdState.insertTransitions(cmdStateTxIf);
    //cmdState.insertTransitions(cmdStateTxElse);


    getParamState.insertTransitions(getParamTxIf);
    getParamState.insertTransitions(getParamTxElse);

    if(dTimeoutVal>0)
    {
        GTASCXMLSend timeoutEvent;
        QString timeoutEventName = QString("%1_timeout_%2").arg(cmdStateId,QString::number(eventId));
        timeoutEvent.setDelay(QString::number(dTimeoutVal)+"ms");
        timeoutEvent.setEvent(timeoutEventName);
        eventId++;
        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        if(!actOnFail.isEmpty())
        {
            if(actOnFail == "stop")
                timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
            else
                timeoutTx.setTarget(iTargetId);
        }
        onEntry.insertSend(timeoutEvent);
        cmdState.insertTransitions(timeoutTx,true);
    }
    cmdState.setOnEntry(onEntry);
    cmdState.setOnExit(onExit);

    GTASCXMLFinal cmdFinalNode;
    cmdFinalNode.setId(cmdStateFinalStateId);

    GTASCXMLOnEntry finalOnEntry;
    GTASCXMLSend finalSend;
    QString finalEventId = QString("%1_final_event").arg(cmdStateId);
    finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
    finalOnEntry.insertSend(finalSend);
    cmdFinalNode.setOnEntry(finalOnEntry);
    cmdStateTxElse.setEvent(finalEventId);

    cmdState.setFinal(cmdFinalNode);
    cmdState.insertTransitions(cmdStateTxElse);
    cmdState.setId(QString("%1_UserDef_While").arg(cmdStateId));
    cmdState.insertStates(getParamState);
    for(int i=0;i<stateList.count();i++)
    {
        cmdState.insertStates(stateList.at(i));
    }
    for(int i=0;i<varList.count();i++)
    {
        GTAICDParameter p =_dbParamList.value(varList.at(i));
        cmdState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            cmdState.insertVarList(QStringList()<<varList.at(i));
        }
    }
    cmdState.embedLog(true,iWhileCmd->getInstanceName());
    iState.insertStates(cmdState);

}

void GTAGenericSCXMLController::createGenericSCXMLWaitUntil(const GTAActionWait *iWaitUntil, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId)
{
    if(iWaitUntil == NULL)
        return;

    QStringList varList = iWaitUntil->getVariableList();
    QStringList dumpList = iWaitUntil->getDumpList();
    varList.append(dumpList);

    int eventId = 0;
    double dTimeoutVal = iWaitUntil->getTimeOutInMs();
    QString actOnFail = iWaitUntil->getActionOnFail();
    varList.removeDuplicates();
    cmdParameterList.append(varList);

    QString cmdCond;

    QStringList dbParamList;
    QStringList localParamList;
    for(int i=0;i<varList.count();i++)
    {
        GTAGenericSCXMLController::ParamType type = getParamType(varList.at(i));
        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(varList.at(i));
            varList[i] = p.getSignalName();
            dbParamList.append(varList.at(i));
        }
        else if(type == GTAGenericSCXMLController::PARAM_LOCAL)
            localParamList.append(varList.at(i));
    }

    iWaitUntil->getSCXMLConditionStatement(dbParamList,cmdCond,QString(),true,localParamList);
    //    validateExpression(cmdCond);

    QString parentId = iState.getId();
    QString currentId = iStateId;
    currentId.replace(parentId,"");
    if(currentId.contains("_s"))
    {
        currentId.replace("_s","");
    }
    else
    {
        currentId.replace("_cs","");
    }
    currentId.replace("_","");

    int stateIdCnt = currentId.toInt();
    GTASCXMLState cmdState;

    QString cmdStateId =  QString("%1_cs%2").arg(parentId,QString::number(stateIdCnt));
    QString cmdStateFinalStateId = QString("%1_final").arg(cmdStateId);
    QString getParamStateId = QString("%1_s0").arg(cmdStateId);


    GTASCXMLTransition getParamTxIf;
    GTASCXMLTransition getParamTxElse;

    GTASCXMLState getParamState;
    getParamState.setId(getParamStateId);


    GTASCXMLOnEntry onEntry1;
    GTASCXMLOnExit onExit1;

    for(int i=0;i<localParamList.count();i++)
    {
        GTAScxmlLogMessage logMessage1;
        logMessage1.isCurrentValParam = true;
        logMessage1.CurrentValue = localParamList.at(i);
        logMessage1.ExpectedValue = localParamList.at(i);
        logMessage1.Identifier = iWaitUntil->getInstanceName();
        logMessage1.FunctionStatus = "NA";
        logMessage1.Result = GTAScxmlLogMessage::OK;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Main;
        QString log1 = logMessage1.getLogMessage(_isGenericSCXMLCofig);
        GTASCXMLLog log;
        log.setLabel("'info'");
        log.setExpression(QString("%1").arg(log1));
        onExit1.insertLog(log);
    }
    if(_EnGenerateInvokeForLocalVar)
        dbParamList.append(localParamList);
    if(dbParamList.count()>0)
    {

        QStringList oEventList;
        insertInvokeForParam(dbParamList,getParamState,oEventList);

        QString invokeCntr = QString("_%1_invokeCntr").arg(iStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry1.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        getParamState.setDataModel(datamodel);

        getParamTxIf.setCondition(QString("((%3)== true) && (%1.value == %2)").arg(invokeCntr,QString::number(oEventList.count()),cmdCond));
        getParamTxIf.setEvent(oEventList.join(" "));
        getParamTxElse.setCondition(QString("((%3) == false) && (%1.value == %2)").arg(invokeCntr,QString::number(oEventList.count()),cmdCond));
        getParamTxElse.setEvent(oEventList.join(" "));

        getParamState.setOnEntry(onEntry1);
        getParamState.setOnExit(onExit1);

    }
    else
    {
        getParamTxIf.setCondition(QString("(%3)== true").arg(cmdCond));
        getParamTxElse.setCondition(QString("(%3)== false").arg(cmdCond));
    }


    GTASCXMLTransition cmdStateTxElse;
    cmdStateTxElse.setTarget(iTargetId);

    //cmdState.insertTransitions(cmdStateTxElse);

    getParamTxIf.setTarget(cmdStateFinalStateId);
    getParamTxElse.setTarget(getParamStateId);

    getParamState.insertTransitions(getParamTxIf);
    getParamState.insertTransitions(getParamTxElse);

    GTASCXMLOnEntry onEntry;
    if(dTimeoutVal>0)
    {
        GTASCXMLSend timeoutEvent;
        QString timeoutEventName = QString("%1_timeout_%2").arg(cmdStateId,QString::number(eventId));
        timeoutEvent.setDelay(QString::number(dTimeoutVal)+"ms");
        timeoutEvent.setEvent(timeoutEventName);
        eventId++;
        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        if(!actOnFail.isEmpty())
        {
            if(actOnFail == "stop")
                timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
            else
                timeoutTx.setTarget(iTargetId);
        }
        onEntry.insertSend(timeoutEvent);
        //cmdState.insertTransitions(timeoutTx,true);
        timeoutTx.setTarget(cmdStateFinalStateId);
        getParamState.insertTransitions(timeoutTx);
    }
    GTASCXMLOnExit onExit;
    cmdState.setOnEntry(onEntry);
    cmdState.setOnExit(onExit);

    GTASCXMLFinal cmdFinalNode;
    cmdFinalNode.setId(cmdStateFinalStateId);

    GTASCXMLOnEntry finalOnEntry;
    GTASCXMLSend finalSend;
    QString finalEventId = QString("%1_final_event").arg(cmdStateId);
    finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
    finalOnEntry.insertSend(finalSend);
    cmdFinalNode.setOnEntry(finalOnEntry);
    cmdStateTxElse.setEvent(finalEventId);

    cmdState.setFinal(cmdFinalNode);
    cmdState.insertTransitions(cmdStateTxElse);

    cmdState.setId(QString("%1_UserDef_WaitUntil").arg(cmdStateId));
    cmdState.insertStates(getParamState);
    for(int i=0;i<varList.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(varList.at(i));
        cmdState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            cmdState.insertVarList(QStringList()<<varList.at(i));
        }
    }
    cmdState.embedLog(true,iWaitUntil->getInstanceName());
    iState.insertStates(cmdState);
}

void GTAGenericSCXMLController::createGenericSCXMLSet(const GTAActionSetList *iSetListCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId)
{
    if(iSetListCmd == NULL)
        return;


    int eventId=0;
    double dTimeoutVal = iSetListCmd->getTimeOutInMs();
    QString actOnFail = iSetListCmd->getActionOnFail();
    QList<GTAActionSet *> setList;
    iSetListCmd->getSetCommandlist(setList);


    QStringList varList = iSetListCmd->getVariableList();
    QStringList dumpList = iSetListCmd->getDumpList();
    varList.append(dumpList);

    varList.removeDuplicates();
    cmdParameterList.append(varList);


    QString parentId = iState.getId();
    QString currentId = iStateId;
    currentId.replace(parentId,"");
    if(currentId.contains("_s"))
    {
        currentId.replace("_s","");
    }
    else
    {
        currentId.replace("_cs","");
    }
    currentId.replace("_","");

    int stateIdCnt = currentId.toInt();

    GTASCXMLState cmdState;

    QString cmdStateId =  QString("%1_s%2").arg(parentId,QString::number(stateIdCnt));

    cmdState.setId(cmdStateId);

    QStringList dbParamList;
    for(int i=0;i<varList.count();i++)
    {
        GTAGenericSCXMLController::ParamType type = getParamType(varList.at(i));
        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(varList.at(i));
            varList[i] = p.getSignalName();
            dbParamList.append(varList.at(i));
        }
        else if((type == GTAGenericSCXMLController::PARAM_LOCAL)&&(_EnGenerateInvokeForLocalVar))
        {
            dbParamList.append(varList.at(i));
        }
    }

    QStringList eventList;
    GTASCXMLOnExit onExit;
    GTASCXMLOnEntry onEntry;
    int invokeCounter = 0;
    for(int i=0;i<setList.count();i++)
    {
        GTAActionSet *pSetCmd = setList.at(i);
        if(pSetCmd != NULL)
        {
            QString fs = pSetCmd->getFunctionStatus();
            QString value1 = pSetCmd->getValue();
            QString param1 = pSetCmd->getParameter();
            bool isSetOnlyFS = pSetCmd->getIsSetOnlyFSFixed();
            bool isSetOnlyValue = pSetCmd->getIsSetOnlyValue();

            GTAICDParameter p = _dbParamList.value(param1);
            GTAICDParameter v = _dbParamList.value(value1);

            QString param = p.getSignalName();
            QString value = v.getSignalName();

            if(param.isEmpty())
                param = param1;

            if(value.isEmpty())
                value = value1;

            if((param.trimmed() != "") && dbParamList.contains(param))
            {
                if((value.trimmed() != "") && dbParamList.contains(value))
                {
                    insertInvokeForParam(QStringList()<<value,cmdState,eventList);
                    //insertInvokeForSetParam(param,QString("%1.value").arg(value),fs,isSetOnlyFS,cmdState,eventList);
                }
                GTAGenericSCXMLController::ParamType valueType = getParamType(value);
                if(valueType == GTAGenericSCXMLController::PARAM_LOCAL)
                {
                    insertInvokeForSetParam(param,QString("%1.value").arg(value),fs,false,cmdState,eventList,true,invokeCounter);
                    invokeCounter += 1;
                }
                else
                {
                    insertInvokeForSetParam(param,QString("%1").arg(value),fs,isSetOnlyFS,cmdState,eventList,isSetOnlyValue,invokeCounter);
                    invokeCounter += (isSetOnlyFS || isSetOnlyValue) ? 1 : 2;
                }
            }
            else if(!isSetOnlyFS)
            {
                GTASCXMLAssign assign;
                if((value.trimmed() != "") && dbParamList.contains(value))
                {
                    insertInvokeForParam(QStringList()<<value,cmdState,eventList);
                    assign.setExpression(QString("%1.value").arg(value));
                }
                else
                {
                    GTAGenericSCXMLController::ParamType valueType = getParamType(value);
                    if(valueType == GTAGenericSCXMLController::PARAM_LOCAL)
                        assign.setExpression(QString("%1.value").arg(value));
                    else
                        assign.setExpression(value);
                }
                GTAGenericSCXMLController::ParamType paramType = getParamType(param);
                if(paramType == GTAGenericSCXMLController::PARAM_LOCAL)
                    assign.setLocation(QString("%1.value").arg(param));
                else
                    assign.setLocation(QString("%1").arg(param));
                onExit.insertAssign(assign);
            }
        }
    }

    if(eventList.count()>0)
    {
        QString invokeCntr = QString("_%1_invokeCntr").arg(iStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        cmdState.setDataModel(datamodel);

        GTASCXMLTransition tx;
        tx.setEvent(eventList.join(" "));
        tx.setTarget(iTargetId);
        tx.setCondition(QString("%1.value == %2").arg(invokeCntr,QString::number(eventList.count())));
        cmdState.insertTransitions(tx);
    }
    else
    {
        GTASCXMLTransition tx;
        tx.setTarget(iTargetId);
        cmdState.insertTransitions(tx);
    }
    if((dTimeoutVal>0) && (eventList.count()>0))
    {
        GTASCXMLSend timeoutEvent;
        QString timeoutEventName = QString("%1_timeout_%2").arg(cmdStateId,QString::number(eventId));
        timeoutEvent.setDelay(QString::number(dTimeoutVal)+"ms");
        timeoutEvent.setEvent(timeoutEventName);
        eventId++;
        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        if(!actOnFail.isEmpty())
        {
            if(actOnFail == "stop")
                timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
            else
                timeoutTx.setTarget(iTargetId);
        }
        onEntry.insertSend(timeoutEvent);
        cmdState.insertTransitions(timeoutTx,true);
    }


    cmdState.setOnEntry(onEntry);
    cmdState.setOnExit(onExit);
    cmdState.setId(QString("%1_UserDef_SetList").arg(cmdStateId));
    for(int i=0;i<varList.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(varList.at(i));
        cmdState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            cmdState.insertVarList(QStringList()<<varList.at(i));
        }
    }
    cmdState.embedLog(true,iSetListCmd->getInstanceName());
    iState.insertStates(cmdState);
}

void GTAGenericSCXMLController::createGenericSCXMLCall(const GTAActionExpandedCall *pCallCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId)
{
    if(pCallCmd == NULL)
        return;
    GTASCXMLState callState;
    QString callCmdName = pCallCmd->getElement();
    callCmdName.replace(" ","_");

    QString parentId = iState.getId();
    QString currentId = iStateId;
    currentId.replace(parentId,"");
    if(currentId.contains("_s"))
    {
        currentId.replace("_s","");
    }
    else
    {
        currentId.replace("_cs","");
    }
    currentId.replace("_","");

    int stateIdCnt = currentId.toInt();

    QString cmdStateId =  QString("%1_cs%2").arg(parentId,QString::number(stateIdCnt));

    callState.setId(cmdStateId);

    getStateChart(pCallCmd,callState);

    GTASCXMLFinal final;

    QString finalStateId = QString("%1_final").arg(cmdStateId);
    final.setId(finalStateId);

    GTASCXMLOnEntry finalOnEntry;
    GTASCXMLSend finalSend;
    QString finalEventId = QString("%1_final_event").arg(cmdStateId);
    finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
    finalOnEntry.insertSend(finalSend);
    final.setOnEntry(finalOnEntry);

    callState.setFinal(final);

    GTASCXMLTransition tx;
    tx.setTarget(iTargetId);
    tx.setEvent(finalEventId);
    callState.insertTransitions(tx);

    callState.setId(QString("%1_UserDef_%2").arg(cmdStateId,callCmdName));
    QStringList varList = pCallCmd->getVariableList();
    for(int i=0;i<varList.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(varList.at(i));
        callState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            callState.insertVarList(QStringList()<<varList.at(i));
        }
    }

    callState.embedLog(true,pCallCmd->getInstanceName());
    iState.insertStates(callState);
}

void GTAGenericSCXMLController::createGenericSCXMLCheckValue(const GTACheckValue *iCheckValueCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId)
{
    if(iCheckValueCmd == NULL)
        return;

    int eventId = 0;
    double dTimeoutVal = iCheckValueCmd->getTimeOutInMs();
    QString actOnFail = iCheckValueCmd->getActionOnFail();

    QStringList varList = iCheckValueCmd->getVariableList();
    QStringList dumpList = iCheckValueCmd->getDumpList();
    //varList.append(dumpList);
    bool isWaitUntilEn = iCheckValueCmd->getWaitUntil();
    varList.removeDuplicates(); dumpList.removeDuplicates();
    cmdParameterList.append(varList);cmdParameterList.append(dumpList);

    QString parentId = iState.getId();
    QString currentId = iStateId;
    currentId.replace(parentId,"");
    if(currentId.contains("_s"))
    {
        currentId.replace("_s","");
    }
    else
    {
        currentId.replace("_cs","");
    }
    currentId.replace("_","");

    int stateIdCnt = currentId.toInt();
    GTASCXMLState cmdState;

    QString cmdStateId =  QString("%1_cs%2").arg(parentId,QString::number(stateIdCnt));
    QString cmdStateFinalStateId = QString("%1_final").arg(cmdStateId);
    QString getParamStateId = QString("%1_s0").arg(cmdStateId);

    GTASCXMLTransition getParamTxIf;
    GTASCXMLTransition getParamTxElse;

    GTASCXMLState getParamState;
    getParamState.setId(getParamStateId);


    GTASCXMLOnEntry onEntry;
    GTASCXMLOnExit onExit;

    QString cmdCond;
    QStringList dbParamList;
    QStringList dbParamListActuals;
    QStringList localParamList;
    QStringList dumpParamlist;

    for(int i=0;i<varList.count();i++)
    {
        GTAGenericSCXMLController::ParamType type = getParamType(varList.at(i));
        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(varList.at(i));
            varList[i] = p.getSignalName();
            dbParamList.append(varList.at(i));
            dbParamListActuals.append(p.getTempParamName());
        }
        else if(type == GTAGenericSCXMLController::PARAM_LOCAL)
            localParamList.append(varList.at(i));
    }

    for(int i=0;i<dumpList.count();i++)
    {
        GTAGenericSCXMLController::ParamType type = getParamType(dumpList.at(i));
        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(dumpList.at(i));
            varList[i] = p.getSignalName();
            dumpParamlist.append(dumpList.at(i));
        }
        else if(type == GTAGenericSCXMLController::PARAM_LOCAL)
            localParamList.append(dumpList.at(i));
    }

    GTACheckValue *CheckValueCmd = dynamic_cast<GTACheckValue *>(iCheckValueCmd->getClone());
    QStringList oEventList;
    int invokecounter = 0;
    //get Invokes for dbParamList
    for (int i=0;i<dbParamListActuals.count();i++)
    {
        QStringList EventList;
        QString param = dbParamListActuals.at(i);QString parVal;
        int index = CheckValueCmd->getIndex(param,parVal);
        if(dbParamListActuals.count()>0 && (index != -1))
        {
            insertInvokeForParam(QStringList(param),getParamState,EventList,invokecounter,iCheckValueCmd->getISCheckFS(index),iCheckValueCmd->getISOnlyCheckValue(index));
            if (iCheckValueCmd->getISCheckFS(index) || iCheckValueCmd->getISOnlyCheckValue(index))
                invokecounter++;
            else
                invokecounter += 2;
        }
        oEventList.append(EventList);
    }
    delete CheckValueCmd; CheckValueCmd = NULL;

    //get Invokes for dumpParamlist
    if(dumpParamlist.count()>0)
        foreach (QString prm, dumpParamlist)
        {
            insertInvokeForParam(QStringList(prm),getParamState,oEventList,invokecounter);
            invokecounter += 2;
        }

    //create invokes for local param if enabled in settings
    if(_EnGenerateInvokeForLocalVar)
    {
        foreach (QString prm, localParamList)
        {
            insertInvokeForParam(QStringList(prm),getParamState,oEventList,invokecounter,false,true);
            invokecounter++;
        }
    }

    // to get conditional statement for all params
    dbParamList.append(dumpParamlist);
    iCheckValueCmd->getSCXMLConditionStatement(dbParamList,cmdCond,_genToolReturnMap,QString(),true,localParamList);


    if(dbParamList.count()>0)
    {

//        QStringList oEventList;
//        for (int i=0;i<iCheckValueCmd->getCount();i++)
//        {
//            QStringList EventList;
//            insertInvokeForParam(dbParamList,getParamState,EventList,iCheckValueCmd->getISCheckFS(i),iCheckValueCmd->getISOnlyCheckValue(i));
//            oEventList.append(EventList);
//        }

        QString invokeCntr = QString("_%1_invokeCntr").arg(iStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);
        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);

        getParamState.setDataModel(datamodel);

        GTASCXMLLog successLog;// = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::SUCCESS_LOG,iCheckValueCmd->getInstanceName(),condstat.trimmed(),"true");
        GTASCXMLLog failLog;// = GTACMDSCXMLUtils::getLogMessage(GTACMDSCXMLUtils::FAIL_LOG,iCheckValueCmd->getInstanceName(),condstat.trimmed(),"false");

//      kaam        ------------------------------------------------------------------------

        GTAScxmlLogMessage logMessage1;
        logMessage1.isCurrentValParam = false;
        logMessage1.CurrentValue = QString("true");
        logMessage1.ExpectedValue = QString("true");
        logMessage1.Identifier = iCheckValueCmd->getInstanceName();
        logMessage1.FunctionStatus = "NA";
        logMessage1.Result = GTAScxmlLogMessage::OK;
        logMessage1.Status = GTAScxmlLogMessage::Success;
        logMessage1.LOD = GTAScxmlLogMessage::Main;

        QString log1 = logMessage1.getLogMessage(true);
        successLog.setLabel("'info'");
        successLog.setExpression(QString("%1").arg(log1));

        logMessage1.CurrentValue = QString("false");
        logMessage1.ExpectedValue = QString("false");
        logMessage1.Result = GTAScxmlLogMessage::KO;
        logMessage1.Status = GTAScxmlLogMessage::Fail;

        log1 = logMessage1.getLogMessage(true);
        failLog.setLabel("'info'");
        failLog.setExpression(QString("%1").arg(log1));

//        karo      ------------------------------------------------------------------------

        getParamTxIf.setCondition(QString("((%3)== true) && (%1.value == %2)").arg(invokeCntr,QString::number(oEventList.count()),cmdCond));
        getParamTxIf.setEvent(oEventList.join(" "));
        getParamTxIf.insertLog(successLog);

        getParamTxElse.setCondition(QString("((%3) == false) && (%1.value == %2)").arg(invokeCntr,QString::number(oEventList.count()),cmdCond));
        getParamTxElse.setEvent(oEventList.join(" "));
        getParamTxElse.insertLog(failLog);

        getParamState.setOnEntry(onEntry);
        getParamState.setOnExit(onExit);

    }
    else
    {
        getParamTxIf.setCondition(QString("(%3)== true").arg(cmdCond));
        getParamTxElse.setCondition(QString("(%3)== false").arg(cmdCond));
    }


    GTASCXMLTransition cmdStateTxElse;
    cmdStateTxElse.setTarget(iTargetId);

    //cmdState.insertTransitions(cmdStateTxElse);

    if(isWaitUntilEn)
    {
        getParamTxIf.setTarget(cmdStateFinalStateId);
        if(actOnFail == "stop")
            getParamTxElse.setTarget(SCXML_FINAL_STATE_ID);
        else
            getParamTxElse.setTarget(getParamStateId);
    }
    else
    {
        getParamTxIf.setTarget(cmdStateFinalStateId);
        if(actOnFail == "stop")
            getParamTxElse.setTarget(SCXML_FINAL_STATE_ID);
        else
            getParamTxElse.setTarget(cmdStateFinalStateId);
    }

    getParamState.insertTransitions(getParamTxIf);
    getParamState.insertTransitions(getParamTxElse);

    if(dTimeoutVal>0)
    {
        GTASCXMLSend timeoutEvent;
        QString timeoutEventName = QString("%1_timeout_%2").arg(cmdStateId,QString::number(eventId));
        timeoutEvent.setDelay(QString::number(dTimeoutVal)+"ms");
        timeoutEvent.setEvent(timeoutEventName);
        eventId++;
        GTASCXMLTransition timeoutTx;
        timeoutTx.setEvent(timeoutEventName);
        if(!actOnFail.isEmpty())
        {
            if(actOnFail == "stop")
                timeoutTx.setTarget(SCXML_FINAL_STATE_ID);
            else
                timeoutTx.setTarget(cmdStateFinalStateId);
        }
        onEntry.insertSend(timeoutEvent);
        getParamState.insertTransitions(timeoutTx,true);
    }


    GTASCXMLFinal cmdFinalNode;
    cmdFinalNode.setId(cmdStateFinalStateId);

    GTASCXMLOnEntry finalOnEntry;
    GTASCXMLSend finalSend;
    QString finalEventId = QString("%1_final_event").arg(cmdStateId);
    finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
    finalOnEntry.insertSend(finalSend);
    cmdFinalNode.setOnEntry(finalOnEntry);
    cmdStateTxElse.setEvent(finalEventId);

    cmdState.setOnEntry(onEntry);
    cmdState.setOnExit(onExit);
    cmdState.setFinal(cmdFinalNode);
    cmdState.insertTransitions(cmdStateTxElse);
    cmdState.setId(QString("%1_UserDef_CheckValue").arg(cmdStateId));
    cmdState.insertStates(getParamState);

    for(int i=0;i<varList.count();i++)
    {
        GTAICDParameter p = _dbParamList.value(varList.at(i));
        cmdState.insertVarList(QStringList()<<p.getSignalName());
        if((_EnGenerateInvokeForLocalVar) && (p.getSignalName() != "") && (p.getSignalName() != "NA"))
        {
            cmdState.insertVarList(QStringList()<<varList.at(i));
        }
    }
    cmdState.embedLog(true,iCheckValueCmd->getInstanceName());
    iState.insertStates(cmdState);
}

void GTAGenericSCXMLController::crateGenericSCXMLMaths(const GTAActionMaths * iMathsCmd, GTASCXMLState &iState,const QString &iStateId, const QString &iTargetId)
{
    if(iMathsCmd == NULL)
        return;

    QStringList varList  = iMathsCmd->getVariableList();
    QString lhs = iMathsCmd->getFirstVariable();
    varList.removeDuplicates();
    cmdParameterList.append(varList);

    QStringList dbParamList;
    QStringList localParamList;
    for(int i=0;i<varList.count();i++)
    {
        GTAGenericSCXMLController::ParamType type = getParamType(varList.at(i));
        if(type == GTAGenericSCXMLController::PARAM_DB)
        {
            GTAICDParameter p = _dbParamList.value(varList.at(i));
            varList[i] = p.getSignalName();
            dbParamList.append(varList.at(i));
        }
        else if(type == GTAGenericSCXMLController::PARAM_LOCAL)
            localParamList.append(varList.at(i));
    }

    QString expr = iMathsCmd->getSCXMLExpr();

    if(expr.contains("Math."))
    {
        expr.replace("Math.","");
    }

    for(int i=0;i<dbParamList.count();i++)
    {
        expr.replace(dbParamList.at(i),QString("%1.value").arg(dbParamList.at(i)));
    }
    for(int i=0;i<localParamList.count();i++)
    {
        expr.replace(localParamList.at(i),QString("%1.value").arg(localParamList.at(i)));
    }

    QString parentId = iState.getId();
    QString currentId = iStateId;
    currentId.replace(parentId,"");
    if(currentId.contains("_s"))
    {
        currentId.replace("_s","");
    }
    else
    {
        currentId.replace("_cs","");
    }
    currentId.replace("_","");

    int stateIdCnt = currentId.toInt();
    GTASCXMLState cmdState;


    QString cmdStateId =  QString("%1_cs%2").arg(parentId,QString::number(stateIdCnt));
    QString cmdStateFinalStateId = QString("%1_final").arg(cmdStateId);
    QString getParamStateId = QString("%1_s0").arg(cmdStateId);
    QString setParmId = QString("%1_s1").arg(cmdStateId);

    cmdState.setId(cmdStateId);

    GTASCXMLTransition tx;
    GTASCXMLState getParamState;

    getParamState.setId(getParamStateId);

    GTASCXMLOnExit onExit;
    GTASCXMLOnEntry onEntry;

    if(dbParamList.count()>0)
    {
        QString invokeCntr = QString("_%1_invokeCntr").arg(getParamStateId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        onEntry.insertAssign(initInvokeCntr);
        GTASCXMLData data;
        data.setDataId(invokeCntr);
        QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        data.setDataExpr(jsonexprParamIdData);

        GTASCXMLDataModel datamodel;
        datamodel.insertDataItem(data);
        getParamState.setDataModel(datamodel);

        QStringList eventList;
        insertInvokeForParam(dbParamList,getParamState,eventList);

        tx.setCondition(QString("%1.value == %2").arg(invokeCntr,QString::number(eventList.count())));
        tx.setEvent(eventList.join(" "));

    }

    tx.setTarget(setParmId);
    getParamState.insertTransitions(tx);

    QString localVar = QString("_%1_localVar").arg(cmdStateId);
    GTASCXMLData data;
    data.setDataId(localVar);
    QString jsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(localVar);
    data.setDataExpr(jsonexprParamIdData);

    GTASCXMLDataModel datamodel;
    datamodel.insertDataItem(data);

    cmdState.setDataModel(datamodel);


    GTASCXMLAssign mathsExprAss;
    mathsExprAss.setExpression(expr);
    mathsExprAss.setLocation(QString("%1.value").arg(localVar));

    onExit.insertAssign(mathsExprAss);
    getParamState.setOnEntry(onEntry);
    getParamState.setOnExit(onExit);

    cmdState.insertStates(getParamState);

    GTASCXMLState setParamState;
    GTASCXMLOnExit setStateOnExit;
    GTASCXMLOnEntry setStateOnEntry;

    setParamState.setId(setParmId);
    GTASCXMLTransition setParamTx;


    if(_EnGenerateInvokeForLocalVar)
    {
        dbParamList.append(localParamList);
    }

    if(dbParamList.contains(lhs))
    {
        QStringList eventList;
        insertInvokeForSetParam(lhs,QString("%1.value").arg(localVar),"NO",false,setParamState,eventList,true);

        QString invokeCntr = QString("_%1_invokeCntr").arg(setParmId);
        GTASCXMLAssign initInvokeCntr;
        initInvokeCntr.setExpression("0");
        initInvokeCntr.setLocation(QString("%1.value").arg(invokeCntr));
        setStateOnEntry.insertAssign(initInvokeCntr);
        GTASCXMLData subData;
        subData.setDataId(invokeCntr);
        QString subJsonexprParamIdData = QString("{\"name\":\"%1\",\"type\":\"integer\",\"unit\":\"NA\",\"value\":\"0\",\"time_stamp\":\"NA\",\"validity_id\":\"NA\"}").arg(invokeCntr);
        subData.setDataExpr(subJsonexprParamIdData);

        GTASCXMLDataModel subDatamodel;
        subDatamodel.insertDataItem(subData);
        setParamState.setDataModel(subDatamodel);

        setParamTx.setCondition(QString("%1.value == %2").arg(invokeCntr,QString::number(eventList.count())));
        setParamTx.setEvent(eventList.join(" "));

    }
    else if(localParamList.contains(lhs))
    {
        GTASCXMLAssign setStateAss;
        setStateAss.setExpression(QString("%1.value").arg(localVar));
        setStateAss.setLocation(QString("%1.value").arg(lhs));
        setStateOnExit.insertAssign(setStateAss);
    }

    setParamTx.setTarget(cmdStateFinalStateId);
    setParamState.insertTransitions(setParamTx);
    setParamState.setOnEntry(setStateOnEntry);
    setParamState.setOnExit(setStateOnExit);

    cmdState.insertStates(setParamState);

    GTASCXMLTransition cmdStateTx;
    cmdStateTx.setTarget(iTargetId);
    //cmdState.insertTransitions(cmdStateTx);

    GTASCXMLFinal cmdFinal;
    cmdFinal.setId(cmdStateFinalStateId);

    GTASCXMLOnEntry finalOnEntry;
    GTASCXMLSend finalSend;
    QString finalEventId = QString("%1_final_event").arg(cmdStateId);
    finalSend.setDelay(QString("5ms"));finalSend.setEvent(finalEventId);
    finalOnEntry.insertSend(finalSend);
    cmdFinal.setOnEntry(finalOnEntry);
    cmdStateTx.setEvent(finalEventId);

    cmdState.setFinal(cmdFinal);
    cmdState.insertTransitions(cmdStateTx);

    iState.insertStates(cmdState);
}


void GTAGenericSCXMLController::GenerateHeaderMetaData(const QString &iExportPath, const QString &iFileName)
{
    QString tempFile = iFileName;
    //JAB
    //tempFile.replace(".gscxml",".metaInfo");
    QFileInfo iFileNameInfo = QFileInfo(iFileName);
    tempFile = iFileNameInfo.baseName() + ".metaInfo";
    QString metaFile = QString("%1/%2").arg(iExportPath,tempFile);

    QDomDocument iDomDoc;

    QDomElement headerList = iDomDoc.createElement(META_INFO_HEADER_LIST);
    for(int i=0;i<_headerLists.count();i++)
    {
        GTAHeader *pHeader = _headerLists.at(i);
        if(pHeader != NULL)
        {
            QDomElement header = iDomDoc.createElement(META_INFO_HEADER);
            pHeader->getDomElement(iDomDoc,header);

            header.setAttribute("ELEMENT_NAME",pHeader->getElemName());

            headerList.appendChild(header);
        }
    }
    iDomDoc.appendChild(headerList);

    QFile outFile(metaFile);
    outFile.open(QFile::WriteOnly|QFile::Text);
    QTextStream writer(&outFile);
    QString xmlContents = iDomDoc.toString();
    // replacing all standard xml representation of linefeed and carriage return by \n and \r
    xmlContents.replace("&#xa;","");
    //xmlContents.replace("&#xa;","&lt;br>&lt;/br>");
    xmlContents.replace("&#xd;","");
    //xmlContents.replace("")
    writer<<xmlContents;
    outFile.close();
}
