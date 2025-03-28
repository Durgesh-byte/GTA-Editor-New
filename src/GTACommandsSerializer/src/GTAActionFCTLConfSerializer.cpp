#include "GTAActionFCTLConfSerializer.h"
#include "GTAUtil.h"
#include "GTACommentSerializer.h"
#include "GTAActionFCTLConf.h"
#include "GTARequirementSerializer.h"

GTAActionFCTLConfSerializer::GTAActionFCTLConfSerializer():GTACmdSerializerInterface()
{
    _pActionCmd = NULL;
}
void GTAActionFCTLConfSerializer::setCommand(const GTACommandBase* ipCmd)
{
    _pActionCmd = dynamic_cast<GTAActionFCTLConf*>((GTACommandBase*) ipCmd);
}

bool GTAActionFCTLConfSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)

{
    bool rc = false;
    if(_pActionCmd != NULL)
    {
        QString actionName = _pActionCmd->getAction(); 

        if(actionName != ACT_FCTL)
            return rc;
        else
            rc = true;
        oElement  = iDomDoc.createElement(XNODE_ACTION);

        /*QString complement = _pActionCmd->getComplement();

        oElement.setAttribute(XNODE_NAME,actionName);
        oElement.setAttribute(XNODE_COMPLEMENT,complement);
        QString hiddenInSCXML = _pActionCmd->isIgnoredInSCXML() ? XNODE_TRUE : XNODE_FALSE;
        oElement.setAttribute(XATTR_IGNORE_SCXML,hiddenInSCXML);*/

        setActionAttributes(_pActionCmd,oElement);


        QString	sConfigName	    =   _pActionCmd->getConfigName() ;	
        QDomElement elementOnConfigName= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnConfigName.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFCONFIGNAME);
        elementOnConfigName.setAttribute(XATTR_VAL,sConfigName);
        oElement.appendChild(elementOnConfigName);


        QString	sLaw	        =	_pActionCmd->getLAW() ;	
        QDomElement elementOnLAW= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnLAW.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFLAW);
        elementOnLAW.setAttribute(XATTR_VAL,sLaw);
        oElement.appendChild(elementOnLAW);

        QString	sSFConf	        =	_pActionCmd->getSFConf() ;	
        QDomElement elementsFConfig= iDomDoc.createElement(XNODE_PARAMETER);
        elementsFConfig.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFSFCONF);
        elementsFConfig.setAttribute(XATTR_VAL,sSFConf);
        oElement.appendChild(elementsFConfig);

        QString	sGrndFlight     =	_pActionCmd->getGrndFlight() ;
        QDomElement elementOnGrdFlight= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnGrdFlight.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFGRNDFLIGHT);
        elementOnGrdFlight.setAttribute(XATTR_VAL,sGrndFlight);
        oElement.appendChild(elementOnGrdFlight);

        QString	sEngines    	=	_pActionCmd->getEngines() ;	
        QDomElement elementOnEngine= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnEngine.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFENGINES);
        elementOnEngine.setAttribute(XATTR_VAL,sEngines);
        oElement.appendChild(elementOnEngine);

        QString	sLandingGear    =	_pActionCmd->getLandingGear() ;	
        QDomElement elementOnLandingGear= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnLandingGear.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFLANDINGGEAR);
        elementOnLandingGear.setAttribute(XATTR_VAL,sLandingGear);
        oElement.appendChild(elementOnLandingGear);

        QString	sCG             =   _pActionCmd->getCG() ;	
        QDomElement elementOnCG= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnCG.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFCG);
        elementOnCG.setAttribute(XATTR_VAL,sCG);
        oElement.appendChild(elementOnCG);

        QString	sGrossWt    	=	_pActionCmd->getGrossWt() ;	
        QDomElement elementOnGrossWt= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnGrossWt.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFGROSSWT);
        elementOnGrossWt.setAttribute(XATTR_VAL,sGrossWt);
        oElement.appendChild(elementOnGrossWt);


        QString	sVCAS       	=	_pActionCmd->getVCAS() ;	
        QDomElement elementOnVCAS= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnVCAS.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFVCAS);
        elementOnVCAS.setAttribute(XATTR_VAL,sVCAS);
        oElement.appendChild(elementOnVCAS);


        QString	sAltitude   	=	_pActionCmd->getAltitude() ;
        QDomElement elementOnAltitude= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnAltitude.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFALTITUDE);
        elementOnAltitude.setAttribute(XATTR_VAL,sAltitude);
        oElement.appendChild(elementOnAltitude);


        QString	sYHYD	        =	_pActionCmd->getYHYD() ;	
        QDomElement elementOnYHYD= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnYHYD.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFYHYD);
        elementOnYHYD.setAttribute(XATTR_VAL,sYHYD);
        oElement.appendChild(elementOnYHYD);

        QString	sGHYD	        =	_pActionCmd->getGHYD() ;	
        QDomElement elementOnGHYD= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnGHYD.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFGHYD);
        elementOnGHYD.setAttribute(XATTR_VAL,sGHYD);
        oElement.appendChild(elementOnGHYD);

        QString	sBHYD	        =	_pActionCmd->getBHYD() ;	
        QDomElement elementOnBHYD= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnBHYD.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFBHYD);
        elementOnBHYD.setAttribute(XATTR_VAL,sBHYD);
        oElement.appendChild(elementOnBHYD);


        QString	sAlpha	        =	_pActionCmd->getAlpha() ;	
        QDomElement elementOnAlpha= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnAlpha.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFALPHA);
        elementOnAlpha.setAttribute(XATTR_VAL,sAlpha);
        oElement.appendChild(elementOnAlpha);

        QString	sFCPC1	        =	_pActionCmd->getFCPC1() ;
        QDomElement elementOnFCPC1= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnFCPC1.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFFCPC1);
        elementOnFCPC1.setAttribute(XATTR_VAL,sFCPC1);
        oElement.appendChild(elementOnFCPC1);

        QString	sFCPC2	        =	_pActionCmd->getFCPC2() ;	
        QDomElement elementOnFCPC2= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnFCPC2.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFFCPC2);
        elementOnFCPC2.setAttribute(XATTR_VAL,sFCPC2);
        oElement.appendChild(elementOnFCPC2);

        QString	sFCPC3	        =	_pActionCmd->getFCPC3() ;	
        QDomElement elementOnFCPC3= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnFCPC3.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFFCPC3);
        elementOnFCPC3.setAttribute(XATTR_VAL,sFCPC3);
        oElement.appendChild(elementOnFCPC3);

        QString	sFCSC1	        =	_pActionCmd->getFCSC1() ;
        QDomElement elementOnFCSC1= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnFCSC1.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFFCSC1);
        elementOnFCSC1.setAttribute(XATTR_VAL,sFCSC1);
        oElement.appendChild(elementOnFCSC1);

        QString	sFCSC2	        =	_pActionCmd->getFCSC2() ;	
        QDomElement elementOnFCSC2= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnFCSC2.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFFCSC2);
        elementOnFCSC2.setAttribute(XATTR_VAL,sFCSC2);
        oElement.appendChild(elementOnFCSC2);

        QString	sBCM	        =	_pActionCmd->getBCM() ;	
        QDomElement elementOnBCM= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnBCM.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFBCM);
        elementOnBCM.setAttribute(XATTR_VAL,sBCM);
        oElement.appendChild(elementOnBCM);

        QString	sFMGEC1	        =	_pActionCmd->getFMGEC1() ;	
        QDomElement elementOnFMGEC1= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnFMGEC1.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFFMGEC1);
        elementOnFMGEC1.setAttribute(XATTR_VAL,sFMGEC1);
        oElement.appendChild(elementOnFMGEC1);

        QString	sFMGEC2	        =	_pActionCmd->getFMGEC2() ;	
        QDomElement elementOnFMGEC2= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnFMGEC2.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFFMGEC2);
        elementOnFMGEC2.setAttribute(XATTR_VAL,sFMGEC2);
        oElement.appendChild(elementOnFMGEC2);

        QString	sAP1	        =	_pActionCmd->getAP1() ;	
        QDomElement elementOnAP1= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnAP1.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFAP1);
        elementOnAP1.setAttribute(XATTR_VAL,sAP1);
        oElement.appendChild(elementOnAP1);

        QString	sAP2	        =	_pActionCmd->getAP2() ;	
        QDomElement elementOnAP2= iDomDoc.createElement(XNODE_PARAMETER);
        elementOnAP2.setAttribute(XATTR_TYPE,XVALUE_FCTL_CONFAP2);
        elementOnAP2.setAttribute(XATTR_VAL,sAP2);
        oElement.appendChild(elementOnAP2);



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

bool GTAActionFCTLConfSerializer::deserialize(const QDomElement &iElement, GTACommandBase *&opCommand)
{
    bool rc = false;
    if(!iElement.isNull())
    {
        /*QString actionName = iElement.attribute(XNODE_NAME);
        QString compliment = iElement.attribute(XNODE_COMPLEMENT);
       */ QString name = iElement.attribute(XATTR_TMPL_NAME);

        /* bool ignoreInSCXML = iElement.attribute(XATTR_IGNORE_SCXML).toUpper()=="TRUE"?true:false;*/

        



        GTAActionFCTLConf * pCmd = new GTAActionFCTLConf();
        /*pCmd->setAction(actionName);
        pCmd->setComplement(compliment);
        pCmd->setIgnoreInSCXML(ignoreInSCXML);*/
        getActionAttributes(iElement,pCmd);

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
                    QString sParameterValue =  parameterElement.attribute(XATTR_VAL);
                    
                    if (sTypeOfParameter == XVALUE_FCTL_CONFCONFIGNAME)
                    {
                        pCmd->setConfigName (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFLAW)
                    {
                        pCmd->setLAW (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFSFCONF)
                    {
                        pCmd->setSFConf (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFGRNDFLIGHT)
                    {
                        pCmd->setGrndFlight (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFENGINES)
                    {
                        pCmd->setEngines (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFLANDINGGEAR)
                    {
                        pCmd->setLandingGear (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFCG)
                    {
                        pCmd->setCG (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFGROSSWT)
                    {
                        pCmd->setGrossWt (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFVCAS)
                    {
                        pCmd->setVCAS (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFALTITUDE)
                    {
                        pCmd->setAltitude (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFYHYD)
                    {
                        pCmd->setYHYD (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFGHYD)
                    {
                        pCmd->setGHYD (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFBHYD)
                    {
                        pCmd->setBHYD (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFALPHA)
                    {
                        pCmd->setAlpha (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFFCPC1)
                    {
                        pCmd->setFCPC1 (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFFCPC2)
                    {
                        pCmd->setFCPC2 (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFFCPC3)
                    {
                        pCmd->setFCPC3 (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFFCSC1)
                    {
                        pCmd->setFCSC1 (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFFCSC2)
                    {
                        pCmd->setFCSC2 (sParameterValue);
                    }
                    else if (sTypeOfParameter == XVALUE_FCTL_CONFBCM)
                    {
                        pCmd->setBCM (sParameterValue);
                    }

                    else if (sTypeOfParameter == XVALUE_FCTL_CONFFMGEC1)
                    {
                        pCmd->setFMGEC1 (sParameterValue);
                    }

                    else if (sTypeOfParameter == XVALUE_FCTL_CONFFMGEC2)
                    {
                        pCmd->setFMGEC2 (sParameterValue);
                    }

                    else if (sTypeOfParameter == XVALUE_FCTL_CONFAP1)
                    {
                        pCmd->setAP1 (sParameterValue);
                    }

                    else if (sTypeOfParameter == XVALUE_FCTL_CONFAP2)
                    {
                        pCmd->setAP2 (sParameterValue);
                    }

                }
            }



        }


        opCommand = pCmd;
        //comment node
        GTACommentSerializer commentSerializer(NULL);
        commentSerializer.deserialize(iElement,opCommand);

        //requiremrnt node
        GTARequirementSerializer requirementSerializer(NULL);
        requirementSerializer.deserialize(iElement,opCommand);

    }
    return rc;

}
#include "GTASerilizerRegisterDef.h"
REGISTER_SERIALIZER(QString("%1 %2").arg(ACT_FCTL,COM_FCTL_CONF),GTAActionFCTLConfSerializer)
