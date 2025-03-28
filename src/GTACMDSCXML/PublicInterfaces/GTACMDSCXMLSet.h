#ifndef GTACMDSCXMLSET_H
#define GTACMDSCXMLSET_H

#include "GTACMDSCXML.h"
#include "GTACMDSCXMLBase.h"
#include "GTAActionSetList.h"
#include "GTAActionSet.h"
#include "GTAEquationBase.h"
#include "GTAEquationRamp.h"
#include "GTACMDSCXMLEquationBase.h"
#include "GTAActionForEach.h"
#include "GTAActionIF.h"
#include "GTAActionElse.h"
#include "GTACommandBase.h"
#include "GTAActionBase.h"
#include "GTAEquationBase.h"
#include "GTAEquationConst.h"

#pragma warning(push, 0)
#include <QFile>
#pragma warning(pop)

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLSet : public GTACMDSCXMLBase
{

public:
    GTACMDSCXMLSet();
    GTACMDSCXMLSet(GTAActionSetList *&pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLSet();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;
    virtual void insertParamInfoList(const GTAICDParameter &iParamInfo);

    void setVMACHiddenDelay(const QString &iVal)
    {
        _VmacHiddenDelay = iVal;
    }
    GTAActionBase* getForEachGrandParent(GTAActionSetList* setCmd);

private:
    GTAActionSetList *_SetCmd;
    GTACMDSCXMLEquationBase *_EquationCmdSCXML;
    QString _VmacHiddenDelay;
    GTAActionForEach* _ForEachLoop = nullptr;
    GTAActionBase* _ForEachGrandParent;

private:
    GTASCXMLState getMultiSetSCXMLState(const QString &iStateId, const QString &iTargetId, GTAActionBase* forEachGrandParent = nullptr) const;
};

#endif // GTACMDSCXMLSET_H
