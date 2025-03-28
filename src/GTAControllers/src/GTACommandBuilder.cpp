#include "GTACommandBuilder.h"

#pragma warning(push, 0)
#include <QStringList>
#pragma warning(pop)

GTACommandBuilder::GTACommandBuilder()
{
}
bool GTACommandBuilder::getAction(const QString iActionCmd, QString &, QString &)
{
	QStringList cmdSplit = iActionCmd.split(" ");
	if(cmdSplit.count()> 2)
	{
		/*if( cmdSplit.at(0).contains("call"))*/

	}
	return false;
}
QString GTACommandBuilder::getActionSet(const QString & iAction, const QString &,
                         const QString &iParam, const QString &iValue )
{
    QString oAction = iAction + " value " + iParam + " to " + iValue;
    return oAction;
}


QString GTACommandBuilder::getActionCall(const QString & iAction, const QString & iComplement, const QString & iParam)
{
	QString oAction = QString("%1 %2 - %3").arg(iAction,iComplement,iParam);
	return oAction;
}
//QString GTACommandBuilder::getActionObject(const QString & iAction, const QString & iComplement, const QString & iParam)
//{
//    QString oAction = iComplement + " - " + iParam;
//    return oAction;
//}

QString GTACommandBuilder::getActionRelease(const QString & iAction, const QString & iComplement,const QString & iParam)
{
	QString oAction = QString("%1 %2 - %3").arg(iAction,iComplement,iParam);
	return oAction;
}

QString GTACommandBuilder::getActionPrint(const QString & iAction, const QString &, const QString & iParam)
{
    QString oAction = iAction + " " + iParam;
    return oAction;
}

QString GTACommandBuilder::getActionWaitFor(const QString & iAction, const QString & iComplement, const QString & iValue)
{
    QString oAction = iAction + " " + iComplement + " " + iValue + " S";
    return oAction;
}

QString GTACommandBuilder::getActionWaitUntil(const QString & iAction, const QString & iComplement,
                                  const QString & iParam,const QString & iValue,const QString & iOperator)
{
    QString oAction = iAction + " " + iComplement + " " + iParam + " " + iOperator + " " + iValue;
    return oAction;
}

QString GTACommandBuilder::getActionManualAction(const QString & iAction, const QString & iValue)
{
    QString oAction = iAction + " - " + iValue;
    return oAction;
}

QString GTACommandBuilder::getActionCondition(const QString &, const QString & iComplement,
                                  const QString & iParam, const QString & iValue, const QString &,
                                  const QString & iAnotherAction)
{
    QString oAction = iComplement + " " + iParam + " " + iValue;
    oAction += "\n" + iAnotherAction;
    return oAction;
}

QString GTACommandBuilder::getCommentSet(const QString & iComplement, const QString & iSimulation)
{
    QString oComment = "";
    QString state = "";
    QString operation = "";
    QStringList braceSplit = iComplement.split("(");
    if(braceSplit.count() > 1)
    {
        QString comp ;
        if(braceSplit[0].contains("+"))
            comp = braceSplit.at(0);
        else
            comp = braceSplit.at(1);
        QStringList plusSplit = comp.split("+");
        if(plusSplit.count() > 1)
        {
            state = plusSplit.at(0);
            operation = plusSplit.at(1).mid(0,plusSplit.at(1).length()-1);
        }

    }
    if(iSimulation.isEmpty())
    {
        oComment = state + ", " + operation;
    }
    else
        oComment = iSimulation + ", " + state + ", " + operation;

	return oComment;
}

QString GTACommandBuilder::getEquationConstants(const QString &iValue)
{
    return iValue;
}
QString GTACommandBuilder::getEquationGain(const QString &iValue)
{
    QString oEquation = "(value*gain) with gain= " + iValue;
    return oEquation;
}
QString GTACommandBuilder::getEquationRamp(const QString &iRamp, const QString &iOffset)
{
    QString oEquation = "(ramp*time+offset) with ramp= " + iRamp + ", offset= " + iOffset;
    return oEquation;
}
QString GTACommandBuilder::getEquationCrenels(const QHash<int, QString> &iLevels, const QHash<int, QString> &iLength)
{
    QString oEquation = "crenels = (level1= " + iLevels.value(1) + ", length= " + iLength.value(1);
            oEquation += ", level2= " + iLevels.value(2) + ", length= " + iLength.value(2);
            oEquation += ", level3= " + iLevels.value(3) + ", length= " + iLength.value(3);
            oEquation += ", level4= " + iLevels.value(4) + ", length= " + iLength.value(4);
            oEquation += ", level5= " + iLevels.value(5) + ")";

            return oEquation;
}
QString GTACommandBuilder::getEquationSineCurve(const QString &iGain, const QString iTrignoOper,
                                                   const QString &iPulsation, const QString &iPhase,
                                                   const QString &iOffset)
{
    QString oEquation = "sine = " +iGain + "*" + iTrignoOper + "(2*@PI*" + iPulsation + "*(@t-" + iPhase + "))+" + iOffset;
    return oEquation;
}
QString GTACommandBuilder::getEquationTrepeze(const QHash<int, QString> &iLevels,
                                                 const QHash<int, QString> &iRamp, const QHash<int, QString> &iLength)
{

    QString oEquation = "trapeze = (level1= " + iLevels.value(1) + ", length= " + iLength.value(1) + ", ramp1= " + iRamp.value(1);
            oEquation += ", level2= " + iLevels.value(2) + ", length= " + iLength.value(2) + ", ramp2= " + iRamp.value(1);
            oEquation += ", level3= " + iLevels.value(3) + ")";
    return oEquation;
}
QString GTACommandBuilder::getCheckValue(const QString & iCheckType, QString & iParam1,const QString & iVal1,const QString & iOperator1,
                             const QString & iParam2,const QString & iVal2,const QString & iOperator2)
{
    QString oCheck = QString("check %1 %2 %3 %4").arg(iCheckType,iParam1,iOperator1,iVal1);
    if(! iParam2.isEmpty() && (! iVal2.isEmpty()) && (! iOperator2.isEmpty()))
    {
        oCheck = QString("check %1 %2 %3 %4 OR %5 %6 %7").arg(iCheckType,iParam1,iOperator1,iVal1,iParam2,iOperator2,iVal2);
    }
	return oCheck;
}
//check FWC Warning “x” is displayed on E/WD
QString GTACommandBuilder::getCheckFWCWarning(const QString &iCheckType, const QString &iWarning,
                                                 const QString &iCondition, const QString & iOnDisplay)
{
    QString oCheck = QString("check %1 \"%2\" is %3 displayed on %4").arg(iCheckType,iWarning,iCondition,iOnDisplay);
    return oCheck;
}

QString GTACommandBuilder::getCheckFWCProcedure(const QString &iCheckType, const QHash<int, QString> iProcedureList,
                                                   const QString &iCondition, const QString &iOnDisplay)
{
    QString oCheck = "check " + iCheckType + " ";
    for(int i = 1 ; i<= iProcedureList.count();i++)
    {
        if(iProcedureList.contains(i))
            oCheck += "\"" +  iProcedureList.value(i) + "\" ";
    }
    oCheck += " is " + iCondition + " displayed on " + iOnDisplay;
	return oCheck;
}
QString GTACommandBuilder::getCheckBiteMessage(const QString & iCheckType, const QString & iMsg, const QString & iCondition,
                              const QString & iReport)
{
    QString oCheck = QString("check %1 \"%2\" is %3 displayed on %4").arg(iCheckType,iMsg,iCondition,iReport);
    return oCheck;
}
QString GTACommandBuilder::getCheckECAMDisplay(const QString & iCheckType, const QString & iDispType, const QString & iValue)
{
    //"check ECAM Display page change to “Engine”"

    QString oCheck = QString("check %1 %2 \"%3\"").arg(iCheckType,iDispType,iValue);
    return  oCheck ;
}
QString GTACommandBuilder::getCheckAudioAlarm(const QString &iCheckType, const QString &iAlarmType, const QString &iValue, const QString & iTime)
{
    //"check Audio Alarm of type “synthetic voice Auto-call-out” is heard after 10s"
    QString oCheck = QString("check %1 of type \"%2 %3\" is heard after %4s").arg(iCheckType,iAlarmType,iValue,iTime);
    return  oCheck ;
}
QString GTACommandBuilder::getCheckDisplayValue(const QString &iCheckType, const QString &iValue)
{
    //check Visual Display “abcda”
    QString oCheck = QString("check %1 \"%2\"").arg(iCheckType,iValue);
    return oCheck;
}
QString GTACommandBuilder::getCheckManual(const QString &, const QString &iValue)
{
    QString oCheck = QString("check that \"%1\"").arg(iValue);
    return oCheck;
}
QString GTACommandBuilder::getComment(const QString &iCommentType, const QString &iValue)
{
    QString oComment = QString("add %1 - %2").arg(iCommentType,iValue);
    return oComment;
}
QString GTACommandBuilder::getInitConfiguration(const QString & iConfig, const QString & iGrndAlt, const QString & iDISA)
{
    QString oConfig = QString("%1\nGround altitude = %2\nDISA = %3 degC").arg(iConfig,iGrndAlt,iDISA);
    return oConfig;
}
QString GTACommandBuilder::getInitConfiguration(const QString & iConfig, const QString & iGrndAlt, const QString & iDISA,
                                    const QString & iAltSpd, const QString & iThrottle)
{
    QString oConfig = QString("%1\nGround altitude = %2\nAlt/Spd = %3\nThrottle 1+2 = %4\nDISA = %5 degC").arg(iConfig,iGrndAlt,iAltSpd,iThrottle,iDISA);
    return oConfig;
}
QString GTACommandBuilder::getInitConfiguration(const QString & iConfig, const QString & iGrndAlt, const QString & iDISA,
                                    const QString & iEngines)
{
    QString oConfig = QString("%1\nGround altitude = %2\nEngines = %3\nDISA = %4degC").arg(iConfig,iGrndAlt,iEngines,iDISA);
    return oConfig;
}
