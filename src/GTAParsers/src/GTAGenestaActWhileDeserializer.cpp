#include "AINGTAGenestaActWhileDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTAActionIF.h"
#include "AINGTAActionIfEnd.h"
#include "AINGTAActionWhile.h"
#include "AINGTAActionElse.h"
#include "AINGTAActionWhileEnd.h"
#include "AINGTAGenestaActSetDeserializer.h"
#include "AINGTAGenestaFileParser.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

AINGTAGenestaActWhileDeserializer::AINGTAGenestaActWhileDeserializer(const QString& iStmt, const QString& iComplement,QString isEquipment,QString isAircraft,const QString& iTimeOut):AINGTAGenestaParserItf(iStmt),_sComplement(iComplement)
{

    _sAircraft = isAircraft;
    _sEquipment = isEquipment;
    _sExternalTimeOut="3000";
    bool isNum;
    iTimeOut.toDouble(&isNum);
    if(isNum)
        _sExternalTimeOut = iTimeOut;
   
}

bool AINGTAGenestaActWhileDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC=false;


    if (_statement.isEmpty())
    {
        _lastError="Nothing to evaluate with empty statement";
        return rC;
    }


    

    if (!_statement.isEmpty())
    {

        QString sParamInfo;
        QString sValInfo;
        
        if (_statement.contains("†"))
        {
            int indexOfIfSeperator = _statement.indexOf(GENESTA_CONDITION_SEPERATOR_IDENTIFIER);
            QString conditionStatement = _statement.mid(0,indexOfIfSeperator);
            QString conditionStatementParam = conditionStatement.remove(_sComplement); //only if statement
            QString condStamentActionStatement = _statement.mid(indexOfIfSeperator+1);
            
            QStringList sComparators;
            sComparators<<"!="<<">="<<"<="<<"<"<<">"<<"=";
            
            bool bComparatorFound =false;
            foreach(QString sComparator, sComparators)
            {
                if (conditionStatement.contains(sComparator))
                {
                    bComparatorFound=true;
                    QStringList paramStatementLst = conditionStatement.split(QString (" %1 ").arg(sComparator));
                    if (paramStatementLst.size()==2)
                    {
                        sParamInfo = paramStatementLst.at(0);
                        QStringList lstOfParamName;
                        getPreprocessedStatements(sParamInfo,lstOfParamName);

                        sValInfo = paramStatementLst.at(1);
                        QStringList lstOfVals;
                        getPreprocessedStatements(sValInfo,lstOfVals);

                        //convert to upper
                        for (int i=0;i<lstOfParamName.size();i++)
                        {
                            QString str = lstOfParamName.at(i);
                            if (AINGTAGenestaParserItf::isTriplet(str))
                            {
                                /*str = str.toUpper();*/str.replace("-","_");;
                                lstOfParamName.takeAt(i);
                                lstOfParamName.insert(i,str);
                            }
                        }
                        for (int i=0;i<lstOfVals.size();i++)
                        {
                            QString str = lstOfVals.at(i);
                            if (AINGTAGenestaParserItf::isTriplet(str))
                            {
                                /*str = str.toUpper();*/str.replace("-","_");;
                                lstOfVals.takeAt(i);
                                lstOfVals.insert(i,str);
                            }
                        }


                        /**
                          combination of parameter values
                          */


                        bool bFADEC;

                        if((_sAircraft == "SA" && ((_sEquipment=="CFM")||(_sEquipment=="PW"))) || (_sAircraft == "LR" &&(_sEquipment=="RR") ))
                            bFADEC = true;
                        else
                            bFADEC = false;
//                        bool bFADEC = _sAircraft=="SA"?true:false;
//                        bFADEC = bFADEC && ((_sEquipment=="CFM")||(_sEquipment=="PW")||(_sEquipment=="RR"));

                        if (bFADEC)
                        {

                            AINGTAActionIF* pIF = getIFCommandForFADC(lstOfParamName,sComparator,lstOfVals,condStamentActionStatement);
                            if(!pIF)
                            {
                                _lastError = QString("Could not evaluate condition statement");
                                if (sParamInfo.size()>2 || sValInfo.size()>2)
                                {

                                }
                            }
                            else
                            {
                                oLstCmds.append(pIF);
                                rC=true;
                            }
                        }
                        else if( (lstOfParamName.size()>1 || lstOfVals.size()>1 ) &&  _sComplement == COM_CONDITION_WHILE)
                        {
                            _lastError = "While condition having multiple channel/engine info cannot be evaluated since no FADEC information is present";
                            return rC;
                        }
                        else
                        {   
                            QList<AINGTACommandBase*> lstOfCondCommands;
                            foreach(QString paramNameInfo,lstOfParamName)
                            {
                                replaceGenSymbols(paramNameInfo);
                                foreach(QString paramValinfo,lstOfVals)
                                {
                                    replaceGenSymbols(paramValinfo);
                                    /*To extract actual parameter name, we need to split it further and remove extra information.
                                    Ex:EIU_PW1.EIU1_BUS_A.Selected_main_engine_TOIL_316 (316) (Unit=deg C) < 23
                                    */
                                    QStringList lstParamInfo = paramNameInfo.split(" ",QString::SkipEmptyParts);
                                    QString sParameterName;



                                    if (!lstParamInfo.isEmpty())
                                        sParameterName = lstParamInfo.at(0);


                                    //on same lines as for parameter value can also have extra information

                                    replaceGenSymbols(paramValinfo);
                                    QStringList lstValInfo = paramValinfo.split(" ",QString::SkipEmptyParts);
                                    QString sValName;


                                    if (!lstValInfo.isEmpty())
                                        sValName = lstValInfo.at(0);

                                    //AINGTACommandBase* pCmd = NULL;

                                    if (!sValName.isEmpty() && !sParameterName.isEmpty())
                                    {

                                        if (AINGTAGenestaParserItf::isTriplet(sValName))
                                            {/*sValName=sValName.toUpper();*/sValName.replace("-","_");}

                                        if (AINGTAGenestaParserItf::isTriplet(sParameterName))
                                            {sParameterName=sParameterName.toUpper();sParameterName.replace("-","_");}

                                        
                                        replaceGenSymbols(sParameterName);
                                        AINGTAActionWhile* pWhileCmd = getNestedWhileCommand(sParameterName,sComparator,sValName,condStamentActionStatement);

                                        oLstCmds.append(pWhileCmd);
                                        rC = true;
                                    
                                    }


                                }
                            }
                            
   

                        }

                    }


                    break;
                }




            }//comparing each comparators

            if (bComparatorFound==false)
                _lastError=QString("Erroneous '%1' statement").arg(_sComplement);
        }
        else
            _lastError="Could not find condition seperator";
    }

   
    return rC;
}
QString AINGTAGenestaActWhileDeserializer::getSelectedChannelForFACD(QString isParameter)
{

    QString channelSelected = "";
    if(_sEquipment == "PW")
    {
        channelSelected = "EIU_PW#.EIU#_BUS_A.Channel_A_Selected_L005_12";
    }
    else if(_sEquipment == "CFM")
    {
        channelSelected = "EIU_CFM#.EIU#_BUS_A.Channel_A_Selected_L005_12";
    }
    else if(_sEquipment == "RR")
    {
        channelSelected = "EIVMU_RR_#.EIVMU#_A1.Channel_A_270_11";
    }

    if(isParameter.contains(_sEquipment+"1"))
    {
        channelSelected.replace("#","1");
    }
    else if(isParameter.contains(_sEquipment+"2"))
    {
        channelSelected.replace("#","2");
    }
    else
        channelSelected.replace("#","1");

    return channelSelected;
}
AINGTAActionWhile* AINGTAGenestaActWhileDeserializer::getNestedWhileCommand(const QString isParameter, const QString& isCondition,const QString& isValName,const QString&  isFinalcondStament)
{


    QString sParameter = isParameter.split(" ",QString::SkipEmptyParts).at(0);
    QString sValName = isValName.split(" ",QString::SkipEmptyParts).at(0);
    AINGTAActionWhile* pWhile = createWhileAction(sParameter,isCondition,sValName);
    
    if (pWhile)
    {

        QList<AINGTACommandBase*> lstOfCmds;
        AINGTAGenestaFileParser::createCommandsFromActionStatement(isFinalcondStament,lstOfCmds,_lastError,"","",_mGenstaSymbolMap);
        if (lstOfCmds.isEmpty())
        {
            _lastError.append(",\nCould not Evaluate Command under condition");
        }
        else
        {
            pWhile->insertChildren(lstOfCmds.at(0),0);
        }
    }
    
   

    return pWhile;
}

AINGTAActionIF* AINGTAGenestaActWhileDeserializer::getIFCommandForFADC(const QStringList& isParameterLst, const QString& isCondition,const QStringList& isValLst,const QString&  isFinalcondStament)
{

    QString sParameForIF;
    QString sParamForElse;
    QString sValueForIF;
    QString sValueForElse;

    int lstSizeForParams = isParameterLst.size();
    int lstSizeForVals   =isValLst.size();
    //
    //for (int i=0;i<isParameterLst.size();i++)
    //{
    //    QString str = isParameterLst.at(i);
    //    if (AINGTAGenestaParserItf::isTriplet(str))
    //    {
    //        str = str.toUpper();
    //        isParameterLst.takeAt(i);
    //        isParameterLst.insert(i,str);
    //    }
    //}
    //for (int i=0;i<isValLst.size();i++)
    //{
    //    QString str = isValLst.at(i);
    //    if (AINGTAGenestaParserItf::isTriplet(str))
    //    {
    //        str = str.toUpper();
    //        isValLst.takeAt(i);
    //        isValLst.insert(i,str);
    //    }
    //}

    AINGTAActionIF* pChannelSelIF = NULL;
    if (lstSizeForParams && lstSizeForVals)
    {
        if (lstSizeForParams==2 || lstSizeForVals==2)
        {
            sParameForIF = isParameterLst.at(0);
            sValueForIF = isValLst.at(0);
           

            if (lstSizeForParams==2)
                sParamForElse = isParameterLst.at(1);
            else
                sParamForElse = isParameterLst.at(0);


            if (lstSizeForVals==2)
                sValueForElse = isValLst.at(1);
            else
                sValueForElse = isValLst.at(0);

        }
        else if (lstSizeForParams>2)
        {
            _lastError = "Multiple parameters not applicable";
            return pChannelSelIF;
        }


        //int paramLstSize=isParameterLst.size();


        QString channelSelected = getSelectedChannelForFACD(sParameForIF);
        pChannelSelIF = createIfAction(channelSelected,ARITH_EQ,"1");

        replaceGenSymbols(sParameForIF);
        replaceGenSymbols(sParamForElse);
        replaceGenSymbols(sValueForIF);
        replaceGenSymbols(sValueForElse);

        AINGTAActionWhile* pWhileForIF = getNestedWhileCommand(sParameForIF,isCondition,sValueForIF,isFinalcondStament);

        pWhileForIF->setComplement(COM_CONDITION_WHILE);
        pWhileForIF->setAction(ACT_CONDITION);
        pWhileForIF->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);

        pChannelSelIF->insertChildren(pWhileForIF,0);

        AINGTAActionWhile* pWhileForElse = getNestedWhileCommand(sParamForElse,isCondition,sValueForElse,isFinalcondStament);
        AINGTAActionElse* pChannelElse = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
        pChannelElse->insertChildren(pWhileForElse,0);
        pWhileForElse->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
        pChannelSelIF->insertChildren(pChannelElse,1);

        pChannelSelIF->insertChildren(new AINGTAActionIfEnd,2);

       
    }
    
 return pChannelSelIF;

}
AINGTAActionIF* AINGTAGenestaActWhileDeserializer::createIfAction(QString isParameter, QString isCondition,QString isValName)
{

    if (isParameter.isEmpty()||isCondition.isEmpty()||isValName.isEmpty())
    {
        return NULL;
    }

    AINGTAActionIF* pIfCmd = new AINGTAActionIF;
    pIfCmd->setParameter(isParameter);
    pIfCmd->setValue(isValName);
    pIfCmd->setCondition(isCondition);
    pIfCmd->setComplement(COM_CONDITION_IF);
    pIfCmd->insertChildren(new AINGTAActionIfEnd,0);
    pIfCmd->setAction(ACT_CONDITION);
    return pIfCmd;
}
AINGTAActionWhile* AINGTAGenestaActWhileDeserializer::createWhileAction(QString isParameter, QString isCondition,QString isValName)
{

    if (isParameter.isEmpty()||isCondition.isEmpty()||isValName.isEmpty())
    {
        return NULL;
    }

    AINGTAActionWhile* pIfCmd = new AINGTAActionWhile;
    pIfCmd->setParameter(isParameter);
    pIfCmd->setValue(isValName);
    pIfCmd->setCondition(isCondition);
    pIfCmd->setComplement(COM_CONDITION_WHILE);
    pIfCmd->insertChildren(new AINGTAActionWhileEnd,0);
    pIfCmd->setAction(ACT_CONDITION);
    return pIfCmd;
}

void AINGTAGenestaActWhileDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
{

    //call object - PMA_Ø_×_¤_HW_FLT (Ø=both engines , ×=channel A, ¤=1)
    QMap<QString,QString> MapOfVarTags;
    QString sStatement = iStatement;
    QRegExp rgExp;

    // pattern should be either "(z" or "( z" or "(  z" so on. where z = Ø × ¤ or +
    QString sPattern = QString("\\(\\s+[%1%2%3%4%5%6%7]|\\([%1%2%3%4%5%6%7]").arg( GENESTA_ENGINE_VAR_IDENTIFIER1,
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
    if(bFADEC)
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
