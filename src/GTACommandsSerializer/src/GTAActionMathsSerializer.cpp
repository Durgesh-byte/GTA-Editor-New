#include "GTAActionMathsSerializer.h"
#include "GTAActionMaths.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAMathExpression.h"
#include "GTATokenConst.h"
#include "GTATokenEndBracket.h"
#include "GTATokenFunction.h"
#include "GTATokenFunSeperator.h"
#include "GTATokenOperator.h"
#include "GTATokenStartBracket.h"
#include "GTATokenVariable.h"
#include "GTATokenCommaSeparator.h"
#include "GTARequirementSerializer.h"

#pragma warning(push, 0)
#include <QDomNodeList>
#include <QDebug>
#pragma warning(pop)

GTAActionMathsSerializer::GTAActionMathsSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionMathsSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionMaths*>((GTACommandBase*)ipCmd);
}
bool GTAActionMathsSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)

{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString actionName = _pActionCmd->getAction();

        if(actionName != ACT_MATHS)
            return rc;
        else
            rc = true;

        


        QString sParameter1 = _pActionCmd->getFirstVariable();
        QString expressionTxt = _pActionCmd->getExpressionTxt();
		

        oElement  = iDomDoc.createElement(XNODE_ACTION);
        //QString complement = _pActionCmd->getComplement();
        //oElement.setAttribute(XNODE_NAME,actionName);
        //oElement.setAttribute(XNODE_COMPLEMENT,complement);
        //QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        //oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);
        setActionAttributes(_pActionCmd,oElement);

        QDomElement ElemOnParam1 = iDomDoc.createElement(XNODE_PARAMETER1);
        ElemOnParam1.setAttribute(XNODE_NAME,sParameter1);
        oElement.appendChild(ElemOnParam1);

        QDomElement expElemTxt = iDomDoc.createElement(XNODE_EXP_TXT);
        QDomText txtNode = iDomDoc.createTextNode(expressionTxt);
        expElemTxt.appendChild(txtNode);
        oElement.appendChild(expElemTxt);

       


        GTAMathExpression* pExp = _pActionCmd->getExpression();
        pExp->serializeExpression(oElement,iDomDoc);
			
			
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

bool GTAActionMathsSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)
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
        QString sExpressionTxt;


        GTAActionMaths * pCmd = new GTAActionMaths();
        //pCmd->setAction(actionName);
        //pCmd->setComplement(compliment);
        //bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;
        //pCmd->setIgnoreInSCXML(ignoreInSCXML);
        getActionAttributes(iElement,pCmd);
            
        QDomNode ParameterNode1 = iElement.namedItem(XNODE_PARAMETER1);
        if(!ParameterNode1.isNull())
        {
            QDomElement ElemOnParam1 = ParameterNode1.toElement();
            if(ElemOnParam1.nodeName() == XNODE_PARAMETER1)
                sParameter1 = ElemOnParam1.attribute(XNODE_NAME);
        }
       
        QDomNode expElementTxt = iElement.namedItem(XNODE_EXP_TXT);
        if (!expElementTxt.isNull())
        {
            sExpressionTxt = expElementTxt.toElement().text();
            pCmd->setExpressionTxt(sExpressionTxt);

        }
        QDomElement expressionElement;
        QDomNodeList childrens = iElement.childNodes();
        for (int i=0;i<childrens.size();i++)
        {
            QDomElement currentElement = childrens.at(i).toElement();
            if (!currentElement.isNull() && currentElement.nodeName()=="Expression")
            {
                expressionElement=currentElement;
                break;
            }
        }
		GTAMathExpression* pExpression = getExpressionFromDom(expressionElement);
        if(NULL!=pExpression)
        {
            pCmd->setExpression(pExpression);
        }
        else
            pCmd->setExpression(new GTAMathExpression);
		pCmd->setFirstVariable(sParameter1);
		
			
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
GTAMathExpression* GTAActionMathsSerializer::getExpressionFromDom(const QDomElement& iExpressionElement,bool isFunctionArgument)
{
   

    if (iExpressionElement.isNull() || iExpressionElement.nodeName()!="Expression")
    {
        
        return NULL;
       
    }
    else
    {
        GTAMathExpression* pExpression = new GTAMathExpression(isFunctionArgument);
        QDomNodeList children = iExpressionElement.childNodes();
        for (int i=0;i<children.count();i++)
        {
            QDomElement currentElement = children.at(i).toElement();
            if (!currentElement.isNull())
            {
                QString nodeName = currentElement.nodeName();
                QString sExp = currentElement.attribute("Representation");
                if (nodeName == "StartBracket")
                {
                    pExpression->pushToken(new GTATokenStartBracket(sExp));
                }
                else if (nodeName == "Variable")
                {
                    pExpression->pushToken(new GTATokenVariable(sExp));
                }
                else if (nodeName == "EndBracket")
                {
                    pExpression->pushToken(new GTATokenEndBracket(sExp));
                }
                else if (nodeName == "Operator")
                {
                    pExpression->pushToken(new GTATokenOperator(sExp));
                }
                else if (nodeName == "Constant")
                {
                    pExpression->pushToken(new GTATokenConst(sExp));
                }
                else if (nodeName == "FunctionSeperator")
                {
                    pExpression->pushToken(new GTATokenFunSeperator(sExp));
                }
                else if(nodeName == "CommaSeparator")
                {
                    pExpression->pushToken(new GTATokenCommaSeparator(sExp));
                }
                else if (nodeName == "Function")
                {
                    QString argSize = currentElement.attribute("ArgSize");
                    GTATokenFunction* pFunction = new GTATokenFunction(sExp,argSize.toInt());
                    QDomElement expressionElement;
                    QDomNodeList subChildren = currentElement.childNodes();
                    for (int j=0; j < subChildren.size(); j++)
                    {
                        QDomElement subCurrentElement = subChildren.at(j).toElement();
                        if (!currentElement.isNull() && subCurrentElement.nodeName()=="Expression")
                        {
                            expressionElement= subCurrentElement;
                            break;
                        }
                    }

                    GTAMathExpression* pFunExp = getExpressionFromDom(expressionElement,true);
                    pFunction->setClosed(true);
                    pFunction->setExpression(pFunExp);
                    pExpression->pushToken(pFunction);
                }
            }
        }
        return pExpression;
    }
}



#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1").arg(ACT_MATHS),GTAActionMathsSerializer)
