#include "GTACheckCompatibility.h"
#include "GTADataController.h"
#include "GTAActionCall.h"
#include "GTADocumentController.h"
#include "GTAUtil.h"
#include "GTACheckBase.h"
#include "GTACheckValue.h"
#include "GTAActionBase.h"
#include "GTAActionSetList.h"
#include "GTAActionSet.h"
#include "GTAActionRelease.h"
#include "GTAActionWait.h"
#include "GTAActionIF.h"
#include "GTAActionWhile.h"
#include "GTAParamValidator.h"
#include "GTAEquationBase.h"
#include "GTAActionRoboArm.h"
#include "GTAActionIRT.h"
#include "GTAActionOneClickPPC.h"
#include "GTAOneClick.h"
#include "GTAOneClickLaunchApplication.h"
#include "GTAAppController.h"
#include "GTAActionMaths.h"
#include "GTAMathExpression.h"

#pragma warning(push, 0)
#include "qstring.h"
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#pragma warning(pop)

#pragma warning (disable : 4456)

int GTACheckCompatibility::_nbParamList = 0;

GTACheckCompatibility::GTACheckCompatibility()
{
	_ParamSubscribed.clear();
	_UUIDElementHash.clear();
	_fColumnName = QString();
	_sColumnName = QString();
	_PirsList.clear();
	_ColPirName.clear();
}

void GTACheckCompatibility::resetCheckComp()
{
	// Reset the static int variable is required
	_nbParamList = 0; 
	_nbColumn = SET_CLASSIQUE;
	_ParamSubscribed.clear();
	_UUIDElementHash.clear();
	_fColumnName = QString();
	_sColumnName = QString();
	_PirsList.clear();
	_ColPirName.clear();
	_ParamList.clear();
	_TripletsList.clear();
	_ValuesList.clear();
	_CommandUUIDs.clear();
	_VmacParamInfo.clear();
}

/**
* This function open the gta document
* iFileName : file name of the document to be opened e.g. abc.obj
* opElemenet : The document object to be retrieved
* return true if the document is open successfully otherwise false
*/
bool GTACheckCompatibility::getElementFromDocument(const QString &iDataDirectory, const QString & iFileName, GTAElement *& opElemenet, bool isUUID)
{
	bool rc = false;
	QString completeFilePath = QDir::cleanPath(QString("%1/%2").arg(iDataDirectory, iFileName));

	GTADocumentController docController;
	GTAHeader* pHdr = NULL;

	GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();

	if (isUUID)
		rc = pAppCtrl->getElementFromDocument(iFileName, opElemenet, true);
	else
		rc = docController.openDocument(completeFilePath, opElemenet, pHdr);


	if (!rc)
		_LastError = pAppCtrl->getLastError();
	return rc;
}

/* This function check if the Parameter is used as an Tag (contains @nameVariable@)
 * @input : iParam - Parameter name
 * @return : bool
*/
bool GTACheckCompatibility::isValidTagVar(const QString &iParam)
{
	if (iParam.contains("@"))
	{
		int cnt = iParam.count('@', Qt::CaseInsensitive);
		if (cnt % 2 != 0)
			return false;
		QString tempParam = iParam;
		tempParam = tempParam.remove(" ");
		if (tempParam.contains("@@"))
			return false;
		return true;
	}
	else
	{
		return false;
	}
}

/* This function checks if the Parameter is a Valid PIR signal
* @input : iParam - Parameter name
* @output : ValidParam - New Parameter name
* @return : bool
*/
bool GTACheckCompatibility::isValidPIRSignal(const QString & iParam, QString &iValidParam)
{
	if (iParam.toLower().contains(".arinc."))
	{
		iValidParam = iParam;
		iValidParam = iValidParam.replace(".arinc", ".A429", Qt::CaseInsensitive);
		return false;
	}
	return true;
}

/* This function allow to know if the Parameter is PIR or not
 * It must be in the DataBase, gets as Media or SourceType : PIR 
 * @input : iParam - Parameter name
 */
bool GTACheckCompatibility::checkIsPIR(const QString &iParam)
{
	QStringList l_paramListKeys = _ParamList.keys();
	
	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		GTAICDParameter parameter = _ParamList.value(l_paramListKeys.filter(iParam, Qt::CaseInsensitive)[0]);

		return (parameter.getMedia() == "PIR" || parameter.getSourceType() == "PIR" || parameter.getParameterType() == "FIB-IPR");
	}
	return false;
}

void GTACheckCompatibility::setParamList(const QHash<QString, GTAICDParameter> &ParamList)
{
	_ParamList = ParamList;
}

bool GTACheckCompatibility::checkIsLocal(const QString &iParam)
{
	if (iParam.contains("_uuid[{"))
		return true;

	if (iParam.contains("GEN_TOOL:"))
		return false;

	QStringList l_paramListKeys = _ParamList.keys();

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		QString paramType = QString();
		bool rc = false;
		rc = checkIsConstant(l_paramListKeys.filter(iParam, Qt::CaseInsensitive)[0], paramType);

		if (paramType != "Float")
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool GTACheckCompatibility::checkForRange(const QString &iParam, const QString& iVal, QString &oMinVal, QString &oMaxVal)
{
	QStringList l_paramListKeys = _ParamList.keys();

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		GTAICDParameter parameter = _ParamList.value(l_paramListKeys.filter(iParam, Qt::CaseInsensitive)[0]);
		bool isNum = false;
		double inputVal = iVal.toDouble(&isNum);
		double maxValue;
		double minValue;
		bool isParamBool = (parameter.getValueType().toLower() == "boolean");

		if (isNum)
		{
			if (isParamBool)
			{
				maxValue = 1;
				isNum = true;
			}
			else
				maxValue = parameter.getMaxValue().toDouble(&isNum);
			if (isNum)
			{
				if (isParamBool)
				{
					minValue = 1;
				}
				else
					minValue = parameter.getMinValue().toDouble(&isNum);

				if (parameter.getValueType().toLower() == "boolean")
				{
					if ((inputVal == 0) || (inputVal == 1))
					{
						return true;
					}
					else
					{
						oMinVal = "0";
						oMaxVal = "1";
						return false;
					}
				}
				else
				{
					if (isNum)
					{
						if ((inputVal >= minValue && inputVal <= maxValue))
							return true;
						else
						{
							oMinVal = parameter.getMinValue();
							oMaxVal = parameter.getMaxValue();
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

/* This function allows to read the Media of the Parameter in Database and returns it.
 * @input: iParam - Parameter Name with which we make a research in DB
*/
QString GTACheckCompatibility::getParamMediaType(const QString & iParam)
{
	QStringList l_paramListKeys = _ParamList.keys();

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		GTAICDParameter paramInfo = _ParamList.value(l_paramListKeys.filter(iParam, Qt::CaseInsensitive)[0]);
		return paramInfo.getMedia();
	}
	return QString();
}

/* This function gets the SourceFile of the Parameter and returns its extension (ICD, PIR, PMR)
 * @input : iParam - Parameter name with which we make a research in DB
*/
QString GTACheckCompatibility::getParamSrcType(const QString & iParam)
{
	QStringList l_paramListKeys = _ParamList.keys();

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		GTAICDParameter paramInfo = _ParamList.value(l_paramListKeys.filter(iParam, Qt::CaseInsensitive)[0]);
		return paramInfo.getSourceType();
	}
	return QString();
}

/* This function allows to check if the Parameter contains in its Name a reference based on ChannelInControl 
 * @input : iParam - Parameter name with which we make a research in DB
 * Possible values to be tested : "CIC" , "CNIC", "both"
*/
bool GTACheckCompatibility::checkIsEngineParameter(const QString &iParam)
{
	if	(iParam.contains(CHANNEL_IN_CONTROL, Qt::CaseInsensitive) 
	  || iParam.contains(CHANNEL_NOT_IN_CONTROL, Qt::CaseInsensitive)
	  || iParam.contains(BOTH_CHANNEL, Qt::CaseInsensitive) )
	{
		return true;
	}
	else
		return false;
}

bool GTACheckCompatibility::checkForChannelInControl(const QString &iParam, QStringList &oUnAvailableParams)
{
	bool rc = false;
	if (iParam.contains(CHANNEL_IN_CONTROL, Qt::CaseInsensitive)
		|| iParam.contains(CHANNEL_NOT_IN_CONTROL, Qt::CaseInsensitive)
		|| iParam.contains(BOTH_CHANNEL, Qt::CaseInsensitive))
	{
		QStringList resolvedParams;
		GTAUtil::resolveChannelParam(iParam, resolvedParams);
		foreach(QString parameter, resolvedParams)
		{
			QString isPIR;
			bool ret = checkIsInDb(parameter, ret, isPIR);
			if (!ret)
			{
				oUnAvailableParams.append(parameter);
			}
			rc = ret;
		}
	}
	return rc;
}


/* This function allows to check if the Parameter is in DB and if it comes from a PIR file
 * @input : iParam - Parameter name with which we make a research in DB
 * @output : isPIR - Return 'PIR' if the parameter is a PIR
 * @output : isInDB - Returns true if the parameter is in DB
 * @return : bool
*/
//TODO: Add overload for not taking isPIR as argument
bool GTACheckCompatibility::checkIsInDb(const QString &iParam, bool &isInDB, QString &isPIR)
{
	QStringList l_paramListKeys = _ParamList.keys();

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		GTAICDParameter paramInfo = _ParamList.value(l_paramListKeys.filter(iParam, Qt::CaseInsensitive)[0]);
		QString srcType = paramInfo.getSourceType();
		if (srcType == PARAM_TYPE_PIR)
		{
			isPIR = srcType;
		}
		isInDB = true;
		return true;
	}
	return false;
}

/* This function allows to check if the Parameter is of type VMAC
* @input : iParam - Parameter name with which we make a research in DB
*/
bool GTACheckCompatibility::checkIsVmac(const QString &iParam)
{
	QStringList l_paramListKeys = _ParamList.keys();

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		GTAICDParameter paramInfo = _ParamList.value(l_paramListKeys.filter(iParam, Qt::CaseInsensitive)[0]);
		return (paramInfo.getParameterType() == "VMAC");
		//return (paramInfo.getMedia() == "VMAC");
	}
	else
		return false;
}
bool GTACheckCompatibility::checkValidVal(const QString &iParam, const QString &iVal, const bool &isInDB)
{
	bool rc = false;
	QStringList l_paramListKeys = _ParamList.keys();

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		if (isInDB)
		{
			GTAICDParameter paramInfo = _ParamList.value(l_paramListKeys.filter(iParam, Qt::CaseInsensitive)[0]);
			QString paramValueType = paramInfo.getValueType();
			QString dummy;
			if (checkIsInDb(iVal,rc, dummy) == false)
			{
				if ((paramValueType.toLower() == "float") || (paramValueType.toLower() == "double"))
				{
					iVal.toDouble(&rc);
				}
				else if ((paramValueType.toLower() == "integer") || (paramValueType.toLower() == "int"))
				{
					iVal.toInt(&rc, 10);
				}
				else if ((paramValueType.toLower() == "boolean") || (paramValueType.toLower() == "bool"))
				{
					if ((iVal == "0") || (iVal == "1") || (iVal == "true") || (iVal == "false"))
						rc = true;
					else
						rc = false;
				}
				else if (paramValueType.toLower() == "char")
				{
					// Old format of STRING
					if (iVal.startsWith("\"") && iVal.endsWith("\""))
					{
						QString temp = iVal;
						temp = temp.remove("\"", Qt::CaseSensitive);
						if (temp.length() == 1)
							rc = true;
						else
							rc = false;
					}
					// New format of STRING
					if (iVal.startsWith("'") && iVal.endsWith("'"))
					{
						QString temp = iVal;
						temp = temp.remove("'", Qt::CaseSensitive);
						if (temp.length() == 1)
							rc = true;
						else
							rc = false;
					}
				}
				else if ((paramValueType.toLower() == "string") || (paramValueType.toLower() == "opaque"))
				{
					// Old format of STRING
					if (iVal.startsWith("\"") && iVal.endsWith("\""))
					{
						rc = true;
					}
					// New format of STRING
					else if (iVal.startsWith("'") && iVal.endsWith("'"))
					{
						rc = true;
					}
					else
						rc = false;
				}
			}
			else
			{
				GTAICDParameter paramInfo1 = _ParamList.value(iVal);
				QString ValValueType = paramInfo1.getValueType();
				if (paramValueType == ValValueType)
					rc = true;
				else
					rc = false;
			}
		}
		else
			rc = false;
	}
	else
		rc = false;
	return rc;
}

bool GTACheckCompatibility::checkIsParamSubscribed(const QString &iParam)
{
	if (_ParamSubscribed.isEmpty())
		return false;

	// It is very useful to manage the CaseInsensitive
	if (_ParamSubscribed.contains(iParam, Qt::CaseInsensitive))
		return true;
	else
		return false;
}


bool GTACheckCompatibility::checkIsSubscribeValid(const QString &iParam)
{
	bool valid = true;
	bool rc2 = false;
	GTAAppController* pController = GTAAppController::getGTAAppController();
	QString dbFile = pController->getSystemService()->getDatabaseFile();
	GTADataController dataCtrlr;
	QStringList invalidParams;

	// Check if the Parameter is already subscribed, if not we append it
	if (_ParamSubscribed.contains(iParam, Qt::CaseInsensitive))
	{
		valid = false;
	}
	else
	{
		_ParamSubscribed.append(iParam);
		// Update _ParamList with Parameters from the Init PIR command
		rc2 = dataCtrlr.getParametersDetail(QStringList(iParam), dbFile, _ParamList, invalidParams);
	}
	// Avoid to have duplicates
	_ParamSubscribed.removeDuplicates();
	return valid;
}


bool GTACheckCompatibility::checkIsUnsubscribeValid(const QString &iParam)
{
	// Check if the Parameter has already been unsubscribed or not subscribed
	if (!_ParamSubscribed.contains(iParam, Qt::CaseInsensitive))
	{
		return false;         //subscribe param is invalid
	}
	else
		return true;
}

bool GTACheckCompatibility::checkValidTimeOut(GTAActionDoWhile *pCmd)
{
	if (pCmd)
	{
		double dParentTimeOut = 0;
		double dTotalTimeOut = 0;
		dParentTimeOut = pCmd->getTimeOutInMs();

		if (pCmd->hasChildren())
		{
			QList<GTACommandBase*> lstChildren = pCmd->getChildren();

			for (int i = 0; i < lstChildren.count(); i++)
			{
				GTACommandBase *pChild = lstChildren.at(i);
				if (pChild)
				{
					if (pChild->getCommandType() == GTAActionBase::ACTION)
					{
						GTAActionBase * pActCmd = dynamic_cast<GTAActionBase *>(pChild);
						if (pActCmd)
							dTotalTimeOut += pActCmd->getTimeOutInMs();
					}
					else if (pChild->getCommandType() == GTAActionBase::CHECK)
					{
						GTACheckBase *pChkCmd = dynamic_cast<GTACheckBase *>(pChild);
						if (pChkCmd)
							dTotalTimeOut += pChkCmd->getTimeOutInMs();
					}
				}
			}
		}

		if (dTotalTimeOut <= dParentTimeOut)
			return true;
		else
			return false;
	}
	return false;
}

/* This function allow to check if the Parameter contains invalid chars
 * @input : text - Parameter name
 * @output : chars - List of invalid chars detected
 * @return : bool
*/
bool GTACheckCompatibility::checkInvalidChars(QString text, QStringList &chars)
{
	GTAParamValidator validator;
	int pos = 0;

	if (text.contains("_uuid[{"))
		return true;

	if (validator.validate(text, pos) == QValidator::Invalid)
	{
		chars.append(validator.getInvalidChars(text));
		return true;
	}
	else
		return false;
}

/* This function allow to check if the Parameter has a Triplet format
 * @input : iParam - Parameter name
 * @return : bool
*/
bool GTACheckCompatibility::checkIsParamDbType(const QString &iParam)
{
	QStringList l_paramListKeys = _ParamList.keys();

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		return true;
	}
	else
		return false;

}

bool GTACheckCompatibility::checkIsConstant(const QString &iVal, QString &oParamType)
{
	bool rc = false;
	oParamType = "Invalid_Type";

	QStringList l_paramListKeys = _ParamList.keys();

	// _ParamList contains only ICD Parameters, if LocalParameter we go out 
	if (l_paramListKeys.contains(iVal, Qt::CaseInsensitive))
	{
		oParamType = _ParamList.value(l_paramListKeys.filter(iVal, Qt::CaseInsensitive)[0]).getValueType();
		if (oParamType.toLower().contains("integer") || oParamType.toLower().contains("int"))
		{
			oParamType = "Integer";
		}
		else if (oParamType.toLower().contains("char"))
		{
			oParamType = "Char";
		}
		else if (oParamType.toLower().contains("string"))
		{
			oParamType = "String";
		}
		else if (oParamType.toLower().contains("bool") || oParamType.toLower().contains("boolean"))
		{
			oParamType = "Boolean";
		}
		else if (oParamType.toLower().contains("float"))
		{
			oParamType = "Float";
		}
		if (oParamType.isEmpty())
			oParamType = "NA";
		return false;
	}
	// Test Float value
	if (iVal.contains("."))
	{
		iVal.toDouble(&rc);
		if (rc)
		{
			oParamType = "Float";
			return rc;
		}
	}
	// Test Integer value
	iVal.toInt(&rc, 10);
	if (rc)
	{
		oParamType = "Integer";
		return rc;
	}
	// Test Boolean value
	if ((iVal == "0") || (iVal == "1") || (iVal == "true") || (iVal == "false"))
	{
		oParamType = "Boolean";
		return true;
	}

	// Test New format of STRING
	if (iVal.startsWith("'") && iVal.endsWith("'"))
	{
		QString temp = iVal;
		temp = temp.remove("'", Qt::CaseSensitive);
		if (temp.length() >= 1)
		{
			oParamType = "String";
			return true;
		}
	}
	// Test Old format of STRING
	if (iVal.startsWith("\"") && iVal.endsWith("\""))
	{
		QString temp = iVal;
		temp = temp.remove("\"", Qt::CaseSensitive);
		if (temp.length() >= 1)
		{
			oParamType = "String";
			return true;
		}
	}
	return rc;
}

GTAActionBase* getForEachGrandParent(GTAActionSetList* setCmd) {
	if (setCmd == nullptr) {
		return nullptr;  // Initially check if the input is null
	}

	// Ensure current is valid and get its parent
	GTAActionBase* parent = dynamic_cast<GTAActionBase*> (setCmd->getParent());
	if (parent == nullptr) {
		return nullptr;  // No parent, so return null
	}

	// Continue with the grandparent check
	while (parent != nullptr) {
		GTAActionBase* grandParent = dynamic_cast<GTAActionBase*> (parent->getParent());
		if (grandParent == nullptr) {
			break;  // No further parents available
		}

		QString action = grandParent->getAction();
		QString complement = grandParent->getComplement();
		if (action == ACT_CONDITION && complement == COM_CONDITION_FOR_EACH) {
			return grandParent;  // Found the matching grandparent, return the pointer
		}

		// Move to the next level in the hierarchy
		parent = grandParent;
	}

	return nullptr;  // Return null if no matching grandparent is found
}

QString columnIndexToExcelColumn(int index) {
	QString result;
	while (index >= 0) {
		int remainder = index % 26;
		result.prepend(QChar('A' + remainder));
		index = index / 26 - 1;
	}
	return result;
}

/* This function gets the command for each row in the current file and then manage its content
* @input : ipCmd
* @input : oErrorList
* @input : uuid
* @input : iRowIdx - Index of the Row
* @input : isCall
* @input : iCallElemName
* @return : bool
*/
bool GTACheckCompatibility::getCommandStatus(GTACommandBase *& ipCmd, QList<ErrorMessage>& oErrorList, QString uuid, QStringList &, const int iRowIdx, bool isCall, const QString &iCallElemName)
{
	bool rc = true;
	GTACommandBase::CommandType cmdType = ipCmd->getCommandType();
	QString id = ipCmd->getObjId();
	if (cmdType == GTACommandBase::ACTION)
	{
		GTAActionBase * pActCmd = dynamic_cast<GTAActionBase *>(ipCmd);
		QString actType = pActCmd->getAction();
		if (actType == ACT_SET)
		{
			GTAActionSetList *pSetList = dynamic_cast<GTAActionSetList *>(pActCmd);
			QList <GTAActionSet*>  _setActionList;
			pSetList->getSetCommandlist(_setActionList);
			CmdInfo sCmdInfo;
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;
			sCmdInfo.cmdType = actType;
			sCmdInfo.pCmd = ipCmd;
			sCmdInfo.rowIdx = iRowIdx;

			for (int i = 0; i<_setActionList.count(); i++)
			{
				QString paramType;
				GTAActionSet *pSet = _setActionList.at(i);
				QString param = QString();
				QString val = QString();

				GTAEquationBase *eq = pSet->getEquation();
				GTAEquationConst* pEqnConst = dynamic_cast<GTAEquationConst*>(eq);
				bool setForEach = false;
				if (NULL != pEqnConst)
				{
					param = pSet->getParameter().trimmed(); 
					if (pEqnConst->getConstant().contains("line.") || param.contains("line."))
					{
						setForEach = true;
					}
				}

				bool rc2 = false;
				GTAAppController* pController = GTAAppController::getGTAAppController();
				QString dbFile = pController->getSystemService()->getDatabaseFile();
				GTADataController dataCtrlr;

				QStringList invalidParams;
				QStringList emptyCases;

				// FOR (SET) Action in LOOP
				if (!_ForColHeader.isEmpty() && setForEach)
				{
					GTAActionForEach *pCond = dynamic_cast<GTAActionForEach *>(ipCmd->getParent());

					GTAActionBase* _ForEachGrandParent = getForEachGrandParent(pSetList);
					
					if (_ForEachGrandParent != nullptr)
					{
						pCond = dynamic_cast<GTAActionForEach*> (_ForEachGrandParent);
					}

					if (pCond != nullptr)
					{
						// Open and read the CSV file
						bool rc = false;
						// Contains the state of action (ENUM NbColumn) in *.h
						_nbColumn = SET_CLASSIQUE;

						QString FileToParse = QString();
						if (pCond->getRelativePath().isEmpty())
						{
							FileToParse = pCond->getPath();
						}
						else
						{
							QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
							FileToParse = repoPath + pCond->getRelativePath();
						}
						// Need to know the delimiter in the CSV file
						QString delim = pCond->getDelimiterValue();

						QFile fileObj(FileToParse);
						rc = fileObj.open(QFile::ReadOnly | QFile::Text);
						QStringList lines;
						QStringList headers;
						int fsColumnIndex = -1;

						// Get the columns name to be read to perform the ForEach command
						QString fColumnName = pSet->getParameter();
						if (fColumnName.contains("__"))
						{
							if (fColumnName.split("__").last() != _fColumnName)
								_fColumnName = fColumnName.split("__").last();
							_nbColumn = WRITE_BACK;
						}

						QString sColumnName = pSet->getValue();
						if (sColumnName.contains("__"))
						{
							if (sColumnName.split("__").last() != _sColumnName)
								_sColumnName = sColumnName.split("__").last();

							if (!sColumnName.startsWith("Result", Qt::CaseInsensitive))
							{
								if (_nbColumn == 3)
									_nbColumn = NEW_FOREACH;
								else if (_nbColumn == 0)
									_nbColumn = OLD_FOREACH;
								else
								{
									// Nothing to do
								}
							}
						}

						QString FSColumnName = pSet->getFunctionStatus();
						if (FSColumnName.contains("__"))
						{
							_FSColumnName = FSColumnName.split("__").last();
						}

						// File can be opened
						int lineCount = 0;
						if (rc)
						{		
							int fColumnIndex = -1;
							int sColumnIndex = -1;
							QStringList paramListFromCsv;
							QString line = QString();

							while (!fileObj.atEnd())
							{
								/* _nbColumn
								*    0  :  SET_CLASSIQUE
								*	 1  :  param && line.value
								*	 2  :  line.param && line.value
								*	 3  :  line.param && value (Exchange param/value)
								*/
								
								line = fileObj.readLine().simplified();
								lines.append(line);
								line.replace(",", delim);
								if (line.contains(delim) && fColumnIndex >= 0 && sColumnIndex >= 0 && pSet->getParameter().contains("__") && pSet->getValue().contains("__"))
								{
									_nbColumn = NEW_FOREACH;
								}
								paramListFromCsv = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

								if (!line.isEmpty())
								{
									if (lineCount > 0)
									{
										if (_nbColumn == 1)
										{
											QString valueExtracted = paramListFromCsv.at(sColumnIndex).trimmed();
											_ValuesList.insert(lineCount - 1, valueExtracted);
											if (valueExtracted.isEmpty())
											{
												emptyCases.append(QString::number(lineCount+1) + ";" + columnIndexToExcelColumn(sColumnIndex));
											}
										}
										if (_nbColumn == 2)
										{
											QString tripletExtracted;
											QString valueExtracted;

											if (fColumnIndex >= 0 && fColumnIndex < paramListFromCsv.size() && sColumnIndex >= 0 && sColumnIndex < paramListFromCsv.size()) {
												tripletExtracted = paramListFromCsv.at(fColumnIndex).trimmed();
												valueExtracted = paramListFromCsv.at(sColumnIndex).trimmed();
											}
											else {
												throw std::out_of_range("Column index out of range");
											}
																						
											_TripletsList.insert(lineCount - 1, tripletExtracted);
											_ValuesList.insert(lineCount - 1, valueExtracted);
											if (tripletExtracted.isEmpty())
											{
												emptyCases.append(QString::number(lineCount+1) + ";" + columnIndexToExcelColumn(fColumnIndex));
											}
											if (valueExtracted.isEmpty())
											{
												emptyCases.append(QString::number(lineCount+1) + ";" + columnIndexToExcelColumn(sColumnIndex));
											}
										}
										if (_nbColumn == 3)
										{
											QString tripletExtracted = paramListFromCsv.at(fColumnIndex).trimmed();
											_TripletsList.insert(lineCount - 1, tripletExtracted);
											if (tripletExtracted.isEmpty())
											{
												emptyCases.append(QString::number(lineCount + 1) + ";" + columnIndexToExcelColumn(fColumnIndex));
											}
										}
									}
									else
									{
										if (_nbColumn >= 2) // Contains both the NEW_FOREACH and the WRITE_BACK action
										{
											// Research of the index of each column name int the ParamListFromCsv QStringList
											for (int i = 0; i < paramListFromCsv.size(); i++)
											{
												if (QString::compare(paramListFromCsv[i], _fColumnName) == 0)
												{
													fColumnIndex = i;
													break;
												}
											}
										}
										for (int j = 0; j < paramListFromCsv.size(); j++)
										{
											if (QString::compare(paramListFromCsv[j], _sColumnName) == 0)
											{
												sColumnIndex = j;
												break;
											}
										}
									}
								}
								lineCount++;
								paramListFromCsv.clear();
							}
							fileObj.close();
						}

						if (!lines.isEmpty() && rc && !_FSColumnName.isEmpty()) {
							// Process headers separately
							headers = QString(lines.first()).split(delim);
							fsColumnIndex = headers.indexOf(_FSColumnName);

							// Remove header from processing lines
							lines.removeFirst();
						}

						// Separate loop to check empty values in the FS column
						lineCount = 1;  // Start at 1 since line 0 is headers
						for (const QString& line : lines) {
							QStringList paramListFromCsv = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

							if (fsColumnIndex != -1 && fsColumnIndex < paramListFromCsv.size()) {
								QString value = paramListFromCsv.at(fsColumnIndex).trimmed();
								if (value.isEmpty()) {
									emptyCases.append(QString::number(lineCount + 1) + ";" + columnIndexToExcelColumn(fsColumnIndex));
								}
							}
							lineCount++;
						}

						// Fill in the QHash according to the number of chosen columns
						if (_nbColumn == 1)
						{
							param = pSet->getParameter().trimmed();
							// Update _ParamList with Parameters from the SET command
							rc2 = dataCtrlr.getParametersDetail(QStringList(param), dbFile, _ParamList, invalidParams);
						}
						else
						{
							param = pSet->getParameter().trimmed();
							if (!_TripletsList.values().isEmpty())
							{
								// Update _ParamList with Parameters from CSV File
								rc2 = dataCtrlr.getParametersDetail(_TripletsList.values(), dbFile, _ParamList, invalidParams);
							}
							else
							{
								// Update _ParamList with Parameters from the SET command
								rc2 = dataCtrlr.getParametersDetail(QStringList(param), dbFile, _ParamList, invalidParams);
							}
						}

						GTAEquationBase *eq = pSet->getEquation();
						GTAEquationConst* pEqnConst = dynamic_cast<GTAEquationConst*>(eq);

						if (NULL != pEqnConst)
						{
                            if (!pEqnConst->getConstant().contains("line."))
							{
								param = pSet->getParameter().trimmed();
								val = pSet->getValue().trimmed();
								if (param.contains("line."))
								{
									for (int j = 0; j < _TripletsList.count(); j++)
									{
										param = _TripletsList.value(j);
										addToSetParamList(ipCmd, pSet, emptyCases, sCmdInfo, param, val, iRowIdx, isCall, iCallElemName);
									}
								}
								else
								{
									addToSetParamList(ipCmd, pSet, emptyCases, sCmdInfo, param, val, iRowIdx, isCall, iCallElemName);	
								}
							}
							else // val contains ("line.")
							{
								for (int j = 0; j < _ValuesList.count(); j++)
								{
									// CSV file has only one column (Value)
									if (_nbColumn == 1)
									{
										param = pSet->getParameter().trimmed();
									}
									// CSV file has two columns (Param and Value)
									else
									{
										param = _TripletsList.value(j);
									}

									val = _ValuesList.value(j);

									GTAEquationBase *eq = pSet->getEquation();
                                    QString currentSignalType = _ParamList.value(param).getSignalType();
									//QString currentSignalType = currentParam->getSignalType();
                                    //bool isSetOnlyValue = _ParamList.value(param)->isOnlyValueDueToSignalType(currentSignalType);
									bool isSetOnlyFSFixed = pSet->getIsSetOnlyFSFixed();
									bool isSetOnlyFSVariable = pSet->getIsSetOnlyFSVariable();
									ParamInfo sParamInfo;
									ParamInfo sValInfo;
									QStringList invalidChars;
									bool containsInvalidChars = checkInvalidChars(param, invalidChars);
									QString srcType = QString();
									bool isInDB = false;

									sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
									if (srcType == PARAM_TYPE_PIR)
									{
										sParamInfo.isPIR = true;
										sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
									}

									sParamInfo.isParamLocal = checkIsLocal(param);
									sParamInfo.containsInvalidChars = containsInvalidChars;
									sParamInfo.invalidChars = invalidChars;
									sParamInfo.isParamReleased = false;
									sParamInfo.isParamUsed = true;
									if (isInDB)
										sParamInfo.isParamVmac = true;
									sParamInfo.isValCorrect = checkValidVal(param, val.trimmed(), isInDB);
									sParamInfo.isValApplicable = true;
									sParamInfo.isFirst = false;
									sParamInfo.val = val;
									sParamInfo.paramName = param;
									sParamInfo.isConstant = checkIsConstant(param, paramType);
                                    sParamInfo.valueType = paramType;
									sParamInfo.isInRange = checkForRange(param, val, sParamInfo.rangeMinVal, sParamInfo.rangeMaxVal);
									sParamInfo.isParamDirectionCorrect = checkParamDirection(ACT_SET, param);
									sParamInfo.emptyCSVCases = emptyCases;
									if (checkIsParamInternal(param))
									{
										sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
									}

									if ((eq != NULL) && (isSetOnlyFSFixed == false) && (isSetOnlyFSVariable == false))
									{
										if (eq->getEquationType() == GTAEquationBase::CONST)
										{
											bool isInDB = false;
											sParamInfo.isEquation = false;
											srcType = QString();
											sValInfo.isParamInDb = checkIsInDb(val, isInDB, srcType);
											if (srcType == PARAM_TYPE_PIR)
											{
												sValInfo.isPIR = true;
												sValInfo.isParamSubscribed = checkIsParamSubscribed(val);
											}

											sValInfo.isParamLocal = checkIsLocal(val);
											sValInfo.isParamReleased = false;
											sValInfo.isParamUsed = true;
											if (isInDB)
												sValInfo.isParamVmac = true;
											sValInfo.isValCorrect = false;
											sValInfo.isValApplicable = false;
											sValInfo.isFirst = false;
											sValInfo.paramName = val;
											sValInfo.val = "";
											sValInfo.isConstant = checkIsConstant(val.trimmed(), paramType);
											sValInfo.isParamDirectionCorrect = checkParamDirection(ACT_SET, val);
											if (checkIsParamInternal(val))
											{
												sValInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, val);
											}
											sValInfo.valueType = paramType;
											invalidChars.clear();
											containsInvalidChars = checkInvalidChars(val, invalidChars);
											sValInfo.containsInvalidChars = containsInvalidChars;
											sValInfo.invalidChars = invalidChars;
											sCmdInfo.paramList.append(sValInfo);
											if (sValInfo.isParamVmac)
											{
												VmacParamInfo vmacInfo;
												vmacInfo.paramInfo = sValInfo;
												vmacInfo.pCmd = ipCmd;
												vmacInfo.rowIdx = iRowIdx;
												vmacInfo.cmdType = actType;
												vmacInfo.isCall = isCall;
												vmacInfo.CallName = iCallElemName;
												_VmacParamInfo.append(vmacInfo);
											}
										}
										else
										{
											sParamInfo.isEquation = true;
											//This has been added to take care of inputs in a liquid ramp.
											//Text = "LGCIU_1.LGCIU_1_A429_BUS.BNGPRESD_LGCIU1_1_SG+60", contains some invalid Charcters( + )
											sValInfo.containsInvalidChars = false;
											invalidChars.clear();
											sValInfo.invalidChars = invalidChars;
											sValInfo.isParamLocal = false;
										}
									}
									else if (isSetOnlyFSFixed == true || isSetOnlyFSVariable == true)
									{
										sParamInfo.isSetCheckFS = true;
										sParamInfo.isValApplicable = false;
										// if the Media is AFDX, CAN and A429 and Src Type is ICD, then FS is applicable to them
										QString MediaType = getParamMediaType(param);
										QString srcType = getParamSrcType(param);
										if ((srcType == "ICD") && ((MediaType == "AFDX") || (MediaType == "CAN") || (MediaType == "A429")))
											sParamInfo.isValidFSType = true;
										else
											sParamInfo.isValidFSType = false;
									}

									sCmdInfo.paramList.append(sParamInfo);

									if (sParamInfo.isParamVmac)
									{
										VmacParamInfo vmacInfo;
										vmacInfo.paramInfo = sParamInfo;
										vmacInfo.pCmd = ipCmd;
										vmacInfo.rowIdx = iRowIdx;
										vmacInfo.cmdType = actType;
										vmacInfo.isCall = isCall;
										vmacInfo.CallName = iCallElemName;
										_VmacParamInfo.append(vmacInfo);
									}
								}
							}
						}
					}
				}
				// For (SET) Action
				else
				{
					param = pSet->getParameter().trimmed();
					// input : EEC_CFM1_`CHA:CIC`.`EEC1_A:CIC`2.HPSOV_Closed_270_14
					if (param.contains(":CIC") || param.contains(":NCIC"))
					{
						param.replace("'", "");
						param.replace("`", "");
						param.replace(":NCIC", "");
						param.replace(":CIC", "");
					}
					// output : EEC_CFM1_CHA.EEC1_A2.HPSOV_Closed_270_14

                    val = pSet->getValue().trimmed();
					QString l_valType = QString();
					bool l_isGetValue = false;

					// Specific case for External Tool (Get) Function
					if (val.startsWith("Result", Qt::CaseInsensitive))
					{
						// We have Result.zone_uuidXxxxX__clickOnXY
						QList<CommandInternalsReturnInfo> retInfoList;
						pController->getCommandList(retInfoList, "External_Tool");

						if (!retInfoList.isEmpty() && val.contains("__"))
						{
							QString l_retNameFirst = val.split("__").first();
							QString retName = val.split("__").first();

							if (l_retNameFirst.contains("_"))
							{
								// We have to get the RetName (Result.XxX)
								retName = l_retNameFirst.split("_uuid").first();
							}

							foreach(CommandInternalsReturnInfo elm, retInfoList)
							{
								// These information come from External Tool Configuration File
								if (elm.retName == retName)
								{
									l_valType = elm.retType;
									l_isGetValue = true;
									_LocalResultExtTool.insert(param, l_valType);
								}
							}
						}
					}

					bool isSetOnlyFSFixed = pSet->getIsSetOnlyFSFixed();
					bool isSetOnlyFSVariable = pSet->getIsSetOnlyFSVariable();
					bool isSetOnlyValue = pSet->getIsSetOnlyValue();

					ParamInfo sParamInfo;
					QStringList invalidChars;
					bool containsInvalidChars = checkInvalidChars(param, invalidChars);
					QString srcType = QString();
					bool isInDB = false;

					if (true == checkIsEngineParameter(param))
					{
						QStringList invalidParams;
						sParamInfo.isEngineParamInDb = checkForChannelInControl(param, invalidParams);
						sParamInfo.invalidEngineParams = invalidParams;
					}
					else
					{
						sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
					}

					sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
					if (srcType == PARAM_TYPE_PIR)
					{
						sParamInfo.isPIR = true;
						sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
					}

					sParamInfo.isParamLocal = checkIsLocal(param);
					sParamInfo.containsInvalidChars = containsInvalidChars;
					sParamInfo.invalidChars = invalidChars;
					sParamInfo.isParamReleased = false;
					sParamInfo.isParamUsed = true;
					if (isInDB)
						sParamInfo.isParamVmac = true;
					sParamInfo.isValCorrect = checkValidVal(param, val.trimmed(), isInDB);
					sParamInfo.isValApplicable = true;
					sParamInfo.isTagVar = val.contains("@");
					sParamInfo.isTagVarValid = isValidTagVar(val);
					sParamInfo.isFirst = false;
					sParamInfo.val = val;
					sParamInfo.paramName = param;
					sParamInfo.isConstant = checkIsConstant(param, paramType);
					sParamInfo.emptyCSVCases = emptyCases;
					// We make the difference between classic assignment and getting value from ExternalTool
					if (l_isGetValue)
					{
						sParamInfo.valueType = l_valType;
						sParamInfo.nextType = sParamInfo.valueType;
					}
					else
					{
						sParamInfo.valueType = paramType;
						sParamInfo.nextType = paramType;
					}
					sParamInfo.isInRange = checkForRange(param, val, sParamInfo.rangeMinVal, sParamInfo.rangeMaxVal);
					sParamInfo.isParamDirectionCorrect = checkParamDirection(ACT_SET, param);
					if (checkIsParamInternal(param))
					{
						sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
					}

					if (isInDB)
					{
						sParamInfo.isSetCheckFS = true;
						sParamInfo.isValApplicable = false;
						QString MediaType = getParamMediaType(param);
						QString srcType = getParamSrcType(param);

						if (isSetOnlyFSFixed == true || isSetOnlyFSVariable == true)
						{
							// if the Media is AFDX, CAN and A429 and Src Type is ICD, then FS is applicable to them
							if (srcType == "ICD" && (MediaType == "AFDX" || MediaType == "CAN" || MediaType == "A429"))
								sParamInfo.isValidFSType = true;
							else
								sParamInfo.isValidFSType = false;
						}

						// Check Only Value
						if (isSetOnlyValue == true)
						{
							if (MediaType != "DIS" || MediaType != "ANA" || MediaType != "VMAC" || MediaType != "PIR" || MediaType != " MODEL")
								sParamInfo.isValidFSType = true;
							else
								sParamInfo.isValidFSType = false;
						}

						if (isSetOnlyFSFixed == false && isSetOnlyFSVariable == false && isSetOnlyValue == false)
						{
							if (srcType == "ICD" && (MediaType == "AFDX" || MediaType == "CAN" || MediaType == "A429") && 
								(MediaType != "DIS" || MediaType != "ANA" || MediaType != "VMAC" || MediaType != "PIR" || MediaType != " MODEL"))
								sParamInfo.isValidFSType = true;
							else
								sParamInfo.isValidFSType = false;
						}
					}
					else
					{
						// Parameter is not in DB
						sParamInfo.isSetCheckFS = false;
						sParamInfo.isValidFSType = false;
					}
					sCmdInfo.paramList.append(sParamInfo);

					if (sParamInfo.isParamVmac)
					{
						VmacParamInfo vmacInfo;
						vmacInfo.paramInfo = sParamInfo;
						vmacInfo.pCmd = ipCmd;
						vmacInfo.rowIdx = iRowIdx;
						vmacInfo.cmdType = actType;
						vmacInfo.isCall = isCall;
						vmacInfo.CallName = iCallElemName;
						_VmacParamInfo.append(vmacInfo);
					}
				}
			}
			_CommandValidityInfo.append(sCmdInfo);
		}
		else if ((actType == ACT_CONDITION) && (pActCmd->getComplement() == COM_CONDITION_IF))
		{
			GTAActionIF *pCond = dynamic_cast<GTAActionIF *>(pActCmd);

			bool isCheckAudioAlarm = false;
			isCheckAudioAlarm = pCond->getChkAudioAlarm();
			if (isCheckAudioAlarm == true)
				return true;

			bool isSetOnlyFS = pCond->getIsFsOnly();
			bool isSetOnlyValue = pCond->getIsValueOnly();

			CmdInfo sCmdInfo;
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;
			sCmdInfo.cmdType = actType;
			sCmdInfo.pCmd = ipCmd;
			sCmdInfo.rowIdx = iRowIdx;

			QString paramType;

			QString param = pCond->getParameter().trimmed();
			QString val = pCond->getValue().trimmed();

			ParamInfo sParamInfo;
			QStringList invalidChars;
			bool containsInvalidChars = checkInvalidChars(param, invalidChars);
			bool isInDB = false;
			QString srcType = QString();
			
			if (true == checkIsEngineParameter(param))
			{
				QStringList invalidParams;
				sParamInfo.isEngineParamInDb = checkForChannelInControl(param, invalidParams);
				sParamInfo.invalidEngineParams = invalidParams;
			}
			else
			{
				sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
				sParamInfo.isParamUsed = false;
			}

			if (srcType == PARAM_TYPE_PIR)
			{
				sParamInfo.isPIR = true;
				sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
				sParamInfo.isParamUsed = true;
			}

			sParamInfo.isParamLocal = checkIsLocal(param);
			sParamInfo.isParamReleased = false;
			if (isInDB)
				sParamInfo.isParamVmac = true;
			sParamInfo.isValCorrect = checkValidVal(param, val.trimmed(), isInDB);
			sParamInfo.isValApplicable = true;
			sParamInfo.isTagVar = val.contains("@");
			sParamInfo.isTagVarValid = isValidTagVar(val);
			sParamInfo.isFirst = false;
			sParamInfo.val = val;
			sParamInfo.paramName = param;
			sParamInfo.isConstant = checkIsConstant(param, paramType);
			sParamInfo.valueType = paramType;
			sParamInfo.containsInvalidChars = containsInvalidChars;
			sParamInfo.invalidChars = invalidChars;
			sParamInfo.isEquation = false;
			sParamInfo.isInRange = checkForRange(param, val, sParamInfo.rangeMinVal, sParamInfo.rangeMaxVal);
			sParamInfo.isParamDirectionCorrect = checkParamDirection(ACT_CONDITION, param);
			if (checkIsParamInternal(param))
			{
				sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
			}
			
			if (isInDB)
			{
				sParamInfo.isSetCheckFS = true;
				sParamInfo.isValApplicable = false;
				setValidFSTypeInParamInfo(sParamInfo, param, isSetOnlyFS, isSetOnlyValue);
			}
			else
			{
				// Parameter is not in DB
				sParamInfo.isSetCheckFS = false;
				sParamInfo.isValApplicable = false;
				sParamInfo.isValidFSType = false;
			}
			
			sCmdInfo.paramList.append(sParamInfo);
			if (sParamInfo.isParamVmac)
			{
				VmacParamInfo vmacInfo;
				vmacInfo.paramInfo = sParamInfo;
				vmacInfo.pCmd = ipCmd;
				vmacInfo.rowIdx = iRowIdx;
				vmacInfo.cmdType = actType;
				vmacInfo.isCall = isCall;
				vmacInfo.CallName = iCallElemName;
				_VmacParamInfo.append(vmacInfo);
			}			
			_CommandValidityInfo.append(sCmdInfo);
		}
		else if ((actType == ACT_CONDITION) && (pActCmd->getComplement() == COM_CONDITION_DO_WHILE))
		{
			GTAActionDoWhile *pCond = dynamic_cast<GTAActionDoWhile *>(pActCmd);

			bool isCheckAudioAlarm = false;
			isCheckAudioAlarm = pCond->getChkAudioAlarm();
			if (isCheckAudioAlarm == true)
				return true;

			CmdInfo sCmdInfo;
			sCmdInfo.cmdType = actType;
			sCmdInfo.pCmd = ipCmd;
			sCmdInfo.rowIdx = iRowIdx;
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;

			QString paramType;
			QString param = pCond->getParameter().trimmed();
			QString val = pCond->getValue().trimmed();
			bool isSetOnlyFS = pCond->getIsFsOnly();
			bool isSetOnlyValue = pCond->getIsValueOnly();
			ParamInfo sParamInfo;
			bool isInDB = false;
			QString srcType = QString();

			if (true == checkIsEngineParameter(param))
			{
				QStringList invalidParams;
				sParamInfo.isEngineParamInDb = checkForChannelInControl(param, invalidParams);
				sParamInfo.invalidEngineParams = invalidParams;
			}
			else
			{
				sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
				sParamInfo.isParamUsed = false;
			}

			if (srcType == PARAM_TYPE_PIR)
			{
				sParamInfo.isPIR = true;
				sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
				sParamInfo.isParamUsed = true;
			}

			sParamInfo.isParamLocal = checkIsLocal(param);
			sParamInfo.isParamReleased = false;
			
			if (isInDB)
				sParamInfo.isParamVmac = true;
			sParamInfo.isValCorrect = checkValidVal(param, val.trimmed(), isInDB);
			sParamInfo.isValApplicable = true;
			sParamInfo.isTagVar = val.contains("@");
			sParamInfo.isTagVarValid = isValidTagVar(val);
			sParamInfo.isFirst = false;
			sParamInfo.val = val;
			sParamInfo.paramName = param;
			sParamInfo.isConstant = checkIsConstant(param, paramType);
			sParamInfo.valueType = paramType;
			sParamInfo.isEquation = false;
			sParamInfo.isInRange = checkForRange(param, val, sParamInfo.rangeMinVal, sParamInfo.rangeMaxVal);
			QStringList invalidChars;
			bool containsInvalidChars = checkInvalidChars(param, invalidChars);
			sParamInfo.containsInvalidChars = containsInvalidChars;
			sParamInfo.invalidChars = invalidChars;
			sParamInfo.isParamDirectionCorrect = checkParamDirection(ACT_CONDITION, param);

			if (checkIsParamInternal(param))
			{
				sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
			}

			if (isInDB)
			{
				sParamInfo.isSetCheckFS = true;
				sParamInfo.isValApplicable = false;
				setValidFSTypeInParamInfo(sParamInfo, param, isSetOnlyFS, isSetOnlyValue);
			}
			else
			{
				// Parameter is not in DB
				sParamInfo.isSetCheckFS = false;
				sParamInfo.isValApplicable = false;
				sParamInfo.isValidFSType = false;
			}

			if (param.startsWith("line"))
			{
				sParamInfo.isSetCheckFS = true;
				sParamInfo.isValApplicable = false;
				sParamInfo.isValidFSType = true;
			}
			
			sCmdInfo.hasValidTimeOut = checkValidTimeOut(pCond);
			sCmdInfo.paramList.append(sParamInfo);
			if (sParamInfo.isParamVmac)
			{
				VmacParamInfo vmacInfo;
				vmacInfo.paramInfo = sParamInfo;
				vmacInfo.pCmd = ipCmd;
				vmacInfo.rowIdx = iRowIdx;
				vmacInfo.cmdType = actType;
				vmacInfo.isCall = isCall;
				vmacInfo.CallName = iCallElemName;
				_VmacParamInfo.append(vmacInfo);
			}
			_CommandValidityInfo.append(sCmdInfo);
		}
		else if ((actType == ACT_CONDITION) && (pActCmd->getComplement() == COM_CONDITION_WHILE))
		{
			GTAActionWhile *pCond = dynamic_cast<GTAActionWhile *>(pActCmd);

			bool isCheckAudioAlarm = false;
			isCheckAudioAlarm = pCond->getChkAudioAlarm();
			if (isCheckAudioAlarm == true)
				return true;

			CmdInfo sCmdInfo;
			sCmdInfo.cmdType = actType;
			sCmdInfo.pCmd = ipCmd;
			sCmdInfo.rowIdx = iRowIdx;
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;

			QString paramType = QString();
			QString param = pCond->getParameter().trimmed();
			QString val = pCond->getValue().trimmed();
			bool isSetOnlyFS = pCond->getIsFsOnly();
			bool isSetOnlyValue = pCond->getIsValueOnly();
			ParamInfo sParamInfo;
			bool isInDB = false;
			QString srcType = QString();
			
			if (true == checkIsEngineParameter(param))
			{
				QStringList invalidParams;
				sParamInfo.isEngineParamInDb = checkForChannelInControl(param, invalidParams);
				sParamInfo.invalidEngineParams = invalidParams;
			}
			else
			{
				sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
				sParamInfo.isParamUsed = false;
			}
			
			if (srcType == PARAM_TYPE_PIR)
			{
				sParamInfo.isPIR = true;
				sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
				sParamInfo.isParamUsed = true;
			}

			sParamInfo.isParamLocal = checkIsLocal(param);
			sParamInfo.isParamReleased = false;
			
			if (isInDB)
				sParamInfo.isParamVmac = true;
			sParamInfo.isValCorrect = checkValidVal(param, val.trimmed(), isInDB);
			sParamInfo.isValApplicable = true;
			sParamInfo.isTagVar = val.contains("@");
			sParamInfo.isTagVarValid = isValidTagVar(val);
			sParamInfo.isFirst = false;
			sParamInfo.val = val;
			sParamInfo.paramName = param;
			sParamInfo.isConstant = checkIsConstant(param, paramType);
			sParamInfo.valueType = paramType;
			sParamInfo.isEquation = false;
			sParamInfo.isInRange = checkForRange(param, val, sParamInfo.rangeMinVal, sParamInfo.rangeMaxVal);
			QStringList invalidChars;
			bool containsInvalidChars = checkInvalidChars(param, invalidChars);
			sParamInfo.containsInvalidChars = containsInvalidChars;
			sParamInfo.invalidChars = invalidChars;
			sParamInfo.isParamDirectionCorrect = checkParamDirection(ACT_CONDITION, param);

			if (checkIsParamInternal(param))
			{
				sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
			}

			if (isInDB)
			{
				sParamInfo.isSetCheckFS = true;
				sParamInfo.isValApplicable = false;
				setValidFSTypeInParamInfo(sParamInfo, param, isSetOnlyFS, isSetOnlyValue);
			}
			else
			{
				// Parameter is not in DB
				sParamInfo.isSetCheckFS = false;
				sParamInfo.isValApplicable = false;
				sParamInfo.isValidFSType = false;
			}

			if (param.startsWith("line"))
			{
				sParamInfo.isSetCheckFS = true;
				sParamInfo.isValApplicable = false;
				sParamInfo.isValidFSType = true;
			}
			sCmdInfo.paramList.append(sParamInfo);
			if (sParamInfo.isParamVmac)
			{
				VmacParamInfo vmacInfo;
				vmacInfo.paramInfo = sParamInfo;
				vmacInfo.pCmd = ipCmd;
				vmacInfo.rowIdx = iRowIdx;
				vmacInfo.cmdType = actType;
				vmacInfo.isCall = isCall;
				vmacInfo.CallName = iCallElemName;
				_VmacParamInfo.append(vmacInfo);
			}
			_CommandValidityInfo.append(sCmdInfo);
		}
		else if ((actType == ACT_CONDITION) && (pActCmd->getComplement() == COM_CONDITION_FOR_EACH))
		{
			GTAActionForEach *pCond = dynamic_cast<GTAActionForEach *>(pActCmd);
			_ForColHeader.insert(pCond->getObjId(), pCond->getHeaderColumns());

			// We must get the keys in QHash
			foreach(QString key, _ForColHeader.keys())
			{	// We must obtain the QStringList (columnName)
				foreach(QStringList header, _ForColHeader.values(key))
				{	// We must iterate on each element to compare them to "Value" in order to raise an error
					foreach(QString elm, header)
					{
						if (elm.compare("Value") == 0)
						{
							// Return FALSE to have a specific error in Log Window after performing CheckCompatibility function
							return false;
						}
					}
				}
			}
			CmdInfo sCmdInfo;
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;
			sCmdInfo.cmdType = actType;
			sCmdInfo.pCmd = ipCmd;
			sCmdInfo.rowIdx = iRowIdx;
			_CommandValidityInfo.append(sCmdInfo);
		}
		else if ((actType == ACT_CONDITION) && (pActCmd->getComplement() == COM_CONDITION_FOR_EACH_END))
		{
			_ForColHeader.clear();
			_TripletsList.clear();
			_ValuesList.clear();
		}
		else if ((actType == ACT_WAIT) && (pActCmd->getComplement() == COM_WAIT_UNTIL))
		{
			GTAActionWait *pCond = dynamic_cast<GTAActionWait *>(pActCmd);

			CmdInfo sCmdInfo;
			sCmdInfo.cmdType = actType;
			sCmdInfo.pCmd = ipCmd;
			sCmdInfo.rowIdx = iRowIdx;
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;
			bool isSetOnlyFS = pCond->getIsFsOnly();
			bool isSetOnlyValue = pCond->getIsValueOnly();
			QString paramType = QString();

			QString param = pCond->getParameter().trimmed();
			// input : EEC_CFM1_`CHA:CIC`.`EEC1_A:CIC`2.HPSOV_Closed_270_14
			if (param.contains(":CIC") || param.contains(":NCIC"))
			{
				param.replace("'", "");
				param.replace("`", "");
				param.replace(":NCIC", "");
				param.replace(":CIC", "");
			}
			// output : EEC_CFM1_CHA.EEC1_A2.HPSOV_Closed_270_14
			QString val = pCond->getValue().trimmed();

			ParamInfo sParamInfo;
			bool isInDB = false;
			QString srcType = QString();

			if (true == checkIsEngineParameter(param))
			{
				QStringList invalidParams;
				sParamInfo.isEngineParamInDb = checkForChannelInControl(param, invalidParams);
				sParamInfo.invalidEngineParams = invalidParams;
			}
			else
			{
				sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
				sParamInfo.isParamUsed = false;
			}
			
			if (srcType == PARAM_TYPE_PIR)
			{
				sParamInfo.isPIR = true;
				sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
				sParamInfo.isParamUsed = true;
			}

			sParamInfo.isParamLocal = checkIsLocal(param);
			sParamInfo.isParamReleased = false;
			
			if (isInDB)
				sParamInfo.isParamVmac = true;
			sParamInfo.isValCorrect = checkValidVal(param, val.trimmed(), isInDB);
			sParamInfo.isValApplicable = true;
			sParamInfo.isTagVar = val.contains("@");
			sParamInfo.isTagVarValid = isValidTagVar(val);
			sParamInfo.isFirst = false;
			sParamInfo.val = val;
			sParamInfo.paramName = param;
			sParamInfo.isConstant = checkIsConstant(param, paramType);
			sParamInfo.valueType = paramType;
			sParamInfo.isEquation = false;
			sParamInfo.isInRange = checkForRange(param, val, sParamInfo.rangeMinVal, sParamInfo.rangeMaxVal);

			QStringList invalidChars;
			bool containsInvalidChars = checkInvalidChars(param, invalidChars);
			sParamInfo.containsInvalidChars = containsInvalidChars;
			sParamInfo.invalidChars = invalidChars;
			sParamInfo.isParamDirectionCorrect = checkParamDirection("WAIT UNTIL", param);

			if (checkIsParamInternal(param))
			{
				sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
			}
			
			if (isInDB)
			{
				sParamInfo.isSetCheckFS = true;
				sParamInfo.isValApplicable = false;
				setValidFSTypeInParamInfo(sParamInfo, param, isSetOnlyFS, isSetOnlyValue);
			}
			else
			{
				// Parameter is not in DB
				sParamInfo.isSetCheckFS = false;
				sParamInfo.isValApplicable = false;
				sParamInfo.isValidFSType = false;
			}

			if (param.startsWith("line"))
			{
				sParamInfo.isSetCheckFS = true;
				sParamInfo.isValApplicable = false;
				sParamInfo.isValidFSType = true;
			}
			
			sCmdInfo.paramList.append(sParamInfo);
			if (sParamInfo.isParamVmac)
			{
				VmacParamInfo vmacInfo;
				vmacInfo.paramInfo = sParamInfo;
				vmacInfo.pCmd = ipCmd;
				vmacInfo.rowIdx = iRowIdx;
				vmacInfo.cmdType = actType;
				vmacInfo.isCall = isCall;
				vmacInfo.CallName = iCallElemName;
				_VmacParamInfo.append(vmacInfo);
			}
			_CommandValidityInfo.append(sCmdInfo);
		}
		else if ((actType == ACT_WAIT) && (pActCmd->getComplement() == COM_WAIT_FOR))
		{
			GTAActionWait *pCond = dynamic_cast<GTAActionWait *>(pActCmd);

			CmdInfo sCmdInfo;
			sCmdInfo.cmdType = actType;
			sCmdInfo.pCmd = ipCmd;
			sCmdInfo.rowIdx = iRowIdx;
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;

			QString paramType = QString();
			QStringList paramListWait = pActCmd->getVariableList();
			QString param = QString();
			QString val = QString();
			ParamInfo sParamInfo;
			bool isInDB = false;
			QString srcType = QString();

			if (!paramListWait.isEmpty())
			{
				param = paramListWait[0];
			}

			sParamInfo.isTagVar = param.contains("@");
			sParamInfo.isTagVarValid = isValidTagVar(param);

			// This case could be removed - It is useless
			if (param.contains("@") && !_LocalValuesList.isEmpty() && _LocalValuesList.contains(param.replace("@", QString())))
			{
				val = _LocalValuesList.value(param.replace("@", QString()));
				param = param.replace("@", QString());
				pCond->setParameter(param);
				pCond->setValue(val);
			}

			// Manage the case where a local parameter is used
			if (!param.contains("@") && !_LocalValuesList.isEmpty() && _LocalValuesList.contains(param))
			{
				val = _LocalValuesList.value(param);
				pCond->setParameter(param);
				pCond->setValue(val);
			}

			sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
			if (srcType == PARAM_TYPE_PIR)
			{
				sParamInfo.isPIR = true;
			}

			sParamInfo.isParamLocal = checkIsLocal(param);
			sParamInfo.isParamReleased = false;
			sParamInfo.isParamUsed = true;
			if (isInDB)
				sParamInfo.isParamVmac = true;
			sParamInfo.isValCorrect = false;
			sParamInfo.isValApplicable = false;
			sParamInfo.isFirst = false;
			sParamInfo.val = "";
			sParamInfo.paramName = param;
			sParamInfo.isConstant = checkIsConstant(param, paramType);
			sParamInfo.valueType = paramType;
			sParamInfo.isEquation = false;
			QStringList invalidChars;
			bool containsInvalidChars = checkInvalidChars(param, invalidChars);
			sParamInfo.containsInvalidChars = containsInvalidChars;
			sParamInfo.invalidChars = invalidChars;

			if (checkIsParamInternal(param))
			{
				sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
			}

			sCmdInfo.paramList.append(sParamInfo);

			if (sParamInfo.isParamVmac)
			{
				VmacParamInfo vmacInfo;
				vmacInfo.paramInfo = sParamInfo;
				vmacInfo.pCmd = ipCmd;
				vmacInfo.rowIdx = iRowIdx;
				vmacInfo.cmdType = actType;
				vmacInfo.isCall = isCall;
				vmacInfo.CallName = iCallElemName;
				_VmacParamInfo.append(vmacInfo);
			}
			_CommandValidityInfo.append(sCmdInfo);
		}
		else if (actType == ACT_REL)
		{
			GTAActionRelease * pActRel = dynamic_cast<GTAActionRelease *>(pActCmd);
			if (pActRel == NULL)
				return false;
			CmdInfo sCmdInfo;
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;

			QStringList paramList = pActRel->getVariableList();
			foreach(QString param, paramList)
			{
				param = param.trimmed();
				bool found = false;

				ParamInfo sParamInfo;
				bool isInDB = false;
				QString srcType = QString();
				sParamInfo.isParamInDb = checkIsInDb(param.trimmed(), isInDB, srcType);

				if (srcType == PARAM_TYPE_PIR)
				{
					sParamInfo.isPIR = true;
					sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
				}

				sParamInfo.isParamReleased = true;
				if (isInDB)
					sParamInfo.isParamVmac = true;
				sParamInfo.isValCorrect = false;
				sParamInfo.isValApplicable = false;
				sParamInfo.isFirst = false;
				sParamInfo.paramName = param;
				sParamInfo.isEquation = false;
				QStringList invalidChars;
				bool containsInvalidChars = checkInvalidChars(param, invalidChars);
				sParamInfo.containsInvalidChars = containsInvalidChars;
				sParamInfo.invalidChars = invalidChars;

				for (int i = _VmacParamInfo.count() - 1; i >= 0; i--)
				{
					VmacParamInfo info = _VmacParamInfo.at(i);
					if ((info.paramInfo.paramName == param) && (info.paramInfo.isParamUsed == true) && (info.paramInfo.isParamReleased == false) && (info.paramInfo.isFirst == false))
					{
						_VmacParamInfo.removeAt(i);
						found = true;
					}
				}
				if (found == false)
				{
					sParamInfo.isFirst = true;
					VmacParamInfo vmacInfo;
					vmacInfo.paramInfo = sParamInfo;
					vmacInfo.pCmd = ipCmd;
					vmacInfo.rowIdx = iRowIdx;
					vmacInfo.cmdType = actType;
					vmacInfo.isCall = isCall;
					vmacInfo.CallName = iCallElemName;
					_VmacParamInfo.append(vmacInfo);
				}
				sCmdInfo.paramList.append(sParamInfo);
				sCmdInfo.cmdType = actType;
				sCmdInfo.rowIdx = iRowIdx;
				sCmdInfo.pCmd = ipCmd;
				_CommandValidityInfo.append(sCmdInfo);
			}
		}
		else if (actType == ACT_GEN_TOOL)
		{
			CmdInfo sCmdInfo;
			sCmdInfo.subCmdType = GTACheckCompatibility::EXTERNAL_TOOL;

			GTAGenericToolCommand *pGenCmd = dynamic_cast<GTAGenericToolCommand*>(pActCmd);
			QList<GTAGenericFunction> funcList = pGenCmd->getCommand();

			ExternalTool externalTool;
			externalTool.toolName = pGenCmd->getToolName();
			externalTool.isMismatch = pGenCmd->isMismatch();
			for (int i = 0; i < funcList.count(); i++)
			{
				GTAGenericFunction objFunc = funcList.at(i);
				_ReturnParamList.insert(id, objFunc.getReturnParamName());
				_ReturnParamObjIdHash.insert(id, iRowIdx);

				ExternalToolFunc externalToolFunction;
				externalToolFunction.FunctionName = objFunc.getFunctionName();
				QList<GTAGenericArgument> args = objFunc.getFunctionArguments();
				for (int j = 0; j < args.count(); j++)
				{
					GTAGenericArgument objArg = args.at(j);
					ExternalToolArgument extenalToolArg;
					extenalToolArg.ArgName = objArg.getArgumentName();
					QString type = objArg.getArgumentType();
					QString keyValue = objArg.getUserSelectedValue().replace("'", "");
					QString val = QString();
					// Make the difference between a local parameter and an argument - Check presence in QHash
					if (!_LocalValuesList.isEmpty() && (_LocalValuesList.contains(keyValue) || _LocalResultExtTool.contains(keyValue)))
					{
						val = _LocalValuesList.value(keyValue);
						if (_LocalResultExtTool.contains(keyValue))
						{
							QString l_val = _LocalResultExtTool.value(keyValue);
							if (l_val == "int")
								val = "10";
							else if (l_val == "float")
								val = "3.14";
							else if (l_val == "string")
								val = QString("'ok'");
							else
								val = QString("'ERROR'");
						}
					}
					else
					{
						val = objArg.getUserSelectedValue();
					}

					if (type == "bool" || type == "boolean")
					{
						if (val != "true" && val != "false")
						{
							extenalToolArg.isArgumentValid = false;
						}
						else
						{
							extenalToolArg.isArgumentValid = true;
						}
					}
					else if (type == "int" || type == "integer")
					{
						bool ok = false;
						val.toInt(&ok);
						if (!ok)
						{
							extenalToolArg.isArgumentValid = false;
						}
						else
						{
							extenalToolArg.isArgumentValid = true;
						}

					}
					else if (type == "float")
					{
						bool ok = false;
						val.toFloat(&ok);
						if (!ok)
						{
							extenalToolArg.isArgumentValid = false;
						}
						else
						{
							extenalToolArg.isArgumentValid = true;
						}
					}					
                    else if (type == "string")
                    {
						extenalToolArg.isArgumentValid = true; // string can be anything typed
					}
					externalToolFunction.argList.append(extenalToolArg);
				}
				externalTool.funcList.append(externalToolFunction);
			}

			sCmdInfo.externalToolInfo.append(externalTool);

			_CommandValidityInfo.append(sCmdInfo);
		}
		else if (actType == ACT_SUBSCRIBE)
		{
			GTAActionSubscribe *pSubAct = dynamic_cast<GTAActionSubscribe *>(pActCmd);
			if (pSubAct)
			{
				QStringList paramList = pSubAct->getVariableList();
				foreach(QString param, paramList)
				{
					param = param.trimmed();

					CmdInfo sCmdInfo;
					sCmdInfo.cmdType = actType;
					sCmdInfo.pCmd = ipCmd;
					sCmdInfo.rowIdx = iRowIdx;
					sCmdInfo.subCmdType = GTACheckCompatibility::INIT_PIR;
					QString paramType = QString();

					ParamInfo sParamInfo;
					bool isInDB = false;
					QString srcType = QString();

					// If FOR EACH command action is used
					if (param.startsWith("line"))
					{
						bool rc2 = false;
						GTAAppController* pController = GTAAppController::getGTAAppController();
						QString dbFile = pController->getSystemService()->getDatabaseFile();
						GTADataController dataCtrlr;

						QStringList invalidParams;

						// FOR (INIT_PIR) Action in LOOP
						if (!_ForColHeader.isEmpty())
						{
							GTAActionForEach *pCond = dynamic_cast<GTAActionForEach *>(ipCmd->getParent());
							if (pCond != nullptr)
							{
								// Open and read the CSV file
								bool rc = false;
								QString FileToParse = QString();
								if (pCond->getRelativePath().isEmpty())
								{
									FileToParse = pCond->getPath();
								}
								else
								{
									QString repoPath = GTAUtil::getRepositoryPath();
									FileToParse = pCond->getRelativePath();
								}
								// Need to know the delimiter in the CSV file
								QString delim = pCond->getDelimiterValue();

								QFile fileObj(FileToParse);
								rc = fileObj.open(QFile::ReadOnly | QFile::Text);
								
								// Get the columns name to be read to perform the ForEach command
								QString fColumnName = pSubAct->getValues()[_nbParamList];
								if(pSubAct->getValues().count() > 1)
									_nbParamList++;
								if (fColumnName.contains("__"))
								{
									if (fColumnName.split("__").last() != _fColumnName)
										_fColumnName = fColumnName.split("__").last();
								}

								if (rc)
								{
									int lineCount = 0;
									int fColumnIndex = -1;

									QStringList paramListFromCsv;
									QString line = QString();

									while (!fileObj.atEnd())
									{
										line = fileObj.readLine().simplified();
										if (delim=="\t")
											paramListFromCsv = line.split(QRegExp("\\s+"), QString::SkipEmptyParts).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");
										else
											paramListFromCsv = line.split(delim, QString::SkipEmptyParts).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

										if (!line.isEmpty())
										{
											// Iterate through CSV
											if (lineCount > 0)
											{
												_PirsList.append(paramListFromCsv.at(fColumnIndex).trimmed());
											}
											else
											{
												// Research of the index of each column name int the ParamListFromCsv QStringList
												for (int i = 0; i < paramListFromCsv.size(); i++)
												{
													if (QString::compare(paramListFromCsv[i], _fColumnName) == 0)
													{
														fColumnIndex = i;
														break;
													}
												}
											}
										}
										lineCount++;
										paramListFromCsv.clear();
									}
									fileObj.close();
								}
							}
						} // END ForColHeader
						
						// Update _ParamList with Parameters from CSV File
						rc2 = dataCtrlr.getParametersDetail(_PirsList, dbFile, _ParamList, invalidParams);
						
						foreach(QString param, _PirsList)
						{
							CmdInfo sCmdInfo;
							sCmdInfo.cmdType = actType;
							sCmdInfo.pCmd = ipCmd;
							sCmdInfo.rowIdx = iRowIdx;
							sCmdInfo.subCmdType = GTACheckCompatibility::INIT_PIR;

							QString paramType;
							ParamInfo sParamInfo;
							bool isInDB = false;
							QString srcType = QString();
							sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
							
							bool isValidSubs = false;
							// Avoid to subscribe a local variable
							if (sParamInfo.isParamInDb && srcType == PARAM_TYPE_PIR)
							{
								// Add param to _ParamSubscribed list
								isValidSubs = checkIsSubscribeValid(param);
								_ColPirName.append(_fColumnName+"_PIKACHU_"+ param);
								sParamInfo.hasValidSubscribe = isValidSubs;
							}
							else
							{
								// Only for local variable
								sParamInfo.hasValidSubscribe = true;
							}
							
							// This attribute simply means that we are in a PIR command 
							sParamInfo.isPIR = true;
							if (srcType == PARAM_TYPE_PIR)
								sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);

							sParamInfo.isParamLocal = checkIsLocal(param);
							sParamInfo.isParamReleased = false;
							sParamInfo.isParamUsed = true;
							if (isInDB)
							{
								sParamInfo.isParamVmac = true;
								sParamInfo.isParamPIR = checkIsPIR(param);
							}
							else
							{
								sParamInfo.isParamVmac = false;
								sParamInfo.isParamPIR = false;
							}

							sParamInfo.isValCorrect = false;
							sParamInfo.isValApplicable = false;
							sParamInfo.isFirst = false;
							sParamInfo.val = "";
							sParamInfo.paramName = param;
							sParamInfo.isConstant = checkIsConstant(param, paramType);
							sParamInfo.valueType = paramType;
							sParamInfo.isEquation = false;

							if (!isValidSubs)
							{
								sParamInfo.invalidSubscribeParam.append(param);
							}

							QStringList invalidChars;
							bool containsInvalidChars = checkInvalidChars(param, invalidChars);
							sParamInfo.containsInvalidChars = containsInvalidChars;
							sParamInfo.invalidChars = invalidChars;
							if (!sParamInfo.isParamInDb || !sParamInfo.isParamPIR)
								sCmdInfo.paramList.append(sParamInfo);
							
							if (!sParamInfo.isParamPIR)
								_CommandValidityInfo.append(sCmdInfo);
						}
						_PirsList.clear();
					}
					
					// By default, we get at least one Init PIR List command action
					sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);

					if (srcType == PARAM_TYPE_PIR)
					{
						sParamInfo.isPIR = true;
					}
					
					sParamInfo.isParamLocal = checkIsLocal(param);
					sParamInfo.isParamReleased = false;
					sParamInfo.isParamUsed = true;
					if(isInDB)
						sParamInfo.isParamVmac = checkIsVmac(param);
					
					// Avoid to have Unvalid PIR signal
					if (param.startsWith("line"))
						sParamInfo.isParamPIR = true;
					else
						sParamInfo.isParamPIR = checkIsPIR(param);

					sParamInfo.isValCorrect = false;
					sParamInfo.isValApplicable = false;
					sParamInfo.isFirst = false;
					sParamInfo.val = "";
					sParamInfo.paramName = param;
					sParamInfo.isConstant = checkIsConstant(param, paramType);
					sParamInfo.valueType = paramType;
					sParamInfo.isEquation = false;
					bool isValidSubs = checkIsSubscribeValid(param);
					sParamInfo.hasValidSubscribe = isValidSubs;
					if (!isValidSubs)
					{
						sParamInfo.invalidSubscribeParam.append(param);
					}
					QStringList invalidChars;
					bool containsInvalidChars = checkInvalidChars(param, invalidChars);
					sParamInfo.containsInvalidChars = containsInvalidChars;
					sParamInfo.invalidChars = invalidChars;

					// Avoid to have Unvalid PIR signal
					if (param.startsWith("line"))
					{
						sParamInfo.isParamInDb = true;
						sParamInfo.isParamPIR = true;
						sParamInfo.hasValidSubscribe = true;
						sParamInfo.isSetCheckFS = true;
						sParamInfo.isValApplicable = false;
						sParamInfo.isValidFSType = true;
					}
					sCmdInfo.paramList.append(sParamInfo);

					_CommandValidityInfo.append(sCmdInfo);
				}
			}
		}
		else if (actType == ACT_UNSUBSCRIBE)
		{
			GTAActionUnSubscribe *pUnSubAct = dynamic_cast<GTAActionUnSubscribe *>(pActCmd);
			if (pUnSubAct)
			{
				QStringList invalidParams;
								
				QStringList paramList = pUnSubAct->getVariableList();
				if (paramList.isEmpty())
				{
					paramList = _ParamSubscribed;
				}
				foreach(QString param, paramList)
				{
					param = param.trimmed();

					CmdInfo sCmdInfo;
					sCmdInfo.cmdType = actType;
					sCmdInfo.pCmd = ipCmd;
					sCmdInfo.rowIdx = iRowIdx;
					sCmdInfo.subCmdType = GTACheckCompatibility::INIT_PIR;
					QString paramType = QString();

					ParamInfo sParamInfo;
					bool isInDB = false;
					QString srcType = QString();
					
					sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
					if (srcType == PARAM_TYPE_PIR)
						sParamInfo.isPIR = true;
					
					sParamInfo.isParamLocal = checkIsLocal(param);
					sParamInfo.isParamReleased = false;
					sParamInfo.isParamUsed = true;
					if (isInDB)
					{
						sParamInfo.isParamVmac = true;
						sParamInfo.isParamPIR = checkIsPIR(param);
					}
					sParamInfo.isValCorrect = false;
					sParamInfo.isValApplicable = false;
					sParamInfo.isFirst = false;
					sParamInfo.val = "";
					sParamInfo.paramName = param;
					sParamInfo.isConstant = checkIsConstant(param, paramType);
					sParamInfo.valueType = paramType;
					sParamInfo.isEquation = false;

					bool isValidUnSubs = true;
					// Avoid to subscribe a local variable
					if (sParamInfo.isParamInDb)
						isValidUnSubs = checkIsUnsubscribeValid(param);
					sParamInfo.hasValidUnsubscribe = isValidUnSubs;

					if (!isValidUnSubs)
						sParamInfo.invalidUnSubscribeParam.append(param);
					else
					{
						_ParamSubscribed.removeOne(param);
						if (param.startsWith("line") && param.contains("__"))
						{
							QString l_colName = param.split("__").last();
							foreach(QString elm, _ColPirName)
							{
								if (elm.startsWith(l_colName + "_PIKACHU_"))
								{
									_ColPirName.removeOne(elm);
									if (_ParamSubscribed.contains(elm.replace(l_colName + "_PIKACHU_", QString())))
										_ParamSubscribed.removeOne(elm.replace(l_colName + "_PIKACHU_", QString()));
								}
							}
						}
					}
					QStringList invalidChars;
					bool containsInvalidChars = checkInvalidChars(param, invalidChars);
					sParamInfo.containsInvalidChars = containsInvalidChars;
					sParamInfo.invalidChars = invalidChars;
					sCmdInfo.paramList.append(sParamInfo);

					if (sParamInfo.isParamVmac)
					{
						VmacParamInfo vmacInfo;
						vmacInfo.paramInfo = sParamInfo;
						vmacInfo.pCmd = ipCmd;
						vmacInfo.rowIdx = iRowIdx;
						vmacInfo.cmdType = actType;
						vmacInfo.isCall = isCall;
						vmacInfo.CallName = iCallElemName;
						_VmacParamInfo.append(vmacInfo);
					}
					if (sParamInfo.isParamInDb)
						_CommandValidityInfo.append(sCmdInfo); 
				}
			}
		}
		else
		{
			//other commands which are single dimensional like print and manual action
			QStringList paramList = pActCmd->getVariableList();
			foreach(QString param, paramList)
			{
				param = param.trimmed();
				CmdInfo sCmdInfo;
				sCmdInfo.cmdType = actType;
				sCmdInfo.pCmd = ipCmd;
				sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;
				sCmdInfo.rowIdx = iRowIdx;

				ParamInfo sParamInfo;
				QString paramType = QString();
				bool isInDB = false;
				QString srcType = QString();
				
				sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
				if (srcType == PARAM_TYPE_PIR)
					sParamInfo.isPIR = true;
				else
					sParamInfo.isPIR = false;
				
				sParamInfo.isTagVar = param.contains("@");
				sParamInfo.isTagVarValid = isValidTagVar(param);
				sParamInfo.isParamLocal = checkIsLocal(param);
				sParamInfo.isParamReleased = false;
				sParamInfo.isParamUsed = false;
				if (isInDB)
					sParamInfo.isParamVmac = true;
				sParamInfo.isValCorrect = false;
				sParamInfo.isValApplicable = false;
				sParamInfo.isFirst = false;
				sParamInfo.val = "";
				sParamInfo.paramName = param;
				sParamInfo.isConstant = checkIsConstant(param, paramType);
				sParamInfo.valueType = paramType;
				sParamInfo.isEquation = false;

				QStringList invalidChars;
				bool containsInvalidChars = checkInvalidChars(param, invalidChars);
				sParamInfo.containsInvalidChars = containsInvalidChars;
				sParamInfo.invalidChars = invalidChars;

				if (checkIsParamInternal(param))
				{
					sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
				}

				sCmdInfo.paramList.append(sParamInfo);

				if (sParamInfo.isParamVmac)
				{
					VmacParamInfo vmacInfo;
					vmacInfo.paramInfo = sParamInfo;
					vmacInfo.pCmd = ipCmd;
					vmacInfo.rowIdx = iRowIdx;
					vmacInfo.cmdType = actType;
					vmacInfo.isCall = isCall;
					vmacInfo.CallName = iCallElemName;
					_VmacParamInfo.append(vmacInfo);
				}
				_CommandValidityInfo.append(sCmdInfo);
			}
			if (actType == ACT_MANUAL)
			{
				GTAActionManual *pManAct = dynamic_cast<GTAActionManual*>(ipCmd);
				if (pManAct != NULL)
				{
					QString param = pManAct->getParameter().trimmed();
					QString paramType = pManAct->getReturnType();
					if (paramType == "int")
						paramType = "Integer";
					else if (paramType == "float")
						paramType = "Float";
					else if (paramType == "bool")
						paramType = "Boolean";
					else if (paramType == "string")
						paramType = "String";

					_LocalParamDetail.insert(param, paramType);
				}
			}
		}
	}
	else if (cmdType == GTACommandBase::CHECK)
	{
		GTACheckBase * pChkCmd = dynamic_cast<GTACheckBase *>(ipCmd);
		GTACheckBase::CheckType chkType = pChkCmd->getCheckType();
		
		if (chkType == GTACheckBase::VALUE)
		{
			GTACheckValue *pChk = dynamic_cast<GTACheckValue *>(pChkCmd);
			int chkParamCnt = pChk->getCount();
			CmdInfo sCmdInfo;
			sCmdInfo.cmdType = "CHECK VALUE";
			sCmdInfo.subCmdType = GTACheckCompatibility::PARAMETER_TYPE;
			sCmdInfo.pCmd = ipCmd;
			sCmdInfo.rowIdx = iRowIdx;
			
			QString param_cmd;
			QString val_cmd;

			for (int i = 0; i<chkParamCnt; i++)
			{
				QString paramType;
				QString param;
				QString val;
				param = pChk->getParameter(i).trimmed();
				param_cmd = param.contains("__") ? param.split("__").last() : param_cmd;
				val = pChk->getValue(i).trimmed();
				val_cmd = val.contains("__") ? val.split("__").last() : val_cmd;
				bool checkForEach = false;

				if (param.contains("line."))
				{
					checkForEach = true;
				}

				// FOR CHECK in LOOP
				if (!_ForColHeader.isEmpty() && checkForEach)
				{
					fillCheckCSVParamList(ipCmd, pChk, i, sCmdInfo, iRowIdx, isCall, iCallElemName);
				}
				else
				{
					addToCheckParamList(ipCmd, pChk, i, sCmdInfo, param, pChk->getValue(i).trimmed(),
						iRowIdx, isCall, iCallElemName);
				}
			}
			_CommandValidityInfo.append(sCmdInfo);

			if (!_ForColHeader.isEmpty())
			{
				GTAActionForEach* pCond = dynamic_cast<GTAActionForEach*>(ipCmd->getParent());

				if (pCond != nullptr)
				{
					// Open and read the CSV file
					_nbColumn = SET_CLASSIQUE;

					QString FileToParse;
					if (pCond->getRelativePath().isEmpty())
					{
						FileToParse = pCond->getPath();
					}
					else
					{
						QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
						FileToParse = repoPath + pCond->getRelativePath();
					}

					QString delim = pCond->getDelimiterValue();
					QFile fileObj(FileToParse);

					if (fileObj.open(QFile::ReadOnly | QFile::Text))
					{
						QStringList paramListFromCsv;
						QString line = fileObj.readLine().simplified();
						line.replace(",", delim);
						paramListFromCsv = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

						bool foundParam = paramListFromCsv.contains(param_cmd);
						bool foundVal = paramListFromCsv.contains(val_cmd);

						// Check if either column is missing or mismatched
						if ((foundParam != foundVal) || (!foundParam && !foundVal))
						{
							QString desc = "The name of one of the columns has been changed, which may cause the Check action to fail. Please update the procedure accordingly";
							QString lineNum = QString::number(iRowIdx);
							ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kWarning, ErrorMessage::MsgSource::kCheckCompatibility, iCallElemName, lineNum, uuid);
							oErrorList.append(error);
						}
					}
				}
			}


		}
	}
	return rc;
}

/* This function add the individual set action param in the paramlist for each csv file row
* @input : ipCmd
* @input : pSet - set action command object
* @input : emptyCases
* @input : sCmdInfo
* @input : param - param name
* @input : value - Value associated with param
* @input : iRowIdx - Index of the Row
* @input : isCall
* @input : iCallElemName
*/
void GTACheckCompatibility::addToSetParamList(GTACommandBase*& ipCmd, GTAActionSet* pSet, QStringList& emptyCases,
	CmdInfo& sCmdInfo, QString& param, QString& value, const int iRowIdx, bool isCall, const QString& iCallElemName)
{
	// input : EEC_CFM1_`CHA:CIC`.`EEC1_A:CIC`2.HPSOV_Closed_270_14
	if (param.contains(":CIC") || param.contains(":NCIC"))
	{
		param.replace("'", "");
		param.replace("`", "");
		param.replace(":NCIC", "");
		param.replace(":CIC", "");
	}

	QString paramType;
	ParamInfo sParamInfo;
	ParamInfo sValInfo;
	QStringList invalidChars;
	bool containsInvalidChars = checkInvalidChars(param, invalidChars);
	QString srcType = QString();
	bool isInDB = false;

	sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
	if (srcType == PARAM_TYPE_PIR)
	{
		sParamInfo.isPIR = true;
		sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
	}

	sParamInfo.isParamLocal = checkIsLocal(param);
	sParamInfo.containsInvalidChars = containsInvalidChars;
	sParamInfo.invalidChars = invalidChars;
	sParamInfo.isParamReleased = false;
	sParamInfo.isParamUsed = true;

	if (isInDB)
	{
		sParamInfo.isParamVmac = true;
	}

	sParamInfo.isValCorrect = checkValidVal(param, value.trimmed(), isInDB);
	sParamInfo.isValApplicable = true;
	sParamInfo.isTagVar = value.contains("@");
	sParamInfo.isTagVarValid = isValidTagVar(value);
	sParamInfo.isFirst = false;
	sParamInfo.val = value;
	sParamInfo.paramName = param;
	sParamInfo.isConstant = checkIsConstant(param, paramType);
	sParamInfo.emptyCSVCases = emptyCases;

	addValueAndNextTypeParamInfo(sParamInfo, value, paramType);

	sParamInfo.isInRange = checkForRange(param, value, sParamInfo.rangeMinVal, sParamInfo.rangeMaxVal);
	sParamInfo.isParamDirectionCorrect = checkParamDirection(ACT_SET, param);
	if (checkIsParamInternal(param))
	{
		sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
	}

	bool isSetOnlyFSFixed = pSet->getIsSetOnlyFSFixed();
	bool isSetOnlyFSVariable = pSet->getIsSetOnlyFSVariable();
	GTAEquationBase* eq = pSet->getEquation();
	if ((eq != NULL) && (isSetOnlyFSFixed == false) && (isSetOnlyFSVariable == false))
	{
		if (eq->getEquationType() == GTAEquationBase::CONST)
		{
			bool isInDB = false;
			sParamInfo.isEquation = false;
			srcType = QString();
			sValInfo.isParamInDb = checkIsInDb(value, isInDB, srcType);
			if (srcType == PARAM_TYPE_PIR)
			{
				sValInfo.isPIR = true;
				sValInfo.isParamSubscribed = checkIsParamSubscribed(value);
			}

			sValInfo.isParamLocal = checkIsLocal(value);
			sValInfo.isParamReleased = false;
			sValInfo.isParamUsed = true;

			if (isInDB)
			{
				sValInfo.isParamVmac = true;
			}

			sValInfo.isValCorrect = false;
			sValInfo.isValApplicable = false;
			sValInfo.isFirst = false;
			sValInfo.paramName = value;
			sValInfo.val = "";
			sValInfo.isConstant = checkIsConstant(value.trimmed(), paramType);
			sValInfo.isParamDirectionCorrect = checkParamDirection(ACT_SET, value);

			if (checkIsParamInternal(value))
			{
				sValInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, value);
			}

			sValInfo.valueType = paramType;
			invalidChars.clear();
			containsInvalidChars = checkInvalidChars(value, invalidChars);
			sValInfo.containsInvalidChars = containsInvalidChars;
			sValInfo.invalidChars = invalidChars;
			sCmdInfo.paramList.append(sValInfo);

			if (sValInfo.isParamVmac)
			{
				addVmacParamInfo(ipCmd, sValInfo, iRowIdx, isCall, iCallElemName, ACT_SET);
			}
		}
		else
		{
			sParamInfo.isEquation = true;
			//This has been added to take care of inputs in a liquid ramp.
			//Text = "LGCIU_1.LGCIU_1_A429_BUS.BNGPRESD_LGCIU1_1_SG+60", contains some invalid Charcters( + )
			sValInfo.containsInvalidChars = false;
			invalidChars.clear();
			sValInfo.invalidChars = invalidChars;
			sValInfo.isParamLocal = false;
		}
	}
	else if (isSetOnlyFSFixed == true || isSetOnlyFSVariable == true)
	{
		sParamInfo.isSetCheckFS = true;
		sParamInfo.isValApplicable = false;
		// if the Media is AFDX, CAN and A429 and Src Type is ICD, then FS is applicable to them
		QString MediaType = getParamMediaType(param);
		QString srcType = getParamSrcType(param);
		if ((srcType == "ICD") && ((MediaType == "AFDX") || (MediaType == "CAN") || (MediaType == "A429")))
		{
			sParamInfo.isValidFSType = true;
		}
		else
		{
			sParamInfo.isValidFSType = false;
		}
	}

	sCmdInfo.paramList.append(sParamInfo);

	if (sParamInfo.isParamVmac)
	{
		addVmacParamInfo(ipCmd, sParamInfo, iRowIdx, isCall, iCallElemName, ACT_SET);
	}
}

/* This function fills the paramlist for each row in the csv file and then manage its content
* @input : ipCmd
* @input : pChk
* @input : checkParamCount - index of check param
* @input : sCmdInfo
* @input : iRowIdx - Index of the Row
* @input : isCall
* @input : iCallElemName
*/
void GTACheckCompatibility::fillCheckCSVParamList(GTACommandBase*& ipCmd, GTACheckValue* pChk, const int checkParamCount,
	CmdInfo& sCmdInfo, const int iRowIdx, bool isCall, const QString& iCallElemName)
{
	GTAActionForEach* pCond = dynamic_cast<GTAActionForEach*>(ipCmd->getParent());

	if (pCond != nullptr)
	{
		GTAAppController* pController = GTAAppController::getGTAAppController();
		QString dbFile = pController->getSystemService()->getDatabaseFile();
		GTADataController dataCtrlr;

		QStringList invalidParams;
		QStringList emptyCases;

		QString param = pChk->getParameter(checkParamCount).trimmed();
		QString paramType = QString();

		// Contains the state of action (ENUM NbColumn) in *.h
		_nbColumn = SET_CLASSIQUE;

		QString fileToParse = QString();
		if (pCond->getRelativePath().isEmpty())
		{
			fileToParse = pCond->getPath();
		}
		else
		{
			const QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
			fileToParse = repoPath + pCond->getRelativePath();
		}

		QFile fileObj(fileToParse);
		bool rc = fileObj.open(QFile::ReadOnly | QFile::Text);
		int fsColumnIndex = -1;

		// Get the columns name to be read to perform the ForEach command
		QString fColumnName = pChk->getParameter(checkParamCount);
		if (fColumnName.contains("__"))
		{
			if (fColumnName.split("__").last() != _fColumnName)
			{
				_fColumnName = fColumnName.split("__").last();
			}

			_nbColumn = WRITE_BACK;
		}

		QString sColumnName = pChk->getValue(checkParamCount);
		if (sColumnName.contains("__"))
		{
			if (sColumnName.split("__").last() != _sColumnName)
			{ 
				_sColumnName = sColumnName.split("__").last();
			}

			if (!sColumnName.startsWith("Result", Qt::CaseInsensitive))
			{
				if (_nbColumn == 3)
				{
					_nbColumn = NEW_FOREACH;
				}
				else if (_nbColumn == 0)
				{
					_nbColumn = OLD_FOREACH;
				}
			}
		}

		int lineCount = 0;
		QStringList lines;
		QString delim = pCond->getDelimiterValue();
		QHash<int, QString> tripletsList;
		QHash<int, QString> valuesList;
		if (rc)
		{
			fillTripletAndValueFromCSV(fileObj, lines, delim, tripletsList, valuesList, emptyCases,
				pChk, checkParamCount);
		}

		// Separate loop to check empty values in the FS column
		lineCount = 1;  // Start at 1 since line 0 is headers
		for (const QString& line : lines) {
			QStringList paramListFromCsv = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

			if (fsColumnIndex != -1 && fsColumnIndex < paramListFromCsv.size()) {
				QString value = paramListFromCsv.at(fsColumnIndex).trimmed();
				if (value.isEmpty()) {
					emptyCases.append(QString::number(lineCount + 1) + ";" + columnIndexToExcelColumn(fsColumnIndex));
				}
			}
			lineCount++;
		}

		// Fill in the QHash according to the number of chosen columns
		if (_nbColumn == 1)
		{
			param = pChk->getParameter(checkParamCount).trimmed();
			// Update _ParamList with Parameters from the CHECK command
			dataCtrlr.getParametersDetail(QStringList(param), dbFile, _ParamList, invalidParams);
		}
		else
		{
			param = pChk->getParameter(checkParamCount).trimmed();
			if (!tripletsList.values().isEmpty())
			{
				// Update _ParamList with Parameters from CSV File
				dataCtrlr.getParametersDetail(tripletsList.values(), dbFile, _ParamList, invalidParams);
			}
			else
			{
				// Update _ParamList with Parameters from the CHECK command
				dataCtrlr.getParametersDetail(QStringList(param), dbFile, _ParamList, invalidParams);
			}
		}

		QString value = QString();
		param = pChk->getParameter(checkParamCount).trimmed();

		if (!param.contains("line."))
		{
			value = pChk->getValue(checkParamCount).trimmed();

			addToCheckParamList(ipCmd, pChk, checkParamCount, sCmdInfo, param, value,
				iRowIdx, isCall, iCallElemName);
		}
		else // val contains ("line.")
		{
			if (valuesList.isEmpty())
			{
				for (int j = 0; j < tripletsList.count(); j++)
				{
						param = tripletsList.value(j);
						value = pChk->getValue(checkParamCount).trimmed();
						addToCheckParamList(ipCmd, pChk, checkParamCount, sCmdInfo, param, value,
							iRowIdx, isCall, iCallElemName);
				}
			}
			else
			{
				for (int j = 0; j < valuesList.count(); j++)
				{
					// CSV file has only one column (Value)
					if (_nbColumn == 1)
					{
						param = pChk->getParameter(checkParamCount).trimmed();
					}
					// CSV file has two columns (Param and Value)
					else
					{
						param = tripletsList.value(j);
					}

					value = valuesList.value(j);

					addToCheckParamList(ipCmd, pChk, checkParamCount, sCmdInfo, param, value,
						iRowIdx, isCall, iCallElemName);
				}
			}
		}
	}
}

/* This function fills the triplet and value list from the csv file
* @input : fileObj - csv file object
* @input : lines - lines read from csv
* @input : delim 
* @input : tripletsList
* @input : valuesList
* @input : emptyCases
* @input : pChk
* @input : checkParamCount
*/
void GTACheckCompatibility::fillTripletAndValueFromCSV(QFile& fileObj, QStringList& lines, const QString& delim, QHash<int, QString>& tripletsList,
	QHash<int, QString>& valuesList, QStringList& emptyCases, GTACheckValue* pChk, const int checkParamCount)
{
	int fColumnIndex = -1;
	int sColumnIndex = -1;
	int lineCount = 0;
	QStringList paramListFromCsv;
	QString line = QString();

	while (!fileObj.atEnd())
	{
		/* _nbColumn
		*    0  :  SET_CLASSIQUE
		*	 1  :  param && line.value
		*	 2  :  line.param && line.value
		*	 3  :  line.param && value (Exchange param/value)
		*/

		line = fileObj.readLine().simplified();
		lines.append(line);
		line.replace(",", delim);
		if (line.contains(delim) && fColumnIndex >= 0 && sColumnIndex >= 0 &&
			pChk->getParameter(checkParamCount).contains("__") && pChk->getValue(checkParamCount).contains("__"))
		{
			_nbColumn = NEW_FOREACH;
		}
		paramListFromCsv = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");

		if (!line.isEmpty())
		{
			if (lineCount > 0)
			{
				if (_nbColumn == 1)
				{
					QString valueExtracted = paramListFromCsv.at(sColumnIndex).trimmed();
					valuesList.insert(lineCount - 1, valueExtracted);
					if (valueExtracted.isEmpty())
					{
						emptyCases.append(QString::number(lineCount + 1) + ";" + columnIndexToExcelColumn(sColumnIndex));
					}
				}
				if (_nbColumn == 2)
				{

					if (fColumnIndex >= 0 && sColumnIndex >= 0)
					{
						QString tripletExtracted = paramListFromCsv.at(fColumnIndex).trimmed();
						QString valueExtracted = paramListFromCsv.at(sColumnIndex).trimmed();

						tripletsList.insert(lineCount - 1, tripletExtracted);
						valuesList.insert(lineCount - 1, valueExtracted);

						if (tripletExtracted.isEmpty())
						{
							emptyCases.append(QString::number(lineCount + 1) + ";" + columnIndexToExcelColumn(fColumnIndex));
						}

						if (valueExtracted.isEmpty())
						{
							emptyCases.append(QString::number(lineCount + 1) + ";" + columnIndexToExcelColumn(sColumnIndex));
						}

					}
				}
				if (_nbColumn == 3)
				{
					QString tripletExtracted = paramListFromCsv.at(fColumnIndex).trimmed();
					tripletsList.insert(lineCount - 1, tripletExtracted);
					if (tripletExtracted.isEmpty())
					{
						emptyCases.append(QString::number(lineCount + 1) + ";" + columnIndexToExcelColumn(fColumnIndex));
					}
				}
			}
			else
			{
				if (_nbColumn >= 2) // Contains both the NEW_FOREACH and the WRITE_BACK action
				{
					// Research of the index of each column name int the ParamListFromCsv QStringList
					for (int i = 0; i < paramListFromCsv.size(); i++)
					{
						if (QString::compare(paramListFromCsv[i], _fColumnName) == 0)
						{
							fColumnIndex = i;
							break;
						}
					}
				}
				for (int j = 0; j < paramListFromCsv.size(); j++)
				{
					if (QString::compare(paramListFromCsv[j], _sColumnName) == 0)
					{
						sColumnIndex = j;
						break;
					}
				}
			}
		}
		lineCount++;
		paramListFromCsv.clear();
	}
	fileObj.close();
}

/* This function add the individual check action param in the paramlist for each csv file row
* @input : ipCmd
* @input : pChk
* @input : checkParamCount - index of check param
* @input : sCmdInfo
* @input : param - param name
* @input : value - Value associated with param
* @input : iRowIdx - Index of the Row
* @input : isCall
* @input : iCallElemName
*/
void GTACheckCompatibility::addToCheckParamList(GTACommandBase*& ipCmd, GTACheckValue* pChk, const int checkParamCount,
	CmdInfo& sCmdInfo, QString& param, QString& value, const int iRowIdx, bool isCall, const QString& iCallElemName)
{
	QString paramType = QString();

	// input : EEC_CFM1_`CHA:CIC`.`EEC1_A:CIC`2.HPSOV_Closed_270_14
	if (param.contains(":CIC") || param.contains(":NCIC"))
	{
		param.replace("'", "");
		param.replace("`", "");
		param.replace(":NCIC", "");
		param.replace(":CIC", "");
	}
	// output : EEC_CFM1_CHA.EEC1_A2.HPSOV_Closed_270_14

	const bool isSetOnlyFS = pChk->getISCheckFS(checkParamCount);
	const bool isSetOnlyValue = pChk->getISOnlyCheckValue(checkParamCount);
	ParamInfo sParamInfo;
	bool isInDB = false;
	QString srcType = QString();

	if (true == checkIsEngineParameter(param))
	{
		QStringList invalidParams;
		sParamInfo.isEngineParamInDb = checkForChannelInControl(param, invalidParams);
		sParamInfo.invalidEngineParams = invalidParams;
	}
	else
	{
		sParamInfo.isParamInDb = checkIsInDb(param, isInDB, srcType);
	}

	if (srcType == PARAM_TYPE_PIR)
	{
		sParamInfo.isPIR = true;
		sParamInfo.isParamSubscribed = checkIsParamSubscribed(param);
	}

	sParamInfo.isParamLocal = checkIsLocal(param);
	sParamInfo.isParamReleased = false;
	sParamInfo.isParamUsed = false;

	if (isInDB)
	{
		sParamInfo.isParamVmac = true;
	}
		
	sParamInfo.isValCorrect = checkValidVal(param, value.trimmed(), isInDB);
	sParamInfo.isValApplicable = true;
	sParamInfo.isTagVar = value.contains("@");
	sParamInfo.isTagVarValid = isValidTagVar(value);
	sParamInfo.isFirst = false;
	sParamInfo.val = value;
	sParamInfo.paramName = param;
	sParamInfo.isConstant = checkIsConstant(param, paramType);
	sParamInfo.valueType = paramType;
	sParamInfo.isEquation = false;
	sParamInfo.isInRange = checkForRange(param, value, sParamInfo.rangeMinVal, sParamInfo.rangeMaxVal);
	sParamInfo.isParamDirectionCorrect = checkParamDirection("CHECK VALUE", param);

	if (checkIsParamInternal(param))
	{
		sParamInfo.isInternalParamValid = checkIsInternalParamValid(ipCmd, param);
	}

	if (isInDB)
	{
		sParamInfo.isSetCheckFS = true;
		sParamInfo.isValApplicable = false;
		setValidFSTypeInParamInfo(sParamInfo, param, isSetOnlyFS, isSetOnlyValue);
	}
	else
	{
		// Parameter is not in DB
		sParamInfo.isSetCheckFS = false;
		sParamInfo.isValApplicable = false;
		sParamInfo.isValidFSType = false;
	}

	if (param.startsWith("line"))
	{
		sParamInfo.isSetCheckFS = true;
		sParamInfo.isValApplicable = false;
		sParamInfo.isValidFSType = true;
	}

	QStringList invalidChars;
	bool containsInvalidChars = checkInvalidChars(param, invalidChars);
	sParamInfo.containsInvalidChars = containsInvalidChars;
	sParamInfo.invalidChars = invalidChars;

	sCmdInfo.paramList.append(sParamInfo);

	if (sParamInfo.isParamVmac)
	{
		addVmacParamInfo(ipCmd, sParamInfo, iRowIdx, isCall, iCallElemName, "CHECK VALUE");
	}
}

/* This function adds the param into vmacParamInfo List
* @input : ipCmd
* @input : sParamInfo
* @input : iRowIdx
* @input : isCall
* @input : iCallElemName
* @input : cmdType
*/
void GTACheckCompatibility::addVmacParamInfo(GTACommandBase*& ipCmd, ParamInfo& sParamInfo, const int iRowIdx, bool isCall,
	const QString& iCallElemName, const QString& cmdType)
{
	VmacParamInfo vmacInfo;
	vmacInfo.paramInfo = sParamInfo;
	vmacInfo.pCmd = ipCmd;
	vmacInfo.rowIdx = iRowIdx;
	vmacInfo.isCall = isCall;
	vmacInfo.CallName = iCallElemName;
	vmacInfo.cmdType = cmdType;
	_VmacParamInfo.append(vmacInfo);
}

/* This function set the validFS Type in param info
* @input : sParamInfo
* @input : param
* @input : isSetOnlyFS
* @input : isSetOnlyValue
*/
void GTACheckCompatibility::setValidFSTypeInParamInfo(ParamInfo& sParamInfo, QString& param,
	const bool& isSetOnlyFS, const bool& isSetOnlyValue)
{
	QString MediaType = getParamMediaType(param);
	QString srcType = getParamSrcType(param);

	if (isSetOnlyFS == true)
	{
		// if the Media is AFDX, CAN and A429 and Src Type is ICD, then FS is applicable to them
		if (srcType == "ICD" && (MediaType == "AFDX" || MediaType == "CAN" || MediaType == "A429"))
		{
			sParamInfo.isValidFSType = true;
		}
		else
		{
			sParamInfo.isValidFSType = false;
		}
	}

	if (isSetOnlyValue == true)
	{
		if (MediaType != "DIS" || MediaType != "ANA" || MediaType != "VMAC" || MediaType != "PIR" || MediaType != " MODEL")
		{
			sParamInfo.isValidFSType = true;
		}
		else
		{
			sParamInfo.isValidFSType = false;
		}
	}

	if (isSetOnlyFS == false && isSetOnlyValue == false)
	{
		if (srcType == "ICD" && (MediaType == "AFDX" || MediaType == "CAN" || MediaType == "A429") &&
			(MediaType != "DIS" || MediaType != "ANA" || MediaType != "VMAC" || MediaType != "PIR" || MediaType != " MODEL"))
		{
			sParamInfo.isValidFSType = true;
		}
		else
		{
			sParamInfo.isValidFSType = false;
		}
	}
}

/* This function set the ValueType And NextType in ParamInfo
* @input : sParamInfo
* @input : value
* @input : paramType
*/
void GTACheckCompatibility::addValueAndNextTypeParamInfo(ParamInfo& sParamInfo, QString& value, QString& paramType)
{
	QString l_valType = QString();
	bool l_isGetValue = false;

	// Specific case for External Tool (Get) Function
	if (value.startsWith("Result", Qt::CaseInsensitive))
	{
		// We have Result.zone_uuidXxxxX__clickOnXY
		QList<CommandInternalsReturnInfo> retInfoList;
		GTAAppController* pController = GTAAppController::getGTAAppController();
		pController->getCommandList(retInfoList, "External_Tool");

		if (!retInfoList.isEmpty() && value.contains("__"))
		{
			QString l_retNameFirst = value.split("__").first();
			QString retName = value.split("__").first();

			if (l_retNameFirst.contains("_"))
			{
				// We have to get the RetName (Result.XxX)
				retName = l_retNameFirst.split("_").first();
			}

			foreach(CommandInternalsReturnInfo elm, retInfoList)
			{
				// These information come from External Tool Configuration File
				if (elm.retName == retName)
				{
					l_valType = elm.retType;
					l_isGetValue = true;
				}
			}
		}
	}

	// We make the difference between classic assignment and getting value from ExternalTool
	if (l_isGetValue)
	{
		sParamInfo.valueType = l_valType;
		sParamInfo.nextType = sParamInfo.valueType;
	}
	else
	{
		sParamInfo.valueType = paramType;
		sParamInfo.nextType = paramType;
	}
}

bool GTACheckCompatibility::checkIsPortingRequired(QList<ErrorMessage> &oMsgList)
{
	bool rc = false;
	if (!_UUIDElementHash.isEmpty())
	{
		QStringList keys = _UUIDElementHash.uniqueKeys();
		foreach(QString key, keys)
		{
			QList<GTAActionCall*> values = _UUIDElementHash.values(key);
			GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
			QString GTADirPath = pAppCtrl->getGTADataDirectory();
			QStringList relFilePathLst = pAppCtrl->getAllRelFilePathsFromUUID(key);
			QStringList legitFiles;
			if (pAppCtrl != NULL)
			{
				for (int i = 0; i < values.count(); i++)
				{
					GTAActionCall *pCall = values.at(i);
					QString elemName = pCall->getElement();
					QString elemnameOnly = elemName.split("/").last();

					int fileCount = relFilePathLst.filter(elemnameOnly).count();
					
					if (fileCount == 1)
					{
						if (!legitFiles.contains(elemName))
						{
							QString absFilePath = QDir::cleanPath(QString("%1/%2").arg(GTADirPath, elemName));
							GTADocumentController docCtrl;
							QString nameFromFile = docCtrl.getDocumentName(absFilePath);
							if (!nameFromFile.isEmpty())
							{
								QString name = elemName.split("/").last();
								name = name.mid(0, name.lastIndexOf(".")).trimmed();
								if (nameFromFile == name)
								{
									legitFiles.append(elemName);
								}
								else
								{
									pCall->setCmdValidaityStatus(false);
									LogMessageInfo msgInfo = _CallCmdHash.value(pCall);
									ErrorMessage error = createErrorMsg("Call is referring to an element whose UUID is not unique. Perform porting to resolve the issue", ErrorMessage::ErrorType::kWarning, ErrorMessage::MsgSource::kCheckCompatibility, msgInfo.fileName, msgInfo.lineNumber, msgInfo.uuid);
									oMsgList.append(error);
									rc |= true;
								}
							}
						}
					}
					else
					{
						pCall->setCmdValidaityStatus(false);
						LogMessageInfo msgInfo = _CallCmdHash.value(pCall);
						ErrorMessage error = createErrorMsg("Call is referring to an element whose UUID is not available. Perform porting to resolve the issue", ErrorMessage::ErrorType::kWarning, ErrorMessage::MsgSource::kCheckCompatibility, msgInfo.fileName, msgInfo.lineNumber, msgInfo.uuid);
						oMsgList.append(error);
						rc |= true;
					}
				}
			}
		}
	}
	return rc;
}


/**
* This function checks the editor content
* for valid details.
* Report is generated and will be displayed by the HMI window.
* Details Checked
* - Parameter used, exists in Database.
* this function calls the private function "checkCommandCompatibility"
*/
ErrorMessageList GTACheckCompatibility::CheckCompatibility(const QString &iDataDirectory, const QString &iDbFile, GTAElement *pElem, bool bPopulateVmacForRelease)
{
	bool rc = false;
	ErrorMessageList Errors;
	QList<ErrorMessage> ErrorList;

	bool globalRc = true;
	bool isVmacIssue = false;
	QString elemRelativePath;

	if (pElem != NULL)
	{
		GTADataController dataCtrlr;
		GTADocumentController doCtrl;
		QStringList chkLstForRecursion;
		QString elemName = pElem->getName();
		elemRelativePath = pElem->getRelativeFilePath();
		QString uuid = pElem->getUuid();
		QStringList paramList = doCtrl.getAllVariableList(pElem, chkLstForRecursion, bPopulateVmacForRelease);

		QStringList paramItems;
		QList<QStringList> complexQuery;
		paramList.removeDuplicates();
		foreach(QString paramName, paramList)
		{
			//failsafe to check query doesn't fail
			paramName.replace("'", "");
			// Modify the query to specify the case insensitivity 
			paramItems.append(" NAME COLLATE NOCASE = '" + paramName + "' ");
		}
		int cntr = 0;
		QStringList tempItems;
		foreach(QString item, paramItems)
		{
			tempItems.append(item);
			if (cntr == 200)
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
			QString whereClause = items.join(" OR ");
			QString query = QString("SELECT * FROM %1 WHERE %2;").arg(PARAMETER_TABLE, whereClause);//
			rc = dataCtrlr.fillQuery(iDbFile, query, _ParamList);
		}

		//clearing the Internal param UUID list
		_CommandUUIDs.clear();
		QStringList checkForRep;
		checkForRep.clear();
		rc = checkCommandCompatibility(iDataDirectory, pElem, ErrorList, false, "", "", checkForRep, QHash<QString, GTAElemRecStruct>(), *(new CompatibilityCallFuncErrors(NO_ERROR)));

		for (int i = 0; i<_VmacParamInfo.count(); i++)
		{
			VmacParamInfo vmacInfo = _VmacParamInfo.at(i);
			GTACommandBase *pVmacCmd = vmacInfo.pCmd;
			int rowIdx = vmacInfo.rowIdx;
			ParamInfo paramInfo = vmacInfo.paramInfo;
			QString subCallName;
			QString msg;
			if (paramInfo.isParamUsed == false && paramInfo.isParamReleased == true)
			{
				globalRc = false;
				isVmacIssue = true;
				pVmacCmd->setCmdValidaityStatus(false);
				QString desc, lineNum;
				msg = QString("Row(%2):  Released Parameter \"%1\" is released without being set before ").arg(paramInfo.paramName, QString::number(rowIdx));
				desc = QString("Released Parameter Name ('%1') is released without being set before").arg(paramInfo.paramName);
				lineNum = QString::number(rowIdx);
				ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kWarning, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
				ErrorList.append(error);
				pVmacCmd->setErrorLogList(msg);
			}
			if (globalRc == true)
			{
				pVmacCmd->setCmdValidaityStatus(true);
			}
		}
	}

	checkIsPortingRequired(ErrorList);

	if (!_TagVariable.isEmpty())
	{
		QStringList parameters = _TagVariable.keys();
		QList<LogMessageInfo> infoList = _TagVariable.values();
		for (int i = 0; i<_TagVariable.count(); i++)
		{

			QString param = parameters.at(i);
			if (!param.trimmed().isEmpty())
			{
				LogMessageInfo info = infoList.at(i);
				QString desc = QString("Verify tagged parameter ('%1') manually").arg(param.trimmed());

				ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kInfo, ErrorMessage::MsgSource::kCheckCompatibility, info.fileName, info.lineNumber, info.uuid);
				ErrorList.append(error);
			}
		}
	}
	/* This sequence allows to remove PIR parameter name of the _ParamSubscribed
	 * We make the difference between the parameter starting with "line." and elements units contained in "line." parameter
	 */
	if (!_ColPirName.isEmpty())
	{
		foreach(QString elm, _ColPirName)
		{
			QString l_colName = elm.split("_").first(); 
			if (elm.startsWith(l_colName + "_PIKACHU_"))
			{
				if (_ParamSubscribed.contains(elm.replace(l_colName + "_PIKACHU_", QString())))
					_ParamSubscribed.removeOne(elm.replace(l_colName + "_PIKACHU_", QString()));
			}
		}
	}
	if (!_ParamSubscribed.isEmpty())
	{		
		for (int i = 0; i < _ParamSubscribed.count(); i++)
		{
			QString param = _ParamSubscribed.at(i);
			QString desc = QString("Parameter ('%1') has not been released").arg(param.trimmed());
			ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, QString(), QString(), QString());
			ErrorList.append(error);
		}
	}

	Errors = ErrorList;

	// Allow to reset all struture and also the static int variable
	resetCheckComp();

	return Errors;
}



bool GTACheckCompatibility::checkCommandCompatibility(const QString &iDataDirectory, GTAElement *& ipElem,
	QList<ErrorMessage> &oErrorList, bool isCall, const QString&, const QString & iCallElemName,
	QStringList &icheckForRep,
	const QHash<QString, GTAElemRecStruct>& hshReferncedItem,
	CompatibilityCallFuncErrors& isRecuriosn)	
{
	bool rc = false;
	bool globalRc = true;
	QStringList invalidParams;
	QString nType = "";
	int childCount = ipElem->getAllChildrenCount();
	QStringList l_mathFunctions;
	l_mathFunctions << EQ_CONST << EQ_GAIN << EQ_RAMP << EQ_TRAPEZE << EQ_CRENELS << EQ_SINECRV <<
		EQ_TRIANGLE << EQ_SAWTOOTH << EQ_SINUS << EQ_SQUARE << EQ_STEP << EQ_IDLE <<
		EQ_EXP << EQ_NOISE << EQ_STOP_TRIGGER << FEQ_CONST << FEQ_RAMPSLOPE;
	//collecting all param UUIDs to check whether internal variable is correct
	for (int i = 0; i<childCount; i++)
	{
		GTACommandBase *pCmd = NULL;
		ipElem->getCommand(i, pCmd);
		QString objID;
		if (pCmd != NULL)
			objID = pCmd->getObjId();
		if (!objID.isEmpty())
			_CommandUUIDs.append(objID);

		GTAActionSetList *pSetList = dynamic_cast<GTAActionSetList *>(pCmd);
		if (pSetList != NULL)
		{
			QList<GTAActionSet *> Setlist; pSetList->getSetCommandlist(Setlist);
			foreach(GTAActionSet* pSet, Setlist)
			{
				objID = pSet->getObjId();
				if (!objID.isEmpty())
					_CommandUUIDs.append(objID);
			}
		}
	}
	QString elemName = ipElem->getName();
	QString elemRelativePath = ipElem->getRelativeFilePath();
	QString uuid = ipElem->getUuid();
	for (int i = 0; i<childCount; i++)
	{
		QList<ErrorMessage> errorList;
		GTACommandBase *pCmd = NULL;

		ipElem->getCommand(i, pCmd);
		QString sCurrElementName = ipElem->getDocumentName();

		if (pCmd != NULL)
		{
			if (pCmd->isIgnoredInSCXML())
			{
				continue;
			}
			if (pCmd->hasReference())
			{
				GTAActionCall *pCall = dynamic_cast<GTAActionCall *>(pCmd);
				QString callElemName = pCall->getElement();
				GTAElement *pCallElem = NULL;

				bool isUUID = true;
				QString callElemUuid = pCall->getRefrenceFileUUID();

				_UUIDElementHash.insertMulti(callElemUuid, pCall);
				LogMessageInfo info;
				info.fileName = elemRelativePath;
				info.lineNumber = QString::number(i);
				info.uuid = uuid;
				_CallCmdHash.insert(pCall, info);


				rc = getElementFromDocument(iDataDirectory, callElemUuid, pCallElem, isUUID);

				if (rc && pCallElem != NULL)
				{
					QMap <QString, QString> tagMap = pCall->getTagValue();
					if (!tagMap.isEmpty())
					{
						pCallElem->replaceTag(tagMap);
						foreach(QString key, tagMap.keys())
						{
							_LocalValuesList.insert(key, tagMap.value(key));
						}
					}

					bool validationEnabled = TestConfig::getInstance()->getValidationEnable();

					if ((pCallElem->getValidationStatus() != XNODE_VALIDATED) && validationEnabled)
					{
						QString desc = QString("The element %1 in use is not validated").arg(callElemName);
						QString lineNum = QString::number(i);
						ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kWarning, ErrorMessage::MsgSource::kCheckCompatibility, callElemName, lineNum, uuid);
						oErrorList.append(error);
					}

					//check if a parent is called from a child. mark it as recursion if present.
					/*
					fun1:
					call fun2
					...
					call funN
					under no circumstances can a nested function call its parent. Not allowed.
					This becomes a recursion and causes an infinite loop.
					*/
					if (!icheckForRep.contains(uuid))
					{
						icheckForRep.append(uuid);
						rc = checkCommandCompatibility(iDataDirectory, pCallElem, errorList, true, sCurrElementName, callElemName, icheckForRep, hshReferncedItem, isRecuriosn);
						icheckForRep.removeOne(uuid);
						isRecuriosn = NO_ERROR;
					}
					else
					{
						rc = false;
						isRecuriosn = RECURSION;
					}
					if (rc)
					{
						pCmd->setCmdValidaityStatus(true);
					}
					else
					{
						if (isRecuriosn == RECURSION)
						{
							QString desc = QString("Call (%1) element has recursion.").arg(callElemName);
							QString lineNum = QString::number(i);
							ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, callElemName, lineNum, callElemUuid);
							oErrorList.append(error);
							pCmd->setCmdValidaityStatus(false);
							globalRc = false;
						}
						else
						{
							int nb_error = 0;
							// If errorList does not contain ErrorType::ERROR, so we raise a WARNING, else an ERROR message 
							foreach(ErrorMessage eMessage, errorList)
							{
								if (eMessage.errorType == ErrorMessage::ErrorType::kError)
									nb_error += 1;
							}
							QString desc = QString("Parameter(s) used in Call (%1) is/are inconsistent ").arg(callElemName);
							QString lineNum = QString::number(i);
							if (nb_error == 0)
							{
								ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kWarning, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
								oErrorList.append(error);
							}
							else
							{
								ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
								oErrorList.append(error);
							}
							// This line can be removed
							oErrorList.append(errorList);
							pCmd->setCmdValidaityStatus(false);
							globalRc = false;
						}
					}
				}
				else
				{
					pCmd->setCmdValidaityStatus(false);
					QString desc = QString("Call (%1) element is not found.").arg(callElemName);
					QString lineNum = QString::number(i);
					ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
					oErrorList.append(error);
					globalRc = false;
				}
			}
			else
			{
				//_CommandValidityInfo.clear();
				if (isCall)
					rc = getCommandStatus(pCmd,oErrorList,uuid, invalidParams, i, isCall, iCallElemName);
				else
				{
					try
					{
						rc = getCommandStatus(pCmd, oErrorList, uuid, invalidParams, i, isCall, elemRelativePath);

					} catch (const std::out_of_range&) {
						pCmd->setCmdValidaityStatus(false);
						QString desc = QString("The name of one of the columns has been changed, which will cause the Set action to fail. Please update the procedure accordingly");
						QString lineNum = QString::number(i);
						ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
						oErrorList.append(error);
						globalRc = false;
					}
				}
								

				if (rc)
				{
					foreach(CmdInfo sCmdInfo, _CommandValidityInfo)
					{
						if (sCmdInfo.subCmdType != PARAMETER_TYPE && sCmdInfo.subCmdType != INIT_PIR)
						{
							if (sCmdInfo.subCmdType == EXTERNAL_TOOL)
							{
								// add all the checks for the tool here.
								QList<ExternalTool> extToolInfoList = sCmdInfo.externalToolInfo;
								foreach(ExternalTool tool, extToolInfoList)
								{
									QList<ExternalToolFunc> funcList = tool.funcList;
									foreach(ExternalToolFunc function, funcList)
									{
										QList<ExternalToolArgument> argList = function.argList;
										foreach(ExternalToolArgument arg, argList)
										{
											if (!arg.isArgumentValid)
											{
												pCmd->setCmdValidaityStatus(false);
												QString desc = QString("EXTERNAL TOOL (%1) with FUNCTION '%2( )' - Entered argument for (%3) is invalid.").arg(tool.toolName, function.FunctionName, QString(arg.ArgName).toUpper());
												ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
												oErrorList.append(error);
												globalRc = false;
											}
										}
									}
									// check for mismatch
									if (tool.isMismatch == true)
									{
										pCmd->setCmdValidaityStatus(false);
										QString desc = QString("EXTERNAL TOOL (%1) - There is a mismatch with the config file.").arg(tool.toolName);
										ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kWarning, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
										oErrorList.append(error);
										globalRc = false;
									}
								}
							}
						}
						else
						{
							QList<ParamInfo> paramInfoList = sCmdInfo.paramList;
							QString cmdType = sCmdInfo.cmdType;

							if (pCmd->getCommandType() == GTACommandBase::ACTION)
							{
								GTAActionBase *pAct = dynamic_cast<GTAActionBase*>(pCmd);
								if (pAct->getComplement() == COM_CONDITION_FOR_EACH)
								{
									GTAActionForEach *pForEach = dynamic_cast<GTAActionForEach*>(pAct);

									QString FileToParse;
									if (pForEach->getRelativePath().isEmpty())
									{
										FileToParse = pForEach->getPath();
									}
									else
									{
										QString repoPath = QString::fromStdString(TestConfig::getInstance()->getRepositoryPath());
										FileToParse = repoPath + pForEach->getRelativePath();
									}
									QFile fileObj(FileToParse);
									bool rc1 = fileObj.open(QFile::ReadOnly | QFile::Text);
									if (!rc1)
									{
										pCmd->setCmdValidaityStatus(false);
										QString desc = QString("The TABLE could not be found in the mentioned path.");
										ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
										oErrorList.append(error);
										globalRc = false;
									}
									fileObj.close();
								}
							}
							foreach(ParamInfo info, paramInfoList)
							{
								QString param = info.paramName;
								QString val = info.val;

								bool testDouble = false;
								bool testInt = false;
								bool isRampValue = val.startsWith("Ramp") ? true : false;

								if (val.contains(":CIC") || val.contains(":NCIC"))
								{
									val.replace("'", "");
									val.replace("`", "");
									val.replace(":NCIC", "");
									val.replace(":CIC", "");
									info.val = val;
								}

								param.toDouble(&testDouble);
								param.toInt(&testInt, 10);

								bool coupleOk = false;
								if (!info.emptyCSVCases.isEmpty())
								{
									pCmd->setCmdValidaityStatus(false);
									for (auto emptycase : info.emptyCSVCases)
									{						
										QString desc = QString("The TABLE contains an empty cell in [%1], it is forbidden. Please fill the cell with a valid Value!").arg(emptycase);
										ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
										oErrorList.append(error);
									}
									globalRc = false;
									break;
								}
								// We MUST verify that the param is not a number in order to do not create a numeric local value
                                if ( (testDouble || testInt) && val.contains("@") && isValidTagVar(val) )
                                {
                                    // Param is a number and Value is an @arg@
                                    // Need to exchange data
                                    QString temp = QString();
                                    temp = val;
                                    val = param;
                                    param = temp;

                                    coupleOk = true;
								}
								else if (testDouble || testInt)
								{
									if (cmdType == "set")
									{
										coupleOk = false;
									}
									else
									{
										coupleOk = true;
									}
								}
								else
								{
									coupleOk = true;
									info.isTagVar = ((val.contains("@") && isValidTagVar(val)) || (param.contains("@") && isValidTagVar(param)));
									if (param.contains("@"))
									{
										info.isTagVarValid = isValidTagVar(param);
										// Need to exchange data
										QString temp = QString();
										temp = val;
										val = param;
										param = temp;
										temp = info.val;
										info.val = info.paramName;
										info.paramName = temp;
									}
									if (val.contains("@"))
										info.isTagVarValid = isValidTagVar(val);
								}

								const QString reservedWords[] = {
									"abstract", "arguments", "await", "boolean", "break", "byte", "case", "catch", "char", "class",
									"const", "continue", "debugger", "default",	"delete", "do", "double", "else", "enum","eval",
									"export", "extends", "false", "final", "finally", "float", "for", "function", "goto", "if",
									"implements","import", "in", "instanceof", "int", "interface", "let", "long",	"native", "new",
									"null", "package",	"private", "protected", "public", "return", "short", "static", "super", "switch",
									"synchronized", "this", "throw", "throws", "transient", "true", "try", "typeof", "var", "void",
									"volatile",	"while", "with", "yield"
								};

								const QString* foundWord = std::find(std::begin(reservedWords), std::end(reservedWords), param);
								
								if (foundWord != std::end(reservedWords)) 
								{
									pCmd->setCmdValidaityStatus(false);
									QString errorDescription = QString("'%1' is not a valid param name as it is a reserved ECMAScript word.").arg(param);
									ErrorMessage error = createErrorMsg(errorDescription, ErrorMessage::ErrorType::kError,
										ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
									oErrorList.append(error);
									globalRc = false;
								}

								// The couple (Param/value) seems to be correct
								if (coupleOk)
								{
									if (cmdType == ACT_MATHS)
									{
										GTAActionMaths *pMaths = dynamic_cast<GTAActionMaths*>(pCmd);
										GTAMathExpression *lMathsExp = pMaths->getExpression();
										QString lExp = lMathsExp->getExpression();

										QRegExp sep("([+-*/]+)");
										QStringList items = lExp.replace(sep, "").split(QRegularExpression("\\s+"));

										// If a = a + 1, this condition would create a new local param (a)
										if (!items.contains(param))
										{
											if (!_LocalParamTypesList.contains(param))
											{
												_LocalParamTypesList.insert(param, "Float");
											}

											for (const auto& elm : items)
											{
												bool rc = false;
												QString LocalValType = QString();

												// Check if the param is already defined and assigned in order to get its paramType
												if (!_LocalValuesList.isEmpty() && _LocalValuesList.contains(elm))
												{
													rc = checkIsConstant(_LocalValuesList.value(elm), LocalValType);
												}
												else
												{
													rc = checkIsConstant(elm, LocalValType);
												}

												if (LocalValType == "Invalid_Type" && isValidTagVar(elm))
												{
													LogMessageInfo tagInfo;
													tagInfo.fileName = elemRelativePath;
													tagInfo.lineNumber = QString::number(i);
													tagInfo.uuid = uuid;
													_TagVariable.insert(elm, tagInfo);
												}
												
                                                if (LocalValType == "String" || LocalValType == "Char") // @14/06/2021 Boolean is now allowed
												{
													pCmd->setCmdValidaityStatus(false);
													QString desc = QString("The ('%1') element is of type %2, it is not compatible with the Maths function.").arg(elm, LocalValType);
													ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
													oErrorList.append(error);
													globalRc = false;
												}
											}
										}
										info.valueType = "Float";

										if (!info.isParamInDb && !info.isTagVar)
										{
											bool isParamDbType = checkIsParamDbType(info.paramName);
											if (!isParamDbType &&
												info.paramName.count('.') == 2)
											{
												pCmd->setCmdValidaityStatus(false);
												QString desc = QString("Parameter ('%1') is not present in the DATABASE and will be treated as a local variable").arg(info.paramName);
												ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility,
													elemRelativePath, QString::number(i), uuid);
												oErrorList.append(error);
												globalRc = false;
											}
										}
									}
									else if (!param.isEmpty() && cmdType == ACT_PRINT)
									{
										bool isParamDbType = checkIsParamDbType(info.paramName);
										if (isParamDbType == false && info.paramName.count('.') == 2)
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("Print Action: Parameter ('%1') is not present in the DATABASE and the value can't be printed").arg(info.paramName);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);
											globalRc = false;
										}
									}
									else if (!param.isEmpty() && !val.isEmpty() || cmdType == ACT_SUBSCRIBE || cmdType == ACT_UNSUBSCRIBE)
									{
										if (info.valueType != "Invalid_Type")
										{
											nType = info.valueType;
										}
										else
										{
											if (!val.isEmpty())
											{
												bool rc;
												rc = checkIsConstant(val, nType);
											}
										}
										info.nextType = nType;

										if (info.isParamInDb)
										{
											//manage the handling of Engine parameters with parameter name
											QList<GTAICDParameter> benchParams;
											QStringList l_paramListKeys = _ParamList.keys();
											bool isVarDbType = checkIsParamDbType(info.val);

											if (l_paramListKeys.contains(param, Qt::CaseInsensitive))
											{
												if (checkIsEngineParameter(l_paramListKeys.filter(param, Qt::CaseInsensitive)[0]))
												{
													QStringList resolvedParams;
													GTAUtil::resolveChannelParam(l_paramListKeys.filter(param, Qt::CaseInsensitive)[0], resolvedParams);
													foreach(QString iterparam, resolvedParams)
													{
														GTAICDParameter benchParam = _ParamList.value(iterparam);
														if (!benchParam.getTempParamName().isEmpty())
															benchParams.append(benchParam);
													}
												}
												else
												{
													GTAICDParameter benchParam = _ParamList.value(l_paramListKeys.filter(param, Qt::CaseInsensitive)[0]);
													benchParams.append(benchParam);
												}
												foreach(GTAICDParameter benchParameter, benchParams)
												{
													QString toolName = benchParameter.getToolName();
													if ((toolName.isEmpty() || toolName.compare("NA", Qt::CaseInsensitive) == 0) && (!info.paramName.startsWith("line")))
													{
														pCmd->setCmdValidaityStatus(false);
														QString desc = QString("Parameter ('%1') has no ToolName. SCXML generation will be affected").arg(info.paramName);
														ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
														oErrorList.append(error);
														globalRc = false;
													}
													
													else if (isVarDbType == false && info.val.count('.') == 2 && !isRampValue)
													{
														pCmd->setCmdValidaityStatus(false);
														QString desc = QString("Parameter ('%1') is not present in the DATABASE").arg(info.val);
														ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
														oErrorList.append(error);
														globalRc = false;
													}

												}
											}
										}
										// if the parameter is not in db and the parameter is not local variable,
										// then report.
										if ((info.isParamInDb == false) && (info.isParamLocal == false) && (info.isConstant == false) && (cmdType != ACT_MANUAL))
										{
											// Nothing to do
										}
										if ((info.isParamInDb == false) && !info.isTagVar)
										{
											bool isParamDbType = checkIsParamDbType(info.paramName);
											bool isVarDbType = checkIsParamDbType(info.val);
											if (isParamDbType == false && info.paramName.count('.') == 2)
											{
												pCmd->setCmdValidaityStatus(false);
												QString desc = QString("Parameter ('%1') is not present in the DATABASE and will be treated as a local variable").arg(info.paramName);
												ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
												oErrorList.append(error);
												globalRc = false;
											}
											else if (isVarDbType == false && info.val.count('.') == 2)
											{
												pCmd->setCmdValidaityStatus(false);
												QString desc = QString("Parameter ('%1') is not present in the DATABASE and will be treated as a local variable").arg(info.val);
												ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
												oErrorList.append(error);
												globalRc = false;
											}
										}
										if ((info.isTagVar))
										{
											if (!info.isTagVarValid)
											{
												pCmd->setCmdValidaityStatus(false);
												QString desc = QString("Parameter ('%1') contains invalid Tags (%2).").arg(param, info.paramName);
												ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
												oErrorList.append(error);
												globalRc = false;
											}
											else
											{
												LogMessageInfo tagInfo;
												tagInfo.fileName = elemRelativePath;
												tagInfo.lineNumber = QString::number(i);
												tagInfo.uuid = uuid;
												_TagVariable.insert(info.val, tagInfo);
											}
										}

										// if the parameter is assigned/checked to a value, like in set or check
										// and if the value assigned is not a correct type,
										// like int value assigned to boolean variable, this code is executed.
										if ((info.isValApplicable == true) && (info.isValCorrect == false) && (info.isParamLocal == false) && (info.isEquation == false))
										{
											QString paramType = QString();
											rc = checkIsConstant(param, paramType);
											QString valueType = QString();
											rc = checkIsConstant(val.trimmed(), valueType);
											if (!_LocalParamTypesList.isEmpty() && _LocalParamTypesList.contains(val))
											{
												valueType = "Float";
											}
											if (paramType != valueType && !_LocalParamTypesList.isEmpty() && !_LocalParamTypesList.contains(val)) //!checkIsLocal(valueType))
											{
												pCmd->setCmdValidaityStatus(false);
												QString desc = QString("Expected Parameter Type (%1) - Entered/Assigned Parameter Type (%2)").arg(paramType, valueType);
												ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
												oErrorList.append(error);
												globalRc = false;
											}
										}
										if (info.isPIR && !info.isParamPIR)
										{
											QString desc = QString("Parameter ('%1') is not a valid PIR signal").arg(param);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);
											globalRc = false;
										}
										if ((info.isParamInDb == true) && (info.isValApplicable == true))
										{
											GTAActionBase *pAct = dynamic_cast<GTAActionBase*>(pCmd);
											if (pAct != nullptr)
											{
												QString paramType = QString();
												QString valueType = QString();
												QString lineNum = QString::number(i);
												QString LocalValType = QString();
												QString l_eqName = QString();
												if (val.contains("{") && !val.startsWith("Result", Qt::CaseInsensitive)) 
													l_eqName = val.split("{").first();

												info.isParamLocal = false;

												// Check if the param is already defined and assigned in order to get its paramType
												if (!_LocalValuesList.isEmpty() && (_LocalValuesList.contains(param) || _LocalResultExtTool.contains(param)))
												{
													LocalValType = _LocalValuesList.value(param);
													rc = checkIsConstant(LocalValType, paramType);

													if (_LocalResultExtTool.contains(param))
													{
														paramType = _LocalResultExtTool.value(param);
													}
												}
												else
												{
													paramType = info.valueType;
												}

												// Check if the value is already defined as local param
												if (!_LocalValuesList.isEmpty() && _LocalValuesList.contains(val))
												{
													LocalValType = _LocalValuesList.value(val);
													rc = checkIsConstant(LocalValType, valueType);
												}
												else
												{
													rc = checkIsConstant(val, valueType);
												}
												// This case is only used for assignement when a Maths function is called
												if (!_LocalParamTypesList.isEmpty() && _LocalParamTypesList.contains(val))
												{
													valueType = "Float";
												}

												// For Conditional execution (call *.obj / *.func args)
												if ((pAct->getAction() == ACT_CONDITION) && (pAct->getComplement() == COM_CONDITION_IF))
												{
													// We compare the type of the value with the type of the parameter
													if ((valueType == "Boolean" && paramType == "Integer") || (valueType == "Integer" && paramType == "Boolean"))
													{
														// Conditions are used as a switch() function, 0/1 can be both integer or boolean values
													}
													else if ((valueType == "Integer" && paramType == "Float") || (valueType == "Float" && paramType == "Integer"))
													{
														// It is not always considered as TRUE
													}
													else if (l_mathFunctions.contains(l_eqName, Qt::CaseSensitive))
													{
														// This case is very special, we affect an equation to a variable
													}
													else if (val.contains("_uuid[{") || (valueType == "Invalid_Type" && isValidTagVar(val)))
													{
														// Nothing to do
													}
													else if (paramType != valueType)
													{
														QString desc = QString("ICD Parameter ('%1') - Entered value (%3) does not match with the expected parameter type (%2)").arg(param, paramType, valueType);
														ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
														oErrorList.append(error);
														globalRc = false;
													}
												}
												else if ((pAct->getAction() == ACT_WAIT) && (pAct->getComplement() == COM_WAIT_UNTIL))
												{
													GTAActionWait *pActWait = dynamic_cast<GTAActionWait*>(pCmd);

													if (pActWait->getIsFsOnly() || pActWait->getIsSDIOnly() || pActWait->getIsParityOnly())
													{
														// Nothing to do
													}
												}
												else // For other execution : SET Action
												{
													if (!_LocalValuesList.isEmpty() && _LocalValuesList.contains(val))
														info.isInRange = checkForRange(param, _LocalValuesList.value(val), info.rangeMinVal, info.rangeMaxVal);
													else
														info.isInRange = checkForRange(param, val, info.rangeMinVal, info.rangeMaxVal);
													// We compare the type of the value with the type of the parameter
													if ((valueType == "Boolean" && paramType == "Integer") || (valueType == "Integer" && paramType == "Boolean"))
													{
														// Conditions are used as a switch() function, 0/1 can be both integer or boolean values
													}
													else if ((valueType == "Integer" && paramType == "Float") || (valueType == "Float" && paramType == "Integer"))
													{
														// It is not always considered as TRUE
													}
													else if (l_mathFunctions.contains(l_eqName, Qt::CaseSensitive) || (valueType == "Invalid_Type" && isValidTagVar(val)))
													{
														// This case is very special, we affect an equation to a variable
													}
													else if (paramType == "Invalid_Type" && valueType == "Invalid_Type" && isValidTagVar(val))
													{
														// Specific case where local variable gets the value of an argument passed in parameter in a child element
													}
													else if (paramType != "Invalid_Type" && valueType == "Invalid_Type" && !_LocalValuesList.isEmpty() && _LocalValuesList.contains(val))
													{
														// Specific case where ICD Parameter gets the value of an argument
													}
													else if (paramType != valueType)
													{
														QString desc = QString("ICD Parameter ('%1') - Entered value (%2) does not match with the expected parameter type (%3)").arg(param, valueType, paramType);
														ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
														oErrorList.append(error);
														globalRc = false;
													}
												}
											}
											else // pAct == nullptr in case of CHECK action
											{
												info.isParamLocal = false;
											}

										}
										if ((info.isParamLocal == true) && (info.isValApplicable == true))
										{
											GTAActionBase *pAct = dynamic_cast<GTAActionBase*>(pCmd);
											if (pAct != nullptr)
											{
												QString paramType = QString();
												QString valueType = QString();
												QString lineNum = QString::number(i);
												bool insertVal = false;
												bool variableStored = false;
												QString l_eqName = QString();
												if (val.contains("{") && val.startsWith("Result", Qt::CaseInsensitive))
													l_eqName = val.split("{").first();

												// Check if the param is already defined and assigned in order to get its paramType
												if (!_LocalValuesList.isEmpty() && (_LocalValuesList.contains(param) || _LocalResultExtTool.contains(param)))
												{
													QString LocalValType = QString();
													LocalValType = _LocalValuesList.value(param);
													rc = checkIsConstant(LocalValType, paramType);

													if (_LocalResultExtTool.contains(param))
														paramType = _LocalResultExtTool.value(param);
												}
												else
												{
													if ((pAct->getAction() == ACT_CONDITION) && (pAct->getComplement() == COM_CONDITION_IF))
														paramType = nType;
													else
														paramType = info.valueType;
												}

												// Check if the val is already defined as local param
												if (!_LocalValuesList.isEmpty() && _LocalValuesList.contains(val))
												{
													QString LocalValType = QString();
													LocalValType = _LocalValuesList.value(val);
													rc = checkIsConstant(LocalValType, valueType);
													variableStored = true;
												}
												else
												{
													// This case is specific for GET function through ExternalTool
													if(val.startsWith("Result", Qt::CaseInsensitive) && val.contains("__"))
														valueType = info.valueType;
													else
														rc = checkIsConstant(val, valueType);
												}

												// For Conditional execution (call *.obj / *.func args)
												if ((pAct->getAction() == ACT_CONDITION) && (pAct->getComplement() == COM_CONDITION_IF))
												{
													// We compare the type of the value with the type of the parameter
													if ((valueType == "Boolean" && paramType == "Integer") || (valueType == "Integer" && paramType == "Boolean"))
													{
														// Conditions are used as a switch() function, 0/1 can be both integer or boolean values
													}
													else if ((valueType == "Float" && paramType == "Integer") || (valueType == "Integer" && paramType == "Float"))
													{
														// It is not always considered as TRUE
													}
													else if (l_mathFunctions.contains(l_eqName, Qt::CaseSensitive))
													{
														// This case is very special, we affect an equation to a variable
													}
													else if (paramType != valueType)
													{
														QString msg = QString("Local Parameter ('%1') - Entered value does not match with its expected parameter type (%2)").arg(param, paramType);
														ErrorMessage error = createErrorMsg(msg, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
														oErrorList.append(error);
														globalRc = false;
													}
												}
												// For Math application
												else if (pAct->getAction() == ACT_MATHS)
												{
													// Nothing to do - DO NOT REMOVE THIS CASE
												}
												// For WHILE / DO_WHILE condition
												else if ((pAct->getAction() == ACT_CONDITION) && (pAct->getComplement() == COM_CONDITION_WHILE || pAct->getComplement() == COM_CONDITION_DO_WHILE))
												{
													// Nothing to do - DO NOT REMOVE THIS CASE
												}
												else // For other execution (SET...)
												{
													bool isParamDbType = checkIsParamDbType(info.paramName);
													if (!isParamDbType == true && valueType != "Invalid_Type")
													{
														if (!variableStored)
														{
															// Insert a couple of param/value for each local parameter
															_LocalValuesList.insert(param, val);
														}
														else
														{
															// Insert a couple of param/value for each local parameter
															QString valueStored = _LocalValuesList.value(val);
															_LocalValuesList.insert(param, valueStored);
														}
														insertVal = true;
													}
													if (!isParamDbType == true && valueType == "Invalid_Type" && isValidTagVar(val))
													{
														if (!variableStored)
														{
															// Insert a couple of param/value for each local parameter
															_LocalValuesList.insert(param, val);
														}
													}

													// We compare the type of the value with the type of the parameter
													if ((valueType == "Boolean" && paramType == "Integer") || (valueType == "Integer" && paramType == "Boolean"))
													{
														// Conditions are used as a switch() function, 0/1 can be both integer or boolean values
													}
													else if ((valueType == "Float" && paramType == "Integer") || (valueType == "Integer" && paramType == "Float"))
													{
														// It is not always considered as TRUE
													}
													else if (l_mathFunctions.contains(l_eqName, Qt::CaseSensitive))
													{
														// This case is very special, we affect an equation to a variable
													}
													else if (paramType == "Invalid_Type" && valueType == "Invalid_Type" && isValidTagVar(val))
													{
														// Specific case where local variable gets the value of an argument passed in parameter in a child element
													}
													else if ((paramType != valueType) && !insertVal)
													{
														QString msg = QString("Local Parameter ('%1') - Entered Value does not match with its parameter type.").arg(param);
														ErrorMessage error = createErrorMsg(msg, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
														oErrorList.append(error);
														globalRc = false;
													}
												}
											}
											else // pAct == nullptr in case of CHECK action
											{
												GTACheckValue *pChkCmd = dynamic_cast<GTACheckValue*>(pCmd);
												QString lineNum = QString::number(i);

												QString l_val = pChkCmd->getValue(0);
												QString valType = QString();
												rc = checkIsConstant(l_val, valType);
												QString valueType = QString();

												// Check if the val is already defined as local param
												if (!_LocalValuesList.isEmpty() && _LocalValuesList.contains(param))
												{
													QString LocalValType = QString();
													LocalValType = _LocalValuesList.value(param);
													rc = checkIsConstant(LocalValType, valueType);
												}
												// We compare the type of the value with the type of the parameter
												if ((valType == "Boolean" && valueType == "Integer") || (valType == "Integer" && valueType == "Boolean"))
												{
													// Conditions are used as a switch() function, 0/1 can be both integer or boolean values												
												}
												else if ((valType == "Float" && valueType == "Integer") || (valType == "Integer" && valueType == "Float"))
												{
													// It is not always considered as TRUE
												}
												else if (valType != valueType)
												{
													if (valType == "Invalid_Type")
													{
														// Nothing to do
													}
													else if (!valueType.isEmpty())
													{
														QString msg = QString("Local Parameter ('%1') - Expected type (%2) and entered type (%3) it does not match").arg(param, valueType, valType);
														ErrorMessage error = createErrorMsg(msg, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, lineNum, uuid);
														oErrorList.append(error);
														globalRc = false;
													}
												}
											}
										}
										if ((info.containsInvalidChars == true))
										{
											QString invalidChars = info.invalidChars.join(" ").trimmed();
											if (!invalidChars.isEmpty())
											{
												pCmd->setCmdValidaityStatus(false);
												QString desc = QString("Parameter ('%1') contains some invalid Characters( %2 ) ").arg(param, invalidChars);
												ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
												oErrorList.append(error);
												globalRc = false;
											}
										}
										/*if ((info.isValidFSType == false) && (info.isSetCheckFS == true))
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("Parameter ('%1') cannot be Set/Check for only FS (Applicable to Only ICD Parameters of AFDX/CAN/A429)").arg(param);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);
											globalRc = false;
										}*/

										if (info.isInRange == false)
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("About Parameter ('%1') its provided value (%2) is out of its range [%3;%4]").arg(param, val, info.rangeMinVal, info.rangeMaxVal);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kWarning, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);
											globalRc = false;
										}
										
										if (info.isParamSubscribed == false)
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("Parameter ('%1') is not subscribed.").arg(info.paramName);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);
											globalRc = false;
										}
										if (!info.hasValidSubscribe)
										{
											pCmd->setCmdValidaityStatus(false);
											QStringList lstInvalidParams = info.invalidSubscribeParam;
											QString invalidParams = lstInvalidParams.join("\n");
											QString desc = QString("Following parameter ('%1') is already subscribed.").arg(invalidParams);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);

											globalRc = false;
										}
										if (!info.hasValidUnsubscribe)
										{
											pCmd->setCmdValidaityStatus(false);
											QStringList lstInvalidParams = info.invalidUnSubscribeParam;
											QString invalidParams = lstInvalidParams.join("\n");
											QString desc = QString("Following parameter ('%1') is already unsubscribed or not subscribed before use.").arg(invalidParams);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);

											globalRc = false;
										}
										
										if (!info.isInternalParamValid)
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("The command uses out of scope internal parameter ('%1')").arg(info.paramName);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);
											globalRc = false;
										}

										if (!info.isEngineParamInDb && !info.invalidEngineParams.isEmpty())
										{
											pCmd->setCmdValidaityStatus(false);
											QString invalidParams = info.invalidEngineParams.join(",");
											QString desc = QString("The channel in control parameters ('%1') are not present in the DATABASE and will be treated as local").arg(invalidParams);
											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);

											globalRc = false;
										}
										if (globalRc == true)
										{
											pCmd->setCmdValidaityStatus(true);
										}
									}
								}
								else // Error on the couple (Param/value)
								{
									if (!param.isEmpty() && !val.isEmpty())
									{
										pCmd->setCmdValidaityStatus(false);
										QString desc = QString("The couple param('%2') / value(%3) cannot be treated, please verify the parameters.").arg(QString::number(i), param, val);
										ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
										oErrorList.append(error);

										globalRc = false;
									}
								}
							}
						}
					}
				}
				else
				{
					pCmd->setCmdValidaityStatus(false);
					QString desc = QString("The TABLE contains a column named \"Value\", it is forbidden. Please use another column name!");
					ErrorMessage error = createErrorMsg(desc, ErrorMessage::ErrorType::kError, ErrorMessage::MsgSource::kCheckCompatibility, elemRelativePath, QString::number(i), uuid);
					oErrorList.append(error);
					globalRc = false;
				}
				/*if (pCmd != NULL)
				{
					if (pCmd->getCommandType() == GTACommandBase::ACTION)
					{
						GTAActionBase * pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
						QString sConfTime, sConfUnit;
						QString sTimeOutTime, sTimeOutUnit;
						pActCmd->getConfCheckTime(sConfTime, sConfUnit);
						pActCmd->getTimeOut(sTimeOutTime, sTimeOutUnit);

						if (pActCmd->hasConfirmatiomTime())
						{
							bool ok = false;
							// to validate both against equal grounds
							if (sConfUnit == "sec")
							{
								// Test Float value
								if (sConfTime.contains("."))
								{
									sConfTime.toDouble(&ok);
									if (ok)
									{
										sConfTime = QString::number(sConfTime.toDouble(&ok) * 1000);
									}
									else
									{
										pCmd->setCmdValidaityStatus(false);
										QString desc = QString("Confirmation time (%1) is wrong, it is not a Float").arg(sConfTime);

										ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
										oErrorList.append(error);

										globalRc = false;
									}
								}
								else
								{
									// Test Integer value
									sConfTime.toInt(&ok, 10);
									if (ok)
									{
										if (ok)
										{
											sConfTime = QString::number(sConfTime.toInt(&ok) * 1000);
										}
										else
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("Confirmation time (%1) is wrong, it is not an Integer").arg(sConfTime);

											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);

											globalRc = false;
										}
									}
								}
								sConfUnit = "msec";
							}
							if (sTimeOutUnit == "sec")
							{
								// Test Float value
								if (sTimeOutTime.contains("."))
								{
									sTimeOutTime.toDouble(&ok);
									if (ok)
									{
										sTimeOutTime = QString::number(sTimeOutTime.toDouble(&ok) * 1000);
									}
									else
									{
										pCmd->setCmdValidaityStatus(false);
										QString desc = QString("Timeout value (%1) is wrong, it is not a Float").arg(sTimeOutTime);

										ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
										oErrorList.append(error);

										globalRc = false;
									}
								}
								else
								{
									// Test Integer value
									sTimeOutTime.toInt(&ok, 10);
									if (ok)
									{
										if (ok)
										{
											sTimeOutTime = QString::number(sTimeOutTime.toInt(&ok) * 1000);
										}
										else
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("Timeout value (%1) is wrong, it is not an Integer").arg(sTimeOutTime);

											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);

											globalRc = false;
										}
									}
								}
								sTimeOutUnit = "msec";
							}
							if ((!sTimeOutTime.isEmpty() && sTimeOutTime != "0") && (!sConfTime.isEmpty() && sConfTime != "0"))
							{
								if (!(sConfTime.toDouble() < sTimeOutTime.toDouble()))
								{
									pCmd->setCmdValidaityStatus(false);
									QString desc = QString("Confirmation time (%1 %2) can not be more or equal to the Timeout value (%3 %4)").arg(sConfTime, sConfUnit, sTimeOutTime, sTimeOutUnit);
									ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
									oErrorList.append(error);

									globalRc = false;
								}
							}
						}
					}
					else if (pCmd->getCommandType() == GTACommandBase::CHECK)
					{
						GTACheckBase * pChkCmd = dynamic_cast<GTACheckBase *>(pCmd);
						QString sConfTime, sConfUnit;
						QString sTimeOutTime, sTimeOutUnit;
						pChkCmd->getConfCheckTime(sConfTime, sConfUnit);
						pChkCmd->getTimeOut(sTimeOutTime, sTimeOutUnit);
						bool hasWaitUntil = false;
						if (pChkCmd->getCheckType() == GTACheckBase::VALUE)
						{
							GTACheckValue * pChkValCmd = dynamic_cast<GTACheckValue*>(pCmd);
							hasWaitUntil = pChkValCmd->getWaitUntil();
						}

						if (pChkCmd->hasConfirmatiomTime() && hasWaitUntil)
						{
							bool ok = false;
							// to validate both against equal grounds
							if (sConfUnit == "sec")
							{
								// Test Float value
								if (sConfTime.contains("."))
								{
									sConfTime.toDouble(&ok);
									if (ok)
									{
										sConfTime = QString::number(sConfTime.toDouble(&ok) * 1000);
									}
									else
									{
										pCmd->setCmdValidaityStatus(false);
										QString desc = QString("Confirmation time (%1) is wrong, it is not a Float").arg(sConfTime);

										ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
										oErrorList.append(error);

										globalRc = false;
									}
								}
								else
								{
									// Test Integer value
									sConfTime.toInt(&ok, 10);
									if (ok)
									{
										if (ok)
										{
											sConfTime = QString::number(sConfTime.toInt(&ok) * 1000);
										}
										else
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("Confirmation time (%1) is wrong, it is not an Integer").arg(sConfTime);

											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);

											globalRc = false;
										}
									}
								}
								sConfUnit = "msec";
							}
							if (sTimeOutUnit == "sec")
							{
								// Test Float value
								if (sTimeOutTime.contains("."))
								{
									sTimeOutTime.toDouble(&ok);
									if (ok)
									{
										sTimeOutTime = QString::number(sTimeOutTime.toDouble(&ok) * 1000);
									}
									else
									{
										pCmd->setCmdValidaityStatus(false);
										QString desc = QString("Timeout value (%1) is wrong, it is not a Float").arg(sTimeOutTime);

										ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
										oErrorList.append(error);

										globalRc = false;
									}
								}
								else
								{
									// Test Integer value
									sTimeOutTime.toInt(&ok, 10);
									if (ok)
									{
										if (ok)
										{
											sTimeOutTime = QString::number(sTimeOutTime.toInt(&ok) * 1000);
										}
										else
										{
											pCmd->setCmdValidaityStatus(false);
											QString desc = QString("Timeout value (%1) is wrong, it is not an Integer").arg(sTimeOutTime);

											ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
											oErrorList.append(error);

											globalRc = false;
										}
									}
								}
								sTimeOutUnit = "msec";
							}
							if ((!sTimeOutTime.isEmpty() && sTimeOutTime != "0") && (!sConfTime.isEmpty() && sConfTime != "0"))
							{
								if (!(sConfTime.toDouble() < sTimeOutTime.toDouble()))
								{
									pCmd->setCmdValidaityStatus(false);
									QString desc = QString("Confirmation time (%1 %2) can not be more or equal to the Timeout value (%3 %4)").arg(sConfTime, sConfUnit, sTimeOutTime, sTimeOutUnit);
									ErrorMessage error = createErrorMsg(desc, ErrorMessage::ERROR, ErrorMessage::CHECK_COMPATIBILITY, elemRelativePath, QString::number(i), uuid);
									oErrorList.append(error);

									globalRc = false;
								}
							}
						}
					}
				}*/
				_CommandValidityInfo.clear();
			}
		}
	}
	return globalRc;
}

ErrorMessage GTACheckCompatibility::createErrorMsg(const QString &iDesc,
	const ErrorMessage::ErrorType iType,
	const ErrorMessage::MsgSource iMsgSrc,
	const QString &iFile,
	const QString &iLine,
	const QString &iUUID)
{
	ErrorMessage error;
	error.description = iDesc;
	error.lineNumber = iLine;
	error.fileName = iFile;
	error.errorType = iType;
	error.source = iMsgSrc;
	error.uuid = iUUID;
	return error;
}

bool GTACheckCompatibility::checkForRecursion(QHash<QString, GTAElemRecStruct>& hshReferncedItem, const QString& sParentName, const QString& sCurrentItemName)
{
	if (!hshReferncedItem.isEmpty())
	{
		if (!sParentName.isEmpty() && hshReferncedItem.contains(sCurrentItemName))
		{
			GTAElemRecStruct refObj = hshReferncedItem.value(sParentName);
			QString sGrandParent = refObj._sParent;
			if (sGrandParent == sCurrentItemName)
			{
				return true;
			}
			else
				return checkForRecursion(hshReferncedItem, sGrandParent, sCurrentItemName);
		}
	}
	return false;
}

bool GTACheckCompatibility::isFileExist(const QString isFilePath)
{
	QFileInfo file(QDir::cleanPath(isFilePath));
	bool rc = file.exists();
	return rc;
}

bool GTACheckCompatibility::checkParamDirection(const QString &iCmdType, const QString &iParam)
{
	bool rc = false;

	QString direction = getParamDirection(iParam);
	if (direction.isEmpty())
		return true;

	if (iCmdType == ACT_SET)
	{
		if (direction == "INPUT" || direction == "I/O")
		{
			rc = true;
		}
	}
	else if (iCmdType == ACT_CONDITION || iCmdType == "WAIT UNTIL" || iCmdType == "CHECK VALUE")
	{
		if (direction == "OUTPUT" || direction == "I/O")
		{
			rc = true;
		}
	}
	return rc;
}


QString GTACheckCompatibility::getParamDirection(const QString &iParam)
{
	QString direction;
	QStringList l_paramListKeys = _ParamList.keys();
	// This QStringList contains only one value
	QStringList l_result;

	if (l_paramListKeys.contains(iParam, Qt::CaseInsensitive))
	{
		l_result = l_paramListKeys.filter(iParam, Qt::CaseInsensitive);
		GTAICDParameter parameter = _ParamList.value(l_result[0]);
		direction = parameter.getDirection();
	}
	return direction;
}

bool GTACheckCompatibility::checkIsParamInternal(const QString &iParam)
{
	QRegExp regExp("_uuid[{");
	if (iParam.contains("_uuid[{"))
	{
		return true;
	}
	else
		return false;
}

bool GTACheckCompatibility::checkIsInternalParamValid(const GTACommandBase *pCmd, const QString &iParam)
{
	if (pCmd)
	{
		QRegExp regExp("\\[\\{[a-zA-Z0-9]{8}-([a-zA-Z0-9]{4}-){3}[a-zA-Z0-9]{12}\\}\\]");
		bool rc = iParam.contains(regExp);
		// int id = regExp.indexIn(iParam);
		QString idinVAL = regExp.capturedTexts().first();
		idinVAL.remove("["); idinVAL.remove("]");
		//check if it is an internal param first
		if (!iParam.contains("line.") && rc)
		{
			foreach(QString uuid, _CommandUUIDs)
			{
				if (uuid == idinVAL)// .compare(idinVAL,Qt::CaseInsensitive))
				{
					return true;
				}
			}
		}

		QList<GTACommandBase*> Parentlst;
		GTACommandBase * pParent = pCmd->getParent();
		while (pParent != NULL)
		{
			Parentlst.append(pParent);
			pParent = pParent->getParent();
		}

		if (Parentlst.count() == 0)
			return false;

		pParent = NULL;
		for (int i = 0; i < Parentlst.count(); i++)
		{
			pParent = Parentlst.at(i);
			QString parentUUID = pParent->getObjId();
			if (parentUUID == idinVAL /*.compare(idinVAL,Qt::CaseInsensitive)*/ && iParam.contains("line."))
			{
				if (_ForColHeader.value(parentUUID).contains(iParam.split(INTERNAL_PARAM_SEP).last()))
					return true;
			}
		}
		Parentlst.clear();
	}
	return false;
}
