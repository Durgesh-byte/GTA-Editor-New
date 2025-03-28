#include "GTAGenericToolCmdSerializer.h"
#include "GTACommentSerializer.h"
#include "GTAUtil.h"
#include "QDomElement"
#include "GTAGenericDB.h"
#include "GTAGenericDbAttribute.h"
#include "GTARequirementSerializer.h"

#define SCXML_JSON_EXPRESSION "SCXMLMod"

GTAGenericToolCmdSerializer::GTAGenericToolCmdSerializer()
{
    _pActionGenericCmd = NULL;
}


bool GTAGenericToolCmdSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionGenericCmd != NULL )
    {

        oElement  = iDomDoc.createElement(XNODE_ACTION);
        setActionAttributes(_pActionGenericCmd,oElement);

        //Parameter Node
        //  QString elementName = _pActionGenericCmd->getElement();
        QDomElement toolNameElem = iDomDoc.createElement(XNODE_TOOL_NAME);
        toolNameElem.setAttribute(XNODE_NAME, _pActionGenericCmd->getToolName());
        toolNameElem.setAttribute("DISPLAY_NAME",_pActionGenericCmd->getToolDisplayName());
        oElement.appendChild(toolNameElem);

        QDomElement domElement = iDomDoc.createElement(XNODE_FUNCTIONS);
        oElement.appendChild(domElement);

        bool isSCXMLModSelected = _pActionGenericCmd->getSCXMLModSelected();
        if (isSCXMLModSelected)
            domElement.setAttribute(SCXML_JSON_EXPRESSION,"true");
        else
            domElement.setAttribute(SCXML_JSON_EXPRESSION,"false");

        //   domElement.setAttribute(XNODE_NAME, elementName);

        QList<GTAGenericFunction> lstFunctions = _pActionGenericCmd->getCommand();
        for (int i = 0; i < lstFunctions.size(); i++)
        {

            QDomElement domFunctionElement = iDomDoc.createElement(XNODE_FUNCTION);
            domElement.appendChild(domFunctionElement);
            GTAGenericFunction objFunction = lstFunctions.at(i);

            QString functionName = objFunction.getFunctionName();
            QString functionDisplayName = objFunction.getDisplayName();
            bool functionAcknowledgement = objFunction.getWaitForAcknowledgement();
            QString functionType = objFunction.getToolType();
            QString functionToolID = objFunction.getToolID();

            //QList<GTAGenericArgument> lstArguments  = objFunction.getFunctionArguments();
            QList<GTAGenericArgument> lstArguments  = objFunction.getAllArguments();
            for(int j = 0; j < lstArguments.count(); j++)
            {
                QDomElement domArgumentElement = iDomDoc.createElement(XNODE_ARG);
                domFunctionElement.appendChild(domArgumentElement);
                GTAGenericArgument objArgument = lstArguments.at(j);

                int argId = objArgument.getId();
                QString argID =  QString::number(argId);
                QString argName = objArgument.getArgumentName();
                QString argType = objArgument.getArgumentType();

                QStringList lstValues = objArgument.getValues();
                QString argValue = lstValues.join(";");
                QString userval = objArgument.getUserSelectedValue();

                int index = objArgument.getUserSelectedHashIndex();
                QString hmiLabel = objArgument.getHMILabel();

                QStringList argConstVal = objArgument.getConstantValue();
                QString constVals = argConstVal.join(";");

                QStringList argDefVals = objArgument.getDefaultValue();
                QString defaultVals = argDefVals.join(";");

                QString mandatoryStr;
                bool mandatory = objArgument.isArgumentMandatory();
                if(mandatory)
                    mandatoryStr = "yes";
                else
                    mandatoryStr = "no";

                QString condition = objArgument.getCondition();
                QString searchType = objArgument.getSearchType();
                QString path = objArgument.getPath();
                QString userDbName = objArgument.getUserDbName();
                QString color = objArgument.getColor();
                QString backColor = objArgument.getBackColor();
                bool isArgConst = objArgument.isArgConst();
                QString isArgConstStr;

                if(isArgConst)
                    isArgConstStr = "yes";
                else
                    isArgConstStr = "no";

                QString isArgDisplayed;
                if(objArgument.isDisplayed())
                {
                    isArgDisplayed = "yes";
                }
                else
                {
                    isArgDisplayed = "no";
                }

                domArgumentElement.setAttribute(XNODE_ID,argID);
                domArgumentElement.setAttribute(XNODE_NAME,argName);
                domArgumentElement.setAttribute(XNODE_TYPE, argType);
                domArgumentElement.setAttribute(XNODE_VALUE, argValue);
                domArgumentElement.setAttribute(XNODE_USER_VALUE,userval);
                domArgumentElement.setAttribute(XNODE_USER_SEL_INDEX,index);
                domArgumentElement.setAttribute(XNODE_ARG_HMI_LABEL,hmiLabel);
                domArgumentElement.setAttribute(XNODE_ARG_CONST_VALS, constVals);
                domArgumentElement.setAttribute(XNODE_ARG_DEF_VALS,defaultVals);
                domArgumentElement.setAttribute(XNODE_ARG_MANDATORY,mandatoryStr);
                domArgumentElement.setAttribute(XNODE_CONDITION,condition);
                domArgumentElement.setAttribute(XNODE_ARG_SEARCH_TYPE,searchType);
                domArgumentElement.setAttribute(XNODE_ARG_SEARCH_PATH,path);
                domArgumentElement.setAttribute(XNODE_ARG_USER_DB_NAME,userDbName);;
                domArgumentElement.setAttribute(XNODE_IS_ARG_CONST,isArgConstStr);
                domArgumentElement.setAttribute("IS_ARG_DISPLAYED",isArgDisplayed);
                domArgumentElement.setAttribute(XNODE_ARG_COLOR, color);
                domArgumentElement.setAttribute(XNODE_ARG_BACKCOLOR, backColor);

                /* QStringList argDefVal = objArgument.getDefaultValue();
                 QString defVals = argDefVal.join(";");
                 domArgumentElement.setAttribute(XNODE_DEF_VALS,defVals);*/


            }
            QDomElement domReturnParamElem = iDomDoc.createElement(XNODE_RETURNTYPE);
            domFunctionElement.appendChild(domReturnParamElem);

            QString paramName = objFunction.getReturnParamName();
            QString paramType = objFunction.getReturnParamType();
            domReturnParamElem.setAttribute(XNODE_NAME, paramName);
            domReturnParamElem.setAttribute(XNODE_TYPE, paramType);
            domFunctionElement.setAttribute("IS_RETURN_TYPE_SIMPLE","no");
            if(objFunction.isReturnTypeSimple())
            {
                GTAGenericAttribute objSimpleReturn = objFunction.getFunctionReturnType();
                QString returnCode = objSimpleReturn.getReturnCode();
                QString trueCond = objSimpleReturn.getTrueCondition();
                QString falseCond = objSimpleReturn.getFalseCondition();
                QString occurence = objSimpleReturn.getAttributeOccurence();
                QString waitUntil;
                if(objSimpleReturn.hasWaitUntil())
                {
                    waitUntil = QString("yes");
                }
                else
                {
                    waitUntil = QString("no");
                }
                QStringList valList = objSimpleReturn.getAttributeValues();
                QString vals = valList.join(";");

                domReturnParamElem.setAttribute(XNODE_RETURN_CODE,returnCode);
                domReturnParamElem.setAttribute(XNODE_OCCURENCE,occurence);
                domReturnParamElem.setAttribute(XNODE_TRUE_COND,trueCond);
                domReturnParamElem.setAttribute(XNODE_FALSE_COND,falseCond);
                domReturnParamElem.setAttribute(XNODE_ATTR_WAIT_UNTIL,waitUntil);
                domReturnParamElem.setAttribute(XNODE_ARG_DESCRIPTION, objSimpleReturn.getAttributeDescription());
                domReturnParamElem.setAttribute(XNODE_VALUE, vals);

                domFunctionElement.setAttribute("IS_RETURN_TYPE_SIMPLE","yes");
            }


            domFunctionElement.setAttribute(XNODE_FUNCTION_NAME,functionName);
            domFunctionElement.setAttribute(XNODE_FUNCTION_DISPLAY_NAME,functionDisplayName);
            domFunctionElement.setAttribute(XNODE_TOOL_TYPE, functionType);
            domFunctionElement.setAttribute(XNODE_TOOL_ID, functionToolID);
            domFunctionElement.setAttribute(XNODE_FUNCTION_ACKNOWLEDGEMENT,functionAcknowledgement == true ? "true":"false");

        }

        QDomElement definitionsElem = iDomDoc.createElement(XNODE_DEFINITIONS);
        oElement.appendChild(definitionsElem);

        QList<GTAGenericParamData> lstDefinition = _pActionGenericCmd->getDefinitions();
        for(int i = 0; i < lstDefinition.count(); i++)
        {
            GTAGenericParamData objParam = lstDefinition.at(i);
            QDomElement dataElem = iDomDoc.createElement(XNODE_DATA);
            definitionsElem.appendChild(dataElem);

            dataElem.setAttribute(XNODE_NAME, objParam.getParamDataName());
            dataElem.setAttribute(XNODE_TYPE, objParam.getParamDataType());

            QList<GTAGenericAttribute> lstAttribute = objParam.getParamAttributeList();

            for(int j = 0; j < lstAttribute.count(); j++)
            {
                GTAGenericAttribute objAttr = lstAttribute.at(j);
                QDomElement attributeElem = iDomDoc.createElement(XNODE_ATTRIBUTE);
                dataElem.appendChild(attributeElem);
                attributeElem.setAttribute(XNODE_NAME,objAttr.getAttributeName());
                attributeElem.setAttribute(XNODE_TYPE,objAttr.getAttributeType());
                attributeElem.setAttribute(XNODE_OCCURENCE,objAttr.getAttributeOccurence());
                QStringList valList = objAttr.getAttributeValues();
                QString vals = valList.join(";");
                attributeElem.setAttribute(XNODE_VALUE, vals);
                attributeElem.setAttribute(XNODE_RETURN_CODE,objAttr.getReturnCode());
                attributeElem.setAttribute(XNODE_TRUE_COND,objAttr.getTrueCondition());
                attributeElem.setAttribute(XNODE_FALSE_COND,objAttr.getFalseCondition());
                attributeElem.setAttribute(XNODE_ARG_DESCRIPTION, objAttr.getAttributeDescription());

                QString waitUntil;
                if(objAttr.hasWaitUntil())
                {
                    waitUntil = QString("yes");
                }
                else
                {
                    waitUntil = QString("no");
                }

                attributeElem.setAttribute(XNODE_ATTR_WAIT_UNTIL,waitUntil);
            }
        }

        QDomElement UserDbElem = iDomDoc.createElement(XNODE_DATABASE);
        oElement.appendChild(UserDbElem);

        QList<GTAGenericDB> UserDbList = _pActionGenericCmd->getUserDbList();
        for(int i = 0; i < UserDbList.count();i++)
        {
            GTAGenericDB dbObj = UserDbList.at(i);

            QDomElement dbElem = iDomDoc.createElement("DB");
            UserDbElem.appendChild(dbElem);
            dbElem.setAttribute("NAME", dbObj.getName());
            dbElem.setAttribute("PATH", dbObj.getPath());
            dbElem.setAttribute("RELATIVE_PATH", dbObj.getRelativePath());
            dbElem.setAttribute("DELIMITER",dbObj.getDelimiter());

            QList<int> colConcatList = dbObj.getColsConcatList();
            QStringList tempCol;
            for(int z = 0; z < colConcatList.count(); z++)
            {
                int num = colConcatList.at(z);
                QString t = QString::number(num);
                tempCol.append(t);
            }

            QString colConcat = tempCol.join(";");
            QString concatStr = dbObj.getConcatString();

            QList<GTAGenericDbAttribute> attributesList = dbObj.getAttributes();
            QDomElement attributesElem = iDomDoc.createElement("ATTRIBUTES");
            attributesElem.setAttribute("CONCAT_LIST",colConcat);
            attributesElem.setAttribute("CONCAT_STR",concatStr);
            dbElem.appendChild(attributesElem);

            for(int j = 0; j < attributesList.count(); j++)
            {
                GTAGenericDbAttribute attribute = attributesList.at(j);
                QString name = attribute.getName();
                int col = attribute.getColumn();
                int Id = attribute.getId();

                QDomElement attributeElem = iDomDoc.createElement("ATTRIBUTE");
                attributesElem.appendChild(attributeElem);
                attributeElem.setAttribute("ID", QString::number(Id));
                attributeElem.setAttribute("NAME", name);
                attributeElem.setAttribute("COLUMN", QString::number(col));
            }
        }

        //comment node
        GTACommentSerializer commentSerializer(_pActionGenericCmd);
        QDomElement domComment;
        rc = commentSerializer.serialize(iDomDoc,domComment);
        if(rc && (!domComment.isNull()))
        {
            oElement.appendChild(domComment);
        }
        GTARequirementSerializer requirementSerializer(_pActionGenericCmd);
        QDomElement domRequirement;
        rc = requirementSerializer.serialize(iDomDoc,domRequirement);
        if( rc && (!domRequirement.isNull()))
        {
                oElement.appendChild(domRequirement);
        }

    }
    return rc;
}

bool GTAGenericToolCmdSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{

    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
    {

        QDomNodeList lstElements =  iElement.childNodes();
        if(lstElements.count())
        {
            GTAGenericToolCommand *pActionGenericCmd = new GTAGenericToolCommand();
            getActionAttributes(iElement,pActionGenericCmd);
            QString toolName;
            QString toolDisplayName;
            if(lstElements.at(0).hasAttributes())
            {
                QDomNamedNodeMap toolNameDetails = lstElements.at(0).attributes();
                toolName = toolNameDetails.namedItem(XNODE_NAME).nodeValue();
                toolDisplayName = toolNameDetails.namedItem("DISPLAY_NAME").nodeValue();
             }
            if(lstElements.at(1).hasChildNodes())
            {
                if(lstElements.at(1).hasAttributes())
                {
                    QDomNamedNodeMap FunctionsDetails = lstElements.at(1).attributes();
                    QString SCXMLMod = FunctionsDetails.namedItem(SCXML_JSON_EXPRESSION).nodeValue();
                    if ((!SCXMLMod.isEmpty()) && (SCXMLMod == "true"))
                        pActionGenericCmd->setSCXMLModSelected(true);
                    else
                        pActionGenericCmd->setSCXMLModSelected(false);
                }
                QDomNodeList lstFunctions = lstElements.at(1).childNodes();
                for(int i = 0 ; i < lstFunctions.count(); i++)
                {
                    GTAGenericFunction objFunc;

                    QDomNamedNodeMap functionAttributes = lstFunctions.at(i).attributes();
                    QString functionName = functionAttributes.namedItem(XNODE_FUNCTION_NAME).nodeValue();
                    QString functionType = functionAttributes.namedItem(XNODE_TOOL_TYPE).nodeValue();
                    QString functionToolID = functionAttributes.namedItem(XNODE_TOOL_ID).nodeValue();
                    QString functionDisplay = functionAttributes.namedItem(XNODE_FUNCTION_DISPLAY_NAME).nodeValue();
                    QString strReturnTypeSimple = functionAttributes.namedItem("IS_RETURN_TYPE_SIMPLE").nodeValue();
                    QString functionacknowledgement = functionAttributes.namedItem(XNODE_FUNCTION_ACKNOWLEDGEMENT).nodeValue();
                    if (functionacknowledgement.isEmpty())
                        functionacknowledgement = "true";

                    bool isRetTypeSimple;
                    if(strReturnTypeSimple == "yes")
                        isRetTypeSimple = true;
                    else if(strReturnTypeSimple == "no")
                        isRetTypeSimple = false;

                    objFunc.setIsReturnTypeSimple(isRetTypeSimple);
                    objFunc.setFunctionName(functionName);
                    objFunc.setToolType(functionType);
                    objFunc.setToolID(functionToolID);
                    objFunc.setDisplayName(functionDisplay);
                    //always wait for acknowledgement unless mentionted otherwise
                    objFunc.setWaitForAcknowledgement((functionacknowledgement == "false") ? false : true);

                    if( lstFunctions.at(i).hasChildNodes())
                    {
                        QList<GTAGenericArgument> Arguments;
                        QDomNodeList lstFunctionArgsReturnParam = lstFunctions.at(i).childNodes();
                        for(int j = 0 ; j < lstFunctionArgsReturnParam.count(); j++)
                        {
                            GTAGenericArgument objArg;
                            if(lstFunctionArgsReturnParam.at(j).nodeName() == XNODE_ARG)
                            {
                                QDomNamedNodeMap argumentAttr = lstFunctionArgsReturnParam.at(j).attributes();

                                QString argID = argumentAttr.namedItem(XNODE_ID).nodeValue();
                                QString argName  = argumentAttr.namedItem(XNODE_NAME).nodeValue();
                                QString argType = argumentAttr.namedItem(XNODE_TYPE).nodeValue();
                                QString argValue = argumentAttr.namedItem(XNODE_VALUE).nodeValue();
                                QStringList argValuelst = argValue.split(";",QString::SkipEmptyParts);
                                QString val = argumentAttr.namedItem(XNODE_USER_VALUE).nodeValue();
                                QString indexStr = argumentAttr.namedItem(XNODE_USER_SEL_INDEX).nodeValue();
                                int index = indexStr.toInt();

                                QString hmiLabel = argumentAttr.namedItem(XNODE_ARG_HMI_LABEL).nodeValue();
                                QString constVals = argumentAttr.namedItem(XNODE_ARG_CONST_VALS).nodeValue();
                                QStringList argConstVals = constVals.split(";",QString::SkipEmptyParts);
                                QString defVals = argumentAttr.namedItem(XNODE_ARG_DEF_VALS).nodeValue();
                                QStringList argDefVals = defVals.split(";",QString::SkipEmptyParts);
                                QString isArConstStr = argumentAttr.namedItem(XNODE_IS_ARG_CONST).nodeValue();
                                bool isArgConst;
                                if(isArConstStr == "yes")
                                    isArgConst = true;
                                else if(isArConstStr == "no")
                                    isArgConst = false;

                                QString mandatoryStr = argumentAttr.namedItem(XNODE_ARG_MANDATORY).nodeValue();
                                QString searchType = argumentAttr.namedItem(XNODE_ARG_SEARCH_TYPE).nodeValue();
                                QString searchPath = argumentAttr.namedItem(XNODE_ARG_SEARCH_PATH).nodeValue();
                                QString condition = argumentAttr.namedItem(XNODE_CONDITION).nodeValue();
                                QString userDbName = argumentAttr.namedItem(XNODE_ARG_USER_DB_NAME).nodeValue();
                                QString strArgDisplayed = argumentAttr.namedItem("IS_ARG_DISPLAYED").nodeValue();
                                QString color = argumentAttr.namedItem(XNODE_ARG_COLOR).nodeValue();
                                QString backColor = argumentAttr.namedItem(XNODE_ARG_BACKCOLOR).nodeValue();
                                bool isArgDisplayed;
                                if(strArgDisplayed == "yes")
                                    isArgDisplayed = true;
                                else if(strArgDisplayed == "no")
                                    isArgDisplayed = false;

                                objArg.setId(argID.toInt());
                                objArg.setArgumentName(argName);
                                objArg.setArgumentType(argType);
                                objArg.setValues(argValuelst);
                                objArg.setUserSelectedValue(val);
                                objArg.setUserSelectedHashIndex(index);
                                objArg.setHMILabel(hmiLabel);
                                objArg.setConstantValue(argConstVals);
                                objArg.setDefaultValue(argDefVals);
                                objArg.setArgIsConst(isArgConst);
                                objArg.setMandatory(mandatoryStr);
                                objArg.setSearchType(searchType);
                                objArg.setPath(searchPath);
                                objArg.setCondition(condition);
                                objArg.setIsDisplayed(isArgDisplayed);
                                objArg.setUserDbName(userDbName);
                                objArg.setColor(color);
                                objArg.setBackColor(backColor);
                                Arguments.append(objArg);
                            }
                            else if(lstFunctionArgsReturnParam.at(j).nodeName() == XNODE_RETURNTYPE )
                            {
                                QDomNamedNodeMap returnTypeAttr = lstFunctionArgsReturnParam.at(j).attributes();
                                QString returnName = returnTypeAttr.namedItem(XNODE_NAME).nodeValue();
                                QString returnType = returnTypeAttr.namedItem(XNODE_TYPE).nodeValue();
                                objFunc.setReturnParamName(returnName);
                                objFunc.setReturnParamType(returnType);
                                if(isRetTypeSimple)
                                {
                                    GTAGenericAttribute objSimpleRetType;
                                    QString attrOccurence = returnTypeAttr.namedItem(XNODE_OCCURENCE).nodeValue();
                                    QString attrVal = returnTypeAttr.namedItem(XNODE_VALUE).nodeValue();
                                    QStringList attrVals = attrVal.split(";",QString::SkipEmptyParts);

                                    QString attrReturnCode = returnTypeAttr.namedItem(XNODE_RETURN_CODE).nodeValue();
                                    QString attrTrueCond = returnTypeAttr.namedItem(XNODE_TRUE_COND).nodeValue();
                                    QString attrFalseCond = returnTypeAttr.namedItem(XNODE_FALSE_COND).nodeValue();
                                    QString attrDesc = returnTypeAttr.namedItem(XNODE_ARG_DESCRIPTION).nodeValue();
                                    QString attrWaitUntil = returnTypeAttr.namedItem(XNODE_ATTR_WAIT_UNTIL).nodeValue();
                                    objSimpleRetType.setAttributeName(returnName);
                                    objSimpleRetType.setAttributeType(returnType);
                                    objSimpleRetType.setAttributeOccurence(attrOccurence);
                                    objSimpleRetType.setAttributeValues(attrVals);
                                    objSimpleRetType.setReturnCode(attrReturnCode);
                                    objSimpleRetType.setTrueCondition(attrTrueCond);
                                    objSimpleRetType.setFalseCondition(attrFalseCond);
                                    objSimpleRetType.setAttributeDescription(attrDesc);
                                    objSimpleRetType.setWaitUntil(attrWaitUntil);

                                    objFunc.setFunctionReturnType(objSimpleRetType);
                                }

                            }
                        }
                        objFunc.setFunctionArguments(Arguments,true);
                        objFunc.setAllArguments(Arguments,true);
                    }

                    pActionGenericCmd->insertFunction(objFunc);
                }

            }
            if(lstElements.at(2).hasChildNodes() && lstElements.at(2).nodeName() == XNODE_DEFINITIONS)
            {
                QDomNodeList lstReturnParams = lstElements.at(2).childNodes();

                QList <GTAGenericParamData> Definitions;
                for(int i = 0; i < lstReturnParams.count(); i++)
                {
                    if(lstReturnParams.at(i).nodeName() == XNODE_DATA)
                    {
                        GTAGenericParamData objParam;
                        QDomNamedNodeMap paramDetails = lstReturnParams.at(i).attributes();
                        QString paramName = paramDetails.namedItem(XNODE_NAME).nodeValue();
                        QString paramType = paramDetails.namedItem(XNODE_TYPE).nodeValue();

                        objParam.setParamDataName(paramName);
                        objParam.setParamDataType(paramType);
                        if(lstReturnParams.at(i).hasChildNodes())
                        {
                            QList<GTAGenericAttribute> Attributes;
                            QDomNodeList lstAttributes = lstReturnParams.at(i).childNodes();
                            for(int j = 0; j < lstAttributes.count(); j++)
                            {
                                if(lstAttributes.at(j).nodeName() == XNODE_ATTRIBUTE)
                                {
                                    GTAGenericAttribute objAttribute;
                                    QDomNamedNodeMap attributeDetails = lstAttributes.at(j).attributes();
                                    QString attrName = attributeDetails.namedItem(XNODE_NAME).nodeValue();
                                    QString attrType = attributeDetails.namedItem(XNODE_TYPE).nodeValue();
                                    QString attrOccurence = attributeDetails.namedItem(XNODE_OCCURENCE).nodeValue();
                                    QString attrVal = attributeDetails.namedItem(XNODE_VALUE).nodeValue();
                                    QStringList attrVals = attrVal.split(";",QString::SkipEmptyParts);

                                    QString attrReturnCode = attributeDetails.namedItem(XNODE_RETURN_CODE).nodeValue();
                                    QString attrTrueCond = attributeDetails.namedItem(XNODE_TRUE_COND).nodeValue();
                                    QString attrFalseCond = attributeDetails.namedItem(XNODE_FALSE_COND).nodeValue();
                                    QString attrDesc = attributeDetails.namedItem(XNODE_ARG_DESCRIPTION).nodeValue();
                                    QString attrWaitUntil = attributeDetails.namedItem(XNODE_ATTR_WAIT_UNTIL).nodeValue();
                                    objAttribute.setAttributeName(attrName);
                                    objAttribute.setAttributeType(attrType);
                                    objAttribute.setAttributeOccurence(attrOccurence);
                                    objAttribute.setAttributeValues(attrVals);
                                    objAttribute.setReturnCode(attrReturnCode);
                                    objAttribute.setTrueCondition(attrTrueCond);
                                    objAttribute.setFalseCondition(attrFalseCond);
                                    objAttribute.setAttributeDescription(attrDesc);
                                    objAttribute.setWaitUntil(attrWaitUntil);
                                    Attributes.append(objAttribute);

                                }
                            }
                            objParam.setParamAttributeList(Attributes);
                        }

                        Definitions.append(objParam);
                    }

                }

                pActionGenericCmd->setDefinitions(Definitions);

            }
            if((lstElements.at(3).hasChildNodes()) && (lstElements.at(3).nodeName() == XNODE_DATABASE))
            {
                QDomNodeList dbNodeList = lstElements.at(3).childNodes();
                QList<GTAGenericDB> UserDbList;
                for(int i = 0;i < dbNodeList.count(); i++)
                {
                    GTAGenericDB dbObj;
                    QDomNode dbNode = dbNodeList.at(i);
                    if(dbNode.nodeName() == "DB")
                    {
                        QString dbName = dbNode.attributes().namedItem("NAME").nodeValue();
                        QString dbPath = dbNode.attributes().namedItem("PATH").nodeValue();
                        QString dbRelativePath = dbNode.attributes().namedItem("RELATIVE_PATH").nodeValue();
                        QString dbDelimiter = dbNode.attributes().namedItem("DELIMITER").nodeValue();

                        dbObj.setName(dbName);
                        dbObj.setPath(dbPath);
                        dbObj.setRelativePath(dbRelativePath);
                        dbObj.setDelimiter(dbDelimiter);

                        QDomNode dbAttributesNode = dbNode.childNodes().at(0);

                        QString colConcat = dbAttributesNode.attributes().namedItem("CONCAT_LIST").nodeValue();
                        QStringList colConcatTemp = colConcat.split(";");
                        QList<int> concatList;
                        for(int z = 0; z < colConcatTemp.count(); z++)
                        {
                            int val = colConcatTemp.at(z).toInt();
                            concatList.append(val);
                        }
                        dbObj.setColsConcatList(concatList);
                        QString concatStr = dbAttributesNode.attributes().namedItem("CONCAT_STR").nodeValue();
                        dbObj.setConcatString(concatStr);

                        QDomNodeList dbAttributeList = dbAttributesNode.childNodes();
                        for(int j = 0; j < dbAttributeList.count(); j++)
                        {
                            QDomNode attrNode = dbAttributeList.at(j);
                            QString Id = attrNode.attributes().namedItem("ID").nodeValue();
                            QString name = attrNode.attributes().namedItem("NAME").nodeValue();
                            QString col = attrNode.attributes().namedItem("COLUMN").nodeValue();
                            dbObj.addAttribute(Id.toInt(),name,col.toInt());
                        }
                        UserDbList.append(dbObj);
                    }
                }
                pActionGenericCmd->setUserDbList(UserDbList);
            }

            pActionGenericCmd->setToolName(toolName);
            pActionGenericCmd->setToolDisplayName(toolDisplayName);
            opCommand = pActionGenericCmd;
            rc = true;

        }
        //comment node
        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);
        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);

    }


    return rc;

}

void GTAGenericToolCmdSerializer::setCommand(const GTACommandBase* pCmd)
{
    //  _pActionGenericCmd = dynamic_cast<GTAGenericToolCommand*> ((GTACommandBase *) pCmd);
    _pActionGenericCmd = dynamic_cast<GTAGenericToolCommand*>((GTACommandBase*)pCmd);
}

#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1").arg(ACT_GEN_TOOL),GTAGenericToolCmdSerializer)
