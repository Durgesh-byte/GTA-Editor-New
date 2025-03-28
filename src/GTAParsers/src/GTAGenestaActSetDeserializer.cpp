#include "AINGTAGenestaActSetDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTAEquationBase.h"
#include "AINGTAEquationConst.h"
#include "AINGTAEquationGain.h"
#include "AINGTAEquationRamp.h"
#include "AINGTAEquationTrepeze.h"
#include "AINGTAEquationCrenels.h"
#include "AINGTAActionSet.h"
#include "AINGTAActionSetList.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

AINGTAGenestaActSetDeserializer::AINGTAGenestaActSetDeserializer(const QString& iStmt,const QString& isAircraft,const QString& isEquipment,const QString& iTimeOut):AINGTAGenestaParserItf(iStmt)
{
    _sAircraft=isAircraft;
    _sEquipment=isEquipment;
    _sExternalTimeOut="3000";
    bool isNum;
    iTimeOut.toDouble(&isNum);
    if(isNum)
        _sExternalTimeOut = iTimeOut;

}

bool AINGTAGenestaActSetDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC = false;
    //AINGTACommandBase*  pCmd = NULL;
    if (!_statement.isEmpty())
    {
        QStringList lstofStatement = _statement.split(" to ");
        if (lstofStatement.size()==2)
        {
            /* example --- [ set value IRS_1.ADIRU1_A429_IR_DATA_BUS3.ADIRU_162_Request_Bus_Designator_1 (162/13) 
            to 
            sine = 1*cos(2*@PI*2*(@t-3))+4 ] */


            //processing parameter part
            QString parameterName = lstofStatement.at(0);
            parameterName.remove("set value");
            QStringList lstofParams;
            getPreprocessedStatements(parameterName,lstofParams);

            if (lstofParams.isEmpty())
            {
                _lastError = "preprocessing of set statement failed";
                return rC;
            }
            else
            {
                AINGTAActionSetList* pSetCommandList = new AINGTAActionSetList(QString(ACT_SET),"");
                QList<AINGTAActionSet*> setCommandList;
                foreach(QString sParamInfo, lstofParams)
                {
                    replaceGenSymbols(sParamInfo);

                    QStringList paramParts = sParamInfo.split(" ",QString::SkipEmptyParts);
                    QString sParamName =paramParts.at(0);


                    //processing equation part;

                    QString parameterVal  = lstofStatement.at(1);
                    parameterVal;

                    AINGTAEquationBase* pEqn = NULL;


                    //todo move this code into equation deserializer.
                    if (!parameterVal.isEmpty())
                    {
                        bool isNum;
                        QStringList lstofvals;
                        getPreprocessedStatements(parameterVal,lstofvals);
                        if(!lstofvals.isEmpty())
                        {
                            parameterVal = lstofvals.at(0);
                        }
                        parameterVal.toDouble(&isNum);

                        if (parameterVal.contains(GENESTA_GAIN_EQN_IDENTIFIER))
                            pEqn = this->getGaingEqn(parameterVal);
                        else if (parameterVal.contains(GENESTA_RAMP_EQN_IDENTIFIER))
                            pEqn = this->getRampEqn(parameterVal);
                        else if (parameterVal.contains(GENESTA_TRAPEZE_EQN_IDENTIFIER))
                            pEqn = this->getTrapezeEqn(parameterVal);
                        else if (parameterVal.contains(GENESTA_CRENELS_EQN_IDENTIFIER))
                            pEqn = this->getCrenelEqn(parameterVal);
                        else if (parameterVal.contains(GENESTA_SINE_EQN_IDENTIFIER))
                            pEqn = this->getSinEqn(parameterVal);
                        else if (isNum)
                            pEqn = this->getConstantEqn(parameterVal);
                        else if (parameterVal.contains("@"))
                            pEqn = this->getConstantEqn(parameterVal);
                        else
                            _lastError="unidentified parameter Equation or value needs to be a number";
                    }



                    if (pEqn!=NULL && !parameterName.isEmpty())
                    {
                        AINGTAActionSet* pSetCmd = new AINGTAActionSet;
                        pSetCmd->setEquation(pEqn);
                        if (AINGTAGenestaParserItf::isTriplet(sParamName))
                        { /*sParamName = sParamName.toUpper();*/ sParamName.replace("-","_");}
                        pSetCmd->setParameter(sParamName);
                        pSetCmd->setAction(ACT_SET_INSTANCE);
                        //Genesta does not have set only FS functionality
                        pSetCmd->setIsSetOnlyFS(false);
                        pSetCmd->setActionOnFail(ACT_FAIL_CONTINUE);
                        setCommandList.append(pSetCmd);
                    }
                    else
                    {
                        if (sParamName.isEmpty())
                            _lastError = "Set value: could not evaluate parameter";

                    }

                }

                if (!setCommandList.isEmpty())
                {
                    pSetCommandList->setSetCommandlist(setCommandList);
                    pSetCommandList->setAction(ACT_SET);
                    oLstCmds.append(pSetCommandList);
                    rC=true;
                }
                else
                {
                    delete pSetCommandList;
                    pSetCommandList=NULL;
                    _lastError = "Set value: could not evaluate parameter";
                }



            }
            

        }
        else
            _lastError="evaluation of set statement failed (\" to \") not found";

    }
    else
        _lastError="Nothing to evaluate with empty statement";





    return rC;
}





AINGTAEquationBase* AINGTAGenestaActSetDeserializer::getConstantEqn(QString sEqnStatement)
{
    AINGTAEquationConst* pConstEqn=new AINGTAEquationConst;
    if (AINGTAGenestaParserItf::isTriplet(sEqnStatement))
    {/*sEqnStatement = sEqnStatement.toUpper();*/sEqnStatement.replace("-","_");}
    pConstEqn->setConstant(sEqnStatement.remove(" "));
    return pConstEqn;
}
AINGTAEquationBase* AINGTAGenestaActSetDeserializer::getGaingEqn(QString sEqnStatement)
{
    QString eqnSplitIdf;
    eqnSplitIdf=QString("%1%2").arg(GENESTA_GAIN_EQN_IDENTIFIER,"=");
    QStringList splitedItem = sEqnStatement.split(eqnSplitIdf);
    QString val = splitedItem.at(1);
    AINGTAEquationGain* pGain = NULL;
    bool isNum = false;
    if (!val.isEmpty())
    {
        val.remove(" ");
        val.toDouble(&isNum);
        if (isNum)
        {
            pGain = new AINGTAEquationGain;
            pGain->setGain(val);

        }
        else
            _lastError="Gain value has to be a number";
    }
    return pGain;
}
AINGTAEquationBase* AINGTAGenestaActSetDeserializer::getRampEqn(QString sEqnStatement)
{
    QStringList splitedItem = sEqnStatement.split(GENESTA_RAMP_EQN_IDENTIFIER);
    AINGTAEquationRamp* pRamp=NULL;
    bool isNum = false;
    if (splitedItem.size()>=2)
    {
        QString val = splitedItem.at(1);
        val.remove(" ");
        if (!val.isEmpty())
        {
            QStringList eqnItemList = val.split(",");;
            if (eqnItemList.size()>=2)
            {
                QString rampVal = eqnItemList.at(0).mid(5);//size of "ramp="
                QString offsetVal = eqnItemList.at(1).mid(7);//size of "offset="
                rampVal.toDouble(&isNum);
                if (isNum)
                {
                    offsetVal.toDouble(&isNum);
                    if(isNum)
                    {
                        pRamp = new AINGTAEquationRamp;
                        pRamp->setStart(offsetVal);
                        QString riseTime(GTA_RAMP_DEF_END_TIME);
                        QString endTime(GTA_RAMP_DEF_RISE_TIME);
                        pRamp->setEnd(riseTime);
                        pRamp->setRiseTime(endTime);
                        
                    }
                    else
                        _lastError="Ramp Equation: offset value hast to be a number";

                }
                else
                    _lastError="Ramp Equation:Ramp value has to be a number";


            }
        }
    }
    return pRamp;
}

AINGTAEquationBase* AINGTAGenestaActSetDeserializer::getTrapezeEqn(QString sEqnStatement)
{
    // trapeze = (level1= 1, length= 111, ramp1= 11, level2= 2, length= 222, ramp2= 22, level3= 3)
    sEqnStatement.remove(" ");
    sEqnStatement.remove(QString("%1%2%3").arg(GENESTA_TRAPEZE_EQN_IDENTIFIER,"=","("));
    sEqnStatement.remove(")");
    QStringList trapezeArguments = sEqnStatement.split(",");
    AINGTAEquationTrepeze* pTrapEqn = NULL;
    int argSize =trapezeArguments.size();
    bool isNum = false;
    if (argSize)
    {

        QHash<QString,QString> eqnArgMap;
        int levCnt =0;
        int rampCnt =0;
        int lenCnt=0;
        for (int i=0;i<argSize;i++)
        {
            QString currentArg = trapezeArguments.at(i);
            if (currentArg.contains("level"))
            {
                QStringList levelVallst = currentArg.split("=");
                if (levelVallst.size()>=2)
                {
                    QString levelVal = levelVallst.at(1);
                    levelVal.toDouble(&isNum);
                    if(isNum)
                        eqnArgMap.insert(QString("level%1").arg(QString::number(levCnt)),levelVal);

                }
                levCnt++;
            }
            else if (currentArg.contains("ramp"))
            {
                QStringList rampVallst = currentArg.split("=");
                if (rampVallst.size()>=2)
                {
                    QString rampVal = rampVallst.at(1);
                    rampVal.toDouble(&isNum);
                    if(isNum)
                        eqnArgMap.insert(QString("ramp%1").arg(QString::number(rampCnt)),rampVal);

                }
                rampCnt++;
            }
            else if (currentArg.contains("length"))
            {

                QStringList lenVallst = currentArg.split("=");
                if (lenVallst.size()>=2)
                {
                    QString lenVal = lenVallst.at(1);
                    lenVal.toDouble(&isNum);
                    if(isNum)
                        eqnArgMap.insert(lenVallst.at(0)+QString::number(lenCnt), lenVal);

                }
                lenCnt++;
            }

        }
        if (eqnArgMap.size()==GENESTA_TRAPEZE_EQN_ARG_SIZE)
        {
            pTrapEqn=new AINGTAEquationTrepeze;
            pTrapEqn->setEquationArguments(eqnArgMap);

        }
        else
            _lastError="Trapeze equation: Inappropriate arguments";

    }

    return pTrapEqn;
}
AINGTAEquationBase* AINGTAGenestaActSetDeserializer::getCrenelEqn(QString sEqnStatement )
{

    sEqnStatement.remove(" ");
    sEqnStatement.remove(QString("%1%2%3").arg(GENESTA_CRENELS_EQN_IDENTIFIER,"=","("));
    sEqnStatement.remove(")");
    QStringList crenelArguments = sEqnStatement.split(",");
    AINGTAEquationCrenels* pEqnCrenel = NULL;
    //bool isNum =false;
    int argSize =crenelArguments.size();
    if (argSize)
    {

        QHash<QString,QString> eqnArgMap;
        int levCnt =0;
        int lenCnt=0;
        bool isNum=false;
        for (int i=0;i<argSize;i++)
        {
            QString currentArg = crenelArguments.at(i);
            if (currentArg.contains("level"))
            {
                QStringList levelVallst = currentArg.split("=");
                if (levelVallst.size()>=2)
                {
                    QString levelVal = levelVallst.at(1);
                    levelVal.toDouble(&isNum);
                    if(isNum)
                        eqnArgMap.insert(QString("level%1").arg(QString::number(levCnt)),levelVal);

                }
                levCnt++;
            }
            else if (currentArg.contains("length"))
            {

                QStringList lenVallst = currentArg.split("=");
                if (lenVallst.size()>=2)
                {
                    QString lenVal = lenVallst.at(1);
                    lenVal.toDouble(&isNum);
                    if(isNum)
                        eqnArgMap.insert(lenVallst.at(0)+QString::number(lenCnt), lenVal);

                }
                lenCnt++;
            }

        }
        if (eqnArgMap.size()==GENESTA_CRENELS_EQN_ARG_SIZE)
        {
            pEqnCrenel=new AINGTAEquationCrenels;
            pEqnCrenel->setEquationArguments(eqnArgMap);
            
        }
        else
            _lastError="Crenels equation: Inappropriate arguments";

    }
    return pEqnCrenel;
}
AINGTAEquationBase* AINGTAGenestaActSetDeserializer::getSinEqn(QString sEqnStatement )
{
    //11*sin(2*@PI*22*(@t-33))+44
    //11*cos(2*@PI*22*(@t-33))+44    --> 11*sin(2*@PI*22*(@t-33))+44
    sEqnStatement.remove(" ");
    _lastError = "Sine equation not implemented";
    return NULL;

}
void AINGTAGenestaActSetDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
{

    //call object - PMA_Ø_×_¤_HW_FLT (Ø=both engines , ×=channel A, ¤=1)
    QMap<QString,QString> MapOfVarTags;
    QString sStatement = iStatement;
    QRegExp rgExp;

    // pattern should be either "(z" or "( z" or "(  z" so on. where z = Ø × ¤ or +
    QString sPattern = QString("\\(\\s+[%1%2%3%4%5%6%7]|\\([%1%2%3%4%5%6%7]").arg(GENESTA_ENGINE_VAR_IDENTIFIER1,
                                                                                  GENESTA_ENGINE_VAR_IDENTIFIER2,
                                                                                  GENESTA_ENGINE_VAR_IDENTIFIER3,
                                                                                  GENESTA_CHANNEL_VAR_IDENTIFIER1,
                                                                                  GENESTA_CHANNEL_VAR_IDENTIFIER2,
                                                                                  GENESTA_VAR1_IDENTIFIER,
                                                                                  GENESTA_VAR2_IDENTIFIER);
    rgExp.setPattern(sPattern);
    int dPos = rgExp.indexIn(sStatement,0);
    QString sStatementParam = sStatement.mid(0,dPos);
    bool bFADEC;

    if((_sAircraft == "SA" && ((_sEquipment=="CFM")||(_sEquipment=="PW"))) || (_sAircraft == "LR" &&(_sEquipment=="RR") ))
        bFADEC = true;
    else
        bFADEC = false;
    //    bool bFADEC = _sAircraft=="SA"?true:false;
    //    bFADEC = bFADEC && ((_sEquipment=="CFM")||(_sEquipment=="PW")||(_sEquipment=="RR"));
    if (bFADEC)
    {
        sStatement.replace("channel in control","both channels");
        sStatement.replace("channel not in control","both channelsR");
    }
    if (dPos!=-1)
    {

        int dEndPos= sStatement.indexOf(")",dPos);
        
        QString varValues = sStatement.mid(dPos+1,dEndPos-dPos-1); //(Ø=both engines, ×=channel A, ¤=1)

        QStringList newVarValueList;//should hold (Ø=engine 1, ×=channel A, ¤=1),(Ø=engine 2, ×=channel A, ¤=1)
        QStringList varVaueLst = varValues.split(",");

        //This is different for differenct command implement getpreprocessed statements for each command deserializer
        if (varValues.contains("channel in control"))
        {
            varValues.replace("channel in control","A");
        }
        if (varValues.contains("channel not in control"))
        {
            varValues.replace("channel not in control","B");
        }


        QStringList splChrVal = varValues.split(",");
        QStringList bothCh;
        QStringList bothEng;
        QStringList otherCh;
        for(int idx=0;idx<splChrVal.count();idx++)
        {
            QString splVal = splChrVal.at(idx);
            QStringList vals = splVal.trimmed().split("=");
            if((vals.count() > 1) && (vals.at(1).contains("both channelsR")))
            {
                bothCh.append(QString("%1=%2").arg(vals.at(0),"channel B"));
                bothCh.append(QString("%1=%2").arg(vals.at(0),"channel A"));
                
            }
            else if((vals.count() > 1) && (vals.at(1).contains("both channels")))
            {
                bothCh.append(QString("%1=%2").arg(vals.at(0),"channel A"));     
                bothCh.append(QString("%1=%2").arg(vals.at(0),"channel B"));
            }
            else if((vals.count() > 1) && (vals.at(1).startsWith("channel")))
            {
                bothEng.append(QString("%1=%2").arg(vals.at(0),vals.at(1)));
            }
            else if((vals.count() > 1) && (vals.at(1).contains("both engines")))
            {
                bothEng.append(QString("%1=%2").arg(vals.at(0),"engine 1"));     
                bothEng.append(QString("%1=%2").arg(vals.at(0),"engine 2"));
            }
            else if((vals.count() > 1) && (vals.at(1).startsWith("engine")))
            {
                bothEng.append(QString("%1=%2").arg(vals.at(0),vals.at(1)));
            }
            else 
            {
                if (vals.size()>1)
                {

                    otherCh.append(QString("%1=%2").arg(vals.at(0),vals.at(1)));
                }
            }
        }
        otherCh.removeDuplicates();
        QString ChStr = ""; 
        if(!bothCh.isEmpty())
            ChStr  = bothCh.join(";")+";";
        QString finalStr;
        if(bothEng.isEmpty())
        {
            finalStr = ChStr;
        }
        else
        {
            foreach(QString eng,bothEng)
            {
                if(!ChStr.isEmpty())
                {
                    QString chxengx = ChStr;
                    chxengx = chxengx.replace(";",","+eng+";");
                    finalStr +=chxengx+";";
                }
                else
                {
                    finalStr = bothEng.join(";");
                }
            }
        }
        if(!finalStr.isEmpty())
        {
            finalStr += ";";
            foreach(QString otrCh,otherCh)
            {
                otrCh = otrCh.trimmed();
                QRegExp regex(";");
                finalStr = finalStr.replace(regex,","+otrCh+";");
            }
        }
        else
        {
            finalStr = otherCh.join(",");
        }
        newVarValueList.clear();
        newVarValueList.append(finalStr.split(";",QString::SkipEmptyParts));
        foreach(QString sReplacedSt, newVarValueList)
        {
            QString newStatement = sStatementParam;
            newStatement.replace(QString(GENESTA_VAR1_IDENTIFIER),QString("@%1@").arg(GTA_VAR1_VARIABLE));
            oList.append(newStatement.append(QString ("(%1)").arg(sReplacedSt)));
        }


    }
    else
    {
        QMap<QString,QString> mapReplaceItems;
        mapReplaceItems.insert(QString(GENESTA_CHANNEL_VAR_IDENTIFIER1),QString("@%1@").arg(GTA_CHANNEL_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_VAR1_IDENTIFIER),QString("@%1@").arg(GTA_VAR1_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_ENGINE_VAR_IDENTIFIER1),QString("@%1@").arg(GTA_ENGINE_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_ENGINE_VAR_IDENTIFIER2),QString("@%1@").arg(GTA_ENGINE_VARIABLE));

        QString currentStatement = iStatement;

        QMapIterator<QString,QString>  it(mapReplaceItems);
        while (it.hasNext())
        {
            it.next();
            currentStatement.replace(it.key(),it.value());
        }
        oList.append(currentStatement);
    }
}
