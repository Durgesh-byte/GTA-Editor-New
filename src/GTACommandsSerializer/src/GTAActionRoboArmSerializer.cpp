#include "GTAActionRoboArmSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionRoboArm.h"
#include "GTARequirementSerializer.h"
GTAActionRoboArmSerializer::GTAActionRoboArmSerializer()
{
    _pActionCmd = NULL;
}
void GTAActionRoboArmSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionRoboArm*>((GTACommandBase*)ipCmd);
}
bool GTAActionRoboArmSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        
        QString functionName = _pActionCmd->getFunction();
        QString sequenceFile = _pActionCmd->getSequenceFile();
        //QList<QDomNode> xmlRpcContents = _pActionCmd->getFunctionCode();
        QByteArray xmlrpcDocRep = _pActionCmd->getFunctionCode();

        oElement  = iDomDoc.createElement(XNODE_ACTION);
        
        //QString actionName = _pActionCmd->getAction();
        //QString complement = _pActionCmd->getComplement();
        //oElement.setAttribute(XNODE_NAME,actionName);
        //oElement.setAttribute(XNODE_COMPLEMENT,complement);
        setActionAttributes(_pActionCmd,oElement);


        QDomElement domFunction = iDomDoc.createElement(XNODE_FUNCTION);
        domFunction.setAttribute(XNODE_NAME,functionName);
        domFunction.setAttribute(XNODE_FILE,sequenceFile);
        QDomElement domContents = iDomDoc.createElement(XNODE_CONTENTS);
        domFunction.appendChild(domContents);

        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);



        QDomDocument xmlrpcDomDoc;
        xmlrpcDomDoc.setContent(xmlrpcDocRep);
        QDomElement domRootElem = xmlrpcDomDoc.documentElement();
        QDomNodeList xmlRpcContents = domRootElem.childNodes();
        int nbOfSteps = xmlRpcContents.count();
        for(int i = 0; i <nbOfSteps; i++)
        {
            QDomNode xmlRpcNode = xmlRpcContents.item(i);
            if(!xmlRpcNode.isNull())
            {
                QDomNode importedNode = iDomDoc.importNode(xmlRpcNode,true);
                domContents.appendChild(importedNode);
            }
        }

        oElement.appendChild(domFunction);

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

bool GTAActionRoboArmSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    if(!iElement.isNull() && iElement.nodeName() == XNODE_ACTION)
    {   QString actionName = iElement.attribute(XNODE_NAME);

        if(actionName != ACT_ROBO_ARM)
            return rc;
        else
            rc = true;

        QString complementName = iElement.attribute(XNODE_COMPLEMENT);
        QByteArray xmlRpcNodeList;
        QDomNodeList childNodeList =iElement.childNodes();
        QString sequenceFile;
        QString functionName ;
        // bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        
        for(int i = 0; i <childNodeList.count(); i++)
        {
            QDomNode childNode = childNodeList.at(i);
            if(childNode.isElement() && childNode.nodeName() == XNODE_FUNCTION)
            {
                QDomElement domElem = childNode.toElement();
                functionName = domElem.attribute(XNODE_NAME);
                functionName.replace(" ","_");
                sequenceFile = domElem.attribute(XNODE_FILE);
                QDomNodeList domFuncChildList = domElem.childNodes();
                for(int j = 0; j< domFuncChildList.count(); j++)
                {
                    QDomNode domContents = domFuncChildList.at(j);
                    if(domContents.isElement() && domContents.nodeName() == XNODE_CONTENTS)
                    {
                        QDomDocument xmlrpcDomDoc;//(/*sequenceFile*/);
                        QDomElement xmlrpcRootElem = xmlrpcDomDoc.createElement(functionName);
                        xmlrpcDomDoc.appendChild(xmlrpcRootElem);

                        QDomNodeList domFuncCodeList = domContents.toElement().childNodes();
                        for(int  k = 0; k < domFuncCodeList.count(); k++)
                        {
                            QDomNode xmlrpcNode = xmlrpcDomDoc.importNode(domFuncCodeList.item(k),true);
                            xmlrpcRootElem.appendChild(xmlrpcNode);
                        }
                        xmlRpcNodeList = xmlrpcDomDoc.toByteArray();
                        break;
                    }
                }
                break;
            }
        }
        GTAActionRoboArm * pIrtCmd = new GTAActionRoboArm(actionName,complementName,functionName);
        getActionAttributes(iElement,pIrtCmd);
        pIrtCmd->setSequenceFile(sequenceFile);
        pIrtCmd->setFunctionCode(xmlRpcNodeList);
        //pIrtCmd->setIgnoreInSCXML(ignoreInSCXML);
        opCommand = pIrtCmd;

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
REGISTER_SERIALIZER(QString("%1").arg(ACT_ROBO_ARM),GTAActionRoboArmSerializer)
