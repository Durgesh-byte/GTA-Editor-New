#include "GTAElement.h"
#include "GTAComment.h"
#include "GTACheckBase.h"
#include "GTAActionElse.h"
#include "GTAUtil.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionPrintTable.h"
#include "GTAActionParentTitle.h"
#include "GTAActionParentTitleEnd.h"
#include "GTAActionFTAKinematicMultiOutput.h"
#include "GTAActionOneClickTitle.h"
#include "GTAActionOneClickParentTitle.h"
#include "GTAActionRelease.h"
#include "GTAActionFailure.h"
#include "GTAActionParentTitleEnd.h"
#include "GTAGenericToolCommand.h"
#include <QDateTime>
#include <QDebug>

GTAElement::GTAElement(ElemType iType)
{
	setElementType(iType);
	_pHeader = NULL;
	initialize(1);
	setUuid(QUuid::createUuid().toString());
	setSaveSatus(GTAElement::SaveOK);
	_bSaveAsReadOnly = false;
	_GtaVersion = QString(TOOL_VERSION_STR);
}

GTAElement::GTAElement(ElemType iType, const int& noOfDefaultChildrens)
{
	setElementType(iType);
	_pHeader = NULL;
	initialize(noOfDefaultChildrens);

	setUuid(QUuid::createUuid().toString());
	setSaveSatus(GTAElement::SaveOK);
	_bSaveAsReadOnly = false;
	_GtaVersion = QString(TOOL_VERSION_STR);
}
GTAElement::GTAElement(ElemType iType, QList<GTACommandBase*>& ipCmdlst)
{
	setElementType(iType);
	_pHeader = NULL;
	_ElemType = iType;
	int noOfChildrens = ipCmdlst.size();
	setUuid(QUuid::createUuid().toString());
	setSaveSatus(GTAElement::SaveOK);
	_bSaveAsReadOnly = false;
	_GtaVersion = QString(TOOL_VERSION_STR);

	if (noOfChildrens>0)
	{
		for (int i = 0; i<noOfChildrens; i++)
		{
			_lstMasterCommands.insert(i, ipCmdlst.at(i));
		}
	}

}

GTAElement::GTAElement(const GTAElement &ipElem)
{
	_ElemType = ipElem._ElemType;
	//    ========================================
	_ElementName = ipElem._ElementName;
	_AuthName = ipElem._AuthName;
	_ValidatorName = ipElem._ValidatorName;
	_ValidationStatus = ipElem._ValidationStatus;
	_ValidationTime = ipElem._ValidationTime;
	_CreatedDate = ipElem._CreatedDate;
	_ModifiedDate = ipElem._ModifiedDate;

	setSaveSatus(ipElem.getSaveStatus());
	_hiddenRows = ipElem._hiddenRows;
	_FSIIRefFileName = ipElem._FSIIRefFileName;
	_ExecStart = ipElem._ExecStart;
	_ExecEnd = ipElem._ExecEnd;
	//    ========================================

	int noOfChildrens = ipElem._lstMasterCommands.size();

	_Uuid = ipElem._Uuid;
	_pHeader = ipElem._pHeader;

	_bSaveAsReadOnly = false;
	_AbsoluteFilePath = ipElem.getAbsoluteFilePath();
	_RelativeFilePath = ipElem.getRelativeFilePath();
	_GtaVersion = ipElem.getGtaVersion();
	for (int i = 0; i<noOfChildrens; i++)
	{
		GTACommandBase* pCmd = ipElem._lstMasterCommands.at(i);
		if (pCmd != NULL)
			_lstMasterCommands.append(pCmd->getClone());
		else
			_lstMasterCommands.append(pCmd);
	}
	setTagVarStructList(ipElem.getTagList());
}

GTAElement::~GTAElement()
{
	ClearAll();
}

void GTAElement::initialize(int nbOfChildren)
{
	_pHeader = NULL;

	_lstMasterCommands.append(NULL);


	if (nbOfChildren>0)
	{
		for (int i = 0; i<nbOfChildren - 1; i++)
		{
			_lstMasterCommands.push_back(NULL);

		}
	}
}
void GTAElement::setElementType(ElemType iType)
{
	_ElemType = iType;
}

GTAElement::ElemType GTAElement::getElementType() const
{
	return _ElemType;
}
void GTAElement::setName(const QString & iVal)
{
	_ElementName = iVal;
}

QString GTAElement::getName() const
{
	return _ElementName;
}
// V26 document viewer column enhancement ------>
void GTAElement::setAuthorName(const QString &iAuthName)
{
	_AuthName = iAuthName;
}

QString GTAElement::getAuthorName()const
{
	return _AuthName;
}

void GTAElement::setValidatorName(const QString &iValidatorName)
{
	_ValidatorName = iValidatorName;
}

QString GTAElement::getValidatorName()const
{
	return _ValidatorName;
}

void GTAElement::setValidationStatus(const QString &iValidationStat)
{
	_ValidationStatus = iValidationStat;
}

QString GTAElement::getValidationStatus()const
{
	return _ValidationStatus;
}

void GTAElement::setValidationTime(const QString &iValidationTime)
{
	_ValidationTime = iValidationTime;
}

QString GTAElement::getValidationTime()const
{
	return _ValidationTime;
}

void GTAElement::setCreationTime(const QString &iCreatedTime)
{
	_CreatedDate = iCreatedTime;
}

QString GTAElement::getCreationTime()const
{
	return _CreatedDate;
}

void GTAElement::setModifiedTime(const QString &iModifiedTime)
{
	_ModifiedDate = iModifiedTime;
}

QString GTAElement::getModifiedTime()const
{
	return _ModifiedDate;
}

/**
 * @brief Set the Maximum Time Estimated value
 * @param iMaxTimeEstimated 
*/
void GTAElement::setMaxTimeEstimated(const QString& iMaxTimeEstimated)
{
	_MaxTimeEstimated = iMaxTimeEstimated;
}

/**
 * @brief Get the Maximum Time Estimated value
 * @return QString
*/
QString GTAElement::getMaxTimeEstimated()const
{
	return _MaxTimeEstimated;
}

/**
 * @brief Set the input CSV file path
 * @param iInputCsvFilePath
*/
void GTAElement::setInputCsvFilePath(const QString& iInputCsvFilePath)
{
	_InputCsvFilePath = iInputCsvFilePath;
}

/**
 * @brief Set the updated CSV file path
 * @param iOutputCsvFilePath
*/
void GTAElement::setOutputCsvFilePath(const QString& iOutputCsvFilePath)
{
	_OutputCsvFilePath = iOutputCsvFilePath;
}

/**
 * @brief Get the input CSV file path
 * @return QString
*/
QString GTAElement::getInputCsvFilePath()const
{
	return _InputCsvFilePath;
}

/**
 * @brief Get the updated CSV file path
 * @return QString
*/
QString GTAElement::getOutputCsvFilePath()const
{
	return _OutputCsvFilePath;
}

/**
 * @brief Set the Purpose and conclusion to document element
 * @param Purpose - value of Purpose
*/
void GTAElement::setPurposeForProcedure(const QString& iPurpose)
{
	_PurposeForProcedure = iPurpose;
}

/**
 * @brief Get the Purpose value from element
 * @return Purpose - value of Purpose
*/
QString GTAElement::getPurposeForProcedure() const
{
	return _PurposeForProcedure;
}

/**
 * @brief Set the summary and conclusion to document element
 * @param conclusion - value of conclusion
*/
void GTAElement::setConclusionForProcedure(const QString& iConclusion)
{
	_ConclusionForProcedure = iConclusion;
}

/**
 * @brief Get the conclusion value from element
 * @return conclusion - value of summary 
*/
QString GTAElement::getConclusionForProcedure() const
{
	return _ConclusionForProcedure;
}

// <-------V26 document viewer  column enhancement 
void GTAElement::setHeader(GTAHeader *& ipHeader)
{
	_pHeader = ipHeader;
}

GTAHeader * GTAElement::getHeader()
{
	return _pHeader;
}
//bool GTAElement::insertEmptyCommand(int iIndex)
//{
//    int nextIndex = iIndex + 1;
//    bool rc = insertCommand(NULL,nextIndex);

//    //Shift the comments to new position
//    int incrementIndexBy = 1;
//    updateCommentsPosition(iIndex,incrementIndexBy);
//    return rc;
//}

bool GTAElement::removeCommand(const int& iIndex)
{
	bool bOK = false;

	//get command to be removed from element
	int itemOnIndex = iIndex;
	GTACommandBase* pCommanTobeRemoved = NULL;
	getCommand(itemOnIndex, pCommanTobeRemoved);

	//get command child count if any there for updating comment position
	int cmdChildrenCount = 0;
	if (pCommanTobeRemoved != NULL)
		cmdChildrenCount = pCommanTobeRemoved->getAllChildrenCount();

	//remove the command
	int itemToRemoveFromIndex = iIndex;
	removeCommand(itemToRemoveFromIndex, _lstMasterCommands, bOK);


	//if (bOK )
	//{
	//    //remove the comments for given index which is beside to command
	//    for (int i = iIndex;i<=iIndex+cmdChildrenCount;i++)
	//    {
	//        removeComment(i);
	//    }

	//    //bring the command to upward
	//    int decrementIndexBy = -1*( cmdChildrenCount+1);
	//    updateCommentsPosition(iIndex,decrementIndexBy);
	//}
	if (bOK) setSaveSatus(GTAElement::Modified);
	return bOK;
}


std::optional<GTACommandBase*> GTAElement::getParentForIndex(int index)
{
	auto allCmds = GetAllCommands();
	if (index <= allCmds.size()) {
		return getParentOfCommand(index);
	}

	return std::nullopt;
}

/**
 * @brief Overload of getParentForIndex which uses command instead of index as input
*/
std::optional<GTACommandBase*> GTAElement::getParentOfCommand(const GTACommandBase* cmd) {
	auto allCmds = GetAllCommands();
	auto thisIdx = getIndexOfCommand(cmd);
	if (thisIdx) {
		return getParentOfCommand(*thisIdx);
	}
	
	return std::nullopt;
}

std::optional<GTACommandBase*> GTAElement::getParentOfCommand(int cmdIndex) {
	auto allCmds = GetAllCommands();
	//look for nearest command with children above, if the last child of given command is below searched index then it's the parent
	for (int i = cmdIndex - 1; i >= 0; --i) {
		auto& potentialParent = allCmds.at(i);
		if (potentialParent && potentialParent->hasChildren()) {
			auto& children = potentialParent->getChildren();
			auto lastChildIdx = getIndexOfCommand(children.last());
			if (lastChildIdx && lastChildIdx > cmdIndex) {
				return potentialParent;
			}
		}
	}

	return std::nullopt;
}

//updates command 
bool GTAElement::updateCommand(const int & iIndex, GTACommandBase* ipCommand)
{
	int index = iIndex;
	bool bFoundStatus = false;
	//int totalElementBeforeUpdate = this->getAllChildrenCount();//required to compute unassigned commands
	if (iIndex == 0)
	{
		if (_lstMasterCommands.count() >0)
		{
			_lstMasterCommands[0] = ipCommand;
			bFoundStatus = true;
		}
	}
	else
		updateCommand(index, ipCommand, _lstMasterCommands, bFoundStatus);

	if (bFoundStatus) setSaveSatus(GTAElement::Modified);
	return bFoundStatus;
}


void GTAElement::updateCommand(int& iIndex, GTACommandBase * ipCmdToUpdate, QList<GTACommandBase*> &isList, bool& bFound)
{
	int commandListSize = isList.size();
	for (int i = 0; i<commandListSize; ++i)
	{
		GTACommandBase* pCommand = isList.at(i);
		if (iIndex == 0)
		{
			GTACommandBase *pCmdOnIndex = isList.at(i);
			if (pCmdOnIndex != NULL && ipCmdToUpdate != NULL)
			{
				GTACommandBase * pParentOfCmdOnIndex = pCmdOnIndex->getParent();
				ipCmdToUpdate->setParent(pParentOfCmdOnIndex);
				delete pCmdOnIndex;
				pCmdOnIndex = NULL;
			}

			isList[i] = ipCmdToUpdate;
			bFound = true;
			return;
		}
		else
		{
			if (NULL != pCommand)
			{
				QList <GTACommandBase*>& childList = pCommand->getChildren();
				if (!childList.isEmpty())
				{
					iIndex--;
					updateCommand(iIndex, ipCmdToUpdate, childList, bFound);
					if (bFound)
						return;
				}
				else
					iIndex--;
			}
			else
				iIndex--;
		}
	}


}
bool GTAElement::getCommand(const int& iIndex, GTACommandBase*& oPCommand) const
{
	int index = iIndex;
	bool bFoundStatus = false;
	if (iIndex == 0)
	{
		oPCommand = _lstMasterCommands.at(0);
		bFoundStatus = true;
	}
	else
	{
		getCommand(index, oPCommand, (QList<GTACommandBase*>)_lstMasterCommands, bFoundStatus);
	}
	return bFoundStatus;

}

bool GTAElement::hasIndex(const int& iIndex)
{
	int currentSize = getAllChildrenCount();
	bool rC = false;
	if (iIndex<currentSize)
		rC = true;
	return rC;


}
int GTAElement::getAllChildrenCount(void) const
{
    // int sizeParentList = _lstMasterCommands.size();
    int totalSize =0;
    foreach(GTACommandBase* pCmd,_lstMasterCommands)
    {
        totalSize++;
        int currentItems=0;
        if (pCmd)
        {
            currentItems=pCmd->getAllChildrenCount();
            if (currentItems)
                totalSize = totalSize + currentItems;
        }

    }
    return totalSize;//_lstMasterCommands.size();
}
int GTAElement::getDirectChildrenCount() const
{
	return _lstMasterCommands.size();

}

bool GTAElement::getDirectChildren(const int& iIndex, GTACommandBase*& oPBase) const
{
	bool rC = false;
	int currentSize = _lstMasterCommands.size();
	if (iIndex<currentSize  && iIndex >= 0)
	{
		oPBase = _lstMasterCommands[iIndex];
		rC = true;
	}
	return rC;

}
bool GTAElement::ClearAll()
{
	GTACommandBase* pSampleCmd = NULL;
	_lstMasterCommands.removeAll(pSampleCmd);
	int count = _lstMasterCommands.count();
	for (int i = 0; i < count; i++)
	{
		GTACommandBase* pCmd = _lstMasterCommands.at(i);
		if (NULL != pCmd)
		{
			delete pCmd;
			pCmd = NULL;
		}
	}
	_lstMasterCommands.clear();
	initialize(0);

	return true;

}
void GTAElement::getCommand(int& iIndex, GTACommandBase*& oPCommand, const QList<GTACommandBase*> &isList, bool& bFound) const
{
	int commandListSize = isList.size();
	for (int i = 0; i<commandListSize; ++i)
	{
		GTACommandBase* pCommand = isList.at(i);
		if (iIndex == 0)
		{
			oPCommand = pCommand;
			bFound = true;
			return;
		}
		else
		{
			if (NULL != pCommand)
			{
				QList <GTACommandBase*> childList = pCommand->getChildren();
				if (!childList.isEmpty())
				{
					iIndex--;
					getCommand(iIndex, oPCommand, childList, bFound);
					if (bFound)
						return;
				}
				else
					iIndex--;
			}
			else
				iIndex--;
		}
	}
}


bool GTAElement::isEmpty()
{
    bool rc = true;
    foreach(GTACommandBase* pCmd ,_lstMasterCommands)
    {
        if (pCmd!=NULL)
        {
            rc = false;
            break;
        }
    }
    return rc;
}


void GTAElement::removeCommand(int& iIndex,QList <GTACommandBase*>& isList,bool& ibRemoveStatus)
{
    int commandListSize=isList.size();
    for(int i=0;i<commandListSize;++i)
    {
        if(ibRemoveStatus==false)
        {
            GTACommandBase* pCommand =isList.at(i);
            if (iIndex==0)
            {
                if (NULL!=pCommand )
                {
                    delete pCommand;
                    pCommand=NULL;

                }
                isList.removeAt(i);
                ibRemoveStatus=true;
                return;
            }
            else
            {
                if (NULL!=pCommand)
                {
                    QList <GTACommandBase*>& childList = pCommand->getChildren();
                    if (!childList.isEmpty())
                    {	iIndex--;
                        removeCommand(iIndex,childList,ibRemoveStatus);
                    }
                    else
                        iIndex--;
                }
                else
                    iIndex--;
            }
        }
    }
}


bool GTAElement::insertCommand(GTACommandBase * pCmd , int  iGlobalPosition , bool)
{
    bool rc = false;

    /*    if (iGlobalPosition == -1 && pCmd == NULL)
    {
        _lstMasterCommands.prepend(NULL);
    }
    else*/ 
    if(iGlobalPosition == 0)
    {
        if(_lstMasterCommands.isEmpty())
            _lstMasterCommands.push_back(pCmd);
        _lstMasterCommands[0]=pCmd;
        rc = true;
    }
    else
    {
        GTACommandBase * pParent = NULL;
        rc = insertCommand(pCmd,iGlobalPosition,_lstMasterCommands,pParent);
    }


    if(rc) setSaveSatus(GTAElement::Modified);
    return rc;
}

bool GTAElement::insertCommand(GTACommandBase * pCmd , int  & iGlobalPosition, QList<GTACommandBase* > & iListToUpdate, GTACommandBase *& ipParent )
{
    bool rc = false;

    int commandListSize = iListToUpdate.size();
    for(int i=0 ; i<commandListSize ; ++i)
    {
        iGlobalPosition--;
        GTACommandBase* pCommand = iListToUpdate.at(i);

        if (iGlobalPosition == 0)
        {
            GTAActionElse* pElse = dynamic_cast<GTAActionElse*> (pCommand);
            GTAActionElse* pCurrentElse = dynamic_cast<GTAActionElse*> (pCmd);

            if(pCommand!=NULL && pCommand->canHaveChildren()==true && (pCommand->isTitle()==false || pCommand->hasReference()==false))
            {
                if(pCommand != NULL)
                    rc = pCommand->insertChildren(pCmd,0);
                // IF ELSE is being inserted then all the command under if or while will have current command as parent except for end statement.
                if (pCurrentElse!=NULL && pCommand!=NULL)
                    reparentForElse(pCurrentElse,pCommand->getChildren(),1);//from 2nd position onwards of the children
            }
            else if(pCommand!=NULL && pCommand->IsEndCommand()==true )
            {
                rc = false;
                break;
            }
            
            else
            {
                if(ipParent != NULL)
                {
                    ipParent->insertChildren(pCmd,i+1);
                }
                else
                    iListToUpdate.insert(i+1,pCmd);

                /* current list starting from next position where else was inserted till penultimate
                 item of  will be parented to else */
                if(pCurrentElse != NULL)
                    reparentForElse(pCurrentElse,iListToUpdate,i+2); 

                rc = true;
            }

            if (pElse)
            {
                reparentForElse(pElse,iListToUpdate,i+1);

            }

            break;
        }
        else if(iGlobalPosition < 0)
            break;


        if(pCommand != NULL)
        {
            QList <GTACommandBase*>& childList = pCommand->getChildren();
            if(pCommand->canHaveChildren()==true &&( pCommand->isTitle()==false || pCommand->hasReference()==false))
            {
                if(iGlobalPosition == 0)
                {
                    if(ipParent != NULL)
                        ipParent->insertChildren(pCmd,i+1);
                    else
                        iListToUpdate.insert(0,pCmd);
                    rc = true;
                }
                else
                {
                    rc = insertCommand(pCmd,iGlobalPosition,childList,pCommand);
                    if(iGlobalPosition == 0 && rc == false)
                    {
                        if(ipParent != NULL)
                        {
                            ipParent->insertChildren(pCmd,i+1);
                        }
                        else
                            iListToUpdate.insert(i+1,pCmd);
                        rc = true;
                    }
                }
            }
        }

    }

    return rc;
}

void GTAElement::appendCommand(GTACommandBase * pCmd)
{
	_lstMasterCommands.append(pCmd);
}

void GTAElement::reparentForElse(GTAActionElse *& ipElseCmd, QList<GTACommandBase*> & ipList, int iPosition)
{
	if (ipElseCmd == NULL)
		return;


	int takePos = iPosition;
	int itemCount = ipList.count();
	for (int j = takePos; j<itemCount - 1; j++)
	{
		GTACommandBase* pCmdToReparent = ipList.takeAt(takePos);
		if (NULL != pCmdToReparent)
		{
			pCmdToReparent->setParent(ipElseCmd);
			ipElseCmd->getChildren().push_back(pCmdToReparent);
		}
		else
			ipElseCmd->getChildren().push_back(NULL);

	}
}

QList<TagVariablesDesc> GTAElement::getTagList()const
{
	QStringList oTagList;
	QStringList oIgnoredTagList;
	int childCount = this->getAllChildrenCount();
	for (int i = 0; i < childCount; i++)
	{
		GTACommandBase* pCmd = NULL;
		bool rc = getCommand(i, pCmd);
		if (pCmd != NULL)
		{
			bool isCommandIgnored = pCmd->isIgnoredInSCXML();
			if (rc && pCmd != NULL)
			{
				QStringList varList = pCmd->getVariableList();
				foreach(QString varName, varList)
				{
					QStringList validTagNameVarList;
					if (varName.contains(TAG_IDENTIFIER))
					{
						collectVariableNames(varName, validTagNameVarList);
						if (!validTagNameVarList.isEmpty())
						{
							if (isCommandIgnored)
								oIgnoredTagList.append(validTagNameVarList);
							else
								oTagList.append(validTagNameVarList);
						}
					}
				}
			}
		}
	}

	//variable names in name.
	/*QStringList tagsInName;
	collectVariableNames(_ElementName,tagsInName);
	if(!tagsInName.isEmpty())
	oTagList.append(tagsInName);*/

	oTagList.removeDuplicates();
	oIgnoredTagList.removeDuplicates();
	foreach(QString tag, oIgnoredTagList)
	{
		//Use Case: when the same tag variable in one command is ignored and is not ignored in other command
		if (oTagList.contains(tag))
			oIgnoredTagList.removeOne(tag);

	}
	
	if (_tagVarList.isEmpty())
	{
		//for the first time when the _tagVarList is empty.
		foreach(QString tagVar, oTagList)
		{
			TagVariablesDesc tagVarStruct;
			tagVarStruct.name = tagVar;
			_tagVarList.append(tagVarStruct);
		}
		foreach(QString tagVar, oIgnoredTagList)
		{
			TagVariablesDesc tagVarStruct;
			tagVarStruct.name = tagVar;
			_tagVarList.append(tagVarStruct);
		}
	}
	else
	{
		QList<TagVariablesDesc> tagVarStructlist = _tagVarList;
		_tagVarList.clear();
		foreach(QString tagVar, oTagList)
		{
			for (int count = 0; count<tagVarStructlist.count(); count++)
			{
				TagVariablesDesc tagStruct = tagVarStructlist.at(count);

				//if _tagVarList contains tag,then just update.
				if (tagStruct.name.compare(tagVar) == 0)
				{
					_tagVarList.append(tagStruct);
					break;
				}
				//condition placed to check if tag is not present in _tagVarList, adding an empty tag(with only name).
				//this condition is only true if the tag wasn't found in the entire list
				if (count == (tagVarStructlist.count() - 1))
				{
					TagVariablesDesc tagVarStruct;
					tagVarStruct.name = tagVar;
					_tagVarList.append(tagVarStruct);
				}
			}

		}
		foreach(QString tagVar, oIgnoredTagList)
		{
			for (int count = 0; count<tagVarStructlist.count(); count++)
			{
				TagVariablesDesc tagStruct = tagVarStructlist.at(count);
				//if _tagVarList contains tag,then just update.
				if (tagStruct.name.compare(tagVar) == 0)
				{
					_tagVarList.append(tagStruct);
					break;
				}
				//condition placed to check if tag is not present in _tagVarList, adding an empty tag(with only name).
				//this condition is only true if the tag wasn't found in the entire list
				if (count == (tagVarStructlist.count() - 1))
				{
					TagVariablesDesc tagVarStruct;
					tagVarStruct.name = tagVar;
					_tagVarList.append(tagVarStruct);
				}

			}
		}
	}
	return _tagVarList;
}


bool  GTAElement::collectVariableNames(const QString& isVar, QStringList& iLstVars)const
{

	// QString isVar="xyz@var1@abc@var2@ijk@var3@@";
	int count = isVar.count(TAG_IDENTIFIER);
	if (count & 0x1)
	{
		return false;
	}
	else
	{
		bool startAppending = false;
		QString variable;
		for (int i = 0; i<isVar.size(); i++)
		{
			QChar c = isVar.at(i);
			if (c == QChar(TAG_IDENTIFIER) && startAppending == false)
				startAppending = true;

			else if (c == QChar(TAG_IDENTIFIER) && startAppending == true)
			{
				startAppending = false;
				iLstVars.append(variable);
				variable.clear();
			}

			if (startAppending)
			{
				if (c != QChar(TAG_IDENTIFIER))
					variable.append(c);
			}
		}
		iLstVars;
	}
	return true;
}
void GTAElement::replaceTag(const QMap <QString, QString>& iTagValMap)
{
	int childCount = getAllChildrenCount();
	for (int i = 0; i<childCount; i++)
	{
		GTACommandBase* pCmd = NULL;
		bool rc = getCommand(i, pCmd);
		if (rc && pCmd != NULL)
		{
			pCmd->ReplaceTag(iTagValMap);
		}
	}
}

void GTAElement::replaceUntagged()
{
	int childCount = _lstMasterCommands.count();
	for (int i = childCount - 1; i >= 0; i--)
	{
		GTACommandBase* pCmd = _lstMasterCommands.at(i);
		bool rc = getCommand(i, pCmd);
		if (rc && pCmd != NULL)
		{
			//if (pCmd->canHaveChildren())
			//{
			pCmd->replaceUntagged();
			//}

			QStringList varList = pCmd->getVariableList();
			bool contains = false;
			foreach(QString varName, varList)
			{
				if (varName.contains(TAG_IDENTIFIER))
				{
					contains = true;
				}
			}
			if (contains)
			{
				delete pCmd;
				_lstMasterCommands.removeAt(i);
			}
		}
	}
	/* for(int i=0;i<cmdsTobeDeleted.count();i++)
	{
	GTACommandBase * pCmd = cmdsTobeDeleted.at(i);
	if(pCmd != NULL)
	{
	GTACommandBase * pParent = pCmd->getParent();
	if(pParent != NULL)
	{
	QList<GTACommandBase *> &cmdList = pParent->getChildrens();
	int idx = cmdList.indexOf(pCmd);
	cmdList.removeAt(idx);
	}
	else
	{
	int idx =  _lstMasterCommands.indexOf(pCmd);
	_lstMasterCommands.removeAt(idx);
	}
	delete pCmd;
	pCmd = NULL;
	}
	}
	cmdsTobeDeleted.clear();*/
}
void GTAElement::getVariableList(QStringList& oParamList, bool isSubscribeParam)const
{
	if (_lstMasterCommands.isEmpty())
		return;

	for (int i = 0; i<_lstMasterCommands.size(); i++)
	{
		GTACommandBase* pCmd = _lstMasterCommands.at(i);
		if (pCmd)
		{

			if (isSubscribeParam)
			{
				GTAActionBase *pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
				if ((pActCmd != NULL) && (pActCmd->getAction() == ACT_SUBSCRIBE))
					getVariableList(oParamList, pCmd);
			}
			else
			{
				getVariableList(oParamList, pCmd);
			}

		}
	}
}
void GTAElement::getVariableList(QStringList& oParamList, GTACommandBase* pCmd)const
{
	if (NULL != pCmd)
	{
		oParamList.append(pCmd->getVariableList());
		if (pCmd->hasChildren())
		{
			QList<GTACommandBase*> chldList = pCmd->getChildren();
			for (auto pChildCmd : chldList)
			{
				if (pChildCmd)
				{
					getVariableList(oParamList, pChildCmd);

				}
			}
		}
	}

}
//QStringList GTAElement::getVariableList()const
//{
//    QStringList lstParam;
//    int directChildrenCnd = getDirectChildrenCount();
//    for (int i=0;i<directChildrenCnd;i++)
//    {
//        GTACommandBase* pCmd = NULL;
//        bool rc = getDirectChildren(i,pCmd);
//        if (rc && pCmd != NULL)
//        {
//            QStringList varList = pCmd->getVariableList();
//            if (!varList.isEmpty())
//                lstParam.append(varList);
//
//        }
//    }
//    lstParam.removeDuplicates();
//    return lstParam;
//
//}
/**
* This function navigate the all the commends in this element and assign an instance name
* instance name is the element id/position of command in list
* iRootInstance: The instance of root command , root command is always a call command that refer another document
* if element belongs to call command , then iRootIntance is the instace name of call command, otherwise QString()
*/
void GTAElement::updateCommandInstanceName(const QString & iRootInstance)
{
	int childCount = getAllChildrenCount();
	int counter = 0;
	for (int i = 0; i<childCount; i++)
	{

		GTACommandBase* pCmd = NULL;
		bool rc = getCommand(i, pCmd);
		if ((rc && pCmd != NULL) && (pCmd->canHaveInstanceName()))
		{
			//removal of newline, carriage return, tab are added so that External tool instance name doesn't return ECMA error
			QString instanceName = QString("%1[%2]#%3").arg(getName(), QString::number(counter++), pCmd->getStatement().replace("\\n", " ").replace("\\r", "", Qt::CaseInsensitive).replace("\\t", "  ").replace("\\v", "  ", Qt::CaseInsensitive));
			if (!iRootInstance.isEmpty())
				instanceName = QString("%1/%2").arg(iRootInstance, instanceName);

			pCmd->setInstanceName(instanceName);
		}
	}
}
void GTAElement::groupCommandByTitleLTRA(QHash <QString, QList<GTACommandBase*> >& ioHshOfCmd)
{
    int childCount = getDirectChildrenCount();
    int itemCount=0;
    for (int i=0;i<childCount;i++)
    {
        GTACommandBase* pCmd = _lstMasterCommands.at(i);
        //        bool rc = getCommand(i,pCmd);
        //        if (rc && pCmd != NULL)
        if (pCmd != NULL)
        {
            if (ioHshOfCmd.isEmpty())
            {
                
                QList<GTACommandBase*> lstOfCmds;
                lstOfCmds.append(pCmd);
                ioHshOfCmd.insert(QString::number(itemCount),lstOfCmds);

            }
            else
            {
                if (pCmd->isTitle() && pCmd->getParent()==NULL) //only title at main level is counted
                {
                    itemCount++;
                }

                QList<GTACommandBase*> lstOfCmds = ioHshOfCmd.value(QString::number(itemCount));
                lstOfCmds.append(pCmd);
                ioHshOfCmd[QString::number(itemCount)]=lstOfCmds;
            }
            
        }
    }
}
//This function will group the items under action title, note that if command is of type one click it will 
//be ignored. One click commands will be under one click title.
void GTAElement::createActionElementGroupedByTitle(GTAElement*& pElement, bool groupComplete)
{
	pElement = new GTAElement(GTAElement::CUSTOM, 0);
	pElement->setName(getName());
	GTAActionParentTitle* pTitle = NULL;
	int insertCntr = 0;
	QList   <GTACommandBase*> cmdList = _lstMasterCommands;
	for (int i = 0; i<cmdList.size(); i++)
	{
		GTACommandBase* pCurCmd = cmdList.at(i);
		if (pCurCmd != NULL)
		{
			GTACommandBase* pCmd = pCurCmd->getClone();
			pCmd->setObjId(pCurCmd->getObjId(), false);
			//GTAActionBase * pActCmd = dynamic_cast<GTAActionBase * >(pCurCmd);

			if (pCmd != NULL&& pCmd->isOneClick() == false && pCmd->isOneClickTitle() == false)
			{

				if (pCmd->isTitle())
				{

					pTitle = new GTAActionParentTitle(dynamic_cast<GTAActionTitle*>(pCmd), "");
					pTitle->insertChildren(new GTAActionParentTitleEnd, 0);

					if (i)
						insertCntr = pElement->getAllChildrenCount();

					pElement->insertCommand(pTitle, insertCntr, false);

					continue;

				}
				if (pTitle && pCmd->canBeCopied())
				{
					bool rc = false;
					rc = pTitle->insertChildren(pCmd, pTitle->getChildren().size() - 1);
					if (pCmd->canHaveChildren())
					{
						createCommandGroupedByTitle(pCmd);

					}
				}
				else
				{
					//fordbg
					//int noOfChildrens = pElement->getAllChildrenCount();
					if (i)
						insertCntr = pElement->getAllChildrenCount();
					pElement->insertCommand(pCmd, insertCntr, false);


				}

			}
			if (groupComplete && (pCmd->hasReference()))
			{
				//if(pCmd->getAllChildrenCount() <=0)


				cmdList.append(pCmd->getChildren());

			}
		}
	}

}


void GTAElement::createCommandGroupedByTitle(GTACommandBase*& pCmd)
{
	QList<GTACommandBase*>& lstOfchildrens = pCmd->getChildren();
	GTAActionParentTitle* pTitle = NULL;
	GTAActionOneClickTitle* pOneClickTitle = NULL;
	for (int i = 0; i<lstOfchildrens.size(); i++)
	{
		GTACommandBase* pCurCmd = lstOfchildrens.at(i);
		if (pCurCmd != NULL)
		{
			if (pCurCmd->isTitle() && pCurCmd->isOneClick() == false && pCurCmd->isOneClickTitle() == false)
			{
				pOneClickTitle = dynamic_cast<GTAActionOneClickTitle*>(pCurCmd);
				if (pOneClickTitle == NULL)
				{
					pTitle = new GTAActionParentTitle(dynamic_cast<GTAActionTitle*>(pCurCmd), "");
					pTitle->insertChildren(new GTAActionParentTitleEnd, 0);
					delete lstOfchildrens.takeAt(i);
					lstOfchildrens.insert(i, pTitle);
					continue;
				}
			}

			if (pCurCmd->canHaveChildren())
			{
				createCommandGroupedByTitle(pCurCmd);
			}

			if (pTitle && pCurCmd->canBeCopied())
			{
				pTitle->insertChildren(pCurCmd, pTitle->getChildren().size() - 1);

				lstOfchildrens.takeAt(i);
				i--;
			}

		}
	}
}
//This function will group the items under one click title, note that if command is of type not one click it will 
//be ignored. other commands commands should be under action title.
void GTAElement::createOneClickElementGroupedByTitle(GTAElement*& pElement)
{
	pElement = new GTAElement(GTAElement::CUSTOM);
	GTAActionOneClickParentTitle* pTitle = NULL;
	int insertCntr = 0;
	QList   <GTACommandBase*> cmdList = _lstMasterCommands;
	for (int i = 0; i<cmdList.size(); i++)
	{
		GTACommandBase* pCurCmd = cmdList.at(i);
		if (pCurCmd != NULL)
		{
			GTACommandBase* pCmd = pCurCmd->getClone();
			if (pCmd != NULL)
			{
				if (pCmd->isOneClickTitle())
				{

					pTitle = new GTAActionOneClickParentTitle(dynamic_cast<GTAActionTitle*>(pCmd), ACT_ONECLICK, COM_ONECLICK_TITLE);
					pElement->insertCommand(pTitle, insertCntr++, false);
					continue;

				}
				if (pCmd->isOneClick())
				{
					if (pTitle)
						pTitle->insertChildren(pCmd, pTitle->getChildren().size());

					else
					{
						pElement->insertCommand(pCmd, insertCntr++, false);
						insertCntr += pCmd->getAllChildrenCount();

					}
				}
			}
		}
	}

}

bool GTAElement::getDirectChildrenIndex(GTACommandBase* ipCmd, int& oIndex)const
{
	bool rc = false;

	oIndex = _lstMasterCommands.indexOf(ipCmd);

	if (oIndex>-1)
	{
		rc = true;
	}

	return rc;
}

QString GTAElement::getExecutionduration()
{
	QString startTime = _ExecStart;
	QString endTime = _ExecEnd;

	startTime.replace("[", "");
	startTime.replace("]", "");
	endTime.replace("[", "");
	endTime.replace("]", "");
	bool ok;
	double startDuration = static_cast<double>(startTime.toLongLong(&ok));
	bool ok1;
	double endDuration = static_cast<double>(endTime.toLongLong(&ok1));

	unsigned int totalDurationInMs = static_cast<unsigned int>(endDuration - startDuration);
	int temp = static_cast<int>(totalDurationInMs / 1000);
	QString days = QString::number(static_cast<int>(temp / (24 * 3600)));
	temp = temp % (24 * 3600);
	QString hrs = QString::number(static_cast<int>(temp / 3600));
	temp = temp % 3600;
	QString min = QString::number(static_cast<int>(temp / 60));
	temp = (temp % 60);
	QString sec = QString::number(temp);

	QString totalDuration = "";
	if (!days.isEmpty() && (days != "0"))
	{
		totalDuration.append(QString("%1 Days ").arg(days));
	}
	if (!hrs.isEmpty() && (hrs != "0"))
	{
		totalDuration.append(QString("%1 Hours ").arg(hrs));
	}
	if (!min.isEmpty() && (min != "0"))
	{
		totalDuration.append(QString("%1 Minutes ").arg(min));
	}
	if (!sec.isEmpty() && (sec != "0"))
	{
		totalDuration.append(QString("%1 Seconds ").arg(sec));
	}
	//    if(!msVal.isEmpty() && (msVal !="0"))
	//    {
	//        totalDuration.append(QString("%1 MilliSeconds").arg(msVal));
	//    }
	return totalDuration.trimmed();
}

QString GTAElement::getGlobalResultStatus()const
{
    int childCount = getDirectChildrenCount();
    QString result= "NA";
    
	for (int i=0;i<childCount;i++)
    {

        GTACommandBase* pCmd = NULL;
        bool rc = getDirectChildren(i,pCmd);
        if (rc && pCmd != NULL)
        {
            if (pCmd->hasReference())
            {
                GTAActionExpandedCall *pCall = dynamic_cast<GTAActionExpandedCall *>(pCmd);
                if(pCall != NULL)
                {
                    result = pCall->getGlobalResultStatus();
                    if (result=="KO")
                        return result;
                }
            }
            if (pCmd->isTitle())
            {
                GTAActionParentTitle *pParentTitle = dynamic_cast<GTAActionParentTitle *>(pCmd);
                if(pParentTitle != NULL)
                {
                    result = pParentTitle->getGlobalResultStatus();
                    if (result=="KO")
                        return result;
                }
            }
            else if (pCmd->createsCommadsFromTemplate())
            {
                GTAActionFTAKinematicMultiOutput *pKinematicCmd = dynamic_cast<GTAActionFTAKinematicMultiOutput*>(pCmd);
                if(pKinematicCmd != NULL)
                {
                    result = pKinematicCmd->getGlobalResultStatus();
                    if (result=="KO")
                        return result;
                }
            }
            else 
            {
                result = pCmd->getExecutionResult();
                if (result=="KO")
                    return result;
            }

        }
    }
    return result;
}

/*
This function returns the Total Number of OK tests in all items
*/
int GTAElement::getNumberOfOK()
{
	int directChildrenCount = getDirectChildrenCount();
	int numberOfOK = 0;

	for (int i = 0; i < directChildrenCount; i++)
	{
		GTACommandBase* pCmd = NULL;
		bool rc = getDirectChildren(i, pCmd);

		if(rc && pCmd != NULL)
		{
			pCmd->getNumberOfOK(numberOfOK, pCmd);
		}
	}
	return numberOfOK;
}

/*
This function returns the Total Number of KO tests in all items
*/
int GTAElement::getNumberOfKO()
{
	int directChildrenCount = getDirectChildrenCount();
	int numberOfKO = 0;

	for (int i = 0; i < directChildrenCount; i++)
	{
		GTACommandBase* pCmd = NULL;
		bool rc = getDirectChildren(i, pCmd);
		if (rc && pCmd != NULL) 
		{
			pCmd->getNumberOfKO(numberOfKO, pCmd);
		}
	}
	return numberOfKO;
}

/*
This function returns the Total Number of NA tests in all items
*/
int GTAElement::getNumberOfNA()
{
	int directChildrenCount = getDirectChildrenCount();
	int numberOfNA = 0;

	for (int i = 0; i < directChildrenCount; i++)
	{
		GTACommandBase* pCmd = NULL;
		bool rc = getDirectChildren(i, pCmd);
		
		if(rc && pCmd != NULL)
		{ 
			pCmd->getNumberOfNA(numberOfNA, pCmd);
		}
	}
	return numberOfNA;
}

/*
This function returns the Total Number of KO with a non-empty defect field
*/
int GTAElement::getNumberOfKOWithDefect()
{
	int directChildrenCount = getDirectChildrenCount();
	int numberOfKOWithDefect = 0;

	for (int i = 0; i < directChildrenCount; i++)
	{
		GTACommandBase* pCmd = NULL;
		bool rc = getDirectChildren(i, pCmd);

		if (rc && pCmd != NULL) {
			pCmd->getNumberOfKOWithDefect(numberOfKOWithDefect, pCmd);
		}
	}
	return numberOfKOWithDefect;
}

/*
This function returns the QString List of defects
*/
QString GTAElement::getListOfDefects()
{
	int directChildrenCount = getDirectChildrenCount();
	QString listOfDefects = "";

	for (int i = 0; i < directChildrenCount; i++)
	{
		GTACommandBase* pCmd = NULL;
		bool rc = getDirectChildren(i, pCmd);

		if (rc && pCmd != NULL) {
			pCmd->getListOfDefects(listOfDefects, pCmd);
		}
	}
	return listOfDefects;
}

bool GTAElement::prependRow()
{
	GTACommandBase* pCmd = NULL;
	if (_lstMasterCommands.isEmpty())
		return false;
	else
		_lstMasterCommands.prepend(pCmd);
	return true;
}
//bool GTAElement::findStrForwardSetHighlight()
//{
//
//}
bool GTAElement::findStringAndSetHighlight(const QRegExp& searchExp, QList<int>& iSelectedRows, QHash<int, QString>& oFoundRows,
	bool bSearchReverse, bool ibTextSearch, bool isSingleSearch, bool isHighlight)
{

	bool rc = false;

	const bool bSingleSearch = isSingleSearch;


	//this is iSelectedRows empty condition first condition

	int searchStartPos = 0;
	int searchEndPos = getAllChildrenCount() - 1;

	// this is when a certain row is selected or multiple rows are slected the always the first row from
	//that selection is chosen, from this row onwards the search nect will highlight
	if (!iSelectedRows.isEmpty())
	{
		searchStartPos = iSelectedRows.first();
	}

	// this is when a search up(reverse search) is selected
	if (bSearchReverse)
	{
		// following is the initial condition for searching in reverse order
		searchStartPos = getAllChildrenCount() - 1;
		searchEndPos = 0;

		//if a single row is selected then that row is selected else if multiple rows are selected
		if (!iSelectedRows.isEmpty())
		{
			if (iSelectedRows.size() == 1)
			{
				searchStartPos = (iSelectedRows.first() == 0) ? 0 : (iSelectedRows.first() - 1);
			}
			else
				searchStartPos = iSelectedRows.last();    //(iSelectedRows.at(0) == 0) ? 0 : iSelectedRows.at(0);
		}
	}

	// following is working. think and change
	QString sCmdStatement;
	if (bSearchReverse)
	{
		for (int i = searchStartPos; i >= searchEndPos; i--)
		{


			rc = searchSetHighlightOnCommand(searchExp, i, ibTextSearch, sCmdStatement, isHighlight);
			if (rc)
			{
				oFoundRows.insert(i, sCmdStatement);
				if (bSingleSearch)
					return rc;
			}
		}
	}
	else
		for (int i = searchStartPos; i <= searchEndPos; i++)
		{

			rc = searchSetHighlightOnCommand(searchExp, i, ibTextSearch, sCmdStatement, isHighlight);
			if (rc)
			{
				oFoundRows.insert(i, sCmdStatement);
				if (bSingleSearch)
					return rc;
			}
		}



	if (bSearchReverse)
	{
		for (int i = getAllChildrenCount() - 1; i >= searchEndPos; i--)
		{
			rc = searchSetHighlightOnCommand(searchExp, i, ibTextSearch, sCmdStatement, isHighlight);
			if (rc)
			{
				oFoundRows.insert(i, sCmdStatement);
				if (bSingleSearch)
					return rc;
			}
		}
	}
	else
		for (int i = 0; i <= getAllChildrenCount() - 1; i++)
		{
			rc = searchSetHighlightOnCommand(searchExp, i, ibTextSearch, sCmdStatement, isHighlight);
			if (rc)
			{
				oFoundRows.insert(i, sCmdStatement);
				if (bSingleSearch)
					return rc;
			}
		}

	return rc;
}

bool GTAElement::searchSetHighlightOnCommand(const QRegExp& searchExp, const int& ipRow, bool ibTextSearch, QString & ofoundCmdStatement, bool isHighlight)
{

	bool rc = false;
	GTACommandBase* pCmd = NULL;
	getCommand(ipRow, pCmd);
	if (ibTextSearch && pCmd != NULL && pCmd->getCommandType() == GTACommandBase::ACTION)
	{
		GTAActionBase* pAction = dynamic_cast<GTAActionBase*> (pCmd);
		if (pAction != NULL)
		{
			if (pAction->searchText(searchExp))
			{
				rc = true;
				if (isHighlight)
					pCmd->setSearchHighlight(true);
				ofoundCmdStatement = pCmd->getStatement();
			}
		}
	}

	if (ibTextSearch && pCmd != NULL && rc == false && pCmd->getCommandType() == GTACommandBase::CHECK)
	{
		GTACheckBase* pChk = dynamic_cast<GTACheckBase*> (pCmd);
		if (pChk != NULL)
		{
			if (pChk->searchText(searchExp))
			{
				rc = true;
				if (isHighlight)
					pCmd->setSearchHighlight(true);
				ofoundCmdStatement = pCmd->getStatement();

			}
		}
	}

	if (ibTextSearch && pCmd != NULL && rc == false) {
		if (pCmd->searchCommentAndRequirements(searchExp)) {
			rc = true;
			if (isHighlight) {
				pCmd->setSearchHighlight(true);
			}
				
			ofoundCmdStatement = pCmd->getStatement();
		}
	}

	if (NULL != pCmd && rc == false && pCmd->searchString(searchExp, ibTextSearch))
	{
		rc = true;
		if (isHighlight)
			pCmd->setSearchHighlight(true);
		ofoundCmdStatement = pCmd->getStatement();

	}
	return rc;
}
/*TODO: this has complexity of n^2 [exactly n(n-1)/2 -1] will need imporvement */
void GTAElement::replaceString(const QRegExp& searchString, const QString& sReplaceWith, QList<int>& iSelectedRows, QList<int>& oFoundRows,
	bool bSearchReverse)
{

	int searchStartPos = 0;
	int searchEndPos = getAllChildrenCount() - 1;

	if (bSearchReverse)
	{
		searchEndPos = 0;
		if (iSelectedRows.isEmpty())
			searchStartPos = getAllChildrenCount() - 1;
		else
			searchStartPos = iSelectedRows.at(0) == 0 ? 0 : iSelectedRows.at(0) - 1;
	}
	else if (iSelectedRows.size() == 1)
		searchStartPos = iSelectedRows.at(0);
	else if (iSelectedRows.size()>1)
	{
		searchStartPos = iSelectedRows.at(0);
		searchEndPos = iSelectedRows.at(iSelectedRows.size() - 1);
		bSearchReverse = false;

	}




	if (bSearchReverse)
	{
		for (int i = searchStartPos; i >= searchEndPos; i--)
		{
			GTACommandBase* pCmd = NULL;
			getCommand(i, pCmd);
			if (NULL != pCmd && pCmd->replacableStrFound(searchString))
			{
				pCmd->setSearchHighlight(true);
				pCmd->stringReplace(searchString, sReplaceWith);
				oFoundRows.append(i);
				if (iSelectedRows.size() == 1)
				{
					return;
				}
			}
		}
	}
	else
	{
		for (int i = searchStartPos; i <= searchEndPos; i++)
		{
			GTACommandBase* pCmd = NULL;
			getCommand(i, pCmd);
			if (NULL != pCmd && pCmd->replacableStrFound(searchString))
			{
				pCmd->setSearchHighlight(true);
				pCmd->stringReplace(searchString, sReplaceWith);
				oFoundRows.append(i);
				if (iSelectedRows.size() == 1)
				{
					return;
				}
			}
		}
	}
	/* if (selectedRow!=-1)
	{
	int sizeOfElement = getAllChildrenCount();

	for (int i = selectedRow;i<getAllChildrenCount();i++)
	{
	GTACommandBase* pCmd = NULL;
	getCommand(i,pCmd);
	if (NULL!=pCmd && pCmd->replacableStrFound(searchString))
	{
	pCmd->setSearchHighlight(true);
	if (iSelectedRows.size()==1)
	return;
	else  (i==iSelectedRows.at(iSelectedRows.size()-1))

	}
	}*/


	// }

}
/*TODO: this has complexity of n^2 [exactly n(n-1)/2 -1] will need imporvement */
bool GTAElement::replaceString(const int& row, const QRegExp& strToReplace, const QString& sReplaceWith)
{
	bool rc = false;
	if (row >= 0 || row<getAllChildrenCount())
	{
		GTACommandBase* pCmd = NULL;
		getCommand(row, pCmd);
		if (pCmd)
		{
			rc = pCmd->replacableStrFound(strToReplace);
			pCmd->stringReplace(strToReplace, sReplaceWith);
		}

	}
	return rc;
}
void GTAElement::resetStringSearch()
{
	/* for (int i=0;i<=getAllChildrenCount();i++)
	{
	GTACommandBase* pCmd = NULL;
	getCommand(i,pCmd);
	if (NULL!=pCmd )
	{
	pCmd->setSearchHighlight(false);

	}
	}*/
	resetStringSearch(_lstMasterCommands);
}
void GTAElement::resetStringSearch(QList<GTACommandBase*>& iPlstCmds)
{
	int size = iPlstCmds.size();
	if (size)
	{
		for (int i = 0; i<size; i++)
		{
			GTACommandBase* curCmd = iPlstCmds.at(i);
			if (curCmd != NULL)
			{
				curCmd->setSearchHighlight(false);
				resetStringSearch(curCmd->getChildren());
			}
		}
	}
}
/*TODO: this has complexity of n^2 [exactly n(n-1)/2 -1] will need imporvement */
bool GTAElement::replaceAllString(const QRegExp& strToReplace, const QString& sReplaceWith)
{

	bool rc = false;
	for (int i = 0; i <= getAllChildrenCount(); i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		if (NULL != pCmd)
		{

			if (pCmd->replacableStrFound(strToReplace))
			{
				rc = true;
				pCmd->setSearchHighlight(true);
				pCmd->stringReplace(strToReplace, sReplaceWith);
			}

		}
	}

	return rc;
}

bool GTAElement::replaceAllEquipment(const QRegExp& strToReplace, const QString& sReplaceWith)
{
	bool rc = false;
	for (int i = 0; i <= getAllChildrenCount(); i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		if (NULL != pCmd)
		{
			if (pCmd->equipmentReplace(strToReplace, sReplaceWith))
			{
				rc = true;
				pCmd->setSearchHighlight(rc);
				setSaveSatus(GTAElement::Modified);
			}
		}
	}
	return rc;
}




void GTAElement::flattenCallCommands()
{

	int currChildrenSize = 0;
	currChildrenSize = _lstMasterCommands.size();
	for (int i = currChildrenSize - 1; i >= 0; i--)
	{
		GTACommandBase* pCurrentChild = _lstMasterCommands.at(i);
		if (pCurrentChild != NULL && pCurrentChild->hasReference())
		{
			_lstMasterCommands.removeAt(i);
			QList<GTACommandBase*>& referenceChildrens = pCurrentChild->getChildren();
			for (int j = 0; j<referenceChildrens.size(); j++)
			{
				GTACommandBase* currentCommand = referenceChildrens.at(j);
				//currentCommand->expandReferences();
				_lstMasterCommands.insert(i + j, currentCommand);
			}

		}
	}
	currChildrenSize = _lstMasterCommands.size();
	for (int i = currChildrenSize - 1; i >= 0; i--)
	{
		GTACommandBase* pCurrentChild = _lstMasterCommands.at(i);
		if (pCurrentChild != NULL)
		{
			pCurrentChild->expandReferences();
		}
	}

}
QString GTAElement::getDocumentName()
{
	QString docName = "";
	QString elemName = getName();
	if (!elemName.isEmpty())
	{
		QString fileExtension = "";
		switch (_ElemType)
		{


		case GTAElement::OBJECT:
			fileExtension = QString(".obj");
			break;
		case GTAElement::FUNCTION:
			fileExtension = QString(".fun");
			break;
		case GTAElement::PROCEDURE:
			fileExtension = QString(".pro");
			break;
		case GTAElement::SEQUENCE:
			fileExtension = QString(".seq");
			break;
		case GTAElement::TEMPLATE:
			fileExtension = QString(".tmpl");
			break;


		}

		docName = QString("%1%2").arg(elemName, fileExtension);
	}

	return docName;

}
QStringList GTAElement::getPrintTables()
{
	int cnt = getAllChildrenCount();
	QStringList lst;
	for (int i = 0; i<cnt; i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		GTAActionPrintTable* pPrint = dynamic_cast<GTAActionPrintTable*> (pCmd);
		if (pPrint)
			lst.append(pPrint->getTableName());
	}

	return lst;
}
QStringList GTAElement::getPrintTableParams(const QString &isTableName)
{
	QStringList lst;
	int cmdCnt = getAllChildrenCount();
	for (int i = 0; i<cmdCnt; i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		GTAActionPrintTable* pPrint = dynamic_cast<GTAActionPrintTable*> (pCmd);
		if (pPrint != NULL && pPrint->getTableName() == isTableName)
			lst = pPrint->getValues();


	}
	return lst;
}
void GTAElement::editPrintTables(const QString& iName, const QStringList& iLst)
{
	int cmdCnt = getAllChildrenCount();
	for (int i = 0; i<cmdCnt; i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		GTAActionPrintTable* pPrint = dynamic_cast<GTAActionPrintTable*> (pCmd);
		if (pPrint != NULL && pPrint->getTableName() == iName)
			pPrint->setValues(iLst);
	}
}

//GTAElement* GTAElement::getClone()
//{
//    QList<GTACommandBase*> lstCloneLst;
//    
//    for (int i=0;i<_lstMasterCommands.size();i++)
//    {
//        GTACommandBase* pCmd = _lstMasterCommands.at(i);
//        if (NULL!=pCmd)
//        {
//            GTACommandBase* pCmdClone = pCmd->getClone();
//            lstCloneLst.append(pCmdClone);
//        }
//    }
//    GTAElement *pElem = new GTAElement(_ElemType,lstCloneLst.size());
//    pElem->setMasterList(lstCloneLst);
//    return pElem;
//}
//void GTAElement::setMasterList(QList   <GTACommandBase*>  iLstOfCmds)
//{
//    if (_NbOfChildren>0)
//    {
//        for (int i=0;i<_NbOfChildren-1;i++)
//        {
//            _lstMasterCommands.push_back(iLstOfCmds.at(i));
//
//        }
//    }
//
//}
void GTAElement::editTitleInprintTableForGenestaImport()
{
	int cmdCnt = getAllChildrenCount();
	GTACommandBase* pPrevCmd = NULL;
	for (int i = cmdCnt - 1; i >= 0; i--)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		GTAActionTitle* pTitle = dynamic_cast<GTAActionTitle*>(pCmd);
		GTAActionPrintTable* pPrint = dynamic_cast<GTAActionPrintTable*> (pPrevCmd);
		if (pTitle != NULL && pPrint != NULL)
		{
			pPrint->setTitleName(pTitle->getTitle());
			removeCommand(i);
			pCmd = NULL;

		}
		pPrevCmd = pCmd;


	}

}
QStringList GTAElement::getPrintTableItems(const QString& iTableName)
{
	int cnt = getAllChildrenCount();
	QStringList lst;
	for (int i = 0; i<cnt; i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		if (pCmd != NULL)
		{
			GTAActionPrintTable* pPrintTable = dynamic_cast<GTAActionPrintTable*>(pCmd);
			if (NULL != pPrintTable && iTableName == pPrintTable->getTableName())
			{
				return pPrintTable->getValues();
			}
		}

	}
	return QStringList();
}
//for import Genesta, append should happen in penultimate row.
void GTAElement::appendReleaseCommand(const QStringList& iRleaseParam, bool forImportGenesta)
{
	if (iRleaseParam.isEmpty() == false)
	{
		GTAActionRelease* pRelease = new GTAActionRelease(ACT_REL, COM_REL_PAR);
		pRelease->setParameterList(iRleaseParam);

		if (forImportGenesta)
		{
			int masterLstSize = _lstMasterCommands.size();
			GTACommandBase* pLastCmd = _lstMasterCommands.last();
			if (pLastCmd != NULL && pLastCmd->isTitle())
				_lstMasterCommands.insert(masterLstSize - 1, pRelease);
			else
				_lstMasterCommands.append(pRelease);
		}
		else
			_lstMasterCommands.append(pRelease);
	}
}
QStringList GTAElement::getFailureCommands()
{
	int cnt = getAllChildrenCount();
	QStringList lst;
	for (int i = 0; i<cnt; i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		if (pCmd != NULL)
		{
			if (pCmd->isFailureCommand())
			{
				GTAActionFailure* pFailCmd = dynamic_cast<GTAActionFailure*> (pCmd);
				if (pFailCmd)
					lst.append(pFailCmd->getFailureName());
			}
		}

	}

	lst.removeDuplicates();
	return lst;
}
GTACommandBase* GTAElement::getFailureCommand(const QString& FailureName)
{

	int cnt = getAllChildrenCount();
	QStringList lst;
	for (int i = 0; i<cnt; i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		if (pCmd != NULL)
		{
			if (pCmd->isFailureCommand())
			{
				GTAActionFailure* pFailCmd = dynamic_cast<GTAActionFailure*> (pCmd);
				if (pFailCmd)
				{
					if (FailureName == pFailCmd->getFailureName())
					{
						return pFailCmd;
					}
				}
			}
		}

	}

	return NULL;
}
bool GTAElement::hasChannelInControl()const
{
	bool hasVariableList = false;
	for (int i = 0; i<_lstMasterCommands.size(); i++)
	{
		GTACommandBase* pCmd = _lstMasterCommands.at(i);
		if (pCmd)
		{
			hasChannelInControl(hasVariableList, pCmd);
			if (hasVariableList)
				break;

		}
	}

	return hasVariableList;
}

void GTAElement::hasChannelInControl(bool& hasVariableList, GTACommandBase* pCmd)const
{

	if (NULL != pCmd)
	{
		if (pCmd->hasChannelInControl())
		{
			hasVariableList = true;
			return;
		}
		if (pCmd->hasChildren())
		{
			QList<GTACommandBase*> chldList = pCmd->getChildren();
			for (auto pChildCmd : chldList)
			{
				if (pChildCmd)
				{
					hasChannelInControl(hasVariableList, pChildCmd);

				}
			}
		}
	}

}

QList<GTACommandBase*> GTAElement::getAllFailureCommands()
{
	QList<GTACommandBase*> failureCmdList;
	for (int i = 0; i<getAllChildrenCount(); i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		if (pCmd != NULL && pCmd->isFailureCommand())
		{
			failureCmdList.append(pCmd);
		}
	}

	return failureCmdList;
}
void GTAElement::addtimeOut(const double& idTimeOut, const QString& iUnit)
{
	for (int i = 0; i<getAllChildrenCount(); i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		if (pCmd != NULL)
		{
			GTAActionBase*  pActBase = dynamic_cast<GTAActionBase*>(pCmd);
			if (pActBase)
			{
				pActBase->addTimeOut(idTimeOut, iUnit);
			}

		}
	}

}
double GTAElement::getTotalTimeOutInMs()const
{
	double dTimeOutInMs = 0.0;

	for (int i = 0; i<getAllChildrenCount(); i++)
	{
		GTACommandBase* pCmd = NULL;
		getCommand(i, pCmd);
		if (pCmd != NULL)
		{
			GTAActionBase*  pActBase = dynamic_cast<GTAActionBase*>(pCmd);
			GTACheckBase*  pChkBase = dynamic_cast<GTACheckBase*>(pCmd);

			if (pActBase)
				dTimeOutInMs = dTimeOutInMs + pActBase->getTimeOutInMs();

			if (pChkBase)
				dTimeOutInMs = dTimeOutInMs + pChkBase->getTimeOutInMs();



		}
	}

	return dTimeOutInMs;

}
void  GTAElement::setHiddenRows(const QStringList& iRows)
{
	_hiddenRows.clear();
	_hiddenRows.append(iRows);
}
QStringList GTAElement::getHiddenRows()const
{
	return _hiddenRows;
}
void GTAElement::removeEmptyItems()
{
	if (isEmpty() == false)
	{
		for (int i = _lstMasterCommands.size() - 1; i >= 0; i--)
		{
			GTACommandBase* pCurCmd = _lstMasterCommands.at(i);
			if (pCurCmd == NULL)
			{
				_lstMasterCommands.takeAt(i);
			}
			else
				pCurCmd->removeEmptyChilds();
		}
	}
}

std::optional<int> GTAElement::getIndexOfCommand(const GTACommandBase* iCmd)
{
	int currentIdx = 0;
	for (const auto* cmd : _lstMasterCommands) {
		if (cmd == NULL) {
			continue;
		}
		if (cmd == iCmd) {
			return currentIdx;
		}
		else if (cmd->hasChildren()) {
			if (searchChildren(currentIdx, cmd, iCmd)) {
				return currentIdx;
			}
		}
		currentIdx++;
	}

	return std::nullopt;
}

bool GTAElement::searchChildren(int& currentIdx, const GTACommandBase* parentCmd, const GTACommandBase* wantedCmd) {
	auto& children = parentCmd->getChildren();
	for (const auto* child : children) {
		currentIdx++;
		if (child == wantedCmd) {
			return true;
		}
		else if (child && child->hasChildren()) {
			if (searchChildren(currentIdx, child, wantedCmd)) {
				return true;
			}
		}
	}

	return false;
}

QList<GTACommandBase*> GTAElement::GetAllCommands() {
	QList<GTACommandBase*> allCmds;
	for (auto* cmd : _lstMasterCommands) {
		allCmds.append(cmd);
		if (cmd && cmd->hasChildren()) {
			appendAllChildren(cmd, allCmds);
		}
	}

	return allCmds;
}

void GTAElement::appendAllChildren(GTACommandBase* parentCmd, QList<GTACommandBase*>& outputCmds) {
	auto& children = parentCmd->getChildren();
	for (auto* child : children) {
		outputCmds.append(child);
		if (child && child->hasChildren()) {
			appendAllChildren(child, outputCmds);
		}
	}
}

void GTAElement::setAbsoluteFilePath(const QString &iAbsFilePath)
{
	_AbsoluteFilePath = iAbsFilePath;
}

QString GTAElement::getAbsoluteFilePath()const
{
	return _AbsoluteFilePath;
}

void GTAElement::setRelativeFilePath(const QString &iRelFilePath)
{
	_RelativeFilePath = iRelFilePath;
}

QString GTAElement::getRelativeFilePath()const
{
	return _RelativeFilePath;
}

void GTAElement::setFSIIRefFile(const QString &iFSIIRefFileName)
{
	_FSIIRefFileName = iFSIIRefFileName;
}

QString GTAElement::getFSIIRefFile()const
{
	return _FSIIRefFileName;
}

void GTAElement::setTagVarStructList(QList<TagVariablesDesc> iTagList)
{
	_tagVarList.clear();
	_tagVarList = iTagList;
}

/**
 * @brief Update the Call commands by storing some variable values if they are set to "InputForInnerCall"
*/
void GTAElement::updateCallCommands()
{
	QList<GTACommandBase*> allCommands = GetAllCommands();
	_lstLocVarValues.clear();
	for (int i = 0; i < allCommands.count(); i++)
	{
		auto cmd = allCommands.at(i);

		// SET command : get value of all "inputForCall" variables
		GTAActionSetList* actionSetList = dynamic_cast<GTAActionSetList*> (cmd);
		if (actionSetList != NULL)
		{
			bool isInputForInnerCall = actionSetList->getComment().contains(ACT_SET_INNERCALL);
			if (isInputForInnerCall && !actionSetList->isIgnoredInSCXML())
			{
				QList <GTAActionSet*> pActionSetList;
				actionSetList->getSetCommandlist(pActionSetList);
				foreach(GTAActionSet * actionSet, pActionSetList)
				{
					QString parameter = actionSet->getParameter();
					QString value = actionSet->getValue();
					_lstLocVarValues.insert(parameter, value);
				}
			}
		}

		// CALL command : update map of tagValues if the call argument is an "inputForCall" variable
		GTAActionCall* actionCall = dynamic_cast<GTAActionCall*> (cmd);
		if (actionCall != NULL)
		{
			QMap<QString, QString> mapOfVarTags = actionCall->getTagValue();
			QMap <QString, QString> mapOfTagValues;
			foreach(QString tag, mapOfVarTags.values())
			{
				tag.remove(TAG_IDENTIFIER);
				if (_lstLocVarValues.contains(tag))
				{					
					mapOfTagValues.insert(tag, _lstLocVarValues.value(tag));
				}
			}
			actionCall->setTagParameterValue(mapOfTagValues);			
		}
	}
}