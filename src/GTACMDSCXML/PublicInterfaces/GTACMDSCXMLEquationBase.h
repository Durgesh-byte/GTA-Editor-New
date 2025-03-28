#ifndef GTACMDSCXMLEQUATIONBASE_H
#define GTACMDSCXMLEQUATIONBASE_H

#include "GTACMDSCXML.h"
#include "GTACMDSCXMLBase.h"
#include "GTAEquationBase.h"
#include "GTAEquationRamp.h"
#include "GTAEquationConst.h"
#include "GTAEquationCrenels.h"
#include "GTAEquationGain.h"
#include "GTAEquationPulse.h"
#include "GTAEquationSawTooth.h"
#include "GTAEquationSinus.h"
#include "GTAEquationSineCrv.h"
#include "GTAEquationStep.h"
#include "GTAEquationTrepeze.h"
#include "GTAEquationTriangle.h"
#include "GTACMDSCXMLUtils.h"
#include "GTACMDSCXMLLog.h"
#include "GTAActionForEach.h"


class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLEquationBase : public GTACMDSCXMLBase
{

public:
    GTACMDSCXMLEquationBase();
    GTACMDSCXMLEquationBase(GTACMDSCXMLEquationBase *&pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLEquationBase();

    virtual void getSetStructureFilled(GTAEquationBase *& ipEq,SingleSetCommandStructure *&ioSetStruct,QList<GTAICDParameter> &irhsParamList, GTAActionForEach* pForEach = nullptr, int forEachRowNumber = 0);
    virtual QList<GTASCXMLAssign> getAssignList(QString iparamArray,int iCount,SingleSetCommandStructure *&ioSetStruct);
};

#endif // GTACMDSCXMLEQUATIONBASE_H
