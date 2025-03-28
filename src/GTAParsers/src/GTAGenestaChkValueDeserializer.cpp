#include "AINGTAGenestaChkValueDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTACheckValue.h"
#include "AINGTAActionIF.h"
#include "AINGTAActionElse.h"
#include "AINGTAActionIfEnd.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

AINGTAGenestaChkValueDeserializer::AINGTAGenestaChkValueDeserializer(const QString& iStmt, const QString& isEquipment, const QString& isAircraft,const QString &iPrecision, const QString& iTimeOut):AINGTAGenestaParserItf(iStmt)
{
    _sEquipment = isEquipment;
    _sAircraft =isAircraft;
    _Precision = iPrecision;
    _sExternalTimeOut="3000";
    bool isNum;
    iTimeOut.toDouble(&isNum);
    if(isNum)
        _sExternalTimeOut = iTimeOut;
}

bool AINGTAGenestaChkValueDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC = false;
    AINGTACommandBase*  pCmd = NULL;
    if (!_statement.isEmpty())
    {
        /** Example:
           * 1.  check Value GEMI_1.ENG_1_ICU_A_ANA_COIL_INSTRUM.EIU1_28VDC_ICU1A != 12 OR IRS_1.ADIRU1_A429_IR_DATA_BUS2.ADIRU_163_Request_Bus_Designator_1 (163/13) >= FMGC_OUT_2.FMGC2_A429_FMGCOWNC2.FLIGHTLEGENCODED_0 (260) (Unit=No Unit)::3000:0:continue:
           * 2.  check Value SW_EIU2.ENG_MASTER_2_MASTER_LEVER_OFF.ENG_MASTER_2_MASTER_LEVER_OFF < 12::3000:0:continue:
             3.  check Value THROTTLE_CARD.CTRL_UNIT_THROTTLE_ENG1_POS_CH×.POS_CARD_TRA_EEC1_CHA (×=both channels) != THROTTLE_CARD.CTRL_UNIT_THROTTLE_ENG2_POS_CH×.POS_CARD_TRA_EEC2_CHA (×=both channels) 
                 OR 
                 THROTTLE_CARD.CTRL_UNIT_THROTTLE_ENG1_POS_CH×.POS_CARD_TRA_EEC1_CHB (×=both channels) > THROTTLE_CARD.CTRL_UNIT_THROTTLE_ENG2_POS_CH×.POS_CARD_TRA_EEC2_CHB (×=both channels)::3000:0:continue:
                 
           */

        QString sCheckValParamStatement = _statement;
        sCheckValParamStatement.remove(GENESTA_CMD_CHECK_VAL);
        QStringList sParameterValInfoLst = sCheckValParamStatement.split(" OR ");
        int dParamValInfosize = sParameterValInfoLst.size();


        QStringList sComparators;
        sComparators<<"!="<<">="<<"<="<<"<"<<">"<<"=";

        AINGTACheckValue* pChkValue = new AINGTACheckValue;
        AINGTACheckValue* pChkValue2 = new AINGTACheckValue;
        
        QString sFunctionalStatus = ACT_FSSSM_NOT_LOST;
        int isSecondParam = 0;
        foreach(QString sParameterValInfo, sParameterValInfoLst)
        {
            if (!sParameterValInfo.isEmpty())
            {

                if(sParameterValInfo.contains("LOST"))
                {
                    sParameterValInfo.replace(QString("LOST"),ACT_FSSSM_LOST);

                }
                foreach(QString sComparator, sComparators)
                {
                    if (sParameterValInfo.contains(QString(" %1 ").arg(sComparator)))
                    {


                        QStringList paramValInfoLst = sParameterValInfo.split(QString(" %1 ").arg(sComparator),QString::SkipEmptyParts);
                        if(paramValInfoLst.size() == 2)
                        {
                            QString sParamNameInfo = paramValInfoLst.at(0);
                            sParamNameInfo = sParamNameInfo.trimmed();
                            if (sParamNameInfo.isEmpty())
                            {
                                _lastError=QString("Parameter does not exist in call command");
                                return rC;
                            }
                            QStringList lstofParams;
                            getPreprocessedStatements(sParamNameInfo,lstofParams);



                            QStringList sParamNameInfoLst = sParamNameInfo.split(" ",QString::SkipEmptyParts);
                            QString sParameterName = sParamNameInfoLst.at(0);

                            QString sParamValInfo = paramValInfoLst.at(1);
                            QStringList lstofValInfo;
                            getPreprocessedStatements(sParamValInfo,lstofValInfo);

                            /*
                                P11 V11 AND
                                P11 V12 AND
                                P12 V11 AND
                                P12 V12 
                                        OR
                                P21 V21 AND
                                P21 V22 AND
                                P22 V21 AND
                                P22 V22 
                           */

                            // int compInserCnt=-1;
                            //QString sFunctionalStatus= sParamValInfo;
                            //                            bool bFADEC = _sAircraft=="SA"?true:false;
                            //                            bFADEC = bFADEC && ((_sEquipment=="CFM")||(_sEquipment=="PW")||(_sEquipment=="RR"));

                            bool bFADEC;

                            if((_sAircraft == "SA" && ((_sEquipment=="CFM")||(_sEquipment=="PW"))) || (_sAircraft == "LR" &&(_sEquipment=="RR") ))
                                bFADEC = true;
                            else
                                bFADEC = false;
//                            if ( (_statement.contains("channel in control")||_statement.contains("channel not in control")) && bFADEC)
//                            {

//                                //  rC = getLstOfCommandsForChannelInCTRL(oLstCmds,lstofParams,lstofValInfo,sComparator);
//                                pChkValue->insertParamenter(sParamName,sComparator,sparamVal,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                                
//                            }
                     //       else
                     //       {

                                if(isSecondParam)
                                    pChkValue->insertBinaryOperator("OR");


                                for (int i=0;i<lstofParams.size();i++)
                                {
                                    for (int j=0;j<lstofValInfo.size();j++)
                                    {
                                        // compInserCnt++;
                                        QString sParamName = lstofParams.at(i);
                                        QString sparamVal = lstofValInfo.at(j);

                                        replaceGenSymbols(sParamName);
                                        replaceGenSymbols(sparamVal);


                                        sParamName = sParamName.split(" ",QString::SkipEmptyParts).at(0);
                                        sparamVal = sparamVal.split(" ",QString::SkipEmptyParts).at(0);
                                        
                                        if (AINGTAGenestaParserItf::isTriplet(sParamName))
                                        {/*sParamName=sParamName.toUpper();*/sParamName.replace("-","_");}
                                        if (AINGTAGenestaParserItf::isTriplet(sparamVal))
                                        {/*sparamVal=sparamVal.toUpper();*/sparamVal.replace("-","_");}

                                        if(lstofParams.size() == 2 && _statement.contains("both channels"))
                                        {
                                            if(i ==0)
                                                pChkValue->insertParamenter(sParamName,sComparator,sparamVal,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                                            else
                                            {
                                                pChkValue2->insertParamenter(sParamName,sComparator,sparamVal,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                                            }
                                        }
                                        else
                                            pChkValue->insertParamenter(sParamName,sComparator,sparamVal,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);

                                        //   if (compInserCnt)
                                        //	    pChkValue->insertBinaryOperator("AND");


                                    }
                                }
                                


                                pCmd=pChkValue;

                                if (pChkValue)
                                {
                                    //default value

                                    pChkValue->setConfCheckTime("0",ACT_TIMEOUT_UNIT_SEC);
                                    pChkValue->setWaitUntil(true);
                                    oLstCmds.append(pCmd);
                                    if(pChkValue2 != NULL && _statement.contains("both channels"))
                                    {
                                        AINGTACommandBase*  pCmd2 = NULL;
                                        pCmd2 = pChkValue2;
                                        pChkValue2->setConfCheckTime("0",ACT_TIMEOUT_UNIT_SEC);
                                        pChkValue2->setWaitUntil(true);
                                        oLstCmds.append(pCmd2);
                                    }
                                    rC = true;
                                }

                                else
                                    _lastError=QString("%1: Could not evaluate parameter and its value").arg(GENESTA_CMD_CHECK_VAL);
                         //   }

                            

                        }
                        else
                            _lastError=QString("%1: Could not evaluate parameter and its value").arg(GENESTA_CMD_CHECK_VAL);

                        break;

                    }
                }
            }
            isSecondParam ++;
        }
        
        
        if(dParamValInfosize<0 || dParamValInfosize>2)
        {
            _lastError=QString("%1: Could not evaluate parameter,check condition and value").arg(GENESTA_CMD_CHECK_VAL);
            delete pChkValue;pChkValue=NULL;pCmd=NULL;
        }
    }
    else
        _lastError="Nothing to evaluate with empty statement";


    return rC;
}
bool  AINGTAGenestaChkValueDeserializer::getLstOfCommandsForChannelInCTRL(QList<AINGTACommandBase*>& oLstCmds,QStringList& iLstofParam,QStringList& iLstofVal,QString sComparator)
{
    bool rC;


    QStringList channelSelectedLst;


    channelSelectedLst.append(getChannelSelectedForSignals(iLstofParam,iLstofVal));





    QString chsForEn1=channelSelectedLst.at(0);//channel for engine 1

    if (AINGTAGenestaParserItf::isTriplet(chsForEn1))
    {/*chsForEn1=chsForEn1.toUpper();*/chsForEn1.replace("-","_");}
    QString chsForEn2 = channelSelectedLst.at(0);
    if (channelSelectedLst.size()==2)
    {
        chsForEn2=channelSelectedLst.at(1); 
    }

    if (AINGTAGenestaParserItf::isTriplet(chsForEn2))
    {
        /*chsForEn1=chsForEn1.toUpper();*/chsForEn1.replace("-","_");;
    }

    //channel for engine 2

    AINGTAActionIF* pIF1 = new AINGTAActionIF(ACT_CONDITION,COM_CONDITION_IF);
    pIF1->insertChildren(new AINGTAActionIfEnd,0);
    pIF1->setParameter(chsForEn1);
    pIF1->setValue("1");
    pIF1->setCondition("=");
    pIF1->setIsManuallyAdded(true);


    AINGTAActionIF* pIF11 = NULL;
    AINGTACheckValue* pCh111 = NULL;
    AINGTAActionElse* pElse11 = NULL;
    AINGTACheckValue* pCh112 = NULL;
    AINGTAActionElse* pElse2 = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
    pIF1->insertChildren(pElse2,0);

    AINGTAActionIF* pIF21 = NULL;
    AINGTACheckValue* pCh211 = NULL;
    AINGTAActionElse* pElse21 = NULL;
    AINGTACheckValue* pCh212 = NULL;


    QString sFunctionalStatus;
    if((iLstofVal.count() > 0) && (iLstofVal.at(0) == ACT_FSSSM_LOST))
    {
        sFunctionalStatus = ACT_FSSSM_LOST;
    }
    else
        sFunctionalStatus= ACT_FSSSM_NOT_LOST;

    //if (!chsForEn1.isEmpty() && !chsForEn2.isEmpty())
    {

        for (int i=0;i<iLstofParam.size();i++)
        {
            QString str = iLstofParam.at(i);
            if (AINGTAGenestaParserItf::isTriplet(str))
            {
                /*str = str.toUpper();*/str.replace("-","_");
                iLstofParam.takeAt(i);
                iLstofParam.insert(i,str);
            }
        }


        for (int i=0;i<iLstofVal.size();i++)
        {
            QString str = iLstofVal.at(i);
            if (AINGTAGenestaParserItf::isTriplet(str))
            {
                /*str = str.toUpper();*/
                str.replace("-","_");
                iLstofVal.takeAt(i);
                iLstofVal.insert(i,str);
            }
        }


        if(iLstofParam.size()==4  )
        {
            pIF11   = new AINGTAActionIF(ACT_CONDITION,COM_CONDITION_IF);
            pIF11->setParameter(chsForEn2);
            pIF11->setValue("1");
            pIF11->setCondition("=");
            pIF11->insertChildren(new AINGTAActionIfEnd,0);

            pCh111  = new AINGTACheckValue;
            pElse11 = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
            pCh112  = new AINGTACheckValue;

            pIF21   = new AINGTAActionIF(ACT_CONDITION,COM_CONDITION_IF); 
            pIF21->insertChildren(new AINGTAActionIfEnd,0);
            pIF21->setParameter(chsForEn2);
            pIF21->setValue("1");
            pIF21->setCondition("=");

            pCh211  = new AINGTACheckValue;
            pElse21 = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
            pCh212  = new AINGTACheckValue;




            if(iLstofVal.size()==4)
            {
                QString p_chA_en1 =iLstofParam[0];replaceGenSymbols(p_chA_en1);p_chA_en1=p_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chA_en2 =iLstofParam[1];replaceGenSymbols(p_chA_en2);p_chA_en2=p_chA_en2.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chB_en1 =iLstofParam[2];replaceGenSymbols(p_chB_en1);p_chB_en1=p_chB_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chB_en2 =iLstofParam[3];replaceGenSymbols(p_chB_en2);p_chB_en2=p_chB_en2.split(" ",QString::SkipEmptyParts).at(0);

                QString v_chA_en1 =iLstofVal[0];replaceGenSymbols(v_chA_en1);v_chA_en1=v_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chA_en2 =iLstofVal[1];replaceGenSymbols(v_chA_en2);v_chA_en2=v_chA_en2.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chB_en1 =iLstofVal[2];replaceGenSymbols(v_chB_en1);v_chB_en1=v_chB_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chB_en2 =iLstofVal[3];replaceGenSymbols(v_chB_en2);v_chB_en2=v_chB_en2.split(" ",QString::SkipEmptyParts).at(0);



                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("OR");
                pCh111->insertParamenter(p_chA_en2,sComparator,v_chA_en2,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh111->setWaitUntil(true);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("OR");
                pCh112->insertParamenter(p_chB_en2,sComparator,v_chB_en2,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh112->setWaitUntil(true);
                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en2,sComparator,v_chA_en2,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("OR");
                pCh211->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh211->setWaitUntil(true);
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chB_en2,sComparator,v_chB_en2,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("OR");
                pCh212->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh212->setWaitUntil(true);
                pElse21->insertChildren(pCh212,0);

                oLstCmds.append(pIF1);
                rC=true;
            }
            else if(iLstofVal.size()==2)
            {
                QString p_chA_en1 =iLstofParam[0];replaceGenSymbols(p_chA_en1);p_chA_en1=p_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chA_en2 =iLstofParam[1];replaceGenSymbols(p_chA_en2);p_chA_en2=p_chA_en2.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chB_en1 =iLstofParam[2];replaceGenSymbols(p_chB_en1);p_chB_en1=p_chB_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chB_en2 =iLstofParam[3];replaceGenSymbols(p_chB_en2);p_chB_en2=p_chB_en2.split(" ",QString::SkipEmptyParts).at(0);

                QString v_chA_en1 =iLstofVal[0];replaceGenSymbols(v_chA_en1);v_chA_en1=v_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chB_en1 =iLstofVal[1];replaceGenSymbols(v_chB_en1);v_chB_en1=v_chB_en1.split(" ",QString::SkipEmptyParts).at(0);
                
                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("OR");
                pCh111->insertParamenter(p_chA_en2,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->setWaitUntil(true);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("OR");
                pCh112->insertParamenter(p_chB_en2,sComparator,v_chB_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh112->setWaitUntil(true);
                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en2,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("OR");
                pCh211->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh211->setWaitUntil(true);
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chB_en2,sComparator,v_chB_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("OR");
                pCh212->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh212->setWaitUntil(true);
                pElse21->insertChildren(pCh212,0);

                oLstCmds.append(pIF1);
                rC=true;
            }
            else if(iLstofVal.size()==1)
            {
                QString p_chA_en1 =iLstofParam[0];replaceGenSymbols(p_chA_en1);p_chA_en1=p_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chA_en2 =iLstofParam[1];replaceGenSymbols(p_chA_en2);p_chA_en2=p_chA_en2.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chB_en1 =iLstofParam[2];replaceGenSymbols(p_chB_en1);p_chB_en1=p_chB_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chB_en2 =iLstofParam[3];replaceGenSymbols(p_chB_en2);p_chB_en2=p_chB_en2.split(" ",QString::SkipEmptyParts).at(0);

                QString v_chA_en1 =iLstofVal[0];replaceGenSymbols(v_chA_en1);v_chA_en1=v_chA_en1.split(" ",QString::SkipEmptyParts).at(0);

                
                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("OR");
                pCh111->insertParamenter(p_chA_en2,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->setWaitUntil(true);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("OR");
                pCh112->insertParamenter(p_chB_en2,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh112->setWaitUntil(true);
                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chB_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en2,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("OR");
                pCh211->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh211->setWaitUntil(true);
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chB_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chB_en2,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("OR");
                pCh212->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh212->setWaitUntil(true);
                pElse21->insertChildren(pCh212,0);

                oLstCmds.append(pIF1);
                rC=true;
            }

            
        }
        else if(iLstofVal.size()==4  )
        {
            pIF11   = new AINGTAActionIF(ACT_CONDITION,COM_CONDITION_IF);
            pIF11->setParameter(chsForEn2);
            pIF11->setValue("1");
            pIF11->insertChildren(new AINGTAActionIfEnd,0);
            pIF21->setCondition("=");

            pCh111  = new AINGTACheckValue;
            pElse11 = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
            pCh112  = new AINGTACheckValue;

            pIF21   = new AINGTAActionIF(ACT_CONDITION,COM_CONDITION_IF); 
            pIF21->insertChildren(new AINGTAActionIfEnd,0);
            pIF21->setParameter(chsForEn2);
            pIF21->setValue("1");
            pIF21->setCondition("=");

            pCh211  = new AINGTACheckValue;
            pElse21 = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
            pCh212  = new AINGTACheckValue;
            


            if(iLstofParam.size()==2)
            {
                QString p_chA_en1 =iLstofParam[0];replaceGenSymbols(p_chA_en1);p_chA_en1=p_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString p_chB_en1 =iLstofParam[1];replaceGenSymbols(p_chB_en1);p_chB_en1=p_chB_en1.split(" ",QString::SkipEmptyParts).at(0);

                QString v_chA_en1 =iLstofVal[0];replaceGenSymbols(v_chA_en1);v_chA_en1=v_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chA_en2 =iLstofVal[1];replaceGenSymbols(v_chA_en2);v_chA_en2=v_chA_en2.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chB_en1 =iLstofVal[2];replaceGenSymbols(v_chB_en1);v_chB_en1=v_chB_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chB_en2 =iLstofVal[3];replaceGenSymbols(v_chB_en2);v_chB_en2=v_chB_en2.split(" ",QString::SkipEmptyParts).at(0);



                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("OR");
                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en2,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh111->setWaitUntil(true);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("OR");
                pCh112->insertParamenter(p_chB_en1,sComparator,v_chB_en2,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh112->setWaitUntil(true);
                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en1,sComparator,v_chA_en2,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("OR");
                pCh211->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh211->setWaitUntil(true);
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chB_en1,sComparator,v_chB_en2,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("OR");
                pCh212->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh212->setWaitUntil(true);
                pElse21->insertChildren(pCh212,0);

                oLstCmds.append(pIF1);
                rC=true;
            }
            else if(iLstofParam.size()==1)
            {
                QString p_chA_en1 =iLstofParam[0];replaceGenSymbols(p_chA_en1);p_chA_en1=p_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                
                QString v_chA_en1 =iLstofVal[0];replaceGenSymbols(v_chA_en1);v_chA_en1=v_chA_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chA_en2 =iLstofVal[1];replaceGenSymbols(v_chA_en2);v_chA_en2=v_chA_en2.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chB_en1 =iLstofVal[2];replaceGenSymbols(v_chB_en1);v_chB_en1=v_chB_en1.split(" ",QString::SkipEmptyParts).at(0);
                QString v_chB_en2 =iLstofVal[3];replaceGenSymbols(v_chB_en2);v_chB_en2=v_chB_en2.split(" ",QString::SkipEmptyParts).at(0);



                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("OR");
                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh111->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh111->setWaitUntil(true);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("OR");
                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh112->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh112->setWaitUntil(true);
                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("OR");
                pCh211->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh211->setWaitUntil(true);
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("OR");
                pCh212->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
                pCh212->setWaitUntil(true);
                pElse21->insertChildren(pCh212,0);

                oLstCmds.append(pIF1);
                rC=true;
            }

        }
        
        if (iLstofParam.size()==2 && iLstofVal.size()==1)
        {
            pCh111  = new AINGTACheckValue;

            pCh211  = new AINGTACheckValue;



            QString p_chA =iLstofParam[0];replaceGenSymbols(p_chA);p_chA=p_chA.split(" ",QString::SkipEmptyParts).at(0);
            QString p_chB =iLstofParam[1];replaceGenSymbols(p_chB);p_chB=p_chB.split(" ",QString::SkipEmptyParts).at(0);

            QString v =iLstofVal[0];replaceGenSymbols(v);v=v.split(" ",QString::SkipEmptyParts).at(0);
            pCh111->insertParamenter(p_chA,sComparator,v,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
            pCh211->insertParamenter(p_chB,sComparator,v,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
            pCh111->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
            pCh111->setWaitUntil(true);
            pCh211->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
            pCh211->setWaitUntil(true);
            pIF1->insertChildren(pCh111,0);
            pElse2->insertChildren(pCh211,0);

            oLstCmds.append(pIF1);
            rC=true;




        }
        else  if (iLstofVal.size()==2 && iLstofParam.size()==1)
        {

            pCh111  = new AINGTACheckValue;

            pCh211  = new AINGTACheckValue;



            QString p =iLstofParam[0];replaceGenSymbols(p);p=p.split(" ",QString::SkipEmptyParts).at(0);


            QString v_chA =iLstofVal[0];replaceGenSymbols(v_chA);v_chA=v_chA.split(" ",QString::SkipEmptyParts).at(0);
            QString v_chB =iLstofVal[1];replaceGenSymbols(v_chB);v_chB=v_chB.split(" ",QString::SkipEmptyParts).at(0);

            pCh111->insertParamenter(p,sComparator,v_chA,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
            pCh211->insertParamenter(p,sComparator,v_chB,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
            pCh111->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
            pCh111->setWaitUntil(true);
            pCh211->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
            pCh211->setWaitUntil(true);
            pIF1->insertChildren(pCh111,0);
            pElse2->insertChildren(pCh211,0);

            oLstCmds.append(pIF1);
            rC=true;
        }
        else  if (iLstofVal.size()==2 && iLstofParam.size()==2)
        {

            pCh111  = new AINGTACheckValue;

            pCh211  = new AINGTACheckValue;



            QString p_chA =iLstofParam[0];replaceGenSymbols(p_chA);p_chA=p_chA.split(" ",QString::SkipEmptyParts).at(0);
            QString p_chB =iLstofParam[1];replaceGenSymbols(p_chB);p_chB=p_chB.split(" ",QString::SkipEmptyParts).at(0);



            QString v_chA =iLstofVal[0];replaceGenSymbols(v_chA);v_chA=v_chA.split(" ",QString::SkipEmptyParts).at(0);
            QString v_chB =iLstofVal[1];replaceGenSymbols(v_chB);v_chB=v_chB.split(" ",QString::SkipEmptyParts).at(0);

            pCh111->insertParamenter(p_chA,sComparator,v_chA,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
            pCh211->insertParamenter(p_chB,sComparator,v_chB,sFunctionalStatus,_Precision.toDouble(),ACT_PRECISION_UNIT_VALUE,false);
            pCh111->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
            pCh111->setWaitUntil(true);
            pCh211->setTimeOut(_sExternalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
            pCh211->setWaitUntil(true);
            pIF1->insertChildren(pCh111,0);
            pElse2->insertChildren(pCh211,0);

            oLstCmds.append(pIF1);
            rC=true;
        }
    }




    if (!rC)
    {
        _lastError = "Invalid channel in control condition encountered";
    }

    return rC;
}


QStringList AINGTAGenestaChkValueDeserializer::getChannelSelectedForSignals(const QStringList& iSignalListLHS,const QStringList& iSignalListRHS)const
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


void AINGTAGenestaChkValueDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
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
    //only applicable for 320neo PW, 320neo CFM, and, 330neo RR

    //    bool bFADEC = _sAircraft=="SA"?true:false;
    //    bFADEC = bFADEC && ((_sEquipment=="CFM")||(_sEquipment=="PW")||(_sEquipment=="RR"));
    bool bFADEC;
    if((_sAircraft == "SA" && ((_sEquipment=="CFM")||(_sEquipment=="PW"))) || (_sAircraft == "LR" &&(_sEquipment=="RR") ))
        bFADEC = true;
    else
        bFADEC = false;

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


/*void AINGTAGenestaChkValueDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
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
    //only applicable for 320neo PW, 320neo CFM, and, 330neo RR

    bool bFADEC = _sAircraft=="SA"?true:false;
    bFADEC = bFADEC && ((_sEquipment=="CFM")||(_sEquipment=="PW")||(_sEquipment=="RR"));
    if(bFADEC)
    {
      //  sStatement.replace("channel in control","both channels");
      //  sStatement.replace("channel not in control","both channelsR");
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
            //varValues.replace("channel in control","CIC");
        }
        if (varValues.contains("channel not in control"))
        {
            varValues.replace("channel not in control","B");
            //varValues.replace("channel not in control","CNIC");
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
}*/



/*
GENESTA IMPORT FOR CHECK VALUE
possible cases for channel in control:			

chs: channel selected based on parameter or value
    CASE BOTH ENGINES NOT PRESENT
        case 1.1	only parameter has channel in ctrl	
        value does not have any combination

            if chsA == 1	
                p_chA = v
            else	
                p_chB = v


        case 1.2 Value has channel in ctrl
        parameter does not 

            if chsA == 1
                p = v_chA
            else
                p =v_chB

        case 1.3 value has channel in ctrl
        parameter has channel in ctrl

            if chsA==1
                p_chA = v_chA
            else
                p_chB = v_chB


    CASE BOTH ENGINE
    Chs will be two , for engine 1 and engine 2

    case 2.1	only parameter has channel in ctrl	
        value does not have any combination

            if chsA_e1 == 1
                if chsA_e2 ==1
                    p_chA_en1 = v
                    p_chA_en2 = v
                else
                    p_chA_en1 =v
                    p_chB_en2 =v
            else	
                if chsA_e2 ==1
                    p_chB_en1 = v
                    p_chA_en2 = v
                else
                    p_chB_en1 =v
                    p_chB_en2 =v


    case 2.2 Value has channel in ctrl
    parameter does not 

        if chsA_e1 == 1
            if chsA_e2 ==1
                p = v_chA_en1
                p = v_chA_en2
            else
                p =v_chA_en1
                p =v_chB_en2
        else	
            if chsA_e2 ==1
                p = v_chB_en1
                p = v_chA_en2
            else
                p =v_chB_en1
                p =v_chB_en2


    case 2.3 value has channel in ctrl
    parameter has channel in ctrl

        if chsA_e1 == 1
            if chsA_e2 ==1
                p_chA_en1 = v_chA_en1
                p_chA_en2 = v_chA_en2
            else
                p_chA_en1 =v_chA_en1
                p_chB_en2 =v_chB_en2
        else	
            if chsA_e2 ==1
                p_chB_en1 = v_chB_en1
                p_chA_en2 = v_chA_en2
            else
                p_chB_en1 =v_chB_en1
                p_chB_en2 =v_chB_en2

*/
