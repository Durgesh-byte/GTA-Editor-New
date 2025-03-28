#ifndef GTACOMMANDBUILDER_H
#define GTACOMMANDBUILDER_H
#include "GTAControllers.h"
#pragma warning(push, 0)
#include <QString>
#include <QHash>
#pragma warning(pop)
class GTAControllers_SHARED_EXPORT GTACommandBuilder
{
public:
    GTACommandBuilder();

	static bool getAction(const QString iActionCmd, QString & oAction, QString & oComplment);

    static QString getActionSet(const QString & iAction, const QString & iComplement,
                             const QString &iParam, const QString &iValue );
	
	static QString getActionCall(const QString & iAction, const QString & iComplement, const QString & iParam);
    //static QString getActionObject(const QString & iAction, const QString & iComplement, const QString & iParam);

    static QString getActionRelease(const QString & iAction, const QString & iComplement,const QString & iParam);

    static QString getActionPrint(const QString & iAction, const QString & iComplement, const QString & iParam);

    static QString getActionWaitFor(const QString & iAction, const QString & iComplement, const QString & iValue);

    static QString getActionWaitUntil(const QString & iAction, const QString & iComplement,
                                      const QString & iParam,const QString & iValue,const QString & iOperator);

    static QString getActionManualAction(const QString & iAction, const QString & iValue);

    static QString getActionCondition(const QString & iAction, const QString & iComplement,
                                      const QString & iParam, const QString & iValue, const QString & iOperator,
                                      const QString & iAnotherAction);



    static QString getCommentSet(const QString & iComplement, const QString & iSimulation);


    static QString getEquationConstants(const QString & iValue) ;

    static QString getEquationGain(const QString & iValue) ;

    static QString getEquationRamp(const QString & iRamp, const QString & iOffset) ;

    static QString getEquationTrepeze(const QHash<int,QString> & iLevels, const QHash<int,QString> & iRamp,
                                      const QHash<int, QString> & iLength) ;

    static QString getEquationCrenels(const QHash<int,QString> & iLevels, const QHash<int, QString> & iLength) ;

    static QString getEquationSineCurve(const QString & iGain, const QString iTrignoOper,
                                        const QString & iPulsation, const QString & iPhase, const QString & iOffset) ;

    //checks
    static QString getCheckValue(const QString & iCheckType, QString & iParam1,const QString & iVal1,const QString & iOperator1,
                                 const QString & iParam2 = QString(),const QString & iVal2 = QString(),const QString & iOperator2 = QString());
    static QString getCheckFWCWarning(const QString & iCheckType, const QString & iWarning,
                                      const QString & iCondition,const QString & iOnDisplay );
    static QString getCheckFWCProcedure(const QString & iCheckType, const QHash<int,QString> iProcedureList,
                                        const QString & iCondition,const QString & iOnDisplay );
    static QString getCheckBiteMessage(const QString & iCheckType, const QString & iMsg, const QString & iCondition,
                                  const QString & iReport);

    static QString getCheckECAMDisplay(const QString & iCheckType, const QString & iDispType, const QString & iValue);
    static QString getCheckAudioAlarm(const QString & iCheckType, const QString & iAlarmType, const QString & iValue, const QString & iTime);

    static QString getCheckDisplayValue(const QString & iCheckType, const QString & iValue);
    static QString getCheckManual(const QString & iCheckType, const QString & iValue);

    //Comments
    static QString getComment(const QString & iCommentType, const QString & iValue);

    //init configuration
    static QString getInitConfiguration(const QString & iConfig, const QString & iGrndAlt, const QString & iDISA);
    static QString getInitConfiguration(const QString & iConfig, const QString & iGrndAlt, const QString & iDISA,
                                        const QString & iEngines);
    static QString getInitConfiguration(const QString & iConfig, const QString & iGrndAlt, const QString & iDISA,
                                        const QString & iAltSpd, const QString & iThrottle);

};

#endif // GTACOMMANDBUILDER_H
