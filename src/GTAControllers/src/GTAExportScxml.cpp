//#pragma warning(push, 0)
#include "GTAExportScxml.h"
#include "GTAActionIF.h"
#include "GTAActionElse.h"
#include "GTACheckBase.h"
#include "GTAAppController.h"
#include "GTAICDParameter.h"
#include "GTADocumentController.h"
#include "GTAActionCall.h"
#include "GTAActionBase.h"
#include "GTAActionWhile.h"
#include "GTAActionForEach.h"
#include "GTAEquationBase.h"
#include "GTACmdNode.h"
#include "GTAScxmlLogMessage.h"
#include "GTAActionRelease.h"
#include "GTACheckAudioAlarm.h"
#include "GTAEquationConst.h"
#include "GTAActionTitle.h"
#include "GTAActionOneClickSet.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionCallProcedures.h"
#include "GTAInvalidCommand.h"
 


#pragma warning(push, 0)
#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include "GTAUtil.h"
#include <QStringList>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include "qnamespace.h"
#pragma warning(pop)

//TODO: Disable overshadowing variables, fix me in separate task (too many instances of that here)
#pragma warning (disable : 4456)
//TODO: Disable assignment in for warning (don't want to mess with functionality in this task)
#pragma warning (disable : 4706)
//TODO: Disable conversion from obj to ref (again too many instances of that here to fix in this task)
#pragma warning (disable : 4239)
GTAExportScxml::GTAExportScxml(const QString & iTemplatePath)
{
    initializeStateNumber();
    _ScxmlTemplatePath = iTemplatePath;
    _IntermediateElemList.clear();
    _VariableList.clear();
    _hasChannelInControl=false;
    _IsPirParamManagement = false;
    _pElement = NULL;
    _pTitleBasedElement = NULL;
}
GTAExportScxml::~GTAExportScxml()
{
    _VariableList.clear();
    _DataModelList.clear();
}

/** This command export the GTA data model to scxml
  * ipElement: Element to be exported to scxml
  * iFilePath : Full file path of scxml
  * return true if file is created successfully
  */

bool GTAExportScxml::exportToScxml(const GTAElement * ipElement,const QString &iFilePath,bool iHasUnsubscibeAtStart,bool iHasUnsubscibeAtEnd)
{
    QTime t;
    initializeStateNumber();
    bool rc = false;
    if(ipElement == NULL)
    {
        _LastError = "Error: Element is NULL";
        return rc;
    }

    _IsPirParamManagement = TestConfig::getInstance()->getPIRInvokeForSCXML();
    QStringList lstOfVariables ;
    ipElement ->getVariableList(lstOfVariables);

    //check for channel for selection settings.
    bool chanSelSetting = TestConfig::getInstance()->getChannelSelection();
    if(chanSelSetting)
        _hasChannelInControl = ipElement->hasChannelInControl();
    /////////////////////////////////////////////////////////////
	qDebug()<<"Group Start :"<<t.currentTime();
    GTAElement * pElement = (GTAElement*)ipElement;
    setElementName(pElement,iFilePath);
    GTAElement* pTitleBasedElement = NULL;
    qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Grouping):"<<t.currentTime();
    pElement->createActionElementGroupedByTitle(pTitleBasedElement);
    qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Grouping) - Finished:"<<t.currentTime();

    qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Other Stuff):"<<t.currentTime();
    _IntermediateElemList.append(pTitleBasedElement);
    GTAElement* pOneclickElement=NULL;
    pElement->createOneClickElementGroupedByTitle(pOneclickElement);
    _IntermediateElemList.append(pOneclickElement);
	qDebug()<<"Group End :"<<t.currentTime();


	qDebug()<<"Other Stuff Start :"<<t.currentTime();
    //for one click element
    QList<GTACommandBase*> cmdChildList;

    int totalCount=pOneclickElement->getDirectChildrenCount();
    for(int i = 0; i < totalCount; i++)
    {
        GTACommandBase* pCmd =NULL;
        //rc = pElement->getDirectChildren(i,pCmd);
        rc = pOneclickElement->getDirectChildren(i,pCmd);
        if(rc && pCmd != NULL)
        {
            cmdChildList.append(pCmd);
        }
    }

    
    totalCount = pTitleBasedElement->getDirectChildrenCount();
    for(int i = 0; i < totalCount; i++)
    {
        GTACommandBase* pCmd =NULL;
        //rc = pElement->getDirectChildren(i,pCmd);
        rc = pTitleBasedElement->getDirectChildren(i,pCmd);
        if(rc && pCmd != NULL)
        {
            cmdChildList.append(pCmd);
        }
    }

    //pElement->updateCommandInstanceName(QString());
    pTitleBasedElement->setName(ipElement->getName());
    pTitleBasedElement->updateCommandInstanceName(QString());
    _pTitleBasedElement = pTitleBasedElement;
    pOneclickElement->setName(ipElement->getName());
    pOneclickElement->updateCommandInstanceName(QString());
	qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Other Stuff)  - Finished:"<<t.currentTime();
    qDebug()<<"Other Stuff End :"<<t.currentTime();

	qDebug()<<"STate Chart Start :"<<t.currentTime();
	qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Make State):"<<t.currentTime();
    //GTACmdNode::setUuid(pElement->getUuid());
    GTACmdNode * pRootNode = NULL;//new GTACmdNode(NULL,getCurrentStateNumber());
    GTACmdNode * pParentNode = NULL;
    MakeState(cmdChildList,pRootNode,pParentNode);
	qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Make State) - Finished:"<<t.currentTime();

	qDebug()<<"STate Chart End :"<<t.currentTime();
    //Determine the variable is a parameter

	qDebug()<<"DB Start :"<<t.currentTime();
	qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Database):"<<t.currentTime();
    GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
    if(pAppCtrl != NULL)
    {

        GTADataController DataCtrl;
        QStringList InvalidParamList;
        _VariableList.removeDuplicates();

        QStringList tempItems;
        QList<QStringList> complexQuery;
        int cntr = 0;
        foreach(QString item , _VariableList)
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
            
            DataCtrl.getParametersDetail(items,pAppCtrl->getGTADbFilePath(),_ParamList,InvalidParamList);
        }



    }

	qDebug()<<"DB End :"<<t.currentTime();
	qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Database) - Finished:"<<t.currentTime();
    _FailureCommandList = pElement->getAllFailureCommands();

	qDebug()<<"Genrate and Save  scxml start :"<<t.currentTime();
	qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Generate & Write Xml):"<<t.currentTime();
    rc = MakeStateChart(pRootNode,iFilePath,iHasUnsubscibeAtStart,iHasUnsubscibeAtEnd);
	qDebug()<<"GTAExportScxml: Export SCXML Button Clicked(Generate & Write Xml) - Finished:"<<t.currentTime();
    if(pRootNode != NULL)
    {
        delete pRootNode;
        pRootNode  = NULL;

    }
    //Release the reference document pointer
    for(int e = 0; e < _IntermediateElemList.count(); e++)
    {
        GTAElement * pElem = _IntermediateElemList.at(e);
        if(pElem != NULL)
        {
            delete pElem;
            pElem = NULL;
        }
    }
    _IntermediateElemList.clear();
	qDebug()<<"Genrate and Save  scxml End :"<<t.currentTime();
    return rc;
}
/**
  * This function create relationship between commands from the provide set of command
  * & assign unique ID called state id
  * The relationship is used to nevigate up & down as well as parent-child relationship between commands
  * Data structure is as follows
  * Root
  *  |(sibling)
  * NextNode -(Parent-Child)- ChildNode
  *  |(sibling)                 |(sibling)
  *  |                        NextNode
  * NextNode - ChildNode
  */

void GTAExportScxml::MakeState(const QList<GTACommandBase*> & iCmdList , GTACmdNode *& ipNode, GTACmdNode *& ipParent, bool IsCondition )
{
    GTACmdNode * pPrevNode = ipNode;
    
    for(int i = 0; i < iCmdList.count(); i++)
    {
        GTACommandBase * pCmd = iCmdList.at(i);
        if(pCmd != NULL && pCmd->isIgnoredInSCXML()==false)
        {
            QStringList cmdVarList = pCmd->getVariableList();
            if(!cmdVarList.isEmpty())
                _VariableList.append(cmdVarList);

            QStringList dumpVarList = pCmd->getDumpList();
            _VariableList.append(dumpVarList);

            TestConfig* testConf = TestConfig::getInstance();
            bool isChannelSettingsEn = testConf->getChannelSelection();
            if(isChannelSettingsEn && _hasChannelInControl)
            {   
                QString sEngine = QString::fromStdString(testConf->getEngineSelection());
                QHash<QString,QString> engineVarMap;
                _VariableList.append(GTAUtil::getChannelSelectionSignals(sEngine,engineVarMap));
            }

            GTACmdNode * pCmdNode  = NULL;

            GTACommandBase::CommandType cmdType = pCmd->getCommandType();
            if(cmdType == GTACommandBase::ACTION)
            {
                GTAActionBase * pActionCmd = dynamic_cast<GTAActionBase*>((GTACommandBase*)pCmd);
                QString actionName = pActionCmd->getAction();
                QString complementName= pActionCmd->getComplement();
                if(actionName == ACT_MANUAL)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                if(actionName == ACT_MATHS)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                if(actionName == ACT_GEN_TOOL)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                if(actionName == ACT_MATH)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_CONDITION)
                {
                    if(complementName == COM_CONDITION_IF)
                    {
                        GTAActionIF * pActionCmd = dynamic_cast<GTAActionIF*>((GTAActionBase*)pCmd);
                        if(pActionCmd)
                        {
                            pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                            pCmdNode->IsConditionCmd = true;

                            QList<GTACommandBase*> childList = pActionCmd->getChildren();
                            GTAActionElse * pElseCmd = NULL;
                            bool hasElse = false;
                            int childCount = childList.count();
                            int ifCount = childCount - 1;
                            if(childCount >= 2)
                            {
                                GTACommandBase * pChildCmd = childList.at(childCount-2);
                                if(pChildCmd != NULL)
                                {
                                    pElseCmd = dynamic_cast<GTAActionElse*>(pChildCmd);
                                    if(pElseCmd != NULL && pElseCmd->getComplement() == COM_CONDITION_ELSE)
                                    {
                                        hasElse = true;
                                        --ifCount;
                                    }
                                }
                            }
                            QList<GTACommandBase*> listOfDirectChildOfIF;
                            for(int i = 0; i < ifCount; i++)
                            {
                                GTACommandBase * pChildCmd = childList.at(i);
                                listOfDirectChildOfIF.append(pChildCmd);
                            }

                            GTACmdNode * pNextCmdNode = NULL;
                            MakeState(listOfDirectChildOfIF,pNextCmdNode,pCmdNode,true);

                            if(hasElse && pElseCmd != NULL)
                            {
                                GTACmdNode * pNextCmdNode = NULL;
                                QList<GTACommandBase*> elseChildList = pElseCmd->getChildren();
                                MakeState(elseChildList,pNextCmdNode,pCmdNode,false);
                            }
                        }
                    }
                    else if(complementName == COM_CONDITION_WHILE)
                    {
                        GTAActionWhile * pActionCmd = dynamic_cast<GTAActionWhile*>((GTAActionBase*)pCmd);
                        if(pActionCmd != NULL )
                        {
                            pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                            pCmdNode->IsWhileLoop = true;
                            QList<GTACommandBase*> childList = pActionCmd->getChildren();
                            QList<GTACommandBase*> commandList;
                            if(childList.count() < 2)
                            {
                                GTAActionPrint * pDummyPrintCommand  = new GTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
                                pDummyPrintCommand->setValues(QStringList("DummyMessage"));
                                childList.insert(0,pDummyPrintCommand);
                            }
                            for(int i = 0; i < childList.count() -1 ; i++)
                            {
                                commandList.append(childList.at(i));
                            }

                            GTACmdNode * pNextCmdNode = NULL;
                            MakeState(commandList,pNextCmdNode,pCmdNode,true);
                        }
                    }
                    else if(complementName == COM_CONDITION_DO_WHILE)
                    {
                        GTAActionDoWhile * pActionCmd = dynamic_cast<GTAActionDoWhile*>((GTAActionBase*)pCmd);
                        if(pActionCmd != NULL )
                        {
                            pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                            pCmdNode->IsWhileLoop = true;
                            QList<GTACommandBase*> childList = pActionCmd->getChildren();
                            QList<GTACommandBase*> commandList;

                            if(childList.count() < 2)
                            {
                                GTAActionPrint * pDummyPrintCommand  = new GTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
                                pDummyPrintCommand->setValues(QStringList("DummyMessage"));
                                childList.insert(0,pDummyPrintCommand);
                            }
                            for(int i = 0; i < childList.count() -1 ; i++)
                            {
                                commandList.append(childList.at(i));
                            }

                            GTACmdNode * pNextCmdNode = NULL;
                            MakeState(commandList,pNextCmdNode,pCmdNode,true);
                        }
                    }
                    else if(complementName == COM_CONDITION_FOR_EACH)
                    {
//                        GTAActionForEach * pActionCmd = dynamic_cast<GTAActionForEach*>((GTAActionBase*)pCmd);
//                        if(pActionCmd != NULL )
//                        {
//                            pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
//                            pCmdNode->IsWhileLoop = true;
//                            QList<GTACommandBase*> childList = pActionCmd->getChildrens();
//                            QList<GTACommandBase*> commandList;

//                            if(childList.count() < 2)
//                            {
//                                GTAActionPrint * pDummyPrintCommand  = new GTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
//                                pDummyPrintCommand->setValues(QStringList("DummyMessage"));
//                                childList.insert(0,pDummyPrintCommand);
//                            }
//                            for(int i = 0; i < childList.count() -1 ; i++)
//                            {
//                                commandList.append(childList.at(i));
//                            }

//                            GTACmdNode * pNextCmdNode = NULL;
//                            MakeState(commandList,pNextCmdNode,pCmdNode,true);
//                        }
                    }
                }
                
                else if(actionName == ACT_TITLE)
                {
                    GTAActionParentTitle * pActionCmd = dynamic_cast<GTAActionParentTitle*>((GTAActionBase*)pCmd);
                    if(pActionCmd != NULL )
                    {
                        pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                        pCmdNode->IsWhileLoop = false;
                        QList<GTACommandBase*> childList = pActionCmd->getChildren();
                        QList<GTACommandBase*> commandList;
                        for(int i = 0; i < childList.count(); i++)
                        {
                            commandList.append(childList.at(i));
                        }

                        GTACmdNode * pNextCmdNode = NULL;
                        MakeState(commandList,pNextCmdNode,pCmdNode,true);
                    }
                }
                else if(actionName == ACT_SUBSCRIBE)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_UNSUBSCRIBE)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_CALL_PROCS)
                {
                    
                    GTACmdNode * pNextCmdNode = NULL;
                    GTAAppController* pAppCtrl = GTAAppController::getGTAAppController();
                    if(pAppCtrl != NULL)
                    {
                        GTAActionCallProcedures * pCallProcs = dynamic_cast<GTAActionCallProcedures*>((GTAActionBase*)pCmd);
                        if(pCallProcs != NULL)
                        {
                            QList<GTACallProcItem> procedures = pCallProcs->getCallProcedures();
                            bool isParallel = pCallProcs->getPrallelExecution();
                            if (isParallel==false)
                            {
                                pCmdNode = new GTACmdNode(pCmd,getCurrentStatNumber());
                                QList<GTACommandBase*> cmdList;

                                foreach( GTACallProcItem callItem,procedures)
                                {

                                    QString docPath = pAppCtrl->getGTADataDirectory();
                                    QString docFilePath = QString("%1/%2").arg(docPath,callItem._elementName);
                                    GTADocumentController docCtrl;

                                    /* double referenceTimeout = pCallCmd->getTimeoutInMilliseconds();
                                    QList<double> reftimeOuts = pCallCmd->getReferenceTimeoutStates();
                                    reftimeOuts.append(referenceTimeout);*/

                                    bool isUUID = true;
                                    QString elemName = callItem._UUID;
                                    if(elemName.isEmpty())
                                    {
                                        elemName = callItem._elementName;
                                        isUUID = false;
                                    }

                                    GTAElement * pElement =NULL;
                                    GTAHeader* pHdr = NULL;

                                    bool rc= pAppCtrl->getElementFromDocument(elemName,pElement,isUUID);
                                    if(pElement)
                                        pHdr = pElement->getHeader();
                                    if (rc && pElement!=NULL)
                                    {
                                        GTAElement * pTitleBasedElement =NULL;
                                        pElement->createActionElementGroupedByTitle(pTitleBasedElement);
                                        if(callItem._tagValue.isEmpty()==false)
                                            pElement->replaceTag(callItem._tagValue);

                                        if(rc && pTitleBasedElement != NULL)
                                        {
                                            pTitleBasedElement->setName(pElement->getName());


                                            int nbOfCommand = pTitleBasedElement->getDirectChildrenCount();
                                            for(int x= 0; x< nbOfCommand; x++)
                                            {
                                                GTACommandBase * pChildCmd = NULL;
                                                rc =  pTitleBasedElement->getDirectChildren(x,pChildCmd);
                                                if(rc && pChildCmd != NULL)
                                                {
                                                    cmdList.append(pChildCmd);

                                                    /*foreach(double dTimeout, reftimeOuts)
                                                    pChildCmd->setReferenceTimeOutStates(dTimeout);*/
                                                }
                                            }
                                            _IntermediateElemList.append(pElement);
                                            _hasChannelInControl=pElement->hasChannelInControl();

                                            bool isChannelSettingsEn = testConf->getChannelSelection();
                                            if(isChannelSettingsEn && _hasChannelInControl)
                                            {  
                                                QString sEngine = QString::fromStdString(testConf->getEngineSelection());
                                                QHash<QString,QString> engineVarMap;
                                                _VariableList.append(GTAUtil::getChannelSelectionSignals(sEngine,engineVarMap));
                                            }

                                            pTitleBasedElement->updateCommandInstanceName(pCallProcs->getInstanceName());
                                            _IntermediateElemList.append(pTitleBasedElement);

                                        }
                                    }


                                }
                                MakeState(cmdList,pNextCmdNode,pCmdNode,true);
                            }
                            else //if in parallel
                            {
                                pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                                QList<GTACommandBase*> cmdList;//calls will be appended to this
                                QList<GTACallProcItem> callItems = pCallProcs->getCallProcedures();
                                int i=0;
                                GTAElement* interMediateElem = new GTAElement(GTAElement::CUSTOM);
                                foreach(GTACallProcItem callItem, callItems)
                                {
                                    GTAActionCall* pCall = new GTAActionCall(ACT_CALL,COM_CALL_PROC,callItem._elementName);
                                    pCall->setTagValue(callItem._tagValue);
                                    
                                    interMediateElem->insertCommand(pCall,i,false);
                                    i++;
                                    cmdList.append(pCall);
                                    pCall->setIsParallelInSCXML(true);
                                    pCall->setRefrenceFileUUID(callItem._UUID);
                                }


                                MakeState(cmdList,pNextCmdNode,pCmdNode,true);
                                //pTitleBasedElement->updateCommandInstanceName(QString());

                                //interMediateElem->insertCommand(pnvalidCmdForDummyState,interMediateElem->getAllChildrenCount(),false);
                                interMediateElem->updateCommandInstanceName(pCallProcs->getInstanceName());

                                _IntermediateElemList.append(interMediateElem);

                            }
                            
                            
                        }
                    }
                }
                else if(actionName == ACT_ONECLICK)
                {
                    if(complementName == COM_ONECLICK_TITLE)
                    {
                        GTAActionOneClickParentTitle * pActionCmd = dynamic_cast<GTAActionOneClickParentTitle*>((GTAActionBase*)pCmd);
                        if(pActionCmd != NULL )
                        {
                            pCmdNode = new GTACmdNode(pCmd,"");
                            pCmdNode->IsWhileLoop = false;
                            QList<GTACommandBase*> childList = pActionCmd->getChildren();
                            QList<GTACommandBase*> commandList;
                            for(int i = 0; i < childList.count(); i++)
                            {
                                commandList.append(childList.at(i));
                            }
                            _oneClickCmdList.append(pActionCmd);
                            GTACmdNode * pNextCmdNode = NULL;
                            MakeState(commandList,pNextCmdNode,pCmdNode,true);
                        }
                        
                    }
                    else if(complementName == COM_ONECLICK_ENV)
                    {
                        pCmdNode = new GTACmdNode(pCmd,"");
                        GTAOneClick *pOneClickCmd = dynamic_cast<GTAOneClick *>(pCmd);
                        bool isStartEnv = pOneClickCmd->getIsStartEnv();
                        bool isStopAlto = pOneClickCmd->getIsStopAlto();
                        bool isstopTestconfig = pOneClickCmd->getIsStopTestConfig();
                        if(isStartEnv)
                            _oneClickCmdList.append(pCmd);
                        else
                        {
                            if(isStopAlto && ! isstopTestconfig)
                            {
                                //_oneClickStopCmdList.append(pCmd);
                                _oneClickCmdList.append(pCmd);
                            }
                            else if(isstopTestconfig && !isStopAlto)
                            {

                                _oneClickCmdList.append(pCmd);
                                //_oneClickStopCmdList.append(pCmd);
                            }
                            else if(isStopAlto && isstopTestconfig)
                            {
                                GTAOneClick *pOneClick1  = new GTAOneClick(*pOneClickCmd);

                                pOneClick1->setIsStopAlto(false);
                                pOneClick1->setIsStopTestConfig(true);
                                pOneClick1->setParent(pOneClickCmd->getParent());
                                _oneClickCmdList.append(pOneClick1);
                                //_oneClickStopCmdList.append(pOneClick1);
                                GTAOneClick *pOneClick2  = new GTAOneClick(*pOneClickCmd);
                                pOneClick2->setIsStopAlto(true);
                                pOneClick2->setIsStopTestConfig(false);
                                pOneClick2->setParent(pOneClickCmd->getParent());
                                _oneClickCmdList.append(pOneClick2);
                                //_oneClickStopCmdList.append(pOneClick2);
                            }
                        }

                    }
                    else if(complementName == COM_ONECLICK_PINPROG)
                    {
                        pCmdNode = new GTACmdNode(pCmd,"");
                        //_oneClickPPCCmdList.append(pCmd);
                        GTAActionOneClickPPC *ppcCmd = dynamic_cast<GTAActionOneClickPPC *>(pCmd);
                        bool isStart = ppcCmd->getIsStart();
                        if(isStart)
                            _oneClickCmdList.append(pCmd);
                        else
                        {
                            //_oneClickStopCmdList.append(pCmd);
                            _oneClickCmdList.append(pCmd);
                        }

                    }
                    else if(complementName == COM_ONECLICK_APP)
                    {
                        pCmdNode = new GTACmdNode(pCmd,"");
                        //_oneClickLaunchAppCmdList.append(pCmd);
                        GTAOneClickLaunchApplication *pLaCmd = dynamic_cast<GTAOneClickLaunchApplication *>(pCmd);
                        QList<GTAOneClickLaunchApplicationItemStruct *> laList = pLaCmd->getLaStruct();

                        if(!laList.isEmpty())
                        {
                            for(int laIdx=0;laIdx<laList.count();laIdx++)
                            {
                                GTAOneClickLaunchApplicationItemStruct *pTmpLaStruct = laList.at(laIdx);

                                QStringList options =  pTmpLaStruct->getOptions();
                                bool isKillMode = pTmpLaStruct->getKillMode();
                                if(isKillMode == false)
                                {
                                    for(int optIdx=0;optIdx<options.count();optIdx++)
                                    {
                                        QString optionName = options.at(optIdx);

                                        GTAOneClickLaunchApplicationItemStruct *pLaStruct = new GTAOneClickLaunchApplicationItemStruct();
                                        pLaStruct->setAppItem(pTmpLaStruct->getAppItem());
                                        pLaStruct->setAppName(pTmpLaStruct->getAppName());
                                        pLaStruct->setConfigFile(pTmpLaStruct->getConfigFile());
                                        pLaStruct->setTestRig(pTmpLaStruct->getTestRig());
                                        pLaStruct->setToolId(pTmpLaStruct->getToolId());
                                        pLaStruct->setToolType(pTmpLaStruct->getToolType());
                                        pLaStruct->insertOption(optionName,pTmpLaStruct->getValue(optionName));
                                        pLaStruct->insertOptionType(optionName,pTmpLaStruct->getType(optionName));
                                        pLaStruct->insertValArgs(optionName,pTmpLaStruct->getArgs(optionName));
                                        pLaStruct->setKillMode(isKillMode);
                                        GTAOneClickLaunchApplication *pLaCmd1 = new GTAOneClickLaunchApplication();
                                        pLaCmd1->insertLAStruct(pLaStruct);
                                        pLaCmd1->setAction(actionName);
                                        pLaCmd1->setActionOnFail(pLaCmd->getActionOnFail());
                                        pLaCmd1->setComment(pLaCmd->getComment());
                                        pLaCmd1->setInstanceName(pLaCmd->getInstanceName());
                                        pLaCmd1->setParent(pLaCmd->getParent());
                                        _oneClickCmdList.append(pLaCmd1);
                                    }
                                    if(options.isEmpty())
                                    {
                                        GTAOneClickLaunchApplicationItemStruct *pLaStruct = new GTAOneClickLaunchApplicationItemStruct();
                                        pLaStruct->setAppItem(pTmpLaStruct->getAppItem());
                                        pLaStruct->setAppName(pTmpLaStruct->getAppName());
                                        pLaStruct->setConfigFile(pTmpLaStruct->getConfigFile());
                                        pLaStruct->setTestRig(pTmpLaStruct->getTestRig());
                                        pLaStruct->setToolId(pTmpLaStruct->getToolId());
                                        pLaStruct->setToolType(pTmpLaStruct->getToolType());
                                        pLaStruct->setKillMode(isKillMode);
                                        GTAOneClickLaunchApplication *pLaCmd1 = new GTAOneClickLaunchApplication();
                                        pLaCmd1->insertLAStruct(pLaStruct);
                                        pLaCmd1->setAction(actionName);
                                        pLaCmd1->setActionOnFail(pLaCmd->getActionOnFail());
                                        pLaCmd1->setComment(pLaCmd->getComment());
                                        pLaCmd1->setInstanceName(pLaCmd->getInstanceName());
                                        pLaCmd1->setParent(pLaCmd->getParent());
                                        _oneClickCmdList.append(pLaCmd1);
                                    }
                                }
                                else
                                {
                                    if(options.isEmpty())
                                    {
                                        GTAOneClickLaunchApplicationItemStruct *pLaStruct = new GTAOneClickLaunchApplicationItemStruct();
                                        pLaStruct->setAppItem(pTmpLaStruct->getAppItem());
                                        pLaStruct->setAppName(pTmpLaStruct->getAppName());
                                        pLaStruct->setConfigFile(pTmpLaStruct->getConfigFile());
                                        pLaStruct->setTestRig(pTmpLaStruct->getTestRig());
                                        pLaStruct->setToolId(pTmpLaStruct->getToolId());
                                        pLaStruct->setToolType(pTmpLaStruct->getToolType());
                                        pLaStruct->setKillMode(isKillMode);
                                        GTAOneClickLaunchApplication *pLaCmd1 = new GTAOneClickLaunchApplication();
                                        pLaCmd1->insertLAStruct(pLaStruct);
                                        pLaCmd1->setAction(actionName);
                                        pLaCmd1->setActionOnFail(pLaCmd->getActionOnFail());
                                        pLaCmd1->setComment(pLaCmd->getComment());
                                        pLaCmd1->setInstanceName(pLaCmd->getInstanceName());
                                        pLaCmd1->setParent(pLaCmd->getParent());
                                        //_oneClickStopCmdList.append(pLaCmd1);
                                        _oneClickCmdList.append(pLaCmd1);
                                    }
                                    else
                                    {
                                        for(int optIdx=0;optIdx<options.count();optIdx++)
                                        {
                                            QString optionName = options.at(optIdx);

                                            GTAOneClickLaunchApplicationItemStruct *pLaStruct = new GTAOneClickLaunchApplicationItemStruct();
                                            pLaStruct->setAppItem(pTmpLaStruct->getAppItem());
                                            pLaStruct->setAppName(pTmpLaStruct->getAppName());
                                            pLaStruct->setConfigFile(pTmpLaStruct->getConfigFile());
                                            pLaStruct->setTestRig(pTmpLaStruct->getTestRig());
                                            pLaStruct->setToolId(pTmpLaStruct->getToolId());
                                            pLaStruct->setToolType(pTmpLaStruct->getToolType());
                                            pLaStruct->insertOption(optionName,pTmpLaStruct->getValue(optionName));
                                            pLaStruct->insertOptionType(optionName,pTmpLaStruct->getType(optionName));
                                            pLaStruct->insertValArgs(optionName,pTmpLaStruct->getArgs(optionName));
                                            pLaStruct->setKillMode(isKillMode);
                                            GTAOneClickLaunchApplication *pLaCmd1 = new GTAOneClickLaunchApplication();
                                            pLaCmd1->insertLAStruct(pLaStruct);
                                            pLaCmd1->setAction(actionName);
                                            pLaCmd1->setActionOnFail(pLaCmd->getActionOnFail());
                                            pLaCmd1->setComment(pLaCmd->getComment());
                                            pLaCmd1->setInstanceName(pLaCmd->getInstanceName());
                                            pLaCmd1->setParent(pLaCmd->getParent());
                                            //_oneClickStopCmdList.append(pLaCmd1);
                                            _oneClickCmdList.append(pLaCmd1);
                                        }
                                    }

                                }
                            }
                        }


                    }
                }
                else if(actionName == ACT_IRT)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }

                

                else if(actionName == ACT_ONECLICK_SET)
                {
                    pCmdNode = new GTACmdNode(pCmd,"");
                    //_oneClickPPCCmdList.append(pCmd);
                    // GTAActionOneClickSetList *ppcCmd = dynamic_cast<GTAActionOneClickSetList *>(pCmd);
                    _oneClickCmdList.append(pCmd);
                    

                }
                else if(actionName == ACT_SET)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_INVALID)
                {
                    GTAInvalidCommand *pinv = dynamic_cast<GTAInvalidCommand *>(pCmd);
                    if(pinv && pinv->getStateExists()==true)
                        pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                /*   else if(actionName == ACT_PRINT_TABLE)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }*/
                else if(actionName == ACT_PRINT)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                } 
                else if(actionName == ACT_FUNC_RETURN)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_ROBO_ARM)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_WAIT)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_CALL)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                    pCmdNode->IsWhileLoop=false;
                    GTAAppController* pAppCtrl = GTAAppController::getGTAAppController();
                    if(pAppCtrl != NULL)
                    {
                        GTAActionCall * pCallCmd = dynamic_cast<GTAActionCall*>((GTAActionBase*)pCmd);
                        if(pCallCmd != NULL)
                        {
                            QString elementName = pCallCmd->getElement();
                            QString docPath = pAppCtrl->getGTADataDirectory();
                            QString docFilePath = QDir::cleanPath(QString("%1/%2").arg(docPath,elementName));


                            //                            QRegExp exp("FSII-");

                            //                            if(elementName.contains(exp))
                            //                            {
                            //                                QHash<QString,QString> FSIINameMap = pAppCtrl->getFSIINameMap();
                            //                                QString explorerName = FSIINameMap.value(elementName);
                            //                                if(!explorerName.isEmpty())
                            //                                    docFilePath = QDir::cleanPath(QString("%1/%2").arg(docPath,explorerName));
                            //                            }
                            GTAElement * pElement =NULL;
                            GTAHeader* pHdr = NULL;


                            bool rc= pAppCtrl->getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pElement,true);


                            if(pElement)
                                pHdr = pElement->getHeader();

                            if (rc && pElement!=NULL)
                            {
                                GTAElement * pTitleBasedElement =NULL;
                                pElement->createActionElementGroupedByTitle(pTitleBasedElement);

                                if(rc && pTitleBasedElement != NULL)
                                {
                                    pTitleBasedElement->setName(pElement->getName());
                                    QMap<QString,QString> mapTagValue = pCallCmd->getTagValue();
                                    if (!mapTagValue.isEmpty())
                                    {
                                        pTitleBasedElement->replaceTag(mapTagValue);
                                    }
                                    QString callTimeOut,callTimeOutUnit;
                                    pCallCmd->getTimeOut(callTimeOut,callTimeOutUnit);
                                    //pTitleBasedElement->addtimeOut(callTimeOut.toDouble(),callTimeOutUnit);

                                    QList<GTACommandBase*> cmdList;
                                    int nbOfCommand = pTitleBasedElement->getDirectChildrenCount();
                                    for(int x= 0; x< nbOfCommand; x++)
                                    {
                                        GTACommandBase * pChildCmd = NULL;
                                        bool rc =  pTitleBasedElement->getDirectChildren(x,pChildCmd);
                                        if(rc && pChildCmd != NULL)
                                        {
                                            cmdList.append(pChildCmd);
                                            
                                            /*foreach(double dTimeout, reftimeOuts)
                                                pChildCmd->setReferenceTimeOutStates(dTimeout);*/
                                        }
                                    }
                                    //pTitleBasedElement->updateCommandInstanceName(QString());
                                    pTitleBasedElement->updateCommandInstanceName(pCallCmd->getInstanceName());
                                    GTACmdNode * pNextCmdNode = NULL;

                                    if(pCallCmd->isParallelInSCXML())
                                    {
                                        GTAInvalidCommand* pnvalidCmdForDummyState= new GTAInvalidCommand;
                                        pnvalidCmdForDummyState->setStateExists(true);
                                        cmdList.append(pnvalidCmdForDummyState); 
                                        pElement->insertCommand(pnvalidCmdForDummyState,pElement->getAllChildrenCount(),false);
                                    }
                                    /*double dCallTimeOut = pCallCmd->getTimeoutInMilliseconds();
                                    double dElementTimeOut = pElement->getTotalTimeOutInMs();
                                    pCallCmd->setTimeOut(QString::number((dCallTimeOut+dElementTimeOut)/1000.0),ACT_TIMEOUT_UNIT_SEC);*/

                                    MakeState(cmdList,pNextCmdNode,pCmdNode,true);
                                    _hasChannelInControl=pElement->hasChannelInControl();
                                    
                                    bool isChannelSettingsEn = testConf->getChannelSelection();
                                    if (isChannelSettingsEn && _hasChannelInControl)
                                    {
                                        QString sEngine = QString::fromStdString(testConf->getEngineSelection());
                                        QHash<QString,QString> engineVarMap;
                                        _VariableList.append(GTAUtil::getChannelSelectionSignals(sEngine,engineVarMap));
                                    }
                                    _IntermediateElemList.append(pElement);
                                    _IntermediateElemList.append(pTitleBasedElement);

                                }
                            }
                        }
                    }
                }
                else if(actionName == ACT_FCTL)
                {
                    bool rc =false;
                    pCmdNode = new GTACmdNode(pCmd,getCurrentStatNumber());
                    GTAAppController* pAppCtrl = GTAAppController::getGTAAppController();
                    GTAElement *pTemplateElem = NULL;
                    if(pAppCtrl != NULL)
                    {
                        GTAActionFTAKinematicMultiOutput * pFctlKineMultiOutCmd = dynamic_cast<GTAActionFTAKinematicMultiOutput*>((GTAActionBase*)pCmd);
                        if(pFctlKineMultiOutCmd != NULL)
                        {
                            QString templName = QString("%1_%2").arg(pFctlKineMultiOutCmd->getAction(),pFctlKineMultiOutCmd->getComplement());
                            
                            rc = pAppCtrl->loadTemplateFile(templName,pFctlKineMultiOutCmd,pTemplateElem );
                            if(pTemplateElem != NULL)
                            {
                                GTAElement * pTitleBasedElement =NULL;

                                pTemplateElem->createActionElementGroupedByTitle(pTitleBasedElement);

                                if(rc && pTitleBasedElement != NULL)
                                {
                                    pTitleBasedElement->setName(pTemplateElem->getName());
                                    QList<GTACommandBase*> cmdList;
                                    int nbOfCommand = pTitleBasedElement->getDirectChildrenCount();
                                    for(int x= 0; x< nbOfCommand; x++)
                                    {
                                        GTACommandBase * pChildCmd = NULL;
                                        rc =  pTitleBasedElement->getDirectChildren(x,pChildCmd);
                                        if(rc && pChildCmd != NULL)
                                            cmdList.append(pChildCmd);
                                    }
                                    //pTitleBasedElement->updateCommandInstanceName(QString());
                                    pTitleBasedElement->updateCommandInstanceName(pFctlKineMultiOutCmd->getInstanceName());
                                    GTACmdNode * pNextCmdNode = NULL;
                                    MakeState(cmdList,pNextCmdNode,pCmdNode,false);
                                    _IntermediateElemList.append(pTemplateElem);
                                    _IntermediateElemList.append(pTitleBasedElement);
                                }
                            }
                        }
                    }
                }
                else if(actionName == ACT_REL)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_GEN_FUNC)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
                else if(actionName == ACT_FAILURE)
                {
                    pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                }
            }
            else if (cmdType==GTAActionBase::CHECK)
            {
                GTACheckBase * pChkCmd = dynamic_cast<GTACheckBase*>((GTACommandBase*)pCmd);
                if (NULL!=pChkCmd)
                {
                    GTACheckBase::CheckType chkType = pChkCmd->getCheckType();

                    if (chkType==GTACheckBase::VALUE)
                    {
                        //rc = false;
                        pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                    }
                    else if (chkType==GTACheckBase::FWC_WARNING)
                    {
                        //rc = false;
                        pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                    }
                    else if (chkType==GTACheckBase::BITE_MESSAGE)
                    {
                        //rc = false;
                        pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                    }
                    else if (chkType == GTACheckBase::AUDIO_ALARM)
                    {
                        pCmdNode = new GTACmdNode(pCmd,getNextStateNumber());
                    }
                }

            }

            if(pCmdNode != NULL )
            {

                pCmdNode->PreviousNode = pPrevNode;

                if(pPrevNode != NULL)
                    pPrevNode->NextNode = pCmdNode;

                if(ipParent != NULL)
                {

                    pCmdNode->Parent = ipParent;
                    QList<GTACmdNode*> * listOfChild = NULL;
                    if(ipParent->Children.contains(IsCondition))
                        listOfChild = ipParent->Children.value(IsCondition);
                    else
                    {
                        listOfChild = new QList<GTACmdNode*>();
                        ipParent->Children.insert(IsCondition,listOfChild);
                    }
                    if(listOfChild != NULL)
                        listOfChild->append(pCmdNode);
                }

                getLastNode(pCmdNode,pPrevNode);
                //pPrevNode = pCmdNode;

                if(ipNode == NULL)
                    ipNode = pCmdNode;

            }
        }
    }
}
/**
* This function scan the ipNode to get the last node, the last node is retrieve from sibling relationship, parent-child relationship is ignored
* @ipNode: The node to be scan for last node
* @opLastNode: the last node to be retrieved from ipNode
* return true if found , otherwise false
*/
bool GTAExportScxml::getLastNode(GTACmdNode * ipNode, GTACmdNode *& opLastNode)
{
    bool rc= false;
    if(ipNode != NULL)
    {
        GTACmdNode * pNextNode = ipNode;
        while(pNextNode != NULL)
        {
            opLastNode = pNextNode;
            rc = true;
            pNextNode = pNextNode->NextNode;
        }
    }
    return rc;
}
bool GTAExportScxml::openScxmlTemplateFile(const QString & iTemplateFileName, QDomDocument & oDomDoc)
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

/**
  * This function open the xml file and import all the node in provided xml document context
  * iDocInContext: xml document context where the xml file nodes are imported
  * oRootElem : The root element in iDocInContext context
  * return true if successfully xml file imported in iDocInContext document context otherwise false;
  */

bool GTAExportScxml::getCloneRootElement(const QString & iTemplateFileName,QDomDocument & iDocInContext, QDomElement & oRootElem )
{
    QDomDocument oDomDoc;
    QString templateFile = iTemplateFileName;
    bool rc = openScxmlTemplateFile(templateFile,oDomDoc);
    if(rc && (!oDomDoc.isNull()))
    {
        QDomElement rootElem = oDomDoc.documentElement();
        if(! rootElem.isNull())
        {
            oRootElem = iDocInContext.importNode(rootElem,true).toElement();
        }
        else rc = false;
    }

    if(oRootElem.isNull())
        rc = false;

    return rc;
}
/**
  * This function open the xml file corresponding to configuration time template and import all the node in provided xml document context
  * by replacing it with appropriate values
  * iDocInContext: xml document context where the xml file nodes are imported
  * oRootElem : The root element in iDocInContext context
  * return true if successfully xml file imported in iDocInContext document context otherwise false;
  */

/*iDomDoc,oConfTimeStateElem,conftimeInMiliSec,iStatId,confTimeTarget*/
bool GTAExportScxml::getConfigurationTimeNode(QDomDocument & iDocInContext,QDomElement& oRootElem , QString iStateID, const QString& confTime, const QString& iTargetID, QString& oConfTimeEventName)
{
    //TODO:config time implementation is to be changed hence this function will return true
    //return false;
    QDomDocument oDomDoc;
    bool isPirSCXMLConfg = TestConfig::getInstance()->getPIRInvokeForSCXML();
    bool rc = openScxmlTemplateFile(TIMEOUT_SCXML,oDomDoc);
    if(rc && (!oDomDoc.isNull()))
    {
        QDomElement rootElem = oDomDoc.documentElement();
        if(! rootElem.isNull())
        {
            oRootElem = iDocInContext.importNode(rootElem,true).toElement();
        }
        else rc = false;
    }

    double ConfTime = QString(confTime).remove("ms").toDouble();
    if(oRootElem.isNull()||! (ConfTime>0))
        rc = false;
    else
    {
        oRootElem.setAttribute(SXA_ID,iStateID);
        QDomNodeList sendNodes = oRootElem.elementsByTagName(SXN_SEND);
        if (!sendNodes.isEmpty())
        {
            oConfTimeEventName = QString("event_%1_timeout").arg(iStateID.replace('.','_'));
            sendNodes.at(0).toElement().setAttribute(SXA_EVENT,oConfTimeEventName);
            sendNodes.at(0).toElement().setAttribute(SXA_DELAY,confTime);
        }
        QDomNodeList transisitonNodes = oRootElem.elementsByTagName(SXN_TRANSITION);
        if (!transisitonNodes.isEmpty())
        {
            transisitonNodes.at(0).toElement().setAttribute(SXA_TARGET,iTargetID);
            //transisitonNodes.at(0).toElement().setAttribute(SXA_EVENT,QString("event_%1_timeout").arg(iStateID.replace('.','_')));
        }
        if(isPirSCXMLConfg)
        {
            QDomElement assignElem = iDocInContext.createElement(SXN_ASSIGN);
            assignElem.setAttribute(SXA_EXPR,"Date.now()");
            assignElem.setAttribute(SXA_LOCATION,QString("%1_Config_StartTime").arg(iStateID));
            QDomNodeList onEntryList = oRootElem.elementsByTagName(SXN_ONENTRY);
            if(!onEntryList.isEmpty())
            {
                onEntryList.at(0).toElement().appendChild(assignElem);
            }
        }
    }
    return rc;
}
/**
* This function open the xml file corresponding to configuration time template and import all the node in provided xml document context
* by replacing it with appropriate values
* iDocInContext: xml document context where the xml file nodes are imported
* oRootElem : The root element in iDocInContext context
* return true if successfully xml file imported in iDocInContext document context otherwise false;
*/

/*iDomDoc,oConfTimeStateElem,conftimeInMiliSec,iStatId,confTimeTarget*/
bool GTAExportScxml::getTimeOutNode(QDomDocument & iDocInContext,QDomElement& oRootElem , QString iStateID, const QString& confTime, const QString& iTargetID)
{
    QDomDocument oDomDoc;   
    bool isPirSCXMLConfg = TestConfig::getInstance()->getPIRInvokeForSCXML();
    bool rc = openScxmlTemplateFile(CONF_TIME_SCXML,oDomDoc);
    if(rc && (!oDomDoc.isNull()))
    {
        QDomElement rootElem = oDomDoc.documentElement();
        if(! rootElem.isNull())
        {
            oRootElem = iDocInContext.importNode(rootElem,true).toElement();
        }
        else rc = false;
    }

    if(oRootElem.isNull())
        rc = false;
    else
    {
        oRootElem.setAttribute(SXA_ID,iStateID);
        QDomNodeList sendNodes = oRootElem.elementsByTagName(SXN_SEND);
        if (!sendNodes.isEmpty())
        {
            sendNodes.at(0).toElement().setAttribute(SXA_EVENT,QString("%1_TimeOut_Event").arg(iStateID.replace('.','_')));
            sendNodes.at(0).toElement().setAttribute(SXA_DELAY,confTime);
        }
        QDomNodeList transisitonNodes = oRootElem.elementsByTagName(SXN_TRANSITION);
        if (!transisitonNodes.isEmpty())
        {
            transisitonNodes.at(0).toElement().setAttribute(SXA_TARGET,iTargetID);
            transisitonNodes.at(0).toElement().removeAttribute(SXA_EVENT);
        }
        if(isPirSCXMLConfg)
        {
            QDomElement assignElem = iDocInContext.createElement(SXN_ASSIGN);
            assignElem.setAttribute(SXA_EXPR,"Date.now()");
            assignElem.setAttribute(SXA_LOCATION,QString("%1_timeout_StartTime").arg(iStateID));
            QDomNodeList onEntryList = oRootElem.elementsByTagName(SXN_ONENTRY);
            if(!onEntryList.isEmpty())
            {
                onEntryList.at(0).toElement().appendChild(assignElem);
            }
        }
    }

    return rc;
}

/**
  * This function create final scxml using the root command node
  * ipRootNode: The root commmand node , it is first command of editor
  * iFilePath: Path where the scxml file is created
  * Return true, if the scxml successfully created otherwise false;
  */
bool GTAExportScxml::MakeStateChart(const GTACmdNode * ipRootNode, const QString &iFilePath,bool iHasUnsubscibeAtStart,bool iHasUnsubscibeAtEnd )
{
    bool rc = false;
    if(ipRootNode == NULL)
    {
        _LastError = "Error: There is not command in editor";
        return rc;
    }

    bool isStandaloneExecMode = TestConfig::getInstance()->getStandAloneExecMode();
    /*if (isStandaloneExecMode)
    {
        iHasUnsubscibeAtEnd = false;
        iHasUnsubscibeAtStart = false;
    }*/

    qSetGlobalQHashSeed(42); // set a fixed hash value

    QDomDocument scxmlDomDoc;
    QDomElement scxmlRootElem;
    QDomElement scxmlDataModelElem;
    QDomElement scxmlSubscribeElem;
    QDomElement scxmlParallelElem;
    QDomElement scxmlUnSubscribeElem;
    QDomElement scxmlInitEnvironmentElem;
    QDomElement scxmlEndEnvironmentElem;
    QDomElement scxmlSubsribeElemForInit;
    QDomElement scxmlUnSubsribeElemForInit;
    QDomElement firstOneClickSetStatElem;
    QDomElement scxmltInitStatElem ;
    QDomNode OneClickDummyNode;
    QList<QDomNode> failureInvokeNodeList;
    rc = openScxmlTemplateFile("main.scxml",scxmlDomDoc);
    if(rc && (!scxmlDomDoc.isNull()))
    {


        scxmlRootElem = scxmlDomDoc.documentElement();
        QDomNodeList listOfChild = scxmlRootElem.childNodes();
        for(int i = 0 ; i < listOfChild.count(); i++)
        {
            QDomElement domElem = listOfChild.item(i).toElement();
            if(! domElem.isNull())
            {
                QString nodeName =domElem.nodeName();
                QString attributeID = domElem.attribute(SXA_ID);
                if( nodeName== SXN_DATAMODEL)
                {
                    scxmlDataModelElem = domElem;
                }
                else if(nodeName == SXN_STATE &&  attributeID== "subscribe")
                {
                    scxmlSubscribeElem = domElem;
                    scxmlSubsribeElemForInit=domElem.cloneNode().toElement();
                }
                else if(nodeName == SXN_STATE && attributeID == "unsubscribe")
                {
                    
                    scxmlUnSubscribeElem = domElem;
                    if(iHasUnsubscibeAtEnd==false)
                    {
                        scxmlRootElem.removeChild(scxmlUnSubscribeElem);
                        scxmlUnSubscribeElem = QDomElement();
                    }
                    else
                    {
                        scxmlUnSubsribeElemForInit=domElem.cloneNode().toElement();
                    }
                    
                    
                }
                else if(nodeName == SXN_PARALLEL && attributeID == "Test_Procedure")
                {
                    scxmlParallelElem = domElem;
                    if(iHasUnsubscibeAtEnd==false)
                    {
                        QDomNodeList txnNodes = scxmlParallelElem.elementsByTagName(SXN_TRANSITION);
                        for (int i=0;i<txnNodes.size();i++)
                        {
                            QDomElement txnNode =txnNodes.at(i).toElement();
                            txnNode.setAttribute(SXA_TARGET,"End_Environment");
                        }
                        
                    }

                }
                else if(nodeName == SXN_PARALLEL && attributeID  == "Init_Environment")
                {
                    scxmlInitEnvironmentElem = domElem;
                }
                else if(nodeName == SXN_STATE && attributeID == "End_Environment")
                {
                    scxmlEndEnvironmentElem = domElem;
                }
                else if(nodeName == SXN_STATE && attributeID == "Init")
                {
                    scxmltInitStatElem = domElem;
                }
            }
        }
        QDomNodeList InitEnvStateNodeList = scxmlInitEnvironmentElem.elementsByTagName(SXN_STATE);
        for(int i=0;i<InitEnvStateNodeList.count();i++)
        {

            QDomNode tempNode = InitEnvStateNodeList.at(i);
            QDomNode idNode = tempNode.attributes().namedItem(SXA_ID);
            if(!idNode.isNull())
            {
                QString idName = idNode.nodeValue();
                if(idName == "oneclick_finished")
                {
                    OneClickDummyNode = tempNode;
                }
            }
        }
        QDomElement mainStatElem;
        QDomElement logStatElem;
        QDomElement ChannelInControlElem;
        //        if(_hasChannelInControl)
        //            insertChannelInControlState(scxmlDomDoc,ChannelInControlElem);
        if(!scxmlParallelElem.isNull())
        {
            QDomNodeList statNodeList = scxmlParallelElem.childNodes();
            for(int x = 0 ; x < statNodeList.count(); x++)
            {
                QDomElement stateElem =  statNodeList.item(x).toElement();
                if(! stateElem.isNull() && stateElem.isElement() && stateElem.nodeName() == SXN_STATE)
                {
                    QString statId = stateElem.attribute(SXA_ID);
                    if(statId == "main")
                    {
                        mainStatElem = stateElem;

                    }
                    else if(statId == "logstate")
                    {
                        logStatElem = stateElem;

                    }
                }
            }
            if(_hasChannelInControl)
                scxmlParallelElem.appendChild(ChannelInControlElem);
        }


        MakeScxml(ipRootNode,scxmlDomDoc,mainStatElem,logStatElem);

        //Create final node for main state
        QDomElement mainStatFinal = scxmlDomDoc.createElement(SXN_FINAL);
        mainStatElem.appendChild(mainStatFinal);
        mainStatFinal.setAttribute(SXA_ID,MAIN_FINAL_STATE);

        QDomElement mainStatFinalOnEntry =  scxmlDomDoc.createElement(SXN_ONENTRY);
        mainStatFinal.appendChild(mainStatFinalOnEntry);
        QDomElement mainStatFinalAssign = scxmlDomDoc.createElement(SXN_ASSIGN);
        mainStatFinalOnEntry.appendChild(mainStatFinalAssign);
        mainStatFinalAssign.setAttribute(SXA_LOCATION,QString(MAIN_FINAL_STATE)+"_done");
        mainStatFinalAssign.setAttribute(SXA_EXPR,QString("'%1'").arg(SX_FLAG_EXECUTED));

        //create log final
        QDomElement logStatFinal = scxmlDomDoc.createElement(SXN_FINAL);
        logStatElem.appendChild(logStatFinal);
        logStatFinal.setAttribute(SXA_ID,"log_final");
        //create final node for scxml
        QDomElement finalSateElem = scxmlDomDoc.createElement(SXN_FINAL);
        finalSateElem.setAttribute(SXA_ID,"final");
        scxmlRootElem.appendChild(finalSateElem);
    }
    else _LastError = QString("Error: Unable to open create file %1 ").arg(iFilePath);

    if(!scxmlInitEnvironmentElem.isNull())
    {
        // int globalOneClickCntr = 0;
        //if(!_oneClickPPCCmdList.isEmpty() || !_oneClickCmdList.isEmpty() || !_oneClickLaunchAppCmdList.isEmpty())
        if(!_oneClickCmdList.isEmpty())
        {
            QString tmplFile  = "Init_OneClick.scxml";
            QDomElement oElem;
            rc = getCloneRootElement(tmplFile,scxmlDomDoc,oElem);
            if(rc)
            {
                scxmlInitEnvironmentElem.appendChild(oElem);
                //now the state is made a parallel state, and parallel state does not has a initial attribute.
                //scxmlInitEnvironmentElem.setAttribute(SXA_INITIALSTATE,"WakeUpLauncher");


                QDomNodeList transitionsList = oElem.elementsByTagName(SXN_TRANSITION);
                if(!transitionsList.isEmpty())
                {
                    QDomNode trasitionNode = transitionsList.at(0);
                    setTranstion(trasitionNode.toElement(),"oneClick_s0","done.invoke.init_0","");
                }
            }
            else
            {
                //now the state is made a parallel state, and parallel state does not has a initial attribute.
                //scxmlInitEnvironmentElem.setAttribute(SXA_INITIALSTATE,"oneClick_s0");
            }
        }
        if(!scxmlEndEnvironmentElem.isNull())
        {
            if(!_oneClickStopCmdList.isEmpty())
            {

                GTACommandBase * pActionCmd = NULL;
                QString targetId;
                for(int i =0;i<_oneClickStopCmdList.count();i++)
                {
                    pActionCmd = _oneClickStopCmdList.at(i);

                    GTAOneClick * pActOCCmd = dynamic_cast<GTAOneClick*>(pActionCmd);
                    GTAActionOneClickPPC * pActPPCCmd = dynamic_cast<GTAActionOneClickPPC*>(pActionCmd);
                    GTAOneClickLaunchApplication * pActLACmd = dynamic_cast<GTAOneClickLaunchApplication*>(pActionCmd);

                    if(pActOCCmd != NULL)
                    {
                        if(i == _oneClickStopCmdList.count() - 1)
                        {
                            targetId = "EndEnv_Final";
                        }
                        else
                        {
                            targetId = QString("oneClickStop_s%1").arg(i+1);
                        }
                        rc = createActionOneClickSetEnvState(pActOCCmd,scxmlDomDoc,scxmlEndEnvironmentElem,QDomElement(),QString("oneClickStop_s%1").arg(i),targetId,QDomElement());
                    }
                    else if(pActPPCCmd != NULL)
                    {
                        if(i == _oneClickStopCmdList.count() - 1)
                        {
                            targetId = "EndEnv_Final";
                        }
                        else
                        {
                            targetId = QString("oneClickStop_s%1").arg(i+1);
                        }
                        rc = createActionOneClickPPCState(pActPPCCmd,scxmlDomDoc,scxmlEndEnvironmentElem,QDomElement(),QString("oneClickStop_s%1").arg(i),targetId,QDomElement());
                    }
                    else if(pActLACmd != NULL)
                    {
                        if(i == _oneClickStopCmdList.count() - 1)
                        {
                            targetId = "EndEnv_Final";
                        }
                        else
                        {
                            targetId = QString("oneClickStop_s%1").arg(i+1);
                        }
                        //scxmlRootElem.insertAfter()
                        rc = createActionOneClickLauncAppState(pActLACmd,scxmlDomDoc,scxmlEndEnvironmentElem,QDomElement(),QString("oneClickStop_s%1").arg(i),targetId,QDomElement());
                    }
                    else
                    {

                    }

                }
                scxmlEndEnvironmentElem.setAttribute("initial","oneClickStop_s0");
            }


        }
        //if(!_oneClickLaunchAppCmdList.isEmpty())
        //{
        //    GTACommandBase * pActionCmd = NULL;
        //    QString targetId;
        //    for(int i =0;i<_oneClickLaunchAppCmdList.count();i++)
        //    {
        //        pActionCmd = _oneClickLaunchAppCmdList.at(i);
        //        GTAOneClickLaunchApplication * pActCmd = dynamic_cast<GTAOneClickLaunchApplication*>(pActionCmd);
        //        if(pActCmd != NULL)
        //        {
        //            if((i == _oneClickLaunchAppCmdList.count() - 1) && _oneClickCmdList.isEmpty() && _oneClickPPCCmdList.isEmpty())
        //            {
        //                targetId = "InitEnv_Final";
        //            }
        //            else
        //            {
        //                globalOneClickCntr = i+1;
        //                targetId = QString("oneClick_s%1").arg(i+1);
        //            }

        //            rc = createActionOneClickLauncAppState(pActCmd,scxmlDomDoc,scxmlInitEnvironmentElem,QDomElement(),QString("oneClick_s%1").arg(i),targetId);
        //        }
        //    }
        //}
        //if(!_oneClickPPCCmdList.isEmpty())
        //{
        //    GTACommandBase * pActionCmd = NULL;
        //    QString targetId;
        //    for(int i =0;i<_oneClickPPCCmdList.count();i++)
        //    {
        //        pActionCmd = _oneClickPPCCmdList.at(i);
        //        GTAActionOneClickPPC * pActCmd = dynamic_cast<GTAActionOneClickPPC*>(pActionCmd);
        //        if(pActCmd != NULL)
        //        {
        //            if((i == _oneClickPPCCmdList.count() - 1) && _oneClickCmdList.isEmpty())
        //            {
        //                targetId = "InitEnv_Final";
        //            }
        //            else
        //            {
        //                globalOneClickCntr = i+1;
        //                targetId = QString("oneClick_s%1").arg(i+1);
        //            }

        //            rc = createActionOneClickPPCState(pActCmd,scxmlDomDoc,scxmlInitEnvironmentElem,QDomElement(),QString("oneClick_s%1").arg(i),targetId);
        //        }
        //    }
        //}
        int oneClickCount =0;
        if(!_oneClickCmdList.isEmpty())
        {
            GTAScxmlDataModel objDM;
            objDM.ID = "OneClickCounter";
            objDM.Expr="0";
            _DataModelList.insert("OneClickCounter",objDM);
            GTACommandBase * pActionCmd = NULL;
            QString targetId;
            QString TitleFinalId = "oneclick_finished" ;
            QDomElement TitleRootElem = scxmlInitEnvironmentElem;
            for(int i =0;i<_oneClickCmdList.count();i++)
            {
                pActionCmd = _oneClickCmdList.at(i);
                QDomElement oneClickStateElem;
                
                GTAOneClick * pActOCCmd = dynamic_cast<GTAOneClick*>(pActionCmd);
                GTAActionOneClickPPC * pActPPCCmd = dynamic_cast<GTAActionOneClickPPC*>(pActionCmd);
                GTAOneClickLaunchApplication * pActLACmd = dynamic_cast<GTAOneClickLaunchApplication*>(pActionCmd);
                GTAActionOneClickSetList * pActSetCmd = dynamic_cast<GTAActionOneClickSetList*>(pActionCmd);
                GTAActionOneClickParentTitle * pActTitleCmd = dynamic_cast<GTAActionOneClickParentTitle*>(pActionCmd);
                if(pActTitleCmd != NULL )
                {
                    
                    if(i == _oneClickCmdList.count() - 1)
                    {
                        targetId = "InitEnv_Final";
                    }
                    else
                    {
                        targetId = QString("oneClick_s%1").arg(i+1);
                    }
                    rc = createActionOneClickTitleState(pActTitleCmd,scxmlDomDoc,scxmlInitEnvironmentElem,QString("oneClick_s%1").arg(i),targetId,oneClickStateElem);
                    if(!oneClickStateElem.isNull())
                    {
                        QDomNode finalNode = oneClickStateElem.toElement().namedItem(SXN_FINAL);
                        if(!finalNode.isNull())
                        {
                            QDomNode idNode = finalNode.attributes().namedItem(SXA_ID);
                            if(!idNode.isNull())
                                TitleFinalId = idNode.nodeValue();
                        }
                    }
                    TitleRootElem=oneClickStateElem;
                    //  scxmlInitEnvironmentElem.appendChild(TitleRootElem);
                }
                if(pActOCCmd != NULL)
                {

                    //GTACommandBase * pParentCmd = pActOCCmd->getParent();
                    //if(pParentCmd  != NULL)
                    //{
                    //    QList<GTACommandBase * > oneClickChilren = pParentCmd->getChildrens();
                    //    GTACommandBase *pLastChild = oneClickChilren.last();
                    //    if(pLastChild!= NULL)
                    //    {
                    //        if(pLastChild == pActOCCmd)
                    //        {
                    //            targetId = TitleFinalId;
                    //        }
                    //        else
                    //        {
                    //            targetId = QString("oneClick_s%1").arg(i+1);
                    //        }
                    //    }
                    //    
                    //}
                    //else
                    //{
                    //    targetId = "DummyState";
                    //}
                    if(i<_oneClickCmdList.count()  -1)
                    {
                        GTACommandBase * pParentCmd = _oneClickCmdList.at(i+1);
                        if(pParentCmd->isOneClickTitle())
                        {
                            //targetId = TitleFinalId;
                            targetId = "oneclick_finished";
                        }
                        else
                        {
                            targetId = QString("oneClick_s%1").arg(i+1);
                        }
                    }
                    else
                    {
                        targetId = "oneclick_finished";
                        //targetId = TitleFinalId;
                    }


                    /*                 if(i == _oneClickCmdList.count() - 1)
                    {
                        targetId = "InitEnv_Final";
                    }
                    else
                    {
                        targetId = QString("oneClick_s%1").arg(i+1);
                    }*/
                    rc = createActionOneClickSetEnvState(pActOCCmd,scxmlDomDoc,scxmlInitEnvironmentElem,QDomElement(),QString("oneClick_s%1").arg(i),targetId,oneClickStateElem);
                    TitleRootElem.appendChild(oneClickStateElem);
                    ++oneClickCount;
                }
                else if(pActPPCCmd != NULL)
                {

                    //GTACommandBase * pParentCmd = pActPPCCmd->getParent();
                    //if(pParentCmd  != NULL)
                    //{
                    //    QList<GTACommandBase * > oneClickChilren = pParentCmd->getChildrens();
                    //    GTACommandBase *pLastChild = oneClickChilren.last();
                    //    if(pLastChild!= NULL)
                    //    {
                    //        if(pLastChild == pActPPCCmd)
                    //        {
                    //            targetId = TitleFinalId;
                    //        }
                    //        else
                    //        {
                    //            targetId = QString("oneClick_s%1").arg(i+1);
                    //        }
                    //    }

                    //}
                    //else
                    //{
                    //    targetId = "DummyState";
                    //}
                    if(i<_oneClickCmdList.count()  -1)
                    {
                        GTACommandBase * pParentCmd = _oneClickCmdList.at(i+1);
                        if(pParentCmd->isOneClickTitle())
                        {
                            //targetId = TitleFinalId;
                            targetId = "oneclick_finished";
                        }
                        else
                        {
                            targetId = QString("oneClick_s%1").arg(i+1);
                        }
                    }
                    else
                    {
                        targetId = "oneclick_finished";
                        //targetId = TitleFinalId;
                    }
                    //if(i == _oneClickCmdList.count() - 1)
                    //{
                    //    targetId = "InitEnv_Final";
                    //}
                    //else
                    //{
                    //    targetId = QString("oneClick_s%1").arg(i+1);
                    //}
                    rc = createActionOneClickPPCState(pActPPCCmd,scxmlDomDoc,scxmlInitEnvironmentElem,QDomElement(),QString("oneClick_s%1").arg(i),targetId,oneClickStateElem);
                    TitleRootElem.appendChild(oneClickStateElem);
                    ++oneClickCount;
                }
                else if(pActLACmd != NULL)
                {
                    
                    if(i<_oneClickCmdList.count()  -1)
                    {
                        GTACommandBase * pParentCmd = _oneClickCmdList.at(i+1);
                        if(pParentCmd->isOneClickTitle())
                        {
                            targetId = "oneclick_finished";
                            //targetId = TitleFinalId;
                        }
                        else
                        {
                            targetId = QString("oneClick_s%1").arg(i+1);
                        }
                    }
                    else
                    {
                        //targetId = TitleFinalId;
                        targetId = "oneclick_finished";
                    }




                    //if(pParentCmd  != NULL)
                    //{
                    //    QList<GTACommandBase * > oneClickChilren = pParentCmd->getChildrens();
                    //    GTACommandBase *pLastChild = oneClickChilren.last();
                    //    if(pLastChild!= NULL)
                    //    {
                    //        if(pLastChild == pActLACmd)
                    //        {
                    //            targetId = TitleFinalId;
                    //        }
                    //        else
                    //        {
                    //            targetId = QString("oneClick_s%1").arg(i+1);
                    //        }
                    //    }
                    //}
                    //else
                    //{
                    //    targetId = "DummyState";
                    //}

                    //if(i == _oneClickCmdList.count() - 1)
                    //{
                    //    targetId = "InitEnv_Final";
                    //}
                    //else
                    //{
                    //    targetId = QString("oneClick_s%1").arg(i+1);
                    //}
                    rc = createActionOneClickLauncAppState(pActLACmd,scxmlDomDoc,scxmlInitEnvironmentElem,QDomElement(),QString("oneClick_s%1").arg(i),targetId,oneClickStateElem);
                    TitleRootElem.appendChild(oneClickStateElem);
                    ++oneClickCount;
                    
                }
                else if(pActSetCmd != NULL)
                {
                    if(i == _oneClickCmdList.count() - 1)
                    {
                        targetId = "InitEnv_Final";
                    }
                    else
                    {
                        targetId = QString("oneClick_s%1").arg(i+1);
                    }
                    QDomElement elem;
                    //rc = createActionOneClickSetState(pActSetCmd,scxmlDomDoc,scxmlInitEnvironmentElem,QString("oneClick_s%1").arg(i),targetId,elem);
                    if(firstOneClickSetStatElem.isNull())
                    {
                        firstOneClickSetStatElem = elem;
                        //scxmlRootElem.insertBefore(scxmlSubsribeElemForInit,elem);
                    }
                    ++oneClickCount;
                }
                else
                {

                }
            }
        }
        
        for(int i=0;i<_FailureCommandList.count();i++)
        {
            if(!i)
            {
                GTAScxmlDataModel objDM;
                objDM.ID = "failureCompleteFlag";
                objDM.Expr = "0";
                _DataModelList.insert("failureCompleteFlag",objDM);
            }

            GTAActionFailure * pFailCmd = dynamic_cast<GTAActionFailure *> (_FailureCommandList.at(i));
            if(pFailCmd!= NULL)
            {
                QString failName = pFailCmd->getFailureName();

                QRegExp rx("([~.:%!&;,*\\^?|/\\\\@#$:\\-\\+{}\\[\\]<>'\"\\s\\(\\)]+)");
                failName.replace(rx,"_");        
                failName.replace("\n","_");
                /////////////////

                QString fileName = QString("%1.scxml").arg(pFailCmd->getAction());
                QDomElement stateElem;
                rc = getCloneRootElement(fileName,scxmlDomDoc,stateElem);

                QDomElement failureInitInvokeElem = stateElem.elementsByTagName(SXN_INVOKE).at(0).toElement();
                QDomElement failureInvokeSubscribeElem = failureInitInvokeElem.cloneNode().toElement();

                
                QString failStateId = QString("%1_s%2").arg(failName,QString::number(i));
                QString failInvokeId = QString("%1_i%2").arg(failName,QString::number(i));
                QString nextFailureame = "";
                QString failStateTargetId = "";


                failureInitInvokeElem.setAttribute(SXA_ID,failInvokeId);
                
                if(_FailureCommandList.count() - 1 > i)
                {
                    GTAActionFailure * pNextFailCmd = dynamic_cast<GTAActionFailure *>(_FailureCommandList.at(i+1));
                    if(pNextFailCmd != NULL)
                    {
                        QString nextFailureame = pNextFailCmd->getFailureName();

                        QRegExp rx("([~.:%!&;,*\\^?|/\\\\@#$:\\-\\+{}\\[\\]<>'\"\\s\\(\\)]+)");
                        nextFailureame.replace(rx,"_");        
                        nextFailureame.replace("\n","_");
                    }
                    failStateTargetId = QString("%1_s%2").arg(nextFailureame,QString::number(i+1));
                }
                stateElem.setAttribute(SXA_ID,failStateId);
                if(i==0)
                {
                    scxmltInitStatElem.setAttribute(SXA_INITIALSTATE, failStateId);

                    QDomElement finalElem = scxmlDomDoc.createElement(SXN_FINAL);
                    finalElem.setAttribute(SXA_ID,"failure_init_final");

                    scxmltInitStatElem.appendChild(finalElem);
                }
                if (i==_FailureCommandList.size()-1)
                {
                    QString sInitiCondition = QString("failureCompleteFlag==%1").arg(QString::number(i+1));
                    scxmltInitStatElem.elementsByTagName(SXN_TRANSITION).at(0).toElement().setAttribute(SXA_CONDITION,sInitiCondition);
                }
                QString retunExpr = QString("%1_%2_failure").arg(failStateId,failName);

                QDomElement functionElem = stateElem.elementsByTagName(SXN_FUNCTION).at(0).toElement();
                functionElem.setAttribute(SXA_NAME,"Failure.load");


                QDomElement argElem1 = stateElem.elementsByTagName(SXN_ARGUMENT).at(0).toElement();
                //QDomElement argElem2= argElem1.cloneNode().toElement();

                argElem1.setAttribute(SXA_NAME,"failure_configuration");
                argElem1.setAttribute(SXA_TYPE,"struct");
                argElem1.setAttribute(SXA_VALUE,QString("failure_list[%1].failureConfiguration").arg(failName));

                //argElem1.setAttribute(SXA_NAME,"failure_identification");
                //argElem1.setAttribute(SXA_TYPE,"string");
                //argElem1.setAttribute(SXA_VALUE,QString("failure_list[%1].objectID").arg(failName));
                
                /*                argElem2.setAttribute(SXA_NAME,"failure_configuration");
                argElem2.setAttribute(SXA_TYPE,"struct");
                argElem2.setAttribute(SXA_VALUE,QString("failure_list[%1].failureConfiguration").arg(failName));  */  
                
                //functionElem.insertAfter(argElem2,argElem1);


                QDomElement retElem = stateElem.elementsByTagName(SXN_RETURNVALUE).at(0).toElement();

                retElem.setAttribute(SXA_NAME,retunExpr);

                QDomElement assignElem = stateElem.elementsByTagName(SXN_ASSIGN).at(0).toElement();
                if(!assignElem.isNull())
                {
                    assignElem.setAttribute(SXA_LOCATION,QString("failure_list[%1].status").arg(failName));
                    assignElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(retunExpr));
                }

                QDomElement TxElem = stateElem.elementsByTagName(SXN_TRANSITION).at(0).toElement();
                if(_FailureCommandList.count()-1>i)
                    TxElem.setAttribute(SXA_TARGET,failStateTargetId);
                else
                    TxElem.setAttribute(SXA_TARGET,"failure_init_final");

                TxElem.setAttribute(SXA_EVENT,"done.invoke._all");

                QDomElement assignElement = scxmlDomDoc.createElement(SXN_ASSIGN);
                assignElement.setAttribute(SXA_LOCATION,"failureCompleteFlag");
                assignElement.setAttribute(SXA_EXPR,QString::number(i+1));
                TxElem.appendChild(assignElement);

                scxmltInitStatElem.appendChild(stateElem);
                
                
                failureInvokeSubscribeElem.setAttribute(SXA_ID,QString("Subsribe_Failure_i%1").arg(QString::number(i)));

                QDomElement invokeFunctionElem = failureInvokeSubscribeElem.elementsByTagName(SXN_FUNCTION).at(0).toElement();
                invokeFunctionElem.setAttribute(SXA_NAME,"Module.createObject");
                QDomElement argInvokeElem1 =  invokeFunctionElem.elementsByTagName(SXN_ARGUMENT).at(0).toElement();

                argInvokeElem1.setAttribute(SXA_NAME,"failure_definition");
                argInvokeElem1.setAttribute(SXA_TYPE,"struct");
                argInvokeElem1.setAttribute(SXA_VALUE,QString("failure_list[%1].failureDefinition").arg(failName));

                QDomElement retElem1 = failureInvokeSubscribeElem.elementsByTagName(SXN_RETURNVALUE).at(0).toElement();

                retElem1.setAttribute(SXA_NAME,retunExpr);

                QDomElement assignInvokeElem = failureInvokeSubscribeElem.elementsByTagName(SXN_ASSIGN).at(0).toElement();
                if(!assignInvokeElem.isNull())
                {
                    assignInvokeElem.setAttribute(SXA_LOCATION,QString("failure_list[%1].objectId").arg(failName));
                    assignInvokeElem.setAttribute(SXA_EXPR,QString("_event.data.%1.objectId").arg(retunExpr));

                    QDomElement assignElem2 = assignInvokeElem.cloneNode().toElement();

                    assignElem2.setAttribute(SXA_LOCATION,QString("failure_list[%1].objectIdStructure").arg(failName));
                    assignElem2.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(retunExpr));

                    QDomElement finalzeElem = failureInvokeSubscribeElem.elementsByTagName(SXN_FINALIZE).at(0).toElement();
                    finalzeElem.insertAfter(assignElem2,assignInvokeElem);
                }
                failureInvokeNodeList.append(failureInvokeSubscribeElem);

            }
        }


        QDomElement transitElem = scxmlDomDoc.createElement(SXN_TRANSITION);
        QDomElement oElem;
        bool unsubscribeRc =getCloneRootElement("unsubscribe.scxml",scxmlDomDoc,oElem);
        if (unsubscribeRc && !oElem.isNull() && !scxmlSubscribeElem.isNull() && iHasUnsubscibeAtStart)
        {
            QString unSubScribeStateId = oElem.attribute(SXA_ID);
            if (unSubScribeStateId.isEmpty())
            {
                unSubScribeStateId = "initUnsubscribe";
                oElem.setAttribute(SXA_ID,unSubScribeStateId);
            }
            
            scxmlRootElem.insertBefore(oElem,scxmlSubscribeElem);
            QDomNodeList lstTxnElement = oElem.elementsByTagName(SXN_TRANSITION);
            QDomElement transitElem1;
            if (!lstTxnElement.isEmpty())
            {
                transitElem1=lstTxnElement.at(0).toElement();
                
            }
            else
            {
                transitElem1 = scxmlDomDoc.createElement(SXN_TRANSITION);
                oElem.appendChild(transitElem1);
                
            }
            transitElem1.setAttribute(SXA_STATUS,"success");
            setTranstion(transitElem1,"subscribe","done.invoke._all","Result_unsubscribe.Status == 'OK'");
            setTranstion(transitElem,unSubScribeStateId,"","");

            QDomElement transitElem2 = transitElem1.cloneNode().toElement();
            transitElem2.setAttribute(SXA_STATUS,"fail");
            setTranstion(transitElem2,"subscribe","done.invoke._all","Result_unsubscribe.Status != 'OK'");


            QString unSubId = scxmlUnSubsribeElemForInit.attribute(SXA_ID);
            QString subId= scxmlSubsribeElemForInit.attribute(SXA_ID);
            QDomElement txNode3 =scxmlDomDoc.createElement(SXN_TRANSITION);
            QDomElement timeoutSend = scxmlDomDoc.createElement(SXN_SEND);
            QString eventName = QString("%1_timeoutEvent").arg(unSubId);
            timeoutSend.setAttribute(SXA_EVENT,eventName);
            // to be changed based on configs
            TestConfig* testConf = TestConfig::getInstance();
            bool isSubscribeUnsubscribeTimeout = testConf->getSubscribeUnsubscribeTimeoutStatus();
            QString SubscribeUnsubscribeTimeout = QString::fromStdString(testConf->getSubscribeUnsubscribeTimeoutValue());
            bool isDoubleOk = false;
            double dTimeout = SubscribeUnsubscribeTimeout.toDouble(&isDoubleOk);
            if (isSubscribeUnsubscribeTimeout)
                timeoutSend.setAttribute(SXA_DELAY,QString("%1ms").arg(QString::number(dTimeout * 1000)));
            else
                timeoutSend.setAttribute(SXA_DELAY,"1200000ms");



            txNode3.setAttribute(SXA_TARGET,subId);
            txNode3.setAttribute(SXA_STATUS,"fail");
            txNode3.setAttribute(SXA_EVENT,eventName);

            QDomElement onEntry = scxmlDomDoc.createElement(SXN_ONENTRY);
            onEntry.appendChild(timeoutSend);
            oElem.appendChild(onEntry);
            oElem.appendChild(txNode3);

            oElem.appendChild(transitElem2);
        }
        else
        {  
            setTranstion(transitElem,"subscribe","","");
        }
        if(!_oneClickCmdList.isEmpty())
        {
            transitElem.setAttribute(SXA_CONDITION,QString("%1==%2").arg("OneClickCounter",QString::number(oneClickCount)));
            transitElem.setAttribute(SXA_EVENT,"Event_oneClick_");
        }
        if(!OneClickDummyNode.isNull())
        {
            OneClickDummyNode.appendChild(transitElem);
        }

        

        //QDomElement transitElem1 = scxmlDomDoc.createElement(SXN_TRANSITION);
        //scxmlEndEnvironmentElem.appendChild(transitElem1);
        //setTranstion(transitElem1,"final","","");
        

        QDomElement txOneClickParallelElem = scxmlDomDoc.createElement(SXN_TRANSITION); 
        if(iHasUnsubscibeAtStart == true)
            txOneClickParallelElem.setAttribute(SXA_TARGET,"initUnsubscribe");
        else
            txOneClickParallelElem.setAttribute(SXA_TARGET,"subscribe");
        scxmlInitEnvironmentElem.appendChild(txOneClickParallelElem);
        if(!_oneClickCmdList.isEmpty())// || !_oneClickPPCCmdList.isEmpty() || !_oneClickLaunchAppCmdList.isEmpty())
        {
            //now the state is made a parallel state, and parallel state does not has a final state.
            QDomElement InitEnvFinalElem = scxmlDomDoc.createElement(SXN_FINAL);
            InitEnvFinalElem.setAttribute(SXA_ID,"InitEnv_Final");
            //scxmlInitEnvironmentElem.appendChild(InitEnvFinalElem);
            txOneClickParallelElem.setAttribute(SXA_EVENT,"done.state.oneclick_finished");
            
            //<transition target="initUnsubscribe" event ="done.state.oneclick_finished"/>

        }
        if(!_oneClickStopCmdList.isEmpty())
        {
            QDomElement EndEnvFinalElem = scxmlDomDoc.createElement(SXN_FINAL);
            EndEnvFinalElem.setAttribute(SXA_ID,"EndEnv_Final");
            scxmlEndEnvironmentElem.appendChild(EndEnvFinalElem);
        }
    }

    if(!scxmlDataModelElem.isNull() )
    {
        QHash<QString,GTAScxmlDataModel> VarModelList;
        foreach(QString  var, _DataModelList.keys())
        {
            if(!var.isEmpty())
            {
                GTAScxmlDataModel objDataModel = _DataModelList.value(var);
                //if(objDataModel.IsPIR == false)
                if((objDataModel.IsApplication == true) )//&& ((objDataModel.IsPIR == false) || (!_IsPirParamManagement))) // IKM : Under Use
                {
                    QString appName = objDataModel.ID;
                    QString actor = objDataModel.getActorName();
                    QStringList forbidChars = QString("+*/^&~%!|(){}~,").split("",QString::SkipEmptyParts);
                    QStringList items  = appName.split("",QString::SkipEmptyParts);
                    QSet<QString> intersection = forbidChars.toSet().intersect(items.toSet());
                    QList<QString> invalidList =  intersection.toList();
                    if(invalidList.isEmpty())
                    {
                        QString expression= QString("{ 'Name':'%1'").arg(appName);
                        QStringList mediaList = objDataModel.getMediaList();
                        foreach(QString media, mediaList)
                        {
                            if (media == "PIR")
                                continue;
                            QList<QString> busNameList = objDataModel.getBusNameList(media);
                            foreach(QString busName, busNameList)
                            {
                                if(actor.isEmpty())
                                    expression += QString(",'%1':{'Media':'%2'}").arg(busName,media);
                                else
                                {
                                    //expression += QString(",'%1':{'Media':'%2','Actor':'%3'}").arg(busName,media,actor);
                                    expression += QString(",'%1':{'Media':'%2'}").arg(busName,media);
                                }
                            }
                        }
                        expression += "}";

                        QDomElement dataElem = scxmlDomDoc.createElement(SXN_DATA);
                        appName = appName.replace("-","_");
                        dataElem.setAttribute(SXA_ID,appName);
                        dataElem.setAttribute(SXA_EXPR,expression);
                        scxmlDataModelElem.appendChild(dataElem);

                    }
                }
                else
                {

                    QStringList forbidChars = QString("+*/^&~%!|(){}~',").split("",QString::SkipEmptyParts);
                    QStringList items  = var.split("",QString::SkipEmptyParts);
                    QSet<QString> intersection = forbidChars.toSet().intersect(items.toSet());
                    QList<QString> invalidList =  intersection.toList();
                    if(invalidList.isEmpty())
                    {
                        VarModelList.insert(var,objDataModel);
                    }



                }
            }
        }

        foreach(QString var, VarModelList.keys())
        {
            if(!var.isEmpty())
            {
                QString param = var;
                GTAScxmlDataModel objDataModel = VarModelList.value(param);
                QDomElement dataElem = scxmlDomDoc.createElement(SXN_DATA);
                updateWithGenToolParam(param);
                if(_genToolParams.contains(param))
                {
                    param =_genToolParams.value(param);
                }
                if(!_addedParams.contains(param))
                {
                    dataElem.setAttribute(SXA_ID,param);
                    if(! objDataModel.Expr.isEmpty())
                    {
                        dataElem.setAttribute(SXA_EXPR,objDataModel.Expr);
                    }
                    else
                    {
                        dataElem.setAttribute(SXA_EXPR,"");
                    }
                    scxmlDataModelElem.appendChild(dataElem);
                    _addedParams.append(param);
                }

            }
        }

        QDomElement dataMainFinal = scxmlDomDoc.createElement(SXN_DATA);
        dataMainFinal.setAttribute(SXA_ID,MAIN_FINAL_DONE);
        scxmlDataModelElem.appendChild(dataMainFinal);
        generateFailureDataModel(scxmlDomDoc,scxmlDataModelElem);
    }

    if(isStandaloneExecMode)
    {
        _DataModelList.clear();
        failureInvokeNodeList.clear();
    }
    updateSubscribeState(scxmlDomDoc,scxmlSubscribeElem,scxmlDataModelElem,scxmlUnSubscribeElem,_DataModelList);

    if(!failureInvokeNodeList.isEmpty())
    {
        for(int i=0;i<failureInvokeNodeList.count();i++)
        {
            scxmlSubscribeElem.appendChild(failureInvokeNodeList.at(i));
        }
        QDomElement SubscribeTxElem = scxmlSubscribeElem.elementsByTagName(SXN_TRANSITION).at(0).toElement();
        SubscribeTxElem.setAttribute(SXA_EVENT,"done.invoke._all");
    }


    if(!scxmlDomDoc.isNull())
    {
        _ParamList.clear();
        _addedParams.clear();

        QString toolVersion = QString("Generated by using GTA Version:%1").arg(TOOL_VERSION_STR); //--IKM
        QDomComment scxmlVersionComment = scxmlDomDoc.createComment(toolVersion) ;
        scxmlRootElem.insertBefore(scxmlVersionComment,scxmlRootElem.firstChild());//toElement().setAttribute("GTA_TOOL_Version",toolVersion);
        //scxmlRootElem.setAttribute(SXA_VERSION,QString("'%1'").arg(TOOLVER)); //--IKM
        scxmlRootElem.setAttribute(SXA_ELEMENT_NAME, _elementName);
        QString xmlContents = scxmlDomDoc.toString();
        scxmlDomDoc.clear();

        
        // replacing all standard xml representation of linefeed and carriage return by \n and \r
        xmlContents.replace("&#xa;","");
        //xmlContents.replace("&#xa;","&lt;br>&lt;/br>");
        xmlContents.replace("&#xd;","");
        //xmlContents.replace("")
        //        foreach(QString stateIDs,_hshStateIDName.keys())
        //        {
        //            xmlContents.replace(QString("\"%1\"").arg(stateIDs),QString("\"%1\"").arg(_hshStateIDName[stateIDs]));
        //        }
        ////////////////////////////////////////////
        QFile outFile(iFilePath);
        outFile.open(QFile::WriteOnly|QFile::Text);
        QTextStream writer(&outFile);
        writer<<xmlContents; 
        outFile.close();
    }

    // reset hash seed with new random value.
    qSetGlobalQHashSeed(-1);

    return rc;
}
/**
  * This function assemble the scxml stats for each command
  * ipRootNode: The command node for which the stat to be created
  * iDomDoc: state xml is create in the context of this document
  * iMainStatElem : The Root element where the state is inserted
  * iLogStatElem : The log state where the log of each command is inserted
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::MakeScxml(const GTACmdNode * ipRootNode, QDomDocument & iDomDoc, QDomElement & iRootElem,QDomElement & iLogStatElem)
{
    bool rc = false;
    if(ipRootNode == NULL || iDomDoc.isNull() || iRootElem.isNull())
        return rc;

    GTACmdNode * pCmdNode = (GTACmdNode*)ipRootNode;
    while (pCmdNode != NULL)
    {
        GTACommandBase * pCmd = pCmdNode->getCommand();

        
        if(pCmd !=NULL && pCmd->isOneClick() )
        {
            pCmdNode = pCmdNode->NextNode;
            if(pCmdNode!= NULL)
                continue;
            else 
            {
                QString MainInitTargetId = getTargetId(pCmdNode);
                QDomNode mainInitAttr = iRootElem.attributes().namedItem(SXA_INITIALSTATE);
                if(!mainInitAttr.isNull())
                {
                    QString mainInitAttrVal = mainInitAttr.nodeValue();
                    if(mainInitAttrVal.trimmed().isEmpty())
                    {
                        if(iRootElem.nodeName()!=SXN_PARALLEL)
                            iRootElem.setAttribute(SXA_INITIALSTATE,MainInitTargetId);
                    }
                }
                else
                {
                    if(iRootElem.nodeName()!=SXN_PARALLEL)
                        iRootElem.setAttribute(SXA_INITIALSTATE,MainInitTargetId);
                }

                break;
            }
        }

        QString sStateId = pCmdNode->getStateId();
        QString instanceName = pCmd->getStatement();
        QStringList varList = pCmd->getVariableList();
        QString stateId = sStateId;


        QString targetId = getTargetId(pCmdNode);
        QDomNode mainInitAttr = iRootElem.attributes().namedItem(SXA_INITIALSTATE);
        QString MainInitTargetId1 = stateId;
        if(!mainInitAttr.isNull())
        {
            QString mainInitAttrVal = mainInitAttr.nodeValue();
            if(mainInitAttrVal.trimmed().isEmpty())
            {
                if(iRootElem.nodeName()!=SXN_PARALLEL)
                    iRootElem.setAttribute(SXA_INITIALSTATE,MainInitTargetId1);
            }
        }
        else
        {
            if(iRootElem.nodeName()!=SXN_PARALLEL)
                iRootElem.setAttribute(SXA_INITIALSTATE,MainInitTargetId1);
        }

        QMap<QString,double> stateIdTimeoutMap;
        getNodeTimeouts(pCmdNode,stateIdTimeoutMap);

        GTACommandBase::CommandType cmdType = pCmd->getCommandType();
        if(NULL!=pCmd&& stateIdTimeoutMap.isEmpty()==false)
            pCmd->setReferenceTimeOutStates(stateIdTimeoutMap.keys());

        if(cmdType == GTACommandBase::ACTION)
        {
            GTAActionBase * pActionCmd = dynamic_cast<GTAActionBase*>((GTACommandBase*)pCmd);
            QString actionName = pActionCmd->getAction();
            QString complementName= pActionCmd->getComplement();
            if(actionName == ACT_MANUAL)
            {
                GTAActionManual * pActCmd = dynamic_cast<GTAActionManual*>(pActionCmd);
                if(pActCmd != NULL)
                {

                    rc = createActionManualState(pActCmd,iDomDoc,iRootElem,stateId,targetId,stateIdTimeoutMap);
                    
                }
            }
            else if(actionName == ACT_CONDITION)
            {
                if(complementName == COM_CONDITION_IF )
                {
                    GTAActionIF * pActCmd = dynamic_cast<GTAActionIF*>(pActionCmd);
                    if(pActCmd != NULL)
                    {
                        QString leftTargetId, rightTargetId;
                        GTACmdNode * pLeftNode = NULL;
                        GTACmdNode * pRightNode = NULL;
                        if(pCmdNode->Children.contains(true))
                        {
                            QList<GTACmdNode*> * leftCmdList = pCmdNode->Children.value(true);
                            if(leftCmdList != NULL && leftCmdList->count()>0)
                            {
                                pLeftNode = leftCmdList->at(0);
                                if(pLeftNode != NULL)
                                    leftTargetId = pLeftNode->getStateId();
                            }
                            else leftTargetId = targetId;
                        }
                        else leftTargetId = targetId;

                        if(pCmdNode->Children.contains(false))
                        {
                            QList<GTACmdNode*> * rightCmdList = pCmdNode->Children.value(false);
                            if(rightCmdList != NULL && rightCmdList->count()>0)
                            {
                                pRightNode = rightCmdList->at(0);
                                if(pRightNode != NULL)
                                    rightTargetId = pRightNode->getStateId();
                            }
                        }
                        else rightTargetId = targetId;

                        QDomElement stateNode;
                        rc = createActionIfState(pActCmd,iDomDoc,iRootElem,stateId,leftTargetId,rightTargetId,targetId,stateNode);


                        if(pRightNode != NULL && stateNode.isNull()==false)
                            MakeScxml(pRightNode,iDomDoc,stateNode,iLogStatElem);
                        if(pLeftNode != NULL && stateNode.isNull()==false)
                            MakeScxml(pLeftNode,iDomDoc,stateNode,iLogStatElem);

                    }
                }
                else if(complementName == COM_CONDITION_WHILE)
                {
                    GTAActionWhile * pActCmd = dynamic_cast<GTAActionWhile*>(pActionCmd);
                    if(pActCmd != NULL)
                    {
                        if(pCmdNode->Children.contains(true))
                        {
                            QList<GTACmdNode*> * childNodeList = pCmdNode->Children.value(true);

                            QString statIdTrueCond = targetId;
                            QString statIdFalseCond = targetId;
                            GTACmdNode *pChildNode = NULL;
                            if(childNodeList != NULL && childNodeList->count() >0)
                            {
                                pChildNode = childNodeList->at(0);
                                if(pChildNode != NULL)
                                    statIdTrueCond = pChildNode->getStateId();
                            }
                            QDomElement stateNode;
                            rc = createActionWhileState(pActCmd,iDomDoc,iRootElem,stateId,statIdTrueCond,statIdFalseCond,targetId,stateNode);
                            pCmdNode->setStateId(stateId);
                            if(pChildNode != NULL)
                                MakeScxml(pChildNode,iDomDoc,stateNode,iLogStatElem);
                        }
                    }
                }
                else if(complementName == COM_CONDITION_DO_WHILE)
                {
                    GTAActionDoWhile * pActCmd = dynamic_cast<GTAActionDoWhile*>(pActionCmd);
                    if(pActCmd != NULL)
                    {
                        if(pCmdNode->Children.contains(true))
                        {
                            QList<GTACmdNode*> * childNodeList = pCmdNode->Children.value(true);

                            QString statIdTrueCond = targetId;
                            QString statIdFalseCond = targetId;
                            GTACmdNode *pChildNode = NULL;
                            if(childNodeList != NULL && childNodeList->count() >0)
                            {
                                pChildNode = childNodeList->at(0);
                                if(pChildNode != NULL)
                                    statIdTrueCond = pChildNode->getStateId();
                            }
                            QDomElement stateNode;
                            rc = createActionDoWhileState(pActCmd,iDomDoc,iRootElem,stateId,statIdTrueCond,statIdFalseCond,targetId,stateNode);
                            pCmdNode->setStateId(stateId);
                            if(pChildNode != NULL)
                                MakeScxml(pChildNode,iDomDoc,stateNode,iLogStatElem);
                        }
                    }
                }
                else if(complementName == COM_CONDITION_FOR_EACH)
                {
//                    GTAActionForEach * pActCmd = dynamic_cast<GTAActionForEach*>(pActionCmd);
//                    if(pActCmd != NULL)
//                    {
//                        if(pCmdNode->Children.contains(true))
//                        {
//                            QList<GTACmdNode*> * childNodeList = pCmdNode->Children.value(true);

//                            QString statIdTrueCond = targetId;
//                            QString statIdFalseCond = targetId;
//                            GTACmdNode *pChildNode = NULL;
//                            if(childNodeList != NULL && childNodeList->count() >0)
//                            {
//                                pChildNode = childNodeList->at(0);
//                                if(pChildNode != NULL)
//                                    statIdTrueCond = pChildNode->getStateId();
//                            }
//                            QDomElement stateNode;
//                            rc = createActionForEachState(pActCmd,iDomDoc,iRootElem,stateId,statIdTrueCond,statIdFalseCond,targetId,stateNode);
//                            pCmdNode->setStateId(stateId);
//                            if(pChildNode != NULL)
//                                MakeScxml(pChildNode,iDomDoc,stateNode,iLogStatElem);
//                        }
//                    }
                }
            }
            
            else if(actionName == ACT_IRT)
            {

                GTAActionIRT * pActCmd = dynamic_cast<GTAActionIRT*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionIRTState(pActCmd,iDomDoc,iRootElem,iLogStatElem,stateId,targetId);
                }
            }
            else if(actionName == ACT_SUBSCRIBE)
            {

                GTAActionSubscribe * pActCmd = dynamic_cast<GTAActionSubscribe*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionSubscribeState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            else if(actionName == ACT_UNSUBSCRIBE)
            {

                GTAActionUnSubscribe * pActCmd = dynamic_cast<GTAActionUnSubscribe*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionUnSubscribeState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            else if(actionName == ACT_TITLE)
            {
                GTAActionParentTitle * pActCmd = dynamic_cast<GTAActionParentTitle*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    if(pCmdNode->Children.contains(true))
                    {
                        QList<GTACmdNode*> * childNodeList = pCmdNode->Children.value(true);

                        QString statIdTrueCond = targetId;
                        QString statIdFalseCond = targetId;
                        GTACmdNode *pChildNode = NULL;
                        if(childNodeList != NULL && childNodeList->count() >0)
                        {
                            pChildNode = childNodeList->at(0);
                            if(pChildNode != NULL)
                                statIdTrueCond = pChildNode->getStateId();
                        }
                        QDomElement oRootElem;
                        rc = createActionTitleState(pActCmd,iDomDoc,iRootElem,stateId,targetId,oRootElem);
                        pCmdNode->setStateId(stateId);
                        if(pChildNode != NULL)
                            MakeScxml(pChildNode,iDomDoc,oRootElem,iLogStatElem);
                    }
                    else
                    {
                        QDomElement oRootElem;
                        rc = createActionTitleState(pActCmd,iDomDoc,iRootElem,stateId,targetId,oRootElem);

                    }
                }
            }
            else if(actionName == ACT_CALL)
            {

                GTAActionCall * pActCmd = dynamic_cast<GTAActionCall*>((GTAActionBase*)pActionCmd);
                if(pActCmd != NULL)
                {
                    if(pCmdNode->Children.contains(true))
                    {
                        QList<GTACmdNode*> * childNodeList = pCmdNode->Children.value(true);

                        QString statIdTrueCond = targetId;
                        QString statIdFalseCond = targetId;
                        GTACmdNode *pChildNode = NULL;
                        if(childNodeList != NULL && childNodeList->count() >0)
                        {
                            pChildNode = childNodeList->at(0);
                            if(pChildNode != NULL)
                                statIdTrueCond = pChildNode->getStateId();
                        }
                        QDomElement oRootElem;
                        rc = createActionCallState(pActCmd,iDomDoc,iRootElem,stateId,targetId,oRootElem);
                        pCmdNode->setStateId(stateId);
                        if(pChildNode != NULL)
                            MakeScxml(pChildNode,iDomDoc,oRootElem,iLogStatElem);
                    }
                    else
                    {
                        QDomElement oRootElem;
                        rc = createActionCallState(pActCmd,iDomDoc,iRootElem,stateId,targetId,oRootElem,true);

                    }
                }
            }
            else if(actionName == ACT_ONECLICK)
            {
                if(complementName == COM_ONECLICK_TITLE)
                {
                    GTAActionOneClickParentTitle * pActCmd = dynamic_cast<GTAActionOneClickParentTitle*>(pActionCmd);
                    if(pActCmd != NULL)
                    {
                        if(pCmdNode->Children.contains(true))
                        {
                            //QList<GTACmdNode*> * childNodeList = pCmdNode->Children.value(true);

                            //QString statIdTrueCond = targetId;
                            //QString statIdFalseCond = targetId;
                            //GTACmdNode *pChildNode = NULL;
                            //if(childNodeList != NULL && childNodeList->count() >0)
                            //{
                            //    pChildNode = childNodeList->at(0);
                            //    if(pChildNode != NULL)
                            //        statIdTrueCond = pChildNode->getStateId();
                            //}
                            //QDomElement oRootElem;
                            ////rc = createActionOneClickTitleState(pActCmd,iDomDoc,iRootElem,stateId,targetId,oRootElem);
                            //pCmdNode->setStateId(stateId);
                            //if(pChildNode != NULL)
                            //    MakeScxml(pChildNode,iDomDoc,oRootElem,iLogStatElem);
                        }
                        else
                        {
                            //QDomElement oRootElem;
                            //rc = createActionOneClickTitleState(pActCmd,iDomDoc,iRootElem,stateId,targetId,oRootElem);

                        }
                    }
                }
                else if(complementName == COM_ONECLICK_ENV)
                {

                    GTAOneClick * pActCmd = dynamic_cast<GTAOneClick*>(pActionCmd);
                    if(pActCmd != NULL)
                    {
                        //QString tmplFile  = "Init_OneClick.scxml";
                        //QDomElement oElem;
                        //rc = getCloneRootElement(tmplFile,iDomDoc,oElem);

                        //QDomNode firstChild = iDomDoc.firstChild();
                        //QDomNodeList firstLevelChildrens = firstChild.childNodes();
                        //for(int i=0;i<firstLevelChildrens.size();i++)
                        //{
                        //    QDomElement domElem = firstLevelChildrens.at(i).toElement();
                        //    if(domElem.nodeName() == SXN_STATE && domElem.attribute(SXA_ID) == "Init_Environment");
                        //    {
                        //        if (domElem.elementsByTagName(SXN_INVOKE).size()!=0)
                        //        {
                        //            QDomElement invokeNode = oElem.elementsByTagName(SXN_INVOKE);


                        //        }
                        //    }
                        //}
                        //
                        //if (rc)
                        //{
                        //    iRootElem.appendChild(oElem);
                        //}
                        // rc = createActionOneClickSetEnvState(pActCmd,iDomDoc,iRootElem,iLogStatElem,stateId,targetId);
                    }
                }
                else if(complementName == COM_ONECLICK_PINPROG)
                {
                    GTAActionOneClickPPC *pActCmd = dynamic_cast<GTAActionOneClickPPC*>(pActionCmd);
                    if(pActCmd != NULL)
                    {
                    }
                }
                else if(complementName == COM_ONECLICK_APP)
                {
                    GTAOneClickLaunchApplication *pActCmd = dynamic_cast<GTAOneClickLaunchApplication*>(pActionCmd);
                    if(pActCmd != NULL)
                    {
                    }
                }


            }

            
            
            else if(actionName == ACT_SET)
            {

                GTAActionSetList * pActCmd = dynamic_cast<GTAActionSetList*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionSetState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            else if(actionName == ACT_INVALID)
            {

                GTAInvalidCommand * pActCmd = dynamic_cast<GTAInvalidCommand*>(pActionCmd);
                if(pActCmd != NULL && pActCmd->getStateExists())
                {
                    rc = createActionInvlaidState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }

            //==============================================================
            else if(actionName == ACT_PRINT)
            {

                GTAActionPrint * pActCmd = dynamic_cast<GTAActionPrint*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    if(complementName == COM_PRINT_FWC_BUFF)
                    {
                        rc = createActionPrintFwcBufferState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                    }
                    else if(complementName == COM_PRINT_BITE_BUFF)
                    {
                        rc = createActionPrintBiteBufferState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                    }
                    else
                        rc = createActionPrintState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
                else
                    if(complementName == COM_PRINT_TIME)
                    {
                        GTAActionPrintTime * pActCmd = dynamic_cast<GTAActionPrintTime*>(pActionCmd);
                        if(pActCmd != NULL)
                        {
                            rc = createActionPrintTimeState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                        }
                    }

                    else if(complementName == COM_PRINT_TABLE)
                    {
                        GTAActionPrintTable * pActCmd = dynamic_cast<GTAActionPrintTable*>(pActionCmd);
                        if(pActCmd != NULL)
                        {
                            rc = createActionPrintTableState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                        }
                    }
            }



            else if(actionName == ACT_FUNC_RETURN)
            {

                GTAActionFunctionReturn * pActCmd = dynamic_cast<GTAActionFunctionReturn*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionReturnFunctionState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            else if(actionName == ACT_ROBO_ARM)
            {

                GTAActionRoboArm * pActCmd = dynamic_cast<GTAActionRoboArm*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionRoboArmState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            else if(actionName == ACT_WAIT)
            {

                GTAActionWait * pActCmd = dynamic_cast<GTAActionWait*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionWaitState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            else if(actionName == ACT_CALL_PROCS)
            {
                GTAActionCallProcedures * pActCmd = dynamic_cast<GTAActionCallProcedures*>(pActionCmd);

                if(pCmdNode->Children.contains(true) )
                {
                    
                    QList<GTACmdNode*> * childList =  pCmdNode->Children.value(true);
                    if(childList != NULL && childList->count() > 0)
                    {
                        GTACmdNode * pNode = childList->at(0);
                        if(pNode != NULL)
                        {
                            if(pActCmd->getPrallelExecution()==true)
                            {
                                QDomElement oRootElem;
                                rc = createActionCallProcsState(pActCmd,iDomDoc,iRootElem,stateId,targetId,oRootElem);
                                pCmdNode->setStateId(stateId);
                                MakeScxml(pNode,iDomDoc,oRootElem,iLogStatElem);
                            }
                            else
                                MakeScxml(pNode,iDomDoc,iRootElem,iLogStatElem);

                        }
                    }
                    


                    
                }
            }
            else if(actionName == ACT_FCTL)
            {
                if(pCmdNode->Children.contains(false))
                {
                    QList<GTACmdNode*> * childList =  pCmdNode->Children.value(false);
                    if(childList != NULL && childList->count() > 0)
                    {
                        GTACmdNode * pNode = childList->at(0);
                        if(pNode != NULL)
                        {
                            MakeScxml(pNode,iDomDoc,iRootElem,iLogStatElem);
                        }
                    }
                }
                GTAActionFTAKinematicMultiOutput * pActCmd = dynamic_cast<GTAActionFTAKinematicMultiOutput*>(pActionCmd);
                if(pActCmd!= NULL)
                {
                    QString templateInstanceName = pActCmd->getCurrentName();
                    QString tableName = QString("%1_%2").arg(templateInstanceName,"table");
                    QStringList tableData = pActCmd->getRefFileData();
                    GTAScxmlDataModel objDM;
                    objDM.ID = tableName;
                    QString tempData = tableData.join(",");
                    tempData = tempData.replace(";",",");
                    objDM.Expr = QString("[%1]").arg(tempData);
                    _DataModelList.insert(tableName,objDM);
                }
            }
            else if(actionName == ACT_REL)
            {

                GTAActionRelease * pActCmd = dynamic_cast<GTAActionRelease*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionReleaseState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            else if(actionName == ACT_FAILURE)
            {

                GTAActionFailure * pActCmd = dynamic_cast<GTAActionFailure*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionFailureState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            /*           else if(actionName == ACT_PRINT_TABLE)
            {

                GTAActionPrintTable * pActCmd = dynamic_cast<GTAActionPrintTable*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionPrintTableState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }*/

            else if(actionName == ACT_GEN_FUNC)
            {

                GTAActionGenericFunction * pActCmd = dynamic_cast<GTAActionGenericFunction*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createGenericFunctionState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
            if(actionName == ACT_MATH)
            {
                GTAActionMath * pActCmd = dynamic_cast<GTAActionMath*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionMathState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }

            }
            if(actionName == ACT_MATHS)
            {
                GTAActionMaths * pActCmd = dynamic_cast<GTAActionMaths*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionMathsState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }

            }
            if(actionName == ACT_GEN_TOOL)
            {
                GTAGenericToolCommand * pActCmd = dynamic_cast<GTAGenericToolCommand*>(pActionCmd);
                if(pActCmd != NULL)
                {
                    rc = createActionGenericToolSCXMLState(pActCmd,iDomDoc,iRootElem,stateId,targetId);
                }
            }
        }
        else if (cmdType==GTAActionBase::CHECK)
        {
            GTACheckBase * pChkCmd = dynamic_cast<GTACheckBase*>((GTACommandBase*)pCmd);
            if (NULL!=pChkCmd)
            {
                GTACheckBase::CheckType chkType = pChkCmd->getCheckType();

                if (chkType==GTACheckBase::VALUE)
                {
                    //rc = false;
                    GTACheckValue* pChkVal = dynamic_cast<GTACheckValue*>(pChkCmd);
                    rc=createCheckValueState(pChkVal,iDomDoc,iRootElem,stateId,targetId,"",false);
                }
                else if (chkType==GTACheckBase::FWC_WARNING)
                {
                    GTACheckFwcWarning* pChkfwc = dynamic_cast<GTACheckFwcWarning*>(pChkCmd);
                    rc=createCheckFWCWrng(pChkfwc,iDomDoc,iRootElem,stateId,targetId);
                }
                else if (chkType==GTACheckBase::BITE_MESSAGE)
                {
                    GTACheckBiteMessage* pChkBiteMsg = dynamic_cast<GTACheckBiteMessage*>(pChkCmd);
                    rc=createCheckBiteMessage(pChkBiteMsg,iDomDoc,iRootElem,stateId,targetId);
                }
                else if (chkType==GTACheckBase::AUDIO_ALARM)
                {
                    GTACheckAudioAlarm* pChkAudioAlarm = dynamic_cast<GTACheckAudioAlarm*>(pChkCmd);
                    rc=createCheckAudioAlarm(pChkAudioAlarm,iDomDoc,iRootElem,stateId,targetId);
                }
            }

        }

        pCmdNode = pCmdNode->NextNode;
    }


    return rc;
}

/**
  * This function create the scxml state for check value command
  * pChk : Command for which the state to be created
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iStateId: State id of the command;
  * iTargetId: The next ID where the stat should go after execution
  * iFailStateID is set in case of confirmation time check is required. 
    This will treat itself as check being created for confirmation time.
  * return true if scxml of the command is created, otherwise false
  * 
  */
bool GTAExportScxml::createCheckValueState(const GTACheckValue *pChk, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId, const QString &iFailStateID, bool externalTimeOut, const QString &externalTimeOutEventName, QDomElement &oStateElem, const QString &iConfTimeInMs, const QString &iconfigStateId, const QString &iTimeOutDelay, const QString iTimeoutName)
{
    bool rc = false;
    if (pChk)
    {

        bool waitUntil = pChk->getWaitUntil();
        QString chkname("check_value");
        QString fileName = chkname + ".scxml";
        bool IsPIRSCXMLConfig = TestConfig::getInstance()->getPIRInvokeForSCXML();
        QString actionOnFail = pChk->getActionOnFail();
        QStringList dumpListParams = pChk->getDumpList();
        QString successStatID = iTargetId;
        QString hiddenWaitTimeTxEvent = "";
        double timeout;
        QString sTimeout,sTimeoutUnit;
        pChk->getTimeOut(sTimeout,sTimeoutUnit);
        timeout = sTimeout.toDouble();
        
        QString failStatID;
        bool bConfigTimeExists=false;

        QString ConfTimeoutLog = QString("%1_ConfTimeoutLog").arg(iStateId);
        QString configTimeStateID;
        QString conftimeInMiliSec = getConfTimeInMiliSecond((GTAActionWait*)pChk);
        QDomElement ConfTimeStateElem;
        QDomElement elementOnConfTimeState;
        GTACheckValue checkValueForConfTime(*pChk);

        //GTAAppSettings appSet;
        bool repeatLog = false; //appSet.getRepeatedLoggingSetting();


        if(!waitUntil)
        {
            if (actionOnFail.contains("stop"))
            {
                failStatID=MAIN_FINAL_STATE;
            }
            else
                failStatID=iTargetId;
        }

        QString sStateId= iStateId;

        QString confTimeTarget = QString("%1_2").arg(sStateId);
        configTimeStateID= QString("%1_Confirmation_time").arg(sStateId);
        QString configTimeEventName;

        if(waitUntil)
        {
            bConfigTimeExists = getConfigurationTimeNode(iDomDoc,ConfTimeStateElem,configTimeStateID,conftimeInMiliSec,confTimeTarget,configTimeEventName);
        }


        QString timeOut = getTimeOutInMiliSecond((GTACheckValue*)pChk);



        QString FinalExpr;
        int paramCount = pChk->getCount();
        QStringList ListOfVarToLog;
        QStringList ParamListForInvoke;
        bool isPIR = false;
        for(int j = 0; j < paramCount; j++ )
        {

            QString sParameter	= pChk->getParameter(j);
            QString sValue		= pChk->getValue(j);
            QString sCondition	= pChk->getCondition(j);
            QString sFStatus = pChk->getFunctionalStatus(j);
            bool isCheckOnlyFS = pChk->getISCheckFS(j);
            bool isCheckOnlyValue = pChk->getISOnlyCheckValue(j);
            bool isStringVar = false;
            if((sValue.contains("\"")) || (sValue.contains("\'")))
            {
                if(sValue.contains("\""))
                    sValue = (sValue.remove("\""));
                else if(sValue.contains("\'"))
                    sValue = (sValue.remove("\'"));
                isStringVar = true;
            }

            updateWithGenToolParam(sValue);
            updateWithGenToolParam(sParameter);

            _hasChannelInControl = false;
            if(sParameter.contains(GTA_CIC_IDENTIFIER) || sValue.contains(GTA_CIC_IDENTIFIER))
                _hasChannelInControl = true;


            QStringList resolvedLhsParams;

            //bool hasLhsChannelInfo = 
			GTAUtil::resolveChannelParam(sParameter,resolvedLhsParams);

            QStringList resolvedRhsParams;
            //bool hasRhsChannelInfo = 
			GTAUtil::resolveChannelParam(sValue,resolvedRhsParams);

            QString tempParam = resolvedLhsParams.at(0);
            //tempParam.replace(QString(":CIC%1").arg(GTA_CIC_IDENTIFIER),"");
            //tempParam.replace(GTA_CIC_IDENTIFIER,"");

            GTAICDParameter paramInfo = getParamObj(tempParam);
            
            if(paramInfo.getName() != "")
                tempParam = paramInfo.getName();
            isPIR = paramInfo.getSourceType()== PARAM_TYPE_PIR?true:false;


            QString sBinaryOperator;
            sParameter = sParameter.trimmed();
            sValue = sValue.trimmed();

            double dPrecisionVal = pChk->getPrecisionValue(j);
            QString sPrecisonUnit = pChk->getPrecisionType(j);

            if(sPrecisonUnit == ACT_PRECISION_UNIT_PERCENT)
            {
                dPrecisionVal = dPrecisionVal*0.01;
            }
            if (paramCount>1)
                sBinaryOperator = pChk->getBinaryOperator(j);


            QStringList ParamList;
            //ParamList<<sParameter<<sValue;
            QString sTrueExpression;
            QString sFalseExpression;
            bool bHasStatus=false;

            getChannelInControlCondition(sParameter,sValue,sCondition,sTrueExpression,sFalseExpression,isStringVar,dPrecisionVal,sPrecisonUnit,ParamList,isCheckOnlyFS);



            //insert in subscribe list and data model
            foreach(QString param, ParamList)
            {
                bool isNum = false;
                param.toDouble(&isNum);

                if(isNum == false)
                {
                    rc = insertValidAppDataModel(param);
                    if(!rc)
                    {
                        if((param.contains("\"")) || (param.contains("\'")))
                        {
                            if(param.contains("\""))
                                param = (param.remove("\""));
                            else if(param.contains("\'"))
                                param = (param.remove("\'"));
                            rc = false;
                        }
                        else
                        {
                            rc = true;

                            if ((!param.isEmpty()) && (isStringVar == false))
                            {
                                GTAScxmlDataModel objDM;


                                objDM.ID = param;
                                _DataModelList.insert(param,objDM);
                                ListOfVarToLog.append(param);
                            }

                        }
                    }
                    else
                    {
                        bHasStatus=true;
                        ParamListForInvoke.append(param);
                        ListOfVarToLog.append(param);
                    }
                }
                else
                    rc= true;
            }

            //if(ParamListForInvoke.contains(sParameter))
            //{
            //    sParameter.append(".Value");

            //}
            //if(ParamListForInvoke.contains(sValue))
            //{
            //    sValue.append(".Value");
            //}

            //in case if subscription is true for value, then the value part should not be in quotes
            //getExpresion(sParameter,sValue,sCondition,sTrueExpression,sFalseExpression,!rc,dPrecisionVal,sPrecisonUnit);

            //sParameter.replace(".Value",".Status");

            QString sTrueStatusExpression;
            if(isPIR)
                sFStatus=ACT_FSSSM_NOT_LOST;

            if ((bHasStatus) && (_ParamList.contains(sParameter)))
            {
                if(sFStatus == ACT_FSSSM_NOT_LOST)
                {
                    sFStatus = ACT_FSSSM_LOST;
                    if(isCheckOnlyFS)
                    {
                        if(sCondition == ARITH_EQ)
                            getChannelInControlCondition(sParameter,QString("'%1'").arg(sFStatus),ARITH_NOTEQ,sTrueStatusExpression,sFalseExpression,!rc,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);
                        else if(sCondition == ARITH_NOTEQ)
                            getChannelInControlCondition(sParameter,QString("'%1'").arg(sFStatus),ARITH_EQ,sTrueStatusExpression,sFalseExpression,!rc,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);
                        else
                            getChannelInControlCondition(sParameter,QString("'%1'").arg(sFStatus),ARITH_NOTEQ,sTrueStatusExpression,sFalseExpression,!rc,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);
                    }
                    else
                    {
                        getChannelInControlCondition(sParameter,QString("'%1'").arg(sFStatus),ARITH_NOTEQ,sTrueStatusExpression,sFalseExpression,!rc,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);
                    }
                    
                }
                else
                {
                    if(isCheckOnlyFS)
                    {
                        getChannelInControlCondition(sParameter,QString("'%1'").arg(sFStatus),sCondition,sTrueStatusExpression,sFalseExpression,!rc,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);
                    }
                    else
                    {
                        getChannelInControlCondition(sParameter,QString("'%1'").arg(sFStatus),ARITH_EQ,sTrueStatusExpression,sFalseExpression,!rc,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);
                    }
                }

                if(isCheckOnlyFS == true)
                {
                    sTrueExpression=QString("((%1))").arg(sTrueStatusExpression);
                }
                else if (isCheckOnlyValue == false)
                {
                    sTrueExpression=QString("((%1)&&(%2))").arg(sTrueExpression,sTrueStatusExpression);
                } //by default true expression has only check value .
            }
            else
                sTrueStatusExpression=QString("(%1)").arg(sTrueStatusExpression);

            rc = false;

            sFalseExpression.prepend("(");
            sFalseExpression.append(")");

            if (sTrueExpression.isEmpty())
                sTrueExpression="true";

            FinalExpr.append(sTrueExpression);

            if (!sBinaryOperator.isEmpty())
            {
                QString binOper;
                if (sBinaryOperator=="AND")
                {
                    if(j != paramCount - 1)
                        binOper = "&&";
                }
                else if (sBinaryOperator=="OR")
                {
                    if(j != paramCount - 1)
                        binOper = "||";
                }
                FinalExpr.append(binOper);
            }

        }

        QDomElement oElem;
        rc = getCloneRootElement(fileName,iDomDoc,oElem);
        oStateElem = oElem;
        if(rc == false || oElem.isNull())
            return false;

        QDomElement oTimeOutElem;
        QString TimeOutTarget = QString("%1_1").arg(sStateId);
        if(externalTimeOut||timeout>0)//case arises when createCheckValueStat is not called for confirmation time
        {
            rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOut,TimeOutTarget);
            if (rc)
            {
                sStateId = TimeOutTarget;
                iRootElem.appendChild(oTimeOutElem);
                failStatID = iFailStateID;
            }
        }

        setStateAttr(oElem,sStateId);
        QString configtimeFailTxxn = successStatID;
        QString configtimeSucessTxxn = successStatID;
        if(actionOnFail==ACT_FAIL_STOP)
            configtimeFailTxxn =MAIN_FINAL_STATE;


        setStateIDName(sStateId,pChk);
        QMap<QString, QDomElement> StatChildXPathMap;
        getScxmlElementPathMap(oElem,oElem.nodeName(),StatChildXPathMap);

        QString invokeXPath = getStateInvokeXPath();
        QDomElement invokeElem = StatChildXPathMap.value(invokeXPath);


        QString invokeID = QString(sStateId).replace("s","i");
        //ParamListForInvoke.removeDuplicates();
        QStringList dumpListForInvoke = getValidParametersToInvoke(dumpListParams);
        QStringList allParamForInvoke = QStringList()<<ParamListForInvoke<<dumpListForInvoke;
        //createInvokeNodes(oElem,invokeElem,allParamForInvoke,invokeID,"ParameterID","info",pChk->getInstanceName());
        createInvokeStateNodes(iDomDoc,oElem,invokeElem,allParamForInvoke,invokeID,"ParameterID","info",pChk->getInstanceName(),false, pChk->getCheckName(),"",sStateId,hiddenWaitTimeTxEvent);
        oElem.removeChild(invokeElem);

        //Transition
        QDomElement ElemOnSuccessTransition = StatChildXPathMap.value(getStateTransistionXPath());

        if(waitUntil)
        {
            failStatID = sStateId;
        }
        GTASXTransition objSuccTransit;
        if (bConfigTimeExists)
            objSuccTransit.Target = configTimeStateID;
        else  if(externalTimeOutEventName.isEmpty()==false && bConfigTimeExists == false )
        {
            objSuccTransit.Target = successStatID;
        }
        else
            objSuccTransit.Target = successStatID;

        objSuccTransit.Condition = QString("(%1)==true").arg(FinalExpr);
        objSuccTransit.status = "success";


        QDomElement ElemOnFailTransit = ElemOnSuccessTransition.cloneNode().toElement();
        ElemOnFailTransit.setAttribute(SXA_STATUS,"fail");
        GTASXTransition objFailTransit;
        objFailTransit.Target = failStatID;
        objFailTransit.status = "fail";
        if (bConfigTimeExists)
            objFailTransit.Target = configTimeStateID;
        else if(externalTimeOutEventName.isEmpty()==false && bConfigTimeExists == false )
        {
            objFailTransit.status = "fail";
            objFailTransit.Target = successStatID;//failStatID ;
        }
        else  if(!waitUntil)
        {
            objFailTransit.Target = failStatID;//successStatID;
        }

        objFailTransit.Condition = QString("(%1)==false").arg(FinalExpr);

        QDomElement dummyStateElem;
        if(!IsPIRSCXMLConfig)
            oElem.insertAfter(ElemOnFailTransit,ElemOnSuccessTransition);
        else
        {
            if(!ParamListForInvoke.isEmpty())
            {
                dummyStateElem = iDomDoc.createElement(SXN_STATE);
                dummyStateElem.setAttribute(SXA_ID,QString("%1_dummyState").arg(sStateId));
                dummyStateElem.appendChild(ElemOnSuccessTransition);
                dummyStateElem.appendChild(ElemOnFailTransit);

                oElem.appendChild(dummyStateElem);
            }
            else
                oElem.insertAfter(ElemOnFailTransit,ElemOnSuccessTransition);

        }
        bool delayCheck = false;
        QString delayEventName = QString("%1_%2").arg(sStateId,"DelayCheckCond");

        if(ParamListForInvoke.isEmpty())
        {
            delayCheck = true;
            ElemOnSuccessTransition.setAttribute(SXA_EVENT,delayEventName);
            ElemOnFailTransit.setAttribute(SXA_EVENT,delayEventName);
            objSuccTransit.Event = delayEventName;
            objFailTransit.Event = delayEventName;
        }
        else
        {
            delayCheck =false;
            if(!IsPIRSCXMLConfig)
            {
                if(!hiddenWaitTimeTxEvent.isEmpty())
                {
                    ElemOnSuccessTransition.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                    ElemOnFailTransit.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                    objSuccTransit.Event = hiddenWaitTimeTxEvent;
                    objFailTransit.Event = hiddenWaitTimeTxEvent;
                }
                else
                {
                    ElemOnSuccessTransition.setAttribute(SXA_EVENT,"done.invoke._all");
                    ElemOnFailTransit.setAttribute(SXA_EVENT,"done.invoke._all");
                    objSuccTransit.Event = "done.invoke._all";
                    objFailTransit.Event = "done.invoke._all";
                }
            }
            else
            {
                if(timeout>0)
                {
                    ElemOnSuccessTransition.setAttribute(SXA_EVENT,delayEventName);
                    ElemOnFailTransit.setAttribute(SXA_EVENT,delayEventName);
                    objSuccTransit.Event = delayEventName;
                    objFailTransit.Event = delayEventName;
                }
                else
                {
                    ElemOnSuccessTransition.removeAttribute(SXA_EVENT);
                    ElemOnFailTransit.removeAttribute(SXA_EVENT);
                }
            }
        }
        if(bConfigTimeExists)
        {
            objFailTransit.Target = failStatID;
            if(!IsPIRSCXMLConfig)
            {
                if(!hiddenWaitTimeTxEvent.isEmpty())
                    objFailTransit.Event = hiddenWaitTimeTxEvent;
                else
                    objFailTransit.Event = "done.invoke._all";
            }
            else
                objFailTransit.Event = delayEventName;
            //QString("%1_TimeOut_Event").arg(iStateId);
        }

        setTransitionAttr(ElemOnSuccessTransition,objSuccTransit);
        setTransitionAttr(ElemOnFailTransit,objFailTransit);

        if(waitUntil == false)
        {
            QString checkStatus = QString("%1_Status").arg(sStateId);
            //Success Sate
            GTAScxmlLogMessage objLog;
            objLog.LOD = GTAScxmlLogMessage::Main;
            objLog.Identifier    = pChk->getInstanceName();
            objLog.ExpectedValue = pChk->getStatement().replace("\n","  ").replace("\"","");
            objLog.CurrentValue = "ExpectedValue";
            objLog.Status = GTAScxmlLogMessage::Success;
            objLog.Result = GTAScxmlLogMessage::OK;

            GTASXAssign objAssignOnSuccess(checkStatus,"'Success'");

            GTASXLog objLogOnSuccess("'info'",objLog.getLogMessage());

            createTransitionChildren(iDomDoc,ElemOnSuccessTransition,objAssignOnSuccess,objLogOnSuccess,objSuccTransit.Condition,QString("ExpectedValue"));

            GTASXAssign objAssignOnFail(checkStatus,"'Fail'");
            GTASXLog objLogOnFail("'info'","");
            //Fail State
            objLog.ExpectedValue =  pChk->getStatement().replace("\n","  ").replace("\"","");
            objLog.CurrentValue = "ExpectedValue";
            objLog.Status = GTAScxmlLogMessage::Fail;
            objLog.Result = GTAScxmlLogMessage::KO;
            objLogOnFail.Label = "'info'";
            objLogOnFail.Expr = objLog.getLogMessage();
            createTransitionChildren(iDomDoc,ElemOnFailTransit,objAssignOnFail,objLogOnFail,objFailTransit.Condition,QString("ExpectedValue"));

        }
        else
        {
            //Success Sate
            GTAScxmlLogMessage objLog;
            objLog.LOD = GTAScxmlLogMessage::Main;
            objLog.Identifier    = pChk->getInstanceName();
            QString expectedValue = pChk->getStatement().replace("\n","  ").replace("\"","");
            expectedValue.replace("`","");
            objLog.ExpectedValue = expectedValue;
            objLog.CurrentValue = "ExpectedValue";
            objLog.Status = GTAScxmlLogMessage::Success;
            objLog.Result = GTAScxmlLogMessage::OK;

            QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
            ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
            ElemOnTrueLog.setAttribute(SXA_EXPR,objLog.getLogMessage());

            QDomNodeList AssignNodeList = ElemOnSuccessTransition.elementsByTagName(SXN_ASSIGN);
            //TODO: Should not assign (a = ...) here but I don't want to touch any functionality in scope of this task, enable warning 4706 after fixed
            for(int a = 0; a = AssignNodeList.count(); a++)
                ElemOnSuccessTransition.removeChild(AssignNodeList.at(a));

            QDomElement ElemOnAssignCond = iDomDoc.createElement(SXN_ASSIGN);
            ElemOnAssignCond.setAttribute(SXA_LOCATION,"ExpectedValue");
            ElemOnAssignCond.setAttribute(SXA_EXPR,objSuccTransit.Condition);
            ElemOnSuccessTransition.appendChild(ElemOnAssignCond);
            ElemOnSuccessTransition.appendChild(ElemOnTrueLog);

            //Fail State
            objLog.ExpectedValue =   pChk->getStatement().replace("\n","  ").replace("\"","");
            objLog.CurrentValue = "ExpectedValue";
            if(bConfigTimeExists)
                objLog.Status = GTAScxmlLogMessage::TimeOut;
            else
                objLog.Status = GTAScxmlLogMessage::Fail;
            if(repeatLog && externalTimeOutEventName.isEmpty())
            {
                objLog.Result = GTAScxmlLogMessage::kNa;
                objLog.LOD = GTAScxmlLogMessage::Detail;
            }
            else if(!externalTimeOutEventName.isEmpty())
            {
                objLog.Result = GTAScxmlLogMessage::KO;
                objLog.LOD = GTAScxmlLogMessage::Main;
            }
            else
            {
                objLog.LOD = GTAScxmlLogMessage::Detail;
            }

            QDomElement ElemOnFailLog = ElemOnTrueLog.cloneNode().toElement();
            ElemOnFailLog.setAttribute(SXA_EXPR,objLog.getLogMessage());

            QDomNodeList Assign1NodeList = ElemOnFailTransit.elementsByTagName(SXN_ASSIGN);
            //TODO: Should not assign (a = ...) here but I don't want to touch any functionality in scope of this task, enable warning 4706 after fixed
            for(int a = 0; a = Assign1NodeList.count(); a++)
                ElemOnFailTransit.removeChild(Assign1NodeList.at(a));

            QDomElement ElemOnAssignCond1 = iDomDoc.createElement(SXN_ASSIGN);
            ElemOnAssignCond1.setAttribute(SXA_LOCATION,"ExpectedValue");
            ElemOnAssignCond1.setAttribute(SXA_EXPR,objFailTransit.Condition);
            ElemOnFailTransit.appendChild(ElemOnAssignCond1);


            if(repeatLog || !externalTimeOutEventName.isEmpty())
                ElemOnFailTransit.appendChild(ElemOnFailLog);


            if(ParamListForInvoke.isEmpty())
            {
                delayCheck = true;
                //ElemOnSuccessTransition.removeAttribute(SXA_EVENT);
                //ElemOnFailTransit.removeAttribute(SXA_EVENT);
            }



        }


        //Action on Fail
        GTAScxmlLogMessage objLog;
        objLog.LOD = GTAScxmlLogMessage::Main;
        objLog.Identifier    = pChk->getInstanceName();
        QString expectedValue = pChk->getStatement().replace("\n","  ").replace("\"","");
        expectedValue.replace("`","");
        objLog.ExpectedValue = expectedValue;
        objLog.CurrentValue = "'TimeOut'";
        objLog.Status = GTAScxmlLogMessage::TimeOut;
        objLog.Result = GTAScxmlLogMessage::KO;
        QString logMsgTimeOut = objLog.getLogMessage();
        logMsgTimeOut = logMsgTimeOut.remove("+");
        logMsgTimeOut = logMsgTimeOut.remove("'");
        logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);
        QDomElement dummyOnEntryElem;
        QString timeoutName="";
        if(waitUntil)
        {
            if(externalTimeOutEventName.isEmpty()==true)
            {

                if(dummyStateElem.isNull())
                {
                    createActionOnFail(iDomDoc,oElem,iStateId,iTargetId,actionOnFail,timeOut,logMsgTimeOut,delayCheck,delayEventName,true,false);
                }


            }
            if(!dummyStateElem.isNull() && IsPIRSCXMLConfig)
            {
                QString confTimeString = "";
                if((!(timeout>0)) && (!externalTimeOutEventName.isEmpty()))
                {
                    timeOut = iConfTimeInMs;
                    confTimeString = "conf_";
                }

                QString newTimeout = timeOut;
                newTimeout = newTimeout.replace("ms","");
                timeout = newTimeout.toDouble();
                QDomNodeList onEntryList = dummyStateElem.elementsByTagName(SXN_ONENTRY);

                if(!onEntryList.isEmpty())
                {
                    dummyOnEntryElem = onEntryList.at(0).toElement();
                }
                else
                {
                    dummyOnEntryElem = iDomDoc.createElement(SXN_ONENTRY);
                }
                QDomElement ElemOnSend = iDomDoc.createElement(SXN_SEND);
                ElemOnSend.setAttribute(SXA_EVENT,delayEventName);
                ElemOnSend.setAttribute(SXA_DELAY,"100ms");
                dummyOnEntryElem.appendChild(ElemOnSend);
                dummyStateElem.appendChild(dummyOnEntryElem);
                if(timeout>0)
                {
                    QDomElement dummyAssignElem3 = iDomDoc.createElement(SXN_ASSIGN);
                    QDomElement dummyAssignElem1 = iDomDoc.createElement(SXN_ASSIGN);
                    dummyAssignElem1.setAttribute(SXA_EXPR,"Date.now()");
                    dummyAssignElem1.setAttribute(SXA_LOCATION,QString("%1_%2timeout_NewTime").arg(iStateId,confTimeString));
                    QDomElement dummyAssignElem2 = iDomDoc.createElement(SXN_ASSIGN);
                    if(externalTimeOutEventName.isEmpty())
                    {
                        timeoutName = QString("%1_timeout_StartTime").arg(iStateId);
                        dummyAssignElem2.setAttribute(SXA_EXPR,QString("%1_%2timeout_NewTime - %1_timeout_StartTime").arg(iStateId,confTimeString));
                    }
                    else
                    {
                        dummyAssignElem2.setAttribute(SXA_EXPR,QString("%1_%2timeout_NewTime - %3_Config_StartTime").arg(iStateId,confTimeString,iconfigStateId));

                        dummyAssignElem3.setAttribute(SXA_EXPR,QString("%1_%2timeout_NewTime - %3").arg(iStateId,confTimeString,iTimeoutName));
                        dummyAssignElem3.setAttribute(SXA_LOCATION,QString("%1_timeout_elapsed").arg(iStateId));

                    }

                    dummyAssignElem2.setAttribute(SXA_LOCATION,QString("%1_%2timeout_elapsed").arg(iStateId,confTimeString));

                    dummyOnEntryElem.appendChild(dummyAssignElem1);
                    dummyOnEntryElem.appendChild(dummyAssignElem2);
                    if(!iTimeoutName.isEmpty())
                        dummyOnEntryElem.appendChild(dummyAssignElem3);

                    QDomNodeList txNodeList = dummyStateElem.elementsByTagName(SXN_TRANSITION);
                    if(!txNodeList.isEmpty())
                    {
                        for(int i=0;i<txNodeList.count();i++)
                        {
                            QString status = txNodeList.at(i).toElement().attribute(SXA_STATUS);

                            QString condition = txNodeList.at(i).toElement().attribute(SXA_CONDITION);
                            QString newCond = QString("((%1) && (%2_%4timeout_elapsed < %3))").arg(condition,iStateId,newTimeout,confTimeString);

                            if((!externalTimeOutEventName.isEmpty()) && (status.contains("success")))
                            {
                                QString newCond1 = QString("((%1) && (%2_%4timeout_elapsed >= %3))").arg(condition,iStateId,newTimeout,confTimeString);
                                txNodeList.at(i).toElement().setAttribute(SXA_CONDITION,newCond1);
                                txNodeList.at(i).toElement().setAttribute(SXA_EVENT,delayEventName);
                            }
                            else
                            {
                                txNodeList.at(i).toElement().setAttribute(SXA_CONDITION,newCond);
                                txNodeList.at(i).toElement().setAttribute(SXA_EVENT,delayEventName);
                            }
                        }
                    }

                    //createActionOnFail(iDomDoc,dummyStateElem,iStateId,iTargetId,actionOnFail,timeOut,logMsgTimeOut,delayCheck,delayEventName,true,false);
                    QDomElement timeoutTx = iDomDoc.createElement(SXN_TRANSITION);
                    timeoutTx.setAttribute(SXA_STATUS,"fail");
                    if(actionOnFail.contains("stop"))
                        timeoutTx.setAttribute(SXA_TARGET,MAIN_FINAL_STATE);
                    else
                        timeoutTx.setAttribute(SXA_TARGET,iTargetId);
                    timeoutTx.setAttribute(SXA_EVENT,delayEventName);
                    if(externalTimeOutEventName.isEmpty())
                        timeoutTx.setAttribute(SXA_CONDITION,QString("%1_timeout_elapsed >= %2").arg(iStateId,newTimeout));
                    else
                    {
                        QString timeoutDelay =iTimeOutDelay;
                        timeoutDelay = timeoutDelay.replace("ms","");
                        timeoutTx.setAttribute(SXA_CONDITION,QString("%1_timeout_elapsed >= %2").arg(iStateId,timeoutDelay));
                    }
                    QDomElement timeoutTxLog = iDomDoc.createElement(SXN_LOG);
                    timeoutTxLog.setAttribute(SXA_EXPR,logMsgTimeOut);
                    timeoutTxLog.setAttribute(SXA_LABEL,"Error");
                    timeoutTx.appendChild(timeoutTxLog);
                    dummyStateElem.appendChild(timeoutTx);
                }

            }
            else
            {

            }
        }
        else
        {
            if(delayCheck == true)
            {
                QDomElement ElemOnEntry = iDomDoc.createElement(SXN_ONENTRY);
                if(! ElemOnEntry.isNull())
                {
                    QDomElement ElemOnSend = iDomDoc.createElement(SXN_SEND);
                    ElemOnSend.setAttribute(SXA_EVENT,delayEventName);
                    ElemOnSend.setAttribute(SXA_DELAY,"100ms");
                    ElemOnEntry.appendChild(ElemOnSend);
                    QDomNodeList txNodeList = oElem.elementsByTagName(SXN_TRANSITION);
                    if(!txNodeList.isEmpty())
                    {
                        oElem.insertBefore(ElemOnEntry,txNodeList.at(0));
                    }
                    else
                    {
                        oElem.appendChild(ElemOnEntry);
                    }
                }
            }
        }

        //Logging all Parameters
        QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
        oElem.appendChild(ElemOnExit);
        ListOfVarToLog.append(dumpListParams);
        if(bConfigTimeExists)
        {
            QDomElement onExitLogElem = iDomDoc.createElement(SXN_LOG);
            onExitLogElem.setAttribute(SXA_EXPR,ConfTimeoutLog);
            //ElemOnExit.appendChild(onExitLogElem);

        }

        //        if(IsPIRSCXMLConfig)
        //        {
        //            QDomElement ElemOnSend = iDomDoc.createElement(SXN_SEND);
        //            ElemOnSend.setAttribute(SXA_EVENT,delayEventName);
        //            ElemOnSend.setAttribute(SXA_DELAY,"10ms");

        //            QDomElement ElemOnEntry = iDomDoc.createElement(SXN_ONENTRY);
        //            oElem.appendChild(ElemOnEntry);

        //            if(!ElemOnEntry.isNull())
        //            {
        //                ElemOnEntry.appendChild(ElemOnSend);

        //            }
        //        }

        insertVariableOnExit(ElemOnExit,ListOfVarToLog,sStateId,pChk->getInstanceName(),iDomDoc);


        //time out in case if parent is a while or do while statement
        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pChk->getParent());
        if(pParent != NULL)
        {
            createLoopTimeOutTranstions(oElem,iDomDoc,sStateId,pParent);
        }

        createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pChk,failStatID);
        iRootElem.appendChild(oElem);

        //        if(externalTimeOutEventName.isEmpty()==false)
        //        {
        //            QDomNodeList transitionNodes = oElem.elementsByTagName(SXN_TRANSITION);
        //            int transitNodeSize = transitionNodes.size();
        //            QList<QDomNode> configTxrNodes;
        //            if (transitionNodes.isEmpty()==false)
        //            {
        //                for(int i=0;i<transitNodeSize;i++)
        //                {
        //                    QDomElement transElemToAppend = transitionNodes.at(i).toElement();
        //                    //QDomElement transElemToAppend = transNode.cloneNode(true).toElement();
        //                    if (transElemToAppend.attribute(SXA_STATUS) == "success")
        //                    {
        //                        transElemToAppend.setAttribute(SXA_TARGET,iStateId);
        //                        transElemToAppend.setAttribute(SXA_EVENT,externalTimeOutEventName);
        //                        //configTxrNodes.append(transElemToAppend);

        //                    }
        //                    if (transElemToAppend.attribute(SXA_STATUS) == "fail")
        //                    {
        //                        transElemToAppend.setAttribute(SXA_TARGET,iTargetId);
        //                        //transElemToAppend.setAttribute(SXA_EVENT,externalTimeOutEventName);
        //                        //transElemToAppend.setAttribute(SXA_TARGET,configtimeFailTxxn);
        //                        //configTxrNodes.append(transElemToAppend);
        //                    }
        //                }

        //                //                foreach(QDomNode node, configTxrNodes)
        //                //                    oElem.appendChild(node);
        //            }
        //        }

        //&& !ParamListForInvoke.isEmpty()

        //        if(IsPIRSCXMLConfig)
        //        {
        //            QDomNodeList finalList = oElem.elementsByTagName(SXN_FINAL);
        //            if(finalList.isEmpty())
        //            {
        //                QDomElement finalElem = iDomDoc.createElement(SXN_FINAL);
        //                finalElem.setAttribute(SXA_ID,QString("%1_Final").arg(sStateId));
        //                oElem.appendChild(finalElem);
        //            }
        //        }

        if((!IsPIRSCXMLConfig) && !externalTimeOutEventName.isEmpty())
            ElemOnSuccessTransition.setAttribute(SXA_EVENT,externalTimeOutEventName);
        QDomElement txConfFailElem = ElemOnSuccessTransition.cloneNode(true).toElement();

        if(externalTimeOutEventName.isEmpty()==false)
        {
            if(!IsPIRSCXMLConfig)
            {
                if(!hiddenWaitTimeTxEvent.isEmpty())
                    txConfFailElem.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                else
                    txConfFailElem.setAttribute(SXA_EVENT,"done.invoke._all");
            }
            txConfFailElem.setAttribute(SXA_TARGET,failStatID);
            if(IsPIRSCXMLConfig)
            {
                dummyStateElem.appendChild(txConfFailElem);
                txConfFailElem.setAttribute(SXA_STATUS,"Info");
                QString cond = txConfFailElem.attribute(SXA_CONDITION);
                if(cond.contains("elapsed >="))
                {
                    cond.replace("elapsed >=","elapsed <");
                    txConfFailElem.setAttribute(SXA_CONDITION,cond);
                }
            }
            else
                oElem.appendChild(txConfFailElem);
        }
        if (bConfigTimeExists)
        {
            // double precision = 0.0;
            checkValueForConfTime.setWaitUntil(pChk->getWaitUntil());
            checkValueForConfTime.setActionOnFail(actionOnFail);
            checkValueForConfTime.setConfCheckTime(0,ACT_TIMEOUT_UNIT_SEC);
            checkValueForConfTime.setTimeOut(0,ACT_TIMEOUT_UNIT_SEC);
            checkValueForConfTime.setParent(pChk->getParent());
            iRootElem.appendChild(ConfTimeStateElem);
            checkValueForConfTime.setInstanceName(pChk->getInstanceName());
            QDomElement configTimeState;
            createCheckValueState(&checkValueForConfTime,iDomDoc,iRootElem,confTimeTarget,iTargetId,failStatID,false,configTimeEventName,configTimeState,conftimeInMiliSec,configTimeStateID,timeOut,timeoutName);

            if(waitUntil)
            {
                delayEventName = QString("%1_%2").arg(iTargetId,"DelayCheckCond");

                createActionOnFail(iDomDoc,configTimeState,iStateId,iTargetId,actionOnFail,timeOut,logMsgTimeOut,delayCheck,delayEventName,true,false);


            }
        }
    }
    return rc;
}

/**
  * This function create the scxml state for a manual action
  * pCmd: Manual action command
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iStateId: State id of the command;
  * iTargetId: The next ID where the stat should go after execution
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::createActionManualState(const GTAActionManual *pCmd,QDomDocument & iDomDoc, QDomElement & iRootElem, const QString & iStateId, const QString & iTargetId,const QMap<QString,double>&)
{
    bool rc = false;
    TestConfig* testConf = TestConfig::getInstance();
    QString manActTimeout = QString::fromStdString(testConf->getManualActionTimeoutValue());	

    if(pCmd != NULL)
    {
        QString VarToLog;
        QString actionName = pCmd->getAction();
        QString message = pCmd->getMessage();
        bool userAckno  = pCmd->getAcknowledgetment();
        bool userFeedback = pCmd->getUserFeedback();
        QString invokeId = QString(iStateId).replace('s','i');
        QString instanceName = pCmd->getInstanceName();
        QString messageForSCXML = message.replace("\n","<br></br>");
        messageForSCXML.replace("`","_");

        QString delayEventId = "";
        QString txEventName = QString("done.invoke.%1").arg(invokeId);

        /* QRegExp regEx("\\n|\\r");
        if (message.contains(regEx))
        {
            message.replace(QRegExp("\\n"),"\\n");
        }*/
        QString oVar = pCmd->getParameter();
        updateWithGenToolParam(oVar);
        QString argVal = "false";
        QString fileName;
        if(userFeedback)
        {
            fileName = actionName.replace(" ","_") + "_feedback.scxml";
            argVal = pCmd->getReturnType();
            argVal = (argVal == "YesNo") ? "bool" : argVal;
        }
        else
        {
            fileName = actionName.replace(" ","_") + "_acknowledgement.scxml";
            if(userAckno)
                argVal = "true";
        }
        //QDomDocument domDoc;
        //rc =openScxmlTemplateFile(fileName,domDoc);
        QDomElement oElem;
        rc = getCloneRootElement(fileName,iDomDoc,oElem);
        if(rc && (!oElem.isNull()))
        {
            //oElem = domDoc.documentElement();
            setStateIDName(iStateId,pCmd);
            oElem.setAttribute(SXA_ID,iStateId);
            iRootElem.appendChild(oElem);

            bool validAppDataModel = insertValidAppDataModel(oVar);
            QString sTargetID = iTargetId;
            QString localVar=oVar;

            if(validAppDataModel)
            {
                localVar =  QString("localVar%1").arg(iStateId);
                sTargetID=QString("ManualSet_%1").arg(iStateId);
            }
            QDomNodeList domNodeList = oElem.elementsByTagName(SXN_TRANSITION);
            QDomElement transElem;
            if(domNodeList.count()>0)
            {
                transElem = domNodeList.at(0).toElement();
                transElem.setAttribute(SXA_EVENT,txEventName);
                transElem.setAttribute(SXA_TARGET,sTargetID);
                transElem.setAttribute(SXA_STATUS,"success");
                QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                GTAScxmlLogMessage objLog;
                objLog.LOD = GTAScxmlLogMessage::Main;
                objLog.Identifier = instanceName;
                objLog.Result = GTAScxmlLogMessage::OK;
                objLog.Status = GTAScxmlLogMessage::Success;
                QString logMsg1 = "";
                if(userFeedback)
                {
                    if(validAppDataModel)
                        objLog.CurrentValue =localVar;
                    else
                        objLog.CurrentValue = oVar;
                    objLog.ExpectedValue = oVar;
                    logMsg1 = objLog.getLogMessage();
                }
                else
                {
                    logMsg1 = objLog.getLogMessage();
                    logMsg1 = logMsg1.remove("+");
                    logMsg1 = logMsg1.remove("'");
                    logMsg1 = QString("'%1'").arg(logMsg1);
                }
                ElemOnLog.setAttribute(SXA_EXPR,logMsg1);
                transElem.appendChild(ElemOnLog);
            }

            domNodeList = oElem.elementsByTagName(SXN_INVOKE);
            if(domNodeList.count()>0)
            {
                QDomElement invokeElem = domNodeList.at(0).toElement();
                invokeElem.setAttribute(SXA_ID,invokeId);
                QString txEvent="";
                setInvokeDelay(iDomDoc,invokeElem,pCmd->getAction(),pCmd->getComplement(),txEvent,1);
                if(!txEvent.isEmpty())
                {
                    transElem.setAttribute(SXA_EVENT,txEvent);
                }
            }



            domNodeList = oElem.elementsByTagName(SXN_ARGUMENT);
            int childCount = domNodeList.count();



            for(int i = 0; i < childCount; i++)
            {
                QDomNode domNode = domNodeList.at(i);
                if(domNode.isElement())
                {
                    QDomElement domElem = domNode.toElement();
                    if(domElem.hasAttribute(SXA_NAME))
                    {
                        domElem.setAttribute(SXA_VALUE,QString("'%1'").arg(argVal));
                    }
                    else
                    {
                        
                        domElem.setAttribute(SXA_VALUE,QString("'%1'").arg(messageForSCXML));
                    }
                }
            }

            domNodeList = oElem.elementsByTagName(SXN_RETURNVALUE);
            if(domNodeList.count()>0)
            {
                QDomElement returnValElem = domNodeList.at(0).toElement();
                returnValElem.setAttribute(SXA_NAME,SX_RET_PARAM_NAME); 
            }

            if(userFeedback)
            {
                domNodeList = oElem.elementsByTagName(SXN_ASSIGN);
                if(domNodeList.count()>0)
                {
                    QDomElement assignElem = domNodeList.at(0).toElement();
                    assignElem.setAttribute(SXA_LOCATION,localVar);
                    QString expression = QString("_event.data.%1").arg(SX_RET_PARAM_NAME);
                    assignElem.setAttribute(SXA_EXPR,expression);

                    if(! oVar.isEmpty() )
                    {
                        GTAScxmlDataModel objDataMode;
                        objDataMode.ID = oVar;
                        _DataModelList.insert(oVar,objDataMode);
                        VarToLog = oVar;
                    }

                }
            }
            /*
            //This was supposed to add log to finalize, since Finalize node does not require log now it is removed
            domNodeList = oElem.elementsByTagName(SXN_LOG);
            if(domNodeList.count()>0)
            {
            QDomElement logElem = domNodeList.at(0).toElement();
            logElem.setAttribute(SXA_EXPR,QString("'%1'").arg(instanceName));
            }
            */


            //Logging all Parameters
            QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
            oElem.appendChild(ElemOnExit);
            QStringList ListOfVarToLog; ListOfVarToLog<<VarToLog;
            insertVariableOnExit(ElemOnExit,ListOfVarToLog,iStateId,instanceName,iDomDoc);

            if(validAppDataModel)
            {
                GTAActionSetList* setCommand = new GTAActionSetList;
                setCommand->setAction(ACT_SET);
                setCommand->setInstanceName(pCmd->getInstanceName());
                GTAActionSet* setInstance = new GTAActionSet;
                QList<GTAActionSet*> setList;
                setList.append(setInstance);
                GTAEquationConst* constEqn = new GTAEquationConst;
                constEqn->setConstant(localVar);
                GTAEquationBase* pBase = constEqn;
                setInstance->setEquation(pBase);
                //setInstance.setFunctionStatus(fs_N)
                setInstance->setAction(ACT_SET_INSTANCE);
                setInstance->setParameter(oVar);
                setCommand->setSetCommandlist(setList);
                setInstance->setIsSetOnlyFSFixed(false);
                setInstance->setIsSetOnlyFSVariable(false);
                setCommand->setTimeOut(ACT_DEFAULT_TIMEOUT,ACT_TIMEOUT_UNIT_SEC);
                createActionSetState(setCommand,iDomDoc,iRootElem,sTargetID,iTargetId);
                delete setCommand;


            }
            

            //time out in case if parent is a while or do while statement
            GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
            if(pParent != NULL)
            {
                createLoopTimeOutTranstions(oElem,iDomDoc,iStateId,pParent);
            }

            //time out in case if 
            createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pCmd,sTargetID);




        }
    }
    return rc;
}
/**
  * This function create the scxml state for a "if" action
  * pCmd: if/else command
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iStateId: State id of the command;
  * iTrueStateId: The next ID if the condition is true
  * iFalseStateId: The next ID if the condition is false
  * iStatOfSibling: The ID of Sibling command node below if
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::createActionIfState(const GTAActionIF * pCmd, QDomDocument & iDomDoc, QDomElement & iRootElem, const QString & iStatId,
                                            const QString & iTrueStateId, const QString iFalseStateId, const QString & iStatOfSibling,QDomElement& ioElem)
{
    bool rc = false;
    if(pCmd == NULL)
        return rc;

    rc = true;
    QString param = pCmd->getParameter();
    QString paramVal = pCmd->getValue();
    updateWithGenToolParam(param);
    updateWithGenToolParam(paramVal);
    QString condition =pCmd->getCondition();
    QString sActionName = pCmd->getAction();
    QString sComplement = pCmd->getComplement();
    QString instanceName = pCmd->getInstanceName();
    QString timeOutInMiliSec = getTimeOutInMiliSecond((GTAActionIF*)pCmd);
    QString conftimeInMiliSec = getConfTimeInMiliSecond((GTAActionIF*)pCmd);
    QString sStateId = iStatId+"_1";
    QString configTimeStateID;
    bool bConfigTimeExists=false;
    QDomElement ConfTimeStateElem;
    QDomElement elementOnConfTimeState;
    GTACheckValue checkValue;
    QString ConfirmParam;
    QString ConfirmParamVal;
    QString hiddenWaitTimeTxEvent;

    bool isOnlyFS = pCmd->getIsFsOnly();
    bool isOnlyValue = pCmd->getIsValueOnly();
    QString fs = pCmd->getFunctionalStatus();

    _hasChannelInControl = false;

    if(paramVal.startsWith("\"") && paramVal.endsWith("\""))
        paramVal.replace("\"","'");

    if(param.startsWith("\"") && param.endsWith("\""))
        param.replace("\"","'");

    if(param.contains(GTA_CIC_IDENTIFIER) || paramVal.contains(GTA_CIC_IDENTIFIER))
        _hasChannelInControl = true;


    QString actionOnFail = pCmd->getActionOnFail();
    QStringList dumpListParams = pCmd->getDumpList();
    bool chkAudioAlarm  = pCmd->getChkAudioAlarm();
    QString heardAfter = pCmd->getHeardAfter();
    if(chkAudioAlarm)
    {
        GTACheckAudioAlarm sAudioAlarm;
        sAudioAlarm.setDumpList(dumpListParams);
        sAudioAlarm.setInstanceName(instanceName);
        sAudioAlarm.setValue(paramVal);
        sAudioAlarm.setTime(heardAfter);
        rc = createCheckAudioAlarm(&sAudioAlarm,iDomDoc,iRootElem,sStateId,iTrueStateId,iFalseStateId,condition);
        return rc;
    }

    QString sPrecisionVal, sPrecisonUnit; 
    pCmd->getPrecision(sPrecisionVal, sPrecisonUnit);

    double dPrecisionVal = sPrecisionVal.toDouble();

    if(sPrecisonUnit == ACT_PRECISION_UNIT_PERCENT)
    {
        dPrecisionVal = dPrecisionVal*0.01;
    }


    QString sFileName = sActionName + "_"+ sComplement + ".scxml";
    QDomElement oElem;
    rc = getCloneRootElement(sFileName,iDomDoc,oElem);
    QDomElement nestedIfState = iDomDoc.createElement(SXN_STATE);
    iRootElem.appendChild(nestedIfState);
    ioElem = nestedIfState;
    QString initialTarget,timeOutStatename;
    if (rc && ! oElem.isNull())
    {  
        QDomElement oTimeOutElem;
        QString TimeOutTarget = QString("%1_1").arg(sStateId);
        rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOutInMiliSec,TimeOutTarget);
        timeOutStatename=sStateId;
        if (rc)
        {
            initialTarget=sStateId;
            sStateId = TimeOutTarget;
            nestedIfState.appendChild(oTimeOutElem);
            
        }
        else
            initialTarget = sStateId;
        

        nestedIfState.setAttribute(SXA_ID,iStatId);
        QDomElement nesteIfTxnState = iDomDoc.createElement(SXN_TRANSITION);
        nesteIfTxnState.setAttribute(SXA_TARGET,iStatOfSibling);
        nestedIfState.appendChild(nesteIfTxnState);
        
        
        QString confTimeTarget = QString("%1_2").arg(sStateId);
        configTimeStateID= QString("%1_Confirmation_time").arg(sStateId);
        QString configTimeEventName;  
        bConfigTimeExists = getConfigurationTimeNode(iDomDoc,ConfTimeStateElem,configTimeStateID,conftimeInMiliSec,confTimeTarget,configTimeEventName);


        QStringList VarList;
        //get fail and pass expressions
        QString ExprTrue, ExprFalse;
        getChannelInControlCondition(param,paramVal,condition,ExprTrue,ExprFalse,false,dPrecisionVal,sPrecisonUnit,VarList);


        QString StatusExprTrue, StatusExprFalse;
        QString fsCond;
        if(fs.trimmed().isEmpty())
        {
            fs = "NO";
        }
        if(fs == ACT_FSSSM_NOT_LOST)
        {
            fs = ACT_FSSSM_LOST;
            if(condition == ARITH_NOTEQ)
                fsCond = ARITH_EQ;
            else if(condition == ARITH_EQ)
                fsCond = ARITH_NOTEQ;
            else
                fsCond = ARITH_EQ;
        }
        else
        {
            if((condition != ARITH_NOTEQ)|| (condition == ARITH_EQ)) 
                fsCond = ARITH_EQ;
        }
        if(fsCond.trimmed().isEmpty())
        {
            fsCond = condition;
        }
        getChannelInControlCondition(param,QString("'%1'").arg(fs),fsCond,StatusExprTrue,StatusExprFalse,false,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);

        if(_ParamList.contains(param))
        {
            if(isOnlyFS)
            {
                ExprTrue = StatusExprTrue;
                ExprFalse = StatusExprFalse;
            }
            else if(!(isOnlyFS || isOnlyValue))
            {
                if(condition == ARITH_NOTEQ)
                {
                    ExprTrue = QString("((%1) || (%2))").arg(ExprTrue,StatusExprTrue);
                    ExprFalse  = QString("((%1) && (%2))").arg(ExprFalse ,StatusExprFalse);
                }
                else
                {
                    ExprTrue = QString("((%1) && (%2))").arg(ExprTrue,StatusExprTrue);
                    ExprFalse  = QString("((%1) || (%2))").arg(ExprFalse ,StatusExprFalse);
                }
            }
        }
        //VarList<<param<<paramVal;

        QStringList ParamList = getValidParametersToInvoke(VarList);
        QStringList dumpListParamForinvoke = getValidParametersToInvoke(dumpListParams);        
        QStringList allParamsForInvoke = QStringList()<<ParamList<<dumpListParamForinvoke;

        if (bConfigTimeExists)
            setStateAttr(oElem,confTimeTarget);
        else
        {
            setStateAttr(oElem,sStateId);
            setStateIDName(sStateId,pCmd);
        }

        QMap<QString,QDomElement> StatChildXPathMap;
        getScxmlElementPathMap(oElem,oElem.nodeName(),StatChildXPathMap);

        QDomElement invokeElem = StatChildXPathMap.value(getStateInvokeXPath());

        QString invokeID = QString(sStateId).replace("s","i");
        //createInvokeNodes(oElem,invokeElem,allParamsForInvoke,invokeID,"ParameterID","Info",instanceName,false,pCmd->getInvokeNodeDelayInMs());
        createInvokeStateNodes(iDomDoc,oElem,invokeElem,allParamsForInvoke,invokeID,"ParameterID","Info",instanceName,false,pCmd->getAction(),pCmd->getComplement(),iStatId,hiddenWaitTimeTxEvent);

        oElem.removeChild(invokeElem);


        ConfirmParam = param;
        ConfirmParamVal = paramVal;
        //if(ParamList.contains(param))
        //{
        //    param.append(".Value");
        //}
        //if(ParamList.contains(paramVal))
        //{
        //    paramVal.append(".Value");
        //}
        
        //getExpresion(param,paramVal,condition,ExprTrue,ExprFalse,false,dPrecisionVal,sPrecisonUnit);

        //create transition on condition does meet
        QDomElement ElemOnTransitTrue = StatChildXPathMap.value(getStateTransistionXPath());
        GTASXTransition objTransitTrue;
        objTransitTrue.Condition = ExprTrue;
        objTransitTrue.Target = iTrueStateId ;

        objTransitTrue.status = "success";
        setTransitionAttr(ElemOnTransitTrue,objTransitTrue);

        //create transition on condition doesnot meet
        QDomElement ElemOnTransitFalse = ElemOnTransitTrue.cloneNode().toElement();
        GTASXTransition objTransitFalse;
        objTransitFalse.Condition = ExprFalse;
        objTransitFalse.Target = iFalseStateId ;
        
        objTransitFalse.status = "fail";
        setTransitionAttr(ElemOnTransitFalse,objTransitFalse);

        bool isPirScxmlConfig = TestConfig::getInstance()->getPIRInvokeForSCXML();
        if(isPirScxmlConfig)
        {
            if(!allParamsForInvoke.isEmpty())
            {
                QDomElement dummyState = iDomDoc.createElement(SXN_STATE);
                dummyState.setAttribute(SXA_ID,QString("%1_dummyState").arg(sStateId));
                dummyState.appendChild(ElemOnTransitTrue);
                dummyState.appendChild(ElemOnTransitFalse);
                oElem.appendChild(dummyState);
            }
            else
            {
                oElem.insertAfter(ElemOnTransitFalse,ElemOnTransitTrue);
            }
        }

        if(!isPirScxmlConfig)
        {
            oElem.insertAfter(ElemOnTransitFalse,ElemOnTransitTrue);
        }

        //Log Node
        GTAScxmlLogMessage objTrueLog;
        objTrueLog.LOD = GTAScxmlLogMessage::Main;
        objTrueLog.Identifier = instanceName;

        GTAScxmlDataModel objTF;
        objTF.ID = "TRUE_VAR";
        objTF.Expr = "'true'";
        _DataModelList.insert(objTF.ID,objTF);
        objTF.ID = "FALSE_VAR";
        objTF.Expr = "'false'";
        _DataModelList.insert(objTF.ID,objTF);

        objTrueLog.ExpectedValue    = QString("'+%1+'").arg("ExpectedValue");
        objTrueLog.CurrentValue     = QString("TRUE_VAR") ;//QString("'%1'").arg("true");//QString("'+%1+'").arg(paramVal);
        objTrueLog.Status = GTAScxmlLogMessage::Success;
        objTrueLog.Result = GTAScxmlLogMessage::OK;

        QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
        ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
        ElemOnTrueLog.setAttribute(SXA_EXPR,objTrueLog.getLogMessage());

        QDomElement trueExprAssignElem = iDomDoc.createElement(SXN_ASSIGN);
        trueExprAssignElem.setAttribute(SXA_LOCATION,"ExpectedValue");
        trueExprAssignElem.setAttribute(SXA_EXPR,ExprTrue);
        ElemOnTransitTrue.appendChild(trueExprAssignElem);

        ElemOnTransitTrue.appendChild(ElemOnTrueLog);

        GTAScxmlLogMessage objFalseLog = objTrueLog;
        objFalseLog.CurrentValue = QString("FALSE_VAR") ;//QString("'%1'").arg("true");
        objFalseLog.LOD = GTAScxmlLogMessage::Main;
        objFalseLog.Status = GTAScxmlLogMessage::Success;
        objFalseLog.Result = GTAScxmlLogMessage::OK;
        QDomElement ElemOnFalseLog = ElemOnTrueLog.cloneNode().toElement();
        ElemOnFalseLog.setAttribute(SXA_EXPR,objFalseLog.getLogMessage());

        QDomElement falseExprAssignElem = iDomDoc.createElement(SXN_ASSIGN);
        falseExprAssignElem.setAttribute(SXA_LOCATION,"ExpectedValue");
        falseExprAssignElem.setAttribute(SXA_EXPR,ExprFalse);
        ElemOnTransitFalse.appendChild(falseExprAssignElem);

        ElemOnTransitFalse.appendChild(ElemOnFalseLog);


        QString eventName = QString("%1_%2").arg(sStateId,"DelayCheckCond");
        bool delayEvenChk = false;
        if(ParamList.isEmpty())
        {
            delayEvenChk  = true;
            ElemOnTransitTrue.setAttribute(SXA_EVENT,eventName);
            ElemOnTransitFalse.setAttribute(SXA_EVENT,eventName);
        }
        else
        {
            if(hiddenWaitTimeTxEvent.isEmpty())
            {
                ElemOnTransitTrue.setAttribute(SXA_EVENT,"done.invoke._all");
                ElemOnTransitFalse.setAttribute(SXA_EVENT,"done.invoke._all");
            }
            else
            {
                ElemOnTransitTrue.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                ElemOnTransitFalse.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);

            }
        }
        if(isPirScxmlConfig)
        {
            ElemOnTransitTrue.removeAttribute(SXA_EVENT);
            ElemOnTransitFalse.removeAttribute(SXA_EVENT);
        }
        //Action on Fail
        GTAScxmlLogMessage objLog;
        objLog.LOD = GTAScxmlLogMessage::Main;
        objLog.Identifier    = pCmd->getInstanceName();
        objLog.ExpectedValue = "True";
        objLog.CurrentValue = "'TimeOut'";
        objLog.Status = GTAScxmlLogMessage::TimeOut;
        objLog.Result = GTAScxmlLogMessage::KO;
        QString logMsgTimeOut = objLog.getLogMessage();
        logMsgTimeOut = logMsgTimeOut.remove("+");
        logMsgTimeOut = logMsgTimeOut.remove("'");
        logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);

        createActionOnFail(iDomDoc,oElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,delayEvenChk,eventName);

        //Logging all Parameters
        QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
        oElem.appendChild(ElemOnExit);
        VarList.append(dumpListParams);
        insertVariableOnExit(ElemOnExit,VarList,sStateId,instanceName,iDomDoc);

        //time out in case if parent is a while or do while statement
        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
        if(pParent != NULL)
        {
            if(bConfigTimeExists)
                createLoopTimeOutTranstions(oElem,iDomDoc,confTimeTarget,pParent);
            else
                createLoopTimeOutTranstions(oElem,iDomDoc,sStateId,pParent);
        }
        createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pCmd,iFalseStateId);

        if (bConfigTimeExists)
        {
            double precision = 0.0;
            checkValue.insertParamenter(ConfirmParam,condition,ConfirmParamVal,ACT_FSSSM_NOT_LOST,precision,QString(""),ACT_PRECISION_UNIT_PERCENT);
            checkValue.setActionOnFail(actionOnFail);
            checkValue.setTimeOut(0,ACT_TIMEOUT_UNIT_SEC);
            checkValue.setParent(pCmd->getParent());
            /*QString timeOut,timeOutUnit;
            pCmd->getTimeOut(timeOut,timeOutUnit);
            checkValue.setTimeOut(timeOut,timeOutUnit);*/
            checkValue.setInstanceName(pCmd->getInstanceName());
            createCheckValueState(&checkValue,iDomDoc,nestedIfState,sStateId,configTimeStateID,iFalseStateId,true,configTimeEventName);
            nestedIfState.appendChild(ConfTimeStateElem);
        }

        QDomElement finalElem = iDomDoc.createElement(SXN_FINAL);
        finalElem.setAttribute(SXA_ID,QString("%1_Final").arg(nestedIfState.attribute(SXA_ID)));
        nestedIfState.appendChild(finalElem);
        nestedIfState.appendChild(oElem);
        nestedIfState.setAttribute(SXA_INITIALSTATE,initialTarget);




        if (bConfigTimeExists)
        {
            //sStateId = confTimeTarget;
            iRootElem.appendChild(ConfTimeStateElem);
            iRootElem.appendChild(elementOnConfTimeState);
        }


    }
    return rc;
}

/**
  * This function create the scxml state for a while action
  * pCmd: For Each command
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iStateId: State id of the command;
  * iTrueStateId: The next ID if the condition is true
  * iFalseStateId: The next ID if the condition is false
  * iStatOfSibling: The ID of Sibling command node below while
  * return true if scxml of the command is created, otherwise false
  */

bool GTAExportScxml::createActionForEachState(GTAActionForEach *pCmd, QDomDocument &iDomDoc,
                                                 QDomElement &iRootElem, QString &ioStatId,
                                                 const QString &iTrueStateId, const QString iFalseStateId,
                                                 const QString & iStatOfSibling, QDomElement& ioElem)
{
    bool rc = false;
    if(pCmd == NULL)
        return rc;

    rc = true;

    //story #358311
    QString path;
    if(pCmd->getRelativePath().isEmpty())
    {
        path = pCmd->getPath();
    }
    else
    {
        QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
        path = repoPath + pCmd->getRelativePath();
    }
    QString paramVal = QString::number(pCmd->getDelimiterIndex());
    //updateWithGenToolParam(param);
    //updateWithGenToolParam(paramVal);
    //QString condition =pCmd->getCondition();
    QString sActionName = pCmd->getAction();
    QString sComplement = pCmd->getComplement();
    QString instanceName = pCmd->getInstanceName();
    QString timeOutInMiliSec = getTimeOutInMiliSecond((GTAActionWhile*)pCmd);
    QString actionOnFail = pCmd->getActionOnFail();
    QStringList dumpListParams = pCmd->getDumpList();
    QString hiddenWaitTimeTxEvent;
    // QString conftimeInMiliSec = getConfTimeInMiliSecond((GTAActionWhile*)pCmd);
    QString sStateId= ioStatId + "_1";

    bool isPirConfigEnabled = TestConfig::getInstance()->getPIRInvokeForSCXML();
    QString ExprTrue, ExprFalse;
    //if(param.contains(GTA_CIC_IDENTIFIER) || paramVal.contains(GTA_CIC_IDENTIFIER))
    _hasChannelInControl = pCmd->hasChannelInControl();

    QString sPrecisionVal, sPrecisonUnit;
    pCmd->getPrecision(sPrecisionVal, sPrecisonUnit);

    double dPrecisionVal = sPrecisionVal.toDouble();

    if(sPrecisonUnit == ACT_PRECISION_UNIT_PERCENT)
    {
        dPrecisionVal = dPrecisionVal*0.01;
    }



    QString sFileName = sActionName + "_"+ sComplement + ".scxml";

    QDomElement oElem;
    rc = getCloneRootElement(sFileName,iDomDoc,oElem);

    QDomElement nestedForEachState = iDomDoc.createElement(SXN_STATE);
    iRootElem.appendChild(nestedForEachState);
    QString nestedForEachStateID=ioStatId;
    QString initialTarget;

    QString timeOutStatename;




    if (rc && ! oElem.isNull())
    {

        QDomElement oTimeOutElem;
        QString TimeOutTarget = QString("%1_1").arg(sStateId);

        pCmd->setSCXMLStateId(sStateId);

        rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOutInMiliSec,TimeOutTarget);
        timeOutStatename=sStateId;
        if (rc)
        {
            initialTarget=sStateId;
            sStateId = TimeOutTarget;
            nestedForEachState.appendChild(oTimeOutElem);
        }
        else
            initialTarget=sStateId;

        nestedForEachState.appendChild(oElem);
        QStringList VarList;

        //QStringList ParamList = getValidParametersToInvoke(VarList);
        QStringList dumpListParamForinvoke = getValidParametersToInvoke(dumpListParams);
        QStringList allParamsForInvoke = QStringList()<<dumpListParamForinvoke;//<<ParamList;

        setStateAttr(oElem,sStateId);
        pCmd->setLoopSCXMLId(sStateId);
        setStateIDName(sStateId,pCmd);

        QMap<QString,QDomElement> StatChildXPathMap;
        getScxmlElementPathMap(oElem,oElem.nodeName(),StatChildXPathMap);

        QDomElement invokeElem = StatChildXPathMap.value(getStateInvokeXPath());

        QString invokeID = QString(sStateId).replace("s","i");
        createInvokeStateNodes(iDomDoc,oElem,invokeElem,allParamsForInvoke,invokeID,"ParameterID","Info",instanceName,false,pCmd->getAction(),pCmd->getComplement(),ioStatId,hiddenWaitTimeTxEvent);


        oElem.removeChild(invokeElem);

        QDomElement dumyStateElem = iDomDoc.createElement(SXN_STATE);
        dumyStateElem.setAttribute(SXA_ID,QString("%1_dummyState").arg(sStateId));


        //create transition on condition does meet
        QDomElement ElemOnTransitTrue = StatChildXPathMap.value(getStateTransistionXPath());
        GTASXTransition objTransitTrue;
        objTransitTrue.Condition = ExprTrue;
        objTransitTrue.Target = iTrueStateId;
        objTransitTrue.status = "";



        //create transition on condition doesnot meet
        QDomElement ElemOnTransitFalse = ElemOnTransitTrue.cloneNode().toElement();
        GTASXTransition objTransitFalse;
        objTransitFalse.Condition = ExprFalse;
        objTransitFalse.Target = iFalseStateId;
        objTransitFalse.status = "";



        //Log Node
        GTAScxmlDataModel objTF;
        objTF.ID = "TRUE_VAR";
        objTF.Expr = "'true'";
        _DataModelList.insert(objTF.ID,objTF);
        objTF.ID = "FALSE_VAR";
        objTF.Expr = "'false'";
        _DataModelList.insert(objTF.ID,objTF);


        GTAScxmlLogMessage objTrueLog;
        objTrueLog.LOD = GTAScxmlLogMessage::Main;
        objTrueLog.Identifier = instanceName;
        objTrueLog.ExpectedValue = QString("'+%1+'").arg("ExpectedValue");
        objTrueLog.CurrentValue = QString("TRUE_VAR");
        objTrueLog.Status = GTAScxmlLogMessage::Success;
        objTrueLog.Result = GTAScxmlLogMessage::OK;

        QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
        ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
        ElemOnTrueLog.setAttribute(SXA_EXPR,objTrueLog.getLogMessage());

        QDomElement trueExprAssignElem = iDomDoc.createElement(SXN_ASSIGN);
        trueExprAssignElem.setAttribute(SXA_LOCATION,"ExpectedValue");
        trueExprAssignElem.setAttribute(SXA_EXPR,ExprTrue);
        ElemOnTransitTrue.appendChild(trueExprAssignElem);

        ElemOnTransitTrue.appendChild(ElemOnTrueLog);

        objTrueLog.Status = GTAScxmlLogMessage::Success;
        objTrueLog.Result = GTAScxmlLogMessage::OK;
        objTrueLog.CurrentValue = QString("FALSE_VAR");
        QDomElement ElemOnFalseLog = ElemOnTrueLog.cloneNode().toElement();
        ElemOnFalseLog.setAttribute(SXA_EXPR,objTrueLog.getLogMessage());

        QDomElement falseExprAssignElem = iDomDoc.createElement(SXN_ASSIGN);
        falseExprAssignElem.setAttribute(SXA_LOCATION,"ExpectedValue");
        falseExprAssignElem.setAttribute(SXA_EXPR,ExprFalse);
        ElemOnTransitFalse.appendChild(falseExprAssignElem);

        ElemOnTransitFalse.appendChild(ElemOnFalseLog);


        QString eventName = QString("%1_%2").arg(sStateId,"DelayCheckCond");
        bool delayEventCheck = true;
        //        if(ParamList.isEmpty())
        //        {
        //            ElemOnTransitTrue.setAttribute(SXA_EVENT,eventName);
        //            ElemOnTransitFalse.setAttribute(SXA_EVENT,eventName);
        //        }
        //        else
        //        {
        if(!isPirConfigEnabled)
        {
            if(!hiddenWaitTimeTxEvent.isEmpty())
            {
                ElemOnTransitTrue.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                ElemOnTransitFalse.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
            }
            else
            {
                ElemOnTransitTrue.setAttribute(SXA_EVENT,"done.invoke._all");
                ElemOnTransitFalse.setAttribute(SXA_EVENT,"done.invoke._all");
            }
        }
        else
        {
            ElemOnTransitTrue.setAttribute(SXA_EVENT,eventName);
            ElemOnTransitFalse.setAttribute(SXA_EVENT,eventName);
        }
        //}



        //Action on Fail
        GTAScxmlLogMessage objLog;
        objLog.Identifier    = instanceName;
        objLog.ExpectedValue = "True";
        objLog.CurrentValue = "'TimeOut'";
        objLog.Status = GTAScxmlLogMessage::TimeOut;
        objLog.Result = GTAScxmlLogMessage::KO;
        QString logMsgTimeOut = objLog.getLogMessage();
        logMsgTimeOut = logMsgTimeOut.remove("+");
        logMsgTimeOut = logMsgTimeOut.remove("'");
        logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);
        QString timeouteventFalg = "";

        if((isPirConfigEnabled) )//&& (!ParamList.isEmpty()))
        {
            createActionOnFail(iDomDoc,dumyStateElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,delayEventCheck,eventName,true,true,timeouteventFalg);

            QDomElement txTimeoutElem = iDomDoc.createElement(SXN_TRANSITION);
            txTimeoutElem.setAttribute(SXA_STATUS,"fail");
            if(actionOnFail.contains("stop"))
                txTimeoutElem.setAttribute(SXA_TARGET,"main_final");
            else
                txTimeoutElem.setAttribute(SXA_TARGET,iStatOfSibling);
            txTimeoutElem.setAttribute(SXA_EVENT,eventName);
            QString elapsedTime = QString("%1_timeout_elapsed").arg(timeOutStatename);


            QString timeoutval = timeOutInMiliSec.replace("ms","");
            QString cond = QString("%1 >= %2").arg(elapsedTime,timeoutval);

            txTimeoutElem.setAttribute(SXA_CONDITION,cond);




            QDomElement logElem = iDomDoc.createElement(SXN_LOG);
            logElem.setAttribute(SXA_EXPR,logMsgTimeOut);
            logElem.setAttribute(SXA_LABEL,"'info'");

            txTimeoutElem.appendChild(logElem);


            dumyStateElem.appendChild(txTimeoutElem);

            QDomNodeList onEntryList = dumyStateElem.elementsByTagName(SXN_ONENTRY);
            if(!onEntryList.isEmpty())
            {
                QDomElement onEntryElem = onEntryList.at(0).toElement();

                QDomElement dummyAssign1 = iDomDoc.createElement(SXN_ASSIGN);
                QDomElement dummyAssign2 = iDomDoc.createElement(SXN_ASSIGN);
                dummyAssign1.setAttribute(SXA_EXPR,"Date.now()");
                dummyAssign1.setAttribute(SXA_LOCATION,QString("%1_timeout_NewTime").arg(timeOutStatename));

                dummyAssign2.setAttribute(SXA_EXPR,QString("%1_timeout_NewTime - %1_timeout_StartTime").arg(timeOutStatename));
                dummyAssign2.setAttribute(SXA_LOCATION,elapsedTime);

                onEntryElem.appendChild(dummyAssign1);
                onEntryElem.appendChild(dummyAssign2);
            }


            //createActionOnFail(iDomDoc,dumyStateElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,false,eventName,true,false,timeouteventFalg);
        }
        else
        {
            createActionOnFail(iDomDoc,oElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,delayEventCheck,eventName,true,true,timeouteventFalg);

            createActionOnFail(iDomDoc,oElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,false,eventName,true,false,timeouteventFalg);
        }


        if(!timeouteventFalg.isEmpty())
        {
            QString cond = objTransitTrue.Condition;
            objTransitTrue.Condition =  QString("((%1) && (%2 != '%3'))").arg(cond,timeouteventFalg,"TimeOut");
            cond = objTransitFalse.Condition;
            objTransitFalse.Condition = QString("((%1) && (%2 != '%3'))").arg(cond,timeouteventFalg,"TimeOut");
        }

        setTransitionAttr(ElemOnTransitTrue,objTransitTrue);
        setTransitionAttr(ElemOnTransitFalse,objTransitFalse);

        if((isPirConfigEnabled) )//&& (!ParamList.isEmpty()))
        {
            dumyStateElem.appendChild(ElemOnTransitTrue);
            dumyStateElem.appendChild(ElemOnTransitFalse);
            oElem.appendChild(dumyStateElem);
        }
        else
            oElem.insertAfter(ElemOnTransitFalse,ElemOnTransitTrue);




        //Logging all Parameters
        QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
        oElem.appendChild(ElemOnExit);
        VarList.append(dumpListParams);
        insertVariableOnExit(ElemOnExit,VarList,sStateId,instanceName,iDomDoc);


        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
        if(pParent != NULL)
        {
            createLoopTimeOutTranstions(oElem,iDomDoc,sStateId,pParent);

        }
        createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pCmd,iFalseStateId);

    }
    ioStatId = sStateId;
    ioElem = nestedForEachState;
    nestedForEachState.setAttribute(SXA_ID,nestedForEachStateID);
    QDomElement nestedForEachTxnState = iDomDoc.createElement(SXN_TRANSITION);
    nestedForEachTxnState.setAttribute(SXA_TARGET,iStatOfSibling);

    QDomElement finalElem = iDomDoc.createElement(SXN_FINAL);
    finalElem.setAttribute(SXA_ID,QString("%1_Final").arg(nestedForEachStateID));
    nestedForEachState.appendChild(finalElem);

    nestedForEachState.appendChild(nestedForEachTxnState);
    nestedForEachState.setAttribute(SXA_INITIALSTATE,initialTarget);

    return rc;
}

/**
  * This function create the scxml state for a while action
  * pCmd: While command
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iStateId: State id of the command;
  * iTrueStateId: The next ID if the condition is true
  * iFalseStateId: The next ID if the condition is false
  * iStatOfSibling: The ID of Sibling command node below while
  * return true if scxml of the command is created, otherwise false
  */

bool GTAExportScxml::createActionWhileState(GTAActionWhile *pCmd, QDomDocument &iDomDoc,
                                               QDomElement &iRootElem, QString &ioStatId,
                                               const QString &iTrueStateId, const QString iFalseStateId,
                                               const QString & iStatOfSibling, QDomElement& ioElem)
{
    bool rc = false;
    if(pCmd == NULL)
        return rc;

    rc = true;
    QString ConfirmParam;
    QString ConfirmParamVal;
    QString param = pCmd->getParameter();
    QString paramVal = pCmd->getValue();
    updateWithGenToolParam(param);
    updateWithGenToolParam(paramVal);
    QString condition =pCmd->getCondition();
    QString sActionName = pCmd->getAction();
    QString sComplement = pCmd->getComplement();
    QString instanceName = pCmd->getInstanceName();
    QString timeOutInMiliSec = getTimeOutInMiliSecond((GTAActionWhile*)pCmd);
    QString actionOnFail = pCmd->getActionOnFail();
    QStringList dumpListParams = pCmd->getDumpList();
    QString hiddenWaitTimeTxEvent;
    // QString conftimeInMiliSec = getConfTimeInMiliSecond((GTAActionWhile*)pCmd);
    QString sStateId= ioStatId + "_1";

    bool isPirConfigEnabled = TestConfig::getInstance()->getPIRInvokeForSCXML();
    QString ExprTrue, ExprFalse;
    _hasChannelInControl = false;
    if(param.contains(GTA_CIC_IDENTIFIER) || paramVal.contains(GTA_CIC_IDENTIFIER))
        _hasChannelInControl = true;

    QString sPrecisionVal, sPrecisonUnit;
    pCmd->getPrecision(sPrecisionVal, sPrecisonUnit);

    double dPrecisionVal = sPrecisionVal.toDouble();

    if(sPrecisonUnit == ACT_PRECISION_UNIT_PERCENT)
    {
        dPrecisionVal = dPrecisionVal*0.01;
    }



    QString sFileName = sActionName + "_"+ sComplement + ".scxml";

    QDomElement oElem;
    rc = getCloneRootElement(sFileName,iDomDoc,oElem);

    QDomElement nestedWhileState = iDomDoc.createElement(SXN_STATE);
    iRootElem.appendChild(nestedWhileState);
    QString nesteWhileStateID=ioStatId;
    QString initialTarget;

    QString timeOutStatename;




    if (rc && ! oElem.isNull())
    {

        QDomElement oTimeOutElem;
        QString TimeOutTarget = QString("%1_1").arg(sStateId);

        pCmd->setSCXMLStateId(sStateId);

        rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOutInMiliSec,TimeOutTarget);
        timeOutStatename=sStateId;
        if (rc)
        {
            initialTarget=sStateId;
            sStateId = TimeOutTarget;
            nestedWhileState.appendChild(oTimeOutElem);

            //            if(isPirConfigEnabled)
            //            {
            //                QDomNodeList onEntryList = oTimeOutElem.elementsByTagName(SXN_ONENTRY);
            //                if(!onEntryList.isEmpty())
            //                {
            //                    QDomElement dummyAssign3 = iDomDoc.createElement(SXN_ASSIGN);
            //                    dummyAssign3.setAttribute(SXA_LOCATION,QString("%1_timeout_StartTime").arg(sStateId));
            //                    onEntryList.at(0).toElement().appendChild(dummyAssign3);
            //                }

            //            }

        }
        else
            initialTarget=sStateId;
        QDomElement oConfTimeStateElem;
        QString confTimeTarget = QString("%1_1").arg(sStateId);
        /* rc = getConfigurationTimeNode(iDomDoc,oConfTimeStateElem,sStateId,conftimeInMiliSec,confTimeTarget);
           if (rc)
           {
               sStateId = confTimeTarget;
               iRootElem.appendChild(oConfTimeStateElem);
           }*/

        nestedWhileState.appendChild(oElem);
        QStringList VarList;
        //VarList<<param<<paramVal;
        getChannelInControlCondition(param,paramVal,condition,ExprTrue,ExprFalse,false,dPrecisionVal,sPrecisonUnit,VarList);

        bool isOnlyFS = pCmd->getIsFsOnly();
        bool isOnlyValue = pCmd->getIsValueOnly();
        QString fs = pCmd->getFunctionalStatus();

        QString StatusExprTrue, StatusExprFalse;
        QString fsCond;
        if(fs.trimmed().isEmpty())
        {
            fs = "NO";
        }
        if(fs == ACT_FSSSM_NOT_LOST)
        {
            fs = ACT_FSSSM_LOST;
            if(condition == ARITH_NOTEQ)
                fsCond = ARITH_EQ;
            else if(condition == ARITH_EQ)
                fsCond = ARITH_NOTEQ;
            else
                fsCond = ARITH_EQ;
        }
        else
        {
            if((condition != ARITH_NOTEQ)|| (condition == ARITH_EQ))
                fsCond = ARITH_EQ;
        }
        if(fsCond.trimmed().isEmpty())
        {
            fsCond = condition;
        }
        getChannelInControlCondition(param,QString("'%1'").arg(fs),fsCond,StatusExprTrue,StatusExprFalse,false,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);
        if(_ParamList.contains(param))
        {
            if(isOnlyFS)
            {
                ExprTrue = StatusExprTrue;
                ExprFalse = StatusExprFalse;
            }
            else if(!(isOnlyFS || isOnlyValue))
            {
                if(condition == ARITH_NOTEQ)
                {
                    ExprTrue = QString("((%1) || (%2))").arg(ExprTrue,StatusExprTrue);
                    ExprFalse  = QString("((%1) && (%2))").arg(ExprFalse ,StatusExprFalse);
                }
                else
                {
                    ExprTrue = QString("((%1) && (%2))").arg(ExprTrue,StatusExprTrue);
                    ExprFalse  = QString("((%1) || (%2))").arg(ExprFalse ,StatusExprFalse);
                }
            }
        }


        QStringList ParamList = getValidParametersToInvoke(VarList);
        QStringList dumpListParamForinvoke = getValidParametersToInvoke(dumpListParams);
        QStringList allParamsForInvoke = QStringList()<<ParamList<<dumpListParamForinvoke;

        setStateAttr(oElem,sStateId);
        pCmd->setLoopSCXMLId(sStateId);
        setStateIDName(sStateId,pCmd);

        QMap<QString,QDomElement> StatChildXPathMap;
        getScxmlElementPathMap(oElem,oElem.nodeName(),StatChildXPathMap);

        QDomElement invokeElem = StatChildXPathMap.value(getStateInvokeXPath());

        QString invokeID = QString(sStateId).replace("s","i");
        //createInvokeNodes(oElem,invokeElem,allParamsForInvoke,invokeID,"ParameterID","Info",instanceName);
        createInvokeStateNodes(iDomDoc,oElem,invokeElem,allParamsForInvoke,invokeID,"ParameterID","Info",instanceName,false,pCmd->getAction(),pCmd->getComplement(),ioStatId,hiddenWaitTimeTxEvent);


        oElem.removeChild(invokeElem);

        //get fail and pass expressions


        //if(ParamList.contains(param))
        //{
        //    param.append(".Value");
        //}
        //if(ParamList.contains(paramVal))
        //{
        //    paramVal.append(".Value");
        //}
        //getExpresion(param,paramVal,condition,ExprTrue,ExprFalse,false,dPrecisionVal,sPrecisonUnit);



        QDomElement dumyStateElem = iDomDoc.createElement(SXN_STATE);
        dumyStateElem.setAttribute(SXA_ID,QString("%1_dummyState").arg(sStateId)); 


        //create transition on condition does meet
        QDomElement ElemOnTransitTrue = StatChildXPathMap.value(getStateTransistionXPath());
        GTASXTransition objTransitTrue;
        objTransitTrue.Condition = ExprTrue;
        objTransitTrue.Target = iTrueStateId;
        objTransitTrue.status = "";



        //create transition on condition doesnot meet
        QDomElement ElemOnTransitFalse = ElemOnTransitTrue.cloneNode().toElement();
        GTASXTransition objTransitFalse;
        objTransitFalse.Condition = ExprFalse;
        objTransitFalse.Target = iFalseStateId;
        objTransitFalse.status = "";



        //Log Node

        GTAScxmlDataModel objTF;
        objTF.ID = "TRUE_VAR";
        objTF.Expr = "'true'";
        _DataModelList.insert(objTF.ID,objTF);
        objTF.ID = "FALSE_VAR";
        objTF.Expr = "'false'";
        _DataModelList.insert(objTF.ID,objTF);


        GTAScxmlLogMessage objTrueLog;
        objTrueLog.LOD = GTAScxmlLogMessage::Main;
        objTrueLog.Identifier = instanceName;
        objTrueLog.ExpectedValue = QString("'+%1+'").arg("ExpectedValue");//paramVal;//QString("'+%1+'").arg(param);
        objTrueLog.CurrentValue = QString("TRUE_VAR");//QString("'%1'").arg("true");//param;//QString("'+%1+'").arg(paramVal);
        objTrueLog.Status = GTAScxmlLogMessage::Success;
        objTrueLog.Result = GTAScxmlLogMessage::OK;

        QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
        ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
        ElemOnTrueLog.setAttribute(SXA_EXPR,objTrueLog.getLogMessage());

        QDomElement trueExprAssignElem = iDomDoc.createElement(SXN_ASSIGN);
        trueExprAssignElem.setAttribute(SXA_LOCATION,"ExpectedValue");
        trueExprAssignElem.setAttribute(SXA_EXPR,ExprTrue);
        ElemOnTransitTrue.appendChild(trueExprAssignElem);

        ElemOnTransitTrue.appendChild(ElemOnTrueLog);

        objTrueLog.Status = GTAScxmlLogMessage::Success;
        objTrueLog.Result = GTAScxmlLogMessage::OK;
        objTrueLog.CurrentValue = QString("FALSE_VAR");//QString("'%1'").arg("false");
        QDomElement ElemOnFalseLog = ElemOnTrueLog.cloneNode().toElement();
        ElemOnFalseLog.setAttribute(SXA_EXPR,objTrueLog.getLogMessage());

        QDomElement falseExprAssignElem = iDomDoc.createElement(SXN_ASSIGN);
        falseExprAssignElem.setAttribute(SXA_LOCATION,"ExpectedValue");
        falseExprAssignElem.setAttribute(SXA_EXPR,ExprFalse);
        ElemOnTransitFalse.appendChild(falseExprAssignElem);

        ElemOnTransitFalse.appendChild(ElemOnFalseLog);


        QString eventName = QString("%1_%2").arg(sStateId,"DelayCheckCond");
        bool delayEventCheck = true;
        if(ParamList.isEmpty())
        {
            ElemOnTransitTrue.setAttribute(SXA_EVENT,eventName);
            ElemOnTransitFalse.setAttribute(SXA_EVENT,eventName);
        }
        else
        {
            if(!isPirConfigEnabled)
            {
                if(!hiddenWaitTimeTxEvent.isEmpty())
                {
                    ElemOnTransitTrue.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                    ElemOnTransitFalse.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                }
                else
                {
                    ElemOnTransitTrue.setAttribute(SXA_EVENT,"done.invoke._all");
                    ElemOnTransitFalse.setAttribute(SXA_EVENT,"done.invoke._all");
                }
            }
            else
            {
                ElemOnTransitTrue.setAttribute(SXA_EVENT,eventName);
                ElemOnTransitFalse.setAttribute(SXA_EVENT,eventName);
            }
        }



        //Action on Fail
        GTAScxmlLogMessage objLog;
        objLog.Identifier    = instanceName;
        objLog.ExpectedValue = "True";
        objLog.CurrentValue = "'TimeOut'";
        objLog.Status = GTAScxmlLogMessage::TimeOut;
        objLog.Result = GTAScxmlLogMessage::KO;
        QString logMsgTimeOut = objLog.getLogMessage();
        logMsgTimeOut = logMsgTimeOut.remove("+");
        logMsgTimeOut = logMsgTimeOut.remove("'");
        logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);
        QString timeouteventFalg = "";

        if((isPirConfigEnabled) && (!ParamList.isEmpty()))
        {
            createActionOnFail(iDomDoc,dumyStateElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,delayEventCheck,eventName,true,true,timeouteventFalg);

            QDomElement txTimeoutElem = iDomDoc.createElement(SXN_TRANSITION);
            txTimeoutElem.setAttribute(SXA_STATUS,"fail");
            if(actionOnFail.contains("stop"))
                txTimeoutElem.setAttribute(SXA_TARGET,"main_final");
            else
                txTimeoutElem.setAttribute(SXA_TARGET,iStatOfSibling);
            txTimeoutElem.setAttribute(SXA_EVENT,eventName);
            QString elapsedTime = QString("%1_timeout_elapsed").arg(timeOutStatename);


            QString timeoutval = timeOutInMiliSec.replace("ms","");
            QString cond = QString("%1 >= %2").arg(elapsedTime,timeoutval);

            txTimeoutElem.setAttribute(SXA_CONDITION,cond);




            QDomElement logElem = iDomDoc.createElement(SXN_LOG);
            logElem.setAttribute(SXA_EXPR,logMsgTimeOut);
            logElem.setAttribute(SXA_LABEL,"'info'");

            txTimeoutElem.appendChild(logElem);


            dumyStateElem.appendChild(txTimeoutElem);

            QDomNodeList onEntryList = dumyStateElem.elementsByTagName(SXN_ONENTRY);
            if(!onEntryList.isEmpty())
            {
                QDomElement onEntryElem = onEntryList.at(0).toElement();

                QDomElement dummyAssign1 = iDomDoc.createElement(SXN_ASSIGN);
                QDomElement dummyAssign2 = iDomDoc.createElement(SXN_ASSIGN);
                dummyAssign1.setAttribute(SXA_EXPR,"Date.now()");
                dummyAssign1.setAttribute(SXA_LOCATION,QString("%1_timeout_NewTime").arg(timeOutStatename));

                dummyAssign2.setAttribute(SXA_EXPR,QString("%1_timeout_NewTime - %1_timeout_StartTime").arg(timeOutStatename));
                dummyAssign2.setAttribute(SXA_LOCATION,elapsedTime);

                onEntryElem.appendChild(dummyAssign1);
                onEntryElem.appendChild(dummyAssign2);
            }


            //createActionOnFail(iDomDoc,dumyStateElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,false,eventName,true,false,timeouteventFalg);
        }
        else
        {
            createActionOnFail(iDomDoc,oElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,delayEventCheck,eventName,true,true,timeouteventFalg);

            createActionOnFail(iDomDoc,oElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,false,eventName,true,false,timeouteventFalg);
        }


        if(!timeouteventFalg.isEmpty())
        {
            QString cond = objTransitTrue.Condition;
            objTransitTrue.Condition =  QString("((%1) && (%2 != '%3'))").arg(cond,timeouteventFalg,"TimeOut");
            cond = objTransitFalse.Condition;
            objTransitFalse.Condition = QString("((%1) && (%2 != '%3'))").arg(cond,timeouteventFalg,"TimeOut");
        }

        setTransitionAttr(ElemOnTransitTrue,objTransitTrue);
        setTransitionAttr(ElemOnTransitFalse,objTransitFalse);

        if((isPirConfigEnabled) && (!ParamList.isEmpty()))
        {
            dumyStateElem.appendChild(ElemOnTransitTrue);
            dumyStateElem.appendChild(ElemOnTransitFalse);
            oElem.appendChild(dumyStateElem);
        }
        else
            oElem.insertAfter(ElemOnTransitFalse,ElemOnTransitTrue);




        //Logging all Parameters
        QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
        oElem.appendChild(ElemOnExit);
        VarList.append(dumpListParams);
        insertVariableOnExit(ElemOnExit,VarList,sStateId,instanceName,iDomDoc);


        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
        if(pParent != NULL)
        {
            createLoopTimeOutTranstions(oElem,iDomDoc,sStateId,pParent);

        }
        createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pCmd,iFalseStateId);

    }
    ioStatId = sStateId;
    ioElem = nestedWhileState;
    nestedWhileState.setAttribute(SXA_ID,nesteWhileStateID);
    QDomElement nestedWhileTxnState = iDomDoc.createElement(SXN_TRANSITION);
    nestedWhileTxnState.setAttribute(SXA_TARGET,iStatOfSibling);

    QDomElement finalElem = iDomDoc.createElement(SXN_FINAL);
    finalElem.setAttribute(SXA_ID,QString("%1_Final").arg(nesteWhileStateID));
    nestedWhileState.appendChild(finalElem);

    nestedWhileState.appendChild(nestedWhileTxnState);
    nestedWhileState.setAttribute(SXA_INITIALSTATE,initialTarget);

    return rc;
}

/**
* This function create the scxml state for a do while action
* pCmd: do While command
* iDomDoc: state xml is create in the context of this document
* iRootElem: Element where the created state is inserted
* iStateId: State id of the command;
* iTrueStateId: The next ID if the condition is true
* iFalseStateId: The next ID if the condition is false
* iStatOfSibling: The ID of Sibling command node below while
* return true if scxml of the command is created, otherwise false
*/


bool GTAExportScxml::createActionDoWhileState(GTAActionDoWhile *pCmd, QDomDocument &iDomDoc,
                                                 QDomElement &iRootElem, QString &ioStatId,
                                                 const QString &iTrueStateId, const QString iFalseStateId,
                                                 const QString & iStatOfSibling, QDomElement& ioElem)
{
    bool rc = false;
    if(pCmd == NULL)
        return rc;

    rc = true;
    QString param = pCmd->getParameter();
    QString paramVal = pCmd->getValue();
    updateWithGenToolParam(param);
    updateWithGenToolParam(paramVal);
    QString condition =pCmd->getCondition();
    QString sActionName = pCmd->getAction();
    QString sComplement = pCmd->getComplement();
    QString instanceName = pCmd->getInstanceName();
    QString timeOutInMiliSec = getTimeOutInMiliSecond((GTAActionDoWhile*)pCmd);
    QString actionOnFail = pCmd->getActionOnFail();
    QStringList dumpListParams = pCmd->getDumpList();
    // QString conftimeInMiliSec = getConfTimeInMiliSecond((GTAActionWhile*)pCmd);
    QString sStateId= ioStatId + "_1";
    QString hiddenWaitTimeTxEvent;
    QString ExprTrue, ExprFalse;
    _hasChannelInControl = false;
    if(param.contains(GTA_CIC_IDENTIFIER) || paramVal.contains(GTA_CIC_IDENTIFIER))
        _hasChannelInControl = true;

    QString sPrecisionVal, sPrecisonUnit;
    pCmd->getPrecision(sPrecisionVal, sPrecisonUnit);

    double dPrecisionVal = sPrecisionVal.toDouble();

    if(sPrecisonUnit == ACT_PRECISION_UNIT_PERCENT)
    {
        dPrecisionVal = dPrecisionVal*0.01;

    }
    QDomElement oElem;

    
    QString sFileName = sActionName + "_"+ sComplement + ".scxml";
    rc = getCloneRootElement(sFileName,iDomDoc,oElem);
    QDomElement nestedWhileState = iDomDoc.createElement(SXN_STATE);
    iRootElem.appendChild(nestedWhileState);
    QString nesteWhileStateID=ioStatId;
    QString initialTarget;
    QString timeOutStatename;

    if (rc && ! oElem.isNull())
    {
        bool isPirConfigEnabled = TestConfig::getInstance()->getPIRInvokeForSCXML();
        QDomElement oTimeOutElem;
        QString TimeOutTarget = QString("%1_1").arg(sStateId);
        //rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOutInMiliSec,TimeOutTarget);
        pCmd->setSCXMLStateId(sStateId);
        rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOutInMiliSec,iTrueStateId);
        timeOutStatename=sStateId;
        if (rc)
        {
            initialTarget=sStateId;
            sStateId = TimeOutTarget;
            nestedWhileState.appendChild(oTimeOutElem);
        }
        else
            initialTarget=sStateId;

        QDomElement oConfTimeStateElem;
        QString confTimeTarget = QString("%1_1").arg(sStateId);
        /* rc = getConfigurationTimeNode(iDomDoc,oConfTimeStateElem,sStateId,conftimeInMiliSec,confTimeTarget);
        if (rc)
        {
        sStateId = confTimeTarget;
        iRootElem.appendChild(oConfTimeStateElem);
        }*/

        nestedWhileState.appendChild(oElem);
        QStringList VarList;
        //VarList<<param<<paramVal;
        getChannelInControlCondition(param,paramVal,condition,ExprTrue,ExprFalse,false,dPrecisionVal,sPrecisonUnit,VarList);


        bool isOnlyFS = pCmd->getIsFsOnly();
        bool isOnlyValue = pCmd->getIsValueOnly();
        QString fs = pCmd->getFunctionalStatus();

        QString StatusExprTrue, StatusExprFalse;
        QString fsCond;
        if(fs.trimmed().isEmpty())
        {
            fs = "NO";
        }
        if(fs == ACT_FSSSM_NOT_LOST)
        {
            fs = ACT_FSSSM_LOST;
            if(condition == ARITH_NOTEQ)
                fsCond = ARITH_EQ;
            else if(condition == ARITH_EQ)
                fsCond = ARITH_NOTEQ;
            else
                fsCond = ARITH_EQ;
        }
        else
        {
            if((condition != ARITH_NOTEQ)|| (condition == ARITH_EQ))
                fsCond = ARITH_EQ;
        }
        if(fsCond.trimmed().isEmpty())
        {
            fsCond = condition;
        }
        getChannelInControlCondition(param,QString("'%1'").arg(fs),fsCond,StatusExprTrue,StatusExprFalse,false,0,ACT_PRECISION_UNIT_VALUE,QStringList(),true);
        if(_ParamList.contains(param))
        {
            if(isOnlyFS)
            {
                ExprTrue = StatusExprTrue;
                ExprFalse = StatusExprFalse;
            }
            else if(!(isOnlyFS || isOnlyValue))
            {
                if(condition == ARITH_NOTEQ)
                {
                    ExprTrue = QString("((%1) || (%2))").arg(ExprTrue,StatusExprTrue);
                    ExprFalse  = QString("((%1) && (%2))").arg(ExprFalse ,StatusExprFalse);
                }
                else
                {
                    ExprTrue = QString("((%1) && (%2))").arg(ExprTrue,StatusExprTrue);
                    ExprFalse  = QString("((%1) || (%2))").arg(ExprFalse ,StatusExprFalse);
                }
            }
        }



        QStringList ParamList = getValidParametersToInvoke(VarList);
        QStringList dumpListParamForinvoke = getValidParametersToInvoke(dumpListParams);        
        QStringList allParamsForInvoke = QStringList()<<ParamList<<dumpListParamForinvoke;

        setStateAttr(oElem,sStateId);
        pCmd->setLoopSCXMLId(sStateId);
        setStateIDName(sStateId,pCmd);

        QMap<QString,QDomElement> StatChildXPathMap;
        getScxmlElementPathMap(oElem,oElem.nodeName(),StatChildXPathMap);

        QDomElement invokeElem = StatChildXPathMap.value(getStateInvokeXPath());

        QString invokeID = QString(sStateId).replace("s","i");
        //createInvokeNodes(iDomDoc,oElem,invokeElem,allParamsForInvoke,invokeID,"ParameterID","Info",instanceName,false,pCmd->getAction(),pCmd->getComplement());

        createInvokeStateNodes(iDomDoc,oElem,invokeElem,allParamsForInvoke,invokeID,"ParameterID","Info",instanceName,false,pCmd->getAction(),pCmd->getComplement(),ioStatId,hiddenWaitTimeTxEvent);


        oElem.removeChild(invokeElem);

        //get fail and pass expressions
        
        //if(ParamList.contains(param))
        //{
        //    param.append(".Value");
        //}
        //if(ParamList.contains(paramVal))
        //{
        //    paramVal.append(".Value");
        //}
        //getExpresion(param,paramVal,condition,ExprTrue,ExprFalse,false,dPrecisionVal,sPrecisonUnit);


        QDomElement dumyStateElem = iDomDoc.createElement(SXN_STATE);
        dumyStateElem.setAttribute(SXA_ID,QString("%1_dummyState").arg(sStateId));


        //create transition on condition does meet
        QDomElement ElemOnTransitTrue = StatChildXPathMap.value(getStateTransistionXPath());
        GTASXTransition objTransitTrue;
        objTransitTrue.Condition = ExprTrue;
        objTransitTrue.Target = iTrueStateId;
        objTransitTrue.status = "";

        

        //create transition on condition doesnot meet
        QDomElement ElemOnTransitFalse = ElemOnTransitTrue.cloneNode().toElement();
        GTASXTransition objTransitFalse;
        objTransitFalse.Condition = ExprFalse;
        objTransitFalse.Target = iFalseStateId;
        objTransitFalse.status = "";


        //Log Node

        GTAScxmlDataModel objTF;
        objTF.ID = "TRUE_VAR";
        objTF.Expr = "'true'";
        _DataModelList.insert(objTF.ID,objTF);
        objTF.ID = "FALSE_VAR";
        objTF.Expr = "'false'";
        _DataModelList.insert(objTF.ID,objTF);


        GTAScxmlLogMessage objTrueLog;
        objTrueLog.LOD = GTAScxmlLogMessage::Main;
        objTrueLog.Identifier = instanceName;
        objTrueLog.ExpectedValue = QString("'+%1+'").arg("ExpectedValue");//paramVal;//QString("'+%1+'").arg(param);
        objTrueLog.CurrentValue = QString("TRUE_VAR");//QString("'true'");//param;//QString("'+%1+'").arg(paramVal);
        objTrueLog.Status = GTAScxmlLogMessage::Success;
        objTrueLog.Result = GTAScxmlLogMessage::OK;

        QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
        ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
        ElemOnTrueLog.setAttribute(SXA_EXPR,objTrueLog.getLogMessage());

        QDomElement trueExprAssignElem = iDomDoc.createElement(SXN_ASSIGN);
        trueExprAssignElem.setAttribute(SXA_LOCATION,"ExpectedValue");
        trueExprAssignElem.setAttribute(SXA_EXPR,ExprTrue);
        ElemOnTransitTrue.appendChild(trueExprAssignElem);

        ElemOnTransitTrue.appendChild(ElemOnTrueLog);

        objTrueLog.Status = GTAScxmlLogMessage::Success;
        objTrueLog.CurrentValue = QString("FALSE_VAR");//QString("'false'");
        QDomElement ElemOnFalseLog = ElemOnTrueLog.cloneNode().toElement();

        ElemOnFalseLog.setAttribute(SXA_EXPR,objTrueLog.getLogMessage());

        QDomElement falseExprAssignElem = iDomDoc.createElement(SXN_ASSIGN);
        falseExprAssignElem.setAttribute(SXA_LOCATION,"ExpectedValue");
        falseExprAssignElem.setAttribute(SXA_EXPR,ExprFalse);
        ElemOnTransitFalse.appendChild(falseExprAssignElem);

        ElemOnTransitFalse.appendChild(ElemOnFalseLog);


        QString eventName = QString("%1_%2").arg(sStateId,"DelayCheckCond");
        bool delayEventCheck = true;
        if(ParamList.isEmpty())
        {
            ElemOnTransitTrue.setAttribute(SXA_EVENT,eventName);
            ElemOnTransitFalse.setAttribute(SXA_EVENT,eventName);
            //delayEventCheck = true;
        }
        else
        {
            if(isPirConfigEnabled)
            {
                ElemOnTransitTrue.setAttribute(SXA_EVENT,eventName);
                ElemOnTransitFalse.setAttribute(SXA_EVENT,eventName);
            }
            else
            {
                if(hiddenWaitTimeTxEvent.isEmpty())
                {
                    ElemOnTransitTrue.setAttribute(SXA_EVENT,"done.invoke._all");
                    ElemOnTransitFalse.setAttribute(SXA_EVENT,"done.invoke._all");
                }
                else
                {
                    ElemOnTransitTrue.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                    ElemOnTransitFalse.setAttribute(SXA_EVENT,hiddenWaitTimeTxEvent);
                }

            }
        }

        //Action on Fail
        GTAScxmlLogMessage objLog;
        objLog.Identifier    = instanceName;
        objLog.ExpectedValue = "True";
        objLog.CurrentValue = "'TimeOut'";
        objLog.Status = GTAScxmlLogMessage::TimeOut;
        objLog.Result = GTAScxmlLogMessage::KO;
        QString logMsgTimeOut = objLog.getLogMessage();
        logMsgTimeOut = logMsgTimeOut.remove("+");
        logMsgTimeOut = logMsgTimeOut.remove("'");
        logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);

        QString timeouteventFalg = "";

        if((isPirConfigEnabled) && (!ParamList.isEmpty()))
        {
            createActionOnFail(iDomDoc,dumyStateElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,delayEventCheck,eventName,true,true,timeouteventFalg);

            QDomElement txTimeoutElem = iDomDoc.createElement(SXN_TRANSITION);
            txTimeoutElem.setAttribute(SXA_STATUS,"fail");
            if(actionOnFail.contains("stop"))
                txTimeoutElem.setAttribute(SXA_TARGET,"main_final");
            else
                txTimeoutElem.setAttribute(SXA_TARGET,iStatOfSibling);
            txTimeoutElem.setAttribute(SXA_EVENT,eventName);
            QString elapsedTime = QString("%1_timeout_elapsed").arg(timeOutStatename);


            QString timeoutval = timeOutInMiliSec.replace("ms","");
            QString cond = QString("%1 >= %2").arg(elapsedTime,timeoutval);

            txTimeoutElem.setAttribute(SXA_CONDITION,cond);




            QDomElement logElem = iDomDoc.createElement(SXN_LOG);
            logElem.setAttribute(SXA_EXPR,logMsgTimeOut);
            logElem.setAttribute(SXA_LABEL,"'info'");

            txTimeoutElem.appendChild(logElem);


            dumyStateElem.appendChild(txTimeoutElem);

            QDomNodeList onEntryList = dumyStateElem.elementsByTagName(SXN_ONENTRY);
            if(!onEntryList.isEmpty())
            {
                QDomElement onEntryElem = onEntryList.at(0).toElement();

                QDomElement dummyAssign1 = iDomDoc.createElement(SXN_ASSIGN);
                QDomElement dummyAssign2 = iDomDoc.createElement(SXN_ASSIGN);
                dummyAssign1.setAttribute(SXA_EXPR,"Date.now()");
                dummyAssign1.setAttribute(SXA_LOCATION,QString("%1_timeout_NewTime").arg(timeOutStatename));

                dummyAssign2.setAttribute(SXA_EXPR,QString("%1_timeout_NewTime - %1_timeout_StartTime").arg(timeOutStatename));
                dummyAssign2.setAttribute(SXA_LOCATION,elapsedTime);

                onEntryElem.appendChild(dummyAssign1);
                onEntryElem.appendChild(dummyAssign2);
            }
        }
        else
        {
            createActionOnFail(iDomDoc,oElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,delayEventCheck,eventName,true,true,timeouteventFalg);

            createActionOnFail(iDomDoc,oElem,timeOutStatename,iStatOfSibling,actionOnFail,timeOutInMiliSec,logMsgTimeOut,false,eventName,true,false,timeouteventFalg);
        }

        if(!timeouteventFalg.isEmpty())
        {
            QString cond = objTransitTrue.Condition;
            objTransitTrue.Condition =  QString("((%1) && (%2 != '%3'))").arg(cond,timeouteventFalg,"TimeOut");
            cond = objTransitFalse.Condition;
            objTransitFalse.Condition = QString("((%1) && (%2 != '%3'))").arg(cond,timeouteventFalg,"TimeOut");
        }
        setTransitionAttr(ElemOnTransitFalse,objTransitFalse);
        setTransitionAttr(ElemOnTransitTrue,objTransitTrue);


        if((isPirConfigEnabled) && (!ParamList.isEmpty()))
        {
            dumyStateElem.appendChild(ElemOnTransitTrue);
            dumyStateElem.appendChild(ElemOnTransitFalse);
            oElem.appendChild(dumyStateElem);
        }
        else
            oElem.insertAfter(ElemOnTransitFalse,ElemOnTransitTrue);




        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
        if(pParent != NULL)
        { 
            createLoopTimeOutTranstions(oElem,iDomDoc,sStateId,pParent);
        }
        createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pCmd,iFalseStateId);

        //Logging all Parameters
        QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
        oElem.appendChild(ElemOnExit);
        VarList.append(dumpListParams);
        insertVariableOnExit(ElemOnExit,VarList,sStateId,instanceName,iDomDoc);
    }
    ioStatId = sStateId;
    ioElem = nestedWhileState;
    nestedWhileState.setAttribute(SXA_ID,nesteWhileStateID);
    QDomElement nestedWhileTxnState = iDomDoc.createElement(SXN_TRANSITION);
    nestedWhileTxnState.setAttribute(SXA_TARGET,iStatOfSibling);

    QDomElement finalElem = iDomDoc.createElement(SXN_FINAL);
    finalElem.setAttribute(SXA_ID,QString("%1_Final").arg(nesteWhileStateID));
    nestedWhileState.appendChild(finalElem);

    nestedWhileState.appendChild(nestedWhileTxnState);
    nestedWhileState.setAttribute(SXA_INITIALSTATE,initialTarget);
    return rc;
}
/**
  * This function return the evaluated target ID,
  * if ipSourceNode is not the last node then next ipSourceNode->NextNode command node stat id is target id for the state
  * Otherwise following cases have different target ID
  * Case-1: if ipSourceNode is last node and have parent which is a while action, then while node state id => target id
  * Case-2: if ipSourceNode is last node and have parent which is not a while action, then parent's next node stat id => target id
  * ipSourceNode: The command node which target ID to be evaluated
  * return target ID of the provide node
  */
QString GTAExportScxml::getTargetId(const GTACmdNode * ipSourceNode)
{
    QString oTargetId = MAIN_FINAL_STATE;
    if(ipSourceNode != NULL)
    {
        if( ipSourceNode->NextNode != NULL)
            oTargetId = ipSourceNode->NextNode->getStateId();
        else
        {
            GTACmdNode * pParentNode = ipSourceNode->Parent;
            while(pParentNode != NULL)
            {
                if(pParentNode->IsWhileLoop)
                {
                    oTargetId = pParentNode->getStateId();
                    break;
                }
                else if(pParentNode->NextNode != NULL)
                {
                    oTargetId = pParentNode->NextNode->getStateId();
                    break;
                }
                pParentNode = pParentNode->Parent;
            }
        }

    }
    return oTargetId;
}

void GTAExportScxml::getExpresion(QString & iParam, const QString & iVal, const QString & iCond, QString & oTrueExpress, QString & oFalseExpress,bool isStringVal,const double iPrecision, const QString &iPrecisonUnit)
{
    bool IsParamBoolType = false;
    if(iVal.compare("true",Qt::CaseInsensitive) == 0 || iVal.compare("false",Qt::CaseInsensitive) ==0  || iVal.compare("YesNo",Qt::CaseInsensitive) == 0)
        IsParamBoolType = true;
    QString Val = iVal;


    if((isStringVal) && (!IsParamBoolType))
    {
        Val = QString("'%1'").arg(iVal);
        Val.replace("''","'");
    }
    if((iParam.contains("\""))   || (iParam.contains("\'")))
    {
        iParam = iParam.replace("\"","");
        iParam = QString("'%1'").arg(iParam);
        iParam.replace("''","'");
    }
    if(iCond == ARITH_EQ)
    {
        if(IsParamBoolType )
        {
            oTrueExpress = QString("%1 == %2").arg(iParam, Val);
            QString otherVal = Val.compare("true",Qt::CaseInsensitive) == 0 ? "false" : "true";
            oFalseExpress= QString("%1 == %2").arg(iParam,otherVal);
        }
        else
        {
            
            if(iPrecision > 0.000001)
            {
                bool isDoubleOk = false;
                iVal.toDouble(&isDoubleOk);
                if(isDoubleOk)
                {
                    if(iPrecisonUnit == ACT_PRECISION_UNIT_PERCENT)
                    {
                        oTrueExpress = QString("almostEqualPercent(%1,%2,%3) == true").arg(iParam, Val,QString::number(iPrecision,'f',5));
                        oFalseExpress= QString("notAlmostEqualPercent(%1,%2,%3) == true").arg(iParam,Val,QString::number(iPrecision,'f',5));
                    }
                    else
                    {
                        oTrueExpress = QString("almostEqualAbsolute(%1,%2,%3) == true").arg(iParam, Val,QString::number(iPrecision,'f',5));
                        oFalseExpress= QString("notAlmostEqualAbsolute(%1,%2,%3) == true").arg(iParam,Val,QString::number(iPrecision,'f',5));
                    }
                }
                else
                {
                    /*oTrueExpress = QString("(%1<=(%2) && %1>=(%3))== true").arg(iParam, QString::number(testValue+iPrecision,'f',5),QString::number(testValue-iPrecision,'f',5));
                    oFalseExpress= QString("(%1<=(%2) && %1>=(%3)) == false").arg(iParam,QString::number(testValue+iPrecision,'f',5),QString::number(testValue-iPrecision,'f',5));*/

                    if(iPrecisonUnit == ACT_PRECISION_UNIT_PERCENT)
                    {
                        oTrueExpress = QString("almostEqualPercent(%1,%2,%3) == true").arg(iParam, Val,QString::number(iPrecision,'f',5));
                        oFalseExpress= QString("notAlmostEqualPercent(%1,%2,%3) == true").arg(iParam,Val,QString::number(iPrecision,'f',5));
                    }
                    else
                    {
                        oTrueExpress = QString("almostEqualAbsolute(%1,%2,%3) == true").arg(iParam, Val,QString::number(iPrecision,'f',5));
                        oFalseExpress= QString("notAlmostEqualAbsolute(%1,%2,%3) == true").arg(iParam,Val,QString::number(iPrecision,'f',5));
                    }
                }
            }
            else
            {
                oTrueExpress = QString("%1 == %2").arg(iParam, Val);
                oFalseExpress= QString("%1 != %2 ").arg(iParam,Val);
            }
        }
    }
    else if (iCond == ARITH_NOTEQ)
    {
        if(IsParamBoolType )
        {
            QString otherVal = Val.compare("true",Qt::CaseInsensitive) == 0 ? "false" : "true";
            oTrueExpress = QString("%1 == %2").arg(iParam, otherVal);
            oFalseExpress= QString("%1 == %2").arg(iParam,Val);
        }
        else
        {
            bool isDoubleOk = false;
            iVal.toDouble(&isDoubleOk);
            if(iPrecision > 0.000001)
            {
                if (isDoubleOk)
                {
                    if (iPrecisonUnit==ACT_PRECISION_UNIT_PERCENT)
                    {
                        oTrueExpress= QString("notAlmostEqualPercent(%1,%2,%3) == true").arg(iParam,Val,QString::number(iPrecision,'f',5));
                        oFalseExpress = QString("almostEqualPercent(%1,%2,%3) == true").arg(iParam, Val,QString::number(iPrecision,'f',5));

                    }
                    else
                    {
                        oTrueExpress= QString("notAlmostEqualAbsolute(%1,%2,%3) == true").arg(iParam,Val,QString::number(iPrecision,'f',5));
                        oFalseExpress = QString("almostEqualAbsolute(%1,%2,%3) == true").arg(iParam, Val,QString::number(iPrecision,'f',5));

                    }
                }
                else
                {
                    /*oTrueExpress   = QString("%1 != %2 ").arg(iParam,Val);
                    oFalseExpress = QString("%1 == %2").arg(iParam, Val);*/

                    if (iPrecisonUnit==ACT_PRECISION_UNIT_PERCENT)
                    {
                        oTrueExpress= QString("notAlmostEqualPercent(%1,%2,%3) == true").arg(iParam,Val,QString::number(iPrecision,'f',5));
                        oFalseExpress = QString("almostEqualPercent(%1,%2,%3) == true").arg(iParam, Val,QString::number(iPrecision,'f',5));

                    }
                    else
                    {
                        oTrueExpress= QString("notAlmostEqualAbsolute(%1,%2,%3) == true").arg(iParam,Val,QString::number(iPrecision,'f',5));
                        oFalseExpress = QString("almostEqualAbsolute(%1,%2,%3) == true").arg(iParam, Val,QString::number(iPrecision,'f',5));

                    }
                }
            }
            else
            {
                oTrueExpress   = QString("%1 != %2 ").arg(iParam,Val);
                oFalseExpress = QString("%1 == %2").arg(iParam, Val);
            }
        }
    }
    else if (iCond == ARITH_LT)
    {
        oTrueExpress = QString("%1  < %2").arg(iParam, Val);
        oFalseExpress = QString("%1 >= %2").arg(iParam,Val);
    }
    else if (iCond == ARITH_GT)
    {
        oTrueExpress = QString("%1  > %2").arg(iParam, Val);
        oFalseExpress = QString("%1 <= %2 ").arg(iParam,Val);
    }
    else if (iCond == ARITH_LTEQ)
    {
        oTrueExpress = QString("%1  <= %2").arg(iParam, Val);
        oFalseExpress = QString("%1 > %2").arg(iParam,Val);
    }
    else if (iCond == ARITH_GTEQ)
    {
        oTrueExpress = QString("%1  >= %2").arg(iParam, Val);
        oFalseExpress = QString("%1 < %2").arg(iParam,Val);
    }
}

bool GTAExportScxml::createActionOneClickSetEnvState(const GTAOneClick *pCmd, QDomDocument &iDomDoc, QDomElement &, QDomElement &, 
                                                        const QString &iStateId, const QString &iTargetId, QDomElement &oRootElem)
{
    bool rc = false;
    if(pCmd != NULL)
    {
        QString actionName = pCmd->getAction();
        QString appVerInfo = pCmd->getApplicationName();
        QString configName = pCmd->getConfigName();
        QStringList options = pCmd->getOptionNames();
        QString testRigType = pCmd->getTestRigType();
        QString testRigName = pCmd->getTestRigName();
        QString actionOnFail = pCmd->getActionOnFail();
        QString tmplFile;
        bool isStartEnv = pCmd->getIsStartEnv();
        bool isStopTestconfig = pCmd->getIsStopTestConfig();
        bool isStopAlto = pCmd->getIsStopAlto();
        if(isStartEnv)
            tmplFile ="One_Click_Set_Environment.scxml";
        else if(isStopAlto)
            tmplFile ="One_Click_Stop_Alto.scxml";
        else if(isStopTestconfig)
            tmplFile ="One_Click_Stop_TestConfig.scxml";

        QString invokeId = QString(iStateId).replace('s','i');
        QStringList dumpListParams = pCmd->getDumpList();

        if(isStopAlto)
        {
            configName = "ATM";
            //testRigType = "Alto";
            //testRigName = "FIB_ATM";
        }
        else if(isStopTestconfig)
        {
            configName = iStateId;
            testRigType = "BIS_G";
            testRigName = "proto_evica";
        }
        QString version = "";

        QStringList temp = appVerInfo.split(":",QString::SkipEmptyParts);
        if(!temp.isEmpty())
        {
            version = temp.at(1);
            version = version.trimmed();
        }

        if(version == "nil")
        {
            version = "";
        }
        QStringList optionList;
        foreach(QString option,options)
        {
            QStringList optionVal = pCmd->getValueForOption(option);
            QString optionType = pCmd->getTypeofOption(option);
            QStringList dirNames = pCmd->getSelectedDirNames(option);
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
        QDomElement statElem;
        rc = getCloneRootElement(tmplFile,iDomDoc,statElem);
        if(rc && (!statElem.isNull()))
        {
            GTAScxmlDataModel objConfigName;
            objConfigName.ID = configName;
            objConfigName.Expr = "[]";
            _DataModelList.insert(objConfigName.ID,objConfigName);
            QString launchStatus = QString("%1.LaunchStatus").arg(configName);
            if(statElem.nodeName() == SXN_STATE)
            {
                statElem.setAttribute(SXA_ID,iStateId);
                setStateIDName(iStateId,pCmd);
                QDomNodeList statChildNodeList = statElem.childNodes();
                int statNodeCount = statChildNodeList.count();
                for(int s = 0 ; s <statNodeCount; s++)
                {
                    QDomNode statChildNode = statChildNodeList.at(s);


                    if(statChildNode.isElement())
                    {
                        QDomElement domStatChildElem = statChildNode.toElement();
                        QString nodeName =domStatChildElem.nodeName();
                        if(nodeName == SXN_ONENTRY)
                        {
                            QDomNodeList assignNodeList = domStatChildElem.elementsByTagName(SXN_ASSIGN);
                            if(! assignNodeList.isEmpty())
                            {
                                QDomElement assingElem= assignNodeList.item(0).toElement();
                                assingElem.setAttribute(SXA_LOCATION,configName);
                                QString expr = QString("{'ConfigurationName':'%3', 'version':'%1', 'options' :{%2}}").arg(version,optionStr,configName);
                                assingElem.setAttribute(SXA_EXPR,expr);
                            }
                        }
                        else if(nodeName == SXN_ONEXIT)
                        {

                            QDomNodeList childList = domStatChildElem.elementsByTagName(SXN_ASSIGN);
                            if(! childList.isEmpty())
                            {
                                if(!isStopTestconfig)
                                {
                                    QDomElement assignElem = childList.item(0).toElement();
                                    assignElem.setAttribute(SXA_LOCATION,configName);
                                    assignElem.setAttribute(SXA_EXPR,"[]");
                                    insertVariableOnExit(domStatChildElem,dumpListParams,iStateId,pCmd->getInstanceName(),iDomDoc);
                                }
                            }
                        }
                        else if(nodeName == SXN_TRANSITION)
                        {

                            QString trueCondExpr = launchStatus + "== 'OK'";
                            QString failCondExpr = launchStatus + "!= 'OK'";

                            QString event = QString("done.invoke.%1").arg(invokeId);
                            domStatChildElem.setAttribute(SXA_EVENT,event);
                            domStatChildElem.setAttribute(SXA_TARGET,iTargetId);
                            domStatChildElem.setAttribute(SXA_CONDITION,trueCondExpr);
                            domStatChildElem.setAttribute(SXA_STATUS,"success");

                            QDomElement ElemOnFalseTransit = iDomDoc.createElement(SXN_TRANSITION);
                            ElemOnFalseTransit.setAttribute(SXA_EVENT,event);
                            if(actionOnFail == ACT_FAIL_STOP)
                                ElemOnFalseTransit.setAttribute(SXA_TARGET,SXN_FINAL);
                            else
                                ElemOnFalseTransit.setAttribute(SXA_TARGET,iTargetId);


                            ElemOnFalseTransit.setAttribute(SXA_CONDITION,failCondExpr);
                            ElemOnFalseTransit.setAttribute(SXA_STATUS,"fail");;
                            statElem.appendChild(ElemOnFalseTransit);

                            //Log Node
                            GTAScxmlLogMessage objLog;
                            objLog.LOD = GTAScxmlLogMessage::Main;
                            
                            objLog.Identifier = pCmd->getInstanceName();
                            objLog.Status = GTAScxmlLogMessage::Success;
                            objLog.Result = GTAScxmlLogMessage::OK;
                            objLog.ExpectedValue = configName;
                            objLog.CurrentValue = configName;
                            QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
                            ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
                            ElemOnTrueLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                            domStatChildElem.appendChild(ElemOnTrueLog);

                            objLog.Status = GTAScxmlLogMessage::Fail;
                            objLog.Result = GTAScxmlLogMessage::KO;

                            QDomElement ElemOnFalseLog = iDomDoc.createElement(SXN_LOG);
                            ElemOnFalseLog.setAttribute(SXA_LABEL,"'info'");
                            ElemOnFalseLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                            ElemOnFalseTransit.appendChild(ElemOnFalseLog);
                        }
                        else if(nodeName == SXN_INVOKE)
                        {
                            domStatChildElem.setAttribute(SXA_ID,invokeId);

                            QDomNodeList invokeChildList = domStatChildElem.childNodes();
                            int invokeChildCount = invokeChildList.count();
                            for(int j = 0; j <invokeChildCount; j++)
                            {
                                QDomNode invokeChild = invokeChildList.at(j);
                                if(invokeChild.isElement())
                                {
                                    QDomElement invokeChildElem =  invokeChild.toElement();
                                    QString invokeChildName = invokeChildElem.nodeName();
                                    if(invokeChildName == SXN_CONTENT)
                                    {
                                        if(!invokeChildElem.isNull())
                                        {
                                            QDomNodeList funcList = invokeChildElem.elementsByTagName(SXN_FUNCTION);
                                            if(! funcList.isEmpty())
                                            {
                                                QDomNodeList funcChildren = funcList.item(0).toElement().childNodes();
                                                for(int f= 0; f < funcChildren.count(); f++)
                                                {
                                                    QDomElement funcChildElem = funcChildren.item(f).toElement();
                                                    QString nodeName = funcChildElem.nodeName();
                                                    if(nodeName == SXN_ARGUMENT)
                                                    {
                                                        funcChildElem.setAttribute(SXA_VALUE,configName);
                                                    }
                                                }
                                            }
                                            QDomNodeList XmlRpc = invokeChildElem.elementsByTagName(SXN_XMLRPC);
                                            if(!XmlRpc.isEmpty())
                                            {
                                                if(!isStopTestconfig)
                                                {
                                                    QDomElement xmlrpcNode = XmlRpc.at(0).toElement();
                                                    xmlrpcNode.setAttribute(SXA_TOOLTYPE,testRigType);
                                                    xmlrpcNode.setAttribute(SXA_TOOLID,testRigName);
                                                }
                                            }
                                        }
                                    }
                                    else if(invokeChildName == SXN_FINALIZE)
                                    {
                                        QDomNodeList finalChildren = invokeChildElem.childNodes();
                                        QDomElement sendElem;
                                        QDomElement cntrAssingElem;
                                        for(int x = 0; x < finalChildren.count(); x++)
                                        {
                                            QDomElement domElem = finalChildren.item(x).toElement();
                                            QString nodeName = domElem.nodeName();
                                            if(nodeName == SXN_ASSIGN)
                                            {
                                                domElem.setAttribute(SXA_LOCATION,launchStatus);

                                                cntrAssingElem = domElem.cloneNode().toElement();
                                                cntrAssingElem.setAttribute(SXA_LOCATION,"OneClickCounter");
                                                cntrAssingElem.setAttribute(SXA_EXPR,"OneClickCounter + 1");
                                                
                                                sendElem = iDomDoc.createElement(SXN_SEND);
                                                sendElem.setAttribute(SXA_EVENT,QString("%2_%1").arg(iStateId,"Event"));
                                            }


                                            
                                        }
                                        if(!cntrAssingElem.isNull())
                                            invokeChildElem.appendChild(cntrAssingElem);
                                        if(!sendElem.isNull())
                                            invokeChildElem.appendChild(sendElem);
                                    }
                                }
                            }

                        }
                    }
                }
                oRootElem = statElem;
                //iRootElem.appendChild(statElem);
            }
        }
    }
    return rc;
}

bool GTAExportScxml::createActionOneClickPPCState(const GTAActionOneClickPPC *pCmd, QDomDocument &iDomDoc, QDomElement &, QDomElement &, const QString &iStateId, const QString &iTargetId,QDomElement& oRootElem)
{
    bool rc = false;
    if(pCmd != NULL)
    {
        QString actionName = pCmd->getAction();
        QString ppcPath = pCmd->getPPCPath();
        QString actionOnFail = pCmd->getActionOnFail();

        QString tmplFile  = "Setup_PinProg.scxml";
        QString invokeId = QString(iStateId).replace('s','i');
        QStringList dumpListParams = pCmd->getDumpList();
        bool isStart = pCmd->getIsStart();
        if(!isStart)
        {
            tmplFile = "One_Click_Stop_PPC.scxml";
            ppcPath  = "";
        }



        QDomElement statElem;
        rc = getCloneRootElement(tmplFile,iDomDoc,statElem);
        if(rc && (!statElem.isNull()))
        {
            QString launchStatus = QString("%1_PPCReturn").arg(iStateId);
            if(statElem.nodeName() == SXN_STATE)
            {
                statElem.setAttribute(SXA_ID,iStateId);
                setStateIDName(iStateId,pCmd);
                QDomNodeList statChildNodeList = statElem.childNodes();
                int statNodeCount = statChildNodeList.count();
                for(int s = 0 ; s <statNodeCount; s++)
                {
                    QDomNode statChildNode = statChildNodeList.at(s);


                    if(statChildNode.isElement())
                    {
                        QDomElement domStatChildElem = statChildNode.toElement();
                        QString nodeName =domStatChildElem.nodeName();
                        if(nodeName == SXN_TRANSITION)
                        {


                            QString trueCondExpr = launchStatus + "== 'OK'";
                            QString failCondExpr = launchStatus + "!= 'OK'";

                            QString event = QString("done.invoke.%1").arg(invokeId);
                            domStatChildElem.setAttribute(SXA_EVENT,event);
                            domStatChildElem.setAttribute(SXA_TARGET,iTargetId);
                            domStatChildElem.setAttribute(SXA_CONDITION,trueCondExpr);
                            domStatChildElem.setAttribute(SXA_STATUS,"success");

                            QDomElement ElemOnFalseTransit = iDomDoc.createElement(SXN_TRANSITION);
                            ElemOnFalseTransit.setAttribute(SXA_EVENT,event);
                            if(actionOnFail == ACT_FAIL_STOP)
                                ElemOnFalseTransit.setAttribute(SXA_TARGET,SXN_FINAL);
                            else
                                ElemOnFalseTransit.setAttribute(SXA_TARGET,iTargetId);


                            ElemOnFalseTransit.setAttribute(SXA_CONDITION,failCondExpr);
                            ElemOnFalseTransit.setAttribute(SXA_STATUS,"fail");;
                            statElem.appendChild(ElemOnFalseTransit);

                            //Log Node
                            GTAScxmlLogMessage objLog;
                            objLog.LOD = GTAScxmlLogMessage::Main;

                            objLog.Identifier = pCmd->getInstanceName();
                            objLog.Status = GTAScxmlLogMessage::Success;
                            objLog.Result = GTAScxmlLogMessage::OK;
                            objLog.ExpectedValue = "Launch Status";
                            objLog.CurrentValue = launchStatus;
                            QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
                            ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
                            ElemOnTrueLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                            domStatChildElem.appendChild(ElemOnTrueLog);

                            objLog.Status = GTAScxmlLogMessage::Fail;
                            objLog.Result = GTAScxmlLogMessage::KO;

                            QDomElement ElemOnFalseLog = iDomDoc.createElement(SXN_LOG);
                            ElemOnFalseLog.setAttribute(SXA_LABEL,"'info'");
                            ElemOnFalseLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                            ElemOnFalseTransit.appendChild(ElemOnFalseLog);
                        }
                        else if(nodeName == SXN_INVOKE)
                        {
                            domStatChildElem.setAttribute(SXA_ID,invokeId);

                            QDomNodeList invokeChildList = domStatChildElem.childNodes();
                            int invokeChildCount = invokeChildList.count();
                            for(int j = 0; j <invokeChildCount; j++)
                            {
                                QDomNode invokeChild = invokeChildList.at(j);
                                if(invokeChild.isElement())
                                {
                                    QDomElement invokeChildElem =  invokeChild.toElement();
                                    QString invokeChildName = invokeChildElem.nodeName();
                                    if(invokeChildName == SXN_CONTENT)
                                    {
                                        if(!invokeChildElem.isNull())
                                        {
                                            QDomNodeList funcList = invokeChildElem.elementsByTagName(SXN_FUNCTION);
                                            if(! funcList.isEmpty())
                                            {
                                                QDomNodeList funcChildren = funcList.item(0).toElement().childNodes();
                                                for(int f= 0; f < funcChildren.count(); f++)
                                                {
                                                    QDomElement funcChildElem = funcChildren.item(f).toElement();
                                                    QString nodeName = funcChildElem.nodeName();
                                                    if(nodeName == SXN_ARGUMENT)
                                                    {
                                                        QDomNode argNode = funcChildElem.attributes().namedItem("name");
                                                        if(!argNode.isNull() && (argNode.nodeValue() == "args_list"))
                                                        {
                                                            if(!ppcPath.isEmpty())
                                                                funcChildElem.setAttribute(SXA_VALUE,QString("['%1']").arg(ppcPath));
                                                            else
                                                                funcChildElem.setAttribute(SXA_VALUE,QString("[]"));
                                                        }

                                                    }
                                                    else if(nodeName == SXN_RETURNVALUE)
                                                    {

                                                        funcChildElem.setAttribute(SXA_NAME,launchStatus);

                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if(invokeChildName == SXN_FINALIZE)
                                    {
                                        QDomNodeList finalChildren = invokeChildElem.childNodes();
                                        QDomElement cntrAssingElem;
                                        QDomElement sendElem;
                                        for(int x = 0; x < finalChildren.count(); x++)
                                        {
                                            QDomElement domElem = finalChildren.item(x).toElement();
                                            QString nodeName = domElem.nodeName();
                                            if(nodeName == SXN_ASSIGN)
                                            {
                                                domElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(launchStatus));
                                                domElem.setAttribute(SXA_LOCATION,launchStatus);
                                                cntrAssingElem = domElem.cloneNode().toElement();
                                                cntrAssingElem.setAttribute(SXA_LOCATION,"OneClickCounter");
                                                cntrAssingElem.setAttribute(SXA_EXPR,"OneClickCounter + 1");
                                                sendElem = iDomDoc.createElement(SXN_SEND);
                                                sendElem.setAttribute(SXA_EVENT,QString("%2_%1").arg(iStateId,"Event"));
                                            }
                                            if(nodeName == SXN_LOG)
                                            {
                                                domElem.setAttribute(SXA_EXPR,launchStatus);
                                            }

                                        }
                                        if(!cntrAssingElem.isNull())
                                            invokeChildElem.appendChild(cntrAssingElem);
                                        if(!sendElem.isNull())
                                            invokeChildElem.appendChild(sendElem);
                                    }
                                }
                            }

                        }
                    }
                }
                //iRootElem.appendChild(statElem);
                oRootElem = statElem;

            }
        }
    }
    return rc;
}


bool GTAExportScxml::createActionOneClickLauncAppState(const GTAOneClickLaunchApplication *pCmd, QDomDocument &iDomDoc, QDomElement &, QDomElement &, const QString &iStateId, const QString &iTargetId,QDomElement &oRootElem)
{
    bool rc = false;
    if(pCmd != NULL)
    {
        QString actionName = pCmd->getAction();
        QString actionOnFail = pCmd->getActionOnFail();


        GTAOneClickLaunchApplicationItemStruct * itemStruct;
        QList<GTAOneClickLaunchApplicationItemStruct *>itemStructList = pCmd->getLaStruct();



        if(itemStructList.isEmpty())
            return false;
        else
            itemStruct = itemStructList.first();

        bool killMode = itemStruct->getKillMode();
        QStringList optionNameList = itemStruct->getOptions();
        QString tmplFile  = "OneClick_Launch_Application.scxml";
        if(killMode)
        {
            //if(optionNameList.isEmpty())
            tmplFile  = "OneClick_Kill_Application.scxml";
        }


        QString invokeId = QString(iStateId).replace('s','i');


        
        QString option = "";
        if(!optionNameList.isEmpty())
        {
            option = optionNameList.first();
        }
        QString optVal = itemStruct->getValue(option);
        QString arg = itemStruct->getArgs(option);
        QString toolId = itemStruct->getToolId();
        QString toolType = itemStruct->getToolType();
        QString appName = itemStruct->getAppName();
        if((killMode) && optionNameList.isEmpty())
        {
            optVal = itemStruct->getAppName();
        }
        QDomElement statElem;
        rc = getCloneRootElement(tmplFile,iDomDoc,statElem);
        if(rc && (!statElem.isNull()))
        {
            QString launchStatus = QString("%1_LaunchApp").arg(iStateId);
            if(statElem.nodeName() == SXN_STATE)
            {
                statElem.setAttribute(SXA_ID,iStateId);
                setStateIDName(iStateId,pCmd);
                QDomNodeList statChildNodeList = statElem.childNodes();
                int statNodeCount = statChildNodeList.count();
                for(int s = 0 ; s <statNodeCount; s++)
                {
                    QDomNode statChildNode = statChildNodeList.at(s);


                    if(statChildNode.isElement())
                    {
                        QDomElement domStatChildElem = statChildNode.toElement();
                        QString nodeName =domStatChildElem.nodeName();
                        if(nodeName == SXN_TRANSITION)
                        {


                            QString trueCondExpr = launchStatus + "== 'OK'";
                            QString failCondExpr = launchStatus + "!= 'OK'";

                            QString event = QString("done.invoke.%1").arg(invokeId);
                            domStatChildElem.setAttribute(SXA_EVENT,event);
                            domStatChildElem.setAttribute(SXA_TARGET,iTargetId);
                            domStatChildElem.setAttribute(SXA_CONDITION,trueCondExpr);
                            domStatChildElem.setAttribute(SXA_STATUS,"success");

                            QDomElement ElemOnFalseTransit = iDomDoc.createElement(SXN_TRANSITION);
                            ElemOnFalseTransit.setAttribute(SXA_EVENT,event);
                            if(actionOnFail == ACT_FAIL_STOP)
                                ElemOnFalseTransit.setAttribute(SXA_TARGET,SXN_FINAL);
                            else
                                ElemOnFalseTransit.setAttribute(SXA_TARGET,iTargetId);


                            ElemOnFalseTransit.setAttribute(SXA_CONDITION,failCondExpr);
                            ElemOnFalseTransit.setAttribute(SXA_STATUS,"fail");;
                            statElem.appendChild(ElemOnFalseTransit);

                            //Log Node
                            GTAScxmlLogMessage objLog;
                            objLog.LOD = GTAScxmlLogMessage::Main;

                            objLog.Identifier = pCmd->getInstanceName();
                            objLog.Status = GTAScxmlLogMessage::Success;
                            objLog.Result = GTAScxmlLogMessage::OK;
                            objLog.ExpectedValue = "Launch Status";
                            objLog.CurrentValue = launchStatus;
                            QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
                            ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
                            ElemOnTrueLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                            domStatChildElem.appendChild(ElemOnTrueLog);

                            objLog.Status = GTAScxmlLogMessage::Fail;
                            objLog.Result = GTAScxmlLogMessage::KO;

                            QDomElement ElemOnFalseLog = iDomDoc.createElement(SXN_LOG);
                            ElemOnFalseLog.setAttribute(SXA_LABEL,"'info'");
                            ElemOnFalseLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                            ElemOnFalseTransit.appendChild(ElemOnFalseLog);
                        }
                        else if(nodeName == SXN_INVOKE)
                        {
                            domStatChildElem.setAttribute(SXA_ID,invokeId);

                            QDomNodeList invokeChildList = domStatChildElem.childNodes();
                            int invokeChildCount = invokeChildList.count();
                            for(int j = 0; j <invokeChildCount; j++)
                            {
                                QDomNode invokeChild = invokeChildList.at(j);
                                if(invokeChild.isElement())
                                {
                                    QDomElement invokeChildElem =  invokeChild.toElement();
                                    QString invokeChildName = invokeChildElem.nodeName();
                                    if(invokeChildName == SXN_CONTENT)
                                    {
                                        if(!invokeChildElem.isNull())
                                        {
                                            QDomNode xmlRPC = invokeChildElem.namedItem(SXN_XMLRPC);
                                            if(!xmlRPC.isNull())
                                            {
                                                xmlRPC.toElement().setAttribute(SXA_TOOLID,toolId);
                                                xmlRPC.toElement().setAttribute(SXA_TOOLTYPE,toolType);
                                            }
                                            QDomNodeList funcList = invokeChildElem.elementsByTagName(SXN_FUNCTION);
                                            if(! funcList.isEmpty())
                                            {
                                                QDomNodeList funcChildren = funcList.item(0).toElement().childNodes();
                                                for(int f= 0; f < funcChildren.count(); f++)
                                                {
                                                    QDomElement funcChildElem = funcChildren.item(f).toElement();
                                                    QString nodeName = funcChildElem.nodeName();
                                                    if(nodeName == SXN_ARGUMENT)
                                                    {
                                                        QDomNode argNode = funcChildElem.attributes().namedItem("name");
                                                        if(!argNode.isNull() && (argNode.nodeValue() == "appname"))
                                                        {
                                                            if(!optVal.isEmpty())
                                                                funcChildElem.setAttribute(SXA_VALUE,QString("'%1'").arg(optVal));
                                                            else
                                                                funcChildElem.setAttribute(SXA_VALUE,QString("'%1'").arg(appName));
                                                        }
                                                        else if(!argNode.isNull() && (argNode.nodeValue() == "args_list"))
                                                        {
                                                            if(!arg.isEmpty())
                                                                funcChildElem.setAttribute(SXA_VALUE,arg);
                                                            else
                                                                funcChildElem.setAttribute(SXA_VALUE,"[]");
                                                        }

                                                    }
                                                    else if(nodeName == SXN_RETURNVALUE)
                                                    {

                                                        funcChildElem.setAttribute(SXA_NAME,launchStatus);

                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if(invokeChildName == SXN_FINALIZE)
                                    {
                                        QDomNodeList finalChildren = invokeChildElem.childNodes();
                                        QDomElement cntrAssingElem;
                                        QDomElement sendElem;
                                        for(int x = 0; x < finalChildren.count(); x++)
                                        {
                                            QDomElement domElem = finalChildren.item(x).toElement();
                                            QString nodeName = domElem.nodeName();
                                            if(nodeName == SXN_ASSIGN)
                                            {
                                                domElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(launchStatus));
                                                domElem.setAttribute(SXA_LOCATION,launchStatus);
                                                cntrAssingElem = domElem.cloneNode().toElement();
                                                cntrAssingElem.setAttribute(SXA_LOCATION,"OneClickCounter");
                                                cntrAssingElem.setAttribute(SXA_EXPR,"OneClickCounter + 1");
                                                sendElem = iDomDoc.createElement(SXN_SEND);
                                                sendElem.setAttribute(SXA_EVENT,QString("%2_%1").arg(iStateId,"Event"));
                                            }
                                            if(nodeName == SXN_LOG)
                                            {
                                                domElem.setAttribute(SXA_EXPR,launchStatus);
                                            }

                                        }
                                        if(!cntrAssingElem.isNull())
                                            invokeChildElem.appendChild(cntrAssingElem);
                                        if(!sendElem.isNull())
                                            invokeChildElem.appendChild(sendElem);
                                    }
                                }
                            }

                        }
                    }
                }
                //iRootElem.appendChild(statElem);
                oRootElem = statElem;
            }
        }
    }
    return rc;
}

/**
  * This function create the scxml state for a IRT action
  * pCmd: IRT command
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iLogStatElem : The log state where the log of the command is inserted
  * iStateId: State id of the command;
  * iTargetId: The next ID where the stat should go after execution
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::createActionIRTState(const GTAActionIRT *pCmd,QDomDocument & iDomDoc, QDomElement & iRootElem,QDomElement & iLogStatElem,
                                             const QString & iStateId, const QString & iTargetId)
{
    bool rc = false;
    if(pCmd != NULL)
    {
        QString actionName = pCmd->getAction();
        QString instanceName = pCmd->getInstanceName();
        QByteArray functionCode = pCmd->getFunctionCode();
        if(functionCode.isNull() || functionCode.isEmpty())
            return rc ;
        QDomDocument xmlrpcDomDoc;
        xmlrpcDomDoc.setContent(functionCode);
        //processIRTXmlRpc(xmlrpcDomDoc);
        QDomElement xmlrpcRootElem = xmlrpcDomDoc.documentElement();
        QDomNodeList xmlRpcFuncList = xmlrpcRootElem.childNodes();
        int functionCount = xmlRpcFuncList.count();
        for(int i = 0; i < functionCount; i++)
        {
            QDomNode xmlRpcFunc = xmlRpcFuncList.item(i);

            QString endId = iStateId;
            QString startId = iStateId;
            if(i != 0 )
            {
                startId += QString("_%1").arg(QString::number(i));
            }
            endId += QString("_%1").arg(QString::number(i+1));
            if(i == functionCount - 1)
                endId = iTargetId;

            QString tmplFile  = actionName.replace(" ","_") + ".scxml";
            QString invokeId = QString(startId).replace('s','i');

            QString variableName = QString("Result_invoke_%1").arg(invokeId);
            QString retVarName = QString("Return_invoke_%1").arg(invokeId);
            QString flagVarName = QString("Flag_invoke_%1").arg(invokeId);

            QDomElement dBArgumentNode; 
            QString irtFunctionName;
            QDomElement oRootElemOnTmpl;
            rc = getCloneRootElement(tmplFile,iDomDoc,oRootElemOnTmpl);
            if(rc && (!oRootElemOnTmpl.isNull()))
            {
                QDomElement oElem = iDomDoc.importNode(oRootElemOnTmpl,true).toElement();
                if(oElem.nodeName() == SXN_STATE)
                {
                    oElem.setAttribute(SXA_ID,startId);
                    setStateIDName(startId,pCmd);
                    QDomNodeList statChildNodeList = oElem.childNodes();
                    int statNodeCount = statChildNodeList.count();
                    for(int s = 0 ; s <statNodeCount; s++)
                    {
                        QDomNode statChildNode = statChildNodeList.at(s);
                        if(statChildNode.isElement())
                        {
                            QDomElement domStatChildElem = statChildNode.toElement();
                            QString nodeName =domStatChildElem.nodeName();
                            if(nodeName == SXN_TRANSITION)
                            {
                                QString trueExpr = retVarName + ".StatusCode == 0";
                                QString falseExpr = retVarName + ".StatusCode != 0";

                                QString event = QString("done.invoke.%1").arg(invokeId);
                                domStatChildElem.setAttribute(SXA_EVENT,event);
                                domStatChildElem.setAttribute(SXA_TARGET,endId);
                                domStatChildElem.setAttribute(SXA_CONDITION,trueExpr);

                                domStatChildElem.setAttribute(SXA_STATUS,"success");
                                QDomElement ElemOnFalseTransit = domStatChildElem.cloneNode().toElement();
                                ElemOnFalseTransit.setAttribute(SXA_CONDITION,falseExpr);
                                ElemOnFalseTransit.setAttribute(SXA_STATUS,"fail");
                                oElem.insertAfter(ElemOnFalseTransit,domStatChildElem);

                                //Log Node
                                GTAScxmlLogMessage objLog;
                                objLog.LOD = GTAScxmlLogMessage::Main;
                                objLog.Identifier =instanceName;
                                objLog.ExpectedValue = retVarName;
                                objLog.CurrentValue = retVarName;
                                objLog.Status = GTAScxmlLogMessage::Success;
                                objLog.Result = GTAScxmlLogMessage::OK;

                                QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
                                ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
                                ElemOnTrueLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                                domStatChildElem.appendChild(ElemOnTrueLog);

                                objLog.Status = GTAScxmlLogMessage::Fail;
                                objLog.Result = GTAScxmlLogMessage::KO;

                                QDomElement ElemOnFalseLog = ElemOnTrueLog.cloneNode().toElement();
                                ElemOnFalseLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                                ElemOnFalseTransit.appendChild(ElemOnFalseLog);

                            }
                            else if(nodeName == SXN_INVOKE)
                            {
                                domStatChildElem.setAttribute(SXA_ID,invokeId);

                                QDomNodeList invokeChildList = domStatChildElem.childNodes();
                                int invokeChildCount = invokeChildList.count();
                                for(int j = 0; j <invokeChildCount; j++)
                                {
                                    QDomNode invokeChild = invokeChildList.at(j);
                                    if(invokeChild.isElement())
                                    {
                                        QDomElement invokeChildElem =  invokeChild.toElement();
                                        QString invokeChildName = invokeChildElem.nodeName();
                                        if(invokeChildName == SXN_CONTENT)
                                        {
                                            if(! xmlRpcFunc.isNull() && (!invokeChildElem.isNull()))
                                            {
                                                QDomElement xmlrpcElem;
                                                if(! iDomDoc.isNull())
                                                {
                                                    QString nodeName = xmlRpcFunc.nodeName();
                                                    nodeName = nodeName.remove("Step").toLower();
                                                    QDomNode importedNode = iDomDoc.importNode(xmlRpcFunc,true);
                                                    if(! importedNode.isNull() && importedNode.isElement())
                                                    {
                                                        xmlrpcElem = importedNode.toElement();
                                                        xmlrpcElem.setTagName(nodeName);
                                                        invokeChildElem.appendChild(xmlrpcElem);
                                                        QDomNodeList returnValList = xmlrpcElem.elementsByTagName("returnvalue");
                                                        if(! returnValList.isEmpty() && ! returnValList.item(0).isNull())
                                                        {
                                                            QDomElement returnValElem = returnValList.item(0).toElement();
                                                            returnValElem.setAttribute(SXA_NAME,variableName);
                                                        }

                                                        QDomNodeList funcNodeList = xmlrpcElem.elementsByTagName(SXN_FUNCTION);
                                                        if(! funcNodeList.isEmpty())
                                                        {
                                                            QDomElement irtFuncElem = funcNodeList.item(0).toElement();
                                                            if(!irtFuncElem.isNull())
                                                            {
                                                                irtFunctionName = irtFuncElem.attribute(SXA_NAME);
                                                                QDomNodeList argList = irtFuncElem.elementsByTagName(SXN_ARGUMENT);
                                                                for (int ai=0;ai<argList.size();i++)
                                                                {
                                                                    QDomElement argumentDB = argList.at(i).toElement();
                                                                    if (argumentDB.attribute(SXA_NAME)=="databaseId")
                                                                    {
                                                                        dBArgumentNode = argumentDB;
                                                                        break;
                                                                    }
                                                                }
                                                            }
                                                        }

                                                    }
                                                }
                                            }
                                        }
                                        else if(invokeChildName == SXN_FINALIZE)
                                        {
                                            QDomElement assignResultElem =  iDomDoc.createElement(SXN_ASSIGN);
                                            assignResultElem.setAttribute(SXA_LOCATION,retVarName);
                                            assignResultElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(variableName));

                                            QDomElement assignFlagElem =  iDomDoc.createElement(SXN_ASSIGN);
                                            assignFlagElem.setAttribute(SXA_LOCATION,flagVarName);
                                            assignFlagElem.setAttribute(SXA_EXPR,QString("'%1'").arg(SX_FLAG_EXECUTED));

                                            invokeChildElem.appendChild(assignResultElem);
                                            invokeChildElem.appendChild(assignFlagElem);

                                            //QDomNodeList logNodeList = invokeChildElem.elementsByTagName(SXN_LOG);
                                            //if(logNodeList.count()>0)
                                            //{
                                            //    QDomElement logElem = logNodeList.at(0).toElement();
                                            //    logElem.setAttribute(SXA_EXPR,QString("'%1: %2'").arg(startId,instanceName));
                                            //}

                                            GTAScxmlDataModel objDMReturn;
                                            objDMReturn.ID = retVarName;

                                            GTAScxmlDataModel objDMFlag;
                                            objDMFlag.ID = flagVarName;
                                            objDMFlag.Expr = QString("'%1'").arg(SX_FLAG_TBE);
                                            _DataModelList.insert(retVarName,objDMReturn);
                                            _DataModelList.insert(flagVarName,objDMFlag);

                                        }
                                    }
                                }

                            }
                        }
                    }

                    iRootElem.appendChild(oElem);

                    //time out in case if parent is a while or do while statement
                    GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
                    if(pParent != NULL)
                    {
                        createLoopTimeOutTranstions(oElem,iDomDoc,startId,pParent);
                    }
                    createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pCmd,endId);
                    //
                    QString logScxmlTmpl = QString("Log_%1.scxml").arg(irtFunctionName);
                    createActionIRTLogState(iDomDoc,iLogStatElem,startId,flagVarName,logScxmlTmpl,instanceName,dBArgumentNode);
                }

            }
        }

    }
    return rc;
}

void GTAExportScxml::processIRTXmlRpc(QDomDocument& iXMLRPCDomDoc)
{
    if (!iXMLRPCDomDoc.isNull())
    {
        QDomElement xmlrpcRootElem = iXMLRPCDomDoc.documentElement();
        QDomNodeList argumentNodes = xmlrpcRootElem.elementsByTagName("argument");
        for(int i=0;i<argumentNodes.size();i++)
        {
            QDomNode currNode = argumentNodes.at(i);
            if (!currNode.isNull())
            {
                QString typeAttributeVal= currNode.toElement().attribute("type");
                if (typeAttributeVal.contains("string") || typeAttributeVal.contains("boolean"))
                {
                    QString valueAttributeVal = currNode.toElement().attribute("value");
                    if (!valueAttributeVal.startsWith("'"))
                    {
                        valueAttributeVal= QString("'%1'").arg(valueAttributeVal);
                        currNode.toElement().setAttribute("value",valueAttributeVal);
                    }
                }
            }
        }
    }
}
/**
  * This function create the scxml state for a IRT Log stat
  * pCmd: IRT command
  * iDomDoc: state xml is create in the context of this document
  * iLogState: Element where the created state is inserted
  * iStateId: State id of the command;
  * iExecutionFlag: Execution enum value. Executed,Logged
  * iTemplFileName: scxml template file path of IRT log
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::createActionIRTLogState(QDomDocument &iDomDoc, QDomElement &iLogState, const QString &iStateId,
                                                const QString &iExecutionFlag, const QString &iTemplFileName, const QString &iCmdInstanceName,const QDomElement& iFunDBArgNode)
{
    bool rc = false;
    if(! iLogState.isNull())
    {
        QString logStatID = QString("Log_%1").arg(iStateId);
        QString logInvokeID = QString(logStatID).replace("s","i");
        QString resultVar = QString("Result_%1").arg(logInvokeID);
        QString returnVar = QString("Return_%1").arg(logInvokeID);
        //retrive transition
        QDomNodeList logStatChildren = iLogState.childNodes();
        QDomElement logInitStatElem;
        for(int i = 0; i < logStatChildren.count(); i++)
        {
            QDomElement domElem = logStatChildren.at(i).toElement();
            if(! domElem.isNull() && domElem.nodeName()== SXN_STATE && domElem.attribute(SXA_ID) == "Log_Init")
            {
                logInitStatElem = domElem;
                break;
            }
        }
        //create nee transition
        if(! logInitStatElem.isNull())
        {
            QDomElement transitElem = iDomDoc.createElement(SXN_TRANSITION);
            transitElem.setAttribute(SXA_CONDITION,QString("%1=='%2'").arg(iExecutionFlag,SX_FLAG_EXECUTED));
            transitElem.setAttribute(SXA_TARGET,logStatID);
            transitElem.setAttribute(SXA_STATUS,"success");
            logInitStatElem.appendChild(transitElem);
        }
        QDomElement logStat;
        rc = getCloneRootElement(iTemplFileName,iDomDoc,logStat);
        if(rc && (!logStat.isNull()))
        {
            iLogState.appendChild(logStat);
            logStat.setAttribute(SXA_ID,logStatID);
            QDomNodeList invokList = logStat.elementsByTagName(SXN_INVOKE);
            if(! invokList.isEmpty())
            {
                QDomElement invokElem = invokList.item(0).toElement();
                if(! invokElem.isNull())
                {
                    invokElem.setAttribute(SXA_ID,logInvokeID);

                    QDomNodeList invokeChildren = invokElem.childNodes();
                    for(int i = 0; i < invokeChildren.count(); i++)
                    {
                        QDomElement invokeChild = invokeChildren.item(i).toElement();
                        if(invokeChild.nodeName() == SXN_CONTENT)
                        {
                            QDomNodeList returnValList = invokeChild.elementsByTagName(SXN_RETURNVALUE);
                            if(!returnValList.isEmpty())
                            {
                                QDomElement retValElem = returnValList.item(0).toElement();
                                if(! retValElem.isNull())
                                    retValElem.setAttribute(SXA_NAME,resultVar);
                            }
                            QDomNodeList funNodeList = invokeChild.elementsByTagName(SXN_FUNCTION);
                            if (!funNodeList.isEmpty() && iFunDBArgNode.isNull()==false)
                            {
                                QDomElement funElem = funNodeList.item(0).toElement();
                                
                                if(! funElem.isNull())
                                {
                                    QDomNodeList argList = funElem.elementsByTagName(SXN_ARGUMENT);

                                    for (int ai=0;ai<argList.size();i++)
                                    {
                                        QDomElement argumentDB = argList.at(i).toElement();
                                        if (argumentDB.attribute(SXA_VALUE).isEmpty()==false)
                                        {
                                            argumentDB.setAttribute(SXA_VALUE,QString("Return_invoke_i%1.Screenshot1").arg(QString::number(i),QString::number(i+1)));
                                            break;
                                        }
                                    }
                                    if(!argList.isEmpty())
                                        funElem.insertBefore(iFunDBArgNode.cloneNode(true),argList.at(0));
                                    else
                                        funElem.appendChild(iFunDBArgNode.cloneNode(true));
                                }
                            }
                        }
                        else if(invokeChild.nodeName() == SXN_FINALIZE)
                        {
                            int assignCount = 0;
                            QDomNodeList finalizeChildren = invokeChild.childNodes();
                            for(int j = 0; j < finalizeChildren.count(); j++)
                            {
                                QDomElement domElem = finalizeChildren.item(j).toElement();
                                if(! domElem.isNull())
                                {
                                    
                                    QString nodeName = domElem.nodeName();
                                    if(nodeName == SXN_ASSIGN && assignCount == 0)
                                    {
                                        assignCount++;
                                        domElem.setAttribute(SXA_LOCATION,iExecutionFlag);
                                        domElem.setAttribute(SXA_EXPR,QString("'%1'").arg(SX_FLAG_LOGGED));
                                    }
                                    else if(nodeName == SXN_ASSIGN && assignCount == 1)
                                    {
                                        assignCount++;
                                        domElem.setAttribute(SXA_LOCATION,returnVar);
                                        domElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(resultVar));

                                        GTAScxmlDataModel objDMReturn;
                                        objDMReturn.ID = returnVar;
                                        _DataModelList.insert(returnVar,objDMReturn);
                                    }
                                    //else if(nodeName == SXN_LOG)
                                    //{
                                    //    domElem.setAttribute(SXA_EXPR,QString("'%1: ' +%2").arg(logStatID,returnVar));
                                    //}

                                }
                            }
                        }
                    }
                }
            }

            //Logging
            QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
            logStat.appendChild(ElemOnExit);
            insertVariableOnExit(ElemOnExit,QStringList(returnVar),logStatID,iCmdInstanceName,iDomDoc);

            


        }



        //        GTAScxmlLogMessage objLog;
        //        objLog.LOD = logStatID;
        //        objLog.Identifier = iCmdInstanceName;
        //        objLog.ExpectedValue = returnVar;
        //        objLog.CurrentValue= QString("'+%1+'").arg(returnVar);
        //        objLog.Status = GTAScxmlLogMessage::None;
        //        objLog.Result = GTAScxmlLogMessage::NA;

        //        QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
        //        transitElem.appendChild(ElemOnLog);
        //        ElemOnLog.setAttribute(SXA_LABEL,"'info'");
        //        ElemOnLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
    }

    return rc;
}
void GTAExportScxml::insertApplicationDataModelInitEnv(const QString & iAppName, const QString & iMedia, const QString & iBusName,const QString &iActor)
{
    GTAScxmlDataModel objAppDM;
    if (iMedia.startsWith(PARAM_TYPE_PIR,Qt::CaseInsensitive))
        objAppDM.IsPIR=true;

    if(_DataModelListForOneClick.contains(iAppName))
    {
        objAppDM = _DataModelListForOneClick.value(iAppName);
        if(objAppDM.IsApplication)
        {
            objAppDM.insert(iMedia,iBusName,iActor);
        }
    }
    else
    {
        objAppDM.ID = iAppName;
        objAppDM.insert(iMedia,iBusName,iActor);
        objAppDM.IsApplication = true;
    }
    _DataModelListForOneClick.insert(iAppName,objAppDM);
}

void GTAExportScxml::insertApplicationDataModel(const QString & iAppName, const QString & iMedia, const QString & iBusName,const QString &iActor, bool isInsertInSubscribe)
{
    GTAScxmlDataModel objAppDM;
    if (iMedia.startsWith(PARAM_TYPE_PIR,Qt::CaseInsensitive))
        objAppDM.IsPIR=true;
    if(_DataModelList.contains(iAppName))
    {
        objAppDM = _DataModelList.value(iAppName);
        QStringList mediaList = objAppDM.getMediaList();
        //this if condition is used for cases when PIR and ICD param have same application name
        //it is necessary to subscribe the ICD parameter
        if (!mediaList.contains(iMedia))
        {
            objAppDM.isGenerateSubscribe = true;
        }
        if(objAppDM.IsApplication)
        {
            objAppDM.insert(iMedia,iBusName,iActor);
        }
    }
    else
    {
        objAppDM.ID = iAppName;
        objAppDM.insert(iMedia,iBusName,iActor);
        objAppDM.IsApplication = true;
        objAppDM.isGenerateSubscribe = isInsertInSubscribe;

        if(iMedia.startsWith(PARAM_TYPE_PIR,Qt::CaseInsensitive))
            objAppDM.IsPIR=true;
    }
    //if((!objAppDM.IsPIR) || (!_IsPirParamManagement) )// IKM: Under Use
    _DataModelList.insert(iAppName,objAppDM);
}
void GTAExportScxml::updateSubscribeState(QDomDocument &iDomDoc, QDomElement &iSubscribeElement, QDomElement &iDataModeElement,QDomElement &iUnSubscribeElement,const QHash<QString,GTAScxmlDataModel> & iDMList )
{
    if(! iSubscribeElement.isNull() && (! iDataModeElement.isNull()))
    {
        QStringList listOfAppToLog;
        QDomElement firstInvokeElem;
        QDomElement nextSateTransitElem;
        QDomElement finalStatTransitElem;
        QDomNodeList lstOfChild = iSubscribeElement.childNodes();
        for(int i = 0; i< lstOfChild.count(); i++)
        {
            QDomElement childElem = lstOfChild.item(i).toElement();
            if(! childElem.isNull())
            {
                if(childElem.nodeName() == SXN_INVOKE)
                {
                    firstInvokeElem = childElem;
                }
                else if(childElem.nodeName() == SXN_TRANSITION)
                {
                    QString targetStat = childElem.attribute(SXA_TARGET);
                    if(targetStat == "Init")
                    {
                        nextSateTransitElem = childElem;
                    }
                    else if(targetStat == "final")
                    {
                        finalStatTransitElem = childElem;
                    }

                }
            }
        }

        QString nextStatExpr;
        QString finalStatExpr;
        int appCounter=0;
        QStringList dmKeyList = iDMList.keys();
        foreach(QString dmKey, dmKeyList)
        {
            GTAScxmlDataModel objDM = iDMList.value(dmKey);
            if(objDM.isGenerateSubscribe && objDM.IsApplication && (/*(objDM.IsPIR==false) || */(!_IsPirParamManagement))) // IKM: Under Use
            {
                appCounter++;

                QString appName = objDM.ID;
                appName = appName.replace("-","_");
                QString resultVar = QString("Result_%1").arg(appName);
                QString returnVar = QString("Return_%1").arg(appName);

                listOfAppToLog.append(returnVar);

                QDomElement invokeElem;
                QString nextInvokeID = QString(getNextStateNumber()).replace("s","i");
                if(appCounter == 1)
                {
                    invokeElem = firstInvokeElem;
                }
                else
                {
                    invokeElem = firstInvokeElem.cloneNode().toElement();
                    iSubscribeElement.appendChild(invokeElem);
                }
                invokeElem.setAttribute(SXA_ID,nextInvokeID);
                QDomNodeList invokeChildrenElem  = invokeElem.childNodes();
                for(int x = 0; x < invokeChildrenElem.count(); x++)
                {
                    QDomElement invokeChildElem = invokeChildrenElem.item(x).toElement();
                    if(! invokeChildElem.isNull())
                    {
                        if(invokeChildElem.nodeName() == SXN_CONTENT)
                        {
                            QDomNodeList functionNodeList = invokeChildElem.elementsByTagName(SXN_FUNCTION);
                            if(!functionNodeList.isEmpty())
                            {
                                QDomElement functionElem = functionNodeList.item(0).toElement();
                                if(! functionElem.isNull())
                                {
                                    QDomNodeList functinChildNodeList = functionElem.childNodes();
                                    for(int f = 0; f < functinChildNodeList.count(); f++)
                                    {
                                        QDomElement funcChildElem = functinChildNodeList.item(f).toElement();
                                        if(funcChildElem.nodeName() == SXN_ARGUMENT)
                                        {
                                            funcChildElem.setAttribute(SXA_VALUE,appName);
                                        }
                                        else if(funcChildElem.nodeName() == SXN_RETURNVALUE)
                                        {
                                            funcChildElem.setAttribute(SXA_NAME,resultVar);
                                        }
                                    }
                                }
                            }
                        }
                        else if(invokeChildElem.nodeName() == SXN_FINALIZE)
                        {
                            QDomNodeList finalizeChildren = invokeChildElem.childNodes();
                            for(int y =0 ; y < finalizeChildren.count(); y++)
                            {
                                QDomElement finalChildElem = finalizeChildren.item(y).toElement();
                                if(! finalChildElem.isNull())
                                {
                                    QString nodeName = finalChildElem.nodeName();
                                    if(nodeName == SXN_ASSIGN)
                                    {
                                        finalChildElem.setAttribute(SXA_LOCATION,returnVar);
                                        finalChildElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(resultVar));

                                        //insert the return value in data model
                                        QDomElement dataElem =  iDomDoc.createElement(SXN_DATA);
                                        dataElem.setAttribute(SXA_ID,returnVar);
                                        iDataModeElement.appendChild(dataElem);

                                        if(nextStatExpr.isEmpty())
                                        {
                                            nextStatExpr =  returnVar + ".Status == 'OK'";
                                            finalStatExpr = returnVar + ".Status != 'OK'";
                                        }
                                        else
                                        {
                                            nextStatExpr +=  QString("&&") + returnVar + ".Status == 'OK'";
                                            finalStatExpr += QString("||") + returnVar + ".Status != 'OK'";
                                        }

                                    }
                                    else if(nodeName == SXN_LOG)
                                    {
                                        finalChildElem.setAttribute(SXA_EXPR,QString("'the following Signals were not subscribed: '+ %1").arg(returnVar));
                                    }
                                }
                            }
                        }
                    }
                }



            }
        }
        if(appCounter ==0 )
        {
            iSubscribeElement.removeChild(firstInvokeElem);
            iSubscribeElement.removeChild(finalStatTransitElem);
            nextSateTransitElem.removeAttribute(SXA_CONDITION);
            nextSateTransitElem.removeAttribute(SXA_EVENT);

            QDomNodeList invokeNodes = iUnSubscribeElement.elementsByTagName(SXN_INVOKE);
            if (!invokeNodes.isEmpty())
            {
                iUnSubscribeElement.removeChild(invokeNodes.at(0));
                QDomNodeList transNodeList = iUnSubscribeElement.childNodes();
                if (!transNodeList.isEmpty())
                {
                    if (transNodeList.size()==2)
                    {
                        for (int i=0;i<transNodeList.size();i++)
                        {
                            QDomNode currentNode = transNodeList.at(i);
                            currentNode.attributes().removeNamedItem(SXA_CONDITION);
                            currentNode.attributes().removeNamedItem(SXA_EVENT);
                        }
                        iUnSubscribeElement.removeChild(transNodeList.at(1));
                    }

                }
            }
            
        }
        else
        {
            nextSateTransitElem.setAttribute(SXA_CONDITION,nextStatExpr);
            finalStatTransitElem.setAttribute(SXA_CONDITION,finalStatExpr);

            //Logging in transition
            GTAScxmlLogMessage objLog;
            objLog.LOD = GTAScxmlLogMessage::Main;
            objLog.Identifier = "Subscribe";
            objLog.CurrentValue = "OK";
            objLog.ExpectedValue = "OK";
            objLog.Result = GTAScxmlLogMessage::OK;
            objLog.Status = GTAScxmlLogMessage::Success;
            QString logMsg1 = objLog.getLogMessage();

            logMsg1 = logMsg1.remove("+");
            logMsg1 = logMsg1.remove("'");
            logMsg1 = QString("'%1'").arg(logMsg1);
            QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
            ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
            ElemOnTrueLog.setAttribute(SXA_EXPR,logMsg1);
            nextSateTransitElem.appendChild(ElemOnTrueLog);

            objLog.ExpectedValue = "OK";
            objLog.CurrentValue = "KO";
            objLog.Result = GTAScxmlLogMessage::KO;
            objLog.Status = GTAScxmlLogMessage::Fail;
            QDomElement ElemOnFalseLog = ElemOnTrueLog.cloneNode().toElement();
            logMsg1 = objLog.getLogMessage();
            logMsg1 = logMsg1.remove("+");
            logMsg1 = logMsg1.remove("'");
            logMsg1 = QString("'%1'").arg(logMsg1);
            ElemOnFalseLog.setAttribute(SXA_EXPR,logMsg1);
            finalStatTransitElem.appendChild(ElemOnFalseLog);

            QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);


            QString subId = iSubscribeElement.attribute(SXA_ID);
            QDomElement timeoutSend = iDomDoc.createElement(SXN_SEND);
            QString eventName = QString("%1_timeoutEvent").arg(subId);
            timeoutSend.setAttribute(SXA_EVENT,eventName);
            // to be changed based on configs

            TestConfig* testConf = TestConfig::getInstance();
            bool isSubscribeUnsubscribeTimeout = testConf->getSubscribeUnsubscribeTimeoutStatus();
            QString SubscribeUnsubscribeTimeout = QString::fromStdString(testConf->getSubscribeUnsubscribeTimeoutValue());
            bool isDoubleOk = false;
            double dTimeout = SubscribeUnsubscribeTimeout.toDouble(&isDoubleOk);
            if (isSubscribeUnsubscribeTimeout)
                timeoutSend.setAttribute(SXA_DELAY,QString("%1ms").arg(QString::number(dTimeout * 1000)));
            else
                timeoutSend.setAttribute(SXA_DELAY,"1200000ms");

            QDomElement txNode3 = iDomDoc.createElement(SXN_TRANSITION);
            txNode3.setAttribute(SXA_TARGET,"final");
            txNode3.setAttribute(SXA_STATUS,"Timeout");
            txNode3.setAttribute(SXA_EVENT,eventName);


            //Log Node
            GTAScxmlLogMessage objWaitLog2;
            objWaitLog2.LOD = GTAScxmlLogMessage::Main;
            objWaitLog2.Identifier = "subscribe";
            objWaitLog2.ExpectedValue = "";
            objWaitLog2.CurrentValue = "";
            objWaitLog2.Result = GTAScxmlLogMessage::KO;
            objWaitLog2.Status = GTAScxmlLogMessage::TimeOut;
            QString logMsg3 = objWaitLog2.getLogMessage();

            QDomElement ElemOnLog2 = iDomDoc.createElement(SXN_LOG);
            ElemOnLog2.setAttribute(SXA_LABEL,"'info'");
            ElemOnLog2.setAttribute(SXA_EXPR,logMsg3);
            txNode3.appendChild(ElemOnLog2);

            QDomElement onEntry = iDomDoc.createElement(SXN_ONENTRY);
            onEntry.appendChild(timeoutSend);

            iSubscribeElement.appendChild(onEntry);
            iSubscribeElement.appendChild(txNode3);


            iSubscribeElement.appendChild(ElemOnExit);
            insertVariableOnExit(ElemOnExit,listOfAppToLog,"Subscribe","Subscribe",iDomDoc);
        }
    }
}
bool GTAExportScxml::insertValidAppDataModel(QString & iParameter,QString& oTypeOfParam, bool isGenerateSubscribe )
{
    bool rc= false;
    oTypeOfParam=PARAM_TYPE_LOCAL;
    if(! iParameter.isEmpty())
    {

        QStringList lstOfParamNames = QStringList(_ParamList.keys());

        rc = lstOfParamNames.contains(iParameter);
        if(rc)
        {
            QRegExp rgExp(iParameter);rgExp.setCaseSensitivity(Qt::CaseInsensitive);
            int index = lstOfParamNames.indexOf(rgExp);
            QString paramName = lstOfParamNames.at(index);
            GTAICDParameter objParam = getParamObj(paramName);
            iParameter= objParam.getName();
            QString sourceType = objParam.getSourceType();
            //
            
            if(objParam.getMedia() != "VMAC")
                insertApplicationDataModel(objParam.getApplicationName(),objParam.getMedia(),objParam.getBusName(),"",isGenerateSubscribe)	;
            else
                insertApplicationDataModel(objParam.getApplicationName(),objParam.getMedia(),objParam.getBusName(),objParam.getEquipmentName(),isGenerateSubscribe)	;


            GTAScxmlDataModel objDM;
            QString paramType = objParam.getValueType();
            QString suffix = objParam.getSuffix();
            QString media = objParam.getMedia();
            objDM.ID = iParameter;
            if (media.startsWith(PARAM_TYPE_PIR,Qt::CaseInsensitive))
                objDM.IsPIR=true;
            if(media == "VCOM")
                objDM.Expr=QString("{'ParameterType':'%1'}").arg(paramType);
            else
                objDM.Expr=QString("{'ParameterType':'%1'}").arg(paramType);



            oTypeOfParam = sourceType;
            if (objParam.getMedia() != "PIR")
                _DataModelList.insert(iParameter,objDM);
            
        }
    }
    return rc;
}
bool  GTAExportScxml::setInvokeFunction(QDomElement & iInvokeElem, const QString & iInvokeID, const QString & iArgumentValue, const QString& iReturnVal)
{
    bool rc = false;
    if (!iInvokeElem.isNull())
    {
        iInvokeElem.setAttribute(SXA_ID,iInvokeID);
        QDomNodeList invokeNodeChild = iInvokeElem.childNodes();
        int invokeNodeChildCount = invokeNodeChild.count();

        for (int k=0;k<invokeNodeChildCount;k++)
        {
            QDomNode invokeChildDom = invokeNodeChild.item(k);
            QString invokeChildName = invokeChildDom.nodeName();
            if (invokeChildName==SXN_CONTENT)
            {
                QDomElement contentElement = invokeChildDom.toElement();
                QDomNodeList contentChildNodeLst = contentElement.childNodes();
                for (int l=0;l<contentChildNodeLst.size();l++)
                {
                    QDomNode contentChildNode = contentChildNodeLst.item(l);
                    QString contentChildName = contentChildNode.nodeName();
                    if (contentChildName==SXN_XMLRPC)
                    {
                        QDomElement xmlRpcElement = contentChildNode.toElement();
                        QDomNodeList xmlRpcChildNodes = xmlRpcElement.childNodes();
                        for (int m =0;m<xmlRpcChildNodes.size();m++)
                        {
                            QDomNode xmlChildNode = xmlRpcChildNodes.item(m);
                            QString xmlChildNodeName = xmlChildNode.nodeName();
                            if (xmlChildNodeName == SXN_FUNCTION)
                            {
                                QDomElement functionElement = xmlChildNode.toElement();
                                QDomNodeList functionChildNodes = functionElement.elementsByTagName(SXN_ARGUMENT);
                                if (functionChildNodes.size())
                                {
                                    if (!iArgumentValue.isEmpty())
                                        functionChildNodes.item(0).toElement().setAttribute(SXA_VALUE,QString("'%1'").arg(iArgumentValue));
                                    rc=true;
                                }

                                QDomNodeList returnValueChildNodes = functionElement.elementsByTagName(SXN_RETURNVALUE);
                                if (returnValueChildNodes.size())
                                {
                                    if (!iReturnVal.isEmpty())
                                        returnValueChildNodes.item(0).toElement().setAttribute(SXA_NAME,QString(SX_RET_PARAM_NAME));
                                    rc=true;
                                }

                            }

                        }

                    }
                }
            }
        }

    }

    return rc;
}
bool GTAExportScxml::setInvokeFinalize(QDomElement & iInvokeElem, const QString & iAssignLocation, const QString & iAssignExpr, const QString &,const QString &)
{
    bool rc = false;
    if (!iInvokeElem.isNull())
    {
        QDomNodeList invokeNodeChild = iInvokeElem.childNodes();
        int invokeNodeChildCount = invokeNodeChild.count();

        for (int k=0;k<invokeNodeChildCount;k++)
        {
            QDomNode invokeChildDom = invokeNodeChild.item(k);
            QString invokeChildName = invokeChildDom.nodeName();
            if ( invokeChildName == SXN_FINALIZE)

            {
                QDomElement finaliseElement = invokeChildDom.toElement();
                QDomNodeList assignChildNodes = finaliseElement.elementsByTagName(SXN_ASSIGN);
                QDomNodeList logChildNodes = finaliseElement.elementsByTagName(SXN_LOG);

                if (assignChildNodes.size())
                {
                    if (!iAssignLocation.isEmpty())
                        assignChildNodes.item(0).toElement().setAttribute(SXA_LOCATION,iAssignLocation);
                    if (!iAssignExpr.isEmpty())
                        assignChildNodes.item(0).toElement().setAttribute(SXA_EXPR,iAssignExpr);
                    rc=true;

                }
                /*if (logChildNodes.size())
                {
                    if (!iLogLabel.isEmpty())
                        logChildNodes.item(0).toElement().setAttribute(SXA_LABEL,QString("'%1 is:' + %1").arg(iLogLabel));
                    if (!iLogExpr.isEmpty())
                        logChildNodes.item(0).toElement().setAttribute(SXA_EXPR,QString("'%1 is:' + %1").arg(iLogExpr));
                    rc=true;
                }*/



            }



        }

    }

    return rc;
}


bool GTAExportScxml::setOnEntryAssign(QDomElement & iOnEntryElem, const QString & iLocation, const QString & iExpression)
{
    bool rc = false;

    QDomNodeList onEntryChildNodes = iOnEntryElem.childNodes();;
    if (!onEntryChildNodes.isEmpty())
    {
        QDomElement onEntryElement= onEntryChildNodes.item(0).toElement();
        if (!onEntryElement.isNull())
        {

            if (!iLocation.isEmpty())
                onEntryElement.setAttribute(SXA_LOCATION,iLocation);
            if (!iExpression.isEmpty())
                onEntryElement.setAttribute(SXA_EXPR,iExpression);
            rc = true;
        }

    }
    return rc;
}
bool GTAExportScxml::setOnEntry( QDomElement& iOnEntryElem, 
                                    const QString& isAssignLocation, const QString& isAssignExpression,
                                    const QString& isLogLabel, const QString& isLogExpression,
                                    const QString& isSendEvent, const QString& isSendDelay)
{
    bool rc = false;

    QDomNodeList onEntryChildNodes = iOnEntryElem.childNodes();

    if (!onEntryChildNodes.isEmpty())
    {

        for (int i=0;i<onEntryChildNodes.size();i++)
        {
            QDomNode onEntryNode=onEntryChildNodes.item(i);
            if (false == onEntryNode.isNull())
            {
                QDomElement onEntryElement= onEntryNode.toElement();

                if (onEntryElement.nodeName() == SXN_ASSIGN)
                {
                    if (!isAssignLocation.isEmpty())
                        onEntryElement.setAttribute(SXA_LOCATION,isAssignLocation);
                    if (!isAssignExpression.isEmpty())
                        onEntryElement.setAttribute(SXA_EXPR,isAssignExpression);
                    rc = true;

                }else if (onEntryElement.nodeName() == SXN_LOG)
                {
                    if (!isLogLabel.isEmpty())
                        onEntryElement.setAttribute(SXA_LABEL,isLogLabel);
                    if (!isLogExpression.isEmpty())
                        onEntryElement.setAttribute(SXA_EXPR,isLogExpression);
                    rc = true;
                } else if (onEntryElement.nodeName() == SXN_SEND)
                {
                    if (!isSendEvent.isEmpty())
                        onEntryElement.setAttribute(SXA_EVENT,isSendEvent);
                    if (!isSendDelay.isEmpty())
                        onEntryElement.setAttribute(SXA_DELAY,isSendDelay);
                    rc = true;
                }



            }

        }


    }
    return rc;
}
bool GTAExportScxml::setOnExitAssign(QDomElement & iOnExitElem, const QString & iLocation, const QString & iExpression)
{
    bool rc = false;

    QDomNodeList onExitChildNodes = iOnExitElem.childNodes();
    if (!onExitChildNodes.isEmpty())
    {
        QDomElement onExitChildElem= onExitChildNodes.item(0).toElement();
        if (!onExitChildElem.isNull())
        {

            if (!iLocation.isEmpty())
                onExitChildElem.setAttribute(SXA_LOCATION,iLocation);
            if (!iExpression.isEmpty())
                onExitChildElem.setAttribute(SXA_EXPR,iExpression);
            rc = true;
        }

    }
    return rc;
}
bool GTAExportScxml::setTranstion(QDomElement& iTransitionElem, const QString& iTarget, const QString& iEvent, const QString& iCond)
{
    bool rc = false;
    if (!iTransitionElem.isNull())
    {
        if (!iTarget.isEmpty())
            iTransitionElem.setAttribute(SXA_TARGET,iTarget);
        if (!iEvent.isEmpty())
            iTransitionElem.setAttribute(SXA_EVENT,iEvent);
        if (!iCond.isEmpty())
            iTransitionElem.setAttribute(SXA_CONDITION,iCond);
        rc = true;
    }
    return rc;
}
void GTAExportScxml::getScxmlElementPathMap(const QDomElement & iRootElem,  const QString & iXPath, QMap<QString,QDomElement>& oPathMap)
{
    if (!iRootElem.isNull())
    {
        QDomNodeList childNodes = iRootElem.childNodes();
        oPathMap.insert(iXPath,iRootElem); 
        //dumpElemenToDebug(iRootElem,iPath);
        for(int i=0;i<childNodes.size();i++)
        {
            QDomElement childElem = childNodes.item(i).toElement();

            if (!childElem.isNull())
            {

                QString childElemName = childElem.nodeName();
                QString nextPath = QString("%1/%2").arg(iXPath,childElemName);
                getScxmlElementPathMap(childElem,nextPath,oPathMap);
            }
        }


    }
}

void GTAExportScxml::setStateAttr(QDomElement & iRootElem, const QString stateID)
{
    if(!iRootElem.isNull())
    {
        iRootElem.setAttribute(SXA_ID,stateID);
    }
}
bool GTAExportScxml::setOnEntryOrExit(QDomElement &iElemOnEntryOrExit,
                                         const GTASXAssign &iObjAssign, const GTASXLog &iObjLog,
                                         const GTASXSend &iObjSend)
{
    bool rc = false;
    if(!iElemOnEntryOrExit.isNull())
    {
        rc = true;
        QDomNodeList childNodes = iElemOnEntryOrExit.childNodes();
        for(int i = 0; i < childNodes.count(); i++)
        {
            QDomElement domElem = childNodes.item(i).toElement();
            if(! domElem.isNull())
            {
                QString nodeName = domElem.nodeName();
                if(nodeName == SXN_LOG)
                {
                    if(domElem.hasAttribute(SXA_LABEL) && ! iObjLog.Label.isEmpty())
                        domElem.setAttribute(SXA_LABEL,iObjLog.Label);
                    if(domElem.hasAttribute(SXA_EXPR )&& ! iObjLog.Expr.isEmpty())
                        domElem.setAttribute(SXA_EXPR,iObjLog.Expr);
                }
                else if(nodeName == SXN_ASSIGN)
                {
                    if(domElem.hasAttribute(SXA_LOCATION)&& ! iObjAssign.Location.isEmpty())
                        domElem.setAttribute(SXA_LOCATION,iObjAssign.Location);

                    if(domElem.hasAttribute(SXA_EXPR) && ! iObjAssign.Expr.isEmpty())
                        domElem.setAttribute(SXA_EXPR,iObjAssign.Expr);
                }
                else if(nodeName == SXN_SEND)
                {
                    if(domElem.hasAttribute(SXA_EVENT) && ! iObjSend.Event.isEmpty())
                        domElem.setAttribute(SXA_EVENT,iObjSend.Event);

                    if(domElem.hasAttribute(SXA_DELAY) && ! iObjSend.Delay.isEmpty())
                        domElem.setAttribute(SXA_DELAY,iObjSend.Delay);
                }
            }
        }
    }
    return rc;
}

bool GTAExportScxml::setTransitionAttr(QDomElement &iElemOnTransit, const GTASXTransition &iObjTransition)

{
    bool rc = false;
    if(! iElemOnTransit.isNull())
    {
        rc = true;
        if(! iObjTransition.Condition.isEmpty())
            iElemOnTransit.setAttribute(SXA_CONDITION,iObjTransition.Condition);
        if( ! iObjTransition.Event.isEmpty())
            iElemOnTransit.setAttribute(SXA_EVENT,iObjTransition.Event);
        if( ! iObjTransition.Target.isEmpty())
            iElemOnTransit.setAttribute(SXA_TARGET,iObjTransition.Target);
        if( ! iObjTransition.status.isEmpty())
            iElemOnTransit.setAttribute(SXA_STATUS,iObjTransition.status);}
    return rc;

}

/**
  * This function set the invoke node of state, arguments are set based on the name attribue of argument node match
  * iElemOnInvoke: Element of invoke which need to be updated by attributes
  * invokeId: invoke ID of the invoke node, it should be unique within the state
  * iArgList: list of argument, argument value is set where the argument name is matches
  * iObjRetVal: Atrributes of return value node of function node
  * iObjAssign: Atrributes of assign node in finalize node
  * iObjAssign: Atrributes of log node in finalize node
  * return true if successfully invoke node is set otherwise false
  */
bool GTAExportScxml::setInvokeAttr(QDomElement & iElemOnInvoke,const QString invokeId,const QList<GTASXFuncArgument> & iArgList,
                                      const GTASXFuncReturnVal & iObjRetVal,const GTASXAssign & iObjAssign,
                                      const GTASXLog &, bool appendBraces)
{
    bool rc = false;
    if(! iElemOnInvoke.isNull())
    {
        QString returnParamStructId= invokeId;
        returnParamStructId = returnParamStructId.replace(".","_");
        iElemOnInvoke.setAttribute(SXA_ID,invokeId);
        rc = true;
        QDomNodeList FuncNodeList = iElemOnInvoke.elementsByTagName(SXN_FUNCTION);
        if(! FuncNodeList.isEmpty())
        {
            QDomElement iElemOnFunction = FuncNodeList.item(0).toElement();
            if(! iElemOnFunction.isNull())
            {
                rc = true;
                QDomNodeList childNodes = iElemOnFunction.childNodes();
                for(int i = 0; i < childNodes.count(); i++)
                {
                    QDomElement domElem = childNodes.item(i).toElement();
                    if(! domElem.isNull())
                    {
                        QString nodeName = domElem.nodeName();
                        QString nameAttrVal;
                        if(domElem.hasAttribute(SXA_NAME))
                            nameAttrVal = domElem.attribute(SXA_NAME);

                        if(nodeName == SXN_ARGUMENT)
                        {
                            foreach(GTASXFuncArgument objArg, iArgList)
                            {
                                if(objArg.Name == nameAttrVal)
                                {
                                    QString paramVal = "";
                                    if(nameAttrVal == "ParameterList")
                                    {

                                        paramVal = QString("%1").arg(objArg.Value);
                                    }
                                    else
                                    {
                                        if(appendBraces)
                                            paramVal = QString("['%1']").arg(getActualParameter(objArg.Value));
                                        else
                                            paramVal = QString("'%1'").arg(getActualParameter(objArg.Value));
                                    }
                                    domElem.setAttribute(SXA_VALUE,paramVal);
                                    break;
                                }
                            }
                        }
                        else if(nodeName == SXN_RETURNVALUE)
                        {
                            if(domElem.hasAttribute(SXA_NAME) && ! iObjRetVal.Name.isEmpty())
                                domElem.setAttribute(SXA_NAME,QString("%1_%2").arg(SX_RET_PARAM_NAME,returnParamStructId));
                        }
                    }
                }
            }
        }

        QDomNodeList FinalizeNodeList = iElemOnInvoke.elementsByTagName(SXN_FINALIZE);

        if(! FinalizeNodeList.isEmpty())
        {
            QDomElement iElemOnFinalize = FinalizeNodeList.item(0).toElement();
            if(! iElemOnFinalize.isNull())
            {
                QDomNodeList childNodes = iElemOnFinalize.childNodes();
                for(int i = 0; i < childNodes.count(); i++)
                {
                    QDomElement domElem = childNodes.item(i).toElement();
                    if(! domElem.isNull())
                    {
                        QString nodeName = domElem.nodeName();
                        if(nodeName == SXN_ASSIGN)
                        {
                            if(domElem.hasAttribute(SXA_LOCATION) && ! iObjAssign.Location.isEmpty())
                                domElem.setAttribute(SXA_LOCATION,iObjAssign.Location);

                            if(domElem.hasAttribute(SXA_EXPR) && ! iObjAssign.Location.isEmpty())
                            {
                                QString expr = QString("_event.data.%1_%2").arg(SX_RET_PARAM_NAME,returnParamStructId);
                                domElem.setAttribute(SXA_EXPR,expr);
                            }

                        }
                        //else if(nodeName == SXN_LOG)
                        //{
                        //    if(domElem.hasAttribute(SXA_LABEL) && ! iObjLog.Label.isEmpty())
                        //    {
                        //        domElem.setAttribute(SXA_LABEL,iObjLog.Label);
                        //    }

                        //    if(domElem.hasAttribute(SXA_EXPR) && ! iObjLog.Expr.isEmpty())
                        //    {
                        //        domElem.setAttribute(SXA_EXPR,iObjLog.Expr);
                        //    }

                        //}
                    }
                }
            }
        }

    }
    return rc;
}

void GTAExportScxml::dumpQNodeToDebug(const QDomNode& iNode,const QString& iString)const
{
    qDebug()<<QString(">>>>>>>>>>>%1>>>>>>>>>>>>>>>>").arg(iString);
    QDomDocument domDbgDocument;
    QDomElement rootNode = domDbgDocument.createElement("NODEDEBUG");
    domDbgDocument.appendChild(rootNode);
    QDomNode importedDom = domDbgDocument.importNode(iNode,true);
    rootNode.appendChild(importedDom.toElement());
    qDebug()<<domDbgDocument.toString();
    qDebug()<<QString("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
void GTAExportScxml::dumpElemenToDebug(const QDomElement& iElement,const QString& iString)const
{
    qDebug()<<QString(">>>>>>>>>>>%1>>>>>>>>>>>>>>>>").arg(iString);
    QDomDocument domDbgDocument;
    QDomElement rootNode = domDbgDocument.createElement("ELEMENTDEBUG");
    rootNode.appendChild(iElement.cloneNode(true));
    domDbgDocument.appendChild(rootNode);
    qDebug()<<domDbgDocument.toString();
    qDebug()<<QString("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}
/**
  * This function create the scxml state for a Set action
  * pCmd: Set command
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iStateId: State id of the command;
  * iTargetId: The next ID where the stat should go after execution
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::createActionSetState(const GTAActionSetList *pCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    // GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
    if(pCmd != NULL)
    {
        QString actionName = pCmd->getAction();
        QString instanceName = pCmd->getInstanceName();
        QString timeOut = getTimeOutInMiliSecond((GTAActionSetList*)pCmd);
        QString actionOnFail = pCmd->getActionOnFail();

        QString tmplFile  = actionName.replace(" ","_") + ".scxml";
        QString invokeIdForParameter = QString(iStateId).replace('s','i');
        QString invokeIdForEquation = QString(iStateId).replace('s','i');

        QString returnVar= QString("Return_MultiSetList_%1").arg(iStateId);
        QString inputVar= QString("MultiSetList_%1").arg(iStateId);
        QStringList logStatements;

        QDomElement ElemOnStateForParam;
        rc = getCloneRootElement(tmplFile,iDomDoc,ElemOnStateForParam);
        QString sStateId = iStateId;

        //Required only if the RHS ie eqn part is an ICD parameter
        QDomElement ElemOnStateForEqn;
        rc= getCloneRootElement(tmplFile,iDomDoc,ElemOnStateForEqn);

        QString newStateId;

        if(rc && !ElemOnStateForParam.isNull() && !ElemOnStateForEqn.isNull())
        {
            QDomElement oTimeOutElem;
            QString TimeOutTarget = QString("%1_1").arg(sStateId);
            rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOut,TimeOutTarget);
            if (rc)
            {
                sStateId = TimeOutTarget;
                iRootElem.appendChild(oTimeOutElem);
            }
            bool  isFSNoChangeAvail = false;
            //If eqn is not an ICD state node for eqn will be removed, its appended so that order is followed
            iRootElem.appendChild(ElemOnStateForEqn);

            setStateAttr(ElemOnStateForEqn,sStateId);
            
            QMap<QString,QDomElement> mapStateChildXPathForEqn;
            getScxmlElementPathMap(ElemOnStateForEqn,ElemOnStateForEqn.nodeName(),mapStateChildXPathForEqn);
            //Element On Entry
            QDomElement ElemOnEntryForEqn = mapStateChildXPathForEqn.value(getOnEntryXPath());
            QDomElement ElemOnEntryAssignForEqn = mapStateChildXPathForEqn.value(getOnEntryAssignXPath());
            QDomElement ElemOnExitAssignForEqn = mapStateChildXPathForEqn.value(getOnExitXPath());
            ElemOnStateForEqn.removeChild(ElemOnEntryAssignForEqn);
            ElemOnStateForEqn.removeChild(ElemOnEntryForEqn);
            ElemOnStateForEqn.removeChild(ElemOnExitAssignForEqn);

            iRootElem.appendChild(ElemOnStateForParam);
            //newStateId = sStateId+"_1";
            //setStateIDName(newStateId,pCmd);
            //setStateAttr(ElemOnStateForParam,newStateId);

            QMap<QString,QDomElement> mapStateChildXPathForParam;
            getScxmlElementPathMap(ElemOnStateForParam,ElemOnStateForParam.nodeName(),mapStateChildXPathForParam);
            //Element On Entry
            QDomElement ElemOnEntryForParam = mapStateChildXPathForParam.value(getOnEntryXPath());
            QDomElement ElemOnEntryAssignForParam = mapStateChildXPathForParam.value(getOnEntryAssignXPath());
            QDomElement ElemOnInvokeForEqn = mapStateChildXPathForEqn.value(getStateInvokeXPath());

            //iRootElem.appendChild(stateS01);

            QStringList VarList;
            int multiLstCntForEqn=0;
            int multiLstCntForParam=0;
            QStringList VarListInEqns;
            bool isLocal = true;
            //through each set action
            int noOfActionCmds = pCmd->getSetActionCount();
            QDomNode stateNodeForFS;
            int invokeIdCntr = 0;
            for (int i=0; i < noOfActionCmds; i++)
            {
                GTAActionSet* pSetAction;
                pCmd->getSetAction(i,pSetAction);
                
                if (NULL!=pSetAction)
                {

                    GTAEquationBase* pEqn = pSetAction->getEquation();
                    bool isFSChkOnlyFixed = pSetAction->getIsSetOnlyFSFixed();
                    bool isFSChkOnlyVariable = pSetAction->getIsSetOnlyFSVariable();
                    QString constVal;
                    int isEqnICD=false;
                    int isEqnLocal = false;
                    bool isEqnAString = false;
                    bool isEqnANumber = false;
                    GTAEquationConst constEqn;
                    QString sParam = pSetAction->getParameter();
                    updateWithGenToolParam(sParam);
                    if (isFSChkOnlyFixed || isFSChkOnlyVariable)
                    {   
                        constEqn.setConstant(pSetAction->getParameter());
                        pEqn = &constEqn;
                    }

                    //for eqn
                    if(pEqn !=NULL && pEqn->getEquationType() == GTAEquationBase::CONST)
                    {
                        GTAEquationConst * pConstEq = dynamic_cast<GTAEquationConst *>(pEqn);
                        constVal = pConstEq->getConstant();
                        updateWithGenToolParam(constVal);
                        constVal.toDouble(&isEqnANumber);
                        isEqnAString = constVal.contains("\"") || constVal.contains("'");
                        if (isEqnAString)
                        {
                            constVal.replace("\"","'");
                            pConstEq->setConstant(constVal);
                        }

                        

                        if(isEqnAString ==false && isAParameter(sParam) )
                        {
                            isLocal = false;
                            //sParam=sParam.toUpper();
                            GTAICDParameter objParam = getParamObj(sParam);
                            if(objParam.getName() != "")
                                sParam = objParam.getName();
                            QString valType = objParam.getValueType();
                            if (valType.contains("String",Qt::CaseInsensitive))
                            {
                                isEqnANumber=true;
                                constVal=QString("'%1'").arg(constVal);
                                pConstEq->setConstant(constVal);
                            }
                        }
                        if(isEqnANumber || isEqnAString)
                        {
                            GTAEquationConst::ConstType numType = GTAEquationConst::NUM;
                            pConstEq->setConstType(numType);

                        }
                        else if(isAParameter(constVal))
                        {
                            pConstEq->setConstType(GTAEquationConst::ICD);
                            //constVal=constVal.toUpper();
                            //pConstEq->setConstant(constVal);
                            isEqnICD=true;

                        }

                        else
                        {
                            pConstEq->setConstType(GTAEquationConst::LOCAL);
                            isEqnLocal = true;
                        }
                    }


                    QString sFunctionStatus = pSetAction->getFunctionStatus();
                    bool isFSNoChange = (sFunctionStatus==ACT_FSSSM_NOCHANGE)?true:false;
                    QString sFunctionType,sEqnStatement;
                    if (NULL!=pEqn)
                    {
                        pEqn->getFunctionStatement(sFunctionType,sEqnStatement);
                    }


                    
                    QString varAssignExprForParameter = QString("%1").arg(constVal);
                    QString newAssignExprForParamForFS;
                    QString newAssignExprForParamForValue;




                    if(isEqnICD || isEqnLocal || isFSNoChange)
                    {

                        GTAScxmlDataModel objDM;
                        objDM.ID = constVal;
                        if(isEqnLocal)
                        {
                            _DataModelList.insert(objDM.ID,objDM);
                        }

                        GTAICDParameter objParam = getParamObj(constVal);
                        constVal =  objParam.getName();
                        VarList.append(constVal);
                        QString appName = objParam.getApplicationName();
                        QString busName = objParam.getBusName();
                        QString signalName= objParam.getSignalName();
                        QString media = objParam.getMedia();
                        QString actor = objParam.getEquipmentName();
                        QString sigType = objParam.getValueType();
                        QString suffix = objParam.getSuffix();
                        if((sFunctionType == "CONSTANTS") && (!sigType.isEmpty()))
                        {
                            if(sigType.toLower() == "opaque")
                                sFunctionType = "String";
                            else
                                sFunctionType = sigType;
                        }

                        /*QStringList nameLst = QStringList()<<appName <<busName <<signalName <<media <<sigType <<suffix;
                            nameLst.removeAll("");
                            varAssignExprForParameter = nameLst.join("__");*/



                        


                        VarListInEqns.append(constVal);
                        QDomElement txNodeElem;
                        if (isFSNoChange)
                        {
                            
                            if(stateNodeForFS.isNull())
                            {
                                stateNodeForFS= iDomDoc.createElement(SXN_STATE);
                                stateNodeForFS.toElement().setAttribute(SXA_ID,iStateId + "_1");
                                isFSNoChangeAvail  = true;
                            }

                            //to maintian order of invoke nodes
                            ElemOnInvokeForEqn = ElemOnStateForEqn.lastChildElement(SXN_INVOKE).toElement();
                            ElemOnInvokeForEqn.setAttribute(SXA_ID,QString("invoke_%1").arg(QString::number(invokeIdCntr++)));
                            QDomNodeList FuncNodeList = ElemOnInvokeForEqn.elementsByTagName(SXN_FUNCTION);
                            QDomNodeList AssignNodeList = ElemOnInvokeForEqn.elementsByTagName(SXN_ASSIGN);
                            QDomElement TxNodeElem = stateNodeForFS.namedItem(SXN_TRANSITION).toElement();
                            if(!FuncNodeList.isEmpty())
                            {
                                //<argument value="MultiSetList" type="array" name="ParameterList"/>
                                QDomElement functionElement= FuncNodeList.at(0).toElement();
                                functionElement.setAttribute(SXA_NAME, GET_MULTI_PARAM_VALUE_SINGLE_SAMPLE);
                                functionElement.elementsByTagName(SXN_ARGUMENT).at(0).toElement().setAttribute(SXA_NAME,"ParameterID");
                                functionElement.elementsByTagName(SXN_ARGUMENT).at(0).toElement().setAttribute(SXA_TYPE,"array");
                                functionElement.elementsByTagName(SXN_ARGUMENT).at(0).toElement().setAttribute(SXA_VALUE,QString("'%1'").arg(getActualParameter(sParam)));
                            }
                            if(!AssignNodeList.isEmpty())
                            {
                                QDomElement assignElem= AssignNodeList.at(0).toElement();
                                assignElem.setAttribute(SXA_LOCATION,sParam);
                            }

                            if(TxNodeElem.isNull())
                            {
                                txNodeElem = iDomDoc.createElement(SXN_TRANSITION);
                                txNodeElem.setAttribute(SXA_STATUS,"success");
                                txNodeElem.setAttribute(SXA_TARGET,iStateId+"_1_1");
                                txNodeElem.setAttribute(SXA_EVENT,"done.invoke._all");
                                stateNodeForFS.appendChild(txNodeElem);
                            }



                            QDomElement newElemOnInvokeForEqn = ElemOnInvokeForEqn.cloneNode(true).toElement();
                            
                            
                            //stateNodeForFS.appendChild(newElemOnInvokeForEqn);
                            
                            //iRootElem.insertBefore(stateNodeForFS,ElemOnStateForEqn);


                            /* GTASXFuncArgument objArg;
                            objArg.Name ="ParameterName";
                            objArg.Value = constVal;
                            QList<GTASXFuncArgument> argList;argList.append(objArg);
                            GTASXFuncReturnVal objReturn;
                            objReturn.Name = constVal;
                            GTASXAssign objAssignOnFinalize;
                            objAssignOnFinalize.Expr = objReturn.Name;
                            objAssignOnFinalize.Location = objReturn.Name;
                            GTASXLog objLogOnFinalize;
                            objLogOnFinalize.Label = "Info";
                            objLogOnFinalize.Expr = QString("'%1'").arg(instanceName);*/



                            //setInvokeAttr(ElemOnInvokeForEqn,invokeIdForEquation+QString("_%1").arg(QString::number(multiLstCntForEqn)),argList,objReturn,objAssignOnFinalize,objLogOnFinalize);

                        }
                        if (isEqnICD || isFSNoChange)
                        {
                            if(media != "VMAC")
                                insertApplicationDataModel(appName,media,busName,"");
                            else
                                insertApplicationDataModel(appName,media,busName,actor);


                            logStatements.append(pSetAction->getStatement());
                            


                            varAssignExprForParameter = varAssignExprForParameter +".Value";
                            if(media == "VCOM")
                                objDM.Expr = QString("{'ParameterType':'%1'}").arg(sigType);
                            else
                                objDM.Expr = QString("{'ParameterType':'%1'}").arg(sigType);


                            _DataModelList.insert(constVal,objDM);
                            //to maintian order of invoke nodes
                            ElemOnInvokeForEqn = ElemOnStateForEqn.lastChildElement(SXN_INVOKE).toElement();
                            QDomNodeList FuncNodeList = ElemOnInvokeForEqn.elementsByTagName(SXN_FUNCTION);
                            if(!FuncNodeList.isEmpty())
                            {
                                //<argument value="MultiSetList" type="array" name="ParameterList"/>
                                QDomElement functionElement= FuncNodeList.at(0).toElement();
                                functionElement.setAttribute(SXA_NAME, GET_MULTI_PARAM_VALUE_SINGLE_SAMPLE);
                                functionElement.elementsByTagName(SXN_ARGUMENT).at(0).toElement().setAttribute(SXA_NAME,"ParameterName");
                                functionElement.elementsByTagName(SXN_ARGUMENT).at(0).toElement().setAttribute(SXA_TYPE,"array");
                                //functionElement.elementsByTagName(SXN_ARGUMENT).at(0).toElement().removeAttribute(SXA_TYPE);
                            }

                            if(multiLstCntForEqn)
                            {
                                QDomElement newElemOnInvokeForEqn = ElemOnInvokeForEqn.cloneNode(true).toElement();
                                ElemOnStateForEqn.insertAfter(newElemOnInvokeForEqn,ElemOnInvokeForEqn);
                                ElemOnInvokeForEqn=newElemOnInvokeForEqn;
                            }
                            multiLstCntForEqn++;



                            GTASXFuncArgument objArg;
                            objArg.Name ="ParameterName";
                            objArg.Value = constVal;
                            QList<GTASXFuncArgument> argList;argList.append(objArg);
                            GTASXFuncReturnVal objReturn;
                            objReturn.Name = constVal;
                            GTASXAssign objAssignOnFinalize;

                            objAssignOnFinalize.Expr = objReturn.Name;
                            objAssignOnFinalize.Location = objReturn.Name;
                            GTASXLog objLogOnFinalize;
                            objLogOnFinalize.Label = "Info";
                            objLogOnFinalize.Expr = QString("'%1'").arg(instanceName);
                            setInvokeAttr(ElemOnInvokeForEqn,invokeIdForEquation+QString("_%1").arg(QString::number(multiLstCntForEqn)),argList,objReturn,objAssignOnFinalize,objLogOnFinalize);
                            QString txEvent1 = "";
                            setInvokeDelay(iDomDoc,ElemOnInvokeForEqn,pCmd->getAction(),"",txEvent1,1);//noOfActionCmds);

                            if(!txEvent1.isEmpty())
                                txNodeElem.setAttribute(SXA_EVENT,txEvent1);

                        }//if equation is an icd parameter
                        



                    }


                    bool isParamANumber = false;
                    sParam.toDouble(&isParamANumber);
                    bool isParameterAString = sParam.contains("\"") || sParam.contains("'");
                    if((isParamANumber == false) && (isParameterAString == false))
                    {

                        rc = QStringList(_ParamList.keys()).contains(sParam,Qt::CaseInsensitive) ;

                        if(rc)
                        {
                            isLocal = false;
                            GTAICDParameter objParam = getParamObj(sParam);
                            sParam = objParam.getName();
                            QString appName = objParam.getApplicationName();
                            QString busName = objParam.getBusName();
                            QString signalName= objParam.getSignalName();
                            QString media = objParam.getMedia();
                            QString actor = objParam.getEquipmentName();
                            QString sigType = objParam.getValueType();
                            QString suffix = objParam.getSuffix();
                            QString valueType = objParam.getParameterType();
                            if((sFunctionType == "CONSTANTS") && (!sigType.isEmpty()))
                            {
                                if(sigType.toLower() == "opaque")
                                    sFunctionType = "String";
                                else
                                    sFunctionType = sigType;
                            }
                            if(isFSNoChange)
                            {
                                varAssignExprForParameter = QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3',%4}").arg(getActualParameter(sParam),sFunctionType,"",sEqnStatement);
                            }
                            else
                            {
                                
                                varAssignExprForParameter = QString("{ 'Parameter': '%1', 'FunctionType':'%2' ,'Status':'%3',%4}").arg(getActualParameter(sParam),sFunctionType,sFunctionStatus,sEqnStatement);
                            }
                            if(media != "VMAC")
                                insertApplicationDataModel(appName,media,busName,"");
                            else 
                                insertApplicationDataModel(appName,media,busName,actor);
                            logStatements.append(pSetAction->getStatement());
                            GTAScxmlDataModel objDM;
                            objDM.ID = sParam;
                            if(media == "VCOM")
                                objDM.Expr = QString("{'ParameterType':'%1'}").arg(sigType);
                            else
                                objDM.Expr = QString("{'ParameterType':'%1'}").arg(sigType);
                            _DataModelList.insert(sParam,objDM);
                        }
                        else
                        {
                            GTAScxmlDataModel objDM;
                            objDM.ID = sParam;
                            _DataModelList.insert(sParam,objDM);
                        }

                        VarList.append(sParam);
                    }

                    QDomElement ElemOnAssign = ElemOnEntryAssignForParam.cloneNode().toElement() ;
                    

                    if((isParamANumber == false) && (isParameterAString == false))
                    {
                        if (QStringList(_ParamList.keys()).contains(sParam,Qt::CaseInsensitive))
                        {
                            sParam = getParamObj(sParam).getName();
                            ElemOnAssign.setAttribute(SXA_EXPR,varAssignExprForParameter);
                            ElemOnAssign.setAttribute(SXA_LOCATION,QString("%1[%2]").arg(inputVar,QString::number(multiLstCntForParam++)));
                            ElemOnEntryForParam.appendChild(ElemOnAssign);
                        }
                        else
                        {
                            ElemOnAssign.setAttribute(SXA_EXPR,varAssignExprForParameter);
                            ElemOnAssign.setAttribute(SXA_LOCATION,QString("%1").arg(sParam));
                        }
                    }
                    
                    if(isFSNoChange)
                    {
                        QDomElement newAssignForFS = ElemOnAssign.cloneNode().toElement();
                        newAssignExprForParamForFS = QString("%1.Status").arg(sParam);
                        newAssignForFS.setAttribute(SXA_EXPR,newAssignExprForParamForFS);
                        QString location = newAssignForFS.attribute(SXA_LOCATION);
                        QString newLocation =  QString("%1.Status").arg(location);
                        newAssignForFS.setAttribute(SXA_LOCATION,newLocation);
                        ElemOnEntryForParam.appendChild(newAssignForFS);
                    }
                    if(isEqnICD)
                    {
                        QString rhsVal = pSetAction->getValue();
                        QDomElement newAssignForVal = ElemOnAssign.cloneNode().toElement();
                        if (rhsVal.isEmpty())
                        {
                            if (isFSChkOnlyFixed)
                                newAssignExprForParamForValue = QString("%1.Value").arg(constEqn.getConstant());
                        }
                        else
                            newAssignExprForParamForValue = QString("%1.Value").arg(rhsVal);
                        newAssignForVal.setAttribute(SXA_EXPR,newAssignExprForParamForValue);
                        QString location = newAssignForVal.attribute(SXA_LOCATION);
                        if (QStringList(_ParamList.keys()).contains(location,Qt::CaseInsensitive) || (location.contains("MultiSetList")))
                        {
                            if(!location.contains("MultiSetList"))
                                location = getParamObj(location).getName();

                            QString newLocation =  QString("%1.FunctionValue.Value").arg(location);
                            newAssignForVal.setAttribute(SXA_LOCATION,newLocation);
                        }
                        else
                        {
                            QString newLocation =  QString("%1").arg(location);
                            newAssignForVal.setAttribute(SXA_LOCATION,newLocation);
                        }

                        
                        ElemOnEntryForParam.appendChild(newAssignForVal);
                    }
                    else if(isEqnLocal)
                    {
                        QString rhsVal = pSetAction->getValue();
                        QDomElement newAssignForVal = ElemOnAssign.cloneNode().toElement();
                        newAssignExprForParamForValue = QString("%1").arg(rhsVal);
                        newAssignForVal.setAttribute(SXA_EXPR,newAssignExprForParamForValue);
                        QString location = newAssignForVal.attribute(SXA_LOCATION);
                        if (QStringList(_ParamList.keys()).contains(location,Qt::CaseInsensitive)|| (location.contains("MultiSetList")))
                        {
                            if(!location.contains("MultiSetList"))
                                location = getParamObj(location).getName();


                            QString newLocation =  QString("%1.FunctionValue.Value").arg(location);
                            newAssignForVal.setAttribute(SXA_LOCATION,newLocation);
                            ElemOnEntryForParam.appendChild(newAssignForVal);
                        }
                        else
                        {
                            QString newLocation =  QString("%1").arg(location);
                            newAssignForVal.setAttribute(SXA_LOCATION,newLocation);
                            ElemOnEntryForParam.appendChild(newAssignForVal);
                        }

                    }
                    else  if(isEqnANumber || isEqnAString)
                    {
                        QString rhsVal = pSetAction->getValue();
                        QDomElement newAssignForVal = ElemOnAssign.cloneNode().toElement();
                        newAssignExprForParamForValue = QString("%1").arg(rhsVal);
                        newAssignForVal.setAttribute(SXA_EXPR,newAssignExprForParamForValue);
                        QString location = newAssignForVal.attribute(SXA_LOCATION);
                        if (QStringList(_ParamList.keys()).contains(location,Qt::CaseInsensitive) || (location.contains("MultiSetList")))
                        {
                            if(!location.contains("MultiSetList"))
                                location = getParamObj(location).getName();


                            QString newLocation =  QString("%1.FunctionValue.Value").arg(location);
                            newAssignForVal.setAttribute(SXA_LOCATION,newLocation);
                            ElemOnEntryForParam.appendChild(newAssignForVal);
                        }
                        else
                        {
                            QString newLocation =  QString("%1").arg(location);
                            newAssignForVal.setAttribute(SXA_LOCATION,newLocation);
                            ElemOnEntryForParam.appendChild(newAssignForVal);
                        }

                    }

                    updateWithGenToolParam(sParam);
                }
            }


            ElemOnEntryForParam.removeChild(ElemOnEntryAssignForParam);
            QDomElement ElemOnTransitForEqn;
            if(multiLstCntForEqn)
            {
                //Transition
                ElemOnTransitForEqn = mapStateChildXPathForEqn.value(getStateTransistionXPath());
                GTASXTransition objTransit;
                //objTransit.Condition = QString("%1.Status == 'OK'").arg(returnVar);
                objTransit.Event = QString("done.invoke._all").arg(invokeIdForParameter);
                objTransit.Target = newStateId;
                objTransit.status = "success";
                VarListInEqns.removeDuplicates();
                /*if(!VarListInEqns.isEmpty())
                    objTransit.Condition = QString ("(%1.Status==OK)==true").arg(VarListInEqns.join(".Status == OK &&"));*/
                setTransitionAttr(ElemOnTransitForEqn,objTransit);


                QDomElement ElemOnTransitFailForEqn = ElemOnTransitForEqn.cloneNode().toElement();
                GTASXTransition objTransitFail;
                /*if(!VarListInEqns.isEmpty())
                    objTransit.Condition = QString ("(%1.Status!=OK)==true").arg(VarListInEqns.join(".Status != OK &&"));*/

                objTransitFail.status = "fail";
                //objTransitFail.Condition = QString("%1.Status != 'OK'").arg(returnVar);
                objTransitFail.Event = QString("done.invoke._all").arg(invokeIdForParameter);
                objTransitFail.Target = actionOnFail.contains("stop") ? MAIN_FINAL_STATE : iTargetId;
                setTransitionAttr(ElemOnTransitFailForEqn,objTransitFail);
                //ElemOnStateForEqn.insertAfter(ElemOnTransitFailForEqn,ElemOnTransitForEqn);

                //on exit for eqn having ICDs 
                QDomElement ElemOnExitForEqn = mapStateChildXPathForEqn.value(getOnExitXPath());
                GTASXAssign objAssingOnExit(inputVar,"[]");
                setOnEntryOrExit(ElemOnExitForEqn,objAssingOnExit,GTASXLog(),GTASXSend());
                
                //Action on Fail
                GTAScxmlLogMessage objLog;
                objLog.LOD = GTAScxmlLogMessage::Main;
                objLog.Identifier    = pCmd->getInstanceName();
                objLog.ExpectedValue = "True";
                objLog.CurrentValue = "'TimeOut'";
                objLog.Status = GTAScxmlLogMessage::TimeOut;
                objLog.Result = GTAScxmlLogMessage::KO;

                QString logMsgTimeOut = objLog.getLogMessage();
                logMsgTimeOut = logMsgTimeOut.remove("+");
                logMsgTimeOut = logMsgTimeOut.remove("'");
                logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);
                QString delayEventName = QString("%1_%2").arg(sStateId,"DelayCheckCond");
                createActionOnFail(iDomDoc,ElemOnStateForEqn,iStateId,iTargetId,actionOnFail,timeOut,logMsgTimeOut,false,delayEventName);


            }
            else
            {
                iRootElem.removeChild(ElemOnStateForEqn);
                if(isFSNoChangeAvail)
                {
                    setStateIDName(sStateId+"_1",pCmd);
                    setStateAttr(ElemOnStateForParam,sStateId+"_1");
                    oTimeOutElem.namedItem(SXN_TRANSITION).toElement().setAttribute(SXA_TARGET,sStateId+"_1");
                    ElemOnTransitForEqn.setAttribute(SXA_TARGET,sStateId+"_1");
                    ElemOnTransitForEqn.removeAttribute(SXA_CONDITION);
                }
                else
                {
                    setStateIDName(sStateId,pCmd);
                    setStateAttr(ElemOnStateForParam,sStateId);
                    oTimeOutElem.namedItem(SXN_TRANSITION).toElement().setAttribute(SXA_TARGET,sStateId);
                    ElemOnTransitForEqn.setAttribute(SXA_TARGET,sStateId);
                    ElemOnTransitForEqn.removeAttribute(SXA_CONDITION);
                }
            }



            //Transition
            QDomElement ElemOnTransitForParam = mapStateChildXPathForParam.value(getStateTransistionXPath());
            GTASXTransition objTransit;
            if(!isLocal)
            {
                objTransit.Event = QString("done.invoke.%1").arg(invokeIdForParameter);
            }
            
            objTransit.Target = iTargetId;
            objTransit.status = "success";
            objTransit.Condition =  QString("%1.Status == 'OK'").arg(returnVar);
            setTransitionAttr(ElemOnTransitForParam,objTransit);



            //Log node
            GTAScxmlLogMessage objTrueLog;
            objTrueLog.LOD = GTAScxmlLogMessage::Main;
            objTrueLog.CurrentValue = "True";
            objTrueLog.ExpectedValue = "True";
            objTrueLog.Identifier = instanceName;
            objTrueLog.Result = GTAScxmlLogMessage::OK;
            objTrueLog.Status = GTAScxmlLogMessage::Success;
            QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
            ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");

            QString logMsg1 = objTrueLog.getLogMessage();
            logMsg1 = logMsg1.remove("+");
            logMsg1 = logMsg1.remove("'");
            logMsg1 = QString("'%1'").arg(logMsg1);
            
            ElemOnTrueLog.setAttribute(SXA_EXPR,logMsg1);
            ElemOnTransitForParam.appendChild(ElemOnTrueLog);


            QDomElement ElemOnExit = mapStateChildXPathForParam.value(getOnExitXPath());
            GTASXAssign objAssingOnExit(inputVar,"[]");
            setOnEntryOrExit(ElemOnExit,objAssingOnExit,GTASXLog(),GTASXSend());


            //Logging all Parameters
            VarList.removeDuplicates();
            insertVariableOnExit(ElemOnExit,VarList,sStateId,instanceName,iDomDoc);


            if (multiLstCntForParam)
            {
                QDomElement ElemOnInvokeForParam = mapStateChildXPathForParam.value(getStateInvokeXPath());
                GTASXFuncArgument objArg;
                objArg.Name ="ParameterList";
                objArg.Value = inputVar;
                QList<GTASXFuncArgument> argList;argList.append(objArg);
                GTASXFuncReturnVal objReturn;
                objReturn.Name = returnVar;
                GTASXAssign objAssignOnFinalize;
                objAssignOnFinalize.Expr = objReturn.Name;
                objAssignOnFinalize.Location = objReturn.Name;
                GTASXLog objLogOnFinalize;
                objLogOnFinalize.Label = "Info";
                objLogOnFinalize.Expr = QString("'%1'").arg(instanceName);

                setInvokeAttr(ElemOnInvokeForParam,invokeIdForParameter,argList,objReturn,objAssignOnFinalize,objLogOnFinalize);
                QString txEvent1 = "";
                setInvokeDelay(iDomDoc,ElemOnInvokeForParam,pCmd->getAction(),"",txEvent1,1);//noOfActionCmds);



                GTAScxmlDataModel objDMMultSet;
                objDMMultSet.ID = inputVar;
                objDMMultSet.Expr = "[]";
                GTAScxmlDataModel objDMStatus;
                objDMStatus.ID = returnVar;
                _DataModelList.insert(objDMStatus.ID,objDMStatus);
                _DataModelList.insert(objDMMultSet.ID,objDMMultSet);

                if(!txEvent1.isEmpty())
                    ElemOnTransitForParam.setAttribute(SXA_EVENT,txEvent1);

                objTransit.Condition = QString("%1.Status == 'OK'").arg(returnVar);


                QDomElement ElemOnTransitFailForParam = ElemOnTransitForParam.cloneNode().toElement();
                GTASXTransition objTransitFail;
                objTransitFail.Condition = QString("%1.Status != 'OK'").arg(returnVar);
                objTransitFail.status = "fail";

                if(txEvent1.isEmpty())
                    objTransitFail.Event = QString("done.invoke.%1").arg(invokeIdForParameter);
                else
                    objTransitFail.Event = txEvent1;

                objTransitFail.Target = actionOnFail.contains("stop") ? MAIN_FINAL_STATE : iTargetId;
                setTransitionAttr(ElemOnTransitFailForParam,objTransitFail);
                ElemOnStateForParam.insertAfter(ElemOnTransitFailForParam,ElemOnTransitForParam);

                //Log node
                GTAScxmlLogMessage objFalseLog;
                objFalseLog.LOD = GTAScxmlLogMessage::Main;
                objFalseLog.CurrentValue = "False";
                objFalseLog.ExpectedValue = "False";
                objFalseLog.Identifier = instanceName;
                objFalseLog.Result = GTAScxmlLogMessage::KO;
                objFalseLog.Status = GTAScxmlLogMessage::Fail;
                QDomElement ElemOnTrueLog = ElemOnTransitFailForParam.childNodes().at(0).toElement();
                ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");

                QString logMsg2 = objFalseLog.getLogMessage();
                logMsg2 = logMsg2.remove("+");
                logMsg2 = logMsg2.remove("'");
                logMsg2 = QString("'%1'").arg(logMsg2);


                ElemOnTrueLog.setAttribute(SXA_EXPR,logMsg2);

                //Action on Fail
                GTAScxmlLogMessage objLog;
                objLog.LOD = GTAScxmlLogMessage::Main;
                objLog.Identifier    = pCmd->getInstanceName();
                objLog.ExpectedValue = "True";
                objLog.CurrentValue = "'TimeOut'";
                objLog.Status = GTAScxmlLogMessage::TimeOut;
                objLog.Result = GTAScxmlLogMessage::KO;
                QString logMsgTimeOut = objLog.getLogMessage();
                logMsgTimeOut = logMsgTimeOut.remove("+");
                logMsgTimeOut = logMsgTimeOut.remove("'");
                logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);
                QString delayEventName = QString("%1_%2").arg(sStateId,"DelayCheckCond");


                createActionOnFail(iDomDoc,ElemOnStateForParam,iStateId,iTargetId,actionOnFail,timeOut,logMsgTimeOut,false,delayEventName);
            }
            else
            {
                QDomElement ElemOnInvoke = mapStateChildXPathForParam.value(getStateInvokeXPath());
                if(!ElemOnInvoke.isNull())
                {
                    ElemOnStateForParam.removeChild(ElemOnInvoke);
                    ElemOnTransitForParam.removeAttribute(SXA_CONDITION);
                }
                

                

            }

            GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
            if(multiLstCntForEqn)
            {
                newStateId = sStateId+"_1";
                setStateIDName(newStateId,pCmd); 
                setStateAttr(ElemOnStateForParam,newStateId);
                ElemOnTransitForEqn.setAttribute(SXA_TARGET,newStateId);


                if(pParent != NULL)
                {
                    createLoopTimeOutTranstions(ElemOnStateForParam,iDomDoc,newStateId,pParent);
                    createLoopTimeOutTranstions(ElemOnStateForEqn,iDomDoc,sStateId,pParent);
                }
            }
            else
            {
                if(pParent != NULL)
                { 
                    createLoopTimeOutTranstions(ElemOnStateForParam,iDomDoc,sStateId,pParent);
                }
            }
            createCallTimeOutTranstions(iDomDoc,ElemOnStateForParam,(GTACommandBase*)pCmd,actionOnFail.contains("stop") ? MAIN_FINAL_STATE : iTargetId);
        }
    }
    return rc;
}
/**
  * This function create the scxml state for print value command, it can be either parameter or message print
  * pGenFunctCmd : Command for which the state to be created
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iStateId: State id of the command;
  * iTargetId: The next ID where the stat should go after execution
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::createActionRoboArmState(const GTAActionRoboArm *pRoboArmCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pRoboArmCmd != NULL)
    {
        QString actionName = pRoboArmCmd->getAction();
        QString instanceName = pRoboArmCmd->getInstanceName();
        QByteArray functionCode = pRoboArmCmd->getFunctionCode();
        if(functionCode.isNull() || functionCode.isEmpty())
            return rc ;
        QDomDocument xmlrpcDomDoc;
        xmlrpcDomDoc.setContent(functionCode);
        processIRTXmlRpc(xmlrpcDomDoc);
        QDomElement xmlrpcRootElem = xmlrpcDomDoc.documentElement();
        QDomNodeList xmlRpcFuncList = xmlrpcRootElem.childNodes();
        int functionCount = xmlRpcFuncList.count();
        for(int i = 0; i < functionCount; i++)
        {
            QDomNode xmlRpcFunc = xmlRpcFuncList.item(i);

            QString endId = iStateId;
            QString startId = iStateId;
            if(i != 0 )
            {
                startId += QString("_%1").arg(QString::number(i));
            }
            endId += QString("_%1").arg(QString::number(i+1));
            if(i == functionCount - 1)
                endId = iTargetId;

            QString tmplFile  = actionName.replace(" ","_") + ".scxml";
            QString invokeId = QString(startId).replace('s','i');

            QString variableName = QString("Result_invoke_%1").arg(invokeId);
            QString retVarName = QString("Return_invoke_%1").arg(invokeId);
            QString flagVarName = QString("Flag_invoke_%1").arg(invokeId);
            QString irtFunctionName;
            QDomElement oRootElemOnTmpl;
            rc = getCloneRootElement(tmplFile,iDomDoc,oRootElemOnTmpl);
            if(rc && (!oRootElemOnTmpl.isNull()))
            {
                QDomElement oElem = iDomDoc.importNode(oRootElemOnTmpl,true).toElement();
                if(oElem.nodeName() == SXN_STATE)
                {
                    oElem.setAttribute(SXA_ID,startId);
                    setStateIDName(startId,pRoboArmCmd);
                    QDomNodeList statChildNodeList = oElem.childNodes();
                    int statNodeCount = statChildNodeList.count();
                    for(int s = 0 ; s <statNodeCount; s++)
                    {
                        QDomNode statChildNode = statChildNodeList.at(s);
                        if(statChildNode.isElement())
                        {
                            QDomElement domStatChildElem = statChildNode.toElement();
                            QString nodeName =domStatChildElem.nodeName();
                            if(nodeName == SXN_TRANSITION)
                            {
                                QString trueExpr = retVarName + ".Status == 'OK'";
                                QString falseExpr = retVarName + ".Status != 'OK'";

                                QString event = QString("done.invoke.%1").arg(invokeId);
                                domStatChildElem.setAttribute(SXA_EVENT,event);
                                domStatChildElem.setAttribute(SXA_TARGET,endId);
                                domStatChildElem.setAttribute(SXA_CONDITION,trueExpr);
                                domStatChildElem.setAttribute(SXA_STATUS,"success");
                                QDomElement ElemOnFalseTransit = domStatChildElem.cloneNode().toElement();
                                ElemOnFalseTransit.setAttribute(SXA_CONDITION,falseExpr);
                                ElemOnFalseTransit.setAttribute(SXA_STATUS,"fail");
                                oElem.insertAfter(ElemOnFalseTransit,domStatChildElem);

                                //Log Node
                                GTAScxmlLogMessage objLog;
                                objLog.LOD = GTAScxmlLogMessage::Main;
                                objLog.Identifier =instanceName;
                                objLog.ExpectedValue = retVarName;
                                objLog.CurrentValue = retVarName;
                                objLog.Status = GTAScxmlLogMessage::Success;
                                objLog.Result = GTAScxmlLogMessage::OK;

                                QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
                                ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
                                ElemOnTrueLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                                domStatChildElem.appendChild(ElemOnTrueLog);

                                objLog.Status = GTAScxmlLogMessage::Fail;
                                objLog.Result = GTAScxmlLogMessage::KO;

                                QDomElement ElemOnFalseLog = ElemOnTrueLog.cloneNode().toElement();
                                ElemOnFalseLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                                ElemOnFalseTransit.appendChild(ElemOnFalseLog);

                            }
                            else if(nodeName == SXN_INVOKE)
                            {
                                domStatChildElem.setAttribute(SXA_ID,invokeId);

                                QDomNodeList invokeChildList = domStatChildElem.childNodes();
                                int invokeChildCount = invokeChildList.count();
                                for(int j = 0; j <invokeChildCount; j++)
                                {
                                    QDomNode invokeChild = invokeChildList.at(j);
                                    if(invokeChild.isElement())
                                    {
                                        QDomElement invokeChildElem =  invokeChild.toElement();
                                        QString invokeChildName = invokeChildElem.nodeName();
                                        if(invokeChildName == SXN_CONTENT)
                                        {
                                            if(! xmlRpcFunc.isNull() && (!invokeChildElem.isNull()))
                                            {
                                                QDomElement xmlrpcElem;
                                                if(! iDomDoc.isNull())
                                                {
                                                    QString nodeName = xmlRpcFunc.nodeName();
                                                    nodeName = nodeName.remove("Step").toLower();
                                                    QDomNode importedNode = iDomDoc.importNode(xmlRpcFunc,true);
                                                    if(! importedNode.isNull() && importedNode.isElement())
                                                    {
                                                        xmlrpcElem = importedNode.toElement();
                                                        xmlrpcElem.setTagName(nodeName);
                                                        invokeChildElem.appendChild(xmlrpcElem);
                                                        QDomNodeList returnValList = xmlrpcElem.elementsByTagName("returnvalue");
                                                        if(! returnValList.isEmpty() && ! returnValList.item(0).isNull())
                                                        {
                                                            QDomElement returnValElem = returnValList.item(0).toElement();
                                                            returnValElem.setAttribute(SXA_NAME,variableName);
                                                        }

                                                        QDomNodeList funcNodeList = xmlrpcElem.elementsByTagName(SXN_FUNCTION);
                                                        if(! funcNodeList.isEmpty())
                                                        {
                                                            QDomElement irtFuncElem = funcNodeList.item(0).toElement();
                                                            if(!irtFuncElem.isNull())
                                                                irtFunctionName = irtFuncElem.attribute(SXA_NAME);
                                                        }

                                                    }
                                                }
                                            }
                                        }
                                        else if(invokeChildName == SXN_FINALIZE)
                                        {
                                            QDomElement assignResultElem =  iDomDoc.createElement(SXN_ASSIGN);
                                            assignResultElem.setAttribute(SXA_LOCATION,retVarName);
                                            assignResultElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(variableName));

                                            //                                            QDomElement assignFlagElem =  iDomDoc.createElement(SXN_ASSIGN);
                                            //                                            assignFlagElem.setAttribute(SXA_LOCATION,flagVarName);
                                            //                                            assignFlagElem.setAttribute(SXA_EXPR,QString("'%1'").arg(SX_FLAG_EXECUTED));

                                            invokeChildElem.appendChild(assignResultElem);
                                            //invokeChildElem.appendChild(assignFlagElem);

                                            QDomElement logElem = iDomDoc.createElement(SXN_LOG);
                                            logElem.setAttribute(SXA_EXPR,retVarName);
                                            GTAScxmlDataModel objDMReturn;
                                            objDMReturn.ID = retVarName;

                                            GTAScxmlDataModel objDMFlag;
                                            objDMFlag.ID = flagVarName;
                                            objDMFlag.Expr = QString("'%1'").arg(SX_FLAG_TBE);
                                            _DataModelList.insert(retVarName,objDMReturn);
                                            //                                            _DataModelList.insert(flagVarName,objDMFlag);

                                        }
                                    }
                                }

                            }
                        }
                    }

                    GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pRoboArmCmd->getParent());
                    if(pParent != NULL)
                    {

                        createLoopTimeOutTranstions(oElem,iDomDoc,startId,pParent);

                    }
                    createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pRoboArmCmd,endId);
                    iRootElem.appendChild(oElem);
                }

            }
        }

    }
    return rc;
}
bool GTAExportScxml::createGenericFunctionState(const GTAActionGenericFunction *pGenFunctCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pGenFunctCmd != NULL)
    {
        QString actionName = pGenFunctCmd->getAction();
        QString instanceName = pGenFunctCmd->getInstanceName();
        QString scxmlText = pGenFunctCmd->getSCXMLText();
        QString actionOnFail = pGenFunctCmd->getActionOnFail(); 
        QString timeOut = getTimeOutInMiliSecond((GTAActionGenericFunction*)pGenFunctCmd);
        QString conftimeInMiliSec = getConfTimeInMiliSecond((GTACommandBase*)pGenFunctCmd);
        QStringList dumpListParams = pGenFunctCmd->getDumpList();
        QStringList dumpListParamForinvoke = getValidParametersToInvoke(dumpListParams); 
        
        QString sStateId = iStateId;
        QDomDocument doc;
        doc.setContent(scxmlText);
        if (!doc.isNull())
        {

            QDomElement oTimeOutElem;
            QString TimeOutTarget = QString("%1_1").arg(sStateId);
            //rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOutInMiliSec,TimeOutTarget);
            rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOut,TimeOutTarget);
            if (rc)
            {
                sStateId = TimeOutTarget;
                iRootElem.appendChild(oTimeOutElem);
            }
            QDomElement ConfTimeStateElem, elementOnConfTimeState;
            QString configTimeStateID= QString("%1_Confirmation_time").arg(sStateId);
            QDomElement rootNode = doc.documentElement();
            QDomNodeList childNodes = rootNode.childNodes();
            QDomElement stateElem = iDomDoc.createElement(SXN_STATE);
            stateElem.setAttribute(SXA_ID,sStateId);
            setStateIDName(sStateId,pGenFunctCmd);
            for (int i=0;i<childNodes.size();i++)
            {
                QDomNode currentNode=childNodes.at(i);
                QDomNode nodeToImport = iDomDoc.importNode(currentNode,true);
                stateElem.appendChild(nodeToImport);

                if (nodeToImport.nodeName() == SXN_TRANSITION)
                {
                    QDomElement txElem = nodeToImport.toElement();
                    QDomNode stsNode = txElem.attributes().namedItem(SXA_STATUS);
                    QString statusVal = "";
                    if(!stsNode.isNull())
                    {
                        statusVal = stsNode.nodeValue();
                    }
                    if((statusVal.toLower() == "success") && (conftimeInMiliSec != "0ms"))
                    {
                        txElem.setAttribute(SXA_TARGET,configTimeStateID);
                    }
                    else
                    {
                        txElem.setAttribute(SXA_TARGET,iTargetId);
                    }

                    if(statusVal.toLower() == "success")
                    {
                        GTAScxmlLogMessage objLogOnExit;
                        objLogOnExit.LOD = GTAScxmlLogMessage::Main;
                        objLogOnExit.Identifier    = pGenFunctCmd->getInstanceName();
                        objLogOnExit.ExpectedValue = "";
                        objLogOnExit.CurrentValue =  "";
                        objLogOnExit.Status = GTAScxmlLogMessage::Success;
                        objLogOnExit.Result = GTAScxmlLogMessage::OK;

                        QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);

                        QString expression = objLogOnExit.getLogMessage();
                        expression= expression.remove("'");
                        expression= expression.remove("+");
                        expression = QString("'%1'").arg(expression);
                        ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                        ElemOnLog.setAttribute(SXA_EXPR,expression);
                        txElem.appendChild(ElemOnLog);
                    }
                    else if (statusVal.toLower() == "fail")
                    {
                        GTAScxmlLogMessage objLogOnExit;
                        objLogOnExit.LOD = GTAScxmlLogMessage::Main;
                        objLogOnExit.Identifier    = pGenFunctCmd->getInstanceName();
                        objLogOnExit.ExpectedValue = "";
                        objLogOnExit.CurrentValue =  "";
                        objLogOnExit.Status = GTAScxmlLogMessage::Fail;
                        objLogOnExit.Result = GTAScxmlLogMessage::KO;
                        QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                        QString expression = objLogOnExit.getLogMessage();
                        expression= expression.remove("'");
                        expression= expression.remove("+");
                        expression = QString("'%1'").arg(expression);
                        ElemOnLog.setAttribute(SXA_EXPR,expression);
                        ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                        txElem.appendChild(ElemOnLog);

                    }

                }
            }

            QDomElement stateNodeForInvoke;
            if(!dumpListParamForinvoke.isEmpty())
            {
                rc = getCloneRootElement("Generic_command.scxml",iDomDoc,stateNodeForInvoke);
                if (rc)
                {
                    QDomElement invokeNode = stateNodeForInvoke.elementsByTagName(SXN_INVOKE).at(0).toElement();
                    QDomElement invokeNodeForReference = iDomDoc.importNode(invokeNode,true).toElement();
                    stateElem.appendChild(invokeNodeForReference);
                    createInvokeNodes(iDomDoc,stateElem,invokeNodeForReference,dumpListParamForinvoke,iStateId+"_i","ParameterID","Info",instanceName,false,pGenFunctCmd->getAction(),pGenFunctCmd->getComplement());
                    stateElem.removeChild(invokeNodeForReference);

                }
            }



            //Action on Fail
            GTAScxmlLogMessage objLog;
            objLog.LOD = GTAScxmlLogMessage::Main;
            objLog.Identifier    = instanceName;
            objLog.ExpectedValue = "True";
            objLog.CurrentValue = "'TimeOut'";
            objLog.Status =GTAScxmlLogMessage::TimeOut;
            objLog.Result = GTAScxmlLogMessage::KO;

            QString logMsgTimeOut = objLog.getLogMessage();
            logMsgTimeOut = logMsgTimeOut.remove("+");
            logMsgTimeOut = logMsgTimeOut.remove("'");
            logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);

            QString delayEventName = QString("%1_%2").arg(iStateId,"DelayCheckCond");
            createActionOnFail(iDomDoc,stateElem,iStateId,iTargetId,actionOnFail,timeOut,logMsgTimeOut,false,delayEventName);

            //iRootElem.appendChild(stateElem);
            //rc = true;



            



            QString confTimeTarget = QString("%1_1").arg(sStateId);
            QString configTimeEventName; 
            bool bConfigTimeExists = getConfigurationTimeNode(iDomDoc,ConfTimeStateElem,configTimeStateID,conftimeInMiliSec,confTimeTarget,configTimeEventName );

            if (bConfigTimeExists)
            {
                elementOnConfTimeState = stateElem.cloneNode().toElement();
                QDomNodeList transitElementNodes = elementOnConfTimeState.elementsByTagName(SXN_TRANSITION);
                elementOnConfTimeState.setAttribute(SXA_ID,confTimeTarget);

                for (int i=0;i<transitElementNodes.size();i++)
                {
                    QDomElement transitElement = transitElementNodes.at(i).toElement();
                    QString status = transitElement.attribute(SXA_STATUS);

                    

                    

                    if (status == "success")
                    {
                        transitElement.setAttribute(SXA_TARGET,iTargetId);
                        transitElement.setAttribute(SXA_EVENT,configTimeEventName);
                        

                    }
                    else if (status == "fail")
                    {
                        transitElement.setAttribute(SXA_TARGET,sStateId);
                        transitElement.setAttribute(SXA_EVENT,configTimeEventName);


                    }

                    




                }

            }
            iRootElem.appendChild(stateElem);
            

            GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pGenFunctCmd->getParent());

            if (bConfigTimeExists)
            {
                sStateId = confTimeTarget;
                iRootElem.appendChild(ConfTimeStateElem);
                QDomNodeList transitElementNodes = stateElem.elementsByTagName(SXN_TRANSITION);              

                for (int i=0;i<transitElementNodes.size();i++)
                {
                    QDomElement transitElement = transitElementNodes.at(i).toElement();
                    QString status = transitElement.attribute(SXA_STATUS);
                    if (status.isEmpty())
                    {
                        transitElement.setAttribute(SXA_TARGET,configTimeStateID);                      

                        //time out in case if parent is a while or do while statement
                        if(pParent != NULL)
                        { 

                            createLoopTimeOutTranstions(elementOnConfTimeState,iDomDoc,configTimeStateID,pParent);
                        }
                        break;

                    }

                }
                
                iRootElem.appendChild(elementOnConfTimeState);
            }

            //time out in case if parent is a while or do while statement

            if(pParent != NULL)
            {

                createLoopTimeOutTranstions(stateElem,iDomDoc,sStateId,pParent);

            }
            createCallTimeOutTranstions(iDomDoc,stateElem,(GTACommandBase*)pGenFunctCmd,iTargetId);

        }
    }
    return rc;

}
bool GTAExportScxml::createActionPrintTimeState(const GTAActionPrintTime *pPrintCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pPrintCmd != NULL)
    {
        QString actionName = pPrintCmd->getAction();
        QString instanceName = pPrintCmd->getInstanceName();

        
        QString sComplement = pPrintCmd->getComplement();

        QString tmplFile  = "printTime.scxml";
        QString invokeId = QString(iStateId).replace('s','i');
        QString VarToLog;
        QDomElement stateElem;
        rc = getCloneRootElement(tmplFile,iDomDoc,stateElem);

        if(rc)
        {
            setStateIDName(iStateId,pPrintCmd);
            stateElem.setAttribute(SXA_ID,iStateId);
            QString sPrintType = (pPrintCmd->getTimeType() == GTAActionPrintTime::UTC)? ACT_PRINT_TIME_TYPE_NOW : ACT_PRINT_TIME_TYPE_NA;
            QString expressoin = "Date()";
            if (sPrintType.toUpper()==ACT_PRINT_TIME_TYPE_NOW)
                expressoin = "Date.now()";


            GTAScxmlLogMessage objLogOnExit;
            objLogOnExit.LOD = GTAScxmlLogMessage::Main;
            objLogOnExit.Identifier    = instanceName;
            objLogOnExit.ExpectedValue = "Time";
            objLogOnExit.CurrentValue =  expressoin;
            objLogOnExit.Status = GTAScxmlLogMessage::Success;
            objLogOnExit.Result = GTAScxmlLogMessage::OK;

            QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
            ElemOnLog.setAttribute(SXA_LABEL,"'info'");
            QString logMsg2 = objLogOnExit.getLogMessage();
            ElemOnLog.setAttribute(SXA_EXPR,QString("%1").arg(logMsg2));
            
            //QDomNodeList logNodes = stateElem.elementsByTagName(SXN_LOG);
            //if (logNodes.isEmpty()==false)
            //{
            //    QDomElement logElement = logNodes.at(0).toElement();
            //    logElement.setAttribute(SXA_EXPR,expressoin);

            //}
            QDomNodeList targetNodes = stateElem.elementsByTagName(SXN_TRANSITION);
            if (targetNodes.isEmpty()==false)
            {
                QDomElement targetElem = targetNodes.at(0).toElement();
                targetElem.setAttribute(SXA_TARGET,iTargetId);
                targetElem.appendChild(ElemOnLog);
                targetElem.removeAttribute(SXA_EVENT);
            }

            createCallTimeOutTranstions(iDomDoc,stateElem,(GTACommandBase*)pPrintCmd,iTargetId);
            iRootElem.appendChild(stateElem);
        }
        
    }
    return rc;
}
bool GTAExportScxml::createActionPrintTableState(const GTAActionPrintTable *pPrintCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pPrintCmd != NULL)
    {
        QString actionName = pPrintCmd->getAction();
        QString instanceName = pPrintCmd->getInstanceName();
        QStringList sParameterNames = pPrintCmd->getValues();
        QString sParameterName;
        if (!sParameterNames.isEmpty())
        {
            sParameterName=sParameterNames.at(0);
            updateWithGenToolParam(sParameterName);
        }
        QString sComplement = pPrintCmd->getComplement();

        QString tmplFile  = "print.scxml";
        QString invokeId = QString(iStateId).replace('s','i');
        QString VarToLog;
        QDomElement stateElem;
        bool hasInvokes = false;
        QStringList paramList;
        GTAScxmlLogMessage logForCallTimeout;
        rc = getCloneRootElement(tmplFile,iDomDoc,stateElem);
        //In case of print message only onexit node is reqd.
        //In case of print print value all the nodes in template is reqd
        //hence creating new element and append required nodes depending upon print.
        QString logMessage =  QString("'|%1| %2'").arg(iStateId,instanceName);
        bool keepInvokeElem =  false;
        setStateIDName(iStateId,pPrintCmd);
        stateElem.setAttribute(SXA_ID,iStateId);
        if (rc)
        {

            QMap<QString,QDomElement> stateChildMap;
            getScxmlElementPathMap(stateElem,stateElem.nodeName(),stateChildMap);
            QDomElement onExitElement = stateChildMap.value(getOnExitXPath());
            QDomElement invokeElement = stateChildMap.value(getStateInvokeXPath());
            QDomElement ElemOnTransit = stateChildMap.value(getStateTransistionXPath());
            QStringList paramListOnExit;


            for(int i=0;i<sParameterNames.size();i++)
            {

                QString sParameterName = sParameterNames.at(i);
                updateWithGenToolParam(sParameterName);
                bool isNumber = false;
                sParameterName.toDouble(&isNumber);
                if(isNumber == false)
                {
                    rc = insertValidAppDataModel(sParameterName);

                    if (i>0 && rc && keepInvokeElem)
                    {
                        QDomNode invokeNode = invokeElement.cloneNode();
                        invokeElement=invokeNode.toElement();
                        stateElem.appendChild(invokeElement);
                    }
                    if ((rc == true) && ! invokeElement.isNull())
                    {
                        //                        GTASXFuncArgument objArg;
                        //                        objArg.Name = "ParameterID";
                        //                        objArg.Value = QString("%1").arg(sParameterName);
                        //                        QList<GTASXFuncArgument> argList; argList.append(objArg);

                        //                        GTASXFuncReturnVal objRetVal;
                        //                        objRetVal.Name = sParameterName;
                        //                        GTASXAssign objAssign(sParameterName,sParameterName);
                        //                        GTASXLog objLog("Info",logMessage);
                        //                        setInvokeAttr(invokeElement,invokeId,argList,objRetVal,objAssign,objLog);
                        //                        keepInvokeElem = true;

                        hasInvokes = true;
                        paramList.append(sParameterName);



                        //                        //Logging all Parameters
                        //                        paramListOnExit<<sParameterName;

                    }
                    else
                    {
                        GTAScxmlDataModel objDM;
                        objDM.ID = sParameterName;
                        _DataModelList.insert(sParameterName,objDM);

                    }

                    GTAScxmlLogMessage objLogOnExit;
                    objLogOnExit.LOD = GTAScxmlLogMessage::Main;
                    objLogOnExit.Identifier    = instanceName;
                    objLogOnExit.ExpectedValue = sParameterName;
                    objLogOnExit.CurrentValue = sParameterName;//QString("'+%1+'").arg(sParameterName);
                    objLogOnExit.Status = GTAScxmlLogMessage::Success;
                    objLogOnExit.Result = GTAScxmlLogMessage::OK;

                    QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                    ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                    ElemOnLog.setAttribute(SXA_EXPR,objLogOnExit.getLogMessage());
                    ElemOnTransit.appendChild(ElemOnLog);
                    logForCallTimeout = objLogOnExit;

                    //Logging all Parameters
                    paramListOnExit<<sParameterName;
                }

            }
            QString hiddenWaitTimeTxEvent;
            createInvokeStateNodes(iDomDoc,stateElem,invokeElement,paramList,invokeId,"ParameterID","Info",pPrintCmd->getInstanceName(),false,pPrintCmd->getAction(),pPrintCmd->getComplement(),iStateId,hiddenWaitTimeTxEvent);



            //            QDomNodeList invokeList = stateElem.elementsByTagName(SXN_INVOKE);

            //            for(int i=0;i<invokeList.count();i++)
            //            {
            //                QDomElement invkElem = invokeList.at(i).toElement();
            //                setInvokeDelay(iDomDoc,invkElem,pPrintCmd->getAction(),pPrintCmd->getComplement());
            //            }


            //Logging all Parameters
            GTAScxmlLogMessage objLog;
            objLog.LOD = GTAScxmlLogMessage::Main;
            objLog.Identifier = instanceName.replace("\"", "");
            objLog.ExpectedValue = "OK";
            objLog.CurrentValue = "OK";
            objLog.Result = GTAScxmlLogMessage::OK;
            objLog.Status = GTAScxmlLogMessage::Success;
            QString logMsg = objLog.getLogMessage();
            logMsg = logMsg.replace("\'","");
            logMsg = logMsg.replace("+","");
            QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
            ElemOnLog.setAttribute(SXA_LABEL,"'info'");
            ElemOnLog.setAttribute(SXA_EXPR,QString("'%1'").arg(logMsg));
            ElemOnTransit.appendChild(ElemOnLog);


            QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
            stateElem.appendChild(ElemOnExit);
            insertVariableOnExit(ElemOnExit,paramListOnExit,iStateId,instanceName,iDomDoc);

            if(! keepInvokeElem )
            {
                stateElem.removeChild(invokeElement);
                ElemOnTransit.removeAttribute(SXA_EVENT);
            }

            GTASXTransition objTransit;
            objTransit.Target = iTargetId;
            objTransit.status="success";




            QString dealyEvetName = QString("%1_%2").arg(iStateId,"dealy_EventName");
            if(hasInvokes)
            {
                bool isPirConfigEnabled = TestConfig::getInstance()->getPIRInvokeForSCXML();
                if(!isPirConfigEnabled)
                {
                    if(!hiddenWaitTimeTxEvent.isEmpty())
                        objTransit.Event = hiddenWaitTimeTxEvent;
                    else
                        objTransit.Event = "done.invoke._all";
                }
                else
                {
                    QDomElement dummyState = iDomDoc.createElement(SXN_STATE);
                    dummyState.setAttribute(SXA_ID,QString("%1_dummyState").arg(iStateId));
                    dummyState.appendChild(ElemOnTransit);
                    stateElem.removeChild(ElemOnTransit);
                    stateElem.appendChild(dummyState);
                }
            }
            else
            {

                objTransit.Event = dealyEvetName;
                QDomNodeList onentryNode = stateElem.elementsByTagName(SXN_ONENTRY);

                QDomElement sendElem = iDomDoc.createElement(SXN_SEND);
                sendElem.setAttribute(SXA_EVENT,dealyEvetName);
                sendElem.setAttribute(SXA_DELAY,"100ms");

                QDomElement onEntryElem;
                if(onentryNode.isEmpty())
                {
                    onEntryElem = iDomDoc.createElement(SXN_ONENTRY);
                    stateElem.appendChild(onEntryElem);
                }
                else
                {
                    onEntryElem = onentryNode.at(0).toElement();
                }
                onEntryElem.appendChild(sendElem);
            }
            setTransitionAttr(ElemOnTransit,objTransit);
            GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pPrintCmd->getParent());
            if(pParent != NULL)
            {
                createLoopTimeOutTranstions(stateElem,iDomDoc,iStateId,pParent);
            }
            else
            {
                QDomElement entryElem = iDomDoc.createElement(SXN_ONENTRY);
                QDomElement sendElem = iDomDoc.createElement(SXN_SEND);
                sendElem.setAttribute(SXA_EVENT,QString("%1_dummytimeout").arg(iStateId));
                sendElem.setAttribute(SXA_DELAY,"3000ms");
                entryElem.appendChild(sendElem);
                stateElem.appendChild(entryElem);

                QDomElement timeoutTx = iDomDoc.createElement(SXN_TRANSITION);
                timeoutTx.setAttribute(SXA_TARGET,iTargetId);
                timeoutTx.setAttribute(SXA_STATUS,"fail");
                timeoutTx.setAttribute(SXA_EVENT,QString("%1_dummytimeout").arg(iStateId));

                GTAScxmlLogMessage objLogOnExit;
                objLogOnExit.LOD = GTAScxmlLogMessage::Main;
                objLogOnExit.Identifier    = instanceName;
                objLogOnExit.ExpectedValue = "";
                objLogOnExit.CurrentValue = ""; //QString("'+%1+'").arg(sParameterName);
                objLogOnExit.Status = GTAScxmlLogMessage::TimeOut;
                objLogOnExit.Result = GTAScxmlLogMessage::KO;

                QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                ElemOnLog.setAttribute(SXA_EXPR,objLogOnExit.getLogMessage());

                timeoutTx.appendChild(ElemOnLog);
                stateElem.appendChild(timeoutTx);

            }
            createCallTimeOutTranstions(iDomDoc,stateElem,(GTACommandBase*)pPrintCmd,iTargetId);
            iRootElem.appendChild(stateElem);


        }
    }
    return rc;
}
/**
  * This function create the scxml state for print value command, it can be either parameter or message print
  * pPrintCmd : Command for which the state to be created
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created state is inserted
  * iStateId: State id of the command;
  * iTargetId: The next ID where the stat should go after execution
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::createActionPrintState(const GTAActionPrint *pPrintCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pPrintCmd != NULL)
    {

        QString actionName = pPrintCmd->getAction();
        QString instanceName = pPrintCmd->getInstanceName();
        QStringList sParameterNames = pPrintCmd->getValues();
        QString sParameterName;
        bool hasInvokes = false;
        if (!sParameterNames.isEmpty())
            sParameterName=sParameterNames.at(0);
        QString sComplement = pPrintCmd->getComplement();

        QString hiddenWaitTimeTxEvent;
        QString tmplFile  = actionName + ".scxml";
        QString invokeId = QString(iStateId).replace('s','i');
        QString VarToLog;
        QDomElement stateElem;
        GTAScxmlLogMessage logForCallTimeout;

        //        //if(!useSeperateInvokesForPIR)
        //            tmplFile = "GetMultiParamValueSingleSample.scxml";

        rc = getCloneRootElement(tmplFile,iDomDoc,stateElem);





        //In case of print message only onexit node is reqd.
        //In case of print print value all the nodes in template is reqd
        //hence creating new element and append required nodes depending upon print.
        QString logMessage =  QString("'|%1| %2'").arg(iStateId,instanceName);
        bool keepInvokeElem =  false;
        setStateIDName(iStateId,pPrintCmd);
        stateElem.setAttribute(SXA_ID,iStateId);
        if (rc)
        {

            QMap<QString,QDomElement> stateChildMap;
            getScxmlElementPathMap(stateElem,stateElem.nodeName(),stateChildMap);
            QDomElement invokeElement = stateChildMap.value(getStateInvokeXPath());
            QDomElement ElemOnTransit = stateChildMap.value(getStateTransistionXPath());

            if (sComplement == COM_PRINT_PARAM)
            {
                QStringList paramListOnExit;
                QStringList paramList;
                for(int i=0;i<sParameterNames.size();i++)
                {

                    QString sParameterName = sParameterNames.at(i);
                    updateWithGenToolParam(sParameterName);
                    bool isNumber = false;
                    sParameterName.toDouble(&isNumber);
                    if(isNumber == false)
                    {
                        rc = insertValidAppDataModel(sParameterName);

                        if (i>0 && rc && keepInvokeElem)
                        {
                            QDomNode invokeNode = invokeElement.cloneNode();
                            invokeElement=invokeNode.toElement();
                            stateElem.appendChild(invokeElement);
                        }
                        if ((rc == true) && ! invokeElement.isNull())
                        {
                            /* GTASXFuncArgument objArg;
                            objArg.Name = "ParameterID";
                            objArg.Value = QString("%1").arg(sParameterName);
                            QList<GTASXFuncArgument> argList; argList.append(objArg);

                            GTASXFuncReturnVal objRetVal;
                            objRetVal.Name = sParameterName;
                            GTASXAssign objAssign(sParameterName,sParameterName);
                            GTASXLog objLog("Info",logMessage);
                            setInvokeAttr(invokeElement,invokeId,argList,objRetVal,objAssign,objLog);
                            keepInvokeElem = true;*/
                            hasInvokes = true;
                            paramList.append(sParameterName);
                        }
                        else
                        {
                            GTAScxmlDataModel objDM;
                            objDM.ID = sParameterName;
                            _DataModelList.insert(sParameterName,objDM);
                        }

                        
                        //Logging all Parameters
                        GTAScxmlLogMessage objLogOnExit;
                        objLogOnExit.LOD = GTAScxmlLogMessage::Main;
                        objLogOnExit.Identifier    = instanceName;
                        objLogOnExit.ExpectedValue = sParameterName;
                        objLogOnExit.CurrentValue = sParameterName;//QString("'+%1+'").arg(sParameterName);
                        objLogOnExit.Status = GTAScxmlLogMessage::Success;
                        objLogOnExit.Result = GTAScxmlLogMessage::OK;

                        QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                        ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                        ElemOnLog.setAttribute(SXA_EXPR,objLogOnExit.getLogMessage());
                        ElemOnTransit.appendChild(ElemOnLog);
                        logForCallTimeout = objLogOnExit;

                        //Logging all Parameters
                        paramListOnExit<<sParameterName;
                    }


                }
                //                if(useSeperateInvokesForPIR)
                //                {
                createInvokeStateNodes(iDomDoc,stateElem,invokeElement,paramList,invokeId,"ParameterID","Info",pPrintCmd->getInstanceName(),false,pPrintCmd->getAction(),pPrintCmd->getComplement(),iStateId,hiddenWaitTimeTxEvent);
                //                }
                //                else
                //                {
                //                    createMultiInvokeNode(iDomDoc,stateElem, invokeElement,paramList,invokeId,"ParameterID", "Info",pPrintCmd->getInstanceName(),false, pPrintCmd->getAction(),pPrintCmd->getComplement(),hiddenWaitTimeTxEvent);
                //                }

                QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
                stateElem.appendChild(ElemOnExit);
                insertVariableOnExit(ElemOnExit,paramListOnExit,iStateId,instanceName,iDomDoc);
            }
            else
            {
                //Logging Message
                sParameterName.replace("\n","<br></br>");
                sParameterName.replace("`","_");
                GTAScxmlLogMessage objLogOnExit;
                objLogOnExit.LOD = GTAScxmlLogMessage::Main;
                objLogOnExit.Identifier    = instanceName;
                objLogOnExit.ExpectedValue = "Message";
                objLogOnExit.CurrentValue =  sParameterName;
                objLogOnExit.Status = GTAScxmlLogMessage::Success;
                objLogOnExit.Result = GTAScxmlLogMessage::OK;

                QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                logForCallTimeout = objLogOnExit;
                QString logMsg2 = objLogOnExit.getLogMessage();
                logMsg2 = logMsg2.remove("+");
                logMsg2 = logMsg2.remove("'");
                ElemOnLog.setAttribute(SXA_EXPR,QString("'%1'").arg(logMsg2));
                ElemOnTransit.appendChild(ElemOnLog);

                //Logging all Parameters
                QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
                stateElem.appendChild(ElemOnExit);

                GTAScxmlLogMessage objLogOnExit1;
                objLogOnExit1.LOD = GTAScxmlLogMessage::Detail;
                objLogOnExit1.Identifier    = instanceName;
                objLogOnExit1.ExpectedValue = "Message";
                objLogOnExit1.CurrentValue = sParameterName;
                objLogOnExit1.Status = GTAScxmlLogMessage::Success;
                objLogOnExit1.Result = GTAScxmlLogMessage::kNa;

                QDomElement ElemOnLog1 = iDomDoc.createElement(SXN_LOG);
                ElemOnLog1.setAttribute(SXA_LABEL,"'info'");
                QString logMsg1 = objLogOnExit1.getLogMessage();
                logMsg1 = logMsg1.remove("+");
                logMsg1 = logMsg1.remove("'");
                ElemOnLog1.setAttribute(SXA_EXPR,QString("'%1'").arg(logMsg1));
                ElemOnExit.appendChild(ElemOnLog1);
            }
            


            //            GTASXAssign objAssign;
            //            GTASXLog objLog("Info",logMessage);
            //            GTASXSend objSend;
            //            setOnEntryOrExit(onExitElement,objAssign,objLog,objSend);



            if(! keepInvokeElem )
            {
                stateElem.removeChild(invokeElement);
                ElemOnTransit.removeAttribute(SXA_EVENT);
            }

            GTASXTransition objTransit;
            objTransit.Target = iTargetId;
            objTransit.status="success";
            QString dealyEvetName = QString("%1_%2").arg(iStateId,"dealy_EventName");
            if(hasInvokes)
            {
                bool isPirConfigEnabled = TestConfig::getInstance()->getPIRInvokeForSCXML();
                if (!isPirConfigEnabled)
                {
                    if(hiddenWaitTimeTxEvent.isEmpty())
                        objTransit.Event = "done.invoke._all";
                    else
                        objTransit.Event = hiddenWaitTimeTxEvent;
                }
                else
                {
                    QDomElement dummyState = iDomDoc.createElement(SXN_STATE);
                    dummyState.setAttribute(SXA_ID,QString("%1_dummyState").arg(iStateId));
                    dummyState.appendChild(ElemOnTransit);
                    stateElem.removeChild(ElemOnTransit);
                    stateElem.appendChild(dummyState);
                }
            }
            else
            {

                objTransit.Event = dealyEvetName;
                QDomNodeList onentryNode = stateElem.elementsByTagName(SXN_ONENTRY);

                QDomElement sendElem = iDomDoc.createElement(SXN_SEND);
                sendElem.setAttribute(SXA_EVENT,dealyEvetName);
                sendElem.setAttribute(SXA_DELAY,"100ms");

                QDomElement onEntryElem;
                if(onentryNode.isEmpty())
                {
                    onEntryElem = iDomDoc.createElement(SXN_ONENTRY);
                    stateElem.appendChild(onEntryElem);
                }
                else
                {
                    onEntryElem = onentryNode.at(0).toElement();
                }
                onEntryElem.appendChild(sendElem);
            }

            setTransitionAttr(ElemOnTransit,objTransit);

            GTAActionBase * pActionBase = dynamic_cast<GTAActionBase *>(pPrintCmd->getParent());
            if(pActionBase != NULL)
            {
                createLoopTimeOutTranstions(stateElem,iDomDoc,iStateId,pActionBase);
            }
            else
            {
                QDomElement entryElem = iDomDoc.createElement(SXN_ONENTRY);
                QDomElement sendElem = iDomDoc.createElement(SXN_SEND);
                sendElem.setAttribute(SXA_EVENT,QString("%1_dummytimeout").arg(iStateId));
                sendElem.setAttribute(SXA_DELAY,"3000ms");
                entryElem.appendChild(sendElem);
                stateElem.appendChild(entryElem);

                QDomElement timeoutTx = iDomDoc.createElement(SXN_TRANSITION);
                timeoutTx.setAttribute(SXA_TARGET,iTargetId);
                timeoutTx.setAttribute(SXA_STATUS,"fail");
                timeoutTx.setAttribute(SXA_EVENT,QString("%1_dummytimeout").arg(iStateId));

                GTAScxmlLogMessage objLogOnExit;
                objLogOnExit.LOD = GTAScxmlLogMessage::Main;
                objLogOnExit.Identifier    = instanceName;
                objLogOnExit.ExpectedValue = "";
                objLogOnExit.CurrentValue = "";//QString("'+%1+'").arg(sParameterName);
                objLogOnExit.Status = GTAScxmlLogMessage::TimeOut;
                objLogOnExit.Result = GTAScxmlLogMessage::KO;

                QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                ElemOnLog.setAttribute(SXA_EXPR,objLogOnExit.getLogMessage());

                timeoutTx.appendChild(ElemOnLog);
                stateElem.appendChild(timeoutTx);

            }

            createCallTimeOutTranstions(iDomDoc,stateElem,(GTACommandBase*)pPrintCmd,iTargetId,logForCallTimeout);


            iRootElem.appendChild(stateElem);

            

        }
    }

    return rc;
}
bool GTAExportScxml::createActionPrintFwcBufferState(const GTAActionPrint *pPrintCmd, QDomDocument &iDomDoc, QDomElement &, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pPrintCmd != NULL)
    {
        QString actionName = pPrintCmd->getAction();
        QString complementName = pPrintCmd->getComplement();
        QString instanceName = pPrintCmd->getInstanceName();
        QStringList sParameterNames = pPrintCmd->getValues();
        QString sParameterName;
        if (!sParameterNames.isEmpty())
            sParameterName = sParameterNames.at(0);

        QString msgType = QString();
        if(pPrintCmd->getPrintType() == GTAActionPrint::BITE_BUFFER)
        {
            msgType = "message";
            QString message =  pPrintCmd->getMessageType();
            if(message == "MCDU Display")
            {
                msgType = "MCDU_Message";
            }
        }

        bool isNumber = false;
        int waitForBuffer = sParameterName.toInt(&isNumber);
        if(isNumber == false)
            waitForBuffer = 0;

        QString templateFile  = actionName.replace(" ","_") + "_" + complementName.replace(" ","_") + ".scxml";
        QString invokeId = QString(iStateId).replace('s','i');

        QByteArray tmplContents;
        rc = openFile(templateFile,tmplContents);

        if(rc && ! tmplContents.isEmpty())
        {
            QString statID = iStateId + "_1";
            QString invID = invokeId + "_1";
            QString returnVal = QString("Return_%1").arg(invID);

            QMap<QString,QString> tagToReplaceMap;
            setStateIDName(statID,pPrintCmd);
            tagToReplaceMap.insert("@StatID",statID);
            tagToReplaceMap.insert("@InvokeID",invID);
            tagToReplaceMap.insert("@ReturnVal",returnVal);
            tagToReplaceMap.insert("@TargetID",iTargetId);
            if(! msgType.isEmpty())
                tagToReplaceMap.insert("@ReportType",msgType);

            QByteArray statElemContents = replaceTag(tmplContents,tagToReplaceMap);
            if(! statElemContents.isEmpty())
            {
                QDomElement ElemOnStat = getElement(statElemContents,iDomDoc);
                if(! ElemOnStat.isNull())
                {
                    //                    QString delay = QString("%1s").arg(QString::number(waitForBuffer));
                    //                    QString event = QString("%1_Wait_For_Buffer_Depth").arg(iStateId);
                    //                    QDomElement ElemOnBufferStat = createBufferDepthState(iDomDoc,iStateId,statID,event,delay);
                    //                    if(! ElemOnBufferStat.isNull())
                    //                    {
                    //                        iRootElem.appendChild(ElemOnBufferStat);
                    //                        iRootElem.appendChild(ElemOnStat);

                    //                        GTAScxmlDataModel objDMReturn;
                    //                        objDMReturn.ID = returnVal;
                    //                        _DataModelList.insert(returnVal,objDMReturn);
                    //                    }

                    QDomNodeList domOnTransitList = ElemOnStat.elementsByTagName(SXN_TRANSITION);
                    if(!domOnTransitList.isEmpty())
                    {
                        QDomElement ElemOnTransit = domOnTransitList.at(0).toElement();
                        if(! ElemOnTransit.isNull())
                        {
                            GTAScxmlLogMessage objLog;
                            objLog.LOD =GTAScxmlLogMessage::Main;
                            objLog.Identifier =  instanceName;
                            objLog.ExpectedValue = returnVal;
                            objLog.CurrentValue = returnVal;//QString("'+%1+'").arg(returnVal);
                            objLog.Status = GTAScxmlLogMessage::Success;
                            objLog.Result = GTAScxmlLogMessage::OK;
                            QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                            ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                            ElemOnLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
                            ElemOnTransit.appendChild(ElemOnLog);
                            ElemOnTransit.setAttribute(SXA_STATUS,"success");
                        }
                    }
                }
            }

        }
    }
    return rc;
}
bool GTAExportScxml::createActionReturnFunctionState(const GTAActionFunctionReturn *pFuncRetCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{

    bool rc = false;
    if(pFuncRetCmd != NULL)
    {
        QString actionName = pFuncRetCmd->getAction();
        QString instanceName = pFuncRetCmd->getInstanceName();
        QString sParameterName = pFuncRetCmd->getReturnItem();



        QString tmplFile  = actionName + ".scxml";
        QString invokeId = QString(iStateId).replace('s','i');
        QString VarToLog;
        QDomElement stateElem;
        rc = getCloneRootElement(tmplFile,iDomDoc,stateElem);
        //In case of print message only onexit node is reqd.
        //In case of print print value all the nodes in template is reqd
        //hence creating new element and append required nodes depending upon print.
        QString logMessage =  QString("'|%1| %2'").arg(iStateId,instanceName);
        bool keepInvokeElem =  false;
        setStateIDName(iStateId,pFuncRetCmd);
        stateElem.setAttribute(SXA_ID,iStateId);
        if (rc)
        {

            QMap<QString,QDomElement> stateChildMap;
            getScxmlElementPathMap(stateElem,stateElem.nodeName(),stateChildMap);
            QDomElement onExitElement = stateChildMap.value(getOnExitXPath());
            QDomElement invokeElement = stateChildMap.value(getStateInvokeXPath());
            QDomElement ElemOnTransit = stateChildMap.value(getStateTransistionXPath());
            QDomElement ElemOnOneEntry = stateChildMap.value(getOnEntryXPath());
            QDomElement ElemOnAssign = stateChildMap.value(getOnEntryAssignXPath());


            QStringList paramListOnExit;
            QString sReturnFunName = QString("Return_function_%1").arg(iStateId);

            
            bool isConstant = false;
            sParameterName.toDouble(&isConstant);

            if(!isConstant)
                isConstant = (sParameterName.startsWith("'") & sParameterName.endsWith("'"));

            if(isConstant == false)
            {
                rc = insertValidAppDataModel(sParameterName);

                if ((rc == true) && ! invokeElement.isNull())
                {
                    GTASXFuncArgument objArg;
                    objArg.Name = "ParameterID";
                    objArg.Value = QString("%1").arg(sParameterName);
                    QList<GTASXFuncArgument> argList; argList.append(objArg);

                    GTASXFuncReturnVal objRetVal;
                    objRetVal.Name = sParameterName;
                    GTASXAssign objAssign(sParameterName,sParameterName);
                    GTASXLog objLog("Info",logMessage);
                    setInvokeAttr(invokeElement,invokeId,argList,objRetVal,objAssign,objLog);

                    //return statement.
                    QDomNodeList lstOfFinalize = invokeElement.elementsByTagName(SXN_FINALIZE);
                    if (lstOfFinalize.isEmpty()==false)
                    {
                        QDomNodeList lstOfAssign = lstOfFinalize.at(0).toElement().elementsByTagName(SXN_ASSIGN);
                        if (lstOfAssign.isEmpty()==false)
                        {

                            QDomElement assignElement = lstOfAssign.at(0).cloneNode(true).toElement();
                            assignElement.setAttribute(SXA_EXPR,QString("%1.Value").arg(sParameterName)); 
                            assignElement.setAttribute(SXA_LOCATION,sReturnFunName);
                            lstOfFinalize.at(0).appendChild(assignElement);
                            
                        }
                    }
                    keepInvokeElem = true;
                }
                else
                {
                    GTAScxmlDataModel objDM;
                    objDM.ID = sParameterName;
                    _DataModelList.insert(sParameterName,objDM);

                    ElemOnAssign.setAttribute(SXA_EXPR,sParameterName);
                    ElemOnAssign.setAttribute(SXA_LOCATION,sReturnFunName);

                }


                //Logging all Parameters
                GTAScxmlLogMessage objLogOnExit;
                objLogOnExit.LOD = GTAScxmlLogMessage::Main;
                objLogOnExit.Identifier    = instanceName;
                objLogOnExit.ExpectedValue = sParameterName;
                objLogOnExit.CurrentValue = sParameterName;//QString("'+%1+'").arg(sParameterName);
                objLogOnExit.Status = GTAScxmlLogMessage::Success;
                objLogOnExit.Result = GTAScxmlLogMessage::OK;

                QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                ElemOnLog.setAttribute(SXA_EXPR,objLogOnExit.getLogMessage());
                ElemOnTransit.appendChild(ElemOnLog);


                //Logging all Parameters
                paramListOnExit<<sParameterName;
            }
            else
            {
                ElemOnAssign.setAttribute(SXA_EXPR,sParameterName);
                ElemOnAssign.setAttribute(SXA_LOCATION,sReturnFunName);
            }

            GTAScxmlDataModel objDM;
            objDM.ID = sReturnFunName;
            _DataModelList.insert(sReturnFunName,objDM);


            QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
            stateElem.appendChild(ElemOnExit);
            insertVariableOnExit(ElemOnExit,paramListOnExit,iStateId,instanceName,iDomDoc);


            GTASXTransition objTransit;
            objTransit.Target = iTargetId;
            objTransit.status="success";
            setTransitionAttr(ElemOnTransit,objTransit);

            if(! keepInvokeElem )
            {
                stateElem.removeChild(invokeElement);
                ElemOnTransit.removeAttribute(SXA_EVENT);
            }
            else
            {
                
                if(ElemOnOneEntry.isNull()==false)
                {
                    stateElem.removeChild(ElemOnOneEntry);

                }
            }
            GTAActionBase * pActionBase = dynamic_cast<GTAActionBase *>(pFuncRetCmd->getParent());
            if(pActionBase != NULL)
            {
                createLoopTimeOutTranstions(stateElem,iDomDoc,iStateId,pActionBase);
            }

            iRootElem.appendChild(stateElem);



        }
    }
    return rc;
}
bool GTAExportScxml::createActionPrintBiteBufferState(const GTAActionPrint *pPrintCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc= createActionPrintFwcBufferState(pPrintCmd,iDomDoc,iRootElem,iStateId,iTargetId);
    return rc;
}
bool GTAExportScxml::createActionTitleState(const GTAActionParentTitle *pTitleCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId, QDomElement &oRootElem)
{
    bool rc = false;
    if(pTitleCmd != NULL)
    {
        QString titleName = pTitleCmd->getTitle();
        QString actionName = pTitleCmd->getAction();
        QString tmplFile  = QString("%1.%2").arg(actionName,"scxml");
        rc = getCloneRootElement(tmplFile,iDomDoc,oRootElem);
        QString sStateId = iStateId;
        //oRootElem.setAttribute(SXA_TARGET,iTargetId);
        setStateAttr(oRootElem,sStateId);
        setStateIDName(sStateId,pTitleCmd);
        QDomElement txnNode = iDomDoc.createElement(SXN_TRANSITION);
        oRootElem.appendChild(txnNode);
        txnNode.setAttribute(SXA_TARGET,iTargetId);


        QDomNodeList finalNodes = oRootElem.elementsByTagName(SXN_FINAL);
        QDomElement finalElem;
        if (!finalNodes.isEmpty())
        {
            finalElem = finalNodes.at(0).toElement();
            
        }
        QList<GTACommandBase *> unIgnoredChildren;
        pTitleCmd->getUningnoredChildren(unIgnoredChildren);
        if(unIgnoredChildren.count() > 1)
        {
            finalElem.setAttribute(SXA_ID,sStateId+"_Final");
        }
        else
        {
            oRootElem.removeChild(finalElem);
            //QDomElement txElem = iDomDoc.createElement(SXN_TRANSITION);
            //txElem.setAttribute(SXA_TARGET,iTargetId);
            //oRootElem.appendChild(txElem);
        }
        iRootElem.appendChild(oRootElem);
        rc = true;
    }
    return rc;
}


bool GTAExportScxml::createActionSubscribeState(const GTAActionSubscribe *pCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;

    QString instanceName = pCmd->getInstanceName();

    QDomElement stateElem = iDomDoc.createElement(SXN_STATE);
    QDomElement transitElem = iDomDoc.createElement(SXN_TRANSITION);
    transitElem.setAttribute(SXA_TARGET,iTargetId);

    QString finalStateID = QString("%1_final").arg(iStateId);

    QStringList subParamList = pCmd->getValues();
    QStringList validParamList;
    for(int i=0;i<subParamList.count();i++)
    {
        if(_ParamList.contains(subParamList.at(i)))
        {
            validParamList.append(subParamList.at(i));
            QString paramType;
            insertValidAppDataModel(QString(subParamList.at(i)),paramType,false);
        }
    }

    for(int i=0;i<validParamList.count();i++)
    {
        QString varName = validParamList.at(i);
        QString subscribeStateId = QString("%1_%2").arg(iStateId,QString::number(i));
        QString subscribeTargetId = QString("%1_%2").arg(iStateId,QString::number(i+1));

        if(i >= validParamList.count() - 1)
        {
            subscribeTargetId = finalStateID;
        }
        QDomElement subscribeState;

        if(!i)
        {
            stateElem.setAttribute(SXA_INITIALSTATE,subscribeStateId);
        }
        QString returnCode;
        getSubscribeStateForInvoke(iDomDoc,varName,subscribeStateId,subscribeState,returnCode);
        if (subscribeState.isNull()==false)
        {
            subscribeState.setAttribute(SXA_ID,subscribeStateId);
            QDomNodeList txnNodes = subscribeState.elementsByTagName(SXN_TRANSITION);
            if(txnNodes.isEmpty()==false)
            {
                QDomElement txNode1 = txnNodes.at(0).toElement();
                txNode1.setAttribute(SXA_TARGET,subscribeTargetId);
                txNode1.setAttribute(SXA_STATUS,"success");
                txNode1.setAttribute(SXA_CONDITION,QString("%1.Status == 'OK'").arg(returnCode));
                txNode1.setAttribute(SXA_EVENT,"done.invoke._all");

                //Log Node
                GTAScxmlLogMessage objWaitLog;
                objWaitLog.LOD = GTAScxmlLogMessage::Main;
                objWaitLog.Identifier = instanceName;
                objWaitLog.ExpectedValue = returnCode;
                objWaitLog.CurrentValue = returnCode;
                objWaitLog.Result = GTAScxmlLogMessage::OK;
                objWaitLog.Status = GTAScxmlLogMessage::Success;
                QString logMsg1 = objWaitLog.getLogMessage();

                QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                ElemOnLog.setAttribute(SXA_EXPR,logMsg1);



                QDomElement txNode2 = txNode1.cloneNode(true).toElement();
                txNode2.setAttribute(SXA_TARGET,subscribeTargetId);
                txNode2.setAttribute(SXA_STATUS,"fail");
                txNode2.setAttribute(SXA_CONDITION,QString("%1.Status != 'OK'").arg(returnCode));
                txNode2.setAttribute(SXA_EVENT,"done.invoke._all");


                //Log Node
                GTAScxmlLogMessage objWaitLog1;
                objWaitLog1.LOD = GTAScxmlLogMessage::Main;
                objWaitLog1.Identifier = instanceName;
                objWaitLog1.ExpectedValue = returnCode;
                objWaitLog1.CurrentValue = returnCode;
                objWaitLog1.Result = GTAScxmlLogMessage::KO;
                objWaitLog1.Status = GTAScxmlLogMessage::Fail;
                QString logMsg2 = objWaitLog1.getLogMessage();

                QDomElement ElemOnLog1 = iDomDoc.createElement(SXN_LOG);
                ElemOnLog1.setAttribute(SXA_LABEL,"'info'");
                ElemOnLog1.setAttribute(SXA_EXPR,logMsg2);


                txNode1.appendChild(ElemOnLog);
                txNode2.appendChild(ElemOnLog1);


                subscribeState.insertAfter(txNode2,txNode1);

            }
            stateElem.appendChild(subscribeState);
        }
    }

    stateElem.appendChild(transitElem);
    stateElem.setAttribute(SXA_ID,iStateId);

    QDomElement finalElem = iDomDoc.createElement(SXN_FINAL);
    finalElem.setAttribute(SXA_ID,finalStateID);

    stateElem.appendChild(finalElem);


    //    if (iRootElem.isNull()==false)
    //    {
    //        QDomNodeList finalNodeList = iRootElem.elementsByTagName(SXN_FINAL);
    //        if(!finalNodeList.isEmpty())
    //        {
    //            QDomElement finalNode = finalNodeList.at(0).toElement();
    //            if (finalNode.isNull() == false && finalNode.nodeName()== SXN_FINAL)
    //            {
    //                transitElem.setAttribute(SXA_TARGET,finalNode.attribute(SXA_ID));
    //                rc = true;
    //            }
    //        }
    //    }
    iRootElem.appendChild(stateElem);
    return rc;
}
bool GTAExportScxml::createActionUnSubscribeState(const GTAActionUnSubscribe *pCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;

    QString instanceName = pCmd->getInstanceName();

    QDomElement stateElem = iDomDoc.createElement(SXN_STATE);
    QDomElement transitElem = iDomDoc.createElement(SXN_TRANSITION);
    transitElem.setAttribute(SXA_TARGET,iTargetId);

    QString finalStateID = QString("%1_final").arg(iStateId);

    QStringList subParamList;
    if(pCmd->isReleaseAll())
    {
        GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
        if((pAppCtrl != NULL) && (_pTitleBasedElement != NULL))
        {
            auto index = _pTitleBasedElement->getIndexOfCommand(pCmd);
            if (index) {
                subParamList = pAppCtrl->getLocalVariables(true, true, *index);
                subParamList.removeDuplicates();
            }
        }
    }
    else
    {
        subParamList = pCmd->getValues();
    }

    QStringList validParamList;
    for(int i=0;i<subParamList.count();i++)
    {
        if(_ParamList.contains(subParamList.at(i)))
        {
            validParamList.append(subParamList.at(i));
            QString paramType;
            insertValidAppDataModel(QString(subParamList.at(i)),paramType,false);
        }
    }

    for(int i=0;i<validParamList.count();i++)
    {
        QString varName = validParamList.at(i);
        QString subscribeStateId = QString("%1_%2").arg(iStateId,QString::number(i));
        QString subscribeTargetId = QString("%1_%2").arg(iStateId,QString::number(i+1));

        if(i >= validParamList.count() - 1)
        {
            subscribeTargetId = finalStateID;
        }
        QDomElement subscribeState;

        if(!i)
        {
            stateElem.setAttribute(SXA_INITIALSTATE,subscribeStateId);
        }
        QString returnCode;
        getUnSubscribeStateForInvoke(iDomDoc,varName,subscribeState,subscribeStateId,returnCode);
        if (subscribeState.isNull()==false)
        {
            subscribeState.setAttribute(SXA_ID,subscribeStateId);
            QDomNodeList txnNodes = subscribeState.elementsByTagName(SXN_TRANSITION);
            if(txnNodes.isEmpty()==false)
            {
                QDomElement txNode1 = txnNodes.at(0).toElement();
                txNode1.setAttribute(SXA_TARGET,subscribeTargetId);
                txNode1.setAttribute(SXA_STATUS,"success");
                txNode1.setAttribute(SXA_CONDITION,QString("%1.Status == 'OK'").arg(returnCode));
                txNode1.setAttribute(SXA_EVENT,"done.invoke._all");

                //Log Node
                GTAScxmlLogMessage objWaitLog;
                objWaitLog.LOD = GTAScxmlLogMessage::Main;
                objWaitLog.Identifier = instanceName;
                objWaitLog.ExpectedValue = returnCode;
                objWaitLog.CurrentValue = returnCode;
                objWaitLog.Result = GTAScxmlLogMessage::OK;
                objWaitLog.Status = GTAScxmlLogMessage::Success;
                QString logMsg1 = objWaitLog.getLogMessage();

                QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                ElemOnLog.setAttribute(SXA_EXPR,logMsg1);



                QDomElement txNode2 = txNode1.cloneNode(true).toElement();
                txNode2.setAttribute(SXA_TARGET,subscribeTargetId);
                txNode2.setAttribute(SXA_STATUS,"fail");
                txNode2.setAttribute(SXA_CONDITION,QString("%1.Status != 'OK'").arg(returnCode));
                txNode2.setAttribute(SXA_EVENT,"done.invoke._all");


                //Log Node
                GTAScxmlLogMessage objWaitLog1;
                objWaitLog1.LOD = GTAScxmlLogMessage::Main;
                objWaitLog1.Identifier = instanceName;
                objWaitLog1.ExpectedValue = returnCode;
                objWaitLog1.CurrentValue = returnCode;
                objWaitLog1.Result = GTAScxmlLogMessage::KO;
                objWaitLog1.Status = GTAScxmlLogMessage::Fail;
                QString logMsg2 = objWaitLog1.getLogMessage();

                QDomElement ElemOnLog1 = iDomDoc.createElement(SXN_LOG);
                ElemOnLog1.setAttribute(SXA_LABEL,"'info'");
                ElemOnLog1.setAttribute(SXA_EXPR,logMsg2);


                txNode1.appendChild(ElemOnLog);
                txNode2.appendChild(ElemOnLog1);


                subscribeState.insertAfter(txNode2,txNode1);

            }
            stateElem.appendChild(subscribeState);
        }
    }

    stateElem.appendChild(transitElem);
    stateElem.setAttribute(SXA_ID,iStateId);

    QDomElement finalElem = iDomDoc.createElement(SXN_FINAL);
    finalElem.setAttribute(SXA_ID,finalStateID);

    stateElem.appendChild(finalElem);


    //    if (iRootElem.isNull()==false)
    //    {
    //        QDomNodeList finalNodeList = iRootElem.elementsByTagName(SXN_FINAL);
    //        if(!finalNodeList.isEmpty())
    //        {
    //            QDomElement finalNode = finalNodeList.at(0).toElement();
    //            if (finalNode.isNull() == false && finalNode.nodeName()== SXN_FINAL)
    //            {
    //                transitElem.setAttribute(SXA_TARGET,finalNode.attribute(SXA_ID));
    //                rc = true;
    //            }
    //        }
    //    }
    iRootElem.appendChild(stateElem);
    return rc;
}

bool GTAExportScxml::createActionInvlaidState(const GTAInvalidCommand *, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &)
{
    bool rc = false;
    QDomElement stateElem = iDomDoc.createElement(SXN_STATE);
    QDomElement transitElem = iDomDoc.createElement(SXN_TRANSITION);
    
    stateElem.appendChild(transitElem);
    stateElem.setAttribute(SXA_ID,iStateId);
    if (iRootElem.isNull()==false)
    {
        QDomNodeList finalNodeList = iRootElem.elementsByTagName(SXN_FINAL);
        if(!finalNodeList.isEmpty())
        {
            QDomElement finalNode = finalNodeList.at(0).toElement();
            if (finalNode.isNull() == false && finalNode.nodeName()== SXN_FINAL)
            {
                transitElem.setAttribute(SXA_TARGET,finalNode.attribute(SXA_ID));
                rc = true;
            }
        }

    }
    iRootElem.appendChild(stateElem);
    return rc;
}
bool GTAExportScxml::createActionCallProcsState(const GTAActionCallProcedures *procCall, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId, QDomElement &oRootElem)
{
    bool rc = false;
    if(procCall != NULL)
    {


        QString tmplFile  = QString("%1.%2").arg(ACT_CALL_PROCS,"scxml");
        rc = getCloneRootElement(tmplFile,iDomDoc,oRootElem);
        int noOfCallProcs = procCall->getCallProcedures().size();

        if (rc)
        {
            iRootElem.appendChild(oRootElem);
            oRootElem.removeAttribute(SXA_INITIALSTATE);
            oRootElem.setAttribute(SXA_ID,iStateId);
            setStateIDName(iStateId,procCall);
            

            QDomNodeList childNodes = oRootElem.childNodes();
            QString dummyStateID = QString("Call_ParallelState_finished_%1").arg(iStateId);

            QString parallelCallCounter = QString("Call_ParallelState_Counter_%1").arg(iStateId);

            GTAScxmlDataModel objDM;
            objDM.ID = parallelCallCounter;
            objDM.Expr="0";
            _DataModelList.insert(parallelCallCounter,objDM);


            for (int i=0;i<childNodes.size();i++)
            {
                QDomNode childNode = childNodes.at(i);
                QString sChildNodeName = childNode.nodeName();

                if (sChildNodeName == SXN_STATE)
                {
                    childNode.toElement().setAttribute(SXA_ID,dummyStateID);

                    QDomNodeList stateTransisionNodes  = childNode.toElement().elementsByTagName(SXN_TRANSITION);
                    if (stateTransisionNodes.isEmpty()==false)
                    {
                        stateTransisionNodes.at(0).toElement().setAttribute(SXA_TARGET,iTargetId);
                        stateTransisionNodes.at(0).toElement().setAttribute(SXA_CONDITION,QString("%1 == %2").arg(parallelCallCounter,QString::number(noOfCallProcs)));
                    }
                }
                else if (sChildNodeName == SXN_TRANSITION)
                {
                    childNode.toElement().setAttribute(SXA_TARGET,iTargetId); 
                    childNode.toElement().setAttribute(SXA_EVENT,QString("done.state.%1").arg(dummyStateID));
                }
            }

            
        }
    }

    return rc;
}
bool GTAExportScxml::createActionCallState(const GTAActionCall *pExtCall, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId, QDomElement &oRootElem,bool isDummy)
{
    bool rc = false;
    if(pExtCall != NULL)
    {
        QString elementName = pExtCall->getElement();
        QString instanceName = pExtCall->getInstanceName();
        QString tmplFile  = QString("%1.%2").arg(ACT_CALL,"scxml");
        rc = getCloneRootElement(tmplFile,iDomDoc,oRootElem);
        QString sStateId = iStateId;

        //oRootElem.setAttribute(SXA_TARGET,iTargetId);
        setStateAttr(oRootElem,sStateId);
        setStateIDName(sStateId,pExtCall);


        QString sTimeOut = getTimeOutInMiliSecond((GTACommandBase*)pExtCall);
        GTAAppController* pAppCtrl = GTAAppController::getGTAAppController(0);
        double docTimeOut = 0.0;
        pAppCtrl->getTotalTimeOutForDocumentInMs(elementName,docTimeOut);
        double dTTimeOut = sTimeOut.remove("ms",Qt::CaseInsensitive).toDouble()+docTimeOut;

        sTimeOut = QString::number( int(dTTimeOut));
        sTimeOut.append("ms");
        
        QString sTargetId = "";

        QDomNodeList finalNodes = oRootElem.elementsByTagName(SXN_FINAL);
        QDomElement finalElem;
        
        /*  QDomElement finalElem = iDomDoc.createElement(SXN_FINAL);
        finalElem.setAttribute(SXA_ID,QString("state_%1").arg(dummyStateID));

        oRootElem.appendChild(finalElem);*/

        if (!finalNodes.isEmpty())
        {
            finalElem = finalNodes.at(0).toElement();
            if(pExtCall->isParallelInSCXML()==true)
            {

                if (iRootElem.isNull()==false)
                {
                    QString sParentId = iRootElem.attribute(SXA_ID);//parent parallel state


                    QDomElement assignElement = iDomDoc.createElement(SXN_ASSIGN);
                    QDomElement onExitElem = iDomDoc.createElement(SXN_ONEXIT);

                    assignElement.setAttribute(SXA_EXPR,QString("Call_ParallelState_Counter_%1+1").arg(sParentId));
                    assignElement.setAttribute(SXA_LOCATION,QString("Call_ParallelState_Counter_%1").arg(sParentId));
                    onExitElem.appendChild(assignElement);

                    
                    oRootElem.appendChild(onExitElem);

                }

                // oRootElem.removeChild(finalElem);
                //finalElem.setAttribute(SXA_ID,QString("%1_Final").arg(iStateId));
                oRootElem.removeAttribute(SXA_INITIALSTATE);
                QDomElement firstChild = iRootElem.toElement().firstChild().toElement();
                if(firstChild.nodeName() == SXN_STATE)
                {
                    sTargetId = firstChild.attribute(SXA_ID);
                }
            }
            else
            {
                //QDomElement txnNode = iDomDoc.createElement(SXN_TRANSITION);
                //oRootElem.appendChild(txnNode);
                //txnNode.setAttribute(SXA_TARGET,iTargetId);
                //finalElem.setAttribute(SXA_ID,sStateId+"_final");
                sTargetId = iTargetId;

            }
            QDomElement txnNode = iDomDoc.createElement(SXN_TRANSITION);
            oRootElem.appendChild(txnNode);
            txnNode.setAttribute(SXA_TARGET,sTargetId);
            if(isDummy == true)
                oRootElem.removeChild(finalElem);
            else
                finalElem.setAttribute(SXA_ID,sStateId+"_Final");
        }
        /* if(pExtCall->hasChildren())
        {
            finalElem.setAttribute(SXA_ID,sStateId+"_final");
        }
        else
        {
            oRootElem.removeChild(finalElem);
            //QDomElement txElem = iDomDoc.createElement(SXN_TRANSITION);
            //txElem.setAttribute(SXA_TARGET,iTargetId);
            //oRootElem.appendChild(txElem);
        }*/
        QDomNodeList oneEntryNodes = oRootElem.elementsByTagName(SXN_ONENTRY);
        if (oneEntryNodes.isEmpty()==false)
        {
            QDomElement oneEntryElem = oneEntryNodes.at(0).toElement();
            QDomNodeList sendNodes = oneEntryElem.elementsByTagName(SXN_SEND);
            QDomElement sendNode;
            if (sendNodes.isEmpty()==false)
                sendNode= sendNodes.at(0).toElement();
            else
            {
                sendNode = iDomDoc.createElement(SXN_SEND);
                oneEntryElem.appendChild(sendNode);
            }
            sendNode.setAttribute(SXA_EVENT,QString("%1_TimeOut_Event").arg(sStateId));
            sendNode.setAttribute(SXA_DELAY,QString(sTimeOut));
        }
        iRootElem.appendChild(oRootElem);
        //createActionOnFail(iDomDoc,oRootElem,sStateId,iTargetId,"",sTimeOut,"Call timeout event");
        rc = true;
    }
    return rc;
}
bool GTAExportScxml::createActionOneClickTitleState(const GTAActionOneClickParentTitle *pTitleCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId, QDomElement &oRootElem)
{
    bool rc = false;
    if(pTitleCmd != NULL)
    {
        QString titleName = pTitleCmd->getTitle();
        // QString actionName = pTitleCmd->getAction();
        QString tmplFile  = QString("title.scxml");
        rc = getCloneRootElement(tmplFile,iDomDoc,oRootElem);
        QString sStateId = iStateId;
        //oRootElem.setAttribute(SXA_TARGET,iTargetId);
        setStateAttr(oRootElem,sStateId);
        setStateIDName(sStateId,pTitleCmd);
        QDomElement txnNode = iDomDoc.createElement(SXN_TRANSITION);
        oRootElem.appendChild(txnNode);
        txnNode.setAttribute(SXA_TARGET,"oneclick_finished" );
        if (pTitleCmd->hasChildren())
            oRootElem.setAttribute(SXA_INITIALSTATE,iTargetId);


        QDomNodeList finalNodes = oRootElem.elementsByTagName(SXN_FINAL);
        QDomElement finalElem;
        if (!finalNodes.isEmpty())
        {
            finalElem = finalNodes.at(0).toElement();

        }
        if(pTitleCmd->hasChildren())
        {
            
            finalElem.setAttribute(SXA_ID,sStateId+"_Final");

        }
        else
        {
            oRootElem.removeChild(finalElem);
            //QDomElement txElem = iDomDoc.createElement(SXN_TRANSITION);
            //txElem.setAttribute(SXA_TARGET,"oneclick_finished" );
            //oRootElem.appendChild(txElem);
        }
        iRootElem.appendChild(oRootElem);

    }
    return rc;
}
bool GTAExportScxml::createActionWaitState(const GTAActionWait *pWaitCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pWaitCmd != NULL)
    {
        if(pWaitCmd->getWaitType()==GTAActionWait::UNTIL)
        {
            GTACheckValue *pCheckCmd = new GTACheckValue();
            pCheckCmd->setActionOnFail(pWaitCmd->getActionOnFail());
            QString instance = pWaitCmd->getInstanceName();
            instance.replace("`","");
            pCheckCmd->setInstanceName(instance);
            QString time,unit;
            pWaitCmd->getConfCheckTime(time,unit);
            pCheckCmd->setConfCheckTime(time,unit);
            QString time1,unit1;
            pWaitCmd->getTimeOut(time1,unit1);
            pCheckCmd->setTimeOut(time1,unit1);
            pCheckCmd->setWaitUntil(true);
            pCheckCmd->setParent(pWaitCmd->getParent());

            bool isFsCheck = pWaitCmd->getIsFsOnly();
            bool isValcheck = pWaitCmd->getIsValueOnly();
            QString funcSts = pWaitCmd->getFunctionalStatus();


            pCheckCmd->setInstancePath(pWaitCmd->getInstancePath());
            QString lhs = pWaitCmd->getParameter();
            QString rhs = pWaitCmd->getValue();
            updateWithGenToolParam(lhs);
            updateWithGenToolParam(rhs);
            QString precision,precisionType;
            pWaitCmd->getPrecision(precision,precisionType);
            QString condition = pWaitCmd->getCondition();
            bool isVal = false;
            double isprecision = precision.toDouble(&isVal);
            if(!isVal)
                isprecision = 0.0;

            if(funcSts.trimmed().isEmpty())
                funcSts = "NO";
            //follwoing is normal operation add two more conditions for FS only and value only
            pCheckCmd->insertParamenter(lhs,condition,rhs,funcSts,isprecision,QString(""),precisionType,isValcheck,isFsCheck);
            rc = createCheckValueState(pCheckCmd,iDomDoc,iRootElem,iStateId,iTargetId,"",false);
        }
        else if(pWaitCmd->getWaitType()==GTAActionWait::FOR)
        {
            rc = createActionWaitForState(pWaitCmd,iDomDoc,iRootElem,iStateId,iTargetId);
        }

    }
    return rc;
}

bool GTAExportScxml::createActionWaitForState(const GTAActionWait *pWaitCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pWaitCmd != NULL)
    {
        QString actionName = pWaitCmd->getAction();
        QString instanceName = pWaitCmd->getInstanceName();
        QString sComplement = pWaitCmd->getComplement();
        QString timeOut = getTimeOutInMiliSecond((GTAActionWait*)pWaitCmd);
        QString conftimeInMiliSec = getConfTimeInMiliSecond((GTAActionWait*)pWaitCmd);
        QString actionOnFail = pWaitCmd->getActionOnFail();
        QStringList dumpList  = pWaitCmd->getDumpList();
        QString sStateId = iStateId;
        QString tmplFile  = QString("%1_%2.%3").arg(actionName,sComplement,"scxml");
        QString configTimeStateID;
        bool bConfigTimeExists=false;
        //GTAAppSettings appSet;
        //bool repeatLog = false; //appSet.getRepeatedLoggingSetting();

        bool useSeperateInvokesForPIR = TestConfig::getInstance()->getPIRInvokeForSCXML();
        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pWaitCmd->getParent());

        
        QString invokeId = QString(sStateId).replace('s','i');
        
        QDomElement ElemOnState;
        QDomElement ConfTimeStateElem;
        QDomElement elementOnConfTimeState;
        rc = getCloneRootElement(tmplFile,iDomDoc,ElemOnState);

        if (rc && ! ElemOnState.isNull())
        {
            QMap<QString,QDomElement> ElemPathMap;
            QStringList pirParamList;
            getScxmlElementPathMap(ElemOnState,ElemOnState.nodeName(),ElemPathMap);
            if (!ElemPathMap.isEmpty())
            {
                QDomElement stateElem;


                if (pWaitCmd->getWaitType()==GTAActionWait::FOR)
                {
                    if (ElemPathMap.contains(SXN_STATE))
                    {
                        stateElem = ElemPathMap.value(SXN_STATE);
                        stateElem.setAttribute(SXA_ID,sStateId);
                        setStateIDName(sStateId,pWaitCmd);
                    }
                    QString sSeconds = pWaitCmd->getCounter();

                    //set Transit
                    QString eventName = QString("%1_%2_%3").arg(actionName,sComplement,invokeId);
                    GTASXTransition objTransit;
                    objTransit.Event = eventName;
                    objTransit.Target =iTargetId;
                    objTransit.status = "success";
                    QDomElement ElemOnTransit = ElemPathMap.value(getStateTransistionXPath());
                    setTransitionAttr(ElemOnTransit,objTransit);

                    bool isNumber = false;
                    double dSeconds = sSeconds.toDouble(&isNumber);
                    QString sTimeVal("0");
                    if (isNumber == true)
                        dSeconds = dSeconds*1000;

                    sTimeVal=QString::number(dSeconds,'f',0);
                    sTimeVal.append("ms");
                    GTASXLog objLog;
                    objLog.Label = "Info";
                    objLog.Expr = QString("'%1'").arg(instanceName);
                    GTASXSend objSend;
                    objSend.Event = eventName;
                    objSend.Delay = sTimeVal;
                    GTASXAssign objAssign;
                    QDomElement ElemOnEntry = ElemPathMap.value(getOnEntryXPath());
                    setOnEntryOrExit(ElemOnEntry,objAssign,objLog,objSend);

                    //Log Node
                    GTAScxmlLogMessage objWaitLog;
                    objWaitLog.LOD = GTAScxmlLogMessage::Main;
                    objWaitLog.Identifier = instanceName;
                    objWaitLog.ExpectedValue = sTimeVal;
                    objWaitLog.CurrentValue = sTimeVal;
                    objWaitLog.Result = GTAScxmlLogMessage::OK;
                    objWaitLog.Status = GTAScxmlLogMessage::Success;
                    QString logMsg1 = objWaitLog.getLogMessage();

                    logMsg1 = logMsg1.remove("+");
                    logMsg1 = logMsg1.remove("'");
                    logMsg1 = QString("'%1'").arg(logMsg1);
                    QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
                    ElemOnLog.setAttribute(SXA_LABEL,"'info'");
                    ElemOnLog.setAttribute(SXA_EXPR,logMsg1);
                    ElemOnTransit.appendChild(ElemOnLog);

                    //time out in case if parent is a while or do while statement
                    
                    if(pParent != NULL)
                    {
                        createLoopTimeOutTranstions(stateElem,iDomDoc,sStateId,pParent);
                    }

                }
                else if (pWaitCmd->getWaitType()==GTAActionWait::UNTIL)
                {
                    QDomElement oTimeOutElem;
                    QString TimeOutTarget = QString("%1_1").arg(sStateId);
                    rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeOut,TimeOutTarget);
                    if (rc)
                    {
                        sStateId = TimeOutTarget;
                        iRootElem.appendChild(oTimeOutElem);
                    }
                    //QString sInvokeDelay = pWaitCmd->getInvokeNodeDelayInMs();
                    QString confTimeTarget = QString("%1_1").arg(sStateId);
                    configTimeStateID= QString("%1_Confirmation_time").arg(sStateId);
                    QString configTimeEventName;  
                    bConfigTimeExists = getConfigurationTimeNode(iDomDoc,ConfTimeStateElem,configTimeStateID,conftimeInMiliSec,confTimeTarget,configTimeEventName);

                    if (ElemPathMap.contains(SXN_STATE))
                    {
                        stateElem = ElemPathMap.value(SXN_STATE);
                        stateElem.setAttribute(SXA_ID,sStateId);
                        setStateIDName(sStateId,pWaitCmd);
                    }
                    QString eventName = QString("%1_%2").arg(sStateId,"DelayCheckCond");

                    QString sParameterName = pWaitCmd->getParameter();
                    QString sValue = pWaitCmd->getValue();
                    QString sCondition = pWaitCmd->getCondition();
                    updateWithGenToolParam(sParameterName);
                    updateWithGenToolParam(sValue);
                    QString sPrecisonUnit,sPrecision;
                    pWaitCmd->getPrecision(sPrecision,sPrecisonUnit);
                    double dPrecisionVal = sPrecision.toDouble();

                    if(sPrecisonUnit == ACT_PRECISION_UNIT_PERCENT)
                    {
                        dPrecisionVal = dPrecisionVal*0.01;
                    }

                    QDomElement invokeElem = ElemPathMap.value(getStateInvokeXPath());
                    QStringList icdParamList;
                    QStringList paramList;
                    paramList.append(sParameterName);
                    paramList.append(sValue);
                    QDomElement insertAfter = invokeElem;
                    QString nextInvokeId = invokeId+"_0";
                    int counter = 0;
                    QStringList VarList;
                    bool isStringVal = false;
                    QStringList dumpListParameterForInvoke= getValidParametersToInvoke(dumpList);
                    paramList.append(dumpListParameterForInvoke);
                    
                    
                    foreach(QString param, paramList)
                    {
                        QDomElement invokeParamElem = invokeElem.cloneNode().toElement();
                        QString sTypeOfParam;
                        rc = insertValidAppDataModel(param,sTypeOfParam);
                        if (rc)
                        {

                            if((sTypeOfParam==PARAM_TYPE_PIR) && useSeperateInvokesForPIR)
                                pirParamList.append(param);

                            icdParamList.append(param);
                            VarList<<param;
                            //                            GTASXFuncArgument objFuncArg;
                            //                            objFuncArg.Name = "ParameterID";
                            //                            objFuncArg.Value = QString("%1").arg(param);

                            //                            GTASXFuncReturnVal objFuncRetVal;
                            //                            objFuncRetVal.Name = param;

                            //                            GTASXAssign objFinalizeAssign;
                            //                            objFinalizeAssign.Location =param;
                            //                            objFinalizeAssign.Expr = param;

                            //                            GTASXLog objFinalizeLog;
                            //                            objFinalizeLog.Expr = QString("'%1'").arg(instanceName);
                            //                            objFinalizeLog.Label = "Info";

                            //                            QList<GTASXFuncArgument> argList; argList.append(objFuncArg);
                            //                            setInvokeAttr(invokeParamElem,nextInvokeId,argList,objFuncRetVal,objFinalizeAssign,objFinalizeLog);
                            //                            setInvokeDelay(iDomDoc,invokeParamElem,pWaitCmd->getAction(),pWaitCmd->getComplement());

                            //                            stateElem.insertAfter(invokeParamElem,insertAfter);
                            //                            insertAfter = invokeParamElem;
                            //                            nextInvokeId = invokeId +"_"+ QString::number(++counter);
                            
                        }
                        else
                        {
                            bool isNumber = false;
                            param.toDouble(&isNumber);
                            if(isNumber == false && ! param.startsWith("\"") && ! param.startsWith("\'"))
                            {
                                VarList<<param;
                                GTAScxmlDataModel objDM;
                                objDM.ID = param;
                                _DataModelList.insert(param,objDM);
                            }
                            else
                            {
                                if(isNumber == false)
                                {
                                    isStringVal = true;
                                }
                            }
                        }
                    }
                    //if(!useSeperateInvokesForPIR)
                    //stateElem.removeChild(invokeElem);
                    
                    createInvokeStateNodes(iDomDoc,stateElem,invokeElem,paramList,invokeId,"ParameterID","Info",pWaitCmd->getInstanceName(),false,pWaitCmd->getAction(),pWaitCmd->getComplement(),sStateId);

                    QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
                    stateElem.appendChild(ElemOnExit);
                    insertVariableOnExit(ElemOnExit,paramList,iStateId,instanceName,iDomDoc);


                    QDomElement ElemOnSuccessTransit = ElemPathMap.value(getStateTransistionXPath());
                    GTASXTransition objSuccessTransit;
                    bool delayEventCheck = false;
                    if(counter > 0)
                    {
                        objSuccessTransit.Event = "done.invoke._all";
                    }
                    else
                    {
                        objSuccessTransit.Event = eventName;
                        delayEventCheck = true;
                    }

                    if(!bConfigTimeExists)
                        objSuccessTransit.Target = iTargetId ;
                    else
                        objSuccessTransit.Target = configTimeStateID;

                    QString condSuccess, condFail;

                    if(icdParamList.contains(sParameterName))
                    {
                        sParameterName.append(".Value");
                    }
                    if(icdParamList.contains(sValue))
                    {
                        sValue.append(".Value");
                    }


                    getExpresion(sParameterName,sValue,sCondition,condSuccess,condFail,isStringVal,dPrecisionVal,sPrecisonUnit);
                    objSuccessTransit.Condition = condSuccess;
                    objSuccessTransit.status = "success";
                    setTransitionAttr(ElemOnSuccessTransit,objSuccessTransit);

                    GTASXTransition objFailTransit;
                    objFailTransit.Event = objSuccessTransit.Event;


                    objFailTransit.Target = sStateId;
                    if(bConfigTimeExists)
                    {
                        //objFailTransit.Target = configTimeStateID;
                        objFailTransit.Event = QString("%1_TimeOut_Event").arg(iStateId);
                    }
                    objFailTransit.Condition = condFail;
                    objFailTransit.status = "fail";
                    QDomElement ElemOnFailTransit = ElemOnSuccessTransit.cloneNode().toElement();
                    setTransitionAttr(ElemOnFailTransit,objFailTransit);
                    stateElem.insertAfter(ElemOnFailTransit,ElemOnSuccessTransit);


                    //Log Node
                    GTAScxmlLogMessage objWaitLog;
                    objWaitLog.LOD = GTAScxmlLogMessage::Main;
                    objWaitLog.Identifier = instanceName;
                    objWaitLog.CurrentValue = sParameterName;
                    objWaitLog.ExpectedValue = QString("'+%1+'").arg(sValue);
                    objWaitLog.Result = GTAScxmlLogMessage::OK;
                    objWaitLog.Status = GTAScxmlLogMessage::Success;

                    QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
                    ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
                    ElemOnTrueLog.setAttribute(SXA_EXPR,objWaitLog.getLogMessage());
                    ElemOnSuccessTransit.appendChild(ElemOnTrueLog);

//                    if(repeatLog)
//                    {
//                        objWaitLog.Result = GTAScxmlLogMessage::NA;
//                        objWaitLog.LOD = GTAScxmlLogMessage::Detail;
//                        QDomElement ElemOnTrueLog1 = iDomDoc.createElement(SXN_LOG);
//                        ElemOnTrueLog1.setAttribute(SXA_LABEL,"'info'");
//                        ElemOnTrueLog1.setAttribute(SXA_EXPR,objWaitLog.getLogMessage());
//                        ElemOnFailTransit.appendChild(ElemOnTrueLog1);
//                    }


                    //Action on Fail
                    GTAScxmlLogMessage objLog;
                    objLog.LOD = GTAScxmlLogMessage::Main;
                    objLog.Identifier    = instanceName;
                    objLog.ExpectedValue = "True";
                    objLog.CurrentValue = "'TimeOut'";
                    objLog.Status =GTAScxmlLogMessage::TimeOut;
                    objLog.Result = GTAScxmlLogMessage::KO;
                    
                    QString logMsgTimeOut = objLog.getLogMessage();
                    logMsgTimeOut = logMsgTimeOut.remove("+");
                    logMsgTimeOut = logMsgTimeOut.remove("'");
                    logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);
                    createActionOnFail(iDomDoc,ElemOnState,iStateId,iTargetId,actionOnFail,timeOut,logMsgTimeOut,delayEventCheck,eventName,true,false);
                    //Logging all Parameters
                    //                    QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
                    //                    ElemOnState.appendChild(ElemOnExit);
                    //                    VarList.append(dumpList);
                    //                    insertVariableOnExit(ElemOnExit,VarList,sStateId,instanceName,iDomDoc);




                    if (bConfigTimeExists)
                    {
                        elementOnConfTimeState = ElemOnState.cloneNode().toElement();
                        QDomNodeList transitElementNodes = elementOnConfTimeState.elementsByTagName(SXN_TRANSITION);
                        elementOnConfTimeState.setAttribute(SXA_ID,confTimeTarget);
                        if(pParent != NULL)
                        {
                            createLoopTimeOutTranstions(elementOnConfTimeState,iDomDoc,confTimeTarget,pParent);
                        }
                        
                        QList <QDomElement> lstOfTransitElements;
                        for (int i=0;i<transitElementNodes.size();i++)
                        {
                            QDomElement transitElem = transitElementNodes.at(i).toElement();
                            lstOfTransitElements.append(transitElem);
                            QString status = transitElem.attribute(SXA_STATUS);
                            QString eventName = transitElem.attribute(SXA_EVENT);
                            if (status == "success" && eventName.contains("TimeOut_Event")==false)
                            {
                                transitElem.setAttribute(SXA_TARGET,iTargetId);

                            }
                            if (status == "fail"&&  eventName.contains("TimeOut_Event")==false)
                            {

                                QString nTargetId = confTimeTarget;
                                transitElem.setAttribute(SXA_TARGET,nTargetId);
                                
                            }
                            else if(status == "fail"&&  eventName.contains("TimeOut_Event")==true)
                            {
                                QString nTargetId = iTargetId;
                                if(actionOnFail == ACT_FAIL_STOP)
                                    nTargetId = MAIN_FINAL_STATE;
                                transitElem.setAttribute(SXA_TARGET,nTargetId);

                            }
                        }
                        foreach(QDomElement transitElement,lstOfTransitElements)
                        {
                            QString status = transitElement.attribute(SXA_STATUS);
                            QString condName = transitElement.attribute(SXA_CONDITION);
                            if (status == "success" && !condName.isEmpty())
                            {
                                QDomElement newTransitElement = transitElement;
                                newTransitElement.setAttribute(SXA_TARGET,confTimeTarget);
                                newTransitElement.setAttribute(SXA_EVENT,configTimeEventName);
                                //elementOnConfTimeState.appendChild(newTransitElement);
                                
                            }
                            if (status == "fail"&&  !condName.isEmpty())
                            {
                                QDomElement newTransitElement = transitElement;
                                QString nTargetId = iTargetId;
                                if(actionOnFail == ACT_FAIL_STOP)
                                    nTargetId = MAIN_FINAL_STATE;
                                newTransitElement.setAttribute(SXA_TARGET,nTargetId);
                                newTransitElement.setAttribute(SXA_EVENT,"done.invoke._all");
                                //elementOnConfTimeState.appendChild(newTransitElement);
                                
                            }
                            

                            
                            

                        }
                        
                    }

                    if(pParent != NULL)
                    {
                        createLoopTimeOutTranstions(ElemOnState,iDomDoc,sStateId,pParent);
                    }
                    createCallTimeOutTranstions(iDomDoc,ElemOnState,(GTACommandBase*)pWaitCmd,iTargetId);


                }

            }

            //            if (pirParamList.isEmpty()==false)
            //            {
            //                QDomElement elementOnSubscribe;
            //                getStateElemenetForPIR(iDomDoc,ElemOnState,pirParamList,elementOnSubscribe);

            //                iRootElem.appendChild(elementOnSubscribe);

            //            }
            //            else
            iRootElem.appendChild(ElemOnState);
            if (bConfigTimeExists)
            {
                //sStateId = confTimeTarget;
                iRootElem.appendChild(ConfTimeStateElem);
                iRootElem.appendChild(elementOnConfTimeState);
            }
            
        }
    }

    return rc;
}

bool GTAExportScxml::getStateElemenetForPIR(QDomDocument& iDomDoc, QDomElement& elementOnState, const QStringList&,QDomElement& oStateOnSubscribe)
{
    
    QString tmplFile = "pir_subscribe.scxml";
    bool rc = getCloneRootElement(tmplFile,iDomDoc,oStateOnSubscribe);
    if (oStateOnSubscribe.isNull()==false)
    {
        oStateOnSubscribe.insertAfter(elementOnState,oStateOnSubscribe.firstChild());
    }
    return rc;


}
bool GTAExportScxml::getSubscribeStateForInvoke(QDomDocument& iDomDoc,  const QString& iPirParameter, const QString& invokeStateID,QDomElement& oStateOnSubscribe,QString &oRetParam)
{

    QString tmplFile = "invoke_subscribe.scxml";
    bool rc = getCloneRootElement(tmplFile,iDomDoc,oStateOnSubscribe);

    if(!oStateOnSubscribe.isNull())
    {
        QString signalName = "";
        GTAICDParameter icdParamObj = _ParamList.value(iPirParameter);
        QString appName=icdParamObj.getApplicationName();
        QString busName=icdParamObj.getBusName();
        QString Media=icdParamObj.getMedia();
        QString paramType=icdParamObj.getValueType();
        if(!iPirParameter.isEmpty())
            signalName=iPirParameter.split(".").last();
        QString expression = QString("{ 'Name':'%1','%2':{'Media':'%3', '%5': {'ParameterType':'%4'}}}").arg(appName,busName,Media,paramType,signalName);
        QString localPirParam = QString("%1_local").arg(signalName);
        QString retParam =  QString("invoke_%1").arg(signalName);
        QDomNodeList oneEntryNodeLst = oStateOnSubscribe.elementsByTagName(SXN_ONENTRY);
        if (!oneEntryNodeLst.isEmpty())
        {
            QDomElement oneEntryElem  = oneEntryNodeLst.at(0).toElement();
            QDomNodeList assignNodeLst = oneEntryElem.elementsByTagName(SXN_ASSIGN);

            if (!assignNodeLst.isEmpty())
            {
                QDomElement assignNode  = assignNodeLst.at(0).toElement();
                assignNode.setAttribute(SXA_EXPR,expression);
                assignNode.setAttribute(SXA_LOCATION,localPirParam);
            }

        }

        //invoke node
        QDomNodeList invokeNodeLst = oStateOnSubscribe.elementsByTagName(SXN_INVOKE);

        if (invokeNodeLst.isEmpty()==false)
        {
            QDomElement invokeElement = invokeNodeLst.at(0).toElement();
            invokeElement.setAttribute(SXA_ID,QString("%1_invoke").arg(invokeStateID));


            //function node

            QDomNodeList functionNodeList = invokeElement.elementsByTagName(SXN_FUNCTION);
            if (!functionNodeList.isEmpty())
            {
                QDomElement funcNodeElem  = functionNodeList.at(0).toElement();
                QDomNodeList argNodeLst = funcNodeElem.elementsByTagName(SXN_ARGUMENT);
                QDomNodeList retNodeLst = funcNodeElem.elementsByTagName(SXN_RETURNVALUE);

                if (!argNodeLst.isEmpty())
                {
                    QDomElement argElem  = argNodeLst.at(0).toElement();
                    argElem.setAttribute(SXA_VALUE,localPirParam);
                }
                if (!retNodeLst.isEmpty())
                {
                    QDomElement retElem  = retNodeLst.at(0).toElement();
                    retElem.setAttribute(SXA_NAME,retParam);
                }
            }


            //finalize node
            {
                QDomNodeList finalizeNodelst = invokeElement.elementsByTagName(SXN_FINALIZE);
                if (!finalizeNodelst.isEmpty())
                {
                    QDomElement funcNodeElem  = finalizeNodelst.at(0).toElement();
                    QDomNodeList assignNodeLst = funcNodeElem.elementsByTagName(SXN_ASSIGN);

                    if (!assignNodeLst.isEmpty())
                    {
                        QDomElement assignNode  = assignNodeLst.at(0).toElement();
                        assignNode.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(retParam));
                        oRetParam=QString("%1_Return").arg(retParam);
                        assignNode.setAttribute(SXA_LOCATION,oRetParam);


                    }
                }
            }

        }
        

    }
    return rc;
}
bool GTAExportScxml::getUnSubscribeStateForInvoke(QDomDocument& iDomDoc, const QString& iPirParameter,QDomElement& oStateOnSubscribe,const QString& invokeStateID,QString &oRetParam)
{

    QString tmplFile = "invoke_unSubscribe.scxml";
    bool rc = getCloneRootElement(tmplFile,iDomDoc,oStateOnSubscribe);

    if(!oStateOnSubscribe.isNull())
    {
        QString signalName = "";
        GTAICDParameter icdParamObj = _ParamList.value(iPirParameter);
        QString appName=icdParamObj.getApplicationName();
        QString busName=icdParamObj.getBusName();
        QString Media=icdParamObj.getMedia();
        QString paramType=icdParamObj.getValueType();
        if(!iPirParameter.isEmpty())
            signalName=iPirParameter.split(".").last();
        QString expression = QString("{ 'Name':'%1','%2':{'Media':'%3', '%5': {'ParameterType':'%4'}}}").arg(appName,busName,Media,paramType,signalName);
        QString localPirParam = QString("%1_local").arg(signalName);
        QString retParam =  QString("invoke_%1").arg(signalName);
        QDomNodeList oneEntryNodeLst = oStateOnSubscribe.elementsByTagName(SXN_ONENTRY);
        if (!oneEntryNodeLst.isEmpty())
        {
            QDomElement oneEntryElem  = oneEntryNodeLst.at(0).toElement();
            QDomNodeList assignNodeLst = oneEntryElem.elementsByTagName(SXN_ASSIGN);

            if (!assignNodeLst.isEmpty())
            {
                QDomElement assignNode  = assignNodeLst.at(0).toElement();
                assignNode.setAttribute(SXA_EXPR,expression);
                assignNode.setAttribute(SXA_LOCATION,localPirParam);
            }

        }

        //invoke node
        QDomNodeList invokeNodeLst = oStateOnSubscribe.elementsByTagName(SXN_INVOKE);

        if (invokeNodeLst.isEmpty()==false)
        {
            QDomElement invokeElement = invokeNodeLst.at(0).toElement();
            invokeElement.setAttribute(SXA_ID,QString("%1_invoke").arg(invokeStateID));


            //function node

            QDomNodeList functionNodeList = invokeElement.elementsByTagName(SXN_FUNCTION);
            if (!functionNodeList.isEmpty())
            {
                QDomElement funcNodeElem  = functionNodeList.at(0).toElement();
                QDomNodeList argNodeLst = funcNodeElem.elementsByTagName(SXN_ARGUMENT);
                QDomNodeList retNodeLst = funcNodeElem.elementsByTagName(SXN_RETURNVALUE);

                if (!argNodeLst.isEmpty())
                {
                    QDomElement argElem  = argNodeLst.at(0).toElement();
                    argElem.setAttribute(SXA_VALUE,localPirParam);
                }
                if (!retNodeLst.isEmpty())
                {
                    QDomElement retElem  = retNodeLst.at(0).toElement();
                    retElem.setAttribute(SXA_NAME,retParam);
                }
            }


            //finalize node
            {
                QDomNodeList finalizeNodelst = invokeElement.elementsByTagName(SXN_FINALIZE);
                if (!finalizeNodelst.isEmpty())
                {
                    QDomElement funcNodeElem  = finalizeNodelst.at(0).toElement();
                    QDomNodeList assignNodeLst = funcNodeElem.elementsByTagName(SXN_ASSIGN);

                    if (!assignNodeLst.isEmpty())
                    {
                        QDomElement assignNode  = assignNodeLst.at(0).toElement();
                        assignNode.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(retParam));
                        oRetParam=QString("%1_Return").arg(retParam);
                        assignNode.setAttribute(SXA_LOCATION,oRetParam);


                    }
                }
            }

        }


    }


    return rc;


}
QString GTAExportScxml::getStateXPath()
{
    return  QString("%1").arg(SXN_STATE);
}
QString GTAExportScxml::getOnEntryXPath()
{
    return  QString("%1/%2").arg(SXN_STATE,SXN_ONENTRY);
}

QString GTAExportScxml::getOnEntryLogXPath()
{
    return  QString("%1/%2/%3").arg(SXN_STATE,SXN_ONENTRY,SXN_LOG);
}
QString GTAExportScxml::getOnEntrySendXPath()
{
    return  QString("%1/%2/%3").arg(SXN_STATE,SXN_ONENTRY,SXN_SEND);
}
QString GTAExportScxml::getOnEntryAssignXPath()
{
    return  QString("%1/%2/%3").arg(SXN_STATE,SXN_ONENTRY,SXN_ASSIGN);
}
QString GTAExportScxml::getStateTransistionXPath()
{
    return  QString("%1/%2").arg(SXN_STATE,SXN_TRANSITION);
}
QString GTAExportScxml::getStateInvokeXPath()
{
    return  QString("%1/%2").arg(SXN_STATE,SXN_INVOKE);
}

QString GTAExportScxml::getInvokeFinalizeXPath()
{
    return  QString("%1/%2/%3").arg(SXN_STATE,SXN_INVOKE,SXN_FINALIZE);
}
QString GTAExportScxml::getInvokeFunctionXPath()
{
    return  QString("%1/%2/%3/%4/%5").arg(SXN_STATE,SXN_INVOKE,SXN_CONTENT,SXN_XMLRPC,SXN_FUNCTION);
}
QString GTAExportScxml::getOnExitXPath()
{
    return  QString("%1/%2").arg(SXN_STATE,SXN_ONEXIT);
}

QString GTAExportScxml::getOnExitLogXPath()
{
    return  QString("%1/%2/%3").arg(SXN_STATE,SXN_ONEXIT,SXN_LOG);
}


QString GTAExportScxml::getOnExitAssignXPath()
{
    return  QString("%1/%2/%3").arg(SXN_STATE,SXN_ONEXIT,SXN_ASSIGN);
}

/**
  * This function return the list of valid parameters (ICD/PMR/PIR)
  * Here valid parameters are the variable present in ICD/PMR/PIR files
  * constant type which are of type number & local which are of strings
  * iVarList: list of variables used in commands, it can be any data type
  */
QStringList GTAExportScxml::getValidParametersToInvoke(const QStringList &iVarList)
{
    QStringList oParamList;
    foreach(QString parameter, iVarList)
    {
        bool isNumber = false;
        parameter.toDouble(&isNumber);
        if(isNumber == false)
        {
            bool rc = insertValidAppDataModel(parameter);
            if(rc )
            {
                oParamList.append(parameter);
            }
            else
            {
                GTAScxmlDataModel objDM;
                objDM.ID = parameter;
                _DataModelList.insert(parameter,objDM);
            }
        }
    }
    return oParamList;
}
/**
  * This function create invoke node for each parameter in iParamList
  * ioStatElem : The stat element where the invoke nodes are inserted
  * iRefInvokeElem: Each node is clon of this node, it shall be removed using ioStatElem::removeChild from the stat after calling this function
  * iParamList: list of valid parameters, valid parameter can be retrieved from getValidParametersToInvoke(), this function do not validate the parameters
  * iBaseInvokeID: The base invoke id is incremented by number of parameter
  * iFuncArgParam: the name of parameter in function argument of invoke node
  * iLogLabel : label of log in finalize node
  * iLogMsg : log message in finalize node
  */
void GTAExportScxml::createInvokeNodes(QDomDocument& iDomDoc, QDomElement& ioStatElem, QDomElement& iRefInvokeElem, const QStringList& iParamList,
                                          const QString& iBaseInvokeID, const QString& iFuncArgParam, const QString& iLogLabel,
                                          const QString& iLogMsg, bool iApendBraces, const QString& iActionName, const QString& iComplementName, QString& oTxEventName)
{
    QDomElement nextInvokeElem = iRefInvokeElem;
    for(int i = 0; i <iParamList.count(); i++)
    {
        QString varName = iParamList.at(i);
        QDomElement varInvokeElem = iRefInvokeElem.cloneNode().toElement();
        QString invokeId = QString("%1.%2").arg(iBaseInvokeID,QString::number(i));

        GTASXFuncArgument objArg;
        objArg.Name = iFuncArgParam;
        objArg.Value = varName;
        GTASXFuncReturnVal objRetVal;
        objRetVal.Name = varName;

        GTASXAssign objAssign;
        objAssign.Location = varName;


        GTASXLog objLog;
        objLog.Label = iLogLabel;
        objLog.Expr = iLogMsg;
        QList<GTASXFuncArgument> argList; argList.append(objArg);
        setInvokeAttr(varInvokeElem,invokeId,argList,objRetVal,objAssign,objLog,iApendBraces);
        ioStatElem.insertAfter(varInvokeElem,nextInvokeElem);
        QString txEvent1 = "";

        setInvokeDelay(iDomDoc,varInvokeElem,iActionName,iComplementName,txEvent1,1);//iParamList.count());
        nextInvokeElem = varInvokeElem;
        if(!txEvent1.isEmpty())
            oTxEventName= oTxEventName+" "+txEvent1;
    }
    oTxEventName = oTxEventName.trimmed();
}

void GTAExportScxml::createMultiInvokeNodeForVIP_VMAC(QDomDocument &iDomDoc, QDomElement &ioStatElem, QDomElement &iRefInvokeElem, const QStringList &iParamList, const QString &iBaseInvokeID, const QString &, const QString &, const QString &, bool, const QString &iActionName, const QString &iComplementName, QString &oTxEventName, const QString &iStateId)
{
    QString txEvent1 = "";
    QHash<QString, QString> actorParamMap;

    for(int i=0;i<iParamList.count();i++)
    {
        QString param = iParamList.at(i);
        if(_ParamList.contains(param))
        {
            QString actor = _ParamList.value(param).getEquipmentName();
            actorParamMap.insertMulti(actor,param);
        }
    }
    QDomNodeList onEntryElemList = ioStatElem.elementsByTagName(SXN_ONENTRY);
    QDomElement onEntryElem;
    if(!onEntryElemList.isEmpty())
    {
        onEntryElem = onEntryElemList.at(0).toElement();
        if(onEntryElem.isNull())
        {
            onEntryElem  = iDomDoc.createElement(SXN_ONENTRY);
            ioStatElem.appendChild(onEntryElem);
        }
    }
    else
    {
        onEntryElem = iDomDoc.createElement(SXN_ONENTRY);
    }

    ioStatElem.appendChild(onEntryElem);

    QStringList keys = actorParamMap.keys();
    for(int i=0;i<keys.count();i++)
    {
        QDomElement varInvokeElem = iRefInvokeElem.cloneNode().toElement();
        QString newInvokeId = QString("%1_%2").arg(iBaseInvokeID,QString::number(i));
        QDomNodeList finalizeElemList = varInvokeElem.elementsByTagName(SXN_FINALIZE);
        QDomElement finalizeElem;
        QDomElement contentElem;
        QDomElement xmlrpcElem;
        QDomElement funcElem;


        if(!finalizeElemList.isEmpty())
            finalizeElem = finalizeElemList.at(0).toElement();


        QStringList paramList = actorParamMap.values(keys.at(i));

        QString multiParamVarName = QString("%1_%2_MultiParam_%3").arg(iStateId, newInvokeId,keys.at(i));
        QString funcRetName = QString("%1_%2_%3_return").arg(iStateId, newInvokeId,keys.at(i));

        QDomElement assignEvent = iDomDoc.createElement(SXN_ASSIGN);
        assignEvent.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(funcRetName));
        assignEvent.setAttribute(SXA_LOCATION,multiParamVarName);
        finalizeElem.appendChild(assignEvent);

        QDomNodeList contentElemList = varInvokeElem.elementsByTagName(SXN_CONTENT);
        if(!contentElemList.isEmpty())
            contentElem = contentElemList.at(0).toElement();
        if(!contentElem.isNull())
        {
            QDomNodeList xmlrpcElemList = contentElem.elementsByTagName(SXN_XMLRPC);
            if(!xmlrpcElemList.isEmpty())
                xmlrpcElem = xmlrpcElemList.at(0).toElement();

            if(!xmlrpcElem.isNull())
            {
                QDomNodeList funcelemList = xmlrpcElem.elementsByTagName(SXN_FUNCTION);

                QDomAttr attr1 = xmlrpcElem.attributeNode(SXA_TOOLTYPE);
                //attr1.setValue();

                if(!funcelemList.isEmpty())
                {
                    funcElem = funcelemList.at(0).toElement();
                    QDomNodeList attrbuteList = funcElem.elementsByTagName(SXN_ARGUMENT);
                    if(!attrbuteList.isEmpty())
                    {
                        QDomElement attribute = attrbuteList.at(0).toElement();
                        attribute.setAttribute(SXA_TYPE,"array");
                        attribute.setAttribute(SXA_VALUE,multiParamVarName);
                        attribute.setAttribute(SXA_NAME,"ParameterList");
                    }
                    QDomNodeList retList = funcElem.elementsByTagName(SXN_RETURNVALUE);
                    if(!retList.isEmpty())
                    {
                        QDomElement ret = retList.at(0).toElement();
                        ret.setAttribute(SXA_NAME,funcRetName);
                    }
                }
            }
        }

        paramList.removeDuplicates();
        for(int i = 0; i <paramList.count(); i++)
        {
            QString varName = paramList.at(i);
            if(_ParamList.contains(varName))
            {
                varName = _ParamList.value(varName).getTempParamName();
                QDomElement assignOnEntry = iDomDoc.createElement(SXN_ASSIGN);
                QDomElement assignFinalize = iDomDoc.createElement(SXN_ASSIGN);

                assignOnEntry.setAttribute(SXA_EXPR,QString("'%1'").arg(varName));
                assignOnEntry.setAttribute(SXA_LOCATION,QString("%1[%2]").arg(multiParamVarName,QString::number(i)));
                onEntryElem.appendChild(assignOnEntry);
                assignFinalize.setAttribute(SXA_LOCATION,QString("%1").arg(paramList.at(i)));
                assignFinalize.setAttribute(SXA_EXPR,QString("%1['%2']").arg(multiParamVarName,varName));
                finalizeElem.appendChild(assignFinalize);
            }
        }
        QDomElement resetAssign = iDomDoc.createElement(SXN_ASSIGN);
        resetAssign.setAttribute(SXA_LOCATION,multiParamVarName);
        resetAssign.setAttribute(SXA_EXPR,"[]");
        onEntryElem.appendChild(resetAssign);
        finalizeElem.appendChild(resetAssign);

        GTAScxmlDataModel newData;
        newData.Expr = "[]";
        newData.ID = multiParamVarName;
        _DataModelList.insert(multiParamVarName,newData);

        varInvokeElem.setAttribute(SXA_ID,newInvokeId);
        ioStatElem.appendChild(varInvokeElem);
        setInvokeDelay(iDomDoc,varInvokeElem,iActionName,iComplementName,txEvent1,1);//iParamList.count());
    }
    if(!txEvent1.isEmpty())
        oTxEventName= txEvent1;//oTxEventName= oTxEventName+" "+txEvent1;

    oTxEventName = oTxEventName.trimmed();
}


void GTAExportScxml::createMultiInvokeNode(QDomDocument& iDomDoc, QDomElement& ioStatElem, QDomElement& iRefInvokeElem, const QStringList& iParamList,
                                              const QString& iBaseInvokeID, QString& oTxEventName, const QString& iActionName, const QString& iComplementName, const QString& iStateId)
{
    QDomNodeList onEntryElemList = ioStatElem.elementsByTagName(SXN_ONENTRY);
    QDomElement onEntryElem;
    if(!onEntryElemList.isEmpty())
    {
        onEntryElem = onEntryElemList.at(0).toElement();
        if(onEntryElem.isNull())
        {
            onEntryElem  = iDomDoc.createElement(SXN_ONENTRY);
            ioStatElem.appendChild(onEntryElem);
        }
    }
    else
    {
        onEntryElem = iDomDoc.createElement(SXN_ONENTRY);
    }



    ioStatElem.appendChild(onEntryElem);
    QDomElement varInvokeElem = iRefInvokeElem.cloneNode().toElement();

    QDomNodeList finalizeElemList = varInvokeElem.elementsByTagName(SXN_FINALIZE);
    QDomElement finalizeElem;
    QDomElement contentElem;
    QDomElement xmlrpcElem;
    QDomElement funcElem;


    if(!finalizeElemList.isEmpty())
        finalizeElem = finalizeElemList.at(0).toElement();

    QString multiParamVarName = QString("%1_%2_MultiParam").arg(iStateId, iBaseInvokeID);
    QString funcRetName = QString("%1_%2_return").arg(iStateId, iBaseInvokeID);

    QDomElement assignEvent = iDomDoc.createElement(SXN_ASSIGN);
    assignEvent.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(funcRetName));
    assignEvent.setAttribute(SXA_LOCATION,multiParamVarName);
    finalizeElem.appendChild(assignEvent);

    QDomNodeList contentElemList = varInvokeElem.elementsByTagName(SXN_CONTENT);
    if(!contentElemList.isEmpty())
        contentElem = contentElemList.at(0).toElement();
    if(!contentElem.isNull())
    {
        QDomNodeList xmlrpcElemList = contentElem.elementsByTagName(SXN_XMLRPC);
        if(!xmlrpcElemList.isEmpty())
            xmlrpcElem = xmlrpcElemList.at(0).toElement();

        if(!xmlrpcElem.isNull())
        {
            QDomNodeList funcelemList = xmlrpcElem.elementsByTagName(SXN_FUNCTION);

            if(!funcelemList.isEmpty())
            {
                funcElem = funcelemList.at(0).toElement();
                QDomNodeList attrbuteList = funcElem.elementsByTagName(SXN_ARGUMENT);
                if(!attrbuteList.isEmpty())
                {
                    QDomElement attribute = attrbuteList.at(0).toElement();
                    attribute.setAttribute(SXA_TYPE,"array");
                    attribute.setAttribute(SXA_VALUE,multiParamVarName);
                    attribute.setAttribute(SXA_NAME,"ParameterList");
                }
                QDomNodeList retList = funcElem.elementsByTagName(SXN_RETURNVALUE);
                if(!retList.isEmpty())
                {
                    QDomElement ret = retList.at(0).toElement();
                    ret.setAttribute(SXA_NAME,funcRetName);
                }
            }
        }
    }

    QStringList paramList = iParamList;
    paramList.removeDuplicates();
    for(int i = 0; i <paramList.count(); i++)
    {
        QString varName = paramList.at(i);
        if(_ParamList.contains(varName))
        {
            varName = _ParamList.value(varName).getTempParamName();
            QDomElement assignOnEntry = iDomDoc.createElement(SXN_ASSIGN);
            QDomElement assignFinalize = iDomDoc.createElement(SXN_ASSIGN);

            assignOnEntry.setAttribute(SXA_EXPR,QString("'%1'").arg(varName));
            assignOnEntry.setAttribute(SXA_LOCATION,QString("%1[%2]").arg(multiParamVarName,QString::number(i)));
            onEntryElem.appendChild(assignOnEntry);
            assignFinalize.setAttribute(SXA_LOCATION,QString("%1").arg(paramList.at(i)));
            assignFinalize.setAttribute(SXA_EXPR,QString("%1['%2']").arg(multiParamVarName,varName));
            finalizeElem.appendChild(assignFinalize);
        }
    }
    QDomElement resetAssign = iDomDoc.createElement(SXN_ASSIGN);
    resetAssign.setAttribute(SXA_LOCATION,multiParamVarName);
    resetAssign.setAttribute(SXA_EXPR,"[]");
    onEntryElem.appendChild(resetAssign);
    finalizeElem.appendChild(resetAssign);

    GTAScxmlDataModel newData;
    newData.Expr = "[]";
    newData.ID = multiParamVarName;
    _DataModelList.insert(multiParamVarName,newData);

    varInvokeElem.setAttribute(SXA_ID,iBaseInvokeID);
    ioStatElem.appendChild(varInvokeElem);
    QString txEvent1 = "";

    setInvokeDelay(iDomDoc,varInvokeElem,iActionName,iComplementName,txEvent1,1);//iParamList.count());
    if(!txEvent1.isEmpty())
        oTxEventName= txEvent1;//oTxEventName= oTxEventName+" "+txEvent1;

    oTxEventName = oTxEventName.trimmed();
}
void GTAExportScxml::createInvokeStateNodes(QDomDocument &iDomDoc, QDomElement &ioStatElem, QDomElement &iRefInvokeElem, const QStringList &iParamList, const QString &iBaseInvokeID, const QString &iFuncArgParam, const QString &iLogLabel, const QString &iLogMsg, bool appendBraces, const QString &iActionName, const QString &iComplementName, const QString iStateId, QString &oTxEventName)
{
    if (iParamList.isEmpty()==false)
    {
        bool useSeperateInvokesForPIR = TestConfig::getInstance()->getPIRInvokeForSCXML();
        if (useSeperateInvokesForPIR==false)
        {
            bool VIPVMACConfEnabled = false;
            //createInvokeNodes(iDomDoc,ioStatElem, iRefInvokeElem,iParamList,iBaseInvokeID,iFuncArgParam, iLogLabel,iLogMsg,appendBraces, iActionName,iComplementName,oTxEventName);
            if(VIPVMACConfEnabled)
                createMultiInvokeNodeForVIP_VMAC(iDomDoc,ioStatElem, iRefInvokeElem,iParamList,iBaseInvokeID,iFuncArgParam, iLogLabel,iLogMsg,appendBraces, iActionName,iComplementName,oTxEventName,iStateId);
            else
                createMultiInvokeNode(iDomDoc, ioStatElem, iRefInvokeElem, iParamList,
                                      iBaseInvokeID, oTxEventName, iActionName, iComplementName, iStateId);
        }
        else
        {

            QString stateId = ioStatElem.attribute(SXA_ID);
            QString dummyStateId = stateId;
            QString invokeId = stateId.append("_i");

            QDomElement nextInvokeElem = iRefInvokeElem;

            QStringList pirParamList;
            QStringList nonPirParamLst;
            
            //split into pir and non pir  and arrange  respectively  >>>>
            foreach(QString sParmeterName, iParamList)
            {

                GTAScxmlDataModel paramModel = _DataModelList.value(sParmeterName);
                
                if ((paramModel.IsPIR) && (_IsPirParamManagement)) // IKM : Under Use
                {
                    pirParamList.append(sParmeterName);
                }
                else
                    nonPirParamLst.append(sParmeterName);
            }

            QList<QStringList> lstParamLst;
            lstParamLst.append(nonPirParamLst);
            QDomElement pirInvokeState;
            QString sPirInvokeStateID = stateId + QString("_pir");
            QString tmplFile = "invoke.scxml";
            QDomElement tempInvoke;
            getCloneRootElement(tmplFile,iDomDoc,tempInvoke);
            bool hasPirInvokes = getPIRStateForInvokes(iDomDoc,sPirInvokeStateID,tempInvoke,iFuncArgParam,iLogLabel,iLogMsg,appendBraces,pirParamList,pirInvokeState,iActionName,iComplementName,oTxEventName);
            if(hasPirInvokes)
            {
                QDomElement txParentParam = iDomDoc.createElement(SXN_TRANSITION);
                pirInvokeState.appendChild(txParentParam);
                if(useSeperateInvokesForPIR)
                    txParentParam.setAttribute(SXA_TARGET,QString("%1_dummyState").arg(dummyStateId));
                else
                    txParentParam.setAttribute(SXA_TARGET,QString("%1_Final").arg(stateId));
                insertFinalState(iDomDoc,ioStatElem,stateId);
                ioStatElem.setAttribute(SXA_INITIALSTATE,sPirInvokeStateID);
                ioStatElem.insertAfter(pirInvokeState,ioStatElem.firstChild());
            }

            //lstParamLst.append(pirParamList);
            // <<<<

            QDomElement prevousParentState;
            for (int ip=0;ip<lstParamLst.size();ip++)
            {
                QStringList paramList = lstParamLst.at(ip);
                if(paramList.isEmpty()==false)
                {
                    QDomElement  paramParentState= iDomDoc.createElement(SXN_STATE);
                    prevousParentState = paramParentState;
                    QString sParentStateID = invokeId+QString("_%1").arg(QString::number(ip));
                    paramParentState.setAttribute(SXA_ID,sParentStateID);
                    ioStatElem.insertAfter(paramParentState,ioStatElem.firstChild());
                    
                    if(hasPirInvokes)
                        ioStatElem.insertAfter(pirInvokeState,paramParentState);
                    
                    // int paramListSize = paramList.count();

                    QDomElement previousInvokeState;

                    QString sInvokeStateFinalNodeId = sParentStateID + QString("_Final");

                    QString initialIdForParamParentState;
                    QString finalIdForParamParentState;
                    QDomElement pirSubscribeState;

                    ioStatElem.setAttribute(SXA_INITIALSTATE,sParentStateID);

                    QDomElement invokeStateElem = iDomDoc.createElement(SXN_STATE);
                    QString baseInvokeId = QString("%1_%2").arg(iStateId, "i0");
                    createMultiInvokeNode(iDomDoc, invokeStateElem, iRefInvokeElem, paramList,
                        baseInvokeId, oTxEventName, iActionName, iComplementName, iStateId);

                    QDomElement invokeTxElem = iDomDoc.createElement(SXN_TRANSITION);
                    invokeTxElem.setAttribute(SXA_STATUS,"success");
                    invokeTxElem.setAttribute(SXA_TARGET,sInvokeStateFinalNodeId);
                    if(!oTxEventName.trimmed().isEmpty())
                        invokeTxElem.setAttribute(SXA_EVENT,oTxEventName);
                    else
                        invokeTxElem.setAttribute(SXA_EVENT,"done.invoke._all");

                    QString getMultiParamStateID = QString("%1_1").arg(sParentStateID);
                    invokeStateElem.setAttribute(SXA_ID, getMultiParamStateID);
                    invokeStateElem.appendChild(invokeTxElem);
                    paramParentState.appendChild(invokeStateElem);
                    if(hasPirInvokes)
                    {
                        QDomElement txnNode = iDomDoc.createElement(SXN_TRANSITION);
                        paramParentState.insertAfter(txnNode,paramParentState.lastChild());
                        txnNode.setAttribute(SXA_TARGET,pirInvokeState.attribute(SXA_ID));
                    }
                    else
                    {
                        QDomElement txParentParam = iDomDoc.createElement(SXN_TRANSITION);
                        paramParentState.appendChild(txParentParam);
                        if(useSeperateInvokesForPIR)
                            txParentParam.setAttribute(SXA_TARGET,QString("%1_dummyState").arg(dummyStateId));
                        else
                            txParentParam.setAttribute(SXA_TARGET,QString("%1_Final").arg(stateId));
                        insertFinalState(iDomDoc,ioStatElem,stateId);
                    }

                    paramParentState.setAttribute(SXA_INITIALSTATE,getMultiParamStateID);


                    QDomElement finalElement = iDomDoc.createElement(SXN_FINAL);
                    finalElement.setAttribute(SXA_ID,sInvokeStateFinalNodeId);
                    paramParentState.insertBefore(finalElement,paramParentState.lastChild());


                }
            }
        }
    }
    
    QStringList items  = oTxEventName.trimmed().split(" ",QString::SkipEmptyParts);

    if(items.count()>1)
    {
        oTxEventName = items.at(0);
    }

    if (iParamList.isEmpty()==false)
    {
        ioStatElem.removeChild(iRefInvokeElem);
    }
}


bool GTAExportScxml::getPIRStateForInvokes(QDomDocument& iDomDoc,const QString& iStateID, const QDomElement& iRefInvokeElem, 
                                              const QString &iFuncArgParam,const QString &iLogLabel, const QString &iLogMsg,bool appendBraces,
                                              const QStringList& iPirParameterList,QDomElement& oStateOnInvokeStates,const QString& iActionName,const QString &iComplementName, QString &oTxName)
{
    bool rc = false;
    if (!iPirParameterList.isEmpty())
    {

        rc = true;
        QDomElement  paramParentState= iDomDoc.createElement(SXN_STATE);
        //GTAAppSettings appSet;
        //QString pirWaitTime;
        //bool isPirWaitTimeSet = appSet.getPirWaitTime(pirWaitTime);
        
        bool isPirWaitTimeSet = TestConfig::getInstance()->getPIRWaitTime();
        QString pirWaitTime = QString::fromStdString(TestConfig::getInstance()->getPIRWaitTimeVal());

        paramParentState.setAttribute(SXA_ID,iStateID);
        QString paramParenStateFinalID = iStateID + QString("_FINAL");
        QString sParamParentStateID = iStateID;
        QString parentInitalID;
        QDomElement nextInvokeNode = iRefInvokeElem;
        int noOfParams = iPirParameterList.size();

        for (int i =0;i<noOfParams;i++)
        {
            QString varName = iPirParameterList.at(i);
            QDomElement  invokeSetStateNode= iDomDoc.createElement(SXN_STATE);//this will aggregate subscirbe state node, invoke state node and unsubscirbestate node respectively
            QString      sCurrentInvokeSetStateID = sParamParentStateID+QString("_invokeSet%1").arg(i);
            invokeSetStateNode.setAttribute(SXA_ID,sCurrentInvokeSetStateID);
            
            QString sSubscribeStateID   = sCurrentInvokeSetStateID + QString("_subscribe_%1").arg(QString::number(i));
            QString sUnSubscribeStateID = sCurrentInvokeSetStateID + QString("_unSubscribe_%1").arg(QString::number(i));
            QString sInvokeStateID      = sCurrentInvokeSetStateID + QString("_invoke_%1").arg(QString::number(i));
            QString sInvokeStateFinalNodeId=sCurrentInvokeSetStateID + QString("_invoke_FINAL%1").arg(QString::number(i));


            if (!i)
                parentInitalID = sCurrentInvokeSetStateID;
            //subscribe state creation before invoke  >>>>

            QDomElement subscribeState;
            QDomElement waitState = iDomDoc.createElement(SXN_STATE);

            invokeSetStateNode.setAttribute(SXA_INITIALSTATE,sSubscribeStateID);
            QString returnCode;
            getSubscribeStateForInvoke(iDomDoc,varName,sSubscribeStateID,subscribeState,returnCode);
            if (subscribeState.isNull()==false)
            {
                subscribeState.setAttribute(SXA_ID,sSubscribeStateID);
                QDomNodeList txnNodes = subscribeState.elementsByTagName(SXN_TRANSITION);
                if(txnNodes.isEmpty()==false)
                {
                    QDomElement tnxElement = txnNodes.at(0).toElement();
                    tnxElement.setAttribute(SXA_TARGET,QString("%1_wait").arg(sInvokeStateID));
                    QString txTrueCond = QString("%1.Status == 'OK'").arg(returnCode);
                    QString txFalseCond = QString("%1.Status != 'OK'").arg(returnCode);

                    QDomElement tnxElement1  = tnxElement.cloneNode().toElement();

                    tnxElement.setAttribute(SXA_CONDITION,txTrueCond);
                    tnxElement1.setAttribute(SXA_CONDITION,txFalseCond);
                    tnxElement1.setAttribute(SXA_TARGET,sInvokeStateFinalNodeId);

                    tnxElement.setAttribute(SXA_EVENT,"done.invoke._all");
                    tnxElement1.setAttribute(SXA_EVENT,"done.invoke._all");

                    subscribeState.insertAfter(tnxElement1,tnxElement);

                }
                invokeSetStateNode.appendChild(subscribeState);


                waitState.setAttribute(SXA_ID,QString("%1_wait").arg(sInvokeStateID));
                QDomElement OnEntryWaitElem = iDomDoc.createElement(SXN_ONENTRY);
                QDomElement sendElem = iDomDoc.createElement(SXN_SEND);
                sendElem.setAttribute(SXA_EVENT,QString("%1_wait_event").arg(sInvokeStateID));
                if(isPirWaitTimeSet)
                {
                    QString dealy = pirWaitTime+"ms";
                    sendElem.setAttribute(SXA_DELAY,dealy);
                }
                else
                {
                    sendElem.setAttribute(SXA_DELAY,"0ms");
                }
                OnEntryWaitElem.appendChild(sendElem);
                QDomElement txWaitElem =iDomDoc.createElement(SXN_TRANSITION);
                txWaitElem.setAttribute(SXA_TARGET,sInvokeStateID);
                txWaitElem.setAttribute(SXA_EVENT,QString("%1_wait_event").arg(sInvokeStateID));
                waitState.appendChild(OnEntryWaitElem);
                waitState.appendChild(txWaitElem);
                invokeSetStateNode.appendChild(waitState);
            }
            //<<<<


            QDomElement  invokeStateElem= iDomDoc.createElement(SXN_STATE);
            invokeStateElem.setAttribute(SXA_ID,sInvokeStateID);
            QDomElement invokeElem = iRefInvokeElem.cloneNode().toElement();
            QString invokeId=iStateID+QString("_invoke").arg(QString::number(i));
            invokeElem.setAttribute(SXA_ID,invokeId);
            invokeStateElem.appendChild(invokeElem);

            QDomElement transitionOnInvokeState = iDomDoc.createElement(SXN_TRANSITION);
            invokeStateElem.appendChild(transitionOnInvokeState);
            transitionOnInvokeState.setAttribute(SXA_TARGET,sUnSubscribeStateID);
            invokeSetStateNode.insertAfter(invokeStateElem,waitState);

            transitionOnInvokeState.setAttribute(SXA_EVENT,"done.invoke._all");


            //invoke node filling of info.
            GTASXFuncArgument objArg;
            objArg.Name = iFuncArgParam;
            objArg.Value = varName;
            GTASXFuncReturnVal objRetVal;
            objRetVal.Name = varName;

            GTASXAssign objAssign;
            objAssign.Location = varName;


            GTASXLog objLog;
            objLog.Label = iLogLabel;
            objLog.Expr = iLogMsg;
            QList<GTASXFuncArgument> argList; argList.append(objArg);
            setInvokeAttr(invokeElem,invokeId,argList,objRetVal,objAssign,objLog,appendBraces);
            QString txEvent1 = "";
            setInvokeDelay(iDomDoc,invokeElem,iActionName,iComplementName,txEvent1,1);//iPirParameterList.count());
            if(!txEvent1.isEmpty())
            {
                oTxName += " "+txEvent1;
            }


            //un-subscribe state creation before invoke  >>>>
            QString returnCode1;
            QDomElement unSubscribeState;
            getUnSubscribeStateForInvoke(iDomDoc,varName,unSubscribeState,sUnSubscribeStateID,returnCode1);
            if (unSubscribeState.isNull()==false)
            {
                unSubscribeState.setAttribute(SXA_ID,sUnSubscribeStateID);
                QDomNodeList txnNodes = unSubscribeState.elementsByTagName(SXN_TRANSITION);
                if(txnNodes.isEmpty()==false)
                {
                    QDomElement tnxElement = txnNodes.at(0).toElement();
                    tnxElement.setAttribute(SXA_TARGET,sInvokeStateFinalNodeId);
                    tnxElement.setAttribute(SXA_EVENT,"done.invoke._all");
                }
                invokeSetStateNode.insertAfter(unSubscribeState,invokeStateElem);
            }

            QDomElement finalElement = iDomDoc.createElement(SXN_FINAL);
            QDomElement oneEntryElem = iDomDoc.createElement(SXN_ONENTRY);
            QDomElement assignElement = iDomDoc.createElement(SXN_ASSIGN);
            assignElement.setAttribute(SXA_EXPR,"'Executed'");
            QString sLocation = QString("%1_invoke_done").arg(sCurrentInvokeSetStateID);
            assignElement.setAttribute(SXA_LOCATION,sLocation);
            oneEntryElem.appendChild(assignElement);
            finalElement.setAttribute(SXA_ID,sInvokeStateFinalNodeId);
            finalElement.appendChild(oneEntryElem);
            invokeSetStateNode.insertAfter(finalElement,unSubscribeState);

            //<<<<



            QDomElement setStateTxnNode = iDomDoc.createElement(SXN_TRANSITION);
            QString nextInvokeStateID ;
            if(i<noOfParams-1)
                nextInvokeStateID = sParamParentStateID+QString("_invokeSet%1").arg(i+1);
            else
                nextInvokeStateID = paramParenStateFinalID;
            invokeSetStateNode.insertAfter(setStateTxnNode,invokeSetStateNode.lastChild());
            setStateTxnNode.setAttribute(SXA_TARGET, nextInvokeStateID);
            setStateTxnNode.removeAttribute(SXA_EVENT);
            paramParentState.appendChild(invokeSetStateNode);

        }


        paramParentState.setAttribute(SXA_INITIALSTATE,parentInitalID);

        {
            QDomElement finalElement = iDomDoc.createElement(SXN_FINAL);
            QDomElement oneEntryElem = iDomDoc.createElement(SXN_ONENTRY);
            QDomElement assignElement = iDomDoc.createElement(SXN_ASSIGN);
            assignElement.setAttribute(SXA_EXPR,"'Executed'");
            QString sLocation = QString("%1_invoke_done").arg(iStateID);
            assignElement.setAttribute(SXA_LOCATION,sLocation);
            oneEntryElem.appendChild(assignElement);
            finalElement.setAttribute(SXA_ID,paramParenStateFinalID);
            finalElement.appendChild(oneEntryElem);
            paramParentState.insertAfter(finalElement,paramParentState.lastChild());
        }


        oStateOnInvokeStates= paramParentState;
    }
    return rc;

}
/**
  * Note: Preferably use createActionOnFail()
  * This function create followings for action of fail of a command,
  * Create OnEntry/Assign nodes if does not exist and provide time out event
  * Always create Transition of state , if action of fail is stop then transition happened to be final otherwise next state
  * iElemOnState: State element where time out to be implemented
  * iDomDoc: state xml is create in the context of this document
  * iRootElem: Element where the created transition & onEntry node is inserted
  * iSend: Attributes of send node of onenty node
  * iTransit: Attributes of transit node, Transition node for action on fail is always created
  * iTransitAssign: Attributes of assign node in transition node
  * iTransitLog: Attributes of log node in transition node
  */
void GTAExportScxml::setActionOnFailAttr(QDomDocument &iDomDoc, QDomElement &iElemOnState,
                                            const GTASXSend &, const GTASXTransition & iTransit,
                                            const GTASXAssign & iTransitAssign, const GTASXLog & iTransitLog,bool bCreateLog, bool isLoop, QString &oTimeoutEventFlag, const QString iDealyEventName)
{
    if(iElemOnState.isNull())
        return;

    QDomElement ElemOnEntry;
    bool isLoopTimeoutInc = TestConfig::getInstance()->getLoopTimeoutSubsteps();
    QDomNodeList onEntryNodeList = iElemOnState.elementsByTagName(SXN_ONENTRY);
    if(onEntryNodeList.isEmpty())
    {
        ElemOnEntry = iDomDoc.createElement(SXN_ONENTRY);
        iElemOnState.insertBefore(ElemOnEntry,iElemOnState.firstChild());
    }
    else
    {
        ElemOnEntry = onEntryNodeList.at(0).toElement();
    }

    if(! ElemOnEntry.isNull())
    {
        //QDomElement ElemOnSend;
        //QDomNodeList sendNodeList = ElemOnEntry.elementsByTagName(SXN_SEND);
        //if(! sendNodeList.isEmpty())
        //{
        //    ElemOnSend = sendNodeList.at(0).toElement();
        //}
        //else
        //{
        //    ElemOnSend = iDomDoc.createElement(SXN_SEND);
        //    ElemOnEntry.appendChild(ElemOnSend);
        //}
        //ElemOnSend.setAttribute(SXA_EVENT,iSend.Event);
        //ElemOnSend.setAttribute(SXA_DELAY,iSend.Delay);

        QDomElement ElemOnTransit = iDomDoc.createElement(SXN_TRANSITION);
        ElemOnTransit.setAttribute(SXA_TARGET,iTransit.Target);

        if(!isLoop)
            ElemOnTransit.setAttribute(SXA_EVENT,iTransit.Event);
        else
        {
            if(isLoopTimeoutInc)
            {
                oTimeoutEventFlag = QString("%1_%2").arg(iTransit.Event,"Flag");
                ElemOnTransit.setAttribute(SXA_CONDITION,QString("%1=='TimeOut'").arg(oTimeoutEventFlag));
                if(!iDealyEventName.isEmpty())
                    ElemOnTransit.setAttribute(SXA_EVENT,iDealyEventName);
            }
        }
        

        ElemOnTransit.setAttribute(SXA_STATUS,iTransit.status);

        if(isLoopTimeoutInc)
            iElemOnState.insertAfter(ElemOnTransit,iElemOnState.lastChild());
        else if(!isLoop)
            iElemOnState.insertAfter(ElemOnTransit,iElemOnState.lastChild());


        QDomElement ElemOnTransitAssign =iDomDoc.createElement(SXN_ASSIGN);
        ElemOnTransitAssign.setAttribute(SXA_EXPR,iTransitAssign.Expr);
        ElemOnTransitAssign.setAttribute(SXA_LOCATION,iTransitAssign.Location);
        ElemOnTransit.appendChild(ElemOnTransitAssign);

        GTAScxmlDataModel objDM;
        objDM.ID = iTransitAssign.Location;
        _DataModelList.insert(iTransitAssign.Location,objDM);

        if(bCreateLog)
        {
            QDomElement ElemOnTransitLog =iDomDoc.createElement(SXN_LOG);
            ElemOnTransitLog.setAttribute(SXA_EXPR,iTransitLog.Expr);
            ElemOnTransitLog.setAttribute(SXA_LABEL,iTransitLog.Label);
            ElemOnTransit.appendChild(ElemOnTransitLog);
        }

    }
}

/**
  * This function create followings for action of fail of a command
  * Create OnEntry/Assign nodes if does not exist and provide time out event
  * Always create Transition of state , if action of fail is stop then transition happened to be final otherwise next state
  * iElemOnState: State element where time out to be implemented
  * iStateId: State id of the command;
  * iTargetId: The next ID where the stat should go after execution
  * iActionOnFail: value of action on fail of the command
  * iTimeOutInMS: Time out value of the command, it should be in mili second, see the usage of getTimeOutInMiliSecond
  */
void GTAExportScxml::createActionOnFail(QDomDocument &iDomDoc, QDomElement &iElemOnState, const QString &iStateId,
                                           const QString &iTargetId, const QString &iActionOnFail, const QString &iTimeOutInMS,
                                           const QString &iLogMessage, bool IsDelayCheckCond,const QString & iDelayEventName,bool bCreateLog, bool isLoop, QString &oTimeoutEventFlag)
{
    GTASXSend objSend;
    objSend.Event = QString("%1_TimeOut_Event").arg(iStateId);
    objSend.Delay = iTimeOutInMS;
    GTASXTransition objTransitOnFail;
    objTransitOnFail.Target = iActionOnFail.contains("stop") ? MAIN_FINAL_STATE : iTargetId;
    objTransitOnFail.Event = objSend.Event;
    objTransitOnFail.status = "fail";
    GTASXAssign objTransitAssign;
    objTransitAssign.Expr = QString("'TimeOut'");
    objTransitAssign.Location = QString("%1_Status").arg(iStateId);

    GTASXLog objTransitLog;
    objTransitLog.Expr= iLogMessage;//QString("'|%1| Fail due to time out'").arg(iStateId);
    objTransitLog.Label="Error";

    setActionOnFailAttr(iDomDoc,iElemOnState,objSend,objTransitOnFail,objTransitAssign,objTransitLog,bCreateLog,isLoop,oTimeoutEventFlag,iDelayEventName);
    
    if(IsDelayCheckCond == true)
    {
        QDomNodeList onEntryNodeList = iElemOnState.elementsByTagName(SXN_ONENTRY);
        if(!onEntryNodeList.isEmpty())
        {
            QDomElement ElemOnEntry = onEntryNodeList.at(0).toElement();
            if(! ElemOnEntry.isNull())
            {
                QDomNodeList elementOnSendItems = ElemOnEntry.elementsByTagName(SXN_SEND);
                QDomElement ElemOnSend;
                if(elementOnSendItems.isEmpty())
                    ElemOnSend = iDomDoc.createElement(SXN_SEND);
                else
                    ElemOnSend = elementOnSendItems.at(0).toElement();
                ElemOnSend.setAttribute(SXA_EVENT,iDelayEventName);
                ElemOnSend.setAttribute(SXA_DELAY,"100ms");
                ElemOnEntry.appendChild(ElemOnSend);
            }
        }
    }
}
void GTAExportScxml::createActionOnFail(QDomDocument &iDomDoc, QDomElement &iElemOnState, const QString &iStateId,
                                           const QString &iTargetId, const QString &iActionOnFail, const QStringList &iTimeOutsInMS,
                                           const QString &iLogMessage, bool IsDelayCheckCond,const QString & iDelayEventName,bool bCreateLog, bool isLoop, QString &oTimeoutEventFlag)
{
    foreach (QString iTimeOutInMS,iTimeOutsInMS)
    {
        GTASXSend objSend;
        objSend.Event = QString("%1_TimeOut_Event").arg(iStateId);
        objSend.Delay = iTimeOutInMS;
        GTASXTransition objTransitOnFail;
        objTransitOnFail.Target = iActionOnFail.contains("stop") ? MAIN_FINAL_STATE : iTargetId;
        objTransitOnFail.Event = objSend.Event;
        objTransitOnFail.status = "fail";
        GTASXAssign objTransitAssign;
        objTransitAssign.Expr = QString("'TimeOut'");
        objTransitAssign.Location = QString("%1_Status").arg(iStateId);

        GTASXLog objTransitLog;
        objTransitLog.Expr= iLogMessage;//QString("'|%1| Fail due to time out'").arg(iStateId);
        objTransitLog.Label="Error";

        setActionOnFailAttr(iDomDoc,iElemOnState,objSend,objTransitOnFail,objTransitAssign,objTransitLog,bCreateLog,isLoop,oTimeoutEventFlag);
    }

    if(IsDelayCheckCond == true)
    {
        QDomNodeList onEntryNodeList = iElemOnState.elementsByTagName(SXN_ONENTRY);
        if(!onEntryNodeList.isEmpty())
        {
            QDomElement ElemOnEntry = onEntryNodeList.at(0).toElement();
            if(! ElemOnEntry.isNull())
            {
                QDomElement ElemOnSend = iDomDoc.createElement(SXN_SEND);
                ElemOnSend.setAttribute(SXA_EVENT,iDelayEventName);
                ElemOnSend.setAttribute(SXA_DELAY,"100ms");
                ElemOnEntry.appendChild(ElemOnSend);
            }
        }
    }
}
/**
  * This function create the additional state e.g. success or fail state associated with the parent state
  * iDomDoc : xml document in context the state to be created
  * iRootElem : Root element where the created state to be inserted
  * iAssign: Assign node to be set
  * iTransit: Transition node to be set
  * iLog : log node to set
  * iStateID: state id of the create state
  */
void GTAExportScxml::createTransitionChildren(QDomDocument & iDomDoc,QDomElement & iElemOnTransit, const GTASXAssign & iAssign, const GTASXLog & iLog,QString & iCondtion,QString & iCondLoc)
{

    QDomElement ElemOnAssign = iDomDoc.createElement(SXN_ASSIGN);
    ElemOnAssign.setAttribute(SXA_LOCATION,iAssign.Location);
    ElemOnAssign.setAttribute(SXA_EXPR,iAssign.Expr);
    iElemOnTransit.appendChild(ElemOnAssign);

    if(!iCondLoc.isEmpty())
    {
        QDomElement ElemOnAssignCond = iDomDoc.createElement(SXN_ASSIGN);
        ElemOnAssignCond.setAttribute(SXA_LOCATION,iCondLoc);
        ElemOnAssignCond.setAttribute(SXA_EXPR,iCondtion);
        iElemOnTransit.appendChild(ElemOnAssignCond);
    }

    GTAScxmlDataModel objDM;
    objDM.ID = iAssign.Location;
    _DataModelList.insert(objDM.ID,objDM);

    QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
    ElemOnLog.setAttribute(SXA_LABEL,iLog.Label);
    ElemOnLog.setAttribute(SXA_EXPR,iLog.Expr);
    iElemOnTransit.appendChild(ElemOnLog);
}

/**
  * This convert the time out value to mili second format e.g. 3 sec is converted to 3000.00ms
  */
QString GTAExportScxml::getTimeOutInMiliSecond(GTACommandBase * pCmd)
{
    QString oTimeInMS;
    if(pCmd == NULL)
        return oTimeInMS;

    QString timeOut, unit;
    if(pCmd->getCommandType() == GTACommandBase::ACTION)
    {
        GTAActionBase * pAction = dynamic_cast<GTAActionBase*>(pCmd);
        pAction->getTimeOut(timeOut,unit);
    }
    else if(pCmd->getCommandType() == GTACommandBase::CHECK)
    {
        GTACheckBase * pCheck = dynamic_cast<GTACheckBase*>(pCmd);
        pCheck->getTimeOut(timeOut,unit);
    }

    oTimeInMS = ((unit == ACT_TIMEOUT_UNIT_MSEC) ? QString::number(timeOut.toDouble(),'f',0):QString::number(timeOut.toDouble() * 1000,'f',0)) + "ms";
    return oTimeInMS;
}
/**
* This convert the conf time value to mili second format e.g. 3 sec is converted to 3000.00ms
*/
QString GTAExportScxml::getConfTimeInMiliSecond(GTACommandBase * pCmd)
{
    QString oTimeInMS = "0";
    if(pCmd == NULL)
        return oTimeInMS;

    QString timeOut, unit;
    if(pCmd->getCommandType() == GTACommandBase::ACTION)
    {
        GTAActionBase * pAction = dynamic_cast<GTAActionBase*>(pCmd);
        pAction->getConfCheckTime(timeOut,unit);
    }
    else if(pCmd->getCommandType() == GTACommandBase::CHECK)
    {
        GTACheckBase * pCheck = dynamic_cast<GTACheckBase*>(pCmd);
        pCheck->getConfCheckTime(timeOut,unit);
    }

    oTimeInMS = ((unit == ACT_TIMEOUT_UNIT_MSEC) ? QString::number(timeOut.toDouble(),'f',0):QString::number(timeOut.toDouble() * 1000,'f',0)) + "ms";
    return oTimeInMS;
}
/**
  * The function open the scxml template file
  * @iFileName : template file name with extension e.g. condition_if.scxml
  * @oContents : retrieve the template contents
  * return true if template open successfully otherwise false
  */
bool GTAExportScxml::openFile(const QString & iFileName, QByteArray & oContents)
{
    bool rc = false;
    QString scxmlTmplFilePath = QString("%1/%2").arg(_ScxmlTemplatePath,iFileName);
    QFile objFile(scxmlTmplFilePath);
    if(objFile.exists() && objFile.open(QFile::Text | QFile::ReadOnly))
    {
        oContents = objFile.readAll();
        rc = true;
    }
    return rc;
}
/**
  * The function replaces the tags which are used in template contents
  * @iTmplContents: template contents
  * @iTagToReplace: it is key-value pair where key is tag and value is tag value
  * return new contents replaced by tag value
  */
QByteArray GTAExportScxml::replaceTag(const QByteArray & iTmplContents,const QMap<QString,QString> & iTagToReplace )
{
    QByteArray oStateXmlContent = iTmplContents;
    foreach(QString tagName, iTagToReplace.keys())
    {
        QString tagValue = iTagToReplace.value(tagName);
        oStateXmlContent.replace(tagName,tagValue.toLatin1());

    }
    return oStateXmlContent;
}

/**
  * This function return the xml element created from the contents of scxml template file
  * @iXmlContents: scxml template contents
  * @iDocInContext: xml document in context where the element is created
  * return the dom element if successfully contents are created to xml otherwise null element is return
  */
QDomElement GTAExportScxml::getElement(QByteArray & iXmlContents,QDomDocument & iDocInContext)
{
    QDomElement oElem;
    if(! iXmlContents.isEmpty())
    {
        QDomDocument domDoc;
        if( domDoc.setContent(iXmlContents))
        {
            QDomElement rootElem = domDoc.documentElement();
            oElem = iDocInContext.importNode(rootElem,true).toElement();
        }
    }
    return oElem;
}
/**
  * This function create wait for buffer depth stat element
  * @iDomDoc: document in context where the stat is created
  * @iStatID: stat ID of this stat
  * @iTargetID: id of target stat
  * @iEvent : event on which the transition to next stat is take place
  * @iDelay : number of second the stat wait for
  * return the xml element of this stat
  */
QDomElement GTAExportScxml::createBufferDepthState(QDomDocument & iDomDoc,const QString & iStatID, const QString & iTargetID, const QString iEvent, const QString & iDelay)
{

    QDomElement oElem;
    if(! iDomDoc.isNull())
    {
        oElem = iDomDoc.createElement(SXN_STATE);
        oElem.setAttribute(SXA_ID,iStatID);

        QDomElement ElemOnEntry = iDomDoc.createElement(SXN_ONENTRY);
        QDomElement ElemOnSend = iDomDoc.createElement(SXN_SEND);
        ElemOnSend.setAttribute(SXA_EVENT,iEvent);
        ElemOnSend.setAttribute(SXA_DELAY,iDelay);
        ElemOnEntry.appendChild(ElemOnSend);
        QDomElement ElemOnTransition = iDomDoc.createElement(SXN_TRANSITION);
        ElemOnTransition.setAttribute(SXA_TARGET,iTargetID);
        ElemOnTransition.setAttribute(SXA_EVENT,iEvent);
        ElemOnTransition.setAttribute(SXA_STATUS,"success");

        oElem.appendChild(ElemOnEntry);
        oElem.appendChild(ElemOnTransition);
    }
    return oElem;
}

bool GTAExportScxml::createCheckAudioAlarm(const GTACheckAudioAlarm *pAudioAlarmCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId,const QString &iFalseStateId,const QString &iOperator)
{
    bool rc = false;
    if(pAudioAlarmCmd != NULL)
    {
        QString checkName = "check_Audio_Alarm";
        QString instanceName = pAudioAlarmCmd->getInstanceName();

        QString message = pAudioAlarmCmd->getAlarmName();


        QString actionOnFail = pAudioAlarmCmd->getActionOnFail();
        QStringList dumpListParams = pAudioAlarmCmd->getDumpList();

        bool isNumber = false;

        QString sWaitForBuffer = pAudioAlarmCmd->getTime();
        int waitForBuffer = sWaitForBuffer.toInt(&isNumber);
        if(isNumber == false)
            waitForBuffer = 0;

        QString templateFile  = checkName  + ".scxml";
        QString invokeId = QString(iStateId).replace('s','i');

        QByteArray tmplContents;
        rc = openFile(templateFile,tmplContents);

        if(rc && ! tmplContents.isEmpty())
        {

            QString failStatID;
            QString statID = iStateId + "_1";
            QString invID = invokeId + "_1";
            QString returnVal = QString("Return_%1").arg(invID);

            if (actionOnFail.contains("stop"))

            {
                failStatID=MAIN_FINAL_STATE;
            }
            else
                failStatID=iTargetId;


            setStateIDName(statID,pAudioAlarmCmd);

            QMap<QString,QString> tagToReplaceMap;
            tagToReplaceMap.insert("@StatID",statID);
            tagToReplaceMap.insert("@InvokeID",invID);
            tagToReplaceMap.insert("@ReturnVal",returnVal);
            tagToReplaceMap.insert("@Delay",QString::number(waitForBuffer));
            tagToReplaceMap.insert("@Sound",message);

            if((iOperator.isEmpty()) || (iOperator == ARITH_EQ))
            {
                tagToReplaceMap.insert("@Operator1","==");
                tagToReplaceMap.insert("@Operator2","!=");
            }
            else
            {
                tagToReplaceMap.insert("@Operator1","!=");
                tagToReplaceMap.insert("@Operator2","==");
            }
            //Log Node
            GTAScxmlLogMessage objLog;
            objLog.LOD = GTAScxmlLogMessage::Main;
            objLog.Identifier = instanceName.replace("\"", "");
            objLog.ExpectedValue = "OK";
            objLog.CurrentValue = "OK";
            objLog.Result = GTAScxmlLogMessage::OK;
            objLog.Status = GTAScxmlLogMessage::Success;
            QString successLogMsg = objLog.getLogMessage();

            successLogMsg = successLogMsg.remove("+");
            successLogMsg = successLogMsg.remove("'");
            successLogMsg = QString("'%1'").arg(successLogMsg);

            objLog.CurrentValue = "KO";
            objLog.Result = GTAScxmlLogMessage::KO;
            objLog.Status = GTAScxmlLogMessage::Fail;
            QString failLogMsg = objLog.getLogMessage();
            failLogMsg = failLogMsg.remove("+");
            failLogMsg = failLogMsg.remove("'");
            failLogMsg = QString("'%1'").arg(failLogMsg);

            tagToReplaceMap.insert("@TargetID1",iTargetId);
            if(iFalseStateId.isEmpty())
                tagToReplaceMap.insert("@TargetID2",failStatID);
            else
                tagToReplaceMap.insert("@TargetID2",iFalseStateId);
            
            tagToReplaceMap.insert("@LogMessage1",successLogMsg);
            tagToReplaceMap.insert("@LogMessage2",failLogMsg);



            QByteArray statElemContents = replaceTag(tmplContents,tagToReplaceMap);
            if(! statElemContents.isEmpty())
            {
                QDomElement ElemOnStat = getElement(statElemContents,iDomDoc);
                if(! ElemOnStat.isNull())
                {
                    QString delay = QString("%1s").arg(QString::number(waitForBuffer));
                    QString event = QString("%1_Delay").arg(iStateId);
                    QDomElement ElemOnBufferStat = createBufferDepthState(iDomDoc,iStateId,statID,event,delay);
                    if(! ElemOnBufferStat.isNull())
                    {
                        iRootElem.appendChild(ElemOnBufferStat);
                        iRootElem.appendChild(ElemOnStat);

                        GTAScxmlDataModel objDMReturn;
                        objDMReturn.ID = returnVal;
                        _DataModelList.insert(returnVal,objDMReturn);
                    }

                    //Logging all Parameters
                    QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
                    ElemOnStat.appendChild(ElemOnExit);
                    QStringList varList;
                    varList<<returnVal<<dumpListParams;
                    insertVariableOnExit(ElemOnExit,varList,iStateId,instanceName,iDomDoc);

                    //DUMPLIST INSERTION
                    QDomNodeList invokeNodes = ElemOnStat.elementsByTagName(SXN_INVOKE);
                    QDomElement invokeForDump;
                    for (int i=0;i<invokeNodes.size();i++)
                    {
                        QDomElement currentInvokeNode = invokeNodes.at(i).toElement();
                        QString currentID = currentInvokeNode.attribute(SXA_ID);
                        if (currentID == "FORDUMPLIST")
                        {
                            invokeForDump = currentInvokeNode;
                            break;
                        }
                    }
                    QStringList dumpListParameterForInvoke= getValidParametersToInvoke(dumpListParams);
                    if (!dumpListParameterForInvoke.isEmpty())
                    {
                        QString invokeID = invokeId+"DL";

                        createInvokeNodes(iDomDoc,ElemOnStat,invokeForDump,dumpListParameterForInvoke,invokeID,"ParameterID","Info",instanceName,false,pAudioAlarmCmd->getCheckName(),"");
                    }

                    GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pAudioAlarmCmd->getParent());
                    if(pParent != NULL)
                    { 

                        createLoopTimeOutTranstions(ElemOnStat,iDomDoc,statID,pParent);
                    }
                    createCallTimeOutTranstions(iDomDoc,ElemOnStat,(GTACommandBase*)pAudioAlarmCmd,failStatID);
                    ElemOnStat.removeChild(invokeForDump);
                }
            }


        }
    }
    return rc;
}

bool GTAExportScxml::createCheckBiteMessage(const GTACheckBiteMessage *pBiteMsgCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pBiteMsgCmd != NULL)
    {
        QString checkName = "check_BITE_Message";
        QString instanceName = pBiteMsgCmd->getInstanceName();

        QString message = pBiteMsgCmd->getMessage();
        if(message.contains("<"))
            message.replace("<","&lt;");
        QString wait_ack = pBiteMsgCmd->getWaitStatus()? "true" : "false";
        
        QString isDisplay = pBiteMsgCmd->getCondition();
        QString actionOnFail = pBiteMsgCmd->getActionOnFail();
        QString reportVal = pBiteMsgCmd->getOnReport();
        QStringList dumpListParams = pBiteMsgCmd->getDumpList();
        QString sWaitForBuffer = pBiteMsgCmd->getBuffDepth();
        bool isNumber = false;
        int waitForBuffer = sWaitForBuffer.toInt(&isNumber);
        int timeoutVal = 3000 + waitForBuffer*1000;
        QString timeout = QString::number(timeoutVal)+"ms";//HARDCODED default timeout //;getTimeOutInMiliSecond((GTACheckBiteMessage *)pBiteMsgCmd);
        QString sStateId = iStateId;
        QDomElement oTimeOutElem;
        QString TimeOutTarget = QString("%1_1").arg(sStateId);
        rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeout,TimeOutTarget);
        if (rc)
        {
            sStateId = TimeOutTarget;
            iRootElem.appendChild(oTimeOutElem);
        }


        QString msgType = "message";
        if(reportVal == "MCDU Display")
        {
            msgType = "MCDU_Message";
        }


        if(isNumber == false)
            waitForBuffer = 0;

        QString templateFile  = checkName  + ".scxml";
        QString invokeId = QString(sStateId).replace('s','i');

        QByteArray tmplContents;
        rc = openFile(templateFile,tmplContents);

        if(rc && ! tmplContents.isEmpty())
        {

            QString failStatID;
            QString statID = sStateId ;//+ "_1";
            QString invID = invokeId ;//+ "_1";
            QString returnVal = QString("Return_%1").arg(invID);

            if (actionOnFail.contains("stop"))
            {
                failStatID=MAIN_FINAL_STATE;
            }
            else
                failStatID=iTargetId;



            QMap<QString,QString> tagToReplaceMap;
            setStateIDName(statID,pBiteMsgCmd);
            tagToReplaceMap.insert("@StatID",statID);
            tagToReplaceMap.insert("@InvokeID",invID);
            tagToReplaceMap.insert("@ReturnVal",returnVal);
            tagToReplaceMap.insert("@BufferDepth",QString::number(waitForBuffer));
            tagToReplaceMap.insert("@Wait_ACK",wait_ack);
            tagToReplaceMap.insert("@Message",message);
            tagToReplaceMap.insert("@ReportType",msgType);

            //Log Node
            GTAScxmlLogMessage objLog;
            objLog.LOD = GTAScxmlLogMessage::Main;
            objLog.Identifier = instanceName.replace("\"", "");
            objLog.ExpectedValue = "OK";
            objLog.CurrentValue = "OK";
            objLog.Result = GTAScxmlLogMessage::OK;
            objLog.Status = GTAScxmlLogMessage::Success;
            QString successLogMsg = objLog.getLogMessage();
            successLogMsg = successLogMsg.remove("+");
            successLogMsg = successLogMsg.remove("'");
            successLogMsg = QString("'%1'").arg(successLogMsg);
            objLog.CurrentValue = "KO";
            objLog.Result = GTAScxmlLogMessage::KO;
            objLog.Status = GTAScxmlLogMessage::Fail;
            QString failLogMsg = objLog.getLogMessage();
            failLogMsg = failLogMsg.remove("+");
            failLogMsg = failLogMsg.remove("'");
            failLogMsg = QString("'%1'").arg(failLogMsg);
            if(isDisplay.isEmpty())
            {
                tagToReplaceMap.insert("@TargetID1",iTargetId);
                tagToReplaceMap.insert("@TargetID2",failStatID);
                tagToReplaceMap.insert("@LogMessage1",successLogMsg);
                tagToReplaceMap.insert("@LogMessage2",failLogMsg);
            }
            else
            {
                tagToReplaceMap.insert("@TargetID1",failStatID);
                tagToReplaceMap.insert("@TargetID2",iTargetId);
                tagToReplaceMap.insert("@LogMessage2",successLogMsg);
                tagToReplaceMap.insert("@LogMessage1",failLogMsg);
            }


            QByteArray statElemContents = replaceTag(tmplContents,tagToReplaceMap);
            if(! statElemContents.isEmpty())
            {
                QDomElement ElemOnStat = getElement(statElemContents,iDomDoc);
                if(! ElemOnStat.isNull())
                {
                    //                    QString delay = QString("%1s").arg(QString::number(waitForBuffer));
                    //                    QString event = QString("%1_Wait_For_Buffer_Depth").arg(sStateId);
                    //                    QDomElement ElemOnBufferStat = createBufferDepthState(iDomDoc,sStateId,statID,event,delay);
                    //                    if(! ElemOnBufferStat.isNull())
                    //                    {
                    //                        iRootElem.appendChild(ElemOnBufferStat);
                    iRootElem.appendChild(ElemOnStat);

                    GTAScxmlDataModel objDMReturn;
                    objDMReturn.ID = returnVal;
                    _DataModelList.insert(returnVal,objDMReturn);
                    //                    }

                    //Logging all Parameters
                    QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
                    ElemOnStat.appendChild(ElemOnExit);
                    QStringList varList;
                    varList<<returnVal<<dumpListParams;
                    insertVariableOnExit(ElemOnExit,varList,sStateId,instanceName,iDomDoc);

                    //DUMPLIST INSERTION
                    QDomNodeList invokeNodes = ElemOnStat.elementsByTagName(SXN_INVOKE);
                    QDomElement invokeForDump;
                    for (int i=0;i<invokeNodes.size();i++)
                    {
                        QDomElement currentInvokeNode = invokeNodes.at(i).toElement();
                        QString currentID = currentInvokeNode.attribute(SXA_ID);
                        if (currentID == "FORDUMPLIST")
                        {
                            invokeForDump = currentInvokeNode;
                            break;
                        }
                    }
                    QStringList dumpListParameterForInvoke= getValidParametersToInvoke(dumpListParams);
                    if (!dumpListParameterForInvoke.isEmpty())
                    {
                        QString invokeID = invokeId+"DL";

                        createInvokeNodes(iDomDoc,ElemOnStat,invokeForDump,dumpListParameterForInvoke,invokeID,"ParameterID","Info",instanceName,false,pBiteMsgCmd->getCheckName());
                    }

                    ElemOnStat.removeChild(invokeForDump);
                    GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pBiteMsgCmd->getParent());
                    if(pParent != NULL)
                    { 
                        createLoopTimeOutTranstions(ElemOnStat,iDomDoc,statID,pParent);
                    }
                    createCallTimeOutTranstions(iDomDoc,ElemOnStat,(GTACommandBase*)pBiteMsgCmd,failStatID);
                }
                GTAScxmlLogMessage objLog;
                objLog.LOD = GTAScxmlLogMessage::Main;
                objLog.Identifier    = pBiteMsgCmd->getInstanceName();
                objLog.ExpectedValue = pBiteMsgCmd->getStatement().replace("\n","  ");
                objLog.CurrentValue = "'TimeOut'";
                objLog.Status = GTAScxmlLogMessage::TimeOut;
                objLog.Result = GTAScxmlLogMessage::KO;
                QString logMsgTimeOut = objLog.getLogMessage();
                logMsgTimeOut = logMsgTimeOut.remove("+");
                logMsgTimeOut = logMsgTimeOut.remove("'");
                logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);
                createActionOnFail(iDomDoc,ElemOnStat,iStateId,iTargetId,actionOnFail,timeout,logMsgTimeOut,false,"");
            }


        }
    }
    return rc;
}


bool GTAExportScxml::createCheckFWCWrng(const GTACheckFwcWarning *pFwcWarnCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pFwcWarnCmd != NULL)
    {
        QString checkName = "check_FWC_Warning";
        QString instanceName = pFwcWarnCmd->getInstanceName();

        QString header = pFwcWarnCmd->getHeader();
        QString message = pFwcWarnCmd->getMessage();
        if(message.contains("<"))
            message.replace("<","&lt;");
        QString wait_ack = pFwcWarnCmd->getWaitAck() ? "true" : "false";
        QString isDisplay = pFwcWarnCmd->getCondition();
        QString actionOnFail = pFwcWarnCmd->getActionOnFail();
        QStringList dumpListParams = pFwcWarnCmd->getDumpList();
        
        QString conftimeInMiliSec = getConfTimeInMiliSecond((GTACheckFwcWarning*)pFwcWarnCmd);
        bool isNumber = false;
        QString sWaitForBuffer = pFwcWarnCmd->getWaitForBuffer();
        int waitForBuffer = sWaitForBuffer.toInt(&isNumber);
        int timeoutVal = waitForBuffer*1000 + 3000;
        QString timeout = QString::number(timeoutVal)+ "ms";//getTimeOutInMiliSecond((GTACheckFwcWarning*)pFwcWarnCmd);
        QString sStateId =  iStateId ;
        QDomElement oTimeOutElem;
        QString TimeOutTarget = QString("%1_1").arg(sStateId);
        rc = getTimeOutNode(iDomDoc,oTimeOutElem,sStateId,timeout,TimeOutTarget);
        if (rc)
        {
            sStateId = TimeOutTarget;
            iRootElem.appendChild(oTimeOutElem);
        }


        QString configTimeStateID;
        bool bConfigTimeExists=false;
        QDomElement ConfTimeStateElem;
        QDomElement elementOnConfTimeState;
        GTACheckValue checkValue;


        //        QString sStateId = iStateId;
        QString confTimeTarget = QString("ConfTime%1_1").arg(sStateId);
        QString ConfigNodeID = QString("conf_delay_%1").arg(sStateId);
        QString confTimeEventName;
        bConfigTimeExists = getConfigurationTimeNode(iDomDoc,ConfTimeStateElem,ConfigNodeID,conftimeInMiliSec,confTimeTarget,confTimeEventName);
        


        if(isNumber == false)
            waitForBuffer = 0;

        QString color = pFwcWarnCmd->getFWCWarningColor();

        QString templateFile  = checkName  + ".scxml";
        QString invokeId = QString(sStateId).replace('s','i');

        QByteArray tmplContents;
        rc = openFile(templateFile,tmplContents);

        if(rc && ! tmplContents.isEmpty())
        {

            QString failStatID;
            QString statID = sStateId ;//+ "_1";
            QString invID = invokeId ;//+ "_1";
            QString returnVal = QString("Return_%1").arg(invID);

            if (actionOnFail.contains("stop"))
            {
                failStatID=MAIN_FINAL_STATE;
            }
            else
                failStatID=iTargetId;



            QMap<QString,QString> tagToReplaceMap;
            setStateIDName(statID,pFwcWarnCmd);
            tagToReplaceMap.insert("@StatID",statID);
            tagToReplaceMap.insert("@InvokeID",invID);
            tagToReplaceMap.insert("@ReturnVal",returnVal);
            tagToReplaceMap.insert("@BufferDepth",QString::number(waitForBuffer));
            tagToReplaceMap.insert("@Wait_ACK",wait_ack);
            //tagToReplaceMap.insert("@Header",header);
            tagToReplaceMap.insert("@Message",(QString("%1 %2").arg(header,message)).trimmed());
            tagToReplaceMap.insert("@Color",color);

            //Log Node
            GTAScxmlLogMessage objLog;
            objLog.LOD = GTAScxmlLogMessage::Main;
            objLog.Identifier = instanceName.replace("\"", "");
            objLog.ExpectedValue = "OK";
            objLog.CurrentValue = "OK";
            objLog.Result = GTAScxmlLogMessage::OK;
            objLog.Status = GTAScxmlLogMessage::Success;
            QString successLogMsg = objLog.getLogMessage();

            successLogMsg = successLogMsg.remove("+");
            successLogMsg = successLogMsg.remove("'");
            successLogMsg = QString("'%1'").arg(successLogMsg);

            objLog.CurrentValue = "KO";
            objLog.Result = GTAScxmlLogMessage::KO;
            objLog.Status = GTAScxmlLogMessage::Fail;
            QString failLogMsg = objLog.getLogMessage();
            failLogMsg = failLogMsg.remove("+");
            failLogMsg = failLogMsg.remove("'");
            failLogMsg = QString("'%1'").arg(failLogMsg);

            if(isDisplay.isEmpty())
            {
                if(bConfigTimeExists)
                    tagToReplaceMap.insert("@TargetID1",ConfigNodeID);
                else
                    tagToReplaceMap.insert("@TargetID1",iTargetId);
                tagToReplaceMap.insert("@TargetID2",failStatID);
                tagToReplaceMap.insert("@LogMessage1",successLogMsg);
                tagToReplaceMap.insert("@LogMessage2",failLogMsg);
            }
            else
            {
                tagToReplaceMap.insert("@TargetID1",failStatID);
                if(bConfigTimeExists)
                    tagToReplaceMap.insert("@TargetID2",ConfigNodeID);
                else
                    tagToReplaceMap.insert("@TargetID2",iTargetId);
                
                tagToReplaceMap.insert("@LogMessage2",successLogMsg);
                tagToReplaceMap.insert("@LogMessage1",failLogMsg);
            }

            QByteArray statElemContents = replaceTag(tmplContents,tagToReplaceMap);
            if(! statElemContents.isEmpty())
            {
                QDomElement ElemOnStat = getElement(statElemContents,iDomDoc);
                if(! ElemOnStat.isNull())
                {
                    //                    QString delay = QString("%1s").arg(QString::number(waitForBuffer));
                    //                    QString event = QString("%1_Wait_For_Buffer_Depth").arg(sStateId);
                    //                    QDomElement ElemOnBufferStat = createBufferDepthState(iDomDoc,sStateId,statID,event,delay);
                    //                    if(! ElemOnBufferStat.isNull())
                    //                    {
                    //                        iRootElem.appendChild(ElemOnBufferStat);
                    iRootElem.appendChild(ElemOnStat);

                    GTAScxmlDataModel objDMReturn;
                    objDMReturn.ID = returnVal;
                    _DataModelList.insert(returnVal,objDMReturn);
                    //                    }

                    //Logging all Parameters
                    QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
                    ElemOnStat.appendChild(ElemOnExit);
                    QStringList varlist;
                    varlist.append(returnVal);
                    varlist.append(dumpListParams);
                    insertVariableOnExit(ElemOnExit,varlist,sStateId,instanceName,iDomDoc);

                    //DUMPLIST INSERTION
                    QDomNodeList invokeNodes = ElemOnStat.elementsByTagName(SXN_INVOKE);
                    QDomElement invokeForDump;
                    for (int i=0;i<invokeNodes.size();i++)
                    {
                        QDomElement currentInvokeNode = invokeNodes.at(i).toElement();
                        QString currentID = currentInvokeNode.attribute(SXA_ID);
                        if (currentID == "FORDUMPLIST")
                        {
                            invokeForDump = currentInvokeNode;
                            break;
                        }
                    }
                    QStringList dumpListParameterForInvoke= getValidParametersToInvoke(dumpListParams);
                    if (!dumpListParameterForInvoke.isEmpty())
                    {
                        QString invokeID = sStateId.replace("s","i_DL");

                        createInvokeNodes(iDomDoc,ElemOnStat,invokeForDump,dumpListParameterForInvoke,invokeID,"ParameterID","Info",instanceName,false,pFwcWarnCmd->getCheckName());
                    }

                    ElemOnStat.removeChild(invokeForDump);


                    GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pFwcWarnCmd->getParent());

                    GTAScxmlLogMessage objLog;
                    objLog.LOD = GTAScxmlLogMessage::Main;
                    objLog.Identifier    = pFwcWarnCmd->getInstanceName();
                    objLog.ExpectedValue = pFwcWarnCmd->getStatement().replace("\n","  ");
                    objLog.CurrentValue = "'TimeOut'";
                    objLog.Status = GTAScxmlLogMessage::TimeOut;
                    objLog.Result = GTAScxmlLogMessage::KO;
                    QString logMsgTimeOut = objLog.getLogMessage();
                    logMsgTimeOut = logMsgTimeOut.remove("+");
                    logMsgTimeOut = logMsgTimeOut.remove("'");
                    logMsgTimeOut = QString("'%1'").arg(logMsgTimeOut);
                    createActionOnFail(iDomDoc,ElemOnStat,iStateId,iTargetId,actionOnFail,timeout,logMsgTimeOut,false,"");

                    if (bConfigTimeExists)
                    {
                        QDomElement confElemChk = ElemOnStat.cloneNode().toElement();
                        iRootElem.appendChild(ConfTimeStateElem);
                        iRootElem.appendChild(confElemChk);
                        setStateAttr(confElemChk,confTimeTarget);

                        QDomNodeList transitionElems = confElemChk.elementsByTagName(SXN_TRANSITION);
                        for(int i=0;i<transitionElems.size();i++)
                        {

                            QDomElement currElemForTXN = transitionElems.at(i).toElement();
                            if(currElemForTXN.attribute(SXA_STATUS)=="success")
                            {
                                currElemForTXN.setAttribute(SXA_TARGET,iTargetId);
                                currElemForTXN.setAttribute(SXA_EVENT,confTimeEventName);
                            }

                        }

                        if(pParent != NULL)
                        { 
                            createLoopTimeOutTranstions(confElemChk,iDomDoc,confTimeTarget,pParent);
                        }
                        
                        
                    }
                    if(pParent != NULL)
                    { 
                        createLoopTimeOutTranstions(ElemOnStat,iDomDoc,statID,pParent);
                    }

                    createCallTimeOutTranstions(iDomDoc,ElemOnStat,(GTACommandBase*)pFwcWarnCmd,failStatID);
                } 
                

            }

            //DUMPLIST INSERTION


        }
    }
    return rc;
}
/**
  * This funcation insert a log node in OnExit Element
  * iElemOnExit : Where the log element node to insterted
  * iVarList : List of variable to be logged
  * iStatId: Id of stat for which the logging is being done
  * iCmdInstanceName: instance name of command
  * iDomDoc : Document in Context
  */
void GTAExportScxml::insertVariableOnExit(QDomElement & iElemOnExit, const QStringList & iVarList, const QString &, const QString iCmdInstanceName, QDomDocument & iDomDoc)
{
    //Logging all Parameters
    QStringList VarList = iVarList;
    VarList.removeDuplicates();
    foreach(QString param, VarList)
    {
        if(param.isEmpty())
            continue;

        bool isNumber =false;
        param.toDouble(&isNumber);
        if(isNumber || param.startsWith("\"") || param.startsWith("\'"))
            continue;

        GTAScxmlLogMessage objLog;
        objLog.LOD = GTAScxmlLogMessage::Detail;
        objLog.Identifier    = iCmdInstanceName;
        objLog.ExpectedValue = param;
        objLog.CurrentValue = param;//QString("' + %1 + '").arg(param);
        objLog.Status = GTAScxmlLogMessage::None;
        objLog.Result = GTAScxmlLogMessage::kNa;

        QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
        ElemOnLog.setAttribute(SXA_LABEL,"'info'");
        ElemOnLog.setAttribute(SXA_EXPR,objLog.getLogMessage());
        iElemOnExit.appendChild(ElemOnLog);
    }
}
bool GTAExportScxml::createActionMathsState(const GTAActionMaths *pCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc= false;
    if(pCmd != NULL)
    {
        QString sComplement = pCmd->getComplement();
        QString sActionName = pCmd->getAction();
        QString sParam1 = pCmd->getFirstVariable();

        QString fileName = QString("%1.scxml").arg(sActionName);
        QString tempStateId = iStateId;

        QDomElement oElem;
        QDomElement invokeElement;
        QStringList varList;
        //pCmd->getVariableList(varList);
        varList = pCmd->getVariableList();

        rc = getCloneRootElement(fileName,iDomDoc,oElem);
        rc = getCloneRootElement("invoke.scxml",iDomDoc,invokeElement);
        setStateIDName(iStateId,pCmd);

        oElem.setAttribute(SXA_ID,iStateId);

        iRootElem.appendChild(oElem);

        QString sExprOnExit = pCmd->getSCXMLExpr();
        QDomElement assignElemOnExit;

        QDomElement onExitElem = oElem.namedItem(SXN_ONEXIT).toElement();
        QString sLocationForExit = sParam1;
        if(!onExitElem.isNull())
        {
            assignElemOnExit = iDomDoc.createElement(SXN_ASSIGN);
            varList.append(sParam1);
            onExitElem.appendChild(assignElemOnExit);
        }
        QDomElement transitionElem = oElem.namedItem(SXN_TRANSITION).toElement();
        QString sTargetId = iTargetId;


        GTAScxmlLogMessage objLog;
        QString identifier = pCmd->getInstanceName();
        identifier = identifier.replace("+","$");
        objLog.LOD = GTAScxmlLogMessage::Main;
        objLog.Identifier    = identifier;
        objLog.ExpectedValue = QString("%1").arg(identifier.remove("'"));//"true";
        objLog.CurrentValue = QString("^@%1@^").arg(sParam1);
        objLog.Status = GTAScxmlLogMessage::Success;
        objLog.Result = GTAScxmlLogMessage::OK;

        QString logMsg = objLog.getLogMessage();


        logMsg = logMsg.replace("'","");
        logMsg = logMsg.replace("+","");

        logMsg = logMsg.replace("$","+");
        logMsg = logMsg.replace("^@","'+");
        logMsg = logMsg.replace("@^","+'");

        QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
        ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
        ElemOnTrueLog.setAttribute(SXA_EXPR,QString("'%1'").arg(logMsg));
        transitionElem.appendChild(ElemOnTrueLog);
        varList.removeDuplicates();
        bool createSetParam=false;
        bool containsParamInvokes = false;
        QStringList paramInvokeList;
        foreach(QString var, varList)
        {
            if(!var.isEmpty())
            {
                bool isNotValid = false;
                var.toDouble(&isNotValid);
                if(var.contains("\"") || var.contains("'"))
                    isNotValid = true;
                if(!isNotValid)
                {
                    bool rc = insertValidAppDataModel(var);

                    if (rc && !invokeElement.isNull())
                    {
                        paramInvokeList.append(var);
                        sExprOnExit.replace(var,QString("%1.Value").arg(var));

                        if(var==sParam1)
                        {
                            sLocationForExit = QString ("LocalVar_%1").arg(iStateId);
                            createSetParam = true;
                            continue;
                        }
                        else
                        {
                            containsParamInvokes = true;
                        }
                        QDomNode invokeNode = invokeElement.cloneNode();
                        QDomElement tempElem =invokeNode.toElement();
                        QDomElement argElem = tempElem.elementsByTagName(SXN_ARGUMENT).at(0).toElement();
                        if(!argElem.isNull())
                            argElem.setAttribute(SXA_VALUE,QString("'%1'").arg(var));

                        QDomElement assignElem = tempElem.elementsByTagName(SXN_ASSIGN).at(0).toElement();
                        QString var1 = var;
                        var1 = var1.replace(".","_");
                        if(!assignElem.isNull())
                        {
                            assignElem.setAttribute(SXA_LOCATION,QString("%1").arg(var));
                            assignElem.setAttribute(SXA_EXPR,QString("_event.data.Return_%1").arg(var1));
                        }
                        QDomElement retElem = tempElem.elementsByTagName("returnvalue").at(0).toElement();
                        if(!retElem.isNull())
                        {
                            retElem.setAttribute(SXA_NAME,QString("Return_%1").arg(var1));
                        }
                        oElem.appendChild(tempElem);
                    }
                    else
                    {
                        GTAScxmlDataModel data;
                        data.ID = var;
                        _DataModelList.insert(var,data);
                    }
                }

            }
        }

        assignElemOnExit.setAttribute(SXA_EXPR,sExprOnExit);
        assignElemOnExit.setAttribute(SXA_LOCATION,sLocationForExit);

        if(createSetParam)
        {
            GTAActionSetList* setCommand = new GTAActionSetList;
            setCommand->setAction(ACT_SET);
            setCommand->setInstanceName(pCmd->getInstanceName());
            setCommand->setTimeOut("3","sec");
            GTAActionSet* setInstance = new GTAActionSet;
            QList<GTAActionSet*> setList;
            setList.append(setInstance);
            GTAEquationConst* constEqn = new GTAEquationConst;
            constEqn->setConstant(sLocationForExit);
            GTAEquationBase* pBase = constEqn;
            setInstance->setEquation(pBase);
            setInstance->setIsSetOnlyFSFixed(false);
            setInstance->setIsSetOnlyFSVariable(false);
            //setInstance.setFunctionStatus(fs_N)
            setInstance->setAction(ACT_SET_INSTANCE);
            setInstance->setParameter(sParam1);
            setCommand->setSetCommandlist(setList);

            sTargetId=QString("MathSet_%1").arg(iStateId);
            createActionSetState(setCommand,iDomDoc,iRootElem,sTargetId,iTargetId);
            delete setCommand;

        }
        transitionElem.setAttribute(SXA_TARGET,sTargetId);
        if(containsParamInvokes)
            transitionElem.setAttribute(SXA_EVENT,"done.invoke._all");

        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
        if(pParent != NULL)
        {
            createLoopTimeOutTranstions(oElem,iDomDoc,iStateId,pParent);
        }
        else
        {
            QDomElement entryElem = iDomDoc.createElement(SXN_ONENTRY);
            QDomElement sendElem = iDomDoc.createElement(SXN_SEND);
            sendElem.setAttribute(SXA_EVENT,QString("%1_dummytimeout").arg(iStateId));
            sendElem.setAttribute(SXA_DELAY,"3000ms");
            entryElem.appendChild(sendElem);
            oElem.appendChild(entryElem);

            QDomElement timeoutTx = iDomDoc.createElement(SXN_TRANSITION);
            timeoutTx.setAttribute(SXA_TARGET,iTargetId);
            timeoutTx.setAttribute(SXA_STATUS,"fail");
            timeoutTx.setAttribute(SXA_EVENT,QString("%1_dummytimeout").arg(iStateId));

            GTAScxmlLogMessage objLogOnExit;
            objLogOnExit.LOD = GTAScxmlLogMessage::Main;
            objLogOnExit.Identifier    = pCmd->getInstanceName();
            objLogOnExit.ExpectedValue = "";
            objLogOnExit.CurrentValue = "";//QString("'+%1+'").arg(sParameterName);
            objLogOnExit.Status = GTAScxmlLogMessage::TimeOut;
            objLogOnExit.Result = GTAScxmlLogMessage::KO;

            QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
            ElemOnLog.setAttribute(SXA_LABEL,"'info'");
            ElemOnLog.setAttribute(SXA_EXPR,objLogOnExit.getLogMessage());

            timeoutTx.appendChild(ElemOnLog);
            oElem.appendChild(timeoutTx);

        }

        createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pCmd,iTargetId);
    }
    return rc;
}
bool GTAExportScxml::createActionMathState(const GTAActionMath *pCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc= false;
    if(pCmd != NULL)
    {
        QString sComplement = pCmd->getComplement();
        QString sActionName = pCmd->getAction();
        QString sParam1 = pCmd->getFirstVariable();
        QString sParam2 = pCmd->getSecondVariable();
        QString sLocation = pCmd->getLocation();
        QString sOperator = pCmd->getOperator();
        QString fileName = QString("%1.scxml").arg(sActionName);
        QString tempStateId = iStateId;

        QDomElement oElem;

        QStringList varList;
        varList.append(sParam1);
        varList.append(sParam2);
        varList.append(sLocation);

        rc = getCloneRootElement(fileName,iDomDoc,oElem);

        setStateIDName(iStateId,pCmd);

        oElem.setAttribute(SXA_ID,iStateId);

        iRootElem.appendChild(oElem);

        QDomElement onEntryElem = oElem.namedItem(SXN_ONENTRY).toElement();
        if(!onEntryElem.isNull())
        {
            if(sComplement == COM_MATHS_ABS)
            {
                QDomElement assignElem = iDomDoc.createElement(SXN_ASSIGN);
                QString tempLocation =QString("%1_%2").arg(tempStateId,sParam1);
                varList.append(tempLocation);
                assignElem.setAttribute(SXA_LOCATION,tempLocation);
                QString expr = QString("sign(%1)").arg(sParam1);
                assignElem.setAttribute(SXA_EXPR,expr);
                onEntryElem.appendChild(assignElem);
                QDomElement absAssignElem = iDomDoc.createElement(SXN_ASSIGN);
                expr = QString("%1*%2").arg(tempLocation,sParam1);
                absAssignElem.setAttribute(SXA_LOCATION,sLocation);
                absAssignElem.setAttribute(SXA_EXPR,expr);
                onEntryElem.appendChild(absAssignElem);
            }
            else if(sComplement == COM_MATHS_POW)
            {
                QDomElement absAssignElem = iDomDoc.createElement(SXN_ASSIGN);
                QString expr = QString("Maths.pow(%1,%2)").arg(sParam1,sParam2);
                absAssignElem.setAttribute(SXA_LOCATION,sLocation);
                absAssignElem.setAttribute(SXA_EXPR,expr);
                onEntryElem.appendChild(absAssignElem);
            }
            else if(sComplement == COM_MATHS_INCREMENT)
            {
                QDomElement absAssignElem = iDomDoc.createElement(SXN_ASSIGN);
                QString expr = QString("%1+%2").arg(sParam1,"1");
                absAssignElem.setAttribute(SXA_LOCATION,sParam1);
                absAssignElem.setAttribute(SXA_EXPR,expr);
                onEntryElem.appendChild(absAssignElem);
            }
            else if(sComplement == COM_MATHS_BINARY)
            {
                QDomElement absAssignElem = iDomDoc.createElement(SXN_ASSIGN);
                QString expr = QString("%1%2%3").arg(sParam1,sOperator,sParam2);
                absAssignElem.setAttribute(SXA_LOCATION,sLocation);
                absAssignElem.setAttribute(SXA_EXPR,expr);
                onEntryElem.appendChild(absAssignElem);
            }
        }
        QDomElement transitionElem = oElem.namedItem(SXN_TRANSITION).toElement();
        transitionElem.setAttribute(SXA_TARGET,iTargetId);

        GTAScxmlLogMessage objLog;
        objLog.LOD = GTAScxmlLogMessage::Main;
        objLog.Identifier    = pCmd->getInstanceName();
        objLog.ExpectedValue = QString("%1").arg(pCmd->getInstanceName().remove("'"));//"true";
        objLog.CurrentValue = "OK";
        objLog.Status = GTAScxmlLogMessage::Success;
        objLog.Result = GTAScxmlLogMessage::OK;
        QString logMsg = objLog.getLogMessage();

        logMsg = logMsg.replace("'","");
        logMsg = logMsg.replace("+","");


        QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
        ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
        ElemOnTrueLog.setAttribute(SXA_EXPR,QString("'%1'").arg(logMsg));
        transitionElem.appendChild(ElemOnTrueLog);
        foreach(QString var, varList)
        {
            if(!var.isEmpty())
            {
                bool isNotValid = false;
                var.toDouble(&isNotValid);
                if(var.contains("\"") || var.contains("'"))
                    isNotValid = true;
                if(!isNotValid)
                {
                    GTAScxmlDataModel data;
                    data.ID = var;
                    _DataModelList.insert(var,data);
                }
            }
        }
        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pCmd->getParent());
        if(pParent != NULL)
        { 
            createLoopTimeOutTranstions(oElem,iDomDoc,iStateId,pParent);
        }

    }
    return rc;
}
/**
* This function create the scxml state for failure command
* @pFailureCmd : Command for which the state to be created
* @iDomDoc: state xml is create in the context of this document
* @iRootElem: Element where the created state is inserted
* @iStateId: State id of the command;
* @iTargetId: The next ID where the stat should go after execution
* return true if scxml of the command is created, otherwise false
*/

bool GTAExportScxml::createActionFailureState(const GTAActionFailure *pFailureCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc =false;
    if(pFailureCmd != NULL)
    {
        QString sActionName = pFailureCmd->getAction();
        QString fileName = QString("%1.scxml").arg(pFailureCmd->getAction());
        QDomElement stateElem;
        rc = getCloneRootElement(fileName,iDomDoc,stateElem);
        if (rc && stateElem.isNull()==false)
        {
            GTAActionFailure::Failure_Type sFailueType = pFailureCmd->getFailureType();
            QString sFailureName = pFailureCmd->getFailureName();

            //avoid symbols in failure command
            QRegExp rx("([~.:%!&;,*\\^?|/\\\\@#$:\\-\\+{}\\[\\]<>'\"\\s\\(\\)]+)");
            sFailureName.replace(rx,"_");        
            sFailureName.replace("\n","_");
            /////////////////


            setStateIDName(iStateId,pFailureCmd);
            stateElem.setAttribute(SXA_ID,iStateId);
            QString invokeId = iStateId;
            invokeId = invokeId.replace("s","i");
            QDomElement invokeElem = stateElem.elementsByTagName(SXN_INVOKE).at(0).toElement();
            invokeElem.setAttribute(SXA_ID,invokeId);
            QString retunExpr = QString("%1_%2_failure").arg(iStateId,sFailureName);

            QDomElement functionElem = stateElem.elementsByTagName(SXN_FUNCTION).at(0).toElement();
            if((sFailueType == GTAActionFailure::START) ||(sFailueType == GTAActionFailure::RESUME))
                functionElem.setAttribute(SXA_NAME,"Failure.run");
            else if(sFailueType == GTAActionFailure::PAUSE)
                functionElem.setAttribute(SXA_NAME,"Failure.hold");
            else if(sFailueType == GTAActionFailure::STOP)
                functionElem.setAttribute(SXA_NAME,"Failure.unload");
            
            QDomElement argumElem = stateElem.elementsByTagName(SXN_ARGUMENT).at(0).toElement();
            if(!argumElem.isNull())
                argumElem.setAttribute(SXA_VALUE,QString("failure_list[%1].objectIdStructure").arg(sFailureName));

            QDomElement assignElem = stateElem.elementsByTagName(SXN_ASSIGN).at(0).toElement();
            if(!assignElem.isNull())
            {
                assignElem.setAttribute(SXA_LOCATION,QString("failure_list[%1].status").arg(sFailureName));
                assignElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(retunExpr));
            }

            QDomElement returnElem = stateElem.elementsByTagName(SXN_RETURNVALUE).at(0).toElement();
            if(!returnElem.isNull())

            {    returnElem.setAttribute(SXA_NAME,retunExpr);
            }


            QDomElement ElemOnTransit= stateElem.elementsByTagName(SXN_TRANSITION).at(0).toElement();

            QString instanceName = pFailureCmd->getInstanceName();

            GTAScxmlLogMessage objLogOnExit;
            objLogOnExit.LOD = GTAScxmlLogMessage::Main;
            objLogOnExit.Identifier    = instanceName;
            objLogOnExit.ExpectedValue = "";
            //objLogOnExit.CurrentValue =  sParameterName;
            objLogOnExit.Status = GTAScxmlLogMessage::Success;
            objLogOnExit.Result = GTAScxmlLogMessage::OK;

            QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
            ElemOnLog.setAttribute(SXA_LABEL,"'info'");
            //           logForCallTimeout = objLogOnExit;
            QString logMsg2 = objLogOnExit.getLogMessage();
            logMsg2 = logMsg2.remove("+");
            logMsg2 = logMsg2.remove("'");
            ElemOnLog.setAttribute(SXA_EXPR,QString("'%1'").arg(logMsg2));
            ElemOnTransit.appendChild(ElemOnLog);
            ElemOnTransit.setAttribute(SXA_TARGET,iTargetId);

            iRootElem.appendChild(stateElem);

            //time out in case if parent is a while or do while statement
            GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pFailureCmd->getParent());
            if(pParent != NULL)
            {
                createLoopTimeOutTranstions(stateElem,iDomDoc,iStateId,pParent);
            }
            createCallTimeOutTranstions(iDomDoc,stateElem,(GTACommandBase*)pFailureCmd,iTargetId);


        }

    }
    return rc;
}



/**
  * This function create the scxml state for release command
  * @pReleaseCmd : Command for which the state to be created
  * @iDomDoc: state xml is create in the context of this document
  * @iRootElem: Element where the created state is inserted
  * @iStateId: State id of the command;
  * @iTargetId: The next ID where the stat should go after execution
  * return true if scxml of the command is created, otherwise false
  */
bool GTAExportScxml::createActionReleaseState(const GTAActionRelease *pReleaseCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if (pReleaseCmd != NULL)
    {
        QString fileName = QString("%1_%2.scxml").arg(pReleaseCmd->getAction(),pReleaseCmd->getComplement());


        QStringList ListOfVarToLog;
        QStringList paramList = pReleaseCmd->getParameterList();

        QString TrueExpr;
        QString FalseExpr;
        QStringList ParamListForInvoke;
        foreach(QString param, paramList)
        {
            VariableType varType = IsTypeOf(param);
            if(varType == Unknown)
            {
                rc = insertValidAppDataModel(param);
                if(rc)
                {
                    ParamListForInvoke.append(param);

                    if(TrueExpr.isEmpty())
                        TrueExpr = QString("(%1.Status == '%2')").arg(param, "OK");
                    else
                        TrueExpr += QString("%3 (%1.Status == '%2')").arg(param, "OK", "&&");


                    if(FalseExpr.isEmpty())
                        FalseExpr = QString("(%1.Status == '%2')").arg(param, "KO");
                    else
                        FalseExpr += QString("%3 (%1.Status == '%2')").arg(param, "KO", "||");
                }
                else
                {
                    GTAScxmlDataModel objDM;
                    objDM.ID = param;
                    _DataModelList.insert(param,objDM);
                }

                ListOfVarToLog.append(param);
            }
        }


        QDomElement oElem;
        rc = getCloneRootElement(fileName,iDomDoc,oElem);
        if(rc == false || oElem.isNull())
            return false;

        setStateIDName(iStateId,pReleaseCmd);
        iRootElem.appendChild(oElem);
        setStateAttr(oElem,iStateId);

        QMap<QString, QDomElement> StatChildXPathMap;
        getScxmlElementPathMap(oElem,oElem.nodeName(),StatChildXPathMap);

        QString invokeXPath = getStateInvokeXPath();
        QDomElement invokeElem = StatChildXPathMap.value(invokeXPath);


        QString invokeID = QString(iStateId).replace("s","i");
        //ParamListForInvoke.removeDuplicates();
        createInvokeNodes(iDomDoc,oElem,invokeElem,ParamListForInvoke,invokeID,"ParameterID","info",pReleaseCmd->getInstanceName(),true,pReleaseCmd->getAction(),pReleaseCmd->getComplement());
        oElem.removeChild(invokeElem);

        //Transition
        QDomElement ElemOnTrueTransit = StatChildXPathMap.value(getStateTransistionXPath());

        GTASXTransition objSuccTransit;
        objSuccTransit.Target = iTargetId;
        objSuccTransit.status = "success";
        objSuccTransit.Condition = QString("(%1)==true").arg(TrueExpr);
        setTransitionAttr(ElemOnTrueTransit,objSuccTransit);

        QDomElement ElemOnFailTransit = ElemOnTrueTransit.cloneNode().toElement();
        GTASXTransition objFailTransit;

        objFailTransit.Target = iTargetId;
        objFailTransit.Condition = QString("(%1)==true").arg(FalseExpr);
        objFailTransit.status = "fail";
        setTransitionAttr(ElemOnFailTransit,objFailTransit);
        oElem.insertAfter(ElemOnFailTransit,ElemOnTrueTransit);

        if(ParamListForInvoke.isEmpty())
        {
            ElemOnTrueTransit.removeAttribute(SXA_EVENT);
            ElemOnFailTransit.removeAttribute(SXA_EVENT);
        }
        else
        {
            ElemOnTrueTransit.setAttribute(SXA_EVENT,"done.invoke._all");
            ElemOnFailTransit.setAttribute(SXA_EVENT,"done.invoke._all");
        }


        //Logging in scxml
        //Success Sate
        QString expVal = objSuccTransit.Condition;
        expVal = expVal.remove("'");
        GTAScxmlLogMessage objSxLog;
        objSxLog.LOD = GTAScxmlLogMessage::Main;
        objSxLog.Identifier    = pReleaseCmd->getInstanceName();
        objSxLog.ExpectedValue = expVal;
        objSxLog.CurrentValue = "ExpectedValue";
        objSxLog.Status = GTAScxmlLogMessage::Success;
        objSxLog.Result = GTAScxmlLogMessage::OK;

        QDomElement ElemOnTrueLog = iDomDoc.createElement(SXN_LOG);
        ElemOnTrueLog.setAttribute(SXA_LABEL,"'info'");
        ElemOnTrueLog.setAttribute(SXA_EXPR,objSxLog.getLogMessage());


        QDomElement ElemOnAssignCond = iDomDoc.createElement(SXN_ASSIGN);
        ElemOnAssignCond.setAttribute(SXA_LOCATION,"ExpectedValue");
        ElemOnAssignCond.setAttribute(SXA_EXPR, objSuccTransit.Condition);
        ElemOnTrueTransit.appendChild(ElemOnAssignCond);
        ElemOnTrueTransit.appendChild(ElemOnTrueLog);

        //Fail State
        objSxLog.ExpectedValue = expVal ;
        objSxLog.CurrentValue = "ExpectedValue";
        objSxLog.Status = GTAScxmlLogMessage::Fail;
        objSxLog.Result = GTAScxmlLogMessage::KO;
        QDomElement ElemOnFailLog = ElemOnTrueLog.cloneNode().toElement();
        ElemOnFailLog.setAttribute(SXA_EXPR,objSxLog.getLogMessage());
        
        
        QDomElement ElemOnAssignCond1 = iDomDoc.createElement(SXN_ASSIGN);
        ElemOnAssignCond1.setAttribute(SXA_LOCATION,"ExpectedValue");
        ElemOnAssignCond1.setAttribute(SXA_EXPR,objFailTransit.Condition);
        ElemOnFailTransit.appendChild(ElemOnAssignCond1);
        ElemOnFailTransit.appendChild(ElemOnFailLog);


        //Logging all Parameters
        QDomElement ElemOnExit = iDomDoc.createElement(SXN_ONEXIT);
        oElem.appendChild(ElemOnExit);
        insertVariableOnExit(ElemOnExit,ListOfVarToLog,iStateId,pReleaseCmd->getInstanceName(),iDomDoc);

        //time out in case if parent is a while or do while statement
        GTAActionBase * pParent = dynamic_cast<GTAActionBase *>(pReleaseCmd->getParent());
        if(pParent != NULL)
        {
            createLoopTimeOutTranstions(oElem,iDomDoc,iStateId,pParent);
        }
        else
        {
            QDomElement entryElem = iDomDoc.createElement(SXN_ONENTRY);
            QDomElement sendElem = iDomDoc.createElement(SXN_SEND);
            sendElem.setAttribute(SXA_EVENT,QString("%1_dummytimeout").arg(iStateId));
            sendElem.setAttribute(SXA_DELAY,"3000ms");
            entryElem.appendChild(sendElem);
            oElem.appendChild(entryElem);

            QDomElement timeoutTx = iDomDoc.createElement(SXN_TRANSITION);
            timeoutTx.setAttribute(SXA_TARGET,iTargetId);
            timeoutTx.setAttribute(SXA_STATUS,"fail");
            timeoutTx.setAttribute(SXA_EVENT,QString("%1_dummytimeout").arg(iStateId));

            GTAScxmlLogMessage objLogOnExit;
            objLogOnExit.LOD = GTAScxmlLogMessage::Main;
            objLogOnExit.Identifier    = pReleaseCmd->getInstanceName();
            objLogOnExit.ExpectedValue = "";
            objLogOnExit.CurrentValue = "";//QString("'+%1+'").arg(sParameterName);
            objLogOnExit.Status = GTAScxmlLogMessage::TimeOut;
            objLogOnExit.Result = GTAScxmlLogMessage::KO;

            QDomElement ElemOnLog = iDomDoc.createElement(SXN_LOG);
            ElemOnLog.setAttribute(SXA_LABEL,"'info'");
            ElemOnLog.setAttribute(SXA_EXPR,objLogOnExit.getLogMessage());

            timeoutTx.appendChild(ElemOnLog);
            oElem.appendChild(timeoutTx);

        }
        createCallTimeOutTranstions(iDomDoc,oElem,(GTACommandBase*)pReleaseCmd,iTargetId);

    }
    return rc;
}
GTAExportScxml::VariableType GTAExportScxml::IsTypeOf(const QString & iVariable)
{
    if(iVariable.isEmpty())
        return EmptyString;

    bool isNumber = false;
    iVariable.toDouble(&isNumber);
    if(isNumber)
        return Number;

    if(iVariable.startsWith("\"") || iVariable.startsWith("\'"))
        return String;

    if(iVariable.toUpper() == "TRUE" || iVariable.toUpper() == "FALSE")
        return Boolean;

    return Unknown;
}
void GTAExportScxml::setStateIDName(const QString& iStateID, const GTACommandBase* pCmd)
{
    if (pCmd!= NULL && !_hshStateIDName.contains(iStateID))
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
        _hshStateIDName[iStateID] = QString("%1_%2").arg(iStateID,stateName);
    }
}
void GTAExportScxml::createLoopTimeOutTranstions(QDomElement &iStateElem, QDomDocument &iDomDoc,const QString &,const GTACommandBase * pParent)
{
    GTACommandBase * pParentCmd = (GTACommandBase* )pParent;

    bool isLoopTimeoutInc = TestConfig::getInstance()->getLoopTimeoutSubsteps();
    if(!isLoopTimeoutInc)
        return;
    GTAScxmlLogMessage logForCallTimeout;
    while(pParentCmd != NULL)
    {
        bool isLoop = false;
        GTAActionBase * pActCmd = dynamic_cast<GTAActionBase *>(pParentCmd);
        if(pActCmd != NULL)
        isLoop = (pActCmd->getComplement() == COM_CONDITION_WHILE) ||(pActCmd->getComplement() == COM_CONDITION_DO_WHILE) /*|| (pActCmd->getComplement() == COM_CONDITION_FOR_EACH)*/;
        if(isLoop)
        {
            QString iParentStateId = pParentCmd->getSCXMLStateId();
            QString iLoopStateId = pParentCmd->getLoopSCXMLStateId();
            if(!iParentStateId.isEmpty())
            {
                QDomElement TxElem= iDomDoc.createElement(SXN_TRANSITION);
                TxElem.setAttribute(SXA_TARGET,iLoopStateId);
                TxElem.setAttribute(SXA_EVENT,QString("%1_TimeOut_Event").arg(iParentStateId));
                QString timeOutEvenFlag = QString("%1_%2").arg(iParentStateId,"TimeOut_Event_Flag");
                QDomElement assignElem = iDomDoc.createElement(SXN_ASSIGN);
                assignElem.setAttribute(SXA_LOCATION,timeOutEvenFlag);
                assignElem.setAttribute(SXA_EXPR,"'TimeOut'");
                GTAScxmlDataModel data;
                data.ID = timeOutEvenFlag;
                _DataModelList.insert(timeOutEvenFlag,data);

                
                logForCallTimeout.Status = GTAScxmlLogMessage::TimeOut;
                logForCallTimeout.Result = GTAScxmlLogMessage::KO;
                logForCallTimeout.ExpectedValue = "";
                QDomElement txLogElem = iDomDoc.createElement(SXN_LOG);
                txLogElem.setAttribute(SXA_LABEL,"info");
                logForCallTimeout.Identifier = pParentCmd->getInstanceName();
                txLogElem.setAttribute(SXA_EXPR,logForCallTimeout.getLogMessage());


                TxElem.appendChild(assignElem);
                TxElem.appendChild(txLogElem);
                iStateElem.appendChild(TxElem);
                
            }
        }
        pParentCmd = pParentCmd->getParent();
    }
}
void GTAExportScxml::createCallTimeOutTranstions(QDomDocument&, QDomElement &, GTACommandBase*,const QString&, GTAScxmlLogMessage)
{
    return;
//there used to be a method here, see github if it's ever needed
}
//to do make this function generic such that the signal name can be configured
void  GTAExportScxml::insertChannelInControlState(QDomDocument& iDomDoc,QDomElement &oStateElem )
{
    if(_hasChannelInControl)
    {
        QString sSTateId = "ChannelInControlSId";
        getCloneRootElement("channelInControl.scxml",iDomDoc,oStateElem);
        if(!oStateElem.isNull())
        {
            QString engine = QString::fromStdString(TestConfig::getInstance()->getEngineSelection());
            QHash<QString,QString> engineVarMap;
            QStringList channelSignals = GTAUtil::getChannelSelectionSignals(engine,engineVarMap);
            if(channelSignals.isEmpty())
                return;

            foreach(QString channelsignal, channelSignals)
            {
                insertValidAppDataModel(channelsignal);
            }
            GTAScxmlDataModel objDM;
            objDM.ID = "ChannelInControlFlag";
            objDM.Expr = "0";
            _DataModelList.insert("ChannelInControlFlag",objDM);

            QDomNode invokeNode1 = oStateElem.namedItem(SXN_INVOKE);
            QDomNode invokeNode2 = invokeNode1;
            createMultiInvokeNode(iDomDoc, oStateElem, invokeNode1.toElement(), channelSignals,
                                  QString("%1_invoke").arg(sSTateId), QString(),"","",sSTateId);

            QDomElement onExitNode = iDomDoc.createElement(SXN_ONEXIT);
            oStateElem.appendChild(onExitNode);
            QDomElement assignNode =iDomDoc.createElement(SXN_ASSIGN);
            onExitNode.appendChild(assignNode);
            if(!assignNode.isNull())
            {
                QDomElement assignElem = assignNode.toElement();
                assignElem.setAttribute(SXA_LOCATION,"ChannelInControlFlag");


                QStringList items;
                foreach(QString channelsignal, channelSignals)
                {
                    if(!channelsignal.trimmed().isEmpty())
                        items.append(QString("%1.Value").arg(channelsignal));
                }
                QString assignExpression = items.join(" || ");
                assignElem.setAttribute(SXA_EXPR,assignExpression);
            }


            oStateElem.removeChild(invokeNode1);
            oStateElem.setAttribute(SXA_ID,sSTateId);
            QDomElement txNode1 = iDomDoc.createElement(SXN_TRANSITION);
            QDomElement txNode2 = iDomDoc.createElement(SXN_TRANSITION);
            oStateElem.appendChild(txNode1);
            oStateElem.appendChild(txNode2);
            if(!txNode1.isNull())
            {
                txNode1.setAttribute(SXA_TARGET,"ChannelInControlSId");
                txNode1.setAttribute(SXA_CONDITION,QString("%1 != 'Executed'").arg("main_final_done"));
                txNode1.setAttribute(SXA_EVENT,"done.invoke._all");
            }
            if(!txNode2.isNull())
            {
                txNode2.setAttribute(SXA_TARGET,"unsubscribe");
                txNode2.setAttribute(SXA_CONDITION,QString("%1 == 'Executed'").arg("main_final_done"));
                txNode2.setAttribute(SXA_EVENT,"done.invoke._all");
            }
        }

    }

}
void  GTAExportScxml::getChannelInControlCondition(const QString & iParam, const QString & iVal, 
                                                      const QString & iCond, QString & oTrueExpress, 
                                                      QString & oFalseExpress,bool isStringVal, 
                                                      const double iPrecision,const QString &iPrecisionUnit,
                                                      QStringList& oVarList, bool isFunctionStatus)
{

    QHash<QString,QString> engineVarMap;

    TestConfig* testConf = TestConfig::getInstance();
    bool isChannelSettingEn = testConf->getChannelSelection();
    QString engine = QString::fromStdString(testConf->getEngineSelection());
    QStringList params = GTAUtil::getChannelSelectionSignals(engine,engineVarMap);
    QString channelSelectionFlag = "";


    if(isChannelSettingEn && _hasChannelInControl)
    {


        if(iParam.contains("CFM1") || iParam.contains("PW1") || iParam.contains("RR1"))
        {
            channelSelectionFlag = QString("(%1.Value)").arg(engineVarMap.value("1"));
        }
        else if(iParam.contains("CFM2") || iParam.contains("PW2") || iParam.contains("RR2"))
        {
            channelSelectionFlag = QString("(%1.Value)").arg(engineVarMap.value("2"));
        }
        else if(iParam.contains("CFM3") || iParam.contains("PW3") || iParam.contains("RR3"))
        {
            channelSelectionFlag = QString("(%1.Value)").arg(engineVarMap.value("3"));
        }
        else if(iParam.contains("CFM4") || iParam.contains("PW4") || iParam.contains("RR4"))
        {
            channelSelectionFlag = QString("(%1.Value)").arg(engineVarMap.value("4"));
        }
        else
        {
            if(params.count()>1)
            {
                channelSelectionFlag = QString("(%1.Value || %2.Value)").arg(params.at(0),params.at(1));
            }
            else if(params.count() == 1)
            {
                channelSelectionFlag = QString("(%1.Value)").arg(params.at(0));
            }
            else
            {
                channelSelectionFlag = "true";
            }
        }

    }


    if (isChannelSettingEn && _hasChannelInControl)
    {
        QString param1ChA = iParam;
        QString param1ChB = iParam;
        QString param2ChA = iVal;
        QString param2ChB = iVal;

        QString cicAIdentifier = QString("A:CIC%1").arg(GTA_CIC_IDENTIFIER);
        QString cicBIdentifier = QString("B:CIC%1").arg(GTA_CIC_IDENTIFIER);
        QString cnicAIdentifier = QString("A:CNIC%1").arg(GTA_CIC_IDENTIFIER);
        QString cnicBIdentifier = QString("B:CNIC%1").arg(GTA_CIC_IDENTIFIER);

        QStringList items1 = GTAUtil::getProcessedParameterForChannel(iParam,true,true);

        if(items1.count() == 2)
        {
            param1ChA = items1[0];
            param1ChB = items1[1];
        }
        if(items1.count() == 1)
        {
            param1ChA = items1[0];
        }
        QStringList items2 = GTAUtil::getProcessedParameterForChannel(iVal,true,true);

        if(items2.count() == 2)
        {
            param2ChA = items2[0];
            param2ChB = items2[1];
        }
        if(items2.count() == 1)
        {
            param2ChA = items2[0];

        }
        //     if(iParam.contains(QString(":CIC%1").arg(GTA_CIC_IDENTIFIER)) || iParam.contains(QString(":CNIC%1").arg(GTA_CIC_IDENTIFIER)))
        //     {
        //
        //
        //         param1ChA.replace(cicAIdentifier,"A");
        //         param1ChB.replace(cicAIdentifier,"B");
        //         param1ChA.replace(cnicAIdentifier,"A");
        //         param1ChB.replace(cnicAIdentifier,"B");


        //         param1ChA.replace(cicBIdentifier,"A");
        //         param1ChB.replace(cicBIdentifier,"B");
        //         param1ChA.replace(cnicBIdentifier,"A");
        //         param1ChB.replace(cnicBIdentifier,"B");

        //if(param1ChA.contains("EEC_CFM1"))
        //{

        //}

        //         param1ChB.replace(GTA_CIC_IDENTIFIER,"");
        //         param1ChA.replace(GTA_CIC_IDENTIFIER,"");
        //         param1ChB.replace(GTA_CIC_IDENTIFIER,"");
        //         param1ChA.replace(GTA_CIC_IDENTIFIER,"");

        //     }
        //     if(iVal.contains(QString(":CIC%1").arg(GTA_CIC_IDENTIFIER)) || iVal.contains(QString(":CNIC%1").arg(GTA_CIC_IDENTIFIER)))
        //     {
        //         param2ChA.replace(cicAIdentifier,"A");
        //         param2ChB.replace(cicAIdentifier,"B");
        //         param2ChA.replace(cnicAIdentifier,"A");
        //         param2ChB.replace(cnicAIdentifier,"B");

        //         param2ChA.replace(cicBIdentifier,"A");
        //         param2ChB.replace(cicBIdentifier,"B");
        //         param2ChA.replace(cnicBIdentifier,"A");
        //         param2ChB.replace(cnicBIdentifier,"B");


        //         param2ChA.replace(GTA_CIC_IDENTIFIER,"");
        //         param2ChB.replace(GTA_CIC_IDENTIFIER,"");
        //         param2ChA.replace(GTA_CIC_IDENTIFIER,"");
        //         param2ChB.replace(GTA_CIC_IDENTIFIER,"");
        //     }

        if(_ParamList.contains(param1ChA))
            oVarList<<param1ChA;
        if(_ParamList.contains(param2ChA))
            oVarList<<param2ChA;
        if(_ParamList.contains(param1ChB))
            oVarList<<param1ChB;
        if(_ParamList.contains(param2ChB))
            oVarList<<param2ChB;
        oVarList.removeDuplicates();

        if(QStringList(_ParamList.keys()).contains(param1ChA,Qt::CaseInsensitive))
        {
            param1ChA = getParamObj(param1ChA).getName();
            //oVarList.takeAt(0);oVarList.insert(0,param1ChA);
            if(!isFunctionStatus)
                param1ChA = param1ChA.append(".Value");
            else
                param1ChA = param1ChA.append(".Status");
        }
        if(QStringList(_ParamList.keys()).contains(param2ChA,Qt::CaseInsensitive))
        {
            param2ChA = getParamObj(param2ChA).getName();
            //oVarList.takeAt(1);oVarList.insert(1,param2ChA);
            if(!isFunctionStatus)
                param2ChA = param2ChA.append(".Value");
            else
                param1ChA = param1ChA.append(".Status");
        }
        if(QStringList(_ParamList.keys()).contains(param1ChB,Qt::CaseInsensitive))
        {

            param1ChB = getParamObj(param1ChB).getName();
            //oVarList.takeAt(2);oVarList.insert(2,param1ChB);
            if(!isFunctionStatus)
                param1ChB = param1ChB.append(".Value");
        }
        if(QStringList(_ParamList.keys()).contains(param2ChB,Qt::CaseInsensitive))
        {
            param2ChB = getParamObj(param2ChB).getName();
            //oVarList.takeAt(3);oVarList.insert(3,param2ChB);
            if(!isFunctionStatus)
                param2ChB = param2ChB.append(".Value");
        }

        QString trueCond1 = "";
        QString falseCond1 = "";
        QString trueCond2 = "";
        QString falseCond2 = "";

	
        getExpresion(param1ChA,param2ChA,iCond,trueCond1,falseCond1,isStringVal,iPrecision,iPrecisionUnit);

        getExpresion(param1ChB,param2ChB,iCond,trueCond2,falseCond2,isStringVal,iPrecision,iPrecisionUnit);
        
        if((items2.count() == 2) || (items1.count() == 2))
        {
            if(iParam.contains(cicAIdentifier) || iParam.contains(cicBIdentifier))
            {
                oTrueExpress = QString("((%1 && (%2)) || (!%1 && (%3)) == true)").arg(channelSelectionFlag,trueCond1,trueCond2);
                oFalseExpress = QString("((%1 && (%2)) || (!%1 && (%3)) == false)").arg(channelSelectionFlag,trueCond1,trueCond2);
            }
            else if(iParam.contains(cnicAIdentifier) || iParam.contains(cnicBIdentifier))
            {
                oTrueExpress = QString("((!%1 && (%2)) || (%1 && (%3)) == true)").arg(channelSelectionFlag,trueCond1,trueCond2);
                oFalseExpress = QString("((!%1 && (%2)) || (%1 && (%3)) == false)").arg(channelSelectionFlag,trueCond1,trueCond2);

            }
            else
            {
                oTrueExpress = QString("((%1 && (%2)) || (!%1 && (%3)) == true)").arg(channelSelectionFlag,trueCond1,trueCond2);
                oFalseExpress = QString("((%1 && (%2)) || (!%1 && (%3)) == false)").arg(channelSelectionFlag,trueCond1,trueCond2);
            }
        }
        else
        {
            if((param1ChA.contains(GTA_CIC_IDENTIFIER)) || (param2ChA.contains(GTA_CIC_IDENTIFIER)))
            {
                //make channel B equation
                oTrueExpress = QString("(%1 && (%2)) == true)").arg(channelSelectionFlag,trueCond2);
                oFalseExpress = QString("(%1 && (%2)) == false)").arg(channelSelectionFlag,trueCond2);
            }
            else if(param1ChB.contains(GTA_CIC_IDENTIFIER) || param2ChB.contains(GTA_CIC_IDENTIFIER))
            {
                //make channel a equation
                oTrueExpress = QString("(%1 && (%2)) == true)").arg(channelSelectionFlag,trueCond1);
                oFalseExpress = QString("(%1 && (%2)) == false)").arg(channelSelectionFlag,trueCond1);
            }
        }
    }
    else
    {
        QString  lhs = iParam;
        QString  rhs = iVal;
        oVarList<<lhs<<rhs;
        if(QStringList(_ParamList.keys()).contains(lhs,Qt::CaseInsensitive))
        {
            QString item = getParamObj(lhs).getName();
            
            if(!item.isEmpty())
                lhs = item;
            oVarList.takeAt(0);oVarList.insert(0,lhs);
            if(isFunctionStatus == false)
                lhs = lhs.append(".Value");
            else
                lhs = lhs.append(".Status");
        }
        if(QStringList(_ParamList.keys()).contains(rhs,Qt::CaseInsensitive))
        {
            QString item = getParamObj(rhs).getName();
            if(!item.isEmpty())
                rhs = item;
            oVarList.takeAt(1);oVarList.insert(0,rhs);
            rhs = rhs.append(".Value");
        }
        
        getExpresion(lhs,rhs,iCond,oTrueExpress,oFalseExpress,isStringVal,iPrecision,iPrecisionUnit);
    }
    if(isChannelSettingEn && _hasChannelInControl)
        oVarList.append(params);
}
void GTAExportScxml::getNodeTimeouts(GTACmdNode* pCmdNode,QMap<QString,double>& timeOuts)
{

    GTACmdNode* pParentNode = pCmdNode->Parent;
    if (pParentNode!=NULL)
    {
        GTACommandBase* pCommand = pParentNode->getCommand();
        GTAActionCall* pCall = dynamic_cast<GTAActionCall*>(pCommand);
        if (NULL!=pCall)
        {
            QString parentStateID = pParentNode->getStateId();
            double dTimeOut=pCall->getTimeoutInMilliseconds();

            timeOuts.insert(parentStateID,dTimeOut);
        }
        getNodeTimeouts(pParentNode,timeOuts);
    }
}

void GTAExportScxml::generateFailureDataModel(QDomDocument &iDomDoc,QDomElement &iDataModel)
{
    QList<QDomNode> failureDeclarations;
    if(!_FailureCommandList.isEmpty())
    {
        QDomElement failureList = iDomDoc.createElement(SXN_DATA);
        failureList.setAttribute(SXA_EXPR,QString("[]"));
        failureList.setAttribute(SXA_ID,QString("failure_list"));
        iDataModel.appendChild(failureList);
        


        for(int i=0;i<_FailureCommandList.count();i++)
        {
            GTAActionFailure * pFailCmd = dynamic_cast<GTAActionFailure * >(_FailureCommandList.at(i));

            if(pFailCmd != NULL)
            {
                QString failName = pFailCmd->getFailureName();



                QDomElement userdefinedFailureName = iDomDoc.createElement(SXN_DATA);
                userdefinedFailureName.setAttribute(SXA_EXPR,QString("{'failureConfiguration':{'advancedProfiles':[]}}"));
                userdefinedFailureName.setAttribute(SXA_ID,QString("%1").arg(failName));
                iDataModel.appendChild(userdefinedFailureName);

                QDomElement userdefinedFailureNameList = iDomDoc.createElement(SXN_DATA);
                userdefinedFailureNameList.setAttribute(SXA_EXPR,QString("%1").arg(failName));
                userdefinedFailureNameList.setAttribute(SXA_ID,QString("failure_list[%1]").arg(failName));
                iDataModel.appendChild(userdefinedFailureNameList);

                QList <QPair<QString,GTAFailureConfig* >> ConfigList = pFailCmd->getParamValConfig();
                for(int j=0;j<ConfigList.count();j++)
                {
                    QPair<QString,GTAFailureConfig* > ConfigItem = ConfigList.at(j);
                    QString paramName = ConfigItem.first;
                    GTAFailureConfig * pSegmentItemList =  ConfigItem.second;

                    QDomElement advanceProfileElem = iDomDoc.createElement(SXN_DATA);
                    advanceProfileElem.setAttribute(SXA_EXPR,QString("{'variable':'%1', 'basicProfiles': []}").arg(paramName));
                    advanceProfileElem.setAttribute(SXA_ID,QString("failure_list[%2].failureConfiguration.advancedProfiles[%1]").arg(QString::number(j),failName));
                    iDataModel.appendChild(advanceProfileElem);

                    for(int k=0;k<pSegmentItemList->noOfProfiles();k++)
                    {
                        QString sVmacForcingType = "";
                        double dDuration = 0.0;
                        GTAFailureProfileBase * pProfileBase = NULL;
                        pSegmentItemList->getFailureProfile(k,pProfileBase,dDuration,sVmacForcingType);
                        if(pProfileBase!= NULL)
                        {
                            QString funcStatement = "";
                            QString funcType = "";
                            pProfileBase->getFunctionStatement(funcType,funcStatement);
                            QString duration = QString::number(dDuration);
                            QString finalStatement = QString("%1,'duration':%2").arg(funcStatement,duration);
                            QDomElement failureDataModel = iDomDoc.createElement(SXN_DATA);
                            failureDataModel.setAttribute(SXA_EXPR,QString("{%1}").arg(finalStatement));
                            failureDataModel.setAttribute(SXA_ID,QString("failure_list[%1].failureConfiguration.advancedProfiles[%2].basicProfiles[%3]").arg(failName,QString::number(j),QString::number(k)));
                            iDataModel.appendChild(failureDataModel);
                        }
                    }



                }


            }
        }
    }
}

bool GTAExportScxml::isAParameter(QString iParameter)
{
    bool isNum;
    iParameter.toDouble(&isNum);
    bool isString = iParameter.startsWith("\"")||iParameter.startsWith("'");
    if (!isNum && !isString)
    {
        if(QStringList(_ParamList.keys()).contains(iParameter,Qt::CaseInsensitive))
            return true;
        /* else if (_ParamList.contains(iParameter.toUpper()))
        {
            return true;
        }*/

    }

    return false;
}

GTAICDParameter GTAExportScxml::getParamObj(QString iParameter)
{
    bool isNum;
    // bool test = false;
    iParameter.toDouble(&isNum);
    bool isString = iParameter.startsWith("\"")||iParameter.startsWith("'");
    if (!isNum && !isString)
    {
        QStringList lstOfParamNames = QStringList(_ParamList.keys());

        bool rc = lstOfParamNames.contains(iParameter,Qt::CaseInsensitive);
        if(rc)
        {
            QRegExp rgExp(iParameter);rgExp.setCaseSensitivity(Qt::CaseInsensitive);
            int index = lstOfParamNames.indexOf(rgExp);

            if((index > 0) && (index <lstOfParamNames.count()))
            {
                QString paramName = lstOfParamNames.at(index);
                return _ParamList.value(paramName);
            }
            else
            {
                for(int i=0;i<lstOfParamNames.count();i++)
                {
                    QString sParam = lstOfParamNames[i];
                    if(sParam == iParameter)
                    {
                        return _ParamList.value(sParam);
                    }
                }

            }
            return GTAICDParameter();
        }
        return GTAICDParameter() ;
    }
    return GTAICDParameter ();
}

QString GTAExportScxml::getActualParameter(const QString &iParam)
{

    GTAICDParameter sparamobj =getParamObj(iParam);
    if(!sparamobj.getName().isEmpty())
        return sparamobj.getTempParamName();
    else
        return iParam;
}

void GTAExportScxml::setElementName(GTAElement* pElement,QString iSaveFilePath)

{
    if(pElement == NULL)
    {
        return;
    }

    _pElement = pElement;
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
void GTAExportScxml::setInvokeDelay(QDomDocument &iDomDoc, QDomNode &iInvokeNode, const QString &iActionName, const QString &iComplementName, QString &oTxEventName, const int mulFact)
{
    GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
    QString cmdName = QString("%1 %2").arg(iActionName,iComplementName).trimmed();
    QString sValue =pAppCtrl->getCommandProperty(cmdName.trimmed(),XATTR_INVOKE_DELAY);
    QString sValueUnit=pAppCtrl->getCommandProperty(cmdName.trimmed(),XATTR_INVOKE_DELAY_UNIT);
    QString sDelayInMs = QString("%1%2").arg(sValue,sValueUnit);
    QString tempDelay = sDelayInMs;
    tempDelay.remove("ms",Qt::CaseInsensitive);

    bool isNum = false;
    double dDelay =   tempDelay.toDouble(&isNum);
    if (tempDelay.isEmpty() || isNum == false)
    {
        sDelayInMs = "0ms";
        tempDelay = "0";
        dDelay = 0;
    }

    QString sInvokeId = "";
    if (iInvokeNode.isNull() == false)
    {
        sInvokeId = iInvokeNode.toElement().attribute(SXA_ID);
        QDomNodeList finalizeNodes = iInvokeNode.toElement().elementsByTagName(SXN_FINALIZE);
        // bool delayNodeExists = false;
        if (!finalizeNodes.isEmpty())
        {
            //            QDomNodeList sendNodeLst = finalizeNodes.at(0).toElement().elementsByTagName(SXN_SEND);
            //            if (!sendNodeLst.isEmpty())
            //            {
            //                for (int i=0;i<sendNodeLst.size();i++)
            //                {
            //                    QDomNode currSendNode = sendNodeLst.at(i);
            //                    QDomElement currentSendElement = currSendNode.toElement();
            //                    if(currentSendElement.hasAttribute(SXA_DELAY))
            //                    {
            //                        currentSendElement.setAttribute(SXA_DELAY,sDelayInMs);
            //                        QString sDelayId = QString("delay_event_%1").arg(sInvokeId);
            //                        currentSendElement.setAttribute(SXA_EVENT,sDelayId);

            //                        delayNodeExists = true;
            //                        break;
            //                    }

            //                }


            //            }
            //            else if(dDelay>0.0001 && delayNodeExists==false)
            //            {
            //                QDomElement sendElem = iDomDoc.createElement(SXN_SEND);
            //                sendElem.setAttribute(SXA_DELAY,sDelayInMs);
            //                QString sDelayId = QString("delay_event_%1").arg(sInvokeId);
            //                sendElem.setAttribute(SXA_EVENT,sDelayId);
            //                finalizeNodes.at(0).appendChild(sendElem);
            //            }
            if(tempDelay == "0")
            {
                //                QDomElement assignElem3 = iDomDoc.createElement(SXN_ASSIGN);

                //                assignElem3.setAttribute(SXA_LOCATION,"start");
                //                assignElem3.setAttribute(SXA_EXPR,"Date.now()");
                //                finalizeNodes.at(0).appendChild(assignElem3);

                //                QDomElement assignElem2 = iDomDoc.createElement(SXN_ASSIGN);
                //                assignElem2.setAttribute(SXA_LOCATION,"newTime");
                //                assignElem2.setAttribute(SXA_EXPR,"Date.now()");
                //                finalizeNodes.at(0).insertAfter(assignElem2,assignElem3);

                //                QDomElement logElem = iDomDoc.createElement(SXN_LOG);
                //                QString delayEq= QString("while ((newTime - start) < %1) {newTime=Date.now()}").arg(tempDelay);
                //                logElem.setAttribute(SXA_EXPR,delayEq);
                //                finalizeNodes.at(0).appendChild(logElem);
            }
            else
            {
                sInvokeId = sInvokeId.replace(".","_");
                QDomElement sendDelayEvent = iDomDoc.createElement(SXN_SEND);
                oTxEventName = QString("HiddenWaitTimeDelay_%1").arg(sInvokeId);
                sendDelayEvent.setAttribute(SXA_EVENT,oTxEventName);

                sendDelayEvent.setAttribute(SXA_DELAY,QString("%1ms").arg(QString::number((mulFact * tempDelay.toInt()))));
                finalizeNodes.at(0).appendChild(sendDelayEvent);
            }
        }
    }
}

void GTAExportScxml::insertFinalState(QDomDocument &iDomDoc,QDomElement &iStateElem,const QString &iId)
{
    QDomElement finalState = iDomDoc.createElement(SXN_FINAL);
    finalState.setAttribute(SXA_ID,QString("%1_Final").arg(iId));
    iStateElem.appendChild(finalState);
}
bool GTAExportScxml::createActionGenericToolSCXMLState(GTAGenericToolCommand *pGenericToolCmd, QDomDocument &iDomDoc, QDomElement &iRootElem, const QString &iStateId, const QString &iTargetId)
{
    bool rc = false;
    if(pGenericToolCmd)
    {

        QString stateId = QString(iStateId);
        QString finalID = stateId + "_final";
        QString timeoutEventName = QString("%1_timeout").arg(iStateId);
        QString toolName = pGenericToolCmd->getToolName();


        QDomElement cmdStateElem = iDomDoc.createElement(SXN_STATE);
        cmdStateElem.setAttribute(SXA_ID,stateId);

        setStateIDName(iStateId,pGenericToolCmd);

        iRootElem.appendChild(cmdStateElem);

        QList<GTAGenericFunction> Functions = pGenericToolCmd->getCommand();
        QList<GTAGenericParamData> Definitions = pGenericToolCmd->getDefinitions();

        QString actOnFail = pGenericToolCmd->getActionOnFail();

        double dTimeout = pGenericToolCmd->getTimeOutInMs();

        QString instanceName = pGenericToolCmd->getInstanceName();

        //        QString cmdPath = pGenericToolCmd->getCommandPath();
        //        cmdPath.replace("call function - ","");
        //        cmdPath.replace("(","");
        //        cmdPath.replace(")","");
        //        QStringList items = cmdPath.split("#");

        //        if(items.count()>0)
        //            items.removeAt(items.count() - 1);
        //        cmdPath = items.join("#");
        //        cmdPath.replace("/","#");

        QString cmdPath = pGenericToolCmd->getObjId();
        if(dTimeout > 0)
        {
            QDomElement onEntryElem = iDomDoc.createElement(SXN_ONENTRY);

            QDomElement timeoutSendElem = iDomDoc.createElement(SXN_SEND);
            timeoutSendElem.setAttribute(SXA_EVENT,timeoutEventName);
            timeoutSendElem.setAttribute(SXA_DELAY,QString::number(dTimeout)+"ms");

            onEntryElem.appendChild(timeoutSendElem);

            cmdStateElem.appendChild(onEntryElem);

            QDomElement logelem = iDomDoc.createElement(SXN_LOG);
            logelem.setAttribute(SXA_EXPR,instanceName);

            //onEntryElem.appendChild(logelem);

        }

        for(int i = 0; i < Functions.count(); i++)
        {

            GTAGenericFunction objFunc = Functions.at(i);
            QString id = stateId +"_Generic_Tool_" + QString::number(i);
            QString funcName = objFunc.getFunctionName();
            bool isWaitUntilTrue = false;

            QString cmdName = QString("%1 %2").arg(pGenericToolCmd->getAction(),pGenericToolCmd->getComplement()).trimmed().replace(" ","_");
            QString location = QString("%1:%2$%3").arg(cmdName,cmdPath.trimmed(),objFunc.getReturnParamName().trimmed());
            location.replace(" ","_");

            if(i==0)
                cmdStateElem.setAttribute(SXA_INITIALSTATE,id);

            QString funcReturnName = QString("%1_%2").arg(id,objFunc.getReturnParamName());

            QString returnParamName = objFunc.getReturnParamName();
            QString returnParamType = objFunc.getReturnParamType();

            QString returnTrueCond;
            isWaitUntilTrue = pGenericToolCmd->isWaitUntilEnabled(returnParamName,returnParamType);
            bool rcTrueCond = pGenericToolCmd->getSCXMLConditionStatement(funcReturnName,returnParamName,true,returnTrueCond,returnParamType);
            QString returnFalseCond;
            bool rcFalseCond = pGenericToolCmd->getSCXMLConditionStatement(funcReturnName,returnParamName,false,returnFalseCond,returnParamType);

            // QString returnCond = objFunc.getSCXMLConditionStatement(funcReturnName);

            GTAScxmlDataModel dataItem;
            dataItem.ID = funcReturnName;
            _DataModelList.insert(funcReturnName,dataItem);
            if(!_genToolReturnMap.contains(location))
                _genToolReturnMap.insert(location,funcReturnName);

            //<STATE>
            QDomElement FuncStateElement = iDomDoc.createElement(SXN_STATE);
            FuncStateElement.setAttribute(SXA_ID, id);

            //<INVOKE>
            QDomElement InvokeElem = iDomDoc.createElement(SXN_INVOKE);
            InvokeElem.setAttribute(SXA_TYPE,"xml-rpc");


            QString invokeId = id;
            invokeId.replace("s","i");

            InvokeElem.setAttribute(SXA_ID, invokeId);

            //<CONTENT>
            QDomElement ContentElem = iDomDoc.createElement(SXN_CONTENT);

            //<FINALIZE>
            QDomElement finalizElem = iDomDoc.createElement(SXN_FINALIZE);


            //<XMLRPC>
            QDomElement XmlRpcElem = iDomDoc.createElement(SXN_XMLRPC);
            XmlRpcElem.setAttribute(SXA_TOOLID,objFunc.getToolID());
            XmlRpcElem.setAttribute(SXA_TOOLTYPE, objFunc.getToolType());

            //<FUNCTION>
            QDomElement FuncElement = iDomDoc.createElement(SXN_FUNCTION);
            FuncElement.setAttribute(SXA_NAME,objFunc.getFunctionName());

            //<Argument>
            //QList<GTAGenericArgument> argList = objFunc.getFunctionArguments();
            QList<GTAGenericArgument> argList = objFunc.getAllArguments();
            for(int j = 0 ; j < argList.count(); j++)
            {

                GTAGenericArgument objArg = argList.at(j);

                QString argType = objArg.getArgumentType();
                QString argValue = objArg.getUserSelectedValue();

                if(argType.toLower() == "string")
                {
                    if(argValue.contains("'"))
                        argValue.replace("'","");
                    argValue = QString("'%1'").arg(argValue);
                }


                QDomElement ArgElem = iDomDoc.createElement(SXN_ARGUMENT);
                ArgElem.setAttribute(SXA_VALUE, argValue);
                ArgElem.setAttribute(SXA_TYPE, argType);
                ArgElem.setAttribute(SXA_NAME, objArg.getArgumentName());
                FuncElement.appendChild(ArgElem);
            }
            //<return value>
            QDomElement RetValElem = iDomDoc.createElement(SXN_RETURNVALUE);
            RetValElem.setAttribute(SXA_NAME,funcReturnName);
            FuncElement.appendChild(RetValElem);

            XmlRpcElem.appendChild(FuncElement);
            ContentElem.appendChild(XmlRpcElem);
            InvokeElem.appendChild(ContentElem);
            FuncStateElement.appendChild(InvokeElem);


            //<ASSIGN>
            QDomElement finalizeAssignElem = iDomDoc.createElement(SXN_ASSIGN);
            finalizeAssignElem.setAttribute(SXA_LOCATION,funcReturnName);
            finalizeAssignElem.setAttribute(SXA_EXPR,QString("_event.data.%1").arg(funcReturnName));

            finalizElem.appendChild(finalizeAssignElem);

            InvokeElem.appendChild(finalizElem);

            QString nextId = stateId +"_Generic_Tool_" + QString::number(i+1);
            if(i >= Functions.count() - 1)
            {
                nextId = finalID;
            }


            //<TRANSITION - SUCCESS>
            QDomElement TransitionElemSuccess = iDomDoc.createElement(SXN_TRANSITION);
            TransitionElemSuccess.setAttribute(SXA_TARGET,nextId);
            TransitionElemSuccess.setAttribute(SXA_STATUS,"success");
            if(rcTrueCond)
                TransitionElemSuccess.setAttribute(SXA_CONDITION,QString("%1").arg(returnTrueCond));
            else
                TransitionElemSuccess.setAttribute(SXA_CONDITION,QString("%1 == 'OK'").arg(returnTrueCond));
            TransitionElemSuccess.setAttribute(SXA_EVENT,"done.invoke._all");


            //LOG for <TRANSITION - SUCCESS>

            GTAScxmlLogMessage objLogSuccess;
            objLogSuccess.LOD = GTAScxmlLogMessage::Main;
            objLogSuccess.Identifier    = pGenericToolCmd->getInstanceName();
            objLogSuccess.ExpectedValue = pGenericToolCmd->getStatement().replace("\n","  ").replace("\'","").replace("\"","");
            objLogSuccess.CurrentValue = funcReturnName;
            objLogSuccess.Status = GTAScxmlLogMessage::Success;
            objLogSuccess.Result = GTAScxmlLogMessage::OK;

            QString logMsgSuccess = objLogSuccess.getLogMessage();
            QDomElement logSuccessElem = iDomDoc.createElement(SXN_LOG);
            logSuccessElem.setAttribute(SXA_EXPR,logMsgSuccess);
            TransitionElemSuccess.appendChild(logSuccessElem);

            QString failId = nextId;

            if((isWaitUntilTrue) && (dTimeout>0))
                failId = id;

            if(actOnFail == "stop")
                failId = SXN_FINAL;

            //<TRANSITION - FAIL>
            QDomElement TransitionElemFail = iDomDoc.createElement(SXN_TRANSITION);
            TransitionElemFail.setAttribute(SXA_TARGET,failId);
            TransitionElemFail.setAttribute(SXA_STATUS,"fail");
            if(rcFalseCond)
                TransitionElemFail.setAttribute(SXA_CONDITION,QString("%1").arg(returnFalseCond));
            else
                TransitionElemFail.setAttribute(SXA_CONDITION,QString("%1 != 'OK'").arg(returnFalseCond));

            TransitionElemFail.setAttribute(SXA_EVENT,"done.invoke._all");

            //LOG for <TRANSITION - FAIL>

            GTAScxmlLogMessage objLogFail;
            objLogFail.LOD = GTAScxmlLogMessage::Main;
            objLogFail.Identifier    = pGenericToolCmd->getInstanceName();
            objLogFail.ExpectedValue = pGenericToolCmd->getStatement().replace("\n","  ").replace("\'","").replace("\"","");
            objLogFail.CurrentValue = funcReturnName;
            objLogFail.Status = GTAScxmlLogMessage::Fail;
            objLogFail.Result = GTAScxmlLogMessage::KO;
            QString logMsgFail = objLogFail.getLogMessage();
            QDomElement logFailElem = iDomDoc.createElement(SXN_LOG);
            logFailElem.setAttribute(SXA_EXPR,logMsgFail);
            TransitionElemFail.appendChild(logFailElem);



            QDomElement TransitionElemTimeout = iDomDoc.createElement(SXN_TRANSITION);

            if(dTimeout>0)
            {
                //<TRANSITION - TIMEOUT>
                QString timeoutId = nextId;
                if(actOnFail == "stop")
                    timeoutId = SXN_FINAL;
                TransitionElemTimeout.setAttribute(SXA_TARGET,timeoutId);
                TransitionElemTimeout.setAttribute(SXA_STATUS,"fail");
                TransitionElemTimeout.setAttribute(SXA_EVENT,timeoutEventName);

                //LOG for <TRANSITION - TIMEOUT>

                GTAScxmlLogMessage objLogTimeOut;
                objLogTimeOut.LOD = GTAScxmlLogMessage::Main;
                objLogTimeOut.Identifier    = pGenericToolCmd->getInstanceName();
                objLogTimeOut.ExpectedValue = pGenericToolCmd->getStatement().replace("\n","  ").replace("\'","").replace("\"","");
                objLogTimeOut.CurrentValue = "TimeOut";
                objLogTimeOut.Status = GTAScxmlLogMessage::TimeOut;
                objLogTimeOut.Result = GTAScxmlLogMessage::KO;
                QString logMsgTimeout = objLogTimeOut.getLogMessage();

                logMsgTimeout.replace("\'","");
                logMsgTimeout.replace("+","");
                logMsgTimeout = QString("'%1'").arg(logMsgTimeout);

                QDomElement logTimeoutElem = iDomDoc.createElement(SXN_LOG);
                logTimeoutElem.setAttribute(SXA_EXPR,logMsgTimeout);
                TransitionElemTimeout.appendChild(logTimeoutElem);
            }

            QDomElement onExitElem = iDomDoc.createElement(SXN_ONEXIT);
            QDomElement exitLogElem = iDomDoc.createElement(SXN_LOG);
            exitLogElem.setAttribute(SXA_EXPR,funcReturnName);
            onExitElem.appendChild(exitLogElem);

            FuncStateElement.appendChild(TransitionElemSuccess);
            FuncStateElement.appendChild(TransitionElemFail);

            if(dTimeout>0)
                FuncStateElement.appendChild(TransitionElemTimeout);

            FuncStateElement.appendChild(onExitElem);

            cmdStateElem.appendChild(FuncStateElement);
        }

        //<TRANSITION> for entire state
        QDomElement transition = iDomDoc.createElement(SXN_TRANSITION);
        transition.setAttribute(SXA_TARGET,iTargetId);

        cmdStateElem.appendChild(transition);

        QDomElement FinalElem = iDomDoc.createElement(SXN_FINAL);
        FinalElem.setAttribute(SXA_ID, finalID);

        cmdStateElem.appendChild(FinalElem);

        rc = true;
    }

    return rc;
}


void GTAExportScxml::updateWithGenToolParam(QString &ioParam)
{
    QStringList iTokens = ioParam.split("$");
    QStringList keys = _genToolReturnMap.keys();
    for(int i=0;i<keys.count();i++)
    {
        QString key = keys.at(i);

        QStringList tokens = key.split("$");
        QString loc = tokens.at(0);
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

