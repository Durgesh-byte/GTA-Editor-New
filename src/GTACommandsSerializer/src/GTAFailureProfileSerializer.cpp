#include "GTAFailureProfileSerializer.h"
#include "GTAUtil.h"
#include "GTAFailProfileExp.h"
#include "GTAFailProfileIdle.h"
#include "GTAFailProfilePulse.h"
#include "GTAFailProfileSawTooth.h"
#include "GTAFailProfileSine.h"
#include "GTAFailProfileConstant.h"
#include "GTAFailProfileNoise.h"
#include "GTAFailProfileStopTrigger.h"
#include "GTAFailProfileRamp.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

GTAFailureProfileSerializer::GTAFailureProfileSerializer(GTAFailureProfileBase *pEquation)
{
    _pFailureProfile = pEquation;
}
bool GTAFailureProfileSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if( _pFailureProfile != NULL)
    {
        // GTAFailureProfileBase::EquationType equationType = _pFailureProfile->getEquationType();
        QHash<QString,QString> paramValMap;
        _pFailureProfile->getFailureProfileArguments(paramValMap);
    


    oElement = iDomDoc.createElement(XNODE_FAILURE_PROFILE);
    oElement.setAttribute(XNODE_NAME,_pFailureProfile->getSimulationName());
    foreach(QString paramName , paramValMap.keys())
    {
        QString paramVal = paramValMap.value(paramName);
        QDomElement domParam = iDomDoc.createElement(XNODE_PARAMETER);
        domParam.setAttribute(XNODE_NAME,paramName);
        domParam.setAttribute(XNODE_VALUE,paramVal);
        oElement.appendChild(domParam);
        rc =true;
    }

    }

    return rc;
}

bool GTAFailureProfileSerializer::deserialize(const QDomElement & iElement, GTAFailureProfileBase *& opCommand)
{
    bool rc = false;
    opCommand = NULL;

    if(! iElement.isNull())
    {

        if(iElement.nodeName() == XNODE_FAILURE_PROFILE)
        {
            QString equationName = iElement.attribute(XNODE_NAME);
            if(equationName.isEmpty())
                return rc;

            QHash<QString,QString> paramValMap;
            QDomNodeList childNodeList = iElement.childNodes();
            int childCount = childNodeList.count();
            for(int i = 0; i < childCount; i++)
            {
                QDomNode domChild = childNodeList.at(i)                ;
                if(! domChild.isNull() && domChild.isElement())
                {
                    QDomElement domChildElem = domChild.toElement();
                    QString paramName = domChildElem.attribute(XNODE_NAME);
                    QString paramVal = domChildElem.attribute(XNODE_VALUE);
                    paramValMap.insert(paramName,paramVal);
                }
            }
            if(equationName == EQ_IDLE && paramValMap.count()==0)
            {

                GTAFailProfileIdle *  pEquation =  new GTAFailProfileIdle();
                opCommand = pEquation;
                rc = true;
            }
            else
                if(paramValMap.count() > 0 )
                {
                    if(equationName == EQ_CONST)
                    {
                        QString constVal = paramValMap.value(EQ_CONST);
                        GTAFailProfileConstant * pEquation =  new GTAFailProfileConstant();
                        pEquation->setFailureProfileArguments(paramValMap);
                        opCommand = pEquation;
                        rc = true;
                    }
                    else if(equationName == EQ_EXP)
                    {   
                        GTAFailProfileExp *  pEquation =  new GTAFailProfileExp();
                        pEquation->setFailureProfileArguments(paramValMap);
                        opCommand = pEquation;
                        rc = true;
                    }
                    else if(equationName == EQ_STOP_TRIGGER)
                    {
                        GTAFailProfileStopTrigger * pEquation = new GTAFailProfileStopTrigger();
                        pEquation->setFailureProfileArguments(paramValMap);
                        opCommand = pEquation;
                        rc = true;
                    }
                    else if(equationName == EQ_RAMP)
                    {
                        GTAFailProfileRamp * pEquation = new GTAFailProfileRamp();
                        pEquation->setFailureProfileArguments(paramValMap);
                        opCommand = pEquation;
                        rc = true;
                    }
                    
                    //else if(equationName == EQ_TRAPEZE)
                    //{
                    //    GTAEquationTrepeze *  pEquation =  new GTAEquationTrepeze();
                    //    pEquation->setEquationArguments(paramValMap);
                    //    opCommand = pEquation;
                    //    rc = true;
                    //}
                    //else if(equationName == EQ_SQUARE)
                    //{
                    //    GTAEquationPulse*  pEquation =  new GTAEquationPulse();
                    //    pEquation->setEquationArguments(paramValMap);
                    //    opCommand = pEquation;
                    //    rc = true;
                    //}
                    else if(equationName == EQ_NOISE)
                    {
                        GTAFailProfileNoise *  pEquation =  new GTAFailProfileNoise();
                        pEquation->setFailureProfileArguments(paramValMap);
                        opCommand = pEquation;
                        rc = true;
                    }
                    else if(equationName == EQ_SINUS)
                    {
                        GTAFailProfileSine *  pEquation =  new GTAFailProfileSine();
                        pEquation->setFailureProfileArguments(paramValMap);
                        opCommand = pEquation;
                        rc = true;
                    }
                    else if(equationName == EQ_SQUARE)
                    {
                        GTAFailProfilePulse *  pEquation =  new GTAFailProfilePulse();
                        pEquation->setFailureProfileArguments(paramValMap);
                        opCommand = pEquation;
                        rc = true;
                    }
                    else if(equationName == EQ_SAWTOOTH)
                    {
                        GTAFailProfileSawTooth *  pEquation =  new GTAFailProfileSawTooth();
                        pEquation->setFailureProfileArguments(paramValMap);
                        opCommand = pEquation;
                        rc = true;
                    }

                }

        }
    }
    return rc;
}
