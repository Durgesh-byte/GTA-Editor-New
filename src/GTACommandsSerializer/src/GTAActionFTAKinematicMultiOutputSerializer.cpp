#include "GTAActionFTAKinematicMultiOutputSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionFTAKinematicMultiOutput.h"
#include "GTARequirementSerializer.h"

GTAActionFTAKinematicMultiOutputSerializer::GTAActionFTAKinematicMultiOutputSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionFTAKinematicMultiOutputSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd=dynamic_cast<GTAActionFTAKinematicMultiOutput*> ((GTACommandBase*)ipCmd);
}
bool GTAActionFTAKinematicMultiOutputSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)

{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_FCTL)
            return rc;
        else
            rc = true;

       
        QString name = _pActionCmd->getCurrentName();

        QString sRefFilePath = _pActionCmd->getRefFilePath();
        QStringList lstRefFileData = _pActionCmd->getRefFileData();

        QStringList  lstInputVariables = _pActionCmd->getInputVariableList();
        QStringList  lstOutputVariables = _pActionCmd->getOutputVariableList();
        QMap<QString,QString> mapVars = _pActionCmd->getTagValuePairForTemapltes();
        QString sEpsilon1 = _pActionCmd->getEpsilon1();
        QString sEpsilon2 = _pActionCmd->getEpsilon2();
        QString sNoOfLines = _pActionCmd->getNoOfLines();
        QString sWaitPilOrder =  _pActionCmd->getWaitPilOrder();
        oElement  = iDomDoc.createElement(XNODE_ACTION);
 
       /* QString complement = _pActionCmd->getComplement(); 
       
        oElement.setAttribute(XNODE_NAME,actionName);
        oElement.setAttribute(XNODE_COMPLEMENT,complement);
        
        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);*/
        setActionAttributes(_pActionCmd,oElement);

        oElement.setAttribute(XATTR_TMPL_NAME,name);
        QDomElement ElemOneEpsilon1= iDomDoc.createElement(XNODE_PARAMETER);
        ElemOneEpsilon1.setAttribute(XATTR_TYPE,XATTR_EPSILON1);
        ElemOneEpsilon1.setAttribute(XATTR_VAL,sEpsilon1);
        oElement.appendChild(ElemOneEpsilon1);


        QDomElement ElemOneEpsilon2= iDomDoc.createElement(XNODE_PARAMETER);
        ElemOneEpsilon2.setAttribute(XATTR_TYPE,XATTR_EPSILON2);
        ElemOneEpsilon2.setAttribute(XATTR_VAL,sEpsilon2);
        oElement.appendChild(ElemOneEpsilon2);

        QDomElement ElemNoOfLines= iDomDoc.createElement(XNODE_PARAMETER);
        ElemNoOfLines.setAttribute(XATTR_TYPE,XATTR_NO_OF_LINES);
        ElemNoOfLines.setAttribute(XATTR_VAL,sNoOfLines);
        oElement.appendChild(ElemNoOfLines);

        QDomElement ElemOnWaitPilOrder= iDomDoc.createElement(XNODE_PARAMETER);
        ElemOnWaitPilOrder.setAttribute(XATTR_TYPE,XATTR_PILE_ORDER);
        ElemOnWaitPilOrder.setAttribute(XATTR_VAL,sWaitPilOrder);
        oElement.appendChild(ElemOnWaitPilOrder);

        for (int i=0;i<lstInputVariables.size();i++)
        {
            QString sVar = _pActionCmd->getInputVariable(i);
            QDomElement ElementinputVariable= iDomDoc.createElement(XNODE_PARAMETER);
            ElementinputVariable.setAttribute(XATTR_TYPE,XATTR_INPUT_VAR);
            ElementinputVariable.setAttribute(XATTR_VAL,sVar);
            ElementinputVariable.setAttribute(XATTR_ORDER,QString::number(i));
            oElement.appendChild(ElementinputVariable);

        }
        for (int i=0;i<lstOutputVariables.size();i++)
        {
            QString sVar = _pActionCmd->getOutputVariable(i);
            QDomElement ElementOutputVar= iDomDoc.createElement(XNODE_PARAMETER);
            ElementOutputVar.setAttribute(XATTR_TYPE,XATTR_OUTPUT_VAR);
            ElementOutputVar.setAttribute(XATTR_VAL,sVar);
            ElementOutputVar.setAttribute(XATTR_ORDER,QString::number(i));
            oElement.appendChild(ElementOutputVar);

        }
        foreach(QString key, mapVars.keys())
        {
            QString sVar = mapVars.value(key);
            QDomElement ElementMapVar= iDomDoc.createElement(XNODE_PARAMETER);
            ElementMapVar.setAttribute(XATTR_TYPE,XATTR_MAP_VAR);
            ElementMapVar.setAttribute(XATTR_NAME,key);
            ElementMapVar.setAttribute(XATTR_VAL,sVar);
            oElement.appendChild(ElementMapVar);
        }
        QDomElement ElementrefFileData= iDomDoc.createElement(XNODE_PARAMETER);
        ElementrefFileData.setAttribute(XATTR_TYPE,XATTR_REF_FILE_DATA);
        ElementrefFileData.setAttribute(XATTR_VAL,lstRefFileData.join("$$"));
        oElement.appendChild(ElementrefFileData);

        QDomElement ElementrefPath= iDomDoc.createElement(XNODE_PARAMETER);
        ElementrefPath.setAttribute(XATTR_TYPE,XATTR_REF_PATH);
        ElementrefPath.setAttribute(XATTR_VAL,sRefFilePath);       
        oElement.appendChild(ElementrefPath);
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

        
    }
    
    return rc;
}

bool GTAActionFTAKinematicMultiOutputSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        //QString actionName = iElement.attribute(XNODE_NAME);
        //QString compliment = iElement.attribute(XNODE_COMPLEMENT);
        QString name = iElement.attribute(XATTR_TMPL_NAME);


        QStringList     lstRefFileData ;
        QStringList     lstInputVariables;
        QStringList     lstOutputVariables;

        QString         sRefFilePath;
        QString         sEpsilon1;
        QString         sEpsilon2;
        QString         sNoOfLines;
        QString         sWaitPilOrder;


        GTAActionFTAKinematicMultiOutput * pCmd = new GTAActionFTAKinematicMultiOutput();
       /* pCmd->setAction(actionName);
        pCmd->setComplement(compliment);*/
        getActionAttributes(iElement,pCmd);
        pCmd->setCurrentName(name);
        QDomNodeList parameterNodeList = iElement.elementsByTagName(XNODE_PARAMETER);
        if (!parameterNodeList.isEmpty())
        {
            rc = true;
            for (int i=0;i<parameterNodeList.size();i++)
            {
                QDomElement parameterElement = parameterNodeList.at(i).toElement();
                if (!parameterElement.isNull())
                {
                    QString sTypeOfParameter = parameterElement.attribute(XATTR_TYPE);
                    if (sTypeOfParameter == XATTR_INPUT_VAR)
                    {
                        QString sOrder = parameterElement.attribute(XATTR_ORDER);
                        bool conversionStatus = false;
                        int iOrder = sOrder.toInt(&conversionStatus);
                        if(conversionStatus)
                            pCmd->insertInputVariable(iOrder,parameterElement.attribute(XATTR_VAL));
                    }
                    else if (sTypeOfParameter == XATTR_OUTPUT_VAR)
                    {
                        QString sOrder = parameterElement.attribute(XATTR_ORDER);
                        bool conversionStatus = false;
                        int iOrder = sOrder.toInt(&conversionStatus);
                        if(conversionStatus)
                            pCmd->insertMonitoredVariable(iOrder,parameterElement.attribute(XATTR_VAL));
                    }
                    else if(sTypeOfParameter == XATTR_MAP_VAR)
                    {
                        QString sOrder = parameterElement.attribute(XATTR_ORDER);
                        QString key = parameterElement.attribute(XATTR_NAME);
                        QString val = parameterElement.attribute(XATTR_VAL);
                        pCmd->insertTagValuepairForTemplates(key,val);
                    }
                    else if (sTypeOfParameter == XATTR_REF_FILE_DATA)
                    {
                        lstRefFileData.append(parameterElement.attribute(XATTR_VAL).split("$$"));
                    }
                    else if (sTypeOfParameter == XATTR_REF_PATH)
                    {
                        sRefFilePath = parameterElement.attribute(XATTR_VAL);
                    }
                    else if (sTypeOfParameter == XATTR_EPSILON1)
                    {
                        sEpsilon1 = parameterElement.attribute(XATTR_VAL);
                    }
                    else if (sTypeOfParameter == XATTR_EPSILON2)
                    {
                        sEpsilon2 = parameterElement.attribute(XATTR_VAL);

                    }
                    else if (sTypeOfParameter == XATTR_NO_OF_LINES)
                    {
                        sNoOfLines = parameterElement.attribute(XATTR_VAL);
                    }
                    else if (sTypeOfParameter == XATTR_PILE_ORDER)
                    {
                        sWaitPilOrder = parameterElement.attribute(XATTR_VAL);
                    }
                }
            }

            pCmd->setRefFilePath(sRefFilePath);
            pCmd->setEpsilon1(sEpsilon1);
            pCmd->setEpsilon2(sEpsilon2);
            pCmd->setNoOfLines(sNoOfLines);
            pCmd->setWaitPilOrder(sWaitPilOrder);
            pCmd->setRefFileData(lstRefFileData);

        }


        opCommand = pCmd;
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
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_FCTL,COM_FCTL_KINEMATIC_MULTI_OUTPUT),GTAActionFTAKinematicMultiOutputSerializer)
