#ifndef GTACMDSCXMLUTILS_H
#define GTACMDSCXMLUTILS_H


#include "GTACMDSCXML.h"
#include "GTASCXMLState.h"
#include "GTAICDParameter.h"
#include "GTACMDSCXMLLog.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

#define TX_STATUS_OK "OK"
#define TX_STATUS_KO "KO"
#define TX_STATUS_NA "NA"
#define TX_STATUS_TIMEOUT "TIMEOUT"

/**
 * This structure fills Ramp properties to be used in Multi-Set SCXML
*/
struct EquationRampCommandStructure
{
    EquationRampCommandStructure(bool iVal):isRampEndValueOffset(iVal)
    {
        rampEndValue = "";
        rampRiseValue = "";
    }
    EquationRampCommandStructure(EquationRampCommandStructure &obj)
    {
        isRampEndValueOffset = obj.isRampEndValueOffset;
        rampEndValue = obj.rampEndValue;
        rampRiseValue = obj.rampRiseValue;
        rampStartParam = obj.rampStartParam;
    }

    bool isRampEndValueOffset;
    GTAICDParameter rampStartParam;
    QString rampEndValue;
    QString rampRiseValue;
};

struct EquationSawToothCommandStructure
{
	EquationSawToothCommandStructure()
	{
        sawPeriodValue = "";
        sawMaxValue = "";
        sawMinValue = "";
        sawStartValue = "";
        sawRampModeValue = "";
	}
	EquationSawToothCommandStructure(EquationSawToothCommandStructure &obj)
    {
        sawPeriodValue = obj.sawPeriodValue;
        sawMaxValue = obj.sawMaxValue;
        sawMinValue = obj.sawMinValue;
        sawStartValue = obj.sawStartValue;
        sawRampModeValue = obj.sawRampModeValue;
    }

    QString sawPeriodValue;
    QString sawMaxValue;
    QString sawMinValue;
    QString sawStartValue;
    QString sawRampModeValue;
};

struct EquationSinusCommandStructure
{
    EquationSinusCommandStructure()
    {
        sinPeriodValue = "";
        sinMaxValue = "";
        sinMinValue = "";
        sinStartValue = "";
        sinDirectionValue = "";
    }
    EquationSinusCommandStructure(EquationSinusCommandStructure &obj)
    {
        sinPeriodValue = obj.sinPeriodValue;
        sinMaxValue = obj.sinMaxValue;
        sinMinValue = obj.sinMinValue;
        sinStartValue = obj.sinStartValue;
        sinDirectionValue = obj.sinDirectionValue;
    }

    QString sinPeriodValue;
    QString sinMaxValue;
    QString sinMinValue;
    QString sinStartValue;
    QString sinDirectionValue;
};

struct EquationPulseCommandStructure
{
    EquationPulseCommandStructure()
    {
        pulPeriodValue = "";
        pulMaxValue = "";
        pulMinValue = "";
        pulDutyCycleValue = "";
        pulDirectionValue = "";
    }
    EquationPulseCommandStructure(EquationPulseCommandStructure &obj)
    {
        pulPeriodValue = obj.pulPeriodValue;
        pulMaxValue = obj.pulMaxValue;
        pulMinValue = obj.pulMinValue;
        pulDutyCycleValue = obj.pulDutyCycleValue;
        pulDirectionValue = obj.pulDirectionValue;
    }

    QString pulPeriodValue;
    QString pulMaxValue;
    QString pulMinValue;
    QString pulDutyCycleValue;
    QString pulDirectionValue;
};

struct EquationTriangleCommandStructure
{
    EquationTriangleCommandStructure()
    {
        trianglePeriodValue = "";
        triangleMaxValue = "";
        triangleMinValue = "";
        triangleStartValue = "";
        triangleDirectionValue = "";
    }
    EquationTriangleCommandStructure(EquationTriangleCommandStructure &obj)
    {
        trianglePeriodValue = obj.trianglePeriodValue;
        triangleMaxValue = obj.triangleMaxValue;
        triangleMinValue = obj.triangleMinValue;
        triangleStartValue = obj.triangleStartValue;
        triangleDirectionValue = obj.triangleDirectionValue;
    }

    QString trianglePeriodValue;
    QString triangleMaxValue;
    QString triangleMinValue;
    QString triangleStartValue;
    QString triangleDirectionValue;
};

struct EquationStepCommandStructure
{
    EquationStepCommandStructure()
    {
        stepPeriodValue = "";
        stepStartValue = "";
        stepEndValue = "";
    }
    EquationStepCommandStructure(EquationStepCommandStructure &obj)
    {
        stepPeriodValue = obj.stepPeriodValue;
        stepStartValue = obj.stepStartValue;
        stepEndValue = obj.stepEndValue;
    }

    QString stepPeriodValue;
    QString stepStartValue;
    QString stepEndValue;
};

struct EquationCrenelsCommandStructure
{
    EquationCrenelsCommandStructure()
    {
        crenelsValues = "";
        crenelsListLevel = { "", "", "", "", "" };
        crenelsListLength = { "", "", "", "", "" };
    }
    EquationCrenelsCommandStructure(EquationCrenelsCommandStructure &obj)
    {
        crenelsValues = obj.crenelsValues;
        crenelsListLevel = obj.crenelsListLevel;
        crenelsListLength = obj.crenelsListLength;
    }

    QString crenelsValues;
    QList<QString> crenelsListLevel;
    QList<QString> crenelsListLength;
};

struct EquationTrapezeCommandStructure
{
    EquationTrapezeCommandStructure()
    {
        trapezeValues = "";
        trapezeListLevel = { "", "", ""};
        trapezeListLength = { "", "", ""};
        trapezeListRamp = { "", ""};
    }
    EquationTrapezeCommandStructure(EquationTrapezeCommandStructure &obj)
    {
        trapezeValues = obj.trapezeValues;
        trapezeListLevel = obj.trapezeListLevel;
        trapezeListLength = obj.trapezeListLength;
        trapezeListRamp = obj.trapezeListRamp;
    }

    QString trapezeValues;
    QList<QString> trapezeListRamp;
    QList<QString> trapezeListLevel;
    QList<QString> trapezeListLength;
};

struct EquationSineCurveCommandStructure
{
    EquationSineCurveCommandStructure()
    {
        sCurvePulsationValue = "";
        sCurveOffsetValue = "";
        sCurveGainValue = "";
        sCurvePhaseValue = "";
        sCurveOperatorValue = "";
    }
    EquationSineCurveCommandStructure(EquationSineCurveCommandStructure &obj)
    {
        sCurvePulsationValue = obj.sCurvePulsationValue;
        sCurveOffsetValue = obj.sCurveOffsetValue;
        sCurveGainValue = obj.sCurveGainValue;
        sCurvePhaseValue = obj.sCurvePhaseValue;
        sCurveOperatorValue = obj.sCurveOperatorValue;
    }

    QString sCurvePulsationValue;
    QString sCurveOffsetValue;
    QString sCurveGainValue;
    QString sCurvePhaseValue;
    QString sCurveOperatorValue;
};

struct EquationGainCommandStructure
{
    EquationGainCommandStructure()
    {
        gainValue = "";
    }
    EquationGainCommandStructure(EquationGainCommandStructure &obj)
    {
        gainValue = obj.gainValue;
    }

    QString gainValue;
};
/**
 * This structure fills Set properties to be used in Multi-Set SCXML
*/
struct SingleSetCommandStructure
{
    SingleSetCommandStructure ()
    {
        EqnRampStruct = new EquationRampCommandStructure(false);
        EqnSawToothStruct = new EquationSawToothCommandStructure();
        EqnSinusStruct = new EquationSinusCommandStructure();
        EqnPulseStruct = new EquationPulseCommandStructure();
        EqnTriangleStruct = new EquationTriangleCommandStructure();
        EqnStepStruct = new EquationStepCommandStructure();
        EqnCrenelsStruct = new EquationCrenelsCommandStructure();
        EqnTrapezeStruct = new EquationTrapezeCommandStructure();
        EqnSineCurveStruct = new EquationSineCurveCommandStructure();
        EqnGainStruct = new EquationGainCommandStructure();
    }
    SingleSetCommandStructure (SingleSetCommandStructure &obj)
    {
        lhsParam = obj.lhsParam;
        rhsParam = obj.rhsParam;
        FSVal = obj.FSVal;
        isFSOnly = obj.isFSOnly;
        funcType = obj.funcType;
        funcValue = obj.funcValue;
        if(obj.funcType == EQ_RAMP)
            this->EqnRampStruct = new EquationRampCommandStructure(obj.EqnRampStruct);
        if(obj.funcType == EQ_SAWTOOTH)
            this->EqnSawToothStruct = new EquationSawToothCommandStructure(*obj.EqnSawToothStruct);
        if(obj.funcType == EQ_SINUS)
            this->EqnSinusStruct = new EquationSinusCommandStructure(*obj.EqnSinusStruct);
        if(obj.funcType == EQ_SQUARE)
            this->EqnPulseStruct = new EquationPulseCommandStructure(*obj.EqnPulseStruct);
        if(obj.funcType == EQ_TRIANGLE)
            this->EqnTriangleStruct = new EquationTriangleCommandStructure(*obj.EqnTriangleStruct);
        if(obj.funcType == EQ_STEP)
            this->EqnStepStruct = new EquationStepCommandStructure(*obj.EqnStepStruct);
        if(obj.funcType == EQ_CRENELS)
            this->EqnCrenelsStruct = new EquationCrenelsCommandStructure(*obj.EqnCrenelsStruct);
        if(obj.funcType == EQ_TRAPEZE)
            this->EqnTrapezeStruct = new EquationTrapezeCommandStructure(*obj.EqnTrapezeStruct);
        if(obj.funcType == EQ_SINECRV)
            this->EqnSineCurveStruct = new EquationSineCurveCommandStructure(*obj.EqnSineCurveStruct);
        if(obj.funcType == EQ_GAIN)
            this->EqnGainStruct = new EquationGainCommandStructure(*obj.EqnGainStruct);
    }
    ~SingleSetCommandStructure ()
    {
        if (NULL != EqnRampStruct)
        {
            delete EqnRampStruct;
            EqnRampStruct = NULL;
        }

        if (NULL != EqnSawToothStruct)
        {
            delete EqnSawToothStruct;
            EqnSawToothStruct = NULL;
        }

        if (NULL != EqnSinusStruct)
        {
            delete EqnSinusStruct;
            EqnSinusStruct = NULL;
        }

        if (NULL != EqnPulseStruct)
        {
            delete EqnPulseStruct;
            EqnPulseStruct = NULL;
        }

        if (NULL != EqnTriangleStruct)
        {
            delete EqnTriangleStruct;
            EqnTriangleStruct = NULL;
        }

        if (NULL != EqnStepStruct)
        {
            delete EqnStepStruct;
            EqnStepStruct = NULL;
        }

        if (NULL != EqnCrenelsStruct)
        {
            delete EqnCrenelsStruct;
            EqnCrenelsStruct = NULL;
        }
        if (NULL != EqnTrapezeStruct)
        {
            delete EqnTrapezeStruct;
            EqnTrapezeStruct = NULL;
        }
        if (NULL != EqnSineCurveStruct)
        {
            delete EqnSineCurveStruct;
            EqnSineCurveStruct = NULL;
        }
        if (NULL != EqnGainStruct)
        {
            delete EqnGainStruct;
            EqnGainStruct = NULL;
        }
    }

    SingleSetCommandStructure * getClone()
    {
        return new SingleSetCommandStructure(*this);
    }

    GTAICDParameter lhsParam;
    GTAICDParameter rhsParam;
    QString FSVal;
    bool isFSOnly;
    QString funcType;
    QString funcValue;
    EquationRampCommandStructure* EqnRampStruct;
    EquationSawToothCommandStructure* EqnSawToothStruct;
    EquationSinusCommandStructure* EqnSinusStruct;
    EquationPulseCommandStructure* EqnPulseStruct;
    EquationTriangleCommandStructure* EqnTriangleStruct;
    EquationStepCommandStructure* EqnStepStruct;
    EquationCrenelsCommandStructure* EqnCrenelsStruct;
    EquationTrapezeCommandStructure* EqnTrapezeStruct;
    EquationSineCurveCommandStructure* EqnSineCurveStruct;
    EquationGainCommandStructure* EqnGainStruct;
};

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLUtils
{

public:

    enum LOG_TYPE{SUCCESS_LOG, FAIL_LOG, TIMEOUT_LOG, INFO_LOG, KO_FAIL_LOG};
    GTACMDSCXMLUtils();
    ~GTACMDSCXMLUtils();

    static void setSCXMLTemplatePath(const QString &iTemplatePath);
    static GTASCXMLState createGetParamInvokeState(const QList<GTAICDParameter> &iParamList,
                                                      const QString &iStateId, 
                                                      const QString &iTargetId, QString timeoutEventName = QString(), 
                                                      bool isActOnFail = false,
                                                      const QString &iIdentifier = QString(), 
                                                      bool iAddLogMessageInTx = false, 
                                                      const QString &iInvokeDelay = QString(), 
                                                      const QString &itimeoutTarget = QString());

    static GTASCXMLState createPopUpInvokeState(const QString &iMsg, const QString &iStateId, const QString &iTargetId, bool &iWaitForAck, const QString &iCmdId);

    static GTASCXMLState createParamPopUpInvokeState(const QString &iMsg, const QString &iStateId, const QString &iTargetId, GTAICDParameter &iParameter, const QString &iCmdId);

    static GTASCXMLState createSetParamInvokeState(GTAICDParameter &iLHSParam,
                                                      GTAICDParameter &iRHSParam,
                                                      const QString &iStateId, 
                                                      const QString &iTargetId, 
                                                      bool iSetOnlyFS, 
                                                      const QString &iFSValue, 
                                                      const QString &iIdentifier = QString(), 
                                                      const QString &iInvokeDelay = QString());

    static GTASCXMLState createSetParamInvokeStateForEquations(GTAICDParameter &iLHSParam,const QString &iStateId, const QString &iTargetId, bool iSetOnlyFS, QString &iFSValue, 
                                                                  QString &iIdentifier, QString &FunctionType, QString &FunctionValue, const QString &iInvokeDelay);

    static void insertGenToolReturnParams(const QString &iId, const QString &iRetParam);
    static QHash<QString, QString> getGenToolRetData();
    static GTASCXMLState subscribeParameters(const QList<GTAICDParameter> &iParamList,const QString &iStateId, const QString &iTargetId, const QString &iIdentifier=QString());
    static GTASCXMLState unSubscribeParameters(const QList<GTAICDParameter> &iParamList,const QString &iStateId, const QString &iTargetId, const QString &iIdentifier=QString());
    static GTASCXMLState releaseVMACParameters(const QList<GTAICDParameter> &iParamList,const QString &iStateId, const QString &iTargetId,const QString &InstanceName,const QString &iInvokeDelay);
    static void updateWithGenToolParam(QString &ioParam);
    static void updateWithGenToolParam(GTAICDParameter &ioParam);
    static GTASCXMLLog getSCXMLLogReport();
    static void insertVariablesInLogReport(const QList<GTAICDParameter> &iParamList,const QStringList iInternalParams,GTACMDSCXMLLog &ioLog);
    static GTASCXMLState createSubscribeForApps(const QList<GTAICDParameter> &iParams, const QString &iStateId, const QString &iTargetId);
    static GTASCXMLState createUnSubscribeAll(const QList<GTAICDParameter> &iParams, const QString &iStateId, const QString &iTargetId, bool unsubscribeFileParamOnly = false);
    static void createUnSubscribeProcedureParam(GTASCXMLState& invokeState, const QString& iStateId, const QStringList& tools, const int index);
    static GTASCXMLLog getLogMessage(LOG_TYPE iLogType,const QString &iIndentfier, const QString &iCurrentVal, const QString &iExpectedVal, bool isExpValParam = true);
    static GTASCXMLInvoke createDummyInvokeForLoop(const QString &iStateId);
    static void logAllParameters(const QString &iIdentifier, const QList<GTAICDParameter> &iParamList, GTASCXMLState &ioState, bool isParamCSV = false);
	//static void logPirParameters(const QString &iIdentifier, const QStringList &iLineList, GTASCXMLState &ioState);
    static QStringList getChannelSelectionSignals(const QString &iEngine);
    static bool getInvokeFromTemplate(const QString &iTemplateFileName, GTASCXMLInvoke &oInvoke);
    static bool getCloneInvoke(const QString & iTemplateFileName, GTASCXMLInvoke &oInvoke);
    static GTASCXMLState getDebugState(const QString &iStateId, const QString &iTargetId, const QStringList &iStateVariables=QStringList(),
                                          const QList<GTAICDParameter> &iparamList=QList<GTAICDParameter>());
    static GTASCXMLState getPreDebugState(const QString &iStateId, const QString &iTargetId, const QString &iPreviousStateId, const QString &iInstanceName);
    static QString getExportingFileName();
    static void setExportingFileName(QString exportingFile);
private:
    static QString _LastError;
    static QString _SCXMLTemplatePath;
    static QHash<QString,QString> _genToolReturnMap;
    static QString _exportingFileName;
private:
    static void groupParameters(const QList<GTAICDParameter> &iParamList, QHash<QString,QHash<QString, GTAICDParameter> > &oToolAppGroup);
    //static bool getCloneInvoke(const QString & iTemplateFileName, GTASCXMLInvoke &oInvoke);
    static bool openScxmlTemplateFile(const QString & iTemplateFileName, QDomDocument & oDomDoc);
    static QStringList getInstanceNameList(QString &iInstanceName);
};

#endif // GTACMDSCXMLUTILS_H
