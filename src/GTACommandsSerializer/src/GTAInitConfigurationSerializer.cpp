#include "GTAInitConfigurationSerializer.h"
#include "GTAUtil.h"
GTAInitConfigurationSerializer::GTAInitConfigurationSerializer(GTAInitConfiguration *pInitConfCmd)
{
    _pInitConfCmd = pInitConfCmd;
}
bool GTAInitConfigurationSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if(_pInitConfCmd != NULL)
    {
        QString configName = _pInitConfCmd->getConfigName();
        QString altSpeed = _pInitConfCmd->getAltSpeed();
        QString acPowerd = _pInitConfCmd->getAcPowered();
        QString altGround= _pInitConfCmd->getAltGround();
        QString altTarget= _pInitConfCmd->getAltTarget();
        QString apu = _pInitConfCmd->getAPU();
        QString apuBleed = _pInitConfCmd->getAPUBleed();
        QString apuGen = _pInitConfCmd->getAPUGen();
        QString athr = _pInitConfCmd->getATHR();
        QString autoPilot1 = _pInitConfCmd->getAutoPilot1();
        QString autoPilot2 = _pInitConfCmd->getAutoPilot2();
        QString bat = _pInitConfCmd->getBat();
        QString disa = _pInitConfCmd->getDISA();
        QString engineState = _pInitConfCmd->getEngineSate();
        QString extPower = _pInitConfCmd->getExternalPower();
        QString masterLevel= _pInitConfCmd->getMasterLevel();
        QString parkBrakes = _pInitConfCmd->getParkBrakes();
        QString rotarySel = _pInitConfCmd->getRotarySelector();
        QString slatFlap = _pInitConfCmd->getSlatFlap();
        QString speedTarget = _pInitConfCmd->getSpeedTarget();
        QString throttleState = _pInitConfCmd->getThrottleState();

        oElement = iDomDoc.createElement(XNODE_INITCONF);
        oElement.setAttribute(XNODE_NAME,configName);

        QDomElement domAltSpeed = iDomDoc.createElement(XNODE_PARAMETER);
        domAltSpeed.setAttribute(XNODE_NAME,XNODE_ALTSPEED);
        domAltSpeed.setAttribute(XNODE_VALUE,altSpeed);
        oElement.appendChild(domAltSpeed);

        QDomElement domAcPowered = iDomDoc.createElement(XNODE_PARAMETER);
        domAcPowered.setAttribute(XNODE_NAME,XNODE_ACPOWERED);
        domAcPowered.setAttribute(XNODE_VALUE,acPowerd);
        oElement.appendChild(domAcPowered);

        QDomElement domAltGrnd = iDomDoc.createElement(XNODE_PARAMETER);
        domAltGrnd.setAttribute(XNODE_NAME,XNODE_ALTGROUND);
        domAltGrnd.setAttribute(XNODE_VALUE,altGround);
        oElement.appendChild(domAltGrnd);

        QDomElement domAltTarget = iDomDoc.createElement(XNODE_PARAMETER);
        domAltTarget.setAttribute(XNODE_NAME,XNODE_ALTTARGET);
        domAltTarget.setAttribute(XNODE_VALUE,altTarget);
        oElement.appendChild(domAltTarget);

        QDomElement domApu = iDomDoc.createElement(XNODE_PARAMETER);
        domApu.setAttribute(XNODE_NAME,XNODE_APU);
        domApu.setAttribute(XNODE_VALUE,apu);
        oElement.appendChild(domApu);

        QDomElement domApuGen = iDomDoc.createElement(XNODE_PARAMETER);
        domApuGen.setAttribute(XNODE_NAME,XNODE_APUGEN);
        domApuGen.setAttribute(XNODE_VALUE,apuGen);
        oElement.appendChild(domApuGen);

        QDomElement domApuBleed = iDomDoc.createElement(XNODE_PARAMETER);
        domApuBleed.setAttribute(XNODE_NAME,XNODE_APUGBLEED);
        domApuBleed.setAttribute(XNODE_VALUE,apuBleed);
        oElement.appendChild(domApuBleed);

        QDomElement domAthr = iDomDoc.createElement(XNODE_PARAMETER);
        domAthr.setAttribute(XNODE_NAME,XNODE_ATHR);
        domAthr.setAttribute(XNODE_VALUE,athr);
        oElement.appendChild(domAthr);

        QDomElement domAutoPil1 = iDomDoc.createElement(XNODE_PARAMETER);
        domAutoPil1.setAttribute(XNODE_NAME,XNODE_AUTOPILOT1);
        domAutoPil1.setAttribute(XNODE_VALUE,autoPilot1);
        oElement.appendChild(domAutoPil1);

        QDomElement domAutoPil2 = iDomDoc.createElement(XNODE_PARAMETER);
        domAutoPil2.setAttribute(XNODE_NAME,XNODE_AUTOPILOT2);
        domAutoPil2.setAttribute(XNODE_VALUE,autoPilot2);
        oElement.appendChild(domAutoPil2);

        QDomElement domBat = iDomDoc.createElement(XNODE_PARAMETER);
        domBat.setAttribute(XNODE_NAME,XNODE_BAT);
        domBat.setAttribute(XNODE_VALUE,bat);
        oElement.appendChild(domBat);

        QDomElement domDisa = iDomDoc.createElement(XNODE_PARAMETER);
        domDisa.setAttribute(XNODE_NAME,XNODE_DISA);
        domDisa.setAttribute(XNODE_VALUE,disa);
        oElement.appendChild(domDisa);

        QDomElement domEngineState = iDomDoc.createElement(XNODE_PARAMETER);
        domEngineState.setAttribute(XNODE_NAME,XNODE_ENGINESTATE);
        domEngineState.setAttribute(XNODE_VALUE,engineState);
        oElement.appendChild(domEngineState);

        QDomElement domExtPower = iDomDoc.createElement(XNODE_PARAMETER);
        domExtPower.setAttribute(XNODE_NAME,XNODE_EXTPOWER);
        domExtPower.setAttribute(XNODE_VALUE,extPower);
        oElement.appendChild(domExtPower);

        QDomElement domMasterLevel = iDomDoc.createElement(XNODE_PARAMETER);
        domMasterLevel.setAttribute(XNODE_NAME,XNODE_MASTERLEVEL);
        domMasterLevel.setAttribute(XNODE_VALUE,masterLevel);
        oElement.appendChild(domMasterLevel);

        QDomElement domParkBrake = iDomDoc.createElement(XNODE_PARAMETER);
        domParkBrake.setAttribute(XNODE_NAME,XNODE_PARKBRAKES);
        domParkBrake.setAttribute(XNODE_VALUE,parkBrakes);
        oElement.appendChild(domParkBrake);

        QDomElement domRotarySel = iDomDoc.createElement(XNODE_PARAMETER);
        domRotarySel.setAttribute(XNODE_NAME,XNODE_ROTARYSEL);
        domRotarySel.setAttribute(XNODE_VALUE,rotarySel);
        oElement.appendChild(domRotarySel);

        QDomElement domSlatFlap = iDomDoc.createElement(XNODE_PARAMETER);
        domSlatFlap.setAttribute(XNODE_NAME,XNODE_SLATFLAP);
        domSlatFlap.setAttribute(XNODE_VALUE,slatFlap);
        oElement.appendChild(domSlatFlap);

        QDomElement domSpeedTarget = iDomDoc.createElement(XNODE_PARAMETER);
        domSpeedTarget.setAttribute(XNODE_NAME,XNODE_SPEEDTARGET);
        domSpeedTarget.setAttribute(XNODE_VALUE,speedTarget);
        oElement.appendChild(domSpeedTarget);

        QDomElement domThrottleState = iDomDoc.createElement(XNODE_PARAMETER);
        domThrottleState.setAttribute(XNODE_NAME,XNODE_THROTTLESTATE);
        domThrottleState.setAttribute(XNODE_VALUE,throttleState);
        oElement.appendChild(domThrottleState);

        rc = true;
    }

    return rc;
}
bool GTAInitConfigurationSerializer::deserialize(const QDomElement &iElement, GTAInitConfigBase *&opCommand)
{
    bool rc = false;

    if(! iElement.isNull())
    {
        GTAInitConfiguration * pInitConfCmd = new GTAInitConfiguration();

        QString confName = iElement.attribute(XNODE_NAME);
        pInitConfCmd->setConfigName(confName);

        QDomNodeList childList = iElement.childNodes();
        if(childList.count() >0)
        {
            for(int i = 0; i<childList.count(); i++)
            {
                QDomNode domNode = childList.at(i);
                if(domNode.isElement())
                {
                    QDomElement domElem = domNode.toElement();
                    QString attrName = domElem.attribute(XNODE_NAME);
                    QString attrVal = domElem.attribute(XNODE_VALUE);
                    if(attrName == XNODE_ALTSPEED)
                    {
                        pInitConfCmd->setAltSpeed(attrVal);
                    }
                    else if(attrName == XNODE_ACPOWERED)
                    {
                        pInitConfCmd->setAcPowered(attrVal);
                    }
                    else if(attrName == XNODE_ALTGROUND)
                    {
                        pInitConfCmd->setAltGround(attrVal);
                    }
                    else if(attrName == XNODE_ALTTARGET)
                    {
                        pInitConfCmd->setAltTarget(attrVal);
                    }
                    else if(attrName == XNODE_APU)
                    {
                        pInitConfCmd->setAPU(attrVal);
                    }
                    else if(attrName == XNODE_APUGBLEED)
                    {
                        pInitConfCmd->setAPUBleed(attrVal);
                    }
                    else if(attrName == XNODE_APUGEN)
                    {
                        pInitConfCmd->setAPUGen(attrVal);
                    }
                    else if(attrName == XNODE_ATHR)
                    {
                        pInitConfCmd->setATHR(attrVal);
                    }
                    else if(attrName == XNODE_AUTOPILOT1)
                    {
                        pInitConfCmd->setAutoPilot1(attrVal);
                    }
                    else if(attrName == XNODE_AUTOPILOT2)
                    {
                        pInitConfCmd->setAutoPilot2(attrVal);
                    }
                    else if(attrName == XNODE_BAT)
                    {
                        pInitConfCmd->setBat(attrVal);
                    }
                    else if(attrName == XNODE_DISA)
                    {
                        pInitConfCmd->setDISA(attrVal);
                    }
                    else if(attrName == XNODE_ENGINESTATE)
                    {
                        pInitConfCmd->setEngineSate(attrVal);
                    }
                    else if(attrName == XNODE_EXTPOWER)
                    {
                        pInitConfCmd->setExternalPower(attrVal);
                    }
                    else if(attrName == XNODE_MASTERLEVEL)
                    {
                        pInitConfCmd->setMasterLevel(attrVal);
                    }
                    else if(attrName == XNODE_PARKBRAKES)
                    {
                        pInitConfCmd->setParkBrakes(attrVal);
                    }
                    else if(attrName == XNODE_ROTARYSEL)
                    {
                        pInitConfCmd->setRotarySelector(attrVal);
                    }
                    else if(attrName == XNODE_SLATFLAP)
                    {
                        pInitConfCmd->setSlatFlap(attrVal);
                    }
                    else if(attrName == XNODE_SPEEDTARGET)
                    {
                        pInitConfCmd->setSpeedTarget(attrVal);
                    }
                    else if(attrName == XNODE_THROTTLESTATE)
                    {
                        pInitConfCmd->setThrottleState(attrVal);
                    }

                }
            }
            rc = true;
            opCommand = pInitConfCmd;

        }
    }
    return rc;
}
