#pragma warning(disable: 4996) //alow use of getenv (warning issued by MSVC)

#include "GTADocumentController.h"
#include "GTAUtil.h"
#include "GTAHeader.h"
#include "GTAActionManualSerializer.h"
#include "GTAActionCondition.h"
#include "GTAActionConditionSerializer.h"
#include "GTACmdSerializer.h"
#include "GTAActionManual.h"
#include "GTAActionBase.h"
#include "GTAActionIF.h"
#include "GTAActionWhile.h"
#include "GTAActionIfSerializer.h"
#include "GTAActionWhileSerializer.h"
#include "GTAActionDoWhileSerializer.h"
#include "GTAActionElse.h"
#include "GTAActionElseSerializer.h"
#include "GTAActionIfEnd.h"
#include "GTAActionWhileEnd.h"
#include "GTAActionDoWhile.h"
#include "GTAActionDoWhileEnd.h"
#include "GTAActionPrint.h"
#include "GTAActionIRT.h"
#include "GTAActionOneClickPPC.h"
#include "GTAActionOneClickPPCSerializer.h"
#include "GTAOneClick.h"
#include "GTAOneClickSerializer.h"
#include "GTAActionIRTSerializer.h"
#include "GTAActionSetList.h"
#include "GTAActionSetListSerializer.h"
#include "GTACheckValue.h"
#include "GTACheckValueSerializer.h"
#include "GTACheckBiteMessage.h"
#include "GTACheckBiteMsgSerializer.h"
#include "GTACheckFwcWarning.h"
#include "GTACheckFwcWarningSerializer.h"
#include "GTAActionTitle.h"
#include "GTAActionTitleSerializer.h"
#include "GTAActionPrintSerializer.h"
#include "GTAActionWait.h"
#include "GTAActionWaitSerializer.h"
#include "GTAActionCall.h"
#include "GTAActionCallSerializer.h"
#include "GTAAppController.h"
#include "GTAICDParameter.h"
#include "GTAActionRelease.h"
#include "GTAActionReleaseSerializer.h"
#include "GTACheckAudioAlarm.h"
#include "GTACheckAudioSerializer.h"
#include "GTAOneClickLaunchApplication.h" 
#include "GTAOneClickAppLaunchSerializer.h"
#include "GTAActionRoboArm.h"
#include "GTAActionRoboArmSerializer.h"
#include "GTAInvalidCommandSerializer.h"
#include "GTAActionPrintTable.h"
#include "GTAActionPrintTableSerializer.h"
#include "GTAActionMathsSerializer.h"
#include "GTAActionMaths.h"
#include "GTAActionMath.h"
#include "GTAActionMathSerializer.h"
#include "GTAActionFTAKinematicMultiOutputSerializer.h"
#include "GTAActionFTAKinematicMultiOutput.h"
#include "GTAActionFCTLConf.h"
#include "GTAActionFCTLConfSerializer.h"
#include "GTAActionOneClickSetList.h"
#include "GTAActionOneClickSetListSerializer.h"
#include "GTAActionGenericFunction.h"
#include "GTAActionGenericFunctionSerializer.h"
#include "GTAActionOneClickTitle.h"
#include "GTAActionOneClickTitleSerializer.h"
#include "GTAActionFailure.h"
#include "GTAActionFailureSerializer.h"
#include "GTAActionCallProcedures.h"
#include "GTAActionCallProceduresSerializer.h"
#include "GTACheckRefresh.h"
#include "GTACheckRefreshSerializer.h"
#include "GTAActionPrintTime.h"
#include "GTAActionPrintTimeSerializer.h"
#include "GTAActionFunctionReturn.h"
#include "GTAActionFunctionReturnSerializer.h"
#include "GTAFactory.h"
#include "GTAGenericToolCmdSerializer.h"

#pragma warning(push, 0)
#include <QFile>
#include <stdlib.h>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QTextDocumentWriter>
#include <QTextCodec>
#include <QTextEdit>
#pragma warning(pop)

GTADocumentController::GTADocumentController()
{
}
bool GTADocumentController::saveDocument(const GTAElement * ipElement,const QString& sHeaderName, const QString & iDocFilePath,bool iOverwrite)
{
    bool rc = false;
	QString headerName = sHeaderName;
    if(ipElement != NULL)
    {
        GTAElement * pElement = (GTAElement*) ipElement;
        rc =  QFile::exists(iDocFilePath);
        if(rc && iOverwrite == false)
        {
            _LastError = QString("File %1 already exists in database ").arg(iDocFilePath);
            return false;
        }

        // For each type of file, XML attributes will be fixed
        qSetGlobalQHashSeed(42);

        QDomDocument DomDoc;
        //DomDoc.setContent(&FileObj);
        QDomProcessingInstruction procInstruct = DomDoc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"iso-8859-1\"");
        DomDoc.appendChild(procInstruct);

		QDomElement RootElem;
		rc = getRootElement(pElement, DomDoc, RootElem);

        if(rc && (! RootElem.isNull()))
        {
            //Program configuration not saved in document as per scrum 5.5
            /* QDomElement DomProgConfig;
            rc = getProgramConfig(pElement,DomDoc,DomProgConfig);
            if(rc && (!DomProgConfig.isNull()))
            {
                RootElem.appendChild(DomProgConfig);
            }
            else return false;*/

            QDomElement DomHeader;
            rc = getHeaderDom(pElement,DomDoc,DomHeader);
            if(rc && (! DomHeader.isNull()))
            {
                RootElem.appendChild(DomHeader);
                getFileInfoDom(DomDoc,pElement,DomHeader);
            }           
            else 
                return false;

            QDomElement purposeElement = DomDoc.createElement("PURPOSE");
            purposeElement.setAttribute("VALUE", pElement->getPurposeForProcedure());
            RootElem.appendChild(purposeElement);

            QDomElement conclusionElement = DomDoc.createElement("CONCLUSION");
            conclusionElement.setAttribute("VALUE", pElement->getConclusionForProcedure());
            RootElem.appendChild(conclusionElement);
        }
        else return false;

        QDomElement DomTagVariables;
        createTagVariables(DomDoc,DomTagVariables,pElement);
        RootElem.appendChild(DomTagVariables);

        QDomElement DomCommands = DomDoc.createElement(XNODE_COMMANDS);
        RootElem.appendChild(DomCommands);
        if(pElement->isReadOnly())
            DomCommands.setAttribute(XATTR_READ_ONLY,"TRUE");
        else
            DomCommands.setAttribute(XATTR_READ_ONLY,"FALSE");

        int totalCount = pElement->getDirectChildrenCount();
        int position = 0;
        QList<GTACommandBase*> listOfCmd;
        for(int i = 0; i < totalCount; i++)
        {

            GTACommandBase* pCmd =NULL;
            rc = pElement->getDirectChildren(i,pCmd);

            if(rc && pCmd != NULL)
            {
                listOfCmd.append(pCmd);
            }
        }
        createCommands(listOfCmd,DomDoc, DomCommands,position);

        QFile FileObj(iDocFilePath);
        rc = FileObj.open(QFile::WriteOnly | QFile::Text);
        if(! rc)
        {
            _LastError = QString("Unable to open file %1").arg(iDocFilePath);
            return rc;
        }


        DomDoc.appendChild(RootElem);
        QString contents = DomDoc.toString();
        QTextStream stream(&FileObj);
        stream <<contents;

        rc = FileObj.flush();
        if(! rc)
            _LastError = "Unable to write the file in database, please check you have enough permission and database has sufficient size";

        FileObj.close();
    }

    // reset hash seed with new random value.
    qSetGlobalQHashSeed(-1);

    return rc;
}
bool GTADocumentController::createCommands(const QList<GTACommandBase*> &iCmdList,QDomDocument & iDomDoc,QDomElement & iParentElem, int & GlobalPosition)
{
    bool rc = false;
    int childCount = iCmdList.count();
    for(int i = 0; i < childCount; i++)
    {
        //GlobalPosition += i; 
        GTACommandBase* pCmd = iCmdList.at(i);
        if(pCmd != NULL)
        {
            QDomElement domElement;
            rc = createCommand(pCmd,iDomDoc,domElement,GlobalPosition);
            /*GTAActionIF * pIfCmd = dynamic_cast<GTAActionIF*>(pCmd);*/
            //if(pIfCmd == NULL)
            //	GlobalPosition++;

            if(!iParentElem.isNull() && (!domElement.isNull()))
                iParentElem.appendChild(domElement);
        }

    }
    return rc;
}
bool GTADocumentController::createCommand(const GTACommandBase * pCmd , QDomDocument & iDomDoc, QDomElement & oElement, int & iGlobalPosition)
{
    bool rc = false;
    if(pCmd != NULL)
    {
        GTACommandBase::CommandType cmdType = pCmd->getCommandType();
        switch(cmdType)
        {
        case GTACommandBase::ACTION :
            {
                GTAActionBase * pActionCmd = dynamic_cast<GTAActionBase*>((GTACommandBase*)pCmd);
                if(pActionCmd != NULL)
                {
                    rc = createAction(pActionCmd,iDomDoc,oElement,iGlobalPosition);

                }
                break;
            }
        case GTACommandBase::CHECK: 

            {
                GTACheckBase * pChkCmnd = dynamic_cast<GTACheckBase*>((GTACommandBase*)pCmd);
                if(pChkCmnd != NULL)
                {
                    rc = createCheck(pChkCmnd,iDomDoc,oElement,iGlobalPosition);

                }
                break;
            }

        case GTACommandBase::COMMENT: break;
        }
    }
    return rc;
}
bool GTADocumentController::createAction(const GTAActionBase * pActionCmd , QDomDocument & iDomDoc , QDomElement & oElement, int & iGlobalPosition)
{
    bool rc=  false;
    if(pActionCmd == NULL)
        return rc;

    GTACmdSerializer * pSerializer = NULL;
    QString actionName =  pActionCmd->getAction();
    QString complementName = pActionCmd->getComplement();

    QString resigrtyName = actionName;
    /* if(!complementName.isEmpty())
        resigrtyName.append(QString(" %1").arg(complementName));*/

    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTACmdSerializerInterface* pSerializerItf = NULL;
    pFactory->create(resigrtyName,pSerializerItf);

    if (pSerializerItf==NULL)
    {
        if(!complementName.isEmpty())
        {
            resigrtyName.append(QString(" %1").arg(complementName));
            pFactory->create(resigrtyName,pSerializerItf);
        }
    }

    if(pSerializerItf!=NULL)
    {
        pSerializerItf->setCommand(pActionCmd);
        rc = pSerializerItf->serialize(iDomDoc,oElement);
        
        if(rc && (! oElement.isNull()))
        {
            oElement.setAttribute(XNODE_ORDER,QString::number(iGlobalPosition));
            iGlobalPosition++;

            if (pActionCmd->canHaveChildren())
            {
                QList<GTACommandBase*> listOfChild = pActionCmd->getChildren();
                QList<GTACommandBase*> childList;
                for(int i =0 ; i < listOfChild.count(); i++)
                {
                    childList.append(listOfChild.at(i));
                }
                rc = createCommands(childList,iDomDoc,oElement,iGlobalPosition);
            }
        }

        delete pSerializerItf;
        pSerializerItf=NULL;
    }

    if(pActionCmd->IsEndCommand())
        iGlobalPosition++;

    

    if(pSerializer != NULL)
    {
        delete pSerializer;
        pSerializer = NULL;
    }

    return rc;
}

bool GTADocumentController::createCheck(const GTACheckBase * pChkCmd , QDomDocument & iDomDoc , QDomElement & oElement, int & iGlobalPosition)
{
    bool rc=  false;
    if(pChkCmd == NULL)
        return rc;

    GTACmdSerializer * pSerializer = NULL;

    GTACheckBase::CheckType checkType = pChkCmd->getCheckType();
    QString CheckName = pChkCmd->getCheckName();

    GTAFactory* pFactory = &GTAFactory::getInstance();
    GTACmdSerializerInterface* pSerializerItf = NULL;
    pFactory->create(CheckName,pSerializerItf);

    if(pSerializerItf!=NULL)
    {
        pSerializerItf->setCommand(pChkCmd);
        rc = pSerializerItf->serialize(iDomDoc,oElement);

        if(rc && (! oElement.isNull()))
        {
            oElement.setAttribute(XNODE_ORDER,QString::number(iGlobalPosition));
            iGlobalPosition++;
        }

        delete pSerializerItf;
        pSerializerItf=NULL;
    }

    if (checkType == GTACheckBase::AUDIO_ALARM)
    {
        QString actionName =  CHK_AUDIO_ALRM;
        GTACheckAudioAlarm * pCmd = dynamic_cast<GTACheckAudioAlarm*>((GTACheckBase*)pChkCmd);
        if(pCmd != NULL)
        {
            pSerializer = new GTACheckAudioSerializer(pCmd);
            rc = pSerializer->serialize(iDomDoc,oElement);
            if(rc && (!oElement.isNull()))
            {
                oElement.setAttribute(XNODE_ORDER,QString::number(iGlobalPosition));
                iGlobalPosition++;
            }
        }
    }

    if(pSerializer != NULL)
    {
        delete pSerializer;
        pSerializer = NULL;
    }

    return rc;
}


bool GTADocumentController::getRootElement(GTAElement *& ipElement, QDomDocument & iDomDoc,QDomElement & oElement)
{
    bool rc = false;

    if(ipElement != NULL)
    {
        QString elementTypeS;
        GTAElement::ElemType elemType =  ipElement->getElementType();
        switch(elemType)
        {
        case GTAElement::OBJECT : elementTypeS = XNODE_OBJECT; break;
        case GTAElement::FUNCTION : elementTypeS = XNODE_FUNCTION; break;
        case GTAElement::PROCEDURE : elementTypeS = XNODE_PROCEDURE; break;
        case GTAElement::SEQUENCE : elementTypeS = XNODE_SEQUENCE; break;
        case GTAElement::TEMPLATE : elementTypeS = XNODE_TEMPLATE; break;
        }

        QString userName = getenv("USERNAME");
        // QString userName = _dupenv_s("USERNAME"); //todo: try using this for removing deprecation warining
        //#ifdef  OS_WINDOWS
        //        userName = getenv("USERNAME")
        //#else
        //        userName = getenv("USER");
        //#endif

        QString elemName = ipElement->getName();
        QString docID = ipElement->getUuid();
        QString relativePath = ipElement->getRelativeFilePath();
        QString gtaVersion = ipElement->getGtaVersion();
        QString authName = ipElement->getAuthorName();
        QString validatorName = ipElement->getValidatorName();
        QString validationTime = ipElement->getValidationTime();
        QString validationStatus = ipElement->getValidationStatus();
        QString creationTime = ipElement->getCreationTime();
        QString maxTimeEstimated = ipElement->getMaxTimeEstimated() + XNODE_MAX_TIME_ESTIMATED_UNIT;
        QString inputCsv = ipElement->getInputCsvFilePath();
        QString outputCsv = ipElement->getOutputCsvFilePath();

        oElement = iDomDoc.createElement(XNODE_MODEL);
        oElement.setAttribute(XNODE_NAME,elemName);
        oElement.setAttribute(XNODE_TYPE,elementTypeS);
        oElement.setAttribute(XNODE_CREATOR,userName);
        oElement.setAttribute(XNODE_DATE,QDateTime::currentDateTime().toString());
        oElement.setAttribute(XNODE_UUID,docID);
        oElement.setAttribute("PATH",relativePath);
        oElement.setAttribute(XNODE_GTA_VER,gtaVersion);
        oElement.setAttribute(XNODE_AUTHOR_NAME,authName);//V26 editor view author column enhancement #322480
        oElement.setAttribute(XNODE_VALIDATOR_NAME,validatorName);
        oElement.setAttribute(XNODE_VALIDATION_TIME,validationTime);
        oElement.setAttribute(XNODE_VALIDATION_STATUS,validationStatus);
        oElement.setAttribute(XNODE_CREATED_DATE,creationTime);
        oElement.setAttribute(XNODE_MAX_TIME_ESTIMATED, maxTimeEstimated);
        oElement.setAttribute(XNODE_INPUT_CSV, inputCsv);
        oElement.setAttribute(XNODE_OUTPUT_CSV, outputCsv);

        QStringList hiddenRows = ipElement->getHiddenRows();
        hiddenRows.removeDuplicates();
        if (hiddenRows.isEmpty()==false)
        {
            QDomNode viewPropertyNode = iDomDoc.createElement(XNODE_VIEW_PROPERTY);
            viewPropertyNode.toElement().setAttribute(XATTR_HIDDEN_ROWS,hiddenRows.join(","));
            oElement.appendChild(viewPropertyNode);

        }
        rc = true;
    }
    else _LastError = QString("GTADocumentController: Command is NULL");

    return rc;
}


bool GTADocumentController::getRootElement(const QDomElement & iDomElem, GTAElement *& opActionCmd)
{
    bool rc = false;
    if(! iDomElem.isNull() && opActionCmd != NULL )
    {
        QDomNamedNodeMap domAttr = iDomElem.attributes();
        if(domAttr.contains(XNODE_NAME) && domAttr.contains(XNODE_TYPE))
        {
            QString name = iDomElem.attribute(XNODE_NAME);
            QString typeValue = iDomElem.attribute(XNODE_TYPE);
            QString docID = iDomElem.attribute(XNODE_UUID);
            opActionCmd->setName(name);

            GTAElement::ElemType elemType = GTAElement::OBJECT;
            if(typeValue == XNODE_OBJECT)
                elemType = GTAElement::OBJECT;
            else if(typeValue == XNODE_PROCEDURE)
                elemType = GTAElement::PROCEDURE;
            else if(typeValue == XNODE_FUNCTION)
                elemType = GTAElement::FUNCTION;
            else if(typeValue == XNODE_SEQUENCE)
                elemType = GTAElement::SEQUENCE;
            else if(typeValue == XNODE_TEMPLATE)
                elemType = GTAElement::TEMPLATE;

            opActionCmd->setElementType(elemType);
            opActionCmd->setUuid(docID);
            rc = true;
        }
        _LastError = QString("GTADocumentController: invalide xml element");
    }
    _LastError = QString("GTADocumentController: Command is NULL");
    return rc;
}

bool GTADocumentController::getProgramConfig(GTAElement *& ipElement, QDomDocument &,QDomElement &)
{
    bool rc = false;
	
    if(ipElement != NULL)
    {
        GTAHeader * pHeader = ipElement->getHeader();
        if(pHeader != NULL)
        {
            /*QString programName = pHeader->getPogram();
            QString equipmentName=pHeader->getEquipment();
            QString standardName =pHeader->getStandard();

            oElement = iDomDoc.createElement(XNODE_CONFIG);
            oElement.setAttribute(XNODE_PROGRAM,programName);
            oElement.setAttribute(XNODE_EQUIPMENT,equipmentName);
            oElement.setAttribute(XNODE_STANDARD,standardName);*/
            rc = true;
        }
        else _LastError = QString("GTADocumentController: Header is not found or NULL");
    }
    else _LastError = QString("GTADocumentController: Command is NULL");
    return rc;
}

bool GTADocumentController::getProgramConfig(const QDomElement & iDomElem, GTAElement *& opElement)
{
    bool rc = false;
    if(! iDomElem.isNull() && opElement != NULL)
    {
        QString progName = iDomElem.attribute(XNODE_PROGRAM);
        QString equipmentName = iDomElem.attribute(XNODE_EQUIPMENT);
        QString standard = iDomElem.attribute(XNODE_STANDARD);
        rc = true;
    }
    //_LastError = QString("Error: GTADocumentController::getProgramConfig: method is not implemented");
    return rc;
}

bool GTADocumentController::getHeaderDom(GTAElement *& ipElement, QDomDocument & iDomDoc,QDomElement & oElement)
{
    bool rc = false;

    if(ipElement != NULL)
    {
        GTAHeader * pHeader = ipElement->getHeader();
        if(pHeader != NULL)
        {
            QString headerName = pHeader->getName();

            QStringList emptyManFields;
            rc = pHeader->mandatoryAttributeFilled(emptyManFields);
            if (!rc)
            {
                _LastError = QString("mandatory header fields empty:\n%1").arg(emptyManFields.join("\n"));
                return rc;
            }

            QDomElement rootElement = iDomDoc.createElement(XNODE_HEADER);
            rootElement.setAttribute(XNODE_NAME,pHeader->getName());
            pHeader->getDomElement(iDomDoc,rootElement);
            // dumpElemenToDebug(rootElement,"test");
            if (!rootElement.isNull())
            {
                QDomNode headerNodeImported = iDomDoc.importNode(rootElement,true);
                oElement=headerNodeImported.toElement();
                rc = true;
            }
        }
    }
    else
    {
        _LastError = QString("Element does not have header definition");
        return false;
    }
    return rc;
}
bool GTADocumentController::getFileInfoFromDoc(const QString& iDocFilePath, QList<GTAFILEInfo>& ioList )
{
    bool rc = false;

    QFile xmlFileObj(iDocFilePath);
    rc = xmlFileObj.open(QIODevice::Text| QFile::ReadOnly);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            rc = getFileInfoFromDoc(DomDoc,ioList);
        }
        DomDoc.clear();
        xmlFileObj.close();
    }
    else
    {
        _LastError=QString("Unable to open file %1").arg(iDocFilePath);
    }

    return rc;

}
bool GTADocumentController::getFileInfoFromDoc(const QDomDocument & iDomDoc, QList<GTAFILEInfo>& ioList )
{
    
    bool rc = false;
    if (!iDomDoc.isNull())
    {

        QDomNodeList lstOfHdrNodes = iDomDoc.elementsByTagName(XNODE_HEADER);
        
        bool isRelative = iDomDoc.elementsByTagName(XNODE_RELATIVE_PATH_STATUS).at(0).toElement().text() == "true";
        QDomNodeList lstOfForNodes = isRelative ? iDomDoc.elementsByTagName(XNODE_LIST_RELATIVE_PATH) : iDomDoc.elementsByTagName(XNODE_LIST_PATH) ;    //for each loop

        int size= lstOfHdrNodes.size();
        if (size==1)
        {
            QDomNode headerDomNode = lstOfHdrNodes.at(0);
            QDomElement headerElement = headerDomNode.toElement();
            QDomNodeList itemFileLst = headerElement.elementsByTagName(XNODE_FILE);

            int itemFileSize = itemFileLst.size();
            if (itemFileSize)
            {
                QStringList paramsDone;
                QHash<QString,GTAFILEInfo> icdInfo;
                for (int i=0;i<itemFileSize;i++)
                {
                    QDomNode itemNode = itemFileLst.at(i);
                    QDomElement itemElement = itemNode.toElement();
                    if (!itemElement.isNull())
                    {
                        QString sIcdNameNode = itemElement.attribute(XATTR_NAME);
                        QString sIcdTypeNode = itemElement.attribute(XATTR_TYPE);
                        QString sApplicationType = itemElement.attribute(XATTR_APP);
                        QString sNodeDescription = itemElement.attribute(XATTR_DESCRIPTION);
                        QString sMandatory = itemElement.attribute(XATTR_MANDATORY);
                        QString sEquipmentNode = itemElement.attribute(XATTR_EQUIPMENT);
                        QString sFilePathNode = itemElement.attribute(XATTR_PATH);

                        QHash <QString,QString> otherInfoMap;
                        otherInfoMap.insert(XATTR_APP,sApplicationType);
                        //QString sNodeToDisplay = QString("Applicatoin:%1,\nDescription:%2").arg()
                        bool bMandatory = false;
                        if (sMandatory.toUpper()=="1" || sMandatory.toUpper() == "TRUE")
                            bMandatory=true;
                        else
                            bMandatory=false;

                        if(paramsDone.contains(sIcdNameNode))
                        {
                            GTAFILEInfo fileInfoObj  = icdInfo.value(sIcdNameNode);
                            fileInfoObj._Application.append(sApplicationType);
                        }
                        else
                        {
                            GTAFILEInfo fileInfoObj;

                            fileInfoObj._Application.append(sApplicationType);
                            fileInfoObj._Equipment = sEquipmentNode;
                            fileInfoObj._filePath = sFilePathNode;
                            fileInfoObj._fileDescription = sNodeDescription;
                            fileInfoObj._filename = sIcdNameNode;
                            fileInfoObj._Mandatory = sMandatory;
                            fileInfoObj._fileType = sIcdTypeNode;
                            icdInfo.insert(sIcdNameNode,fileInfoObj);
                        }
                        //ioHeader->addHeaderItem(sNodeName,bMandatory,sNodeValue,sNodeDescription/*,otherInfoMap*/);

                    }
                }
                foreach(QString icdKey , icdInfo.keys())
                {
                    ioList.append(icdInfo.value(icdKey));
                }
                rc=true;
            }
			else
            {
                _LastError="No ICD/PMR/PIR file information found";

            }
            if(lstOfForNodes.size()>0)
            {
                // adding for each files into the BDB
                QHash<QString,GTAFILEInfo> forInfo;
                for (int i=0;i<lstOfForNodes.size();i++)
                {

                    QDomNode subHeaderDomNode = lstOfForNodes.at(i);
                    QDomElement itemElement = subHeaderDomNode.toElement();
                    QString filepath = itemElement.text();
                    QString initialpath = filepath;
                    QString TableDir = GTAAppController::getGTAAppController()->getGTATableDirectory(); //remove one of the two "TABLE"
                    int typeSepIndex = TableDir.lastIndexOf("/");
                    TableDir = TableDir.mid(0, typeSepIndex);

                    if (isRelative) {
                        filepath = QDir::cleanPath(TableDir + itemElement.text() );
                    }
                    
                    GTAFILEInfo fileInfoObj;
                    fileInfoObj._Application.append("");
                    fileInfoObj._Equipment = "";
                    fileInfoObj._filePath = filepath;
                    fileInfoObj._fileDescription = "";
                    fileInfoObj._filename = filepath.split("/",QString::SkipEmptyParts).last();
                    fileInfoObj._Mandatory = "FALSE";
                    fileInfoObj._fileType = TABLES_DIR;

                    forInfo.insert(fileInfoObj._filename,fileInfoObj);
                }
                foreach(QString forKey , forInfo.keys())
                {
                    ioList.append(forInfo.value(forKey));
                }
            }
            else
            {
                _LastError="No TABLE file information found";

            }

        }
        else
        {
            if(lstOfHdrNodes.isEmpty())
                _LastError = "No header information found";
            else
                _LastError = "Multiple Header node found";

        }

    }
    else
        _LastError = "Dom Document is empty";

    return rc;
}
bool GTADocumentController::getHeader(const QDomDocument & iDomDoc, GTAHeader *& ioHeader)
{
    bool rc = false;
    if (!iDomDoc.isNull())
    {
        QDomNodeList lstOfHdrNodes = iDomDoc.elementsByTagName(XNODE_HEADER);
        int size= lstOfHdrNodes.size();
        if (size==1)
        {
            QDomNode headerDomNode = lstOfHdrNodes.at(0);
            if(!headerDomNode.isNull())
            {
                QDomElement headerElement = headerDomNode.toElement();
                QString NodeName = headerElement.attribute(XATTR_NAME);
                ioHeader->setName(NodeName);
                QDomNodeList itemNodeLst = headerElement.elementsByTagName(XNODE_ITEM);
                int itemNodeSize = itemNodeLst.size();
                if (itemNodeSize)
                {
                    bool foundGTAVer = false;
                    for (int i=0;i<itemNodeSize;i++)
                    {
                        QDomNode itemNode = itemNodeLst.at(i);
                        QDomElement itemElement = itemNode.toElement();
                        if (!itemElement.isNull())
                        {
                            QString sNodeName = itemElement.attribute(XATTR_NAME);
							//finding if current header has GTA Version mentioned
                            if (sNodeName.compare(XNODE_GTA_VER) == 0)
                                foundGTAVer = true;
                            QString sNodeValue= itemElement.attribute(XATTR_VAL);
                            QString sNodeDescription = itemElement.attribute(XATTR_DESCRIPTION);
                            QString sMandatory = itemElement.attribute(XATTR_MANDATORY);
                            QString sShowInLTRA = itemElement.attribute(XATTR_SHOW_IN_LTRA);

                            bool bMandatory = false;
                            if (sMandatory.toUpper()=="1" || sMandatory.toUpper() == "TRUE")
                                bMandatory=true;
                            else
                                bMandatory=false;

                            bool bShowInLTRA = true;
                            if (!sShowInLTRA.isEmpty() && sShowInLTRA.toUpper() != "1" && sShowInLTRA.toUpper() != "TRUE")
                            {
                                bShowInLTRA = false;
                            }

                            ioHeader->addHeaderItem(sNodeName,bMandatory,sNodeValue,sNodeDescription, bShowInLTRA);
                        }
                    }
					//add a blank header field with GTA version name to be edited later by user or code
//                    if (!foundGTAVer)
//                    {
//                        ioHeader->addHeaderItem(XNODE_GTA_VER,true,QString(),QString());
//                    }
                    rc=true;

                }
                else
                {
                    rc = false;
                    _LastError="header node Empty";
                }

                //QDomNodeList itemFileLst = headerElement.elementsByTagName(XNODE_FILE);
                //int itemFileSize = itemFileLst.size();
                //if (itemFileSize)
                //{
                //    for (int i=0;i<itemFileSize;i++)
                //    {
                //        QDomNode itemNode = itemFileLst.at(i);
                //        QDomElement itemElement = itemNode.toElement();
                //        if (!itemElement.isNull())
                //        {
                //            QString sNodeName = itemElement.attribute(XATTR_NAME);
                //            QString sNodeValue= itemElement.attribute(XATTR_TYPE);
                //            QString sApplicationType = itemElement.attribute(XATTR_APP);
                //            QString sNodeDescription = itemElement.attribute(XATTR_DESCRIPTION);
                //            QString sMandatory = itemElement.attribute(XATTR_MANDATORY);

                //            QHash <QString,QString> otherInfoMap;
                //            otherInfoMap.insert(XATTR_APP,sApplicationType);
                //            //QString sNodeToDisplay = QString("Applicatoin:%1,\nDescription:%2").arg()
                //            bool bMandatory = false;
                //            if (sMandatory.toUpper()=="1" || sMandatory.toUpper() == "TRUE")
                //                bMandatory=true;
                //            else
                //                bMandatory=false;

                //            ioHeader->addHeaderItem(sNodeName,bMandatory,sNodeValue,sNodeDescription/*,otherInfoMap*/);

                //        }
                //    }
                //    rc=true;
                //}


            }

        }
        else
        {
            rc = false;
            if (lstOfHdrNodes.isEmpty())
                _LastError = "Header node not found";
            else
                _LastError = "Multiple Header node found";
        }
    }
    else
        _LastError = "Dom Document is empty";

    return rc;
}

bool GTADocumentController::saveDocument(const GTAHeader * ipHeader, const QString & iDocFilePath, bool iOverwrite )
{
    bool rc = false;
    if(ipHeader != NULL )
    {
        //bool mandatoryCheck = ipHeader->mandatoryAttributeFilled();
        //if (mandatoryCheck==false)
        //{
        //    _LastError = "Mandatory field for header is not filled";
        //    return false;
        //}
        
        QString hdrName = ipHeader->getName();
        if (!hdrName.isEmpty())
        {
            QDomDocument hdrDomDoc;
            QDomElement root = hdrDomDoc.createElement("HEADER");
            root.setAttribute(XATTR_NAME,hdrName);
            hdrDomDoc.appendChild(root);
            ipHeader->getDomElement(hdrDomDoc,root);
            //dumpElemenToDebug(root,"elemente test");
            if (!root.isNull())
            {
                QString contents = hdrDomDoc.toString();
                if(QFile(iDocFilePath).exists() && iOverwrite == false)
                    return rc;

                if(!contents.isEmpty())
                {
                    QFile FileObj(iDocFilePath);
                    FileObj.open(QFile::WriteOnly);
                    QTextStream writer(&FileObj);
                    rc = true;
                    writer <<contents;
                    FileObj.close();
                }
            }
            
        }
        else
        {
            _LastError = "Header cannot be saved with empty name";
            rc=false;

        }
        /*QDomDocument  pDomDoc = ipHeader->getDomElement();
        if(pDomDoc != NULL)
        {
            QString contents = pDomDoc->toString();
            if(QFile(iDocFilePath).exists() && iOverwrite == false)
                return rc;

            if(!contents.isEmpty())
            {
                QFile FileObj(iDocFilePath);
                FileObj.open(QFile::WriteOnly);
                QTextStream writer(&FileObj);
                rc = true;
                writer <<contents;
                FileObj.close();
            }
        }*/
    }
    return rc;
}
bool GTADocumentController::setDocReadOnlyState(const QString & iDocFilePath, bool bReadOnlyState)
{
    bool rc = false;

    QFile xmlFileObj(iDocFilePath);
    rc = xmlFileObj.open(QFile::Text| QFile::ReadWrite);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            QDomElement rootElement = DomDoc.documentElement();
            QDomNodeList commandNodeList = rootElement.elementsByTagName(XNODE_COMMANDS);
            if(commandNodeList.isEmpty()== false)
            {
                QDomElement commandNode = commandNodeList.at(0).toElement();
                if(bReadOnlyState)
                    commandNode.setAttribute(XATTR_READ_ONLY,"TRUE");
                else
                    commandNode.setAttribute(XATTR_READ_ONLY,"FALSE");
            }
            else
            {
                rc = false;
                _LastError = "No commands Found";
            }
        }
        else
        {
            rc = false;
            _LastError = "Could not read document";
        }

        xmlFileObj.close();
    }
    else
    {
        _LastError = "Could not open file";
    }


    return rc;
}

bool GTADocumentController::isDocumentValid(const QString & iDocFilePath)
{
    bool rc = false;
    QFile xmlFileObj(iDocFilePath);
    rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
    }
    return rc;
}


bool GTADocumentController::openDocument(const QString & iDocFilePath, GTAElement *& opElement,GTAHeader*& opHeader,bool ioOnlyModel)
{
    bool rc = false;

    QFile xmlFileObj(iDocFilePath);

    rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
    QByteArray data = xmlFileObj.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);


//    QTextStream readStream(xmlFileObj);
//    readStream.setCodec();

    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(str);
        if(rc)
        {
            QDomElement rootElement = DomDoc.documentElement();
            QString elementName = rootElement.attribute(XNODE_NAME);
            QString docType = rootElement.attribute(XNODE_TYPE);
            QString docID = rootElement.attribute(XNODE_UUID);
            QString path = rootElement.attribute("PATH");
            QString gtaVersion = rootElement.attribute(XNODE_GTA_VER);
            QString authorkaNaam = rootElement.attribute(XNODE_AUTHOR_NAME);
            QString validatorName = rootElement.attribute(XNODE_VALIDATOR_NAME);
            QString validationStatus = rootElement.attribute(XNODE_VALIDATION_STATUS);
            QString timeValidated = rootElement.attribute(XNODE_VALIDATION_TIME);
            QString creationDate = rootElement.attribute(XNODE_CREATED_DATE);
            QString modificationTime = rootElement.attribute(XNODE_DATE);
            QString maxTimeEstimated = rootElement.attribute(XNODE_MAX_TIME_ESTIMATED).replace(XNODE_MAX_TIME_ESTIMATED_UNIT,"");
            QString inputCsv = rootElement.attribute(XNODE_INPUT_CSV);
            QString outputCsv = rootElement.attribute(XNODE_OUTPUT_CSV);

            bool hasElement = true;
            GTAElement::ElemType elemType = GTAElement::UNKNOWN;
            if(docType  == XNODE_OBJECT)
                elemType = GTAElement::OBJECT;
            else if(docType  == XNODE_FUNCTION)
                elemType = GTAElement::FUNCTION;
            else if(docType  == XNODE_PROCEDURE)
                elemType = GTAElement::PROCEDURE;
            else if(docType  == XNODE_SEQUENCE)
                elemType = GTAElement::SEQUENCE;
            else if(docType  == XNODE_TEMPLATE)
                elemType = GTAElement::TEMPLATE;
            else
            {
                _LastError = "Does not contain Element data";
                hasElement=false;
            }

            if (hasElement)
            {
                opElement = new GTAElement(elemType);
                opElement->setName(elementName);
                opElement->setUuid(docID);
                opElement->setGtaVersion(gtaVersion);

                if(!authorkaNaam.isEmpty())
                    opElement->setAuthorName(authorkaNaam);
                else
                    opElement->setAuthorName("NA");

                if(!creationDate.isEmpty())
                    opElement->setCreationTime(creationDate);
                else
                    opElement->setCreationTime(QString());

                if(!validatorName.isEmpty())
                    opElement->setValidatorName(validatorName);
                else
                    opElement->setValidatorName("NA");

                if(!validationStatus.isEmpty())
                    opElement->setValidationStatus(validationStatus);
                else
                    opElement->setValidationStatus(XNODE_NOT_VALIDATED);

                if(!timeValidated.isEmpty())
                    opElement->setValidationTime(timeValidated);
                else
                    opElement->setValidationTime("NA");

                if(!modificationTime.isEmpty())
                    opElement->setModifiedTime(modificationTime);
                else
                    opElement->setModifiedTime(QString());

                if (!maxTimeEstimated.isEmpty())
                    opElement->setMaxTimeEstimated(maxTimeEstimated);
                else
                    opElement->setMaxTimeEstimated("");

                if (!inputCsv.isEmpty())
                    opElement->setInputCsvFilePath(inputCsv);
                else
                    opElement->setInputCsvFilePath("");

                if (!outputCsv.isEmpty())
                    opElement->setOutputCsvFilePath(outputCsv);
                else
                    opElement->setOutputCsvFilePath("");


                if(!path.isEmpty())
                    opElement->setRelativeFilePath(path);
                else
                {
                    GTAAppController *pAppController = GTAAppController::getGTAAppController();
                    if(pAppController)
                    {
                        QString gtaDirPath = pAppController->getGTADataDirectory();
                        QString relativePath = iDocFilePath;
                        relativePath.replace(gtaDirPath,"") ;
                        opElement->setRelativeFilePath(QDir::cleanPath(relativePath));
                    }
                }

                QDomNodeList cmdNodeList = rootElement.elementsByTagName(XNODE_COMMANDS);
                int childCount = cmdNodeList.count();
                if(childCount >0 )
                {
                    QDomElement cmdElement = cmdNodeList.at(0).toElement();
                    if (cmdElement.attribute(XATTR_READ_ONLY).toUpper() == "TRUE")
                        opElement->setSaveAsReadOnly(true);
                    else
                        opElement->setSaveAsReadOnly(false);
                    loadCommand(cmdElement,opElement);
                }

                cmdNodeList = rootElement.elementsByTagName(XNODE_VIEW_PROPERTY);
                childCount = cmdNodeList.count();
                if(childCount >0 )
                {
                    QDomElement cmdElement = cmdNodeList.at(0).toElement();
                    QString hiddenNodeInfo = cmdElement.attribute(XATTR_HIDDEN_ROWS);
                    if(hiddenNodeInfo.isEmpty()==false)
                    {
                        QStringList hiddenNodes = hiddenNodeInfo.split(",");
                        if (opElement!=NULL && hiddenNodes.isEmpty()==false)
                        {
                            opElement->setHiddenRows(hiddenNodes);
                        }
                    }
                }

                if (rootElement.elementsByTagName("PURPOSE").count() > 0)
                {
                    QString purpose = rootElement.elementsByTagName("PURPOSE").at(0).toElement().attribute("VALUE");
                    opElement->setPurposeForProcedure(purpose);
                }
                
                if (rootElement.elementsByTagName("CONCLUSION").count() > 0) 
                {
                    QString conclusion = rootElement.elementsByTagName("CONCLUSION").at(0).toElement().attribute("VALUE");
                    opElement->setConclusionForProcedure(conclusion);
                }     
            }
            QDomNodeList tagNodeList = rootElement.elementsByTagName(XNODE_TAGVARIABLES);
            QDomNode tagDomNode = tagNodeList.at(0);
            QDomElement tagElement = tagDomNode.toElement();
            QDomNodeList tagVarNodeList = tagElement.elementsByTagName(XNODE_TAGVARIABLE);

            QList<TagVariablesDesc> tagVarList;
            if(tagVarNodeList.count() > 0)
            {
                for(int i=0 ; i<tagVarNodeList.count() ;i++)
                {
                    QDomElement tagVarElement = tagVarNodeList.at(i).toElement();
                    TagVariablesDesc tag;
                    tag.name = tagVarElement.attribute(XATTR_NAME);
                    tag.type = tagVarElement.attribute(XATTR_TYPE);
                    tag.unit = tagVarElement.attribute(XATTR_UNIT);
                    tag.supportedValue = tagVarElement.attribute(XATTR_VAL);
                    tag.min = tagVarElement.attribute(XATTR_MIN);
                    tag.max = tagVarElement.attribute(XATTR_MAX);
                    tag.Desc = tagVarElement.attribute(XATTR_DESCRIPTION);

                    tagVarList.append(tag);
                }
                opElement->setTagVarStructList(tagVarList);
            }
            if (ioOnlyModel == false)
            {
                opHeader=new GTAHeader;

                bool rHd = getHeader(DomDoc,opHeader);
                if (!rHd)
                {
                    delete opHeader;opHeader=NULL;
                }
				//check if the GTA Version in header is empty
				//update the field with relevant GTA Version value from the element
                /*if (opHeader->getGTAVersion().isEmpty())
                {
                    opHeader->setGTAVersion(gtaVersion);
                }*/
                
            }
        }
        else 
            _LastError = QString("Document content not readable %1").arg(iDocFilePath);
        xmlFileObj.close();
    }
    else _LastError = QString("Unable to open document %1").arg(iDocFilePath);

    return rc;
}

bool GTADocumentController::loadCommand(const QDomElement & iCmdNode, GTAElement *& iopElement)
{
    bool rc = false;
    if(iopElement == NULL)
    {
        _LastError = "Element is NULL";
        return rc;
    }

    QDomNodeList childNodeList = iCmdNode.childNodes();
    //int childCount = childNodeList.count();
    GTACommandBase * pRootCmd= NULL;
    int	orderChange =0;
    BuildElement(childNodeList,iopElement,pRootCmd,orderChange);
    return rc;
}
bool GTADocumentController::BuildElement(const QDomNodeList & iCmdNodeList, GTAElement *& iopElement, GTACommandBase *& pParent ,int& ioOrderChange )
{
    bool rc = false;
    int childCount = iCmdNodeList.count();

    for(int i = 0; i < childCount; i++)
    {
        QDomNode childNode = iCmdNodeList.at(i);
        if(childNode.isElement() && childNode.nodeName() == XNODE_ACTION)
        {
            QDomElement domActElem = childNode.toElement();
            int position = i;
            rc = loadAction(domActElem,iopElement,position,pParent,ioOrderChange);

        }
        else if(childNode.isElement() && childNode.nodeName() == XNODE_CHECK)
        {
            QDomElement domActElem = childNode.toElement();
            int position = i;
            rc = loadCheck(domActElem,iopElement,position,pParent,ioOrderChange);

        }
    }
    return rc;
}
/**
 * @brief Loads an action into the provided GTAElement based on the QDomElement input.
 *
 * This function parses the QDomElement representing an action and creates an appropriate
 * GTACommandBase command. It then inserts this command into the given GTAElement at the specified position.
 * Particular attention is paid to actions involving 'START_RECORD' and 'STOP_RECORD', where additional
 * configuration is applied.
 *
 * @param iActionElem The QDomElement representing the action to be loaded.
 * @param iopElement Reference to a pointer to the GTAElement where the command will be inserted.
 *        This element may be modified by the function.
 * @param ioPosition Reference to the insertion position within iopElement. This value may be updated by the function.
 * @param ioOrderChange Reference to an integer that might be modified to reflect any order changes in the processing.
 *
 * @return bool True if the action is successfully loaded and inserted into iopElement; otherwise, false.
 *
 * @note If 'START_RECORD' or 'STOP_RECORD' is encountered within a GTAGenericToolCommand,
 *       the function performs additional steps:
 *       - It configures the tool name and display name for the command based on the current TestConfig instance.
 *       - It updates the tool ID for the first function in the command's function list, if such a list exists.
 *       - It checks for any mismatch in the tool names and functions, setting a mismatch flag if needed.
 */
bool GTADocumentController::loadAction(const QDomElement &iActionElem, GTAElement *&iopElement, int &ioPosition, GTACommandBase *&, int& ioOrderChange)
{
    bool rc = false;
    if(iopElement == NULL)
        return rc;

    if(! iActionElem.isNull())
    {
        // GTACmdSerializer * pSerializer = NULL;
        GTACommandBase *pCommand = NULL;

        QString actionName = iActionElem.attribute(XNODE_NAME);
        QString complimentName = iActionElem.attribute(XNODE_COMPLEMENT);
        ioPosition = iActionElem.attribute(XNODE_ORDER).toInt();

        QString resigrtyName = actionName;
        /*if(!complimentName.isEmpty())
            resigrtyName.append(QString(" %1").arg(complimentName));*/

        GTAFactory* pFactory = &GTAFactory::getInstance();
        GTACmdSerializerInterface* pSerializerItf = NULL;
        pFactory->create(resigrtyName,pSerializerItf);


        if (pSerializerItf==NULL)
        {
            if(!complimentName.isEmpty())
            {
                resigrtyName.append(QString(" %1").arg(complimentName));
                pFactory->create(resigrtyName,pSerializerItf);
            }
        }

        if(pSerializerItf!=NULL)
        {
            
            rc =pSerializerItf->deserialize(iActionElem,pCommand);
            if(rc && pCommand != NULL)
            {
                GTAGenericToolCommand *pGenToolCmd = dynamic_cast<GTAGenericToolCommand*>(pCommand);
                if(pGenToolCmd)
                {
                    SyncReturnInExternalFunction(pGenToolCmd);
                    if (pGenToolCmd->getStatement().contains(START_RECORD) || pGenToolCmd->getStatement().contains(STOP_RECORD))
                    {
                        TestConfig* testConf = TestConfig::getInstance();
                        QString toolName = QString::fromStdString(testConf->getRecordingToolName());

                        // Check if toolName is not empty and not null before proceeding
                        if (!toolName.isEmpty() && !toolName.isNull())
                        {
                            pGenToolCmd->setToolName(toolName);
                            pGenToolCmd->setToolDisplayName(toolName);

                            QList<GTAGenericFunction>& selectedFunctions = pGenToolCmd->getFunctions();
                            if (!selectedFunctions.isEmpty()) {
                                GTAGenericFunction& firstFunction = selectedFunctions[0];
                                firstFunction.setToolID(toolName);
                            }

                            pGenToolCmd->setFunctions(selectedFunctions);
                        }
                    }

                    bool retVal = checkForMismatch(pGenToolCmd->getToolName(),pGenToolCmd->getFunctions());
                    if(retVal)
                    {
                        pGenToolCmd->setMismatch(true);
                        if(pCommand)
                        {
                            pCommand = NULL;
                            pCommand = pGenToolCmd;
                        }
                    }
                }

                rc = iopElement->insertCommand(pCommand,ioPosition,false);
                if (pCommand->canHaveChildren())
                {
                    QDomNodeList childCmdList = iActionElem.childNodes();
                    BuildElement(childCmdList,iopElement,pCommand,ioOrderChange);

                    if (pCommand->isMutuallExclusiveStatement() == false)
                    {
                        GTACommandBase* pEndCmd = NULL;
                        bool eRC = pCommand->createEndCommand(pEndCmd);
                        if (eRC==true && pEndCmd!=NULL)
                        {
                            int endPosition = pCommand->getChildren().count();
                            pCommand->insertChildren(pEndCmd,endPosition);

                        }
                    }
                }
            }

            delete pSerializerItf;
            pSerializerItf=NULL;
        }
    }
    return rc;
}

/**
 * @brief : Sync the return parameter values of the external tools function 
 *          of the saved procedure with the current values in the external tools config files.
 *
 * @param pGenToolCmd: Generic tool command object
 */
void GTADocumentController::SyncReturnInExternalFunction(GTAGenericToolCommand *& pGenToolCmd)
{
    GTAAppController* pAppController = GTAAppController::getGTAAppController();

    if (pAppController)
    {
        QList<GTAGenericFunction> currentFunctions = pGenToolCmd->getFunctions();
        QList<GTAGenericFunction> UpdatedFunctions;

        pAppController->getAllExternalToolNames();
        QList<GTAGenericFunction> AllExternalToolFunctions = pAppController->getAllExternalToolFunctionNames(pGenToolCmd->getToolName());

        for each (GTAGenericFunction GenericFunc in currentFunctions)
        {
            if (GenericFunc.getReturnParamName().isEmpty() && GenericFunc.getReturnParamType().isEmpty())
            {
                for each (GTAGenericFunction tempFunction in AllExternalToolFunctions)
                {
                    if (tempFunction.getFunctionName() == GenericFunc.getFunctionName())
                    {
                        GenericFunc.setReturnParamName(tempFunction.getReturnParamName());
                        GenericFunc.setReturnParamType(tempFunction.getReturnParamType());
                    }
                }
            }

            UpdatedFunctions.push_back(GenericFunc);
        }

        pGenToolCmd->setFunctions(UpdatedFunctions);
    }
}

bool GTADocumentController::loadCheck(const QDomElement &iActionElem, GTAElement *&iopElement, int &ioPosition, GTACommandBase *&, int&)
{
    bool rc = false;
    if(iopElement == NULL)
        return rc;

    if(! iActionElem.isNull())
    {
        GTACmdSerializer * pSerializer = NULL;
        GTACommandBase *pCommand = NULL;

        QString checkName = iActionElem.attribute(XNODE_NAME);
        ioPosition = iActionElem.attribute(XNODE_ORDER).toInt();

        GTAFactory* pFactory = &GTAFactory::getInstance();
        GTACmdSerializerInterface* pSerializerItf = NULL;
        pFactory->create(checkName,pSerializerItf);

        if(pSerializerItf!=NULL)
        {
            rc =pSerializerItf->deserialize(iActionElem,pCommand);
            if(rc && pCommand != NULL)
            {
                rc = iopElement->insertCommand(pCommand,ioPosition,false);
            }

            delete pSerializerItf;
            pSerializerItf=NULL;
        }

        if (checkName == CHK_AUDIO_ALRM)
        {
            pSerializer = new GTACheckAudioSerializer(NULL);
            rc = pSerializer->deserialize(iActionElem,pCommand);

            if(rc && pCommand != NULL)
            {
                rc = iopElement->insertCommand(pCommand,ioPosition,false);
            }
			if (NULL != pSerializer)
			{
				delete pSerializer;
				pSerializer = NULL;
			}
        }
        
    }
    return rc;
}

bool GTADocumentController::checkForMismatch(const QString &iToolName, const QList<GTAGenericFunction> &iFunctionList)
{
    bool isMismatched = false;
    GTAAppController *pAppController = GTAAppController::getGTAAppController();
    if(pAppController)
    {
        pAppController->getAllExternalToolNames();
        QList<GTAGenericFunction> functions = pAppController->getAllExternalToolFunctionNames(iToolName);
        for(int i = 0; i < iFunctionList.count(); i++)
        {
            GTAGenericFunction cmdFunc = iFunctionList.at(i);

            if(functions.contains(cmdFunc))
            {
                int index = functions.indexOf(cmdFunc);
                int lastindex = functions.lastIndexOf(cmdFunc);
                if (index != lastindex)
                {
                    index = getGenListIndex(functions,cmdFunc);
                }
                if(index == -1)
                {
                    isMismatched = true;
                    _LastError = "The Functions do not match";
                }
                if(index >= 0 && index < functions.count())
                {
                    GTAGenericFunction checkFunc = functions.at(index);
                    if(cmdFunc != checkFunc)
                    {
                        isMismatched = true;
                        break;
                    }
                }
            }
            else
            {
                isMismatched = true;
                break;
            }

        }

    }
    return isMismatched;
}

int GTADocumentController::getGenListIndex(QList<GTAGenericFunction> &functions, GTAGenericFunction &cmdFunc)
{
    for (int i=0;i<functions.count();i++)
    {
        GTAGenericFunction Func = functions.at(i);
        if ((Func.getToolID() == cmdFunc.getToolID()) && (Func.getFunctionName() == cmdFunc.getFunctionName()) && (Func.getToolType() == cmdFunc.getToolType()))
            return i;
    }
    return -1;
}

/**
  * This function return the document name from provided document path
  */
QString GTADocumentController::getDocumentName(const QString & iDocFileFullPath)
{
    QString docName;
    QFile xmlFileObj(iDocFileFullPath);
    bool rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            QDomElement rootElement = DomDoc.documentElement();
            docName = rootElement.attribute("NAME");
        }
    }
    return docName;
}

void GTADocumentController::setDocumentPath(const QString & iDocFileFullPath, const QString &iRelFilePath)
{

    QFile xmlFileObj(QDir::cleanPath(iDocFileFullPath));
    bool rc = xmlFileObj.open(QFile::Text| QFile::ReadWrite);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            xmlFileObj.close();
            QDomElement rootElement = DomDoc.documentElement();
            QString path = rootElement.attributes().namedItem("PATH").nodeValue();
            if(path != iRelFilePath)
            {
                QFile xmlNewFileObj(iDocFileFullPath);
                bool rC = xmlNewFileObj.open(QFile::Text | QFile::ReadWrite | QFile::Truncate);
                if(rC)
                {

                    rootElement.attributes().namedItem("PATH").setNodeValue(iRelFilePath);
                    QTextStream stream(&xmlNewFileObj);
                    stream << DomDoc.toString();
                    xmlNewFileObj.close();
                }
            }
        }
    }
}

bool GTADocumentController::setDocumentUUID(const QString & iAbsoluteFilePath,const QString & iUUID)
{
    QFile xmlFileObj(QDir::cleanPath(iAbsoluteFilePath));
    bool rc = xmlFileObj.open(QFile::Text| QFile::ReadWrite);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            xmlFileObj.close();
            QFile xmlNewFileObj(iAbsoluteFilePath);
            bool rC = xmlNewFileObj.open(QFile::Text | QFile::ReadWrite | QFile::Truncate);
            if(rC)
            {
                QDomElement rootElement = DomDoc.documentElement();
                rootElement.attributes().namedItem(XNODE_UUID).setNodeValue(iUUID);
                QTextStream stream(&xmlNewFileObj);
                stream << DomDoc.toString();
                xmlNewFileObj.close();
            }
        }
    }
    return rc;
}

void GTADocumentController::setDocumentName(const QString & iDocFileFullPath,
                                               const QString &iRelativeFilePath,
                                               const QString &iName)
{

    QFile xmlFileObj(QDir::cleanPath(iDocFileFullPath));
    bool rc = xmlFileObj.open(QFile::Text| QFile::ReadWrite);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            xmlFileObj.close();
            QFile xmlNewFileObj(iDocFileFullPath);
            bool rC = xmlNewFileObj.open(QFile::Text | QFile::ReadWrite | QFile::Truncate);
            if(rC)
            {
                QDomElement rootElement = DomDoc.documentElement();
                rootElement.attributes().namedItem("NAME").setNodeValue(iName);
                rootElement.attributes().namedItem("PATH").setNodeValue(iRelativeFilePath);
                QTextStream stream(&xmlNewFileObj);
                stream << DomDoc.toString();
                xmlNewFileObj.close();
            }

        }
    }
}


/**
 * @brief This function returns the document info from provided document path i.e. UUID, Created date, Author name, modified date, validation time, validation status, validator name, maximum time estimated
 * @param iDocFileFullPath : Full path of the document
 * @param oUUID : UUID of the document
 * @param oDateModified : Modified date of the document
 * @param oAuthorName : Author name of the document
 * @param oValidatorName : Validator name of the document
 * @param oValidationStatus : Validation status of the document
 * @param oValidationTime : Validation time of the document
 * @param oCreationDate : Creation date of the document
 * @param oGtaVersion : GTA creation version of the document
 * @param oMaxTimeEstimated : Maximum Time Estimated of the document
 * @param oInputCsv : Input Csv File of the document (if applied)
 * @param oOutputCsv : Output Csv File of the document (if applied)
*/
void GTADocumentController::getDocumentInfo(const QString & iDocFileFullPath,
    QString &oUUID,
    QString &oDateModified,
    QString &oAuthorName,
    QString &oValidatorName,
    QString &oValidationStatus,
    QString &oValidationTime,
    QString &oCreationDate,
    QString &oGtaVersion,
    QString &oMaxTimeEstimated,
    QString &oInputCsv,
    QString &oOutputCsv)
{
    QFile xmlFileObj(iDocFileFullPath);
    bool rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            QDomElement rootElement = DomDoc.documentElement();
            oUUID = rootElement.attribute(XNODE_UUID);
            oDateModified = rootElement.attribute(XNODE_DATE);
            oAuthorName = rootElement.attribute(XNODE_AUTHOR_NAME);
            oValidatorName = rootElement.attribute(XNODE_VALIDATOR_NAME);
            oValidationStatus = rootElement.attribute(XNODE_VALIDATION_STATUS);
            oValidationTime = rootElement.attribute(XNODE_VALIDATION_TIME);
            oCreationDate = rootElement.attribute(XNODE_CREATED_DATE);
            oGtaVersion = rootElement.attribute(XNODE_GTA_VER);
            oMaxTimeEstimated = rootElement.attribute(XNODE_MAX_TIME_ESTIMATED);
            oInputCsv = rootElement.attribute(XNODE_INPUT_CSV);
            oOutputCsv = rootElement.attribute(XNODE_OUTPUT_CSV);
        }
		DomDoc.clear();
        xmlFileObj.close();
    }
}


QString GTADocumentController::getDescriptionFromHeader(const QString & iDocFileFullPath)
{
    QString desc;
    QFile xmlFileObj(iDocFileFullPath);
    bool rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);
        if(rc)
        {
            QDomNodeList headerNodeLst = DomDoc.elementsByTagName(XNODE_HEADER);
            QDomNodeList headerItemList = headerNodeLst.at(0).childNodes();

            for(int i = 0; i < headerItemList.count(); i++)
            {
                QDomNode headerItemNode = headerItemList.at(i);
                QString val = headerItemNode.attributes().namedItem("NAME").nodeValue();
                if(val.compare("DESCRIPTION",Qt::CaseInsensitive) == 0)
                {
                    desc = headerItemNode.attributes().namedItem("VALUE").nodeValue();
                    break;
                }
            }

        }
    }
    xmlFileObj.close();
    return desc;
}

/**
* @brief Sets the validation status, author and date of the document.
* @param docFileFullPath Path to the document.
* @param validationStatus Status: validated or not.
* @param validatorName Author of the validation.
* @param validationTime Date and time of validation.
*/
void GTADocumentController::setValidationInfo(const QString& docFileFullPath, const QString& validationStatus, const QString& validatorName,  const QString& validationTime) const
{
    QFile xmlFileObj(QDir::cleanPath(docFileFullPath));
    if (xmlFileObj.open(QFile::Text | QFile::ReadWrite))
    {
        QDomDocument DomDoc;
        if (DomDoc.setContent(&xmlFileObj))
        {
            xmlFileObj.close();
            QFile xmlNewFileObj(docFileFullPath);
            if (xmlNewFileObj.open(QFile::Text | QFile::ReadWrite | QFile::Truncate))
            {
                QDomElement rootElement = DomDoc.documentElement();
                rootElement.setAttribute(XNODE_VALIDATION_STATUS, validationStatus);
                rootElement.setAttribute(XNODE_VALIDATOR_NAME, validatorName);
                rootElement.setAttribute(XNODE_VALIDATION_TIME, validationTime);

                QTextStream stream(&xmlNewFileObj);
                stream << DomDoc.toString();
                xmlNewFileObj.close();
            }
        }
    }
}

/**
 * @brief This function returns the document description and info from provided document path
 * @param iDocFileFullPath : Full path of the document
 * @param oUUID : UUID of the document
 * @param oDateModified : Modified date of the document
  * @param oAuthorName : Author name of the document
 * @param oValidatorName : Validator name of the document
 * @param oValidationStatus : Validation status of the document
 * @param oValidationTime : Validation time of the document
 * @param oCreationDate : Creation date of the document
 * @param oDescription : Description of the document
 * @param oGtaVersion : GTA creation version of the document
 * @param oMaxTimeEstimated : Maximum Time Estimated of the document
 * @param oInputCsv : Input Csv File of the document (if applied)
 * @param oOutputCsv : Output Csv File of the document (if applied)
*/
void GTADocumentController::getDescAndInfo(const QString & iDocFileFullPath,
    QString &oUUID,
    QString &oDateModified,
    QString &oAuthorName,
    QString &oValidatorName,
    QString &oValidationStatus,
    QString &oValidationTime,
    QString &oCreationDate,
    QString &oDescription,
    QString &oGtaVersion,
    QString &oMaxTimeEstimated,
    QString &oInputCsv,
    QString& oOutputCsv)
{
    QString desc;
    QFile xmlFileObj(iDocFileFullPath);
    if(xmlFileObj.open(QFile::Text| QFile::ReadOnly))
    {
        QDomDocument DomDoc;
        if(DomDoc.setContent(&xmlFileObj))
        {
            QDomElement rootElement = DomDoc.documentElement();
            oUUID = rootElement.attribute(XNODE_UUID);
            oDateModified = rootElement.attribute(XNODE_DATE);
            oAuthorName = rootElement.attribute(XNODE_AUTHOR_NAME);
            oValidatorName = rootElement.attribute(XNODE_VALIDATOR_NAME);
            oValidationStatus = rootElement.attribute(XNODE_VALIDATION_STATUS);
            oValidationTime = rootElement.attribute(XNODE_VALIDATION_TIME);
            oCreationDate = rootElement.attribute(XNODE_CREATED_DATE);
            oGtaVersion = rootElement.attribute(XNODE_GTA_VER);
            oMaxTimeEstimated = rootElement.attribute(XNODE_MAX_TIME_ESTIMATED);
            oInputCsv = rootElement.attribute(XNODE_INPUT_CSV);
            oOutputCsv = rootElement.attribute(XNODE_OUTPUT_CSV);
            QDomNodeList headerNodeLst = DomDoc.elementsByTagName(XNODE_HEADER);
            QDomNodeList headerItemList = headerNodeLst.at(0).childNodes();

            for(int i = 0; i < headerItemList.count(); i++)
            {
                QDomNode headerItemNode = headerItemList.at(i);
                QString val = headerItemNode.attributes().namedItem("NAME").nodeValue();
                if(val.compare("DESCRIPTION",Qt::CaseInsensitive) == 0)
                {
                    oDescription = headerItemNode.attributes().namedItem("VALUE").nodeValue();
                    break;
                }
            }
        }
        DomDoc.clear();
        xmlFileObj.close();
    }
}

bool GTADocumentController::getHeaderFromDoc(const QString& iDocPath, GTAHeader*& ioHeader)
{
    bool rc = false;
    ioHeader=NULL;
    QFile xmlFileObj(iDocPath);
    rc = xmlFileObj.open(QFile::Text| QFile::ReadOnly);
    if(rc)
    {
        QDomDocument DomDoc;
        rc = DomDoc.setContent(&xmlFileObj);   
        if (rc)
        {
            rc = getHeader(DomDoc,ioHeader);
        }
        xmlFileObj.close();
    }
    if (!rc)
    {
        if(NULL!=ioHeader)
        {delete ioHeader;ioHeader=NULL;}
    }
    return rc;
}

bool GTADocumentController::getFileInfoDom( QDomDocument& iRefDoc, GTAElement* pElement, QDomElement& aggregatingElem)
{
    bool rc = false;
    if (NULL!=pElement)
    {
        QStringList chkLstForRecursion;
        QStringList paramList = getAllVariableList(pElement,chkLstForRecursion);//pElement->getParameterList();
        QStringList lstTripletParam;
        for (int i=0;i<paramList.size();i++)
        {
            QString parameter = paramList.at(i);
            QStringList lstOfTriplet = parameter.split(".");
            if (lstOfTriplet.size()==3) // will skip non triplets
            {
                lstTripletParam.append(parameter);
            }
        }

        if (!lstTripletParam.isEmpty())
        {
            GTAAppController* pAppCtrlr= GTAAppController::getGTAAppController();
            if(pAppCtrlr)
            {
                QList<GTAICDParameter> parameterList;
                lstTripletParam.removeDuplicates();
                rc = pAppCtrlr->getICDDetails(lstTripletParam,parameterList);
                QMap<QString,GTAICDParameter> uniqueParameterPaths;
                foreach(GTAICDParameter icdParam, parameterList)
                {
                    uniqueParameterPaths.insert(icdParam.getSourceFile(),icdParam);
                }
                parameterList.clear();
                parameterList.append(uniqueParameterPaths.values());
//                foreach(QString key,uniqueParameterPaths.keys() )
//                {
//                    parameterList.append(uniqueParameterPaths.value(key));
//                }
                if(!parameterList.isEmpty())
                {
                    foreach(GTAICDParameter icdParam, parameterList)
                    {
                        QString sName = "";
                        QString sEquip = icdParam.getEquipmentName();

                        if(sEquip.isEmpty())
                            sEquip = icdParam.getSourceType();

                        QString sApplication = icdParam.getApplicationName();
                        QString sfile = icdParam.getSourceFile();
                        QFileInfo info(sfile);
                        sName  = info.fileName();
                        QString sMandatory = "FALSE";
                        QString sDescritpion = "Icd name used for the corresponding application";
                        QString sType = icdParam.getSourceType();
                        QDomElement fileElement = iRefDoc.createElement(XNODE_FILE);
                        fileElement.setAttribute(XATTR_TYPE,sType);
                        if(sfile.contains(pAppCtrlr->getGTALibraryPath()))
                            sfile.remove(pAppCtrlr->getGTALibraryPath());
                        fileElement.setAttribute(XATTR_PATH, sfile);
                        fileElement.setAttribute(XATTR_NAME, sName);
                        fileElement.setAttribute(XATTR_APP, sApplication);
                        fileElement.setAttribute(XATTR_EQUIPMENT, sEquip);
                        fileElement.setAttribute(XATTR_MANDATORY,sMandatory);
                        fileElement.setAttribute(XATTR_DESCRIPTION,sDescritpion);


                        aggregatingElem.appendChild(fileElement);

                    }
                }
            }
        }
        
    }
    return rc;
}

/**
  * This function return the list of variables used in all commands of the element, including reference of other document
  * @pElem: Element for which the variables to be retrieved
  */
QStringList GTADocumentController::getAllVariableList(const GTAElement * ipElem,QStringList& ioreferedItems, bool listReleaseVar) const
{
    QStringList oVarList;
    GTAElement * pElem = (GTAElement*)ipElem;
    if(pElem != NULL)
    {


        int cmdCount = pElem->getAllChildrenCount();
        for(int i= 0; i< cmdCount; i++)
        {
            GTACommandBase * pCmd =NULL;
            pElem->getCommand(i,pCmd);
            if(pCmd != NULL)
            {
                if(pCmd->hasReference())
                {
                    GTAActionCall * pCallCmd = dynamic_cast<GTAActionCall*>(pCmd);
                    if(pCallCmd != NULL)
                    {
                        GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
                        if(pAppCtrl != NULL)
                        {
                            GTAElement * pElemOnCall = NULL;
                            QString fileName = pCallCmd->getElement();
                            QString callArgName = pCallCmd->getFunctionWithArgument();
                            if (!ioreferedItems.contains(callArgName))
                            {
                                bool rc =  pAppCtrl->getElementFromDocument(pCallCmd->getRefrenceFileUUID(),pElemOnCall,true);
                                if(rc && pElemOnCall != NULL)
                                {

                                    ioreferedItems.append(callArgName);
                                    pElemOnCall->replaceTag(pCallCmd->getTagValue());
                                    QStringList varListFromCall = getAllVariableList(pElemOnCall,ioreferedItems,listReleaseVar);
                                    if(!varListFromCall.isEmpty())
                                    {
                                        oVarList.append(varListFromCall);
                                    }
                                    delete pElemOnCall;
                                    pElemOnCall = NULL;

                                }
                                else
                                {
                                    //return oVarList;
                                }


                                
                            }
                        }
                    }

                }
                else
                {
                    if(!listReleaseVar)
                    {
                        QStringList varList = pCmd->getVariableList();
                        if(! varList.isEmpty())
                            oVarList.append(varList);
                    }
                    else
                    {
                        QStringList varList = pCmd->getVariableList();
                        if(pCmd->getCommandType() == GTACommandBase::ACTION)
                        {
                            GTAActionBase * pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
                            QString actType = pActCmd->getAction();
                            if(actType == ACT_REL)
                            {
                                oVarList.removeDuplicates();
                                //remove all parameters from oVarList using the varList;
                                for(int j=0; j<varList.count(); j++)
                                {
                                    int idx = oVarList.indexOf(varList.at(j));
                                    if(idx>=0)
                                    {
                                        oVarList.removeAt(idx);
                                    }
                                    else
                                    {
                                        oVarList.append(varList.at(j));
                                    }
                                }
                                varList.clear();
                            }
                        }
                        if(! varList.isEmpty())
                            oVarList.append(varList);
                    }
                }
            }
            // ioreferedItems.clear();
        }

    }
    return oVarList;
}


bool GTADocumentController::createTagVariables(QDomDocument &iDomDoc, QDomElement &iParentElem, GTAElement *& ipElement)
{
    QList<TagVariablesDesc> tagList = ipElement->getTagList();
    if(!tagList.isEmpty())
    {
       iParentElem = iDomDoc.createElement(XNODE_TAGVARIABLES);
        foreach(TagVariablesDesc tag, tagList)
        {
            QDomElement varElement = iDomDoc.createElement(XNODE_TAGVARIABLE);
            varElement.setAttribute(XATTR_NAME,tag.name);
            varElement.setAttribute(XATTR_TYPE,tag.type);
            varElement.setAttribute(XATTR_UNIT,tag.unit);
            varElement.setAttribute(XATTR_DESCRIPTION,tag.Desc);
            varElement.setAttribute(XATTR_VAL,tag.supportedValue);
            varElement.setAttribute(XATTR_MIN,tag.min);
            varElement.setAttribute(XATTR_MAX,tag.max);
            iParentElem.appendChild(varElement);
        }
    }
    return true;
}
