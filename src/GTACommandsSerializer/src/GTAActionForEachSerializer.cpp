#include "GTAActionForEachSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionForEach.h"
#include "GTARequirementSerializer.h"

GTAActionForEachSerializer::GTAActionForEachSerializer()
{
    _pActionCmd = NULL;
}

GTAActionForEachSerializer::~GTAActionForEachSerializer()
{
    _pActionCmd = NULL;
}

void GTAActionForEachSerializer::setCommand(const GTACommandBase* pCmd)
{
    _pActionCmd = dynamic_cast<GTAActionForEach*>((GTACommandBase*) pCmd);
}

bool GTAActionForEachSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString path = _pActionCmd->getPath();
        QString relativePath = _pActionCmd->getRelativePath();
		bool relativePathChkB = _pActionCmd->getRelativePathChkB();
		QString relativePathChkBQ = relativePathChkB ? "true" : "false";
		int delimiterIndex = _pActionCmd->getDelimiterIndex();
        QString delimiterIndexQ = QString::number(delimiterIndex);
        QStringList sDumpList = _pActionCmd ->getDumpList();
        bool overwriteresults = _pActionCmd->getOverWriteResults();
        QString overwriteresultsQ = overwriteresults ? "true" : "false";
        oElement  = iDomDoc.createElement(XNODE_ACTION);

        setActionAttributes(_pActionCmd,oElement);



        QDomElement domPath = iDomDoc.createElement(XNODE_LIST_PATH);
        QDomText domPathTxt = iDomDoc.createTextNode(path);
        domPath.appendChild(domPathTxt);
        oElement.appendChild(domPath);

		// RelativePathCheckBox is checked
		if (relativePathChkB)
		{
			//(story #358311) : saving relative path tag
			if (!relativePath.isEmpty())
			{
				QDomElement domRelativePath = iDomDoc.createElement(XNODE_LIST_RELATIVE_PATH);
				QDomText domRelativePathTxt = iDomDoc.createTextNode(relativePath);
				domRelativePath.appendChild(domRelativePathTxt);
				oElement.appendChild(domRelativePath);
				QDomElement domRelativePathStatus = iDomDoc.createElement(XNODE_RELATIVE_PATH_STATUS);
				QDomText domRelativePathStatusTxt = iDomDoc.createTextNode(relativePathChkBQ);
				domRelativePathStatus.appendChild(domRelativePathStatusTxt);
				oElement.appendChild(domRelativePathStatus);
			}
		}
		else 
		{
			QDomElement domRelativePathStatus = iDomDoc.createElement(XNODE_RELATIVE_PATH_STATUS);
			QDomText domRelativePathStatusTxt = iDomDoc.createTextNode(relativePathChkBQ);
			domRelativePathStatus.appendChild(domRelativePathStatusTxt);
			oElement.appendChild(domRelativePathStatus);
		}

        QDomElement domDelimiterIdx = iDomDoc.createElement(XNODE_FOR_DELIMITER_IDX);
        QDomText domDelimiterIdxText = iDomDoc.createTextNode(delimiterIndexQ);
        domDelimiterIdx.appendChild(domDelimiterIdxText);
        oElement.appendChild(domDelimiterIdx);

        QDomElement domOverwriteResults = iDomDoc.createElement(XNODE_FOR_OVERWRITE_RESULTS);
        QDomText domOverwriteResultsText = iDomDoc.createTextNode(overwriteresultsQ);
        domOverwriteResults.appendChild(domOverwriteResultsText);
        oElement.appendChild(domOverwriteResults);

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
        GTACommentSerializer commentSerializer(_pActionCmd);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }

        GTARequirementSerializer requirementSerializer(_pActionCmd);
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

bool GTAActionForEachSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {   QString actionName = iElement.attribute(XNODE_NAME);
        QString complementName = iElement.attribute(XNODE_COMPLEMENT);

        QString path;
        QString relativePath;
		bool relativePathChkB = false;
        QString delimiterValue;
        int delimiterIndex;
        bool overwriteresults;

        QStringList dumpList;

        QDomNodeList childList = iElement.childNodes();
        //TODO: What is node is missing? Needs fail mechanism
        for(int i = 0; i< childList.count(); i++)
        {
            QDomNode domNode = childList.at(i);
            if(domNode.isElement())
            {

                QDomElement childElem = domNode.toElement();
                QString snodename = childElem.nodeName();
                QString textVal = childElem.text();
                if(snodename == XNODE_LIST_PATH)
                    path = textVal;
				else if (snodename == XNODE_RELATIVE_PATH_STATUS)
					relativePathChkB = (textVal == "true") ? true : false;
				else if(snodename == XNODE_LIST_RELATIVE_PATH)
                    relativePath = textVal;
                else if(snodename == XNODE_FOR_DELIMITER_VAL)
                    delimiterValue = textVal;
                else if(snodename == XNODE_FOR_DELIMITER_IDX)
                    delimiterIndex = textVal.toInt();
                else if(snodename == XNODE_FOR_OVERWRITE_RESULTS)
                    overwriteresults = (textVal == "true")? true : false;
                else if (snodename == XNODE_DUMPLIST)
                {

                    QDomNodeList dumpParamNodes = domNode.childNodes();
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
                }
                else
                    break;
            }
        }

        GTAActionForEach * pCondCmd = new GTAActionForEach(actionName,complementName);
        getActionAttributes(iElement,pCondCmd);

        //(story #358311)
        //Saving relative path from absolute path if it contains "/TABLES" in it.
        QString tablesDir = QString("/%1").arg(TABLES_DIR);
		if (relativePathChkB)
		{
			if (!relativePath.isEmpty())
			{
				pCondCmd->setRelativePath(relativePath);
                QString repoPath = GTAUtil::getRepositoryPath();
                pCondCmd->setRepositoryPath(repoPath);
			}
		}
		else
		{
			pCondCmd->setRelativePath(QString());
            pCondCmd->setPath(path);
		}

		pCondCmd->setRelativePathChkB(relativePathChkB);
        pCondCmd->setDelimiterIndex(delimiterIndex);
        pCondCmd->setOverWriteResults(overwriteresults);
        pCondCmd->setColumnMap();

        if (!dumpList.isEmpty())
            pCondCmd->setDumpList(dumpList);
        opCommand = pCondCmd;

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


#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_CONDITION,COM_CONDITION_FOR_EACH),GTAActionForEachSerializer)

