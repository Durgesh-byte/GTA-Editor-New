#include "GTAGenericToolController.h"

#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QDomDocument>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFile>
#include <QUrl>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QValidator>
#include <QBuffer>
#pragma warning(pop)

GTAGenericToolController::GTAGenericToolController()
{

}

QHash<QString,QString> GTAGenericToolController::getAllToolConfigrations(QStringList &errList)
{
    //QString toolCongigPath = GTASystemServices::getToolConfigPath();
    QStringList toolConfigList;
    QStringList errListFiles;

    GTASystemServices *pSystemServices = GTASystemServices::getSystemServices();
    _ToolConfigPathDir = pSystemServices->getToolConfigurationPath();


    QDir dir(_ToolConfigPathDir);
    //bool exitStatus;

    if(dir.exists())
    {
        QFileInfoList fileList = dir.entryInfoList(QStringList("*.xml"),QDir::Files | QDir::NoSymLinks);
        for(int i = 0; i < fileList.size(); i++)
        {
            QFileInfo file = fileList.at(i);
            QString configFileName = file.absoluteFilePath();

            QFile xmlFile(configFileName);
            bool rc = xmlFile.open(QFile::ReadOnly | QFile::Text);
            QDomDocument * pDomDoc = NULL;
            if (rc)
            {

                pDomDoc = new QDomDocument();
                if(pDomDoc != NULL)
                {
                    if(pDomDoc->setContent(&xmlFile))
                    {
                        QDomElement toolNameElem = pDomDoc->documentElement();
                        QString toolName = toolNameElem.attribute("name");
                        QString toolDisplayName = toolNameElem.attribute("toolDisplayName");
                        if(!toolName.trimmed().isEmpty())
                        {
                            _AllGenTools.insert(toolName,configFileName);
                            _UserToolNameHash.insert(toolName,toolDisplayName);
                            toolConfigList.append(toolName);
                        }
                    }
					delete pDomDoc;
					pDomDoc = NULL;
                }
            }
            xmlFile.close();
        }
    }
    else
    {
        errList.append(QString("Could not open dir %1").arg(_ToolConfigPathDir));
    }
    return _UserToolNameHash;
}

/*bool GTAGenericToolController::validateConfigXml(const QString &iconfigFileName)
{

    GTASystemServices *sys  = GTASystemServices::getSystemServices();
    QString schemaFile = sys->getGenericToolSchemaFile();


    QFile Schemafile(schemaFile);

    QByteArray schemaBuffer(Schemafile.readAll());
    QBuffer buffer(&schemaBuffer);
    buffer.open(QIODevice::ReadOnly);

    QXmlSchema schema;
    MessageHandler msgHandeler;
    schema.setMessageHandler(&msgHandeler);

    bool rc = schema.load(&buffer);
    qDebug()<<msgHandeler.statusMessage();
    if(rc)
    {
        if (schema.isValid())
        {
            QFile file(iconfigFileName);
            if(file.open(QIODevice::ReadOnly))
            {
                QString toolConfigFileContent = file.readAll();
                QXmlSchemaValidator validator(schema);

                rc = validator.validate(toolConfigFileContent);
                if(!rc)
                {
                    qDebug()<<msgHandeler.statusMessage();
                }
                return rc;
            }
        }
    }
    else
    {
        qDebug()<<msgHandeler.statusMessage();
    }

}*/

bool GTAGenericToolController::ParseInputFile(const QString &iInputFile)
{
    bool rc = false;
    QFile xmlFile(iInputFile);
    if(xmlFile.exists())
    {
        bool fileCheck = xmlFile.open(QFile::ReadOnly | QFile::Text);
        QDomDocument * pDomDoc = NULL;
        if (fileCheck)
        {
            pDomDoc = new QDomDocument();
            if(pDomDoc != NULL)
            {
                if( ! pDomDoc->setContent(&xmlFile))
                {
                    delete pDomDoc;
                    pDomDoc = NULL;
                    // errList.append(QString("Could not open %1").arg(configFileName));
                    rc = false;
                    return rc;

                }

                parseInputFile(iInputFile, pDomDoc);

				delete pDomDoc;
				pDomDoc = NULL;
            }
        }
        xmlFile.close();
    }
	return rc;
}

bool GTAGenericToolController::ParseToolConfigFile(const QString &iToolName)
{
    bool rc = true;
    QString configFileName;

    configFileName = getPathByToolName(iToolName);

    QFile xmlFile(configFileName);
    if(xmlFile.exists())
    {
        bool fileCheck = xmlFile.open(QFile::ReadOnly | QFile::Text);
        QDomDocument * pDomDoc = NULL;
        if (fileCheck)
        {
            pDomDoc = new QDomDocument();
            if(pDomDoc != NULL)
            {
                if( ! pDomDoc->setContent(&xmlFile))
                {
                    delete pDomDoc;
                    pDomDoc = NULL;
                    // errList.append(QString("Could not open %1").arg(configFileName));
                    rc = false;
                    return rc;

                }
                //if(validateConfigXml(configFileName))
                parseConfigFile(iToolName, pDomDoc);
                //else
                //{
                //    rc = false;
                //    qDebug("Invalid Schema");
                //}
				delete pDomDoc;
				pDomDoc = NULL;
            }
        }
        xmlFile.close();
    }
    return rc;
}


QList<GTAGenericFunction> GTAGenericToolController::parseFunctions(QDomDocument *pDomDoc)
{
    _UserDBNames.clear();
    QList<GTAGenericFunction> Functions;
    QDomNodeList nodeList = pDomDoc->elementsByTagName("functions");           // <functions> ... </functions>
    QDomNode nodeItem;
    if (nodeList.count())
    {
        nodeItem = nodeList.at(0);
        QDomNamedNodeMap nodeItemAttr = nodeItem.attributes();
        if(nodeItemAttr.contains("type"))
        {
            QDomNode nodeItemAttrNode = nodeItemAttr.namedItem("type");
            QString functionType = nodeItemAttrNode.nodeValue();              // STATIC or DYNAMIC
            _ToolConfigData.setFunctionType(functionType);
        }
        if(nodeItem.hasChildNodes())
        {
            QDomNodeList nodeItemList = nodeItem.childNodes();              //all functions under <functions> tag in nodeItemList
            // int size =  nodeItemList.size();

            for(int i = 0; i < nodeItemList.size(); i++)
            {
                QStringList argToUse;
                GTAGenericFunction funcObj;
                QDomNode domFunction = nodeItemList.at(i);
                if(domFunction.nodeName() == "function")                        // parsing each function and details like function name , type and type ID
                {
                    QDomNamedNodeMap domFunctionAttr = domFunction.attributes();
                    QString funcName = domFunctionAttr.namedItem("name").nodeValue();
                    funcObj.setFunctionName(funcName);

                    QDomNode nodeToolType = domFunctionAttr.namedItem("toolType");
                    QString funcToolType = nodeToolType.nodeValue();
                    funcObj.setToolType(funcToolType);

                    QDomNode nodeToolID = domFunctionAttr.namedItem("toolId");
                    QString funcToolID = nodeToolID.nodeValue();
                    funcObj.setToolID(funcToolID);

                    if(domFunctionAttr.contains("functionDisplayName"))
                    {
                        QString displayName = domFunctionAttr.namedItem("functionDisplayName").nodeValue();
                        funcObj.setDisplayName(displayName);
                    }
                    if(domFunctionAttr.contains("waitForAcknowledgement"))
                    {
                        bool ok;
                        int acknowledgement = domFunctionAttr.namedItem("waitForAcknowledgement").nodeValue().toInt(&ok);
                        //always check if the user entered a correct value to the argument
                        if (ok)
                        {
                            // waitForAcknowledgement <= 0 will not wait for acknowledgement
                            funcObj.setWaitForAcknowledgement(acknowledgement >= 1 ? true : false);
                        }
                        else
                        {
                            // always wait for acknowledgement by default
                            funcObj.setWaitForAcknowledgement(true);
                        }
                    }
                    else
                    {
                        // always wait for acknowledgement by default
                        funcObj.setWaitForAcknowledgement(true);
                    }
                    if(domFunctionAttr.contains("argList"))
                    {
                        QString argList = domFunctionAttr.namedItem("argList").nodeValue();
                        argToUse = argList.split(";",QString::SkipEmptyParts);
                        // funcObj.setArgsToUseList(argToUse);
                    }


                    QDomNodeList domArgumentsTag = domFunction.childNodes();                            // Arguments and Return types are two child nodes of function tag
                    if(domArgumentsTag.count())
                    {
                        int count = 0;
                        QList<GTAGenericArgument> FunctionArguments;
                        QDomNodeList domArgumetList = domArgumentsTag.at(0).childNodes();               //Arguments
                        for(int j = 0; j < domArgumetList.size(); j++)
                        {

                            GTAGenericArgument argumentObj;
                            QDomNode nodeArgument = domArgumetList.at(j);
                            if(nodeArgument.nodeName() == "argument")
                            {
                                QDomNamedNodeMap domArgumentDetails = nodeArgument.attributes();
                                QString argumentName = domArgumentDetails.namedItem("name").nodeValue();

                                argumentObj.setId(count);
                                argumentObj.setArgumentName(argumentName);

                                QString argumentType = domArgumentDetails.namedItem("type").nodeValue();
                                argumentObj.setArgumentType(argumentType);

                                if(domArgumentDetails.contains("HMILabel"))
                                {
                                    QString argumentHMILabel = domArgumentDetails.namedItem("HMILabel").nodeValue();
                                    argumentObj.setHMILabel(argumentHMILabel);
                                }

                                if(domArgumentDetails.contains("cond"))
                                {
                                    QString argumentCondition = domArgumentDetails.namedItem("cond").nodeValue();
                                    argumentObj.setCondition(argumentCondition);
                                }
                                if(domArgumentDetails.contains("values"))
                                {
                                    QString argumentValues = domArgumentDetails.namedItem("values").nodeValue();
                                    QStringList valuesList = argumentValues.split(";",QString::SkipEmptyParts);
                                    argumentObj.setValues(valuesList);

                                }
                                if(domArgumentDetails.contains("defaultValue"))
                                {
                                    QString argumentDefaultVal = domArgumentDetails.namedItem("defaultValue").nodeValue();
                                    QStringList defaultVals = argumentDefaultVal.split(";",QString::SkipEmptyParts);
                                    argumentObj.setDefaultValue(defaultVals);
                                }
                                if(domArgumentDetails.contains("constValue"))
                                {
                                    QString argumentConstVal = domArgumentDetails.namedItem("constValue").nodeValue();
                                    QStringList constVals = argumentConstVal.split(";",QString::SkipEmptyParts);
                                    argumentObj.setConstantValue(constVals);
                                }
                                if(domArgumentDetails.contains("mandatory"))
                                {
                                    QString argumentMandatory = domArgumentDetails.namedItem("mandatory").nodeValue();
                                    argumentObj.setMandatory(argumentMandatory);
                                }
                                if(domArgumentDetails.contains("searchType"))
                                {
                                    QString argumentSearchType = domArgumentDetails.namedItem("searchType").nodeValue();
                                    argumentObj.setSearchType(argumentSearchType);

                                    if(argumentSearchType == XNODE_FILE_SEARCH_TYPE || argumentSearchType == XNODE_FOLDER_SEARCH_TYPE)
                                    {
                                        QString path = domArgumentDetails.namedItem("path").nodeValue();
                                        argumentObj.setPath(path);
                                    }
                                    if(argumentSearchType == XNODE_USER_DEF_DB_SEARCH_TYPE)
                                    {
                                        QString name = domArgumentDetails.namedItem("userDbName").nodeValue();
                                        if(!name.isEmpty())
                                        {
                                            argumentObj.setUserDbName(name);
                                            _UserDBNames << name;
                                        }

                                    }
                                }
                                FunctionArguments.append(argumentObj);
                                count++;

                            }

                        }
                        if(!argToUse.isEmpty())
                        {

                            for (int m = 0; m < FunctionArguments.count(); m++)
                            {
                                GTAGenericArgument obj = FunctionArguments.at(m);
                                if(argToUse.contains(obj.getArgumentName()))
                                {
                                    QString condition = obj.getCondition();
                                    if(!condition.isEmpty())
                                    {
                                        if(!argToUse.contains(condition))
                                        {

                                            for(int n = 0; n < FunctionArguments.count(); n++)
                                            {
                                                GTAGenericArgument objCondition =  FunctionArguments.at(n);
                                                if(objCondition.getArgumentName() == condition)
                                                {

                                                    objCondition.setIsDisplayed(true);
                                                    FunctionArguments.removeAt(n);
                                                    FunctionArguments.insert(n,objCondition);
                                                    obj.setIsDisplayed(true);
                                                    break;
                                                }
                                            }
                                        }

                                    }

                                }
                                else
                                {
                                    obj.setIsDisplayed(false);
                                    FunctionArguments.removeAt(m);
                                    FunctionArguments.insert(m,obj);
                                }
                            }

                        }

                        funcObj.setFunctionArguments(FunctionArguments);
                        funcObj.setAllArguments(FunctionArguments);

                        QDomNode nodeReturnType;
                        bool returnNodeFound = false;
                        for (int index = 0; index < domArgumentsTag.count(); index++)
                        {
                            if (domArgumentsTag.at(index).nodeName() == "returns")
                            {
                                QDomNodeList nodeReturn = domArgumentsTag.at(index).childNodes();
                                if ((nodeReturn.count() > 0) && (nodeReturn.at(0).nodeName() == "return"))
                                {
                                    nodeReturnType = nodeReturn.at(0);
                                    returnNodeFound = true;
                                }
                            }
                        }
         
                        if (returnNodeFound)
                        {
                            QDomNamedNodeMap domReturnTypeDetails = nodeReturnType.attributes();
                            QString functionReturnParamName = domReturnTypeDetails.namedItem("name").nodeValue();
                            funcObj.setReturnParamName(functionReturnParamName);

                            QString functionReturnType = domReturnTypeDetails.namedItem("type").nodeValue();
                            funcObj.setReturnParamType(functionReturnType);

                            if(domReturnTypeDetails.contains("returnCode"))
                            {
                                GTAGenericAttribute funcReturnType;
                                funcReturnType.setAttributeName(functionReturnParamName);
                                funcReturnType.setAttributeType(functionReturnType);

                                QString returnCode = domReturnTypeDetails.namedItem("returnCode").nodeValue();
                                funcReturnType.setReturnCode(returnCode);

                                QString trueCond = domReturnTypeDetails.namedItem("trueCond").nodeValue();
                                funcReturnType.setTrueCondition(trueCond);

                                QString falseCond = domReturnTypeDetails.namedItem("falseCond").nodeValue();
                                funcReturnType.setFalseCondition(falseCond);

                                QString occurence = domReturnTypeDetails.namedItem("occurence").nodeValue();
                                funcReturnType.setAttributeOccurence(occurence);

                                QString waitUntil = domReturnTypeDetails.namedItem("waitUntil").nodeValue();
                                funcReturnType.setWaitUntil(waitUntil);


                                if(domReturnTypeDetails.contains("values"))
                                {
                                    QString paramAttributeValues = domReturnTypeDetails.namedItem("values").nodeValue();
                                    QStringList paramAttrValues = paramAttributeValues.split(";", QString::SkipEmptyParts);
                                    funcReturnType.setAttributeValues(paramAttrValues);
                                }

                                QString paramAttributeDescription = domReturnTypeDetails.namedItem("description").nodeValue();
                                funcReturnType.setAttributeDescription(paramAttributeDescription);

                                funcObj.setFunctionReturnType(funcReturnType);
                                funcObj.setIsReturnTypeSimple(true);
                            }

                        }
                    }
                }
                Functions.append(funcObj);

            }
        }
    }
    return Functions;
}

QList<GTAGenericParamData> GTAGenericToolController::parseDefinitions(QDomDocument *pDomDoc)
{
    //Parsing <Definitions> tag

    QList<GTAGenericParamData> Definitions;
    QDomNodeList nodeDefinitionsList = pDomDoc->elementsByTagName("definitions");
    QDomNode nodeDefinition;
    if(nodeDefinitionsList.count())
    {
        nodeDefinition = nodeDefinitionsList.at(0);                                   // Child nodes of <Definitions>
        if(nodeDefinition.hasChildNodes())
        {
            QDomNodeList nodeDataList = nodeDefinition.childNodes();                  //All <data>
            for(int i = 0; i < nodeDataList.size(); i++)
            {
                GTAGenericParamData definitionObj;
                QDomNode domData = nodeDataList.at(i);
                if(domData.nodeName() == "data")
                {
                    QDomNamedNodeMap domDataDetails = domData.attributes();
                    QString paramName = domDataDetails.namedItem("name").nodeValue();
                    definitionObj.setParamDataName(paramName);

                    QString paramType = domDataDetails.namedItem("type").nodeValue();
                    definitionObj.setParamDataType(paramType);

                    QDomNodeList paramAttributesTag = domData.childNodes();

                    if(paramAttributesTag.count())
                    {
                        QList<GTAGenericAttribute> Attributes;

                        QDomNodeList paramAttributeList = paramAttributesTag.at(0).childNodes();
                        for(int j = 0; j < paramAttributeList.size(); j++)
                        {
                            GTAGenericAttribute attributeObj;
                            QDomNode nodeAttribute = paramAttributeList.at(j);
                            if(nodeAttribute.nodeName() == "attribute")
                            {
                                QDomNamedNodeMap paramAttributeDetails = nodeAttribute.attributes();
                                QString paramAttributeName = paramAttributeDetails.namedItem("name").nodeValue();
                                attributeObj.setAttributeName(paramAttributeName);

                                QString paramAttributeType = paramAttributeDetails.namedItem("type").nodeValue();
                                attributeObj.setAttributeType(paramAttributeType);

                                QString paramAttributeOccurence = paramAttributeDetails.namedItem("occurence").nodeValue();
                                attributeObj.setAttributeOccurence(paramAttributeOccurence);

                                QString paramAttributeDescription = paramAttributeDetails.namedItem("description").nodeValue();
                                attributeObj.setAttributeDescription(paramAttributeDescription);

                                if(paramAttributeDetails.contains("values"))
                                {
                                    QString paramAttributeValues = paramAttributeDetails.namedItem("values").nodeValue();
                                    QStringList paramAttrValues = paramAttributeValues.split(";", QString::SkipEmptyParts);
                                    attributeObj.setAttributeValues(paramAttrValues);
                                }

                                if(paramAttributeDetails.contains("returnCode"))
                                {
                                    QString paramAttributeReturnCode = paramAttributeDetails.namedItem("returnCode").nodeValue();
                                    attributeObj.setReturnCode(paramAttributeReturnCode);

                                    QString trueCondition = paramAttributeDetails.namedItem("trueCond").nodeValue();
                                    attributeObj.setTrueCondition(trueCondition);

                                    QString fasleCondition = paramAttributeDetails.namedItem("falseCond").nodeValue();
                                    attributeObj.setFalseCondition(fasleCondition);
                                }
                                if(paramAttributeDetails.contains("waitUntil"))
                                {
                                    QString paramWaitUntil = paramAttributeDetails.namedItem("waitUntil").nodeValue();
                                    attributeObj.setWaitUntil(paramWaitUntil);
                                }
                            }
                            Attributes.append(attributeObj);
                        }
                        definitionObj.setParamAttributeList(Attributes);
                    }
                }
                Definitions.append(definitionObj);
            }
        }
    }
    return Definitions;
}

void GTAGenericToolController::parseGenericDBFile()
{
    GTASystemServices* sysServices = GTASystemServices::getSystemServices();
    QString genericDBFilePath = sysServices->getGenericDBPath();
    QDir dir(genericDBFilePath);

    if(dir.exists())
    {
        QString configFile = QDir::cleanPath(QString("%1%2%3").arg(genericDBFilePath,QDir::separator(),"Generic DB.xml"));
        QFile xmlFile(configFile);
        if(xmlFile.exists())
        {
            bool fileCheck = xmlFile.open(QFile::ReadOnly | QFile::Text);
            QDomDocument * pDomDoc = NULL;
            if (fileCheck)
            {
                pDomDoc = new QDomDocument();
                if(pDomDoc != NULL)
                {
                    if( ! pDomDoc->setContent(&xmlFile))
                    {
                        delete pDomDoc;
                        pDomDoc = NULL;
                      //  bool rc = false;
                        return;// rc;

                    }

                    QList<GTAGenericDB> UserDefDb;
                    QDomNodeList lstNodeDb = pDomDoc->elementsByTagName("DB");
                    for(int i = 0; i < lstNodeDb.count(); i++)
                    {
                        QDomNode nodeDb = lstNodeDb.at(i);
                        QDomNamedNodeMap attrNodeDb = nodeDb.attributes();
                        QString name = attrNodeDb.namedItem("name").nodeValue();
                        if(_UserDBNames.contains(name))
                        {
                            GTAGenericDB genDbObj;
                            genDbObj.setName(name);
                            QString relPath = attrNodeDb.namedItem("relativePath").nodeValue();
                            QString path = attrNodeDb.namedItem("path").nodeValue();
                            if (!relPath.isEmpty())
                            {
                                path = QString("%1%2").arg(sysServices->getLibraryPath(), relPath);
                            }
							genDbObj.setPath(path);
                            genDbObj.setRelativePath(relPath);

                            QString delimiter = attrNodeDb.namedItem("delimiter").nodeValue();
                            genDbObj.setDelimiter(delimiter);

                            QDomNode attributeNode = nodeDb.childNodes().at(0);
                            if(attributeNode.nodeName() == "attributes")
                            {
                                QList<int> colsConcatList;
                                QString cols = attributeNode.attributes().namedItem("columnToReadFromSearchResult").nodeValue();
                                QStringList temp = cols.split(";",QString::SkipEmptyParts);
                                for(int x = 0; x < temp.count(); x++)
                                {
                                    int val = temp.at(x).toInt();
                                    colsConcatList.append(val);
                                }
                                genDbObj.setColsConcatList(colsConcatList);

                                QString concatStr = attributeNode.attributes().namedItem("concatString").nodeValue();
                                genDbObj.setConcatString(concatStr);

                                QDomNodeList lstAttributes = attributeNode.childNodes();
                                for(int j = 0; j < lstAttributes.count(); j++)
                                {
                                    QDomNode attribute = lstAttributes.at(j);
                                    QString id = attribute.attributes().namedItem("id").nodeValue();
                                    QString attrName = attribute.attributes().namedItem("name").nodeValue();
                                    QString column = attribute.attributes().namedItem("column").nodeValue();
                                    genDbObj.addAttribute(id.toInt(),attrName, column.toInt());
                                }
                            }
                            UserDefDb.append(genDbObj);

                        }
                    }
                    _ToolConfigData.setUserDB(UserDefDb);
					delete pDomDoc;
					pDomDoc = NULL;
                }
            }
            xmlFile.close();
        }
    }
}

void GTAGenericToolController::parseConfigFile(const QString &iToolName, QDomDocument *pDomDoc)
{


    _ToolConfigData.setToolName(iToolName);

    //parsing <tool> for tool display name

    QDomElement toolDisplayNameElem = pDomDoc->documentElement();
    QString toolDisplayName = toolDisplayNameElem.attribute("toolDisplayName");
    if(!toolDisplayName.trimmed().isEmpty())
    {
        _ToolConfigData.setToolDisplayName(toolDisplayName);
    }

    QList<GTAGenericFunction> Functions = parseFunctions(pDomDoc);
    _ToolConfigData.setFunctions(Functions);

    if(!_UserDBNames.isEmpty())
        parseGenericDBFile();

    QList<GTAGenericParamData> Definitions = parseDefinitions(pDomDoc);
    _ToolConfigData.setDefinitions(Definitions);

    // parsing HMI tag

    QDomNodeList nodeHMIList = pDomDoc->elementsByTagName("HMI");
    QDomNode nodeHMI;
    if(nodeHMIList.count())
    {
        nodeHMI = nodeHMIList.at(0);
        QDomNamedNodeMap nodeHMIDetails = nodeHMI.attributes();
        if(nodeHMIDetails.contains("hasTimeout"))
        {
            QString hasTimeOut = nodeHMIDetails.namedItem("hasTimeout").nodeValue();
            if(hasTimeOut.toInt() == 0)
                _ToolConfigData.setTimeOut(false);
            else if(hasTimeOut.toInt() == 1)
                _ToolConfigData.setTimeOut(true);
        }
        if(nodeHMIDetails.contains("hasActionOnFail"))
        {
            QString hasActionOnFail = nodeHMIDetails.namedItem("hasActionOnFail").nodeValue();
            if(hasActionOnFail.toInt() == 0)
                _ToolConfigData.setActionOnFail(false);
            else if(hasActionOnFail.toInt() == 1)
                _ToolConfigData.setActionOnFail(true);
        }
        if(nodeHMIDetails.contains("hasComment"))
        {
            QString hasComment = nodeHMIDetails.namedItem("hasComment").nodeValue();
            if(hasComment.toInt() == 0)
                _ToolConfigData.setHasComment(false);
            else if(hasComment.toInt() == 1)
                _ToolConfigData.setHasComment(true);
        }
        if(nodeHMIDetails.contains("hasDumpList"))
        {
            QString hasDumpList = nodeHMIDetails.namedItem("hasDumpList").nodeValue();
            if(hasDumpList.toInt() == 0)
                _ToolConfigData.setHasDumpList(false);
            else if(hasDumpList.toInt() == 1)
                _ToolConfigData.setHasDumpList(true);
        }

    }
    
}


QList<GTAGenericFunction> GTAGenericToolController::parseInputFileFunctions(QDomDocument *pDomDoc)
{
    QList<GTAGenericFunction> Functions;
    QDomNodeList nodeItemList = pDomDoc->elementsByTagName("function");           // <functions> ... </functions>
    if (nodeItemList.count())
    {

        for(int i = 0; i < nodeItemList.size(); i++)
        {
            GTAGenericFunction funcObj;
            QDomNode domFunction = nodeItemList.at(i);
            if(domFunction.nodeName() == "function")                        // parsing each function and details like function name , type and type ID
            {
                QDomNamedNodeMap domFunctionAttr = domFunction.attributes();
                QString funcName = domFunctionAttr.namedItem("name").nodeValue();
                funcObj.setFunctionName(funcName);

                //bool rc = domFunction.hasChildNodes();
                QDomNodeList domArgumetList = domFunction.childNodes();                            // Arguments and Return types are two child nodes of function tag
                if(domArgumetList.count())
                {
                    QList<GTAGenericArgument> FunctionArguments;
                    for(int j = 0; j < domArgumetList.size(); j++)
                    {
                        GTAGenericArgument argumentObj;
                        QDomNode nodeArgument = domArgumetList.at(j);
                        if(nodeArgument.nodeName() == "argument")
                        {
                            QDomNamedNodeMap domArgumentDetails = nodeArgument.attributes();
                            QString argumentName = domArgumentDetails.namedItem("name").nodeValue();
                            argumentObj.setArgumentName(argumentName);

                            QString argumentType = domArgumentDetails.namedItem("type").nodeValue();
                            argumentObj.setArgumentType(argumentType);


                            if(domArgumentDetails.contains("value"))
                            {
                                QString argumentValues = domArgumentDetails.namedItem("value").nodeValue();
                                QStringList valuesList = argumentValues.split(";",QString::SkipEmptyParts);
                                argumentObj.setValues(valuesList);
                            }


                            FunctionArguments.append(argumentObj);
                        }
                        if(nodeArgument.nodeName() == "returnvalue")
                        {
                            QDomNamedNodeMap domReturnTpeDetails = nodeArgument.attributes();
                            QString functionReturnParamName = domReturnTpeDetails.namedItem("name").nodeValue();
                            funcObj.setReturnParamName(functionReturnParamName);

                            QString functionReturnType = domReturnTpeDetails.namedItem("type").nodeValue();
                            funcObj.setReturnParamType(functionReturnType);
                        }

                    }

                    funcObj.setFunctionArguments(FunctionArguments);
                }
            }
            Functions.append(funcObj);
        }
    }

    return Functions;
}

void GTAGenericToolController::parseInputFile(const QString &, QDomDocument *pDomDoc)
{
    //QStringList toolNameSplit = iInputFile.split(QDir::separator(),QString::SkipEmptyParts);
    //QString toolName = toolNameSplit.last();

    //_ToolInputData.setToolName(iInputFile);
    _ToolInputData.setToolName(_ToolConfigData.getToolName());

    QList<GTAGenericFunction> Functions = parseInputFileFunctions(pDomDoc);
    _ToolInputData.setFunctions(Functions);


    /*  QList<GTAGenericParamData> Definitions = parseDefinitions(pDomDoc);
    _ToolInputData.setDefinitions(Definitions);

    // parsing HMI tag

    QDomNodeList nodeHMIList = pDomDoc->elementsByTagName("HMI");
    QDomNode nodeHMI;
    if(nodeHMIList.count())
    {
        nodeHMI = nodeHMIList.at(0);
        QDomNamedNodeMap nodeHMIDetails = nodeHMI.attributes();
        if(nodeHMIDetails.contains("hasTimeout"))
        {
            QString hasTimeOut = nodeHMIDetails.namedItem("hasTimeout").nodeValue();
            if(hasTimeOut.toInt() == 0)
                _ToolInputData.setTimeOut(false);
            else if(hasTimeOut.toInt() == 1)
                _ToolInputData.setTimeOut(true);
        }
        if(nodeHMIDetails.contains("hasActionOnFail"))
        {
            QString hasActionOnFail = nodeHMIDetails.namedItem("hasActionOnFail").nodeValue();
            if(hasActionOnFail.toInt() == 0)
                _ToolInputData.setActionOnFail(false);
            else if(hasActionOnFail.toInt() == 1)
                _ToolInputData.setActionOnFail(true);
        }
    } */
}

QString GTAGenericToolController::getPathByToolName(const QString &iToolName)
{
    QString configFilePath = "";
    if(!(_AllGenTools.isEmpty()))
    {
        if(_AllGenTools.contains(iToolName))
            configFilePath = _AllGenTools.value(iToolName);

    }
    return configFilePath;
}

QList<GTAGenericDB> GTAGenericToolController::getUserDefinedDb()
{
    return _ToolConfigData.getUserDB();
}


QList<GTAGenericFunction> GTAGenericToolController::getGenericToolFunctions(const QString &iToolName)
{
    bool rc = ParseToolConfigFile(iToolName);
	QList<GTAGenericFunction> EmptyList;
	if(rc)
        return _ToolConfigData.getFunctions();

	return EmptyList;
}

QList<GTAGenericParamData> GTAGenericToolController::getGenericToolDefintions(const QString &iToolName)
{
    QList<GTAGenericParamData> EmptyList;
    if(iToolName == _ToolConfigData.getToolName())
    {
        return _ToolConfigData.getDefinitions();
    }
    else
        return EmptyList;
}

/*QList<GTAGenericAttribute> GTAGenericToolController::getGenericToolFunctionReturn(const QString &iReturnName)
{
    QList<GTAGenericAttribute> attrList;
    GTAGenericParamData data;
    QList<GTAGenericParamData> defs = _ToolConfigData.getDefinitions();
    bool found = false;
    for(int i=0;i<defs.count();i++)
    {
        data = defs.at(i);
        if(data.getParamDataName() == iReturnName)
        {
            found = true;
            break;
            
        }
    }

    if(found)
    {
        attrList = data.getParamAttributeList();
    }

    return attrList;
}*/

QList<GTAGenericFunction> GTAGenericToolController::matchConfigAndInput()
{

    QList<GTAGenericFunction> ToolFunctions = _ToolConfigData.getFunctions();
    QList<GTAGenericFunction> InputFunctions = _ToolInputData.getFunctions();

    for(int i = 0; i < InputFunctions.count(); i++)
    {

        GTAGenericFunction input = InputFunctions.at(i);
        if(ToolFunctions.contains(input))
            continue;
        else
            InputFunctions.removeAt(i);
    }

    return InputFunctions;
}

QList<GTAGenericFunction> GTAGenericToolController::getInputFileFunctions(const QString &iInputFileName)
{
    ParseInputFile(iInputFileName);
    QList<GTAGenericFunction> InputFunctions = matchConfigAndInput();
    return InputFunctions;
}


bool GTAGenericToolController::hasActionOnfail()
{
    return _ToolConfigData.isActionOnFail();
}

bool GTAGenericToolController::hasTimeOut()
{
    return _ToolConfigData.isTimeOut();
}

bool GTAGenericToolController::hasDumpList()
{
    return _ToolConfigData.hasDumpList();
}

bool GTAGenericToolController::hasComment()
{
    return _ToolConfigData.hasComment();
}
