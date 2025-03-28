#include "AINGTAGenestaActIFDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTAActionIF.h"
#include "AINGTAActionElse.h"
#include "AINGTAActionIfEnd.h"
#include "AINGTAActionWhile.h"
#include "AINGTAActionWhileEnd.h"
#include "AINGTAGenestaActSetDeserializer.h"
#include "AINGTAGenestaFileParser.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

AINGTAGenestaActIFDeserializer::AINGTAGenestaActIFDeserializer(const QString& iStmt, const QString& iComplement,QString isEquipment,QString isAircraft, const QString& iTimeOut):AINGTAGenestaParserItf(iStmt),_sComplement(iComplement)
{

    _sAircraft = isAircraft;
    _sEquipment = isEquipment;
    _sExternalTimeOut="3000";
    bool isNum;
    iTimeOut.toDouble(&isNum);
    if(isNum)
        _sExternalTimeOut = iTimeOut;
}

bool AINGTAGenestaActIFDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
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
        
        if (_statement.contains(GENESTA_CONDITION_SEPERATOR_IDENTIFIER))
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
                        QList<SGenestaParamInfo> lstOfParam;
                        getPreprocessedStatements(sParamInfo,lstOfParam);

                        sValInfo = paramStatementLst.at(1);
                        QList<SGenestaParamInfo> lstOfVals;
                        getPreprocessedStatements(sValInfo,lstOfVals);

                        bool bFADEC;

                        if((_sAircraft == "SA" && ((_sEquipment=="CFM")||(_sEquipment=="PW"))) || (_sAircraft == "LR" &&(_sEquipment=="RR") ))
                            bFADEC = true;
                        else
                            bFADEC = false;

                        //                        bool bFADEC = _sAircraft=="SA"?true:false;
                        //                        bFADEC = bFADEC && ((_sEquipment=="CFM")||(_sEquipment=="PW")||(_sEquipment=="RR"));

                        bool bHasChannelInfo1 = _statement.contains(GENESTA_CHANNEL_VAR_IDENTIFIER1);
                        bool bHasChannelInfo2 =  _statement.contains(GENESTA_CHANNEL_VAR_IDENTIFIER2);
                        bool bHasChannelInfo = bHasChannelInfo1 | bHasChannelInfo2;
                        if (bFADEC && bHasChannelInfo)
                        {
                            AINGTAActionIF* pIF;
                            if(conditionStatement.contains("channel in control") || conditionStatement.contains("channel not in control"))
                            {
                                pIF = getIFCommandForFADC(lstOfParam,sComparator,lstOfVals,condStamentActionStatement,true);
                            }
                            else
                            {

                                if(conditionStatement.contains("both channels"))
                                {
                                    QList<SGenestaParamInfo> tmpLstOfParam;
                                    SGenestaParamInfo tmpGenestaParamInfo = lstOfParam.at(0);
                                    tmpLstOfParam.append(tmpGenestaParamInfo);
                                    pIF = getNestedIFCommand(tmpLstOfParam,sComparator,lstOfVals,condStamentActionStatement);
                                    if(!pIF)
                                        _lastError = QString("Could not evaluate condition statement");
                                    else
                                        oLstCmds.append(pIF);

                                    tmpLstOfParam.clear();
                                    tmpGenestaParamInfo = lstOfParam.at(1);
                                    tmpLstOfParam.append(tmpGenestaParamInfo);
                                    pIF = getNestedIFCommand(tmpLstOfParam,sComparator,lstOfVals,condStamentActionStatement);

                                }
                                else
                                {
                                    pIF = getNestedIFCommand(lstOfParam,sComparator,lstOfVals,condStamentActionStatement);
                                }
                            }

                            
                            if(!pIF)
                                _lastError = QString("Could not evaluate condition statement");
                            else
                            {
                                oLstCmds.append(pIF);
                                rC=true;
                            }

                        }
                        else
                        {

                            AINGTAActionIF* pIF = getNestedIFCommand(lstOfParam,sComparator,lstOfVals,condStamentActionStatement);
                            if(!pIF)
                                _lastError = QString("Could not evaluate condition statement");
                            else
                            {
                                oLstCmds.append(pIF);
                                rC=true;
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
QStringList AINGTAGenestaActIFDeserializer::getChannelSelectedForSignals(const QStringList& iSignalListLHS,const QStringList& iSignalListRHS)const
{
    QStringList channelSelectedLst;
    QStringList iSignalList = QStringList()<<iSignalListLHS<<iSignalListRHS;
    for (int il=0;il<iSignalList.size();il++)
    {
        QString channelSelected = "";

        QString sparamVal = iSignalList.at(0);
        replaceGenSymbols(sparamVal);
        if(_sEquipment == "PW")
        {
            channelSelected = "EIU_PW#.EIU#_BUS_A.channel_A_selected_L005_12";
        }
        else if(_sEquipment == "CFM")
        {
            channelSelected = "EIU_CFM#.EIU#_BUS_A.channel_A_selected_L005_12";
        }
        else if(_sEquipment == "RR")
        {
            channelSelected = "EIVMU_RR_#.EIVMU#_A1.Channel_A_270_11";
        }
        if(sparamVal.contains(_sEquipment+"1"))
        {
            channelSelected.replace("#","1");
            channelSelectedLst.append(channelSelected);
        }
        else if(sparamVal.contains(_sEquipment+"2"))
        {
            channelSelected.replace("#","2");
            channelSelectedLst.append(channelSelected);
        }
        else
        {
            if(il==iSignalList.size()-1 && channelSelectedLst.isEmpty())                
            {
                channelSelected.replace("#","1");
                channelSelectedLst.append(channelSelected);
            }
        }

    }
    channelSelectedLst.removeDuplicates();

    return channelSelectedLst;
}
QString AINGTAGenestaActIFDeserializer::getSelectedChannelForFACD(QString isParameter)
{

    QString channelSelected = "";
    if(_sEquipment == "PW")
    {
        channelSelected = "EIU_PW#.EIU#_BUS_A.channel_A_selected_L005_12";
    }
    else if(_sEquipment == "CFM")
    {
        channelSelected = "EIU_CFM#.EIU#_BUS_A.channel_A_selected_L005_12";
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


AINGTAActionIF* AINGTAGenestaActIFDeserializer::getIFCommandForFADC(const QList<SGenestaParamInfo>& isParameterLst, const QString& isCondition,const QList<SGenestaParamInfo>& isValLst,const QString&  isFinalcondStament, bool isChannelInControl)
{

    QList<SGenestaParamInfo> sParamelstIfSet;
    QList<SGenestaParamInfo> sValueLstIFSet;

    QList<SGenestaParamInfo> sParamLstElseSet;
    QList<SGenestaParamInfo> sValueLstElseSet;


    if (isParameterLst.size()==2)
    {
        sParamelstIfSet.append(isParameterLst.at(0));
        sParamLstElseSet.append(isParameterLst.at(1));
    }

    else if (isParameterLst.size()==4)
    {
        sParamelstIfSet.append(isParameterLst.at(0));
        sParamelstIfSet.append(isParameterLst.at(1));
        sParamLstElseSet.append(isParameterLst.at(2));
        sParamLstElseSet.append(isParameterLst.at(3));
    }
    else if (isParameterLst.size()==1)
    {
        sParamelstIfSet.append(isParameterLst.at(0));
    }


    if (isValLst.size()==2)
    {
        sValueLstIFSet.append(isValLst.at(0));
        sValueLstElseSet.append(isValLst.at(1));
    }

    else if (isValLst.size()==4)
    {
        sValueLstIFSet.append(isValLst.at(0));
        sValueLstIFSet.append(isValLst.at(1));
        sValueLstElseSet.append(isValLst.at(2));
        sValueLstElseSet.append(isValLst.at(3));
    }
    else if (isValLst.size()==1)
    {
        sValueLstIFSet.append(isValLst.at(0));
        sValueLstElseSet.append(isValLst.at(0));
    }


    isParameterLst.size();
    AINGTAActionIF* pChannelSelIF;
    if(isChannelInControl)
    {
        QString channelSelected = getSelectedChannelForFACD(sParamelstIfSet.at(0)._Parameter);
        pChannelSelIF = createIfAction(channelSelected,ARITH_EQ,"1");

        AINGTAActionIF* pIfForIF = getNestedIFCommand(sParamelstIfSet,isCondition,sValueLstIFSet,isFinalcondStament);

        pChannelSelIF->insertChildren(pIfForIF,0);

       /* AINGTAActionIF* pIFForElse = getNestedIFCommand(sParamLstElseSet,isCondition,sValueLstElseSet,isFinalcondStament);
        AINGTAActionElse* pChannelElse = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
        pChannelElse->insertChildren(pIFForElse,0);
        pChannelSelIF->insertChildren(pChannelElse,1);*/
    }


    return pChannelSelIF;


    




}
AINGTAActionIF* AINGTAGenestaActIFDeserializer::getNestedIFCommand(const QList<SGenestaParamInfo>& isParameterLst, const QString& isCondition,const QList<SGenestaParamInfo>& isValLst,const QString&  isFinalcondStament)
{

    
    AINGTAActionIF* pFirstIF = NULL;
    AINGTAActionIF* pParentIF = NULL;
    int paramLstSize=isParameterLst.size();
    int valSize = isValLst.size();
    for (int i=0;i<paramLstSize;i++)
    {
        
        SGenestaParamInfo paramInfo = isParameterLst.at(i) ;
        QString paramName = paramInfo._Parameter;
        replaceGenSymbols(paramName);
        QStringList lstParamInfo = paramName.split(" ",QString::SkipEmptyParts);
        if (!lstParamInfo.isEmpty())
            paramName = lstParamInfo.at(0);

        for (int j=0;j<valSize;j++)
        {

            SGenestaParamInfo ValueInfo = isValLst.at(j) ;

            if (   paramInfo.hasChannel() && ValueInfo.hasChannel() ) 
                if(paramInfo._Channel != ValueInfo._Channel)
                    continue;

            if (   paramInfo.hasEngine() && ValueInfo.hasEngine() ) 
                if(paramInfo._Engine != ValueInfo._Engine)
                    continue;
            
            QString sValue = ValueInfo._Parameter;
            replaceGenSymbols(sValue);
            QStringList lstvalInfo = sValue.split(" ",QString::SkipEmptyParts);
            if (!lstvalInfo.isEmpty())
                sValue = lstvalInfo.at(0);
            
            AINGTAActionIF* pCurrentIf = createIfAction(paramName,isCondition,sValue);

            if (!i && !j)
                pFirstIF = pCurrentIf;

            if (paramLstSize-1 == i && valSize -1 == j)
            {
                QList<AINGTACommandBase*> lstOfCmds;
                AINGTAGenestaFileParser::createCommandsFromActionStatement(isFinalcondStament,lstOfCmds,_lastError,"","",_mGenstaSymbolMap);
                if (lstOfCmds.isEmpty())
                {
                    _lastError.append(",\nCould not Evaluate Command under condition");
                }
                else
                {
                    pCurrentIf->insertChildren(lstOfCmds.at(0),0);
                }


            }

            if (pParentIF)
            {

                pParentIF->insertChildren(pCurrentIf,0);

            }
            pParentIF = pCurrentIf;
        }



        


    }

    return pFirstIF;
}
AINGTAActionIF* AINGTAGenestaActIFDeserializer::createIfAction(QString isParameter, QString isCondition,QString isValName)
{

    if (isParameter.isEmpty()||isCondition.isEmpty()||isValName.isEmpty())
    {
        return NULL;
    }
    

    AINGTAActionIF* pIfCmd = new AINGTAActionIF;
    if (AINGTAGenestaParserItf::isTriplet(isParameter))
    {
        //isParameter = isParameter.toUpper() ;
        isParameter.replace("-","_");
    }
    pIfCmd->setParameter(isParameter);

    if (AINGTAGenestaParserItf::isTriplet(isValName))
    {/*isValName = isValName.toUpper();*/isValName.replace("-","_");   }
    pIfCmd->setValue(isValName);
    pIfCmd->setCondition(isCondition);
    pIfCmd->setComplement(COM_CONDITION_IF);
    pIfCmd->insertChildren(new AINGTAActionIfEnd,0);
    pIfCmd->setAction(ACT_CONDITION);
    return pIfCmd;
}

void AINGTAGenestaActIFDeserializer::getPreprocessedStatements(const QString& iStatement, QList<SGenestaParamInfo> &oList)
{

    //call object - PMA_Ø_×_¤_HW_FLT (Ø=both engines , ×=channel A, ¤=1)
    QMap<QString,QString> MapOfVarTags;
    QString sStatement = iStatement;
    QRegExp rgExp;
    QStringList ListOfParam;
    
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
//    if(bFADEC)
//    {
//        sStatement.replace("channel in control","both channels");
//        sStatement.replace("channel not in control","both channelsR");
//    }

    if (dPos!=-1)
    {

        int dEndPos= sStatement.indexOf(")",dPos);
		QString sParam = sStatement.mid(0,dPos);
        QString varValues = sStatement.mid(dPos+1,dEndPos-dPos-1); //(Ø=both engines, ×=channel A, ¤=1)

        QStringList newVarValueList;//should hold (Ø=engine 1, ×=channel A, ¤=1),(Ø=engine 2, ×=channel A, ¤=1)
        QStringList varVaueLst = varValues.split(",");

        //This is different for differenct command implement getpreprocessed statements for each command deserializer
//        if (varValues.contains("channel in control"))
//        {
//            varValues.replace("channel in control","A");
//            //varValues.replace("channel in control","CIC");
//        }
//        if (varValues.contains("channel not in control"))
//        {
//            varValues.replace("channel not in control","B");
//            //varValues.replace("channel not in control","CNIC");
//        }


        QStringList splChrVal = varValues.split(",");
        QStringList bothCh;
        QStringList bothEng;
        QStringList otherCh;
        for(int idx=0;idx<splChrVal.count();idx++)
        {
            QString splVal = splChrVal.at(idx);
            QStringList vals = splVal.trimmed().split("=");
            if((vals.count() > 1) && (vals.at(1).contains("channel in control")))
            {
                sParam.replace(GENESTA_CHANNEL_VAR_IDENTIFIER1, "A");
                sParam.replace("_CHA.", QString("_%1CHA:CIC%1.").arg(GTA_CIC_IDENTIFIER));
                sParam.replace("EEC1_A", QString("%1EEC1_A:CIC%1").arg(GTA_CIC_IDENTIFIER));
				sParam.replace("EEC2_A", QString("%1EEC2_A:CIC%1").arg(GTA_CIC_IDENTIFIER));
                sStatementParam = sParam;
                bothCh.append(QString("%1=%2").arg(vals.at(0),vals.at(1)));
            }
            else if((vals.count() > 1) && (vals.at(1).contains("channel not in control")))
            {
                sParam.replace(GENESTA_CHANNEL_VAR_IDENTIFIER1, "A");
                sParam.replace("_CHA.", QString("_%1CHA:CNIC%1.").arg(GTA_CIC_IDENTIFIER));
                sParam.replace("EEC1_A", QString("%1EEC1_A:CNIC%1").arg(GTA_CIC_IDENTIFIER));
				sParam.replace("EEC2_A", QString("%1EEC2_A:CNIC%1").arg(GTA_CIC_IDENTIFIER));
                sStatementParam = sParam;
                bothCh.append(QString("%1=%2").arg(vals.at(0),vals.at(1)));

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
            newStatement.replace(QString(GENESTA_VAR2_IDENTIFIER),QString("@%1@").arg(GTA_VAR2_VARIABLE));
            ListOfParam.append(newStatement.append(QString ("(%1)").arg(sReplacedSt)));
        }


    }
    else
    {
        QMap<QString,QString> mapReplaceItems;
        mapReplaceItems.insert(QString(GENESTA_CHANNEL_VAR_IDENTIFIER1),QString("@%1@").arg(GTA_CHANNEL_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_CHANNEL_VAR_IDENTIFIER2),QString("@%1@").arg(GTA_CHANNEL_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_VAR1_IDENTIFIER),QString("@%1@").arg(GTA_VAR1_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_VAR2_IDENTIFIER),QString("@%1@").arg(GTA_VAR2_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_ENGINE_VAR_IDENTIFIER1),QString("@%1@").arg(GTA_ENGINE_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_ENGINE_VAR_IDENTIFIER2),QString("@%1@").arg(GTA_ENGINE_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_ENGINE_VAR_IDENTIFIER3),QString("@%1@").arg(GTA_ENGINE_VARIABLE));

        QString currentStatement = iStatement;

        QMapIterator<QString,QString>  it(mapReplaceItems);
        while (it.hasNext())
        {
            it.next();
            currentStatement.replace(it.key(),it.value());
        }
        ListOfParam.append(currentStatement);
    }

    for (int i=0;i<ListOfParam.size();i++)
    {
        SGenestaParamInfo obj = AINGTAGenestaParserItf::getParamInfoObjFromParamStr(ListOfParam.at(i));
        oList.append(obj);

    }
}
