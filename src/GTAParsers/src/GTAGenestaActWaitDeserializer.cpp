#include "AINGTAGenestaActWaitDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTAActionIF.h"
#include "AINGTAActionElse.h"
#include "AINGTAActionWait.h"
#include "AINGTAActionIfEnd.h"
#include "AINGTACheckValue.h"
#include "AINGTAGenestaFileParser.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

AINGTAGenestaActWaitDeserializer::AINGTAGenestaActWaitDeserializer(const QString& iStmt,const QString& isEquipment,const QString& isAircraft,const QString& iTimeOut):AINGTAGenestaParserItf(iStmt)
{
    _sAircraft=isAircraft ;
    _sEquipment=isEquipment;
    _externalTimeOut = iTimeOut;

}

bool AINGTAGenestaActWaitDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC = false;
    AINGTACommandBase*  pCmd = NULL;
    if (!_statement.isEmpty())
    {
        if (_statement.indexOf(QString("%1 %2").arg(GENESTA_CMD_WAIT,COM_WAIT_FOR))==0)
        {
            //wait for 10 S::add comment - comment for wait for::::
            QString sTimeInfo = _statement;
            sTimeInfo.remove(QString("%1 %2").arg(GENESTA_CMD_WAIT,COM_WAIT_FOR));

            QMap<QString,QString> mapOfSymbols = AINGTAGenestaFileParser::getGenstaSymbolMap();
            foreach(QString symbol, mapOfSymbols.keys())
            {
                sTimeInfo.replace(symbol,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),mapOfSymbols.value(symbol)));
            }

            QStringList timeInfoLst = sTimeInfo.split(" ",QString::SkipEmptyParts);
            
            if (!timeInfoLst.isEmpty() && !timeInfoLst.at(0).isEmpty())
            {
                AINGTAActionWait * opActionCmd =  new AINGTAActionWait(AINGTAActionWait::FOR);
                opActionCmd->setCounter(timeInfoLst.at(0));
                opActionCmd->setComplement(COM_WAIT_FOR);
                opActionCmd->setAction(ACT_WAIT);
                oLstCmds.append(opActionCmd);
                pCmd = opActionCmd;
                rC = true;
            }
            else
                _lastError = QString("could not evaluate %1 %2").arg(GENESTA_CMD_WAIT,COM_WAIT_FOR);

        }
        else if (_statement.indexOf(QString("%1 %2").arg(GENESTA_CMD_WAIT,COM_WAIT_UNTIL))==0)
        {
            
            //wait until GEMI_2.ENG_2_ICU_A_ANA_COIL_INSTRUM.EIU2_28VDC_ICU1A != 36
            QString sParameterInfo = _statement;
            sParameterInfo.remove(QString("%1 %2").arg(GENESTA_CMD_WAIT,COM_WAIT_UNTIL));
            

            bool bFADEC;

            if((_sAircraft == "SA" && ((_sEquipment=="CFM")||(_sEquipment=="PW"))) || (_sAircraft == "LR" &&(_sEquipment=="RR") ))
                bFADEC = true;
            else
                bFADEC = false;


            if ( (_statement.contains("channel in control")||_statement.contains("channel not in control")) && bFADEC)
            {
                QString statement = _statement;
                QStringList sComparators;
                sComparators<<" != "<<" >= "<<" <= "<<" < "<<" > "<<" = ";
                QString parameterInfo;
                QString valInfo;

                QString sComparator;
                bool bComparatorFound =false;
                foreach(QString sComparatorChk, sComparators)
                {
                    
                    if (sParameterInfo.contains(sComparatorChk))
                    {
                        bComparatorFound=true;
                        QStringList paramValInfo = statement.split(sComparatorChk);
                        parameterInfo = paramValInfo.at(0);
                        valInfo = paramValInfo.at(1);
                        sComparator=sComparatorChk.remove(" ");
                        break;
                    }
                }

                if(!bComparatorFound)
                {
                    _lastError = QString("could not evaluate %1 %2").arg(GENESTA_CMD_WAIT,COM_WAIT_UNTIL);
                    return false;
                }

                QStringList lstOfParamInfo,lstofValInfo;
                getPreprocessedStatements(parameterInfo,lstOfParamInfo);
                getPreprocessedStatements(valInfo,lstofValInfo);
                rC = getLstOfCommandsForChannelInCTRL(oLstCmds,lstOfParamInfo,lstofValInfo,sComparator);

            }
            else 

            {

                QStringList lstofParameters;
                getPreprocessedStatements(sParameterInfo,lstofParameters);
                foreach(QString sParameterNameInfo,lstofParameters)
                {
                    replaceGenSymbols(sParameterNameInfo);
                    QStringList sParamNameInfoLst = sParameterNameInfo.split(" ",QString::SkipEmptyParts);
                    QString parameterName = sParamNameInfoLst.at(0);
                    
                    QStringList sComparators;
                    sComparators<<"!="<<">="<<"<="<<"<"<<">"<<"=";

                    bool bComparatorFound =false;
                    foreach(QString sComparator, sComparators)
                    {
                        if (sParameterInfo.contains(sComparator))
                        {
                            bComparatorFound=true;

                            QStringList paramStatementLst;
                            if((sParameterInfo.contains("channel A") || sParameterInfo.contains("channel B") || sParameterInfo.contains("both channels") || sParameterInfo.contains("channel in control") || sParameterInfo.contains("channel not in control")) && (sComparator == "="))
                            {
                                QStringList tempParamStmtLst = sParameterInfo.split(QString("%1").arg(sComparator));
                                paramStatementLst.append(tempParamStmtLst.at(0) + tempParamStmtLst.at(1));
                                paramStatementLst.append(tempParamStmtLst.at(2));
                            }
                            else if((sParameterInfo.contains("Unit=")) && (sComparator == "="))
                            {
                                QStringList tempParamStmtLst = sParameterInfo.split(QString("%1").arg(sComparator));
                                paramStatementLst.append(QString("%1=%2").arg(tempParamStmtLst.at(0),tempParamStmtLst.at(1)));
                                paramStatementLst.append(tempParamStmtLst.at(2));
                            }
                            else
                                paramStatementLst = sParameterInfo.split(QString("%1").arg(sComparator));

                            if (paramStatementLst.size()== 2)
                            {
                                QString sParameterNameInfo = sParamNameInfoLst.at(0);
                                QString sParamValInfo = paramStatementLst.at(1);


                                replaceGenSymbols(parameterName);
                                

                                QStringList sParamValInfoLst = sParamValInfo.split(" ",QString::SkipEmptyParts);

                                if (!parameterName.isEmpty() && !sParamValInfoLst.isEmpty())
                                {

                                    QString sParamVal=sParamValInfoLst.at(0);
                                    replaceGenSymbols(sParamVal);

                                    QMapIterator<QString,QString>  it(_mGenstaSymbolMap);
                                    while (it.hasNext())
                                    {
                                        it.next();
                                        sParamVal.replace(it.key(),QString("%1%2%1").arg( QString(TAG_IDENTIFIER),it.value()));
                                    }

                                    AINGTAActionWait * opActionCmd =  new AINGTAActionWait(AINGTAActionWait::UNTIL);
                                    opActionCmd->setParameter(parameterName);
                                    opActionCmd->setCondition(sComparator);
                                    opActionCmd->setValue(sParamVal);
                                    opActionCmd->setComplement(COM_WAIT_UNTIL);
                                    opActionCmd->setAction(ACT_WAIT);
                                    oLstCmds.append(opActionCmd);
                                    pCmd = opActionCmd;
                                    rC=true;
                                }


                            }
                            
                            break;
                        }
                    }

                    if(pCmd==NULL)
                        _lastError = QString("could not evaluate %1 %2").arg(GENESTA_CMD_WAIT,COM_WAIT_UNTIL);


                }
            }


            // QStringList sParameterInfoList = sParameterInfo.split(" ",QString::SkipEmptyParts);


        }
        else
            _lastError="Wait statement not recognized";
    }
    else
        _lastError="Nothing to evaluate with empty statement";

    return rC;
}

void AINGTAGenestaActWaitDeserializer::getWaitUntilInfoFromParamInfo(QString paramValStmt,QString iParamInfoStmt,QString& oParamName, QString& oComparator, QString& oValue)
{

    replaceGenSymbols(iParamInfoStmt);
    QStringList sParamNameInfoLst = iParamInfoStmt.split(" ",QString::SkipEmptyParts);
    oParamName = sParamNameInfoLst.at(0);

    QStringList sComparators;
    sComparators<<"!="<<">="<<"<="<<"<"<<">"<<"=";

    bool bComparatorFound =false;
    foreach(QString sComparator, sComparators)
    {
        if (paramValStmt.contains(sComparator))
        {
            bComparatorFound = true;
            oComparator = sComparator;
            QStringList paramStatementLst = paramValStmt.split(QString(" %1 ").arg(sComparator));
            QString sParamVal1;
            if (paramStatementLst.size()==2)
            {
                QString sParameterNameInfo1 = paramStatementLst.at(0);
                QString sParamValInfo = paramStatementLst.at(1);


                QStringList sParamValInfoLst = sParamValInfo.split(" ",QString::SkipEmptyParts);

                if (!oParamName.isEmpty() && !sParamValInfoLst.isEmpty())
                {

                    oValue=sParamValInfoLst.at(0);

                }

            }
        }

        if (bComparatorFound)
            return;


    }

}
bool  AINGTAGenestaActWaitDeserializer::getLstOfCommandsForChannelInCTRL(QList<AINGTACommandBase*>& oLstCmds,QStringList& iLstofParam,QStringList& iLstofVal,QString sComparator)
{
    bool rC;


    QStringList channelSelectedLst;


    channelSelectedLst.append(getChannelSelectedForSignals(iLstofParam,iLstofVal));





    QString chsForEn1=channelSelectedLst.at(0);//channel for engine 1
    QString chsForEn2 = channelSelectedLst.at(0);
    if (channelSelectedLst.size()==2)
    {
        chsForEn2=channelSelectedLst.at(1); 
    }
    //channel for engine 2

    if (AINGTAGenestaParserItf::isTriplet(chsForEn1))
    {/*chsForEn1 = chsForEn1.toUpper() ;*/chsForEn1.replace("-","_");}
    if (AINGTAGenestaParserItf::isTriplet(chsForEn2))
    { /*chsForEn2 = chsForEn2.toUpper() ;*/chsForEn2.replace("-","_");}


    AINGTAActionIF* pIF1 = new AINGTAActionIF(ACT_CONDITION,COM_CONDITION_IF);
    pIF1->insertChildren(new AINGTAActionIfEnd,0);
    pIF1->setParameter(chsForEn1);
    pIF1->setValue("1");
    pIF1->setCondition("=");

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


    QString sFunctionalStatus= ACT_FSSSM_NOT_LOST;
    //if (!chsForEn1.isEmpty() && !chsForEn2.isEmpty())
    {

        for (int i=0;i<iLstofParam.size();i++)
        {
            QString str = iLstofParam.at(i);
            str.remove(QString("%1 %2").arg(ACT_WAIT,COM_WAIT_UNTIL));
            str = str.trimmed();
            
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
            str.remove(QString("%1 %2").arg(ACT_WAIT,COM_WAIT_UNTIL));
            str = str.trimmed();
            if (AINGTAGenestaParserItf::isTriplet(str))
            {
                /*str = str.toUpper();*/str.replace("-","_");;
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
            pCh111->setWaitUntil(true);
            pCh111->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
            pElse11 = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
            pCh112  = new AINGTACheckValue;
            pCh112->setWaitUntil(true);
            pCh112->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);

            pIF21   = new AINGTAActionIF(ACT_CONDITION,COM_CONDITION_IF); 
            pIF21->insertChildren(new AINGTAActionIfEnd,0);
            pIF21->setParameter(chsForEn2);
            pIF21->setValue("1");
            pIF21->setCondition("=");

            pCh211  = new AINGTACheckValue;
            pCh211->setWaitUntil(true);
            pCh211->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);
            pElse21 = new AINGTAActionElse(ACT_CONDITION,COM_CONDITION_ELSE);
            pCh212  = new AINGTACheckValue;
            pCh212->setWaitUntil(true);
            pCh212->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);



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



                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("AND");
                pCh111->insertParamenter(p_chA_en2,sComparator,v_chA_en2,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("AND");
                pCh112->insertParamenter(p_chB_en2,sComparator,v_chB_en2,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);

                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en2,sComparator,v_chA_en2,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("AND");
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chB_en2,sComparator,v_chB_en2,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("AND");
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

                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("AND");
                pCh111->insertParamenter(p_chA_en2,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("AND");
                pCh112->insertParamenter(p_chB_en2,sComparator,v_chB_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);

                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en2,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("AND");
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chB_en2,sComparator,v_chB_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("AND");
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


                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("AND");
                pCh111->insertParamenter(p_chA_en2,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("AND");
                pCh112->insertParamenter(p_chB_en2,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);

                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chB_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en2,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("AND");
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chB_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chB_en2,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("AND");
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



                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("AND");
                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en2,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("AND");
                pCh112->insertParamenter(p_chB_en1,sComparator,v_chB_en2,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);

                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en1,sComparator,v_chA_en2,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("AND");
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chB_en1,sComparator,v_chB_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chB_en1,sComparator,v_chB_en2,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("AND");
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



                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh111->insertBinaryOperator("AND");
                pCh111->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pIF11->insertChildren(pCh111,0);

                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh112->insertBinaryOperator("AND");
                pCh112->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);

                pElse11->insertChildren(pCh112,0);
                pIF11->insertChildren(pElse11,1);

                pIF1->insertChildren(pIF11,0);



                pElse2->insertChildren(pIF21,0);
                pCh211->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh211->insertBinaryOperator("AND");
                pIF21->insertChildren(pCh211,0);

                pIF21->insertChildren(pElse21,1);

                pCh212->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertParamenter(p_chA_en1,sComparator,v_chA_en1,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
                pCh212->insertBinaryOperator("AND");
                pElse21->insertChildren(pCh212,0);

                oLstCmds.append(pIF1);
                rC=true;
            }

        }

        if (iLstofParam.size()==2 && iLstofVal.size()==1)
        {
            pCh111  = new AINGTACheckValue;
            pCh111->setWaitUntil(true);
            pCh111->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);

            pCh211  = new AINGTACheckValue;
            pCh211->setWaitUntil(true);
            pCh211->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);



            QString p_chA =iLstofParam[0];replaceGenSymbols(p_chA);p_chA=p_chA.split(" ",QString::SkipEmptyParts).at(0);
            QString p_chB =iLstofParam[1];replaceGenSymbols(p_chB);p_chB=p_chB.split(" ",QString::SkipEmptyParts).at(0);

            QString v =iLstofVal[0];replaceGenSymbols(v);v=v.split(" ",QString::SkipEmptyParts).at(0);
            pCh111->insertParamenter(p_chA,sComparator,v,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
            pCh211->insertParamenter(p_chB,sComparator,v,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);


            pIF1->insertChildren(pCh111,0);
            pElse2->insertChildren(pCh211,0);

            oLstCmds.append(pIF1);
            rC=true;




        }
        else  if (iLstofVal.size()==2 && iLstofParam.size()==1)
        { 


            pCh111  = new AINGTACheckValue;
            pCh111->setWaitUntil(true);
            pCh111->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);

            pCh211  = new AINGTACheckValue;
            pCh211->setWaitUntil(true);
            pCh211->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);



            QString p =iLstofParam[0];replaceGenSymbols(p);p=p.split(" ",QString::SkipEmptyParts).at(0);



            QString v_chA =iLstofVal[0];replaceGenSymbols(v_chA);v_chA=v_chA.split(" ",QString::SkipEmptyParts).at(0);
            QString v_chB =iLstofVal[1];replaceGenSymbols(v_chB);v_chB=v_chB.split(" ",QString::SkipEmptyParts).at(0);

            pCh111->insertParamenter(p,sComparator,v_chA,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
            pCh211->insertParamenter(p,sComparator,v_chB,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);


            pIF1->insertChildren(pCh111,0);
            pElse2->insertChildren(pCh211,0);

            oLstCmds.append(pIF1);
            rC=true;
        }
        else  if (iLstofVal.size()==2 && iLstofParam.size()==2)
        { 


            pCh111  = new AINGTACheckValue;
            pCh111->setWaitUntil(true);
            pCh111->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);

            pCh211  = new AINGTACheckValue;
            pCh211->setWaitUntil(true);
            pCh211->setTimeOut(_externalTimeOut,ACT_TIMEOUT_UNIT_MSEC);



            QString p_chA =iLstofParam[0];replaceGenSymbols(p_chA);p_chA=p_chA.split(" ",QString::SkipEmptyParts).at(0);
            QString p_chB =iLstofParam[1];replaceGenSymbols(p_chB);p_chB=p_chB.split(" ",QString::SkipEmptyParts).at(0);


            QString v_chA =iLstofVal[0];replaceGenSymbols(v_chA);v_chA=v_chA.split(" ",QString::SkipEmptyParts).at(0);
            QString v_chB =iLstofVal[1];replaceGenSymbols(v_chB);v_chB=v_chB.split(" ",QString::SkipEmptyParts).at(0);

            pCh111->insertParamenter(p_chA,sComparator,v_chA,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);
            pCh211->insertParamenter(p_chB,sComparator,v_chB,sFunctionalStatus,0,ACT_PRECISION_UNIT_VALUE,false);


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
QStringList AINGTAGenestaActWaitDeserializer::getChannelSelectedForSignals(const QStringList& iSignalListLHS,const QStringList& iSignalListRHS)const
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

/*
void AINGTAGenestaActWaitDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
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
    //only applicable for 320neo PW, 320neo CFM, and, 330neo RR

//    bool bFADEC = _sAircraft=="SA"?true:false;
//    bFADEC = bFADEC && ((_sEquipment=="CFM")||(_sEquipment=="PW")||(_sEquipment=="RR"));



    bool bFADEC;

    if((_sAircraft == "SA" && ((_sEquipment=="CFM")||(_sEquipment=="PW"))) || (_sAircraft == "LR" &&(_sEquipment=="RR") ))
        bFADEC = true;
    else
        bFADEC = false;
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
          //  varValues.replace("channel in control","CIC");
        }
        if (varValues.contains("channel not in control"))
        {
            varValues.replace("channel not in control","B");
          //  varValues.replace("channel not in control","CNIC");
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
*/

void AINGTAGenestaActWaitDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
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
    //only applicable for 320neo PW, 320neo CFM, and, 330neo RR

    bool bFADEC;

    if((_sAircraft == "SA" && ((_sEquipment == "CFM")||(_sEquipment == "PW"))) || (_sAircraft == "LR" &&(_sEquipment == "RR") ))
        bFADEC = true;
    else
        bFADEC = false;


    //if(bFADEC)
    // {
    //     sStatement.replace("channel in control","both channels");
    //     sStatement.replace("channel not in control","both channelsR");
    // }



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

