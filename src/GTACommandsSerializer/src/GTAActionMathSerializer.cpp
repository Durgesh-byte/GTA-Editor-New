#include "GTAActionMathSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionMath.h"
#include "GTARequirementSerializer.h"

GTAActionMathSerializer::GTAActionMathSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
GTAActionMathSerializer::~GTAActionMathSerializer()
{
    _pActionCmd = NULL;
}
void GTAActionMathSerializer::setCommand(const GTACommandBase* iPCmd)
{
    _pActionCmd = dynamic_cast<GTAActionMath*>((GTACommandBase*) iPCmd);
}
bool GTAActionMathSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)

{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_MATH)
            return rc;
        else
            rc = true;

       


        QString sParameter1 = _pActionCmd->getFirstVariable();
		QString sParameter2 = _pActionCmd->getSecondVariable();
		QString sLocation   = _pActionCmd->getLocation();
		QString sOperator   = _pActionCmd->getOperator();

        oElement  = iDomDoc.createElement(XNODE_ACTION);
 

        setActionAttributes(_pActionCmd,oElement);



            QDomElement ElemOnOperator = iDomDoc.createElement(XNODE_OPERATOR);
            ElemOnOperator.setAttribute(XNODE_NAME,sOperator);
            oElement.appendChild(ElemOnOperator);


            QDomElement ElemOnParam1 = iDomDoc.createElement(XNODE_PARAMETER1);
            ElemOnParam1.setAttribute(XNODE_NAME,sParameter1);
            oElement.appendChild(ElemOnParam1);
			
			
			QDomElement ElemOnParam2 = iDomDoc.createElement(XNODE_PARAMETER2);
            ElemOnParam2.setAttribute(XNODE_NAME,sParameter2);
            oElement.appendChild(ElemOnParam2);
			
			
			QDomElement ElemOnLocation = iDomDoc.createElement(XNODE_LOCATION);
            ElemOnLocation.setAttribute(XNODE_NAME,sLocation);
            oElement.appendChild(ElemOnLocation);			
			
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

bool GTAActionMathSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        QString actionName = iElement.attribute(XNODE_NAME);
        QString compliment = iElement.attribute(XNODE_COMPLEMENT);

		QString sParameter1;
		QString sParameter2;
		QString sLocation;
		QString sOperator;

        bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
       

        GTAActionMath * pCmd = new GTAActionMath();
        pCmd->setAction(actionName);
        pCmd->setComplement(compliment);
        pCmd->setIgnoreInSCXML(ignoreInSCXML);
            QDomNode operatorNode = iElement.namedItem(XNODE_OPERATOR);
            if(!operatorNode.isNull())
            {
                QDomElement ElemOnOperator = operatorNode.toElement();
                if(ElemOnOperator.nodeName() == XNODE_OPERATOR)
                    sOperator = ElemOnOperator.attribute(XNODE_NAME);
            }
            QDomNode ParameterNode1 = iElement.namedItem(XNODE_PARAMETER1);
            if(!ParameterNode1.isNull())
            {
                QDomElement ElemOnParam1 = ParameterNode1.toElement();
                if(ElemOnParam1.nodeName() == XNODE_PARAMETER1)
                    sParameter1 = ElemOnParam1.attribute(XNODE_NAME);
            }
            QDomNode ParameterNode2 = iElement.namedItem(XNODE_PARAMETER2);
            if(!ParameterNode2.isNull())
            {
                QDomElement ElemOnParam2 = ParameterNode2.toElement();
                if(ElemOnParam2.nodeName() == XNODE_PARAMETER2)
                    sParameter2 = ElemOnParam2.attribute(XNODE_NAME);
            }
            QDomNode LocationNode = iElement.namedItem(XNODE_LOCATION);
            if(!LocationNode.isNull())
            {
                QDomElement ElemOnLocation = LocationNode.toElement();
                if(ElemOnLocation.nodeName() == XNODE_LOCATION)
                    sLocation = ElemOnLocation.attribute(XNODE_NAME);
            }
			
			pCmd->setFirstVariable(sParameter1);
			pCmd->setSecondVariable(sParameter2);
			pCmd->setLocation(sLocation);
			pCmd->setOperator(sOperator);
			
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
REGISTER_SERIALIZER(QString("%1").arg(ACT_MATH),GTAActionMathSerializer)
