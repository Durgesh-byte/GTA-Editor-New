#include "AINGTAGenestaActReleaseDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTAActionRelease.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

AINGTAGenestaActReleaseDeserializer::AINGTAGenestaActReleaseDeserializer(const QString& iStmt,const QString& isEquipment,const QString& isAircraft):AINGTAGenestaParserItf(iStmt)
{

    _sAircraft=isAircraft;
    _sEquipment=isEquipment;

}

bool AINGTAGenestaActReleaseDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC = false;
    //AINGTACommandBase*  pCmd = NULL;
    if (!_statement.isEmpty())
    {
        
        QString sCheckValParamStatement = _statement;
        sCheckValParamStatement.remove(GENESTA_CMD_RELEASE);
        sCheckValParamStatement.trimmed();
        QStringList lstofStatements;
        getPreprocessedStatements(sCheckValParamStatement,lstofStatements);
        
        if (lstofStatements.isEmpty())
        {_lastError="could not process statement";return false;}
        
        AINGTAActionRelease* pRelease = NULL;
        QStringList parameterList;
        foreach(QString statement,lstofStatements)
        {
            if (NULL==pRelease)
                pRelease = new AINGTAActionRelease(ACT_REL,COM_REL_PAR);
            if (!statement.isEmpty())
            {
                replaceGenSymbols(statement);
                QStringList sParamInfoList = statement.split(" ",QString::SkipEmptyParts);
                QString sParameter = sParamInfoList.at(0);
                if (AINGTAGenestaParserItf::isTriplet(sParameter))
                {/*sParameter = sParameter.toUpper();*/sParameter.replace("-","_");}
                parameterList.append(sParameter);


            }
        }

        if (pRelease)
        {
            pRelease->setParameterList(parameterList);
            if (parameterList.isEmpty())
            {
                _lastError = "could not fetch parameters";
                rC = false;
            }
            else
                rC = true;

            oLstCmds.append(pRelease);
            
        }
        

    }
    else
        _lastError="Nothing to evaluate with empty statement";


    return rC;
}


void AINGTAGenestaActReleaseDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
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
