#include "GTAEquationSerializer.h"
#include "GTAUtil.h"
#include "GTAEquationConst.h"
#include "GTAEquationGain.h"
#include "GTAEquationCrenels.h"
#include "GTAEquationRamp.h"
#include "GTAEquationSineCrv.h"
#include "GTAEquationTrepeze.h"
#include "GTAEquationTriangle.h"
#include "GTAEquationSinus.h"
#include "GTAEquationStep.h"
#include "GTAEquationPulse.h"
#include "GTAEquationSawTooth.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

GTAEquationSerializer::GTAEquationSerializer(GTAEquationBase *pEquation)
{
    _pEquation = pEquation;
}
bool GTAEquationSerializer::serialize(QDomDocument &iDomDoc, QDomElement &oElement)
{
    bool rc = false;
    if( _pEquation != NULL)
    {
        QHash<QString,QString> paramValMap;
        _pEquation->getEquationArguments(paramValMap);

        oElement = iDomDoc.createElement(XNODE_EQUATION);
        oElement.setAttribute(XNODE_NAME,_pEquation->getSimulationName());

        QString solidState = _pEquation->getSolidState();
        QString continuousState = _pEquation->getContinousState();
        QDomElement domParam = iDomDoc.createElement("STATE");
        domParam.setAttribute("Solid",solidState);
        domParam.setAttribute("Continuous",continuousState);
        oElement.appendChild(domParam);

        for (auto paramName : paramValMap.keys())
        {
            QString paramVal = paramValMap.value(paramName);
            QDomElement subDomParam = iDomDoc.createElement(XNODE_PARAMETER);
            subDomParam.setAttribute(XNODE_NAME,paramName);
            subDomParam.setAttribute(XNODE_VALUE,paramVal);
            oElement.appendChild(subDomParam);
            rc =true;
        }
    }
    return rc;
}

bool GTAEquationSerializer::deserialize(const QDomElement & iElement, GTACommandBase *& opCommand)
{
    bool rc = false;
    opCommand = NULL;

    if(! iElement.isNull())
    {

        if(iElement.nodeName() == XNODE_EQUATION)
        {
            QString equationName = iElement.attribute(XNODE_NAME);
            if(equationName.isEmpty())
                return rc;

            QHash<QString,QString> paramValMap;
            QDomNodeList childNodeList = iElement.childNodes();
            int childCount = childNodeList.count();

            QString solid;
            QString cont;
            for(int i = 0; i < childCount; i++)
            {
                QDomNode domChild = childNodeList.at(i);
                if(! domChild.isNull() && domChild.isElement())
                {
                    if(domChild.nodeName() == XNODE_PARAMETER)
                    {
                        QDomElement domChildElem = domChild.toElement();
                        QString paramName = domChildElem.attribute(XNODE_NAME);
                        QString paramVal = domChildElem.attribute(XNODE_VALUE);
                        paramValMap.insert(paramName,paramVal);
                    }
                    if(domChild.nodeName() == "STATE")
                    {
                        solid = domChild.attributes().namedItem("Solid").nodeValue();
                        cont = domChild.attributes().namedItem("Continuous").nodeValue();
                    }

                }

            }
            if(paramValMap.count() > 0)
            {
                if(equationName == EQ_CONST)
                {
                    QString constVal = paramValMap.value(EQ_CONST);
                    GTAEquationConst * pEquation =  new GTAEquationConst();
                    pEquation->setEquationArguments(paramValMap);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_GAIN)
                {
                    QString value = paramValMap.value(EQ_GAIN);
                    GTAEquationGain *  pEquation =  new GTAEquationGain();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_CRENELS)
                {   
                    GTAEquationCrenels *  pEquation =  new GTAEquationCrenels();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_RAMP)
                {
                    GTAEquationRamp * pEquation = new GTAEquationRamp();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_SINECRV)
                {
                    GTAEquationSineCrv * pEquation = new GTAEquationSineCrv();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_TRAPEZE)
                {
                    GTAEquationTrepeze *  pEquation =  new GTAEquationTrepeze();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_SQUARE)
                {
                    GTAEquationPulse*  pEquation =  new GTAEquationPulse();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_STEP)
                {
                    GTAEquationStep *  pEquation =  new GTAEquationStep();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_SINUS)
                {
                    GTAEquationSinus *  pEquation =  new GTAEquationSinus();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_TRIANGLE)
                {
                    GTAEquationTriangle*  pEquation =  new GTAEquationTriangle();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }
                else if(equationName == EQ_SAWTOOTH)
                {
                    GTAEquationSawTooth *  pEquation =  new GTAEquationSawTooth();
                    pEquation->setEquationArguments(paramValMap);
                    pEquation->setSolidState(solid);
                    pEquation->setContinousState(cont);
                    opCommand = pEquation;
                    rc = true;
                }

            }

        }
    }
    return rc;
}
