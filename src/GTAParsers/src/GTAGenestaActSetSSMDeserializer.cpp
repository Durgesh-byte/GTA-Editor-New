#include "AINGTAGenestaActSetSSMDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTAActionSet.h"
#include "AINGTAActionSetList.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

AINGTAGenestaActSetSSMDeserializer::AINGTAGenestaActSetSSMDeserializer(const QString& iStmt,const QString& isAircraft,const QString& isEquipment):AINGTAGenestaParserItf(iStmt)
{
    _sAircraft=isAircraft;
    _sEquipment=isEquipment;

}

bool AINGTAGenestaActSetSSMDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
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



                    //todo move this code into equation deserializer.
                    if (!parameterVal.isEmpty())
                    {
                        
                        QStringList lstofvals;
                        getPreprocessedStatements(parameterVal,lstofvals);
                        if(!lstofvals.isEmpty())
                        {
                            parameterVal = lstofvals.at(0);
                        }
                        else
                            _lastError="SSM VALUE NOT PROVIDED";
                    }



                    if (parameterVal.isEmpty()==false && !parameterName.isEmpty())
                    {
                        AINGTAActionSet* pSetCmd = new AINGTAActionSet;
                        
                        if (AINGTAGenestaParserItf::isTriplet(sParamName))
                        {/*sParamName = sParamName.toUpper();*/ sParamName.replace("-","_");}
                        pSetCmd->setParameter(sParamName);
                        pSetCmd->setAction(ACT_SET_INSTANCE);
                        //Genesta does not have set only FS functionality
                        pSetCmd->setIsSetOnlyFS(false);
                        pSetCmd->setActionOnFail(ACT_FAIL_CONTINUE);
                        setCommandList.append(pSetCmd);
                        pSetCmd->setIsSetOnlyFS(true);
                        pSetCmd->setFunctionStatus(parameterVal);
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
void AINGTAGenestaActSetSSMDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
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
