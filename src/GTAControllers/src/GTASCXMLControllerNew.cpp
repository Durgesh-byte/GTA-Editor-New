/*****************************************************************//**
 * \file   GTASCXMLControllerNew.cpp
 *********************************************************************/
#include "GTASCXMLControllerNew.h"
#include "GTAScxmlLogMessage.h"
#include "GTAUtil.h"
#include "GTAActionParentTitleEnd.h"
#include "GTACMDSCXMLPrint.h"
#include "GTACMDSCXMLUtils.h"
#include "GTACMDSCXMLPrintTable.h"
#include "GTACMDSCXMLPrintTime.h"
#include "GTACMDSCXMLManualAction.h"
#include "GTACMDSCXMLSet.h"
#include "GTACMDSCXMLUnInitPIRParameters.h"
#include "GTACMDSCXMLInitPIRParameters.h"
#include "GTACMDSCXMLReleaseVMACParameters.h"
#include "GTACMDSCXMLWaitFor.h"
#include "GTACMDSCXMLCALL.h"
#include "GTACMDSCXMLCheckValue.h"
#include "GTACMDSCXMLWaitUntil.h"
#include "GTACMDSCXMLExternalTool.h"
#include "GTACMDSCXMLConditionIfElse.h"
#include "GTACMDSCXMLConditionIf.h"
#include "GTACMDSCXMLConditionElse.h"
#include "GTACMDSCXMLConditionWhile.h"
#include "GTACMDSCXMLConditionDoWhile.h"
#include "GTACMDSCXMLConditionForEach.h"
#include "GTACMDSCXMLContainerLoop.h"
#include "GTACMDSCXMLSCXML.h"
#include "GTAActionIfEnd.h"
#include "GTACMDSCXMLMaths.h"
#include "GTAActionWhileEnd.h"
#include "GTAActionDoWhileEnd.h"
#include "GTAActionForEach.h"
#include "GTAActionForEachEnd.h"
#include "GTACMDSCXMLCheckFWCMessage.h"
#include "GTACMDSCXMLCheckBITEMessage.h"
#include "GTACMDSCXMLCallParallel.h"
#include "GTAActionWait.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"
#include "GTACMDSCXMLLog.h"
#include "GTACMDSCXMLUtils.h"

#pragma warning(push, 0)
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#pragma warning(pop)

 /* ========================================================================================================================================================================
			...
		  ;::::;																			!! ATTENTION TRAVELLER !!
		;::::; :;
	  ;:::::'   :;											This file is very dangerous and deadly. Many coders have lost their life going through
	 ;:::::;     ;.											this code. You need programming skills to read a C++ file. YOU NEED EXTREME ENDURANCE
	,:::::'       ;           OOO\							TO READ GTA'S METHODS! The methods of this file have been under investigation for months
	::::::;       ;          OOOOO\							and many mysteries remain regarding their features. It is so long, nobody dares refactoring it.
	;:::::;       ;         OOOOOOOO						Previous investigators have found leaking holes in the depths and were sucked and trapped
   ,;::::::;     ;'         / OOOOOOO						by them. The leak was so strong, it was noticeable from the surface by users of the lake.
 ;:::::::::`. ,,,;.        /  / DOOOOOO						Fortunately, the majority of holes have been plugged by some brave code-divers. However,
.';:::::::::::::::::;,     /  /     DOOOO					some may remain and may trap you as well.
,::::::;::::::;;;;::::;,   /  /        DOOO
;`::::::`'::::::;;;::::: ,#/  /          DOOO				Do not make this mistake. If you are not sufficiently experienced with GTA code,
:`:::::::`;::::::;;::: ;::#  /            DOOO				PROCEED NO FURTHER.
::`:::::::`;:::::::: ;::::# /              DOO
`:`:::::::`;:::::: ;::::::#/               DOO
 :::`:::::::`;; ;:::::::::##                OO
 ::::`:::::::`;::::::::;:::#                OO
 `:::::`::::::::::::;'`:;::#                O
  `:::::`::::::::;' /  / `:#
   ::::::`:::::;'  /  /   `#
 ============================================================================================================================================================================*/


GTASCXMLControllerNew::GTASCXMLControllerNew(const QString& iTemplatePath)
{
	_pElement = nullptr;
	_ScxmlTemplatePath = iTemplatePath;
	_pAppCtrl = GTAAppController::getGTAAppController();
	stateIdCntr = 0;
	titleIdCntr = 0;
	GTACMDSCXMLUtils::setSCXMLTemplatePath(iTemplatePath);
}

GTASCXMLControllerNew::~GTASCXMLControllerNew()
{
	for (auto& cmdS : _pCmdSCXMLList)
	{
		if (cmdS != nullptr)
		{
			delete cmdS;
			cmdS = nullptr;
		}
	}
	_pCmdSCXMLList.clear();
}

/**
 * @brief Get the list of parameters involved in the procedure
 * @param iVariableList
 */
void GTASCXMLControllerNew::GetParamList(QStringList& iVariableList)
{
	if (_pAppCtrl != nullptr)
	{
		GTADataController DataCtrl;
		QStringList InvalidParamList;
		iVariableList.removeDuplicates();

		QStringList tempItems;
		QList<QStringList> complexQuery;
		int cntr = 0;

		foreach(QString item, iVariableList)
		{
			tempItems.append(item);
			if (cntr == 100)
			{
				complexQuery.append(tempItems);
				tempItems.clear();
				cntr = 0;
			}
			cntr++;
		}

		complexQuery.append(tempItems);
		foreach(QStringList items, complexQuery)
		{
			DataCtrl.getParametersDetail(items, _pAppCtrl->getGTADbFilePath(), _icdParamList, InvalidParamList);
		}

		for (auto& param : iVariableList)
		{
			for (auto& paramDB : _icdParamList.keys())
			{
				if (param.toUpper() == paramDB.toUpper())
				{
					int pos = iVariableList.indexOf(param);
					iVariableList.replace(pos,paramDB);
				}
			}
		}
	}
}


bool GTASCXMLControllerNew::getExecutionMode()
{
	return _executionMode;
}


/**
 * @brief Controls the export of the SCXML
 * @param pElement Current procedure element
 * @param fileName Name of the current document
 * @param FilePath Path of the current document
 * @param iVarList
 * @param mode
 * @return bool True if everything went well
 */
bool GTASCXMLControllerNew::exportToScxml(const GTAElement* ipElement, const QString& iFilePath, bool iHasUnsubscibeAtStart, bool iHasUnsubscibeAtEnd, QStringList& iVarList, GTAAppController::ExecutionMode mode)
{	
	qSetGlobalQHashSeed(0); // set a fixed hash value

	bool rc = false;
	if (ipElement == nullptr)
	{
		_LastError = "Error: Element is nullptr";
		return rc;
	}

	_executionMode = false;
	if (mode == GTAAppController::DEBUG_MODE)
		_executionMode = true;

	GTACMDSCXMLSCXML scxml;
	scxml.setDebugMode(_executionMode);
	QStringList variableList = iVarList;
	if (ipElement->getElementType() != GTAElement::CUSTOM)
	{
		_pElement = (GTAElement*)ipElement;
		setElementName(_pElement, iFilePath);
		_pElement->updateCommandInstanceName(QString());

		pTitleBasedElement = new GTAElement(_pElement->getElementType());

		QStringList icheckforRep;

		GroupElementByTitleComplete(_pElement, variableList, pTitleBasedElement, icheckforRep);

		if (!icheckforRep.isEmpty())
		{
			rc = false;
			_LastError = "Recursive calling of function/object/procedure";
			return rc;
		}
		pTitleBasedElement->setName(ipElement->getName());
	}
	else
	{
		_pElement = (GTAElement*)ipElement;
		setElementName(_pElement, iFilePath);
		pTitleBasedElement = new GTAElement(*_pElement);
	}

	QHash<QString, QString> engineVarMAp;
	TestConfig* testConf = TestConfig::getInstance();
	QString engSet = QString::fromStdString(testConf->getEngineSelection());
	bool isUSCXMLEn = testConf->getUSCXMLBasedSCXMLEN();
	bool channelSelection = testConf->getChannelSelection();

	//To prevent automatic insertion of CIC variables in data model if ICD with triplet defined in ChannelSelectionParamInfo.xml file is present in database
	if (channelSelection)
	{
		_EngineChannelSignals = GTAUtil::getChannelSelectionSignals(engSet, engineVarMAp);
		variableList.append(_EngineChannelSignals);
		
	}

	// update the list of parameters from the database.
	GetParamList(variableList);

	// get the state chart container
	rc = getCMDStateChartForElement(pTitleBasedElement);
	
	bool isStandaloneMode = testConf->getStandAloneExecMode();
	
	bool isSubscribeUnsubscribeTimeout = testConf->getSubscribeUnsubscribeTimeoutStatus();
	QString SubscribeUnsubscribeTimeout = QString::fromStdString(testConf->getSubscribeUnsubscribeTimeoutValue());

	bool isDoubleOk = false;
	double dTimeout = SubscribeUnsubscribeTimeout.toDouble(&isDoubleOk);
	if (isSubscribeUnsubscribeTimeout)
	{
		scxml.setSubUnsubTimeout(QString("%1ms").arg(QString::number(dTimeout * 1000)));
	}
	else
		scxml.setSubUnsubTimeout("1200000ms");

	scxml.setCommandList(_pCmdSCXMLList);
	scxml.setStandaloneMode(isStandaloneMode);
	scxml.setUnSubEnd(iHasUnsubscibeAtEnd);
	scxml.setUnSubStart(iHasUnsubscibeAtStart);
	scxml.setUnsubscribeFileParamOnly(getUnsubscribeFileParamOnly());

	int nIndex = iFilePath.lastIndexOf("/");
	QString fileName = iFilePath.right(iFilePath.size() - nIndex -1);

	QFile outFile(iFilePath);
	QTextStream out(stdout);
	outFile.open(QFile::WriteOnly | QFile::Text);

	QTextStream writer(&outFile);
	QString toolVersion = QString("<!--Generated by using GTA Version:%1-->\n").arg(TOOL_VERSION_STR);
	writer << toolVersion;
	GTACMDSCXMLUtils::setExportingFileName(fileName);
	QString xmlContents = scxml.getSCXMLString(isUSCXMLEn);
	xmlContents.replace("&#xa;", "");
	xmlContents.replace("&#xd;", "");

	writer << xmlContents;
	outFile.close();
	
	qSetGlobalQHashSeed(-1); // reset hash seed with new random value.
	return rc;
}

void GTASCXMLControllerNew::setElementName(GTAElement* pElement, QString iSaveFilePath)
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

void GTASCXMLControllerNew::GroupElementByTitleComplete(const GTAElement* pElem, QStringList& oVariableList, GTAElement* oGroupedElem, QStringList& icheckforRep)
{
	if (nullptr == pElem || nullptr == oGroupedElem)
		return;

	int cmdCnt = pElem->getDirectChildrenCount();
	QString uuid = pElem->getUuid();

	GTAActionTitle pTitle(ACT_TITLE);
	pTitle.setTitle("StartOfTestProcedure");
	GTAActionParentTitle* pTitleCmd = new GTAActionParentTitle(&pTitle, "");

	int titleChildCnt = 0;
	for (int i = 0; i < cmdCnt; i++)
	{
		GTACommandBase* pCmd1 = nullptr;
		pElem->getDirectChildren(i, pCmd1);

		if (pCmd1 != nullptr)
		{
			GTACommandBase* pCmd = pCmd1->getClone();
			if (pCmd == nullptr)
				continue;

			//check for global ignore step settings and generate SCXML accordingly
			if (pCmd->isIgnoredInSCXML())
				continue;

			QStringList pCmdVariableList = pCmd->getVariableList();
			QStringList& refpCmdVariableList = pCmdVariableList;
			updatedVariableList(refpCmdVariableList, oVariableList);

			if (pCmd->isTitle())
			{
				if (pTitleCmd != nullptr)
				{
					oGroupedElem->appendCommand(pTitleCmd);
				}

				GTAActionTitle* pActTitleCmd = dynamic_cast<GTAActionTitle*>(pCmd);
				pTitleCmd = new GTAActionParentTitle(pActTitleCmd, "");
				titleChildCnt = 0;

				//DEV comment:memory leak plug [God please no... no!]
				delete pActTitleCmd;
				pActTitleCmd = nullptr;
			}

			else if (pTitleCmd && pCmd->hasReference())
			{
				GTAActionCall* pCallCmd = dynamic_cast<GTAActionCall*>(pCmd);
				if (pCallCmd != nullptr)
				{
					GTAActionExpandedCall* pExtCall = new GTAActionExpandedCall(*pCallCmd);
					QString callElemName = pCallCmd->getElement();
					QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
					QString complement = pCallCmd->getComplement();
					pExtCall->setTagValue(pCallCmd->getTagValue());

					//Load the call document in the element;
					GTAElement* pElemOnCall = nullptr;
					GTAAppController* pController = GTAAppController::getGTAAppController();
					bool rc = pController->getElementFromDocument(pCallCmd->getRefrenceFileUUID(), pElemOnCall, true);

					if (rc && pElemOnCall != nullptr)
					{
						/*
						  grouping of a call element only if it was not called previously.
						  The grouping is limited to one recursive call
						  This is done to ensure there is no infinite recursion
						*/
						//check if a parent is called from a child. mark it as recursion if present.
						/*
						 fun1:
						 call fun2
							call fun3
								call fun4
									...
										call funN
						 under no circumstances can a nested function call its parent. Not allowed.
						 This becomes a recursion and causes an infinite loop.
						*/
						if (!icheckforRep.contains(uuid))
						{
							icheckforRep.append(uuid);
							pElemOnCall->replaceTag(pCallCmd->getTagValue());
							pElemOnCall->updateCommandInstanceName(pCallCmd->getInstanceName());

							GTAElement groupedCallElem(GTAElement::CUSTOM);
							GroupElementByTitleComplete(pElemOnCall, oVariableList, &groupedCallElem, icheckforRep);

							if (pElemOnCall != nullptr)
							{
								delete pElemOnCall;
								pElemOnCall = nullptr;
							}
							

							//remove the current element from the list to allow another call
							icheckforRep.removeOne(uuid);

							groupedCallElem.replaceTag(pCallCmd->getTagValue());

							int size = groupedCallElem.getDirectChildrenCount();
							int callChildCntr = 0;
							for (int j = 0; j < size; j++)
							{
								GTACommandBase* pCurCmd = nullptr;
								groupedCallElem.getDirectChildren(j, pCurCmd);

								if (pCurCmd != nullptr)
								{
									pExtCall->insertChildren(pCurCmd->getClone(), callChildCntr++);
								}
							}

//                          ============================calling monitor======================================
							//handle rest of the functions differently if a call monitor is present
							if (complement == COM_CALL_MON)
							{
								//put the rest of the commands into a dummy element and send it for further grouping.
								GTAElement dummyElement(GTAElement::CUSTOM);
								for (int k = i + 1; k < cmdCnt; k++)
								{
									GTACommandBase* pNextCmd = nullptr;
									pElem->getDirectChildren(k, pNextCmd);
									if (nullptr != pNextCmd)
										dummyElement.appendCommand(pNextCmd->getClone());
								}
								GTAElement* subGroupedCallElem = new GTAElement(GTAElement::CUSTOM);
								GroupElementByTitleComplete(&dummyElement, oVariableList, subGroupedCallElem, icheckforRep);

								//add the grouped element children under one title or as children of call monitor itself
								//device a new child list for monitor itself. Here you can call all commands that run parallel to the monitor
								GTAActionTitle pDummyTitle(ACT_TITLE);
								pDummyTitle.setTitle("ParallelProcedure");
								GTAActionParentTitle* pParentTitleCmd = new GTAActionParentTitle(&pDummyTitle, "");

								int parentChildCount = 0;
								for (int ii = 0; ii < subGroupedCallElem->getDirectChildrenCount(); ii++)
								{
									GTACommandBase* pCmd12 = nullptr;
									subGroupedCallElem->getDirectChildren(ii, pCmd12);
									if (pCmd12 != nullptr)
									{
										pParentTitleCmd->insertChildren(pCmd12->getClone(), parentChildCount++);
									}
								}

								if (pParentTitleCmd->hasChildren())
								{
									GTAActionTitle pFaltuTitle(ACT_TITLE);
									pFaltuTitle.setTitle("ParallelProcedure1");
									GTAActionParentTitle* pParentFaltuTitleCmd = new GTAActionParentTitle(&pFaltuTitle, "");

									int parallelChildCount = 0;
									for (int forchildren = 0; forchildren < pExtCall->getChildren().count(); forchildren++)
									{
										GTACommandBase* pCommand = pExtCall->getChildren().at(forchildren);
										if (pCommand != nullptr)
										{
											pParentFaltuTitleCmd->insertChildren(pCommand->getClone(), parallelChildCount++);
										}
									}

									pExtCall->clearChildrenList();
									pExtCall->insertChildren(pParentFaltuTitleCmd, 0);
									pExtCall->insertChildren(pParentTitleCmd, 1);
								}

								if (pTitleCmd != nullptr)
								{
									pTitleCmd->insertChildren(pExtCall, titleChildCnt++);
								}
								break;
							}

							if (pTitleCmd != nullptr)
							{
								pTitleCmd->insertChildren(pExtCall, titleChildCnt++);
							}
						}

						else
						{
							if (pTitleCmd != nullptr)
							{
								pExtCall->clearChildrenList();
								pTitleCmd->insertChildren(pExtCall, titleChildCnt++);
							}
						}
					}

					else
					{
						pExtCall->setBrokenLink(true);
					}
				}
			}

			else if (pTitleCmd != nullptr)
			{
				GTAActionCallProcedures* pCallParallel = dynamic_cast<GTAActionCallProcedures*>(pCmd);
				if (pCmd->canHaveChildren())
				{
					/*
					This code scope adds a condition command under a Title.
					Creation of new if-else commands named pIfCmd2 and pElseCmd2.
					These 2 commands are filled sequentially with children by using CreateCommandGroupedByTitle function to fetch children under a title(childrenTitle).
					*/
					GTAActionParentTitle* childrenTitle = nullptr;
					GTAActionIF* pIfCmd1 = dynamic_cast<GTAActionIF*>(pCmd);
					GTACommandBase* pElseCmd2 = nullptr;
					GTACommandBase* pIfCmd2 = nullptr;
					if (pIfCmd1 != nullptr)
					{
						pIfCmd2 = new GTAActionIF(*pIfCmd1);
						pIfCmd2->clearChildrenList();
						QList<GTACommandBase*> ifChildren = pIfCmd1->getChildren();

						//removing blank editor lines to prevent missing out children
						ifChildren.removeAll(nullptr);

						for (int j = 0; j < ifChildren.count(); j++)
						{
							GTACommandBase* pBaseCmd = ifChildren.at(j)->getClone();
							GTAActionBase* pActBase = dynamic_cast<GTAActionBase*>(pBaseCmd);
							GTACheckBase* pChkBase = dynamic_cast<GTACheckBase*>(pBaseCmd);

							if (pActBase != nullptr)
							{
								if ((pActBase->getAction() == ACT_CONDITION) && (pActBase->getComplement() == COM_CONDITION_ELSE))
								{
									pElseCmd2 = pBaseCmd;
									break;
								}
								else
								{
									pIfCmd2->insertChildren(pBaseCmd, j);
								}
							}
							else if (pChkBase)
							{
								pIfCmd2->insertChildren(pBaseCmd, j);
							}
						}

						createCommandGroupedByTitle(pIfCmd2, childrenTitle, oVariableList, icheckforRep);

						//clear any existing children as all children are now grouped under childrenTitle
						pIfCmd2->clearChildrenList();

						//add children of childrenTitle to the IfCmd2
						if (childrenTitle != nullptr)
						{
							QList<GTACommandBase*> childChildren = childrenTitle->getChildren();
							if (childChildren.count() > 0)
							{
								for (int j = 0; j < childChildren.count(); j++)
								{
									GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();
									if (pchildTitleCmd != nullptr)
									{
										pIfCmd2->insertChildren(pchildTitleCmd, j);
									}
								}
							}
							delete childrenTitle;
						}

						if (pElseCmd2 != nullptr)
						{
							GTAActionParentTitle* childrenTitle1 = nullptr;

							createCommandGroupedByTitle(pElseCmd2, childrenTitle1, oVariableList, icheckforRep);

							//clear any existing children as all children are now grouped under childrenTitle1
							pElseCmd2->clearChildrenList();

							//add children of childrenTitle1 to the pElseCmd2
							if (childrenTitle1 != nullptr)
							{
								QList<GTACommandBase*> childChildren = childrenTitle1->getChildren();
								if (childChildren.count() > 0)
								{
									for (int j = 0; j < childChildren.count(); j++)
									{
										GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();
										if (pchildTitleCmd != nullptr)
										{
											pElseCmd2->insertChildren(pchildTitleCmd, j);
										}
									}
								}
							}

							int ifChildrenCnt = (pIfCmd2->getChildren()).count();
							pIfCmd2->insertChildren(pElseCmd2, ifChildrenCnt);
							delete childrenTitle1;
						}

						if (pCmd->canBeCopied())
							pTitleCmd->insertChildren(pIfCmd2, titleChildCnt++);
					}

					else
					{
						/*
						This code scope adds end condition commands or Else Commands under a Title.
						The pCmd is filled with children by using CreateCommandGroupedByTitle function to fetch children under a title(childrenTitle).
						*/
						createCommandGroupedByTitle(pCmd, childrenTitle, oVariableList, icheckforRep);
						if (childrenTitle != nullptr)
						{
							//clear any existing children as all children are now grouped under childrenTitle
							pCmd->clearChildrenList();

							QList<GTACommandBase*> childChildren = childrenTitle->getChildren();
							if (childChildren.count() > 0)
							{
								for (int j = 0; j < childChildren.count(); j++)
								{
									GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();
									if (pchildTitleCmd != nullptr)
									{
										GTAActionIfEnd* ifEnd = dynamic_cast<GTAActionIfEnd*>(pchildTitleCmd);
										GTAActionWhileEnd* whileEnd = dynamic_cast<GTAActionWhileEnd*>(pchildTitleCmd);
										GTAActionDoWhileEnd* doWhileEnd = dynamic_cast<GTAActionDoWhileEnd*>(pchildTitleCmd);
										GTAActionForEachEnd* forEachEnd = dynamic_cast<GTAActionForEachEnd*>(pchildTitleCmd);

										if (!ifEnd && !whileEnd && !doWhileEnd && !forEachEnd)
											pCmd->insertChildren(pchildTitleCmd, j);
									}
								}
							}
							delete childrenTitle;
						}

						//an isolated else shouldn't be created in the editor itself
						//snippet added below as a failsafe
						GTAActionElse* pElseCmd = dynamic_cast<GTAActionElse*>(pCmd);
						if (pCmd->canBeCopied() || pElseCmd)
							pTitleCmd->insertChildren(pCmd, titleChildCnt++);
					}
				}

				else if (pCallParallel != nullptr)
				{
					GTACommandBase* pCallProcCmd = nullptr;
					GTAActionParentTitle* childrenGroupedTitle = nullptr;
					pCallProcCmd = new GTAActionCallProcedures(*pCallParallel);
					pCallProcCmd->clearChildrenList();

					//removing children because all parallel calls are saved in a list of GTACallProcItem
					//no harm done in removing children as it was never intended to have one
					//first we insert all parallel calls into it as call commands, then we simply semd it for grouping like a condition command
					pCallParallel->clearChildrenList();
					int lst = 0;
					for (auto& proc : pCallParallel->getCallProcedures())
					{
						QString subUuid = proc._UUID;
						GTAActionCall* pCall = new GTAActionCall();

						if (proc._type == GTACallProcItem::PROCEDURE)
							pCall->setElement(proc._elementName + ".pro");
						else
							pCall->setElement(proc._elementName + ".fun");

						pCall->setAction(ACT_CALL);
						pCall->setComplement(proc._type == GTACallProcItem::PROCEDURE ? COM_CALL_PROC : COM_CALL_FUNC);
						pCall->setObjId();
						pCall->setRefrenceFileUUID(subUuid);
						pCall->setTagValue(proc._tagValue);
						pCall->setIsParallelInSCXML(true);
						pCall->setInstanceName(pCallParallel->getInstanceName());
						pCallProcCmd->insertChildren(pCall, lst++);
					}

					createCommandGroupedByTitle(pCallProcCmd, childrenGroupedTitle, oVariableList, icheckforRep);

					//add children of childrenGroupedTitle to the pCallParallel
					pCallProcCmd->clearChildrenList();
					if (childrenGroupedTitle != nullptr)
					{
						QList<GTACommandBase*> childChildren = childrenGroupedTitle->getChildren();
						if (childChildren.count() > 0)
						{
							for (int j = 0; j < childChildren.count(); j++)
							{
								GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();
								if (pchildTitleCmd != nullptr)
								{
									pCallParallel->insertChildren(pchildTitleCmd, j);
								}
							}
						}
					}

					pCallProcCmd->clearChildrenList();
					childrenGroupedTitle->clearChildrenList();

					if (pCallProcCmd != nullptr)
					{
						delete  pCallProcCmd;
						pCallProcCmd = nullptr;
					}

					if (childrenGroupedTitle != nullptr)
					{
						delete  childrenGroupedTitle;
						childrenGroupedTitle = nullptr;
					}

					if (pCmd->canBeCopied() && pCallParallel->getChildren().count() > 0)
						pTitleCmd->insertChildren(pCallParallel, titleChildCnt++);
				}

				else if (pCmd->canBeCopied())
				{
					pTitleCmd->insertChildren(pCmd, titleChildCnt++);
				}
			}
		}
	}

	if (pTitleCmd != nullptr)
	{
		oGroupedElem->appendCommand(pTitleCmd);
	}
}

/**
* @brief Allows to group the element's command's commands by their titles
* @param ipCmd Targeted element's command
* @param ipTitleCmd output regrouped command
* @param oVariableList
* @param icheckforRep
*/
void GTASCXMLControllerNew::createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle*& ipTitleCmd, QStringList& oVariableList, QStringList& icheckforRep)
{
	int GlobalTitleCnt = 0;
	if (ipCmd != nullptr)
	{
		QList<GTACommandBase*> children = ipCmd->getChildren();
		QList<GTACommandBase*> childrenForParams;

		// let's make clones!
		for (const auto& child : children)
		{
			childrenForParams.append(child->getClone());
		}

		QList<GTACommandBase*> additionalChildren;
		for (const auto& child : childrenForParams)
		{
			if (child != nullptr && child->hasChildren())
			{
				for (const auto& addchild : child->getChildren())
				{
					additionalChildren.append(addchild->getClone());
				}
			}
		}

		for (const auto& additionalChild : additionalChildren)
			childrenForParams.append(additionalChild);
		additionalChildren.clear();

		GTAActionTitle pTitle(ACT_TITLE);
		pTitle.setTitle("StartOfTestProcedure");
		GTAActionParentTitle* pTitleCmd = new GTAActionParentTitle(&pTitle, "");

		int titleChildCnt = 0;
		if (childrenForParams.count() > children.count())
		{
			for (auto& pCmd1 : childrenForParams)
			{
				if (pCmd1 != nullptr)
				{
					GTACommandBase* pCmd = pCmd1->getClone();

					if (pCmd == nullptr)
						continue;

					if (pCmd->isIgnoredInSCXML())
						continue;

					if (ipCmd->getCommandType() == GTACommandBase::ACTION) 
					{
						GTAActionBase* pAct = dynamic_cast<GTAActionBase*>(ipCmd);

						if (pAct->getComplement() == COM_CONDITION_FOR_EACH) 
						{
							GTAActionForEach* pForEach = dynamic_cast<GTAActionForEach*>(pAct);

							//Get list of all children and add all parameters to subscribe for set and check. 
							if (pCmd->getCommandType() == GTACommandBase::ACTION)
							{
								GTAActionBase* pActChild = dynamic_cast<GTAActionBase*>(pCmd);

								//In this case we modify the set to act as multiset, with each of its set being a line in the csv file. 
								if (pActChild->getAction() == ACT_SET)
								{
									QStringList tempParamList;
									GTAActionSetList* pSetChild = dynamic_cast<GTAActionSetList*>(pActChild);

									QList <GTAActionSet*> setList;
									pSetChild->getSetCommandlist(setList);

									GTAActionSet* setToEdit = setList.at(0);
									QString param = setToEdit->getParameter();
									QString rightParam = setToEdit->getValue();

									if (rightParam.startsWith("line") && !param.startsWith("line"))
									{
										param = rightParam;
									}

									QString columnName = param.split("__").last();
									tempParamList = pForEach->getCSVColumnByTitle(columnName);
									updatedVariableList(tempParamList, oVariableList);
								}
							}

							else if (pCmd->getCommandType() == GTACommandBase::CHECK) 
							{
								GTACheckValue* pCheckChild = dynamic_cast<GTACheckValue*>(pCmd);
								QStringList tempParamList;
								QStringList columnNames;
								QStringList paramList = pCheckChild->getListParameters();

								foreach(QString param, paramList) 
								{
									if (param.startsWith("line")) 
									{
										columnNames.append(param.split("__").last());
									}
								}

								foreach(QString column, columnNames) 
								{
									tempParamList = pForEach->getCSVColumnByTitle(column);
									updatedVariableList(tempParamList, oVariableList);
								}
							}
						}
					}
					QStringList pCmdVariableList = pCmd->getVariableList();
					QStringList& refpCmdVariableList = pCmdVariableList;
					updatedVariableList(refpCmdVariableList, oVariableList);
				}
			}
		}

		int i = 0;
		for (auto& pCmd1 : children)
		{
			if (pCmd1 != nullptr)
			{
				GTACommandBase* pCmd = pCmd1->getClone();
				if (pCmd == nullptr)
					continue;

				if (pCmd->isIgnoredInSCXML())
					continue;

				if (ipCmd->getCommandType() == GTACommandBase::ACTION) 
				{
					GTAActionBase* pAct = dynamic_cast<GTAActionBase*>(ipCmd);

					if (pAct->getComplement() == COM_CONDITION_FOR_EACH) {
						GTAActionForEach* pForEach = dynamic_cast<GTAActionForEach*>(pAct);

						//Get list of all children and add all parameters to subscribe for set and check. 
						if (pCmd->getCommandType() == GTACommandBase::ACTION)
						{
							GTAActionBase* pActChild = dynamic_cast<GTAActionBase*>(pCmd);

							//In this case we modify the set to act as multiset, with each of its set being a line in the csv file. 
							if (pActChild->getAction() == ACT_SET)
							{
								QStringList tempParamList;
								GTAActionSetList* pSetChild = dynamic_cast<GTAActionSetList*>(pActChild);
								QList <GTAActionSet*> setList;
								pSetChild->getSetCommandlist(setList);
								GTAActionSet* setToEdit = setList.at(0);
								QString param = setToEdit->getParameter();
								QString rightParam = setToEdit->getValue();

								if (rightParam.startsWith("line") && !param.startsWith("line")) 
								{
									param = rightParam;
								}

								QString columnName = param.split("__").last();
								tempParamList = pForEach->getCSVColumnByTitle(columnName);
								updatedVariableList(tempParamList, oVariableList);
							}
						}

						else if (pCmd->getCommandType() == GTACommandBase::CHECK) 
						{
							GTACheckValue* pCheckChild = dynamic_cast<GTACheckValue*>(pCmd);
							QStringList tempParamList;
							QStringList columnNames;
							QStringList paramList = pCheckChild->getListParameters();

							foreach(QString param, paramList) 
							{
								if (param.startsWith("line")) 
								{
									columnNames.append(param.split("__").last());
								}
							}

							foreach(QString column, columnNames) {
								tempParamList = pForEach->getCSVColumnByTitle(column);
								updatedVariableList(tempParamList, oVariableList);
							}
						}
					}
				}

				QStringList pCmdVariableList = pCmd->getVariableList();
				QStringList& refpCmdVariableList = pCmdVariableList;
				updatedVariableList(refpCmdVariableList, oVariableList);

				if (pCmd->isTitle())
				{
					if (pTitleCmd != nullptr)
					{
						if (ipTitleCmd != nullptr)
						{
							GlobalTitleCnt = ipTitleCmd->getChildren().size();
							ipTitleCmd->insertChildren(pTitleCmd, GlobalTitleCnt++);
						}
						else
						{
							ipTitleCmd = new GTAActionParentTitle(*pTitleCmd); // need to delete pTitleCmd right after because redefined below
							delete pTitleCmd;
						}
					}

					GTAActionTitle* pActTitleCmd = dynamic_cast<GTAActionTitle*>(pCmd);
					pTitleCmd = new GTAActionParentTitle(pActTitleCmd, "");
					titleChildCnt = 0;

					//Jesus, this is so dirty...
					delete pActTitleCmd;
					pActTitleCmd = nullptr;

				}

				else if (pTitleCmd && pCmd->hasReference())
				{
					GTAActionCall* pCallCmd = dynamic_cast<GTAActionCall*>(pCmd);

					if (pCallCmd != nullptr)
					{
						GTAActionExpandedCall* pExtCall = new GTAActionExpandedCall(*pCallCmd);
						QString callElemName = pCallCmd->getElement();
						QString functionNameWithArg = pCallCmd->getFunctionWithArgument();
						pExtCall->setTagValue(pCallCmd->getTagValue());

						//Load the call document in the element;
						GTAElement* pElemOnCall = nullptr;
						GTAAppController* pController = GTAAppController::getGTAAppController();
						bool rc = pController->getElementFromDocument(pCallCmd->getRefrenceFileUUID(), pElemOnCall, true);

						if (rc && pElemOnCall != nullptr)
						{
							pElemOnCall->replaceTag(pCallCmd->getTagValue());
							pElemOnCall->updateCommandInstanceName(pCallCmd->getInstanceName());
							GTAElement groupedCallElem(GTAElement::CUSTOM);

							GroupElementByTitleComplete(pElemOnCall, oVariableList, &groupedCallElem, icheckforRep);

							if (pElemOnCall != nullptr)
							{
								delete pElemOnCall;
								pElemOnCall = nullptr;
							}

							groupedCallElem.replaceTag(pCallCmd->getTagValue());
							int size = groupedCallElem.getDirectChildrenCount();
							int callChildCntr = 0;

							for (int j = 0; j < size; j++)
							{
								GTACommandBase* pCurCmd = nullptr;
								groupedCallElem.getDirectChildren(j, pCurCmd);

								if (pCurCmd != nullptr)
								{
									pExtCall->insertChildren(pCurCmd->getClone(), callChildCntr++);
								}
							}

                            //handle rest of the functions differently if a call monitor is present
                            if (pCallCmd->getComplement() == COM_CALL_MON)
                            {
                                //put the rest of the commands into a dummy element and send it for further grouping.
                                GTAElement* dummyElement = new GTAElement(GTAElement::CUSTOM);
                                for (int k = i + 1; k < children.count(); k++)
                                {
                                    GTACommandBase* pNextCmd = nullptr;
                                    pNextCmd = children.at(k);
                                    if (nullptr != pNextCmd)
                                        dummyElement->appendCommand(pNextCmd->getClone());
                                }

                                GTAElement* subGroupedCallElem = new GTAElement(GTAElement::CUSTOM);
                                GroupElementByTitleComplete(dummyElement, oVariableList, subGroupedCallElem, icheckforRep);

                                //add the grouped element children under one title or as children of call monitor itself
                                //device a new child list for monitor itself. Here you can call all commands that run parallel to the monitor
                                GTAActionTitle pDummyTitle(ACT_TITLE);
                                pDummyTitle.setTitle("ParallelProcedure");
                                GTAActionParentTitle* pParentTitleCmd = new GTAActionParentTitle(&pDummyTitle, "");

                                int parentChildCount = 0;
                                for (int ii = 0; ii < subGroupedCallElem->getDirectChildrenCount(); ii++)
                                {
                                    GTACommandBase* pCmd12 = nullptr;
									subGroupedCallElem->getDirectChildren(ii, pCmd12);

                                    if (pCmd12 != nullptr)
                                    {
                                        pParentTitleCmd->insertChildren(pCmd12->getClone(), parentChildCount++);
                                    }
                                }

                                if (pParentTitleCmd->hasChildren())
                                {
                                    GTAActionTitle pFaltuTitle(ACT_TITLE);
                                    pFaltuTitle.setTitle("ParallelProcedure1");
                                    GTAActionParentTitle* pParentFaltuTitleCmd = new GTAActionParentTitle(&pFaltuTitle, "");

                                    int parallelChildCount = 0;
                                    for (int forchildren = 0; forchildren < pExtCall->getChildren().count(); forchildren++)
                                    {
                                        GTACommandBase* pCommand = pExtCall->getChildren().at(forchildren);
                                        if (pCommand != nullptr)
                                        {
                                            pParentFaltuTitleCmd->insertChildren(pCommand->getClone(), parallelChildCount++);
                                        }
                                    }

                                    pExtCall->clearChildrenList();
                                    pExtCall->insertChildren(pParentFaltuTitleCmd, 0);
                                    pExtCall->insertChildren(pParentTitleCmd, 1);

                                }

                                if (pTitleCmd != nullptr)
                                {
                                    pTitleCmd->insertChildren(pExtCall, titleChildCnt++);
                                }
								break;
							}

							if (pTitleCmd != nullptr)
							{
								pTitleCmd->insertChildren(pExtCall, titleChildCnt++);
							}
						}

						else
						{
							pExtCall->setBrokenLink(true);
						}
					}
				}

				else
				{
					GTAActionCallProcedures* pCallParallel = dynamic_cast<GTAActionCallProcedures*>(pCmd);

					if (pTitleCmd)
					{
						if (pCmd->canHaveChildren())
						{
							/*
							 This code scope adds a condition command under a Title.
							 Creation of new if-else commands named pIfCmd2 and pElseCmd2.
							 These 2 commands are filled sequentially with children by using CreateCommandGroupedByTitle function to fetch children under a title(childrenTitle).
							*/
							GTAActionParentTitle* childrenTitle = nullptr;
							GTAActionIF* pIfCmd1 = dynamic_cast<GTAActionIF*>(pCmd);
							GTACommandBase* pElseCmd2 = nullptr;
							GTACommandBase* pIfCmd2 = nullptr;

							if (pIfCmd1 != nullptr)
							{
								pIfCmd2 = new GTAActionIF(*pIfCmd1);
								pIfCmd2->clearChildrenList();
								QList<GTACommandBase*> ifChildren = pIfCmd1->getChildren();

								//removing blank editor lines to prevent missing out children
								ifChildren.removeAll(nullptr);

								for (int j = 0; j < ifChildren.count(); j++)
								{
									GTACommandBase* pBaseCmd = ifChildren.at(j);

									GTAActionBase* pActBase = dynamic_cast<GTAActionBase*>(pBaseCmd);
									GTACheckBase* pChkBase = dynamic_cast<GTACheckBase*>(pBaseCmd);

									if (pActBase != nullptr)
									{
										if ((pActBase->getAction() == ACT_CONDITION) && (pActBase->getComplement() == COM_CONDITION_ELSE))
										{
											pElseCmd2 = pBaseCmd;
											break;
										}

										else
										{
											pIfCmd2->insertChildren(pBaseCmd, j);
										}
									}

									else if (pChkBase)
									{
										pIfCmd2->insertChildren(pBaseCmd, j);
									}
								}

								createCommandGroupedByTitle(pIfCmd2, childrenTitle, oVariableList, icheckforRep);

								//clear any existing children as all children are now grouped under childrenTitle
								pIfCmd2->clearChildrenList();

								//add children of childrenTitle to the IfCmd2
								if (childrenTitle != nullptr)
								{
									QList<GTACommandBase*> childChildren = childrenTitle->getChildren();
									if (childChildren.count() > 0)
									{
										for (int j = 0; j < childChildren.count(); j++)
										{
											GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();
											if (pchildTitleCmd != nullptr)
											{
												pIfCmd2->insertChildren(pchildTitleCmd, j);
											}
										}
									}
									delete childrenTitle;
								}

								if (pElseCmd2 != nullptr)
								{
									GTAActionParentTitle* childrenTitle1 = nullptr;

									createCommandGroupedByTitle(pElseCmd2, childrenTitle1, oVariableList, icheckforRep);

									//clear any existing children as all children are now grouped under childrenTitle1
									pElseCmd2->clearChildrenList();

									//add children of childrenTitle1 to the pElseCmd2
									if (childrenTitle1 != nullptr)
									{
										QList<GTACommandBase*> childChildren = childrenTitle1->getChildren();
										if (childChildren.count() > 0)
										{
											for (int j = 0; j < childChildren.count(); j++)
											{
												GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();
												if (pchildTitleCmd != nullptr)
												{
													pElseCmd2->insertChildren(pchildTitleCmd, j);
												}
											}
										}
										delete childrenTitle1;
									}

									int ifChildrenCnt = (pIfCmd2->getChildren()).count();
									pIfCmd2->insertChildren(pElseCmd2, ifChildrenCnt);
								}

								if (pCmd->canBeCopied())
									pTitleCmd->insertChildren(pIfCmd2, titleChildCnt++);
							}

							else
							{
								/*
								This code scope adds end condition commands or Else Commands under a Title.
								Creation of new if-else commands named pIfCmd2 and pElseCmd2.
								These 2 commands are filled sequentially with children by using CreateCommandGroupedByTitle function to fetch children under a title(childrenTitle).
								*/
								createCommandGroupedByTitle(pCmd, childrenTitle, oVariableList, icheckforRep);

								if (childrenTitle != nullptr)
								{
									//clear any existing children as all children are now grouped under childrenTitle
									pCmd->clearChildrenList();

									QList<GTACommandBase*> childChildren = childrenTitle->getChildren();
									if (childChildren.count() > 0)
									{
										for (int j = 0; j < childChildren.count(); j++)
										{
											GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();
											if (pchildTitleCmd != nullptr)
											{
												GTAActionIfEnd* ifEnd = dynamic_cast<GTAActionIfEnd*>(pchildTitleCmd);
												GTAActionWhileEnd* whileEnd = dynamic_cast<GTAActionWhileEnd*>(pchildTitleCmd);
												GTAActionDoWhileEnd* doWhileEnd = dynamic_cast<GTAActionDoWhileEnd*>(pchildTitleCmd);
												GTAActionForEachEnd* forEachEnd = dynamic_cast<GTAActionForEachEnd*>(pchildTitleCmd);

												if (!ifEnd && !whileEnd && !doWhileEnd && !forEachEnd)
													pCmd->insertChildren(pchildTitleCmd, j);
											}
										}
									}
									delete childrenTitle;
								}

								//an isolated else shouldn't be created in the editor itself
								//snippet added below as a failsafe
								GTAActionElse* pElseCmd = dynamic_cast<GTAActionElse*>(pCmd);
								if (pCmd->canBeCopied() || pElseCmd)
									pTitleCmd->insertChildren(pCmd, titleChildCnt++);
							}
						}

						else if (pCallParallel != nullptr)
						{
							GTACommandBase* pCallProcCmd = nullptr;
							GTAActionParentTitle* childrenGroupedTitle = nullptr;

							pCallProcCmd = new GTAActionCallProcedures(*pCallParallel);
							pCallProcCmd->clearChildrenList();

							for (int lst = 0; lst < pCallParallel->getCallProcedures().count(); lst++)
							{
								GTACallProcItem proc = pCallParallel->getCallProcedures().at(lst);
								QString uuid = proc._UUID;
								GTAActionCall* pCall = new GTAActionCall();

								if (proc._type == GTACallProcItem::PROCEDURE)
									pCall->setElement(proc._elementName + ".pro");
								else
									pCall->setElement(proc._elementName + ".fun");

								pCall->setAction(ACT_CALL);
								pCall->setComplement(proc._type == GTACallProcItem::PROCEDURE ? COM_CALL_PROC : COM_CALL_FUNC);
								pCall->setObjId();
								pCall->setRefrenceFileUUID(uuid);
								pCall->setTagValue(proc._tagValue);
								pCall->setIsParallelInSCXML(true);
								pCall->setInstanceName(pCallParallel->getInstanceName());
								pCallProcCmd->insertChildren(pCall, lst);
							}

							createCommandGroupedByTitle(pCallProcCmd, childrenGroupedTitle, oVariableList, icheckforRep);

							//add children of childrenGroupedTitle to the pCallParallel
							pCallParallel->clearChildrenList();
							if (childrenGroupedTitle != nullptr)
							{
								QList<GTACommandBase*> childChildren = childrenGroupedTitle->getChildren();
								if (childChildren.count() > 0)
								{
									for (int j = 0; j < childChildren.count(); j++)
									{
										GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();
										if (pchildTitleCmd != nullptr)
										{
											pCallParallel->insertChildren(pchildTitleCmd, j);
										}
									}
								}
							}

							pCallProcCmd->clearChildrenList();
							childrenGroupedTitle->clearChildrenList();

							if (pCallProcCmd != nullptr)
							{
								delete  pCallProcCmd;
								pCallProcCmd = nullptr;
							}

							if (childrenGroupedTitle != nullptr)
							{
								delete  childrenGroupedTitle;
								childrenGroupedTitle = nullptr;
							}

							if (pCmd->canBeCopied() && pCallParallel->getChildren().count() > 0)
								pTitleCmd->insertChildren(pCallParallel, titleChildCnt++);
						}

						else if (pCmd->canBeCopied())
						{
							pTitleCmd->insertChildren(pCmd, titleChildCnt++);
						}
					}

					else
					{
						if (pCmd->canHaveChildren())
						{
							GTAActionParentTitle* childrenTitle = new GTAActionParentTitle(nullptr, "");
							createCommandGroupedByTitle(pCmd, childrenTitle, oVariableList, icheckforRep);

							if (childrenTitle != nullptr)
							{
								QList<GTACommandBase*> childChildren = childrenTitle->getChildren();

								if (childChildren.count() > 0)
								{
									pCmd->clearChildrenList();
									for (int j = 0; j < childChildren.count(); j++)
									{
										GTACommandBase* pchildTitleCmd = childChildren.at(j)->getClone();

										if (pchildTitleCmd != nullptr)
										{
											GTAActionIfEnd* ifEnd = dynamic_cast<GTAActionIfEnd*>(pchildTitleCmd);
											GTAActionWhileEnd* whileEnd = dynamic_cast<GTAActionWhileEnd*>(pchildTitleCmd);
											GTAActionDoWhileEnd* doWhileEnd = dynamic_cast<GTAActionDoWhileEnd*>(pchildTitleCmd);
											GTAActionForEachEnd* forEachEnd = dynamic_cast<GTAActionForEachEnd*>(pchildTitleCmd);

											if (!ifEnd && !whileEnd && !doWhileEnd && !forEachEnd)
											{
												pCmd->insertChildren(pchildTitleCmd, j);
											}
										}
									}
								}
								delete childrenTitle;
							}
						}

						GTAActionElse* pElseCmd = dynamic_cast<GTAActionElse*>(pCmd);
						if (pCmd->canBeCopied() || pElseCmd)
						{
							ipTitleCmd->insertChildren(pCmd, ipTitleCmd->getChildren().size());
						}
					}
				}
			}
			i++;
		}

		if (pTitleCmd != nullptr)
		{
			if (ipTitleCmd != nullptr)
			{
				GlobalTitleCnt = ipTitleCmd->getChildren().size();
				ipTitleCmd->insertChildren(pTitleCmd, GlobalTitleCnt++);
			}
			else
			{
				ipTitleCmd = new GTAActionParentTitle(*pTitleCmd);
				delete pTitleCmd;
			}
		}
	}
}

void GTASCXMLControllerNew::updatedVariableList(QStringList& iVarList, QStringList& oVarList)
{
	for (int i = 0;i < iVarList.count();i++)
	{
		QString param = iVarList.at(i);
		QStringList paramNames = _icdParamList.keys();
		for (const auto& paramInList : paramNames)
		{
			if (param != paramInList && param.toUpper() == paramInList.toUpper())
			{
				_VarList.removeAll(param);
				param = paramInList;	
			}
				
		}
		QStringList retParams = GTAUtil::getProcessedParameterForChannel(param, true, true);
		retParams.removeDuplicates();
		_VarList.append(retParams);
		_VarList.removeDuplicates();
		oVarList.append(retParams);
	}
	
}

/**
 * @brief Stores the element into a state chart container
 * @param pElement Current procedure element
 * @return bool True if everything went well
 */
bool GTASCXMLControllerNew::getCMDStateChartForElement(const GTAElement* pElement)
{
	
    bool rc = false;
    if(pElement != nullptr)
    {
		TestConfig* testConf = TestConfig::getInstance();
		QString engSet = QString::fromStdString(testConf->getEngineSelection());
		bool isGenNewLogFormat = testConf->getNewLogFormat();
		bool isUSCXMLEn = testConf->getUSCXMLBasedSCXMLEN();

		rc = true;
		int childrenCnt = pElement->getDirectChildrenCount();
		QString titleStateId = "";
		GTACMDSCXMLTitle* pCurrTitleSCXMLCmd = nullptr;
		for (int i = 0;i < childrenCnt;i++)
		{
			GTACommandBase* pCmd = nullptr;
			GTACommandBase* pNextCmd = nullptr;
			GTACMDSCXMLBase* pCMDSCXMLBase = nullptr;
			QString stateId;
			QString targetId;
			QString previousstateId;
			bool curCmd = pElement->getDirectChildren(i, pCmd);
			bool NxtCmd = pElement->getDirectChildren(i + 1, pNextCmd);

			//selective execution needs to identify the final state
			int j = i;
			while ((nullptr != pNextCmd) && (pNextCmd->isIgnoredInSCXML()))
			{
				if (j < childrenCnt - 1)
				{
					NxtCmd = pElement->getDirectChildren(j + 1, pNextCmd);
				}
				j++;

				//condition to break out of while when all commands after current command pCmd is ignoredinSCXML
				if ((j == childrenCnt) && (pNextCmd->isIgnoredInSCXML()))
				{
					pNextCmd = nullptr;
					break;
				}
			}

			if (curCmd && (pCmd != nullptr))
			{
				if (pCmd->isTitle())
				{
					titleStateId = QString("TS%1").arg(QString::number(titleIdCntr++));
					stateId = titleStateId;
					if ((titleIdCntr - 1) == 0)
					{
						previousstateId = stateId;
					}
					else
					{
						previousstateId = QString("TS%1").arg(QString::number(titleIdCntr - 2));
					}
				}
				else
				{
					stateId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr++));
					if ((stateIdCntr - 1) == 0)
					{
						previousstateId = stateId;
					}
					else
					{
						previousstateId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr - 2));
					}
				}

				if (pCurrTitleSCXMLCmd != nullptr && !pCurrTitleSCXMLCmd->getChildren().isEmpty())
				{
					GTACMDSCXMLBase* child = pCurrTitleSCXMLCmd->getChildren().last();
					int index = pCurrTitleSCXMLCmd->getChildren().indexOf(child);
					while (nullptr == child && index > 0)
					{
						child = pCurrTitleSCXMLCmd->getChildren().at(--index);
						if (index == 0)
							break;
					}
					if (nullptr != child)
						previousstateId = child->getStateId();
				}

				if (NxtCmd && pNextCmd != nullptr)
				{
					if (pNextCmd->isTitle())
					{
						targetId = QString("TS%1").arg(QString::number(titleIdCntr));
					}
					else
					{
						targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
					}
				}
				else
				{
					targetId = SCXML_MAIN_TP_FINAL;
				}

				GTACommandBase::CommandType  cmdType = pCmd->getCommandType();
				if (cmdType == GTACommandBase::ACTION)
				{
					GTAActionBase* pActCmd = dynamic_cast<GTAActionBase*>(pCmd);
					QString act = pActCmd->getAction();
					QString complement = pActCmd->getComplement();
					QString com = act;
					if (!complement.trimmed().isEmpty())
						com = QString("%1_%2").arg(act, complement);
					com.replace(" ", "_");

					if (act == ACT_PRINT)
					{
						GTACMDSCXMLPrint* pPrintSCXMLCmd = nullptr;
						GTACMDSCXMLPrintTable* pPrintTableSCXMLCmd = nullptr;
						GTACMDSCXMLPrintTime* pPrintTimeSCXMLCmd = nullptr;
						if (complement == COM_PRINT_MSG)
						{
							GTAActionPrint* pPrintCmd = dynamic_cast<GTAActionPrint*>(pActCmd);
							pPrintSCXMLCmd = new GTACMDSCXMLPrint(pPrintCmd, stateId, targetId);
							pPrintSCXMLCmd->setCommandName(com);
							pCMDSCXMLBase = pPrintSCXMLCmd;
						}
						else if (complement == COM_PRINT_PARAM)
						{
							GTAActionPrint* pPrintCmd = dynamic_cast<GTAActionPrint*>(pActCmd);
							pPrintSCXMLCmd = new GTACMDSCXMLPrint(pPrintCmd, stateId, targetId);
							pPrintSCXMLCmd->setCommandName(com);
							pCMDSCXMLBase = pPrintSCXMLCmd;
						}
						else if (complement == COM_PRINT_TABLE)
						{
							GTAActionPrintTable* pPrintTableCmd = dynamic_cast<GTAActionPrintTable*>(pActCmd);
							pPrintTableSCXMLCmd = new GTACMDSCXMLPrintTable(pPrintTableCmd, stateId, targetId);
							pPrintTableSCXMLCmd->setCommandName(com);
							pCMDSCXMLBase = pPrintTableSCXMLCmd;
						}
						else if (complement == COM_PRINT_TIME)
						{
							GTAActionPrintTime* pPrintTime = dynamic_cast<GTAActionPrintTime*>(pActCmd);
							pPrintTimeSCXMLCmd = new GTACMDSCXMLPrintTime(pPrintTime, stateId, targetId);
							pPrintTimeSCXMLCmd->setCommandName(com);
							pCMDSCXMLBase = pPrintTimeSCXMLCmd;
						}

					}
					else if (act == ACT_TITLE)
					{
						GTAActionParentTitle* pTitleCmd = dynamic_cast<GTAActionParentTitle*>(pActCmd);
						GTACMDSCXMLTitle* pTitleSCXMLCmd = new GTACMDSCXMLTitle(pTitleCmd, stateId, targetId);
						pTitleSCXMLCmd->setCommandName(com);

						if (pCurrTitleSCXMLCmd != nullptr)
						{
							_pCmdSCXMLList.append(pCurrTitleSCXMLCmd);
						}

						pCurrTitleSCXMLCmd = pTitleSCXMLCmd;
						QList<GTACommandBase*> children = pTitleCmd->getChildren();
						getCMDStateChartForCommand(children, pCurrTitleSCXMLCmd, previousstateId);
						if (NxtCmd && pNextCmd != nullptr)
						{
							if (pNextCmd->isTitle())
							{
								targetId = QString("TS%1").arg(QString::number(titleIdCntr));
							}
							else
							{
								targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
							}
						}
						else
						{
							targetId = SCXML_MAIN_TP_FINAL;
						}
						pCurrTitleSCXMLCmd->setTargetId(targetId);
						pCurrTitleSCXMLCmd->enableReportLogGen(isGenNewLogFormat);
					}
					else if (act == ACT_MATHS)
					{
						GTAActionMaths* pMaths = dynamic_cast<GTAActionMaths*>(pActCmd);
						GTACMDSCXMLMaths* pMathsCmdSCXML = new GTACMDSCXMLMaths(pMaths, stateId, targetId);
						pMathsCmdSCXML->setCommandName(com);
						pCMDSCXMLBase = pMathsCmdSCXML;
					}
				}

				QStringList variables = pCmd->getVariableList();
				QStringList dumpListVars = pCmd->getDumpList();
				variables.append(dumpListVars);
				variables.removeDuplicates();
				for (const auto& var : variables)
				{
					QStringList resolvedParams;
					QStringList temp;
					temp << var;
					updatedVariableList(temp, resolvedParams);
					for (const auto& resParam : resolvedParams)
					{
						for (auto& param : _icdParamList.keys())
						{
							QString upperParam = param.toUpper();
							if (resParam.toUpper() == upperParam)
							{
								pCMDSCXMLBase->insertParamInfoList(_icdParamList.value(param));
							}

							else
							{
								GTAICDParameter ICDParam;
								ICDParam.setSignalName(var);
								ICDParam.setTempParamName(var);
								ICDParam.setToolName("NA");
								pCMDSCXMLBase->insertParamInfoList(ICDParam);
							}
						}
						for (const auto& signal : _EngineChannelSignals)
						{
							if (_icdParamList.contains(signal))
							{
								pCMDSCXMLBase->insertParamInfoList(_icdParamList.value(signal));
							}
						}
					}
				}
				if (pCMDSCXMLBase != nullptr)
				{
					pCMDSCXMLBase->enableReportLogGen(isGenNewLogFormat);
				}
				if (pCMDSCXMLBase != nullptr)
				{
					pCMDSCXMLBase->setEngSettings(engSet);
					pCMDSCXMLBase->setUSCXMLEnStatus(isUSCXMLEn);
				}
				if (pCurrTitleSCXMLCmd != nullptr)
					pCurrTitleSCXMLCmd->append(pCMDSCXMLBase);
				else
					_pCmdSCXMLList.append(pCMDSCXMLBase);
			}
		}
		if (pCurrTitleSCXMLCmd != nullptr)
		{
			_pCmdSCXMLList.append(pCurrTitleSCXMLCmd);
			pCurrTitleSCXMLCmd->enableReportLogGen(isGenNewLogFormat);
		}
	}
	
	return rc;
}


/**
 * @brief Stores the commands list in state chart container
 * @param ipCmdList List of commands
 * @param ioParentState Parent CMDSCXML container
 * @param iPreviousStateID
 * @return bool True if everything went well
 */
bool GTASCXMLControllerNew::getCMDStateChartForCommand(const QList<GTACommandBase*> ipCmdList, GTACMDSCXMLBase* ioParentState, QString& iPreviousStateID)
{
    bool rc = false;
    if(!ipCmdList.isEmpty())
    {
		TestConfig* testConf = TestConfig::getInstance();
		QString engSet = QString::fromStdString(testConf->getEngineSelection());
		bool isGenNewLogFormat = testConf->getNewLogFormat();
		bool isUSCXMLEn = testConf->getUSCXMLBasedSCXMLEN();

		bool isEnReptLog = false; //settings.getRepeatedLoggingSetting();
		GTAAppController* pAppCtrl = GTAAppController::getGTAAppController();


		QString sHiddenDelayValue;
		QString sHiddenDelayValueUnit;
		rc = true;
		QString titleStateId = "";
		GTACMDSCXMLTitle* pCurrTitleSCXMLCmd = nullptr;
		bool isCICParampresent = false;
		QString previousstateId;

		for (int i = 0;i < ipCmdList.count();i++)
		{
			GTACommandBase* pCmd = nullptr;
			GTACommandBase* pNextCmd = nullptr;
			GTACMDSCXMLBase* pCMDSCXMLBase = nullptr;
			QString stateId;
			QString targetId;
			pCmd = ipCmdList.at(i);
			if (pCmd != nullptr && pCmd->isIgnoredInSCXML())
				continue;

			if (i < ipCmdList.count() - 1)
				pNextCmd = ipCmdList.at(i + 1);

			//selective execution needs to identify the final state
			int j = i;
			while ((nullptr != pNextCmd) && (pNextCmd->isIgnoredInSCXML()))
			{
				if (j < ipCmdList.count() - 1)
					pNextCmd = ipCmdList.at(j + 1);
				j++;

				//condition to break out of while when all commands after current command pCmd is ignoredinSCXML
				if ((j == ipCmdList.count()) && (pNextCmd->isIgnoredInSCXML()))
				{
					pNextCmd = nullptr;
					break;
				}
			}
			if (pCmd != nullptr)
			{
				if (pCmd->isTitle())
				{
					titleStateId = QString("TS%1").arg(QString::number(titleIdCntr++));
					stateId = titleStateId;
					if ((titleIdCntr - 1) == 0)
					{
						previousstateId = stateId;
					}
					else
					{
						//previous title
						previousstateId = QString("TS%1").arg(QString::number(titleIdCntr - 2));
					}
				}
				else
				{

					if (titleStateId.isEmpty())
					{
						titleStateId = ioParentState->getStateId();
					}
					stateId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr++));
					if ((stateIdCntr - 1) == 0)
					{
						previousstateId = stateId;
					}
					else
					{
						previousstateId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr - 2));
					}
				}

				//if the function is called recursively/based on first generation
				if (i == 0)
				{
					previousstateId = iPreviousStateID;
				}

				if (pNextCmd != nullptr)
				{
					if (pNextCmd->isTitle())
					{
						targetId = QString("TS%1").arg(QString::number(titleIdCntr));
					}
					else
					{
						targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
					}
				}
				else
				{
					targetId = QString("%1_%2").arg(ioParentState->getStateId(), SCXML_FINAL);
				}
				GTACommandBase::CommandType cmdType = pCmd->getCommandType();
				QString act;
				if (cmdType == GTACommandBase::ACTION)
				{
					GTAActionBase* pActCmd = dynamic_cast<GTAActionBase*>(pCmd);
					act = pActCmd->getAction();
					QString complement = pActCmd->getComplement();
					QString com = act;
					if (!complement.trimmed().isEmpty())
						com = QString("%1_%2").arg(act, complement);

					sHiddenDelayValue = pAppCtrl->getCommandProperty(QString(QString("%1 %2").arg(act, complement)).trimmed(), XATTR_INVOKE_DELAY);
					sHiddenDelayValueUnit = pAppCtrl->getCommandProperty(QString(QString("%1 %2").arg(act, complement)).trimmed(), XATTR_INVOKE_DELAY_UNIT);

					if (complement == COM_WAIT_UNTIL)
					{
						GTAActionWait* pWaitUntilcmd = dynamic_cast<GTAActionWait*>(pCmd);
						if (pWaitUntilcmd->getIsLoopSampling()) {
							int loopSampling;
							if (pWaitUntilcmd->getUnitLoopSampling() == "sec") {
								loopSampling = sHiddenDelayValue.toInt() + pWaitUntilcmd->getLoopSampling().toInt() * 1000;
							}
							else
							{
								loopSampling = sHiddenDelayValue.toInt() + pWaitUntilcmd->getLoopSampling().toInt();
							}
							// Divided by 2 because it's added to getParameter and evalCond
							sHiddenDelayValue = QString::number(loopSampling / 2);
						}


					}


					com.replace(" ", "_");
					if (act == ACT_PRINT)
					{
						GTACMDSCXMLPrint* pPrintSCXMLCmd = nullptr;
						GTACMDSCXMLPrintTable* pPrintTableSCXMLCmd = nullptr;
						GTACMDSCXMLPrintTime* pPrintTimeSCXMLCmd = nullptr;
						isEnReptLog = false; //true; @31/03/21 is unecessary 

						if (complement == COM_PRINT_MSG)
						{
							GTAActionPrint* pPrintCmd = dynamic_cast<GTAActionPrint*>(pActCmd);
							pPrintSCXMLCmd = new GTACMDSCXMLPrint(pPrintCmd, stateId, targetId);
							pPrintSCXMLCmd->setCommandName(com);
							pCMDSCXMLBase = pPrintSCXMLCmd;
						}
						else if (complement == COM_PRINT_PARAM)
						{
							GTAActionPrint* pPrintCmd = dynamic_cast<GTAActionPrint*>(pActCmd);
							pPrintSCXMLCmd = new GTACMDSCXMLPrint(pPrintCmd, stateId, targetId);
							isCICParampresent = pPrintCmd->hasChannelInControl();

							pPrintSCXMLCmd->setCommandName(com);
							pCMDSCXMLBase = pPrintSCXMLCmd;
						}
						else if (complement == COM_PRINT_TABLE)
						{
							GTAActionPrintTable* pPrintTableCmd = dynamic_cast<GTAActionPrintTable*>(pActCmd);
							pPrintTableSCXMLCmd = new GTACMDSCXMLPrintTable(pPrintTableCmd, stateId, targetId);
							isCICParampresent = pPrintTableCmd->hasChannelInControl();
							pPrintTableSCXMLCmd->setCommandName(com);
							pCMDSCXMLBase = pPrintTableSCXMLCmd;
						}
						else if (complement == COM_PRINT_TIME)
						{
							GTAActionPrintTime* pPrintTime = dynamic_cast<GTAActionPrintTime*>(pActCmd);
							pPrintTimeSCXMLCmd = new GTACMDSCXMLPrintTime(pPrintTime, stateId, targetId);
							pPrintTimeSCXMLCmd->setCommandName(com);
							isCICParampresent = pPrintTime->hasChannelInControl();
							pCMDSCXMLBase = pPrintTimeSCXMLCmd;
						}
					}
					else if (act == ACT_SUBSCRIBE)
					{
						// Initialize PIR Parameter contained in pActCmd 
						GTAActionSubscribe* pSubscribeCmd = dynamic_cast<GTAActionSubscribe*>(pActCmd);
						GTACMDSCXMLInitPIRParameters* pSubscribeSCXMLCmd = new GTACMDSCXMLInitPIRParameters(pSubscribeCmd, stateId, targetId);
						isCICParampresent = pSubscribeCmd->hasChannelInControl();
						pSubscribeSCXMLCmd->setCommandName(com);
						pCMDSCXMLBase = pSubscribeSCXMLCmd;
						// Filling the global List CurrentSubscribedPIR with local PIR Parameters
						CurrentsubscribedPIR.append(pSubscribeCmd->getVariableList()); 

						// Filling the global List CurrentSubscribedPIR with PIR Parameters in case of Line.PIR
						QStringList paramList;
						QStringList paramListTemp;
						QList<GTAICDParameter> paramListFinal;
						QList<QList<GTAICDParameter>>paramMultiList;

						if (pSubscribeCmd->getValues().at(0).startsWith("line")) {
							paramListTemp.append(pSubscribeCmd->getValues().at(0));
							getFilteredVariableList(paramListTemp, paramList, pSubscribeCmd->getInstanceName());
							GTACommandBase* cmdBase = pSubscribeCmd->getParent();
							GTAActionForEach* actionForEach = dynamic_cast<GTAActionForEach*>(cmdBase);
							for (const auto& value : pSubscribeCmd->getValues()) {
								QString title = value.split("__").at(1);
								QStringList CSVContent = actionForEach->getCSVColumnByTitle(title);
								QStringList paramListFromCSV;
								for (auto param : paramList) {
									if (CSVContent.contains(param)) {
										// Create a QHash that contains <line.PIR as a key , All PIR Parameters as values>
										LinePIRParameters.insertMulti(pSubscribeCmd->getValues().at(0) , param);
									}
								}

							}
						}

						
					}
					
					else if (act == ACT_UNSUBSCRIBE)
					{
						GTAActionUnSubscribe* pUnSubscribeCmd = dynamic_cast<GTAActionUnSubscribe*>(pActCmd);
						GTACMDSCXMLUnInitPIRParameters* pUnSubscribeSCXMLCmd = new GTACMDSCXMLUnInitPIRParameters(pUnSubscribeCmd, stateId, targetId);
						QStringList UnsubedVar = pUnSubscribeCmd->getVariableList(); 
						// Delete Unsubed PIR from global list CurrentsubscribedPIR
						if (!CurrentsubscribedPIR.isEmpty())
						{
							for (int n = 0; n < CurrentsubscribedPIR.count(); n++)
							{
								for (int m = 0; m < UnsubedVar.count(); m++)
								{
									if (CurrentsubscribedPIR.at(n) == UnsubedVar.at(m))
										CurrentsubscribedPIR.removeAt(n);
								}
							}
						}
						pUnSubscribeSCXMLCmd->setCommandName(com);
						isCICParampresent = pUnSubscribeCmd->hasChannelInControl();

						pCMDSCXMLBase = pUnSubscribeSCXMLCmd;

						// Delete All Unsubed PIR from global list CurrentsubscribedPIR in case of Release all
						if (pUnSubscribeCmd->isReleaseAll())
						{		
								for (const auto& resParam : CurrentsubscribedPIR)
								{
									if (_icdParamList.contains(resParam) && pCMDSCXMLBase != nullptr)
									{
										pCMDSCXMLBase->insertParamInfoList(_icdParamList.value(resParam));
										CurrentsubscribedPIR.removeAt(0);

									}
									else if (resParam.contains("line"))
									{
										QHashIterator<QString, QString> iter(LinePIRParameters);
										while (iter.hasNext())
										{
											iter.next();
											if (iter.key() == resParam)
											{
												if (_icdParamList.contains(iter.value()) && pCMDSCXMLBase != nullptr)
												{
													pCMDSCXMLBase->insertParamInfoList(_icdParamList.value(iter.value()));
												}
											}
										}
										CurrentsubscribedPIR.removeAt(0);
									}
							}
								if (isCICParampresent)
								{
									for (const auto& signal : _EngineChannelSignals)
									{
										if (_icdParamList.contains(signal))
										{
											pCMDSCXMLBase->insertParamInfoList(_icdParamList.value(signal));
										}
									}
								}
							}
							
						}

					else if (act == ACT_CALL_PROCS)
					{
						GTAActionCallProcedures* pCallProcs = dynamic_cast<GTAActionCallProcedures*>(pActCmd);
						GTACMDSCXMLCallParallel* pCallParallel = new GTACMDSCXMLCallParallel(pCallProcs, stateId, targetId);
						pCallParallel->setCommandName(com);
						isCICParampresent = pCallProcs->hasChannelInControl();


						for (int cont = 0;cont < pCallProcs->getChildren().count();cont++)
						{
							GTACommandBase* pCmdBase = pCallProcs->getChildren().at(cont);
							QString callStateID = QString("p%1_%2").arg(QString::number(cont), stateId);
							QString callTargetID = QString("p%1_%2").arg(QString::number(cont), targetId);

							GTACMDSCXMLContainerLoop* pLoopSCXMLCmd = new GTACMDSCXMLContainerLoop(pCmdBase, callStateID, callTargetID);
							pLoopSCXMLCmd->setCommandName(com);

							getCMDStateChartForCommand(pCmdBase->getChildren(), pLoopSCXMLCmd, previousstateId);
							pCallParallel->append(pLoopSCXMLCmd);
						}

						if (pNextCmd != nullptr)
						{
							if (pNextCmd->isTitle())
							{
								targetId = QString("TS%1").arg(QString::number(titleIdCntr));
							}
							else
							{
								targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
							}
						}
						else
						{
							targetId = QString("%1_%2").arg(titleStateId, SCXML_FINAL);
						}
						pCallParallel->setTargetId(targetId);

						pCMDSCXMLBase = pCallParallel;
					}
					else if (act == ACT_GEN_TOOL)
					{
						GTAGenericToolCommand* pGenToolCmd = dynamic_cast<GTAGenericToolCommand*>(pActCmd);
						GTACMDSCXMLExternalTool* pExtToolSCXMLCmd = new GTACMDSCXMLExternalTool(pGenToolCmd, stateId, targetId);
						pExtToolSCXMLCmd->setCommandName(com);
						isCICParampresent = pGenToolCmd->hasChannelInControl();

						pGenToolCmd->getCommand().count();
						QList<GTAGenericFunction> Functions = pGenToolCmd->getCommand();
						QString uuid = pGenToolCmd->getObjId();
						for (int x = 0; x < Functions.count(); x++)
						{
							GTAGenericFunction Function = Functions.at(x);
							QString stateID = QString("%1_fun_%2$%3").arg(stateId, QString::number(x), Function.getFunctionName().trimmed());
							_InternalCmdID.insertMulti(uuid, QString("%1_fun_%2$%3").arg(stateId, QString::number(x), Function.getFunctionName().trimmed()));
							_InternalStateID.insertMulti(stateID, pGenToolCmd->getInstanceName());
						}
						pCMDSCXMLBase = pExtToolSCXMLCmd;

					}
					else if (act == ACT_TITLE)
					{

						if (pCurrTitleSCXMLCmd != nullptr)
						{
							ioParentState->append(pCurrTitleSCXMLCmd);
						}

						GTAActionParentTitle* pTitleCmd = dynamic_cast<GTAActionParentTitle*>(pActCmd);
						GTACMDSCXMLTitle* pTitleSCXMLCmd = new GTACMDSCXMLTitle(pTitleCmd, stateId, targetId);
						pTitleSCXMLCmd->setCommandName(com);
						isCICParampresent = pTitleCmd->hasChannelInControl();

						pCurrTitleSCXMLCmd = pTitleSCXMLCmd;
						QList<GTACommandBase*> children = pTitleCmd->getChildren();
						getCMDStateChartForCommand(children, pCurrTitleSCXMLCmd, previousstateId);

						if (pNextCmd != nullptr)
						{
							if (pNextCmd->isTitle())
							{
								targetId = QString("TS%1").arg(QString::number(titleIdCntr));
							}
							else
							{
								targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
							}
						}
						else
						{
							targetId = QString("%1_%2").arg(ioParentState->getStateId(), SCXML_FINAL);
						}
						pCurrTitleSCXMLCmd->setTargetId(targetId);
						pCurrTitleSCXMLCmd->enableReportLogGen(isGenNewLogFormat);

					}
					else if (act == ACT_WAIT)
					{
						if (complement == COM_WAIT_FOR)
						{
							GTAActionWait* pWaitCmd = dynamic_cast<GTAActionWait*>(pActCmd);
							GTACMDSCXMLWaitFor* pWaitForSCXMLCmd = new GTACMDSCXMLWaitFor(pWaitCmd, stateId, targetId);
							pWaitForSCXMLCmd->setCommandName(com);
							isCICParampresent = pWaitCmd->hasChannelInControl();
							pCMDSCXMLBase = pWaitForSCXMLCmd;
						}
						else if (complement == COM_WAIT_UNTIL)
						{
							GTAActionWait* pWaitCmd = dynamic_cast<GTAActionWait*>(pActCmd);
							GTACMDSCXMLWaitUntil* pWaitUntilSCXMLCmd = new GTACMDSCXMLWaitUntil(pWaitCmd, stateId, targetId);
							isCICParampresent = pWaitCmd->hasChannelInControl();
							pWaitUntilSCXMLCmd->setCommandName(com);
							pCMDSCXMLBase = pWaitUntilSCXMLCmd;
						}
					}
					else if (act == ACT_REL)
					{
						GTAActionRelease* pReleaseCmd = dynamic_cast<GTAActionRelease*>(pActCmd);
						GTACMDSCXMLReleaseVMACParameters* pRelVMACSCXMLCmd = new GTACMDSCXMLReleaseVMACParameters(pReleaseCmd, stateId, targetId);
						pRelVMACSCXMLCmd->setCommandName(com);

						isCICParampresent = pReleaseCmd->hasChannelInControl();

						pCMDSCXMLBase = pRelVMACSCXMLCmd;
					}
					else if (act == ACT_CALL_EXT)
					{
						GTAActionExpandedCall* pCallCmd = dynamic_cast<GTAActionExpandedCall*>(pActCmd);
						GTACMDSCXMLCALL* pCALLSCXMLCmd = new GTACMDSCXMLCALL(pCallCmd, stateId, targetId);
						pCALLSCXMLCmd->setCommandName(com);
						isCICParampresent = pCallCmd->hasChannelInControl();
						QList<GTACommandBase*> children = pCallCmd->getChildren();
						bool isCallStateComplete = false;
						if (complement == COM_CALL_MON)
						{
							for (int cont = 0; cont < pCallCmd->getChildren().count(); cont++)
							{
								GTACommandBase* pCmdBase = pCallCmd->getChildren().at(cont);
								QString callStateID = QString("p%1_%2").arg(QString::number(cont), stateId);
								QString callTargetID = QString("p%1_%2").arg(QString::number(cont), targetId);

								GTACMDSCXMLContainerLoop* pLoopSCXMLCmd = new GTACMDSCXMLContainerLoop(pCmdBase, callStateID, callTargetID);
								pLoopSCXMLCmd->setCommandName(com);

								getCMDStateChartForCommand(pCmdBase->getChildren(), pLoopSCXMLCmd, previousstateId);
								pCALLSCXMLCmd->append(pLoopSCXMLCmd);
							}
							isCallStateComplete = true;
						}
						else
						{
							isCallStateComplete = getCMDStateChartForCommand(children, pCALLSCXMLCmd, previousstateId);
						}
						if (isCallStateComplete)
						{
							if (pNextCmd != nullptr)
							{
								if (pNextCmd->isTitle())
								{
									targetId = QString("TS%1").arg(QString::number(titleIdCntr));
								}
								else
								{
									targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
								}
							}
							else
							{
								targetId = QString("%1_%2").arg(titleStateId, SCXML_FINAL);
							}
							pCALLSCXMLCmd->setTargetId(targetId);
							//                            if(pCallCmd->getComplement() == COM_CALL_MON)
							//                            {
							//                                _Monitors.append(pCALLSCXMLCmd);
							//                            }
							//                            else
							pCMDSCXMLBase = pCALLSCXMLCmd;
						}
					}
					else if (act == ACT_CONDITION)
					{
						bool isTimoutIncluded = TestConfig::getInstance()->getLoopTimeoutSubsteps();

						if (complement == COM_CONDITION_IF)
						{
							// safe casts
							GTAActionIF* pIfElseCmd = dynamic_cast<GTAActionIF*>(pActCmd);
							isCICParampresent = pIfElseCmd->hasChannelInControl();

							// temporary objects
							GTAActionIF* _tempJustIfCmd = new GTAActionIF(*pIfElseCmd);
							GTAActionElse* _tempJustElseCmd = new GTAActionElse("", "");
							_tempJustElseCmd->setParent(_tempJustIfCmd);
							_tempJustIfCmd->clearChildrenList();

							// children reorganization
							//TODO: not messing with this 300 lines of madness
							#pragma warning(disable : 4456)
							int j = 0;
							for (const auto& originalChild : pIfElseCmd->getChildren())
							{
								GTACommandBase* child = originalChild->getClone();
								if (child != nullptr)
								{
									GTAActionBase* pActBase = dynamic_cast<GTAActionBase*>(child);
									GTACheckBase* pChkBase = dynamic_cast<GTACheckBase*>(child);
									if (pActBase != nullptr)
									{
										QString com1 = pActBase->getComplement();
										if (com1 == COM_CONDITION_ELSE)
										{
											int k = 0;
											for (const auto& elseChild : child->getChildren())
											{
												_tempJustElseCmd->insertChildren(elseChild, k++);
											}
											break;
										}
										else if (com1 == COM_CONDITION_ENDIF)
										{
											break;
										}
										else
										{
											_tempJustIfCmd->insertChildren(child, j++);
										}
									}
									else if (pChkBase != nullptr)
									{
										_tempJustIfCmd->insertChildren(child, j++);
									}
								}
							}

							QString elseStateId = QString("%1_Else").arg(stateId);
							QString ifStateId = QString("%1_If").arg(stateId);
							QString ifElseFinalId = QString("%1_final").arg(stateId);

							// children copy to decorrelate ownerships from originals
							const QList<GTACommandBase*> tempifChildren = _tempJustIfCmd->getChildren();
							const QList<GTACommandBase*> tempelseChildren = _tempJustElseCmd->getChildren();

							QList<GTACommandBase*> tempIf_children_copy;
							for (const auto& child : tempifChildren)
							{
								GTACommandBase* new_child = child->getClone();
								new_child->setParent(_tempJustIfCmd);
								tempIf_children_copy.append(new_child);
							}

							QList<GTACommandBase*> tempElse_children_copy;
							for (const auto& child : tempelseChildren)
							{
								GTACommandBase* new_child = child->getClone();
								new_child->setParent(_tempJustElseCmd);
								tempElse_children_copy.append(new_child);
							}

							// CMDSCXML sub-conditions
							GTACMDSCXMLConditionIf* pConditionIfSCXMLCmd = new GTACMDSCXMLConditionIf(_tempJustIfCmd, ifStateId, ifElseFinalId);
							pConditionIfSCXMLCmd->setCommandName(com);
							getCMDStateChartForCommand(tempIf_children_copy, pConditionIfSCXMLCmd, previousstateId);

							GTACMDSCXMLConditionElse* pConditionElseSCXMLCmd = new GTACMDSCXMLConditionElse(_tempJustElseCmd, elseStateId, ifElseFinalId);
							pConditionElseSCXMLCmd->setCommandName(com);
							getCMDStateChartForCommand(tempElse_children_copy, pConditionElseSCXMLCmd, previousstateId);

							if (pNextCmd != nullptr)
							{
								if (pNextCmd->isTitle())
								{
									targetId = QString("TS%1").arg(QString::number(titleIdCntr));
								}
								else
								{
									targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
								}
							}
							else
							{
								targetId = QString("%1_%2").arg(titleStateId, SCXML_FINAL);
							}

							// CMDSCXML condition
							GTACMDSCXMLConditionIfElse* pConditionIfElseSCXMLCmd = new GTACMDSCXMLConditionIfElse(pIfElseCmd, stateId, targetId);
							pConditionIfElseSCXMLCmd->setCommandName(com);
							pConditionIfElseSCXMLCmd->append(pConditionIfSCXMLCmd);
							pConditionIfElseSCXMLCmd->append(pConditionElseSCXMLCmd);

							// pointer copy
							pCMDSCXMLBase = pConditionIfElseSCXMLCmd;

							// destroy temporary if and else
							if (_tempJustIfCmd != nullptr)
							{
								delete _tempJustIfCmd;
								_tempJustIfCmd = nullptr;
							}

							if (_tempJustElseCmd != nullptr)
							{
								delete _tempJustElseCmd;
								_tempJustElseCmd = nullptr;
							}
						
							QString modifiedstateID = QString("%1_if$condition_if").arg(stateId);
							_InternalCmdID.insert(pIfElseCmd->getObjId(), modifiedstateID);
							_InternalStateID.insertMulti(modifiedstateID, pIfElseCmd->getInstanceName());						
						}

						else if (complement == COM_CONDITION_WHILE)
						{
							GTAActionWhile* pWhile = dynamic_cast<GTAActionWhile*>(pActCmd);
							isCICParampresent = pWhile->hasChannelInControl();

							// temporary object
							GTAActionWhile* tempWhile = new GTAActionWhile(*pWhile);
							tempWhile->clearChildrenList();

							// children reorganization
							int j = 0;
							for (const auto& originalChild : pWhile->getChildren())
							{
								GTACommandBase* child = originalChild->getClone();
								if (child != nullptr)
								{
									GTAActionBase* pActBase = dynamic_cast<GTAActionBase*>(child);
									GTACheckBase* pChkBase = dynamic_cast<GTACheckBase*>(child);
									if (pActBase != nullptr)
									{
										QString com1 = pActBase->getComplement();
										if (com1 == COM_CONDITION_ENDWHILE)
										{
											break;
										}
										else
										{
											tempWhile->insertChildren(child, j++);
										}
									}
									else if (pChkBase != nullptr)
									{
										tempWhile->insertChildren(child, j++);
									}
								}
							}

							const QList<GTACommandBase*> tempWhile_children = tempWhile->getChildren();
							QList<GTACommandBase*> tempWhile_children_copy; // decorrelate the ownerships
							for (const auto& child : tempWhile_children)
							{
								GTACommandBase* new_child = child->getClone();
								new_child->setParent(pWhile);
								tempWhile_children_copy.append(new_child);
							}

							// CMDSCXML container
							QString loopContStateId = QString("%1_WhileStates").arg(stateId);
							QString loopContTargetId = QString("%1_GetParametrs").arg(stateId);
							GTACMDSCXMLContainerLoop* pConditionLoopSCXMLCmd = new GTACMDSCXMLContainerLoop(pWhile, loopContStateId, loopContTargetId);
							pConditionLoopSCXMLCmd->setCommandName(com);
							getCMDStateChartForCommand(tempWhile_children_copy, pConditionLoopSCXMLCmd, previousstateId);

							if (pNextCmd != nullptr)
							{
								if (pNextCmd->isTitle())
								{
									targetId = QString("TS%1").arg(QString::number(titleIdCntr));
								}
								else
								{
									targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
								}
							}
							else
							{
								targetId = QString("%1_%2").arg(titleStateId, SCXML_FINAL);
							}

							// CMDSCXML condition
							GTACMDSCXMLConditionWhile* pConditionWhileSCXMLCmd = new GTACMDSCXMLConditionWhile(pWhile, stateId, targetId);
							pConditionWhileSCXMLCmd->setCommandName(com);
							pConditionWhileSCXMLCmd->append(pConditionLoopSCXMLCmd);
							pConditionWhileSCXMLCmd->setLoopTimeoutInInternalStates(isTimoutIncluded);

							// pointer copy
							pCMDSCXMLBase = pConditionWhileSCXMLCmd;

							// destroy temporary while
							if (tempWhile != nullptr)
							{
								delete tempWhile;
								tempWhile = nullptr;
							}
						}

						else if (complement == COM_CONDITION_DO_WHILE)
						{
							GTAActionDoWhile* pDoWhile = dynamic_cast<GTAActionDoWhile*>(pActCmd);
							isCICParampresent = pDoWhile->hasChannelInControl();

							// temporary object
							GTAActionDoWhile* tempDoWhile = new GTAActionDoWhile(*pDoWhile);
							tempDoWhile->clearChildrenList();

							// children reorganization
							int j = 0;
							//Restore sanity warnings
							#pragma warning(default : 4456)
							for (const auto& originalChild : pDoWhile->getChildren())
							{
								GTACommandBase* child = originalChild->getClone();
								if (child != nullptr)
								{
									GTAActionBase* pActBase = dynamic_cast<GTAActionBase*>(child);
									GTACheckBase* pChkBase = dynamic_cast<GTACheckBase*>(child);
									if (pActBase != nullptr)
									{
										QString com1 = pActBase->getComplement();
										if (com1 == COM_CONDITION_DO_WHILE_END)
										{
											break;
										}
										else
										{
											tempDoWhile->insertChildren(child, j++);
										}
									}
									else if (pChkBase != nullptr)
									{
										tempDoWhile->insertChildren(child, j++);
									}
								}
							}

							const QList<GTACommandBase*> tempDoWhile_children = tempDoWhile->getChildren();
							QList<GTACommandBase*> tempDoWhile_children_copy; // decorrelate the ownerships
							for (const auto& child : tempDoWhile_children)
							{
								GTACommandBase* new_child = child->getClone();
								new_child->setParent(pDoWhile);
								tempDoWhile_children_copy.append(new_child);
							}

							// CMDSCXML container
							QString loopContStateId = QString("%1_DoWhileStates").arg(stateId);
							QString loopContTargetId = QString("%1_GetParametrs").arg(stateId);
							GTACMDSCXMLContainerLoop* pConditionLoopSCXMLCmd = new GTACMDSCXMLContainerLoop(pDoWhile, loopContStateId, loopContTargetId);
							pConditionLoopSCXMLCmd->setCommandName(com);
							getCMDStateChartForCommand(tempDoWhile_children_copy, pConditionLoopSCXMLCmd, previousstateId);

							if (pNextCmd != nullptr)
							{
								if (pNextCmd->isTitle())
								{
									targetId = QString("TS%1").arg(QString::number(titleIdCntr));
								}
								else
								{
									targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
								}
							}
							else
							{
								targetId = QString("%1_%2").arg(titleStateId, SCXML_FINAL);
							}

							// CMDSCXML condition
							GTACMDSCXMLConditionDoWhile* pConditionDoWhileSCXMLCmd = new GTACMDSCXMLConditionDoWhile(pDoWhile, stateId, targetId);
							pConditionDoWhileSCXMLCmd->setCommandName(com);
							pConditionDoWhileSCXMLCmd->setLoopTimeoutInInternalStates(isTimoutIncluded);
							pConditionDoWhileSCXMLCmd->append(pConditionLoopSCXMLCmd);

							// pointer copy
							pCMDSCXMLBase = pConditionDoWhileSCXMLCmd;

							// destroy temporary dowhile
							if (tempDoWhile != nullptr)
							{
								delete tempDoWhile;
								tempDoWhile = nullptr;
							}
						}

						else if (complement == COM_CONDITION_FOR_EACH)
						{
							isEnReptLog = false;
							GTAActionForEach* pForEach = dynamic_cast<GTAActionForEach*>(pActCmd);
							isCICParampresent = pForEach->hasChannelInControl();

							// temporary object
							GTAActionForEach* tempForEach = new GTAActionForEach(*pForEach);
							tempForEach->clearChildrenList();

							//adding the loop id in list
							QHash<QString, QString> map;
							QString uuid = pForEach->getObjId();
							map.insert(uuid, stateId);
							_LoopID.prepend(map);

							// children reorganization
							int l = 0;
							for (const auto& originalChild : pForEach->getChildren())
							{
								GTACommandBase* child = originalChild->getClone();
								if (child != nullptr)
								{
									GTAActionBase* pActBase = dynamic_cast<GTAActionBase*>(child);
									GTACheckBase* pChkBase = dynamic_cast<GTACheckBase*>(child);
									if (pActBase != nullptr)
									{
										QString com1 = pActBase->getComplement();
										if (com1 == COM_CONDITION_FOR_EACH_END)
										{
											break;
										}
										else
										{
											tempForEach->insertChildren(child, l);
										}
									}
									else if (pChkBase != nullptr)
									{
										tempForEach->insertChildren(child, l);
									}
								}
								l++;
							}

							const QList<GTACommandBase*> tempForEach_children = tempForEach->getChildren();
							QList<GTACommandBase*> tempForEach_children_copy; // decorrelate the ownerships
							for (const auto& child : tempForEach_children)
							{
								GTACommandBase* new_child = child->getClone();
								new_child->setParent(pForEach);
								tempForEach_children_copy.append(new_child);
							}

							// CMDSCXML container
							QString loopContStateId = QString("%1_ForEachStates").arg(stateId);
							QString loopContTargetId = QString("%1_Decision").arg(stateId);
							GTACMDSCXMLContainerLoop* pConditionLoopSCXMLCmd = new GTACMDSCXMLContainerLoop(pForEach, loopContStateId, loopContTargetId);
							pConditionLoopSCXMLCmd->setCommandName(com);
							getCMDStateChartForCommand(tempForEach_children_copy, pConditionLoopSCXMLCmd, previousstateId);

							//adding the loop id in list
							if (!_LoopID.isEmpty())
								_LoopID.removeOne(map);

							if (pNextCmd != nullptr)
							{
								if (pNextCmd->isTitle())
								{
									targetId = QString("TS%1").arg(QString::number(titleIdCntr));
								}
								else
								{
									targetId = QString("%1_s%2").arg(titleStateId, QString::number(stateIdCntr));
								}
							}
							else
							{
								targetId = QString("%1_%2").arg(titleStateId, SCXML_FINAL);
							}

							// CMDSCXML condition
							GTACMDSCXMLConditionForEach* pConditionForEachSCXMLCmd = new GTACMDSCXMLConditionForEach(pForEach, stateId, targetId);
							pConditionForEachSCXMLCmd->setCommandName(com);
							pConditionForEachSCXMLCmd->setLoopTimeoutInInternalStates(isTimoutIncluded);
							pConditionForEachSCXMLCmd->append(pConditionLoopSCXMLCmd);

							// pointer copy
							pCMDSCXMLBase = pConditionForEachSCXMLCmd;

							// destroy temporary foreach
							if (tempForEach != nullptr)
							{
								delete tempForEach;
								tempForEach = nullptr;
							}
						}
					}

					else if (act == ACT_SET)
					{
						GTAActionSetList* pSetList = dynamic_cast<GTAActionSetList*>(pActCmd);
						GTACMDSCXMLSet* pSetSCXMLCmd = new GTACMDSCXMLSet(pSetList, stateId, targetId);
						pSetSCXMLCmd->setCommandName(com);
						isCICParampresent = pSetList->hasChannelInControl();
						//                        QString uuid = pSetList->getObjId();
						QList<GTAActionSet*> pSets;
						pSetList->getSetCommandlist(pSets);

						QString HiddenDelayVMACValue = pAppCtrl->getCommandProperty(QString("set VMAC").trimmed(), XATTR_INVOKE_DELAY);
						QString HiddenDelayVMACValueUnit = pAppCtrl->getCommandProperty(QString("set VMAC").trimmed(), XATTR_INVOKE_DELAY_UNIT);

						int delay = 0;
						if (HiddenDelayVMACValueUnit != "ms")
						{
							delay = HiddenDelayVMACValue.toInt() * 1000;
						}
						else
						{
							delay = HiddenDelayVMACValue.toInt();
						}
						if (delay > 0)
							pSetSCXMLCmd->setVMACHiddenDelay(QString::number(delay) + "ms");


						//                        QString uuid = pSets.count() > 0 ? pSets.at(0)->getObjId() : "";
						for (const auto& set : pSets)
						{
							QString uuid = set->getObjId();
							//                            _InternalCmdID.insert(uuid,QString("%1_SetCmd_%2_SetParam_i0$set").arg(stateId,QString::number(i)));
							QString modifiedstateID = QString("%1_SetCmd_SetParam_0$set").arg(stateId);
							_InternalCmdID.insert(uuid, modifiedstateID);
							_InternalStateID.insertMulti(modifiedstateID, pSetList->getInstanceName());
						}
						//                        _InternalCmdID.insert(uuid,QString("%1_SetCmd_0_SetParam_0_Return_0$set").arg(stateId));
						pCMDSCXMLBase = pSetSCXMLCmd;
					}
					else if (act == ACT_MANUAL)
					{
						GTACMDSCXMLManualAction* pManualActionSCXMLCmd = nullptr;
						GTAActionManual* pManualActCmd = dynamic_cast<GTAActionManual*>(pActCmd);
						pManualActionSCXMLCmd = new GTACMDSCXMLManualAction(pManualActCmd, stateId, targetId);

						TestConfig* subTestConf = TestConfig::getInstance();

						if (testConf->getManualActionTimeoutStatus())
						{
							QString manActTimeout = QString::fromStdString(subTestConf->getManualActionTimeoutValue());
							bool isDoubleOk = false;
							double dTimeout = manActTimeout.toDouble(&isDoubleOk);
							pManualActionSCXMLCmd->setTimeout(QString::number(dTimeout * 1000));
						}
						isCICParampresent = pManualActCmd->hasChannelInControl();

						pManualActionSCXMLCmd->setCommandName(com);

						pCMDSCXMLBase = pManualActionSCXMLCmd;
					}
					else if (act == ACT_GEN_FUNC)
					{
						GTAActionGenericFunction* pGenFunc = dynamic_cast<GTAActionGenericFunction*>(pActCmd);
						isCICParampresent = pGenFunc->hasChannelInControl();
					}
					else if (act == ACT_MATHS)
					{
						GTAActionMaths* pMaths = dynamic_cast<GTAActionMaths*>(pActCmd);
						GTACMDSCXMLMaths* pMathsCmdSCXML = new GTACMDSCXMLMaths(pMaths, stateId, targetId);
						pMathsCmdSCXML->setCommandName(com);
						isCICParampresent = pMaths->hasChannelInControl();

						QString uuid = pMaths->getObjId();
						QString modifiedstateID = QString("%1_MathsSetParam_SetParam_i0$Maths").arg(stateId);
						_InternalCmdID.insert(uuid, modifiedstateID);
						_InternalStateID.insertMulti(modifiedstateID, pMaths->getInstanceName());
						pCMDSCXMLBase = pMathsCmdSCXML;
					}
				}
				else if (cmdType == GTACommandBase::CHECK)
				{
					GTACheckBase* pCheckCmd = dynamic_cast<GTACheckBase*>(pCmd);

					//DEV: Route everything through CheckBase class

					GTACheckBase::CheckType chkType = pCheckCmd->getCheckType();
					if (chkType == GTACheckBase::VALUE)
					{
						GTACheckValue* pChkValCmd = dynamic_cast<GTACheckValue*>(pCmd);
						GTACMDSCXMLCheckValue* pChkValSCXMLCmd = new GTACMDSCXMLCheckValue(pChkValCmd, stateId, targetId);
						isCICParampresent = pChkValCmd->hasChannelInControl();

						QString com = QString("%1_%2").arg("CHECK", pChkValCmd->getCheckName());
						com.replace(" ", "_");
						pChkValSCXMLCmd->setCommandName(com);

						sHiddenDelayValue = pAppCtrl->getCommandProperty(QString(QString("%1").arg(pChkValCmd->getCheckName())).trimmed(), XATTR_INVOKE_DELAY);
						sHiddenDelayValueUnit = pAppCtrl->getCommandProperty(QString(QString("%1").arg(pChkValCmd->getCheckName())).trimmed(), XATTR_INVOKE_DELAY_UNIT);

						pCMDSCXMLBase = pChkValSCXMLCmd;

						QString modifiedstateID = QString("%1_evalCond_check_value$check").arg(stateId);
						_InternalCmdID.insert(pCheckCmd->getObjId(), modifiedstateID);
						_InternalStateID.insertMulti(modifiedstateID, pCheckCmd->getInstanceName());
					}
					else if (chkType == GTACheckBase::FWC_WARNING)
					{
						GTACheckFwcWarning* pFWCCmd = dynamic_cast<GTACheckFwcWarning*>(pCmd);
						GTACMDSCXMLCheckFWCMessage* pChkFWCSCXMLCmd = new GTACMDSCXMLCheckFWCMessage(pFWCCmd, stateId, targetId);
						QString com = QString("%1_%2").arg("CHECK", "FWC_Message");
						com.replace(" ", "_");
						pChkFWCSCXMLCmd->setCommandName(com);
						isCICParampresent = pFWCCmd->hasChannelInControl();

						sHiddenDelayValue = pAppCtrl->getCommandProperty(QString("FWC Warning"), XATTR_INVOKE_DELAY);
						sHiddenDelayValueUnit = pAppCtrl->getCommandProperty(QString("FWC Warning"), XATTR_INVOKE_DELAY_UNIT);

						pCMDSCXMLBase = pChkFWCSCXMLCmd;
					}
					else if (chkType == GTACheckBase::BITE_MESSAGE)
					{
						GTACheckBiteMessage* pBiteCmd = dynamic_cast<GTACheckBiteMessage*>(pCmd);
						GTACMDSCXMLCheckBITEMessage* pChkBITESCXMLCmd = new GTACMDSCXMLCheckBITEMessage(pBiteCmd, stateId, targetId);
						QString com = QString("%1_%2").arg("CHECK", "BITE_Message");
						com.replace(" ", "_");
						pChkBITESCXMLCmd->setCommandName(com);
						isCICParampresent = pBiteCmd->hasChannelInControl();

						sHiddenDelayValue = pAppCtrl->getCommandProperty(QString("BITE_Message"), XATTR_INVOKE_DELAY);
						sHiddenDelayValueUnit = pAppCtrl->getCommandProperty(QString("BITE_Message"), XATTR_INVOKE_DELAY_UNIT);

						pCMDSCXMLBase = pChkBITESCXMLCmd;
					}
					else if (chkType == GTACheckBase::AUDIO_ALARM)
					{
						// GTACheckAudioAlarm *pAudioAlarm = dynamic_cast<GTACheckAudioAlarm *>(pCmd);
					}
				}
				else
				{
					//invalid command;
				}

				if (nullptr != pCMDSCXMLBase)
				{
					pCMDSCXMLBase->setPreviousStateId(previousstateId);
					pCMDSCXMLBase->setDebugModeStatus(_executionMode);    //DEV: variable which tells me if debug mode is enabled
				}


				QStringList variables;

				QStringList dumpListVars = pCmd->getDumpList();
				QStringList tempVariables;
				

				//This has been added to exclude External tool variables to be added in data model of SCXML.
				//Fix for Tuleap issue 258626 and 258623

				if (act != ACT_GEN_TOOL && (cmdType == GTACommandBase::ACTION || cmdType == GTACommandBase::CHECK))
				{
					tempVariables.append(dumpListVars);
					tempVariables.append(pCmd->getVariableList());
				}

			



				//replace internal variables with relevant scxml values
				getFilteredVariableList(tempVariables, variables, pCmd->getInstanceName());

				QRegExp regExp("_uuid\\[\\{[a-zA-Z0-9]{8}-([a-zA-Z0-9]{4}-){3}[a-zA-Z0-9]{12}\\}\\]");

				variables.removeDuplicates();
				for (int k = 0;k < variables.count();k++)
				{
					QString var = variables.at(k);
					QStringList resolvedParams;
					QStringList temp;
					temp << var;
					updatedVariableList(temp, resolvedParams);
					for (const auto& resParam : resolvedParams)
					{
						if (pCMDSCXMLBase != nullptr)
						{
							QStringList paramListTemp = _icdParamList.keys();
							QStringList paramListTempUpper = _icdParamList.keys();

							for (auto &param : paramListTempUpper)
							{
								param = param.toUpper();
							}
							if (paramListTempUpper.contains(resParam.toUpper()) && !resParam.contains(regExp))
							{
								int pos = paramListTempUpper.lastIndexOf(resParam.toUpper());
								QString paramName = paramListTemp[pos];
								pCMDSCXMLBase->insertParamInfoList(_icdParamList.value(paramName));  //.value(resParam));
							}
							else if (resParam.contains(regExp))
							{
								GTAICDParameter param;
								// int pos = regExp.indexIn(var);
								QStringList list = regExp.capturedTexts();
								param.setUUID(list.first().remove("_uuid"));
								var = var.remove(regExp);
								//param.setSignalName(var);
								param.setTempParamName(var);
								param.setToolName("NA");
								param.setToolType(INTERNAL_PARAM);
								pCMDSCXMLBase->insertLoopInfoList(resParam.split(".").last(), param);
								pCMDSCXMLBase->insertParamInfoList(param);
							}
							else
							{
								GTAICDParameter param;
								//param.setSignalName(var);
								param.setTempParamName(var);
								param.setToolName("NA");
								pCMDSCXMLBase->insertParamInfoList(param);
							}
						}
					}
				}
				if (isCICParampresent)
				{
					for (const auto& signal : _EngineChannelSignals)
					{
						if (_icdParamList.contains(signal))
						{
							pCMDSCXMLBase->insertParamInfoList(_icdParamList.value(signal));
						}
					}
				}

				if (pCMDSCXMLBase != nullptr)
				{
					pCMDSCXMLBase->enableRepitativeLogging(isEnReptLog);
					pCMDSCXMLBase->enableReportLogGen(isGenNewLogFormat);
					//              QString cmdName = pCMDSCXMLBase->getCommandName();
					//              QStringList internalVars = _pAppCtrl->getInternalVariablesForCmd(cmdName);
					//              foreach(QString item,internalVars)
					//              {
					//QString location = QString("%1:%2$%5").arg(cmdName,pActCmd->getObjId(),returnParamName.trimmed());
					//location.replace(" ","_");
					//                  GTACMDSCXMLUtils::insertGenToolReturnParams(location,item);
					//              }
				}

				if (pCMDSCXMLBase != nullptr)
				{
					int delay = 0;
					if (sHiddenDelayValueUnit != "ms")
					{
						delay = sHiddenDelayValue.toInt() * 1000;
					}
					else
					{
						delay = sHiddenDelayValue.toInt();
					}
					if (delay > 0)
						pCMDSCXMLBase->setHiddenDelay(QString::number(delay) + "ms");

					pCMDSCXMLBase->setEngSettings(engSet);
					pCMDSCXMLBase->setUSCXMLEnStatus(isUSCXMLEn);
					if (pCurrTitleSCXMLCmd != nullptr)
						pCurrTitleSCXMLCmd->append(pCMDSCXMLBase);
					else
						ioParentState->append(pCMDSCXMLBase);
				}
			}
		}
		if (pCurrTitleSCXMLCmd != nullptr)
		{
			ioParentState->append(pCurrTitleSCXMLCmd);
			ioParentState->enableReportLogGen(isGenNewLogFormat);
			pCurrTitleSCXMLCmd->enableReportLogGen(isGenNewLogFormat);
		}
	}
	return rc;
}

//this function handles the loop variables and includes them into variable list with proper name
void GTASCXMLControllerNew::getFilteredVariableList(QStringList variablesTemp, QStringList& variables, const QString& iInstanceName)
{
	bool addVars = false;
	foreach(QString variable, variablesTemp) {
		if (variable.contains("line.")) {
			addVars = true;
		}
	}
	if (addVars) {
		variablesTemp.append(_VarList);
	}
	QRegExp regExp("_uuid\\[\\{[a-zA-Z0-9]{8}-([a-zA-Z0-9]{4}-){3}[a-zA-Z0-9]{12}\\}\\]");
	foreach(QString variable, variablesTemp)
	{
		if (variable.contains(regExp))
		{
			if (!_LoopID.isEmpty() && variable.contains("line."))
			{
				QString loopstateID = "";
				QString vartemp = variable;
				for (int i = 0;i < _LoopID.count();i++)
				{
					//Map< uuid, _LoopID >
					QHash<QString, QString> loopIDMap = _LoopID.at(i);
					QHash<QString, QString>::iterator j;
					for (j = loopIDMap.begin(); j != loopIDMap.end(); ++j)
					{
						QString uuid = j.key();
						if (vartemp.contains(uuid))
						{
							//variable.remove(QString("_uuid[%1]").arg(uuid));
							loopstateID = j.value();
						}
					}
				}
				if (loopstateID != "")
				{
					variable = QString("%1_line_arr[%1_arr_cntr].%2").arg(loopstateID, variable.remove("line.")).trimmed();
					variable.replace(QString("%2%1").arg(variable.split(INTERNAL_PARAM_SEP).last(), INTERNAL_PARAM_SEP), "");

					//add the variables to the list
					variables.append(variable);
					continue;
				}
			}
			if (!_InternalCmdID.isEmpty())
			{
				QString stateID = "";
				QString vartemp = variable;
				QHash<QString, QString>::iterator j;
				for (j = _InternalCmdID.begin(); j != _InternalCmdID.end(); ++j)
				{
					QString uuid = j.key();
					QString temp = j.value();
					QString instance = _InternalStateID.value(temp);
					if (!instance.trimmed().isEmpty())
					{
						QStringList levels = instance.split("#");
						//the instance in the hash contains the name of the specific (external tool, set, check, etc. command)
						//we need to remove this and also the line number of that command
						//change this code only if you know what you are doing. Most importantly, DO NOT break External tool!
						levels.removeLast();
						instance = levels.join("#");
						instance.remove(instance.length() - 3, 3);
					}
					if (vartemp.contains(uuid) && vartemp.contains(temp.split("$").last()) && (!instance.isEmpty() && iInstanceName.contains(instance)))
					{
						stateID = j.value();
						stateID = temp.split("$").first();
						stateID.replace("$", "");
					}
				}
				if (stateID != "")
				{
					variable = QString("%1_%2").arg(stateID, variable.trimmed());
					variable.replace(QString("%2%1").arg(variable.split(INTERNAL_PARAM_SEP).last(), INTERNAL_PARAM_SEP), "");

					//add the variables to the list
					variables.append(variable);
					continue;
				}
			}
		}
		//add the variables to the list
		variables.append(variable);
		variables.removeDuplicates();
	}

}

void GTASCXMLControllerNew::setUnsubscibeFileParamOnly(const bool unsubscibeFileParamOnly)
{
	_unsubscribeFileParamOnly = unsubscibeFileParamOnly;
}