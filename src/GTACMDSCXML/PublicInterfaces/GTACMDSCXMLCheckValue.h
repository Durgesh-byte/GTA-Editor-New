#ifndef GTACMDSCXMLCHECKVALUE_H
#define GTACMDSCXMLCHECKVALUE_H

#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTACheckValue.h"
#include "GTAActionForEach.h"

#pragma warning(push, 0)
#include <QFile>
#pragma warning(pop)

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLCheckValue : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLCheckValue();
    GTACMDSCXMLCheckValue(GTACheckValue *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLCheckValue();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;
	GTASCXMLState getParamState(const QString &iStateId, const QString &iTargetId) const;

private:
    GTACheckValue *_CheckValueCmd;
	bool _isParamInCSV = false;
private:
    GTASCXMLState getSCXMLCheckValue() const;
    GTASCXMLState evalutateConditionAndTxState(const QString &iStateId, const QString &iTargetId, const QString &iLoopBackStateId, bool isConfTimeEn = false, bool ParamStateHasInvokes = true) const;
    QString getExecStateID() const;
    QString getExecTargetID() const;
    QString getFinalTargetID() const;
    QString getPreDebugStateID() const;
    QString getPreDebugVariable() const;
    QString getLineArray(const QStringList &templist) const;
};

#endif // GTACMDSCXMLCheckValue_H
