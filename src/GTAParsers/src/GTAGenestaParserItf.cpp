#include "AINGTAGenestaParserItf.h"
#include "AINGTAUtil.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

QMap<QString,QString> AINGTAGenestaParserItf::_mGenstaSymbolMap;
AINGTAGenestaParserItf::AINGTAGenestaParserItf(const QString& iStmt):_statement(iStmt)
{    
}
AINGTAGenestaParserItf::~AINGTAGenestaParserItf()
{
}
void AINGTAGenestaParserItf::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
{

    //call object - PMA_Ø_×_¤_HW_FLT (Ø=both engines , ×=channel A, ¤=1)
    QMap<QString,QString> MapOfVarTags;
    QString sStatement = iStatement;
    QRegExp rgExp;

    // pattern should be either "(z" or "( z" or "(  z" so on. where z = Ø × ¤ or +
    QString allSymbols, sPattern;
    foreach(QString sSymbol,_mGenstaSymbolMap.keys())
    {
        allSymbols.append(sSymbol);
    }
    if(allSymbols.isEmpty())
    {
        sPattern = QString("\\(\\s+[%1%2%3%4%5%6%7]|\\([%1%2%3%4%5%6%7]").arg(  GENESTA_ENGINE_VAR_IDENTIFIER1,
                                                                                        GENESTA_ENGINE_VAR_IDENTIFIER2,
                                                                                        GENESTA_ENGINE_VAR_IDENTIFIER3,
                                                                                        GENESTA_CHANNEL_VAR_IDENTIFIER1,
                                                                                        GENESTA_CHANNEL_VAR_IDENTIFIER2,
                                                                                        GENESTA_VAR1_IDENTIFIER,
                                                                                        GENESTA_VAR2_IDENTIFIER);
    }
    else
    {
        sPattern = QString("\\(\\s+[%1]|\\([%1]").arg(allSymbols );
    }
    rgExp.setPattern(sPattern);
    int dPos = rgExp.indexIn(sStatement,0);
    QString sStatementParam = sStatement.mid(0,dPos);
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
           // varValues.replace("channel in control","CIC");
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
            if((vals.count() > 1) && (vals.at(1).contains("both channels")))
            {
                bothCh.append(QString("%1=%2").arg(vals.at(0),"channel A"));     
                bothCh.append(QString("%1=%2").arg(vals.at(0),"channel B"));
            }
            else if((vals.count() > 1) && (vals.at(1).startsWith("channel")))
            {
                bothCh.append(QString("%1=%2").arg(vals.at(0),vals.at(1)));
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
        //QMap<QString,QString> mapReplaceItems;
        //mapReplaceItems.insert(QString(GENESTA_CHANNEL_VAR_IDENTIFIER1),QString("@%1@").arg(GTA_CHANNEL_VARIABLE));
        //mapReplaceItems.insert(QString(GENESTA_CHANNEL_VAR_IDENTIFIER2),QString("@%1@").arg(GTA_CHANNEL_VARIABLE));
        //mapReplaceItems.insert(QString(GENESTA_VAR1_IDENTIFIER),QString("@%1@").arg(GTA_VAR1_VARIABLE));
        //mapReplaceItems.insert(QString(GENESTA_VAR2_IDENTIFIER),QString("@%1@").arg(GTA_VAR2_VARIABLE));
        //mapReplaceItems.insert(QString(GENESTA_ENGINE_VAR_IDENTIFIER1),QString("@%1@").arg(GTA_ENGINE_VARIABLE));
        //mapReplaceItems.insert(QString(GENESTA_ENGINE_VAR_IDENTIFIER2),QString("@%1@").arg(GTA_ENGINE_VARIABLE));
        //mapReplaceItems.insert(QString(GENESTA_ENGINE_VAR_IDENTIFIER3),QString("@%1@").arg(GTA_ENGINE_VARIABLE));

        QString currentStatement = iStatement;

        QMapIterator<QString,QString>  it(_mGenstaSymbolMap);
        while (it.hasNext())
        {
            it.next();
            currentStatement.replace(it.key(),it.value());
        }
        oList.append(currentStatement);
    }
}

QString AINGTAGenestaParserItf::getGTAVarName(const QString& iVar,const int& iVarNo )
{
    QString varNo=QString();
    if (iVarNo!=-1)
        varNo = QString::number(iVarNo);


    if (iVar.contains(GENESTA_CHANNEL_VAR_IDENTIFIER1))
        return QString("%1%2").arg(GTA_CHANNEL_VARIABLE , varNo); 
    else if (iVar.contains(GENESTA_VAR1_IDENTIFIER))
        return QString("%1%2").arg(GTA_VAR1_VARIABLE , varNo); 
    else if (iVar.contains(GENESTA_ENGINE_VAR_IDENTIFIER1))
        return QString("%1%2").arg(GTA_ENGINE_VARIABLE , varNo); 
    else if (iVar.contains(GENESTA_ENGINE_VAR_IDENTIFIER2))
        return QString("%1%2").arg( GTA_ENGINE_VARIABLE, varNo);
    else
        return QString();


}
bool AINGTAGenestaParserItf:: replaceGenSymbols(QString& ioStatement)
{
    bool rC = true;
    QRegExp rgExp;
    QString sPattern =  QString("\\(\\s+[%1%2%3%4%5%6%7]|\\([%1%2%3%4%5%6%7]").arg( GENESTA_ENGINE_VAR_IDENTIFIER1,
                                                                                    GENESTA_ENGINE_VAR_IDENTIFIER2,
                                                                                    GENESTA_ENGINE_VAR_IDENTIFIER3,
                                                                                    GENESTA_CHANNEL_VAR_IDENTIFIER1,
                                                                                    GENESTA_CHANNEL_VAR_IDENTIFIER2,
                                                                                    GENESTA_VAR1_IDENTIFIER,
                                                                                    GENESTA_VAR2_IDENTIFIER);
    rgExp.setPattern(sPattern);
    if(ioStatement.isEmpty()==false)
       {
           ioStatement=ioStatement.replace("_G.",".");

       }
    else
        return false;
    int dPos = rgExp.indexIn(ioStatement,0);
    QString sStatementParam = ioStatement.mid(0,dPos);
    if (dPos!=-1)
    {
        rC=true;
        int dEndPos= ioStatement.indexOf(")",dPos);
        QString varValues = ioStatement.mid(dPos+1,dEndPos-dPos-1); //(Ø=both engines, ×=channel A, ¤=1)

        QStringList newVarValueList;//should hold (Ø=engine 1, ×=channel A, ¤=1),(Ø=engine 2, ×=channel A, ¤=1)
        QStringList varVaueLst = varValues.split(",");


        foreach(QString sVarVal,varVaueLst)
        {

            QStringList sVarValInfoLst = sVarVal.split("=");
            if (sVarValInfoLst.size()==2)
            {
                QString sVarname = sVarValInfoLst.at(0).split(" ",QString::SkipEmptyParts).at(0);
                if (sVarname.size())
                {
                    QString sVal = sVarValInfoLst.at(1);
                    if (sVal.contains("engine",Qt::CaseInsensitive))
                    {
                        sVal.remove("engine",Qt::CaseInsensitive);
                        sVal.remove(" ");
                    }
                    else if (sVal.contains("channel",Qt::CaseInsensitive))
                    {
                        sVal.remove("channel",Qt::CaseInsensitive);
                        sVal.remove(" ");
                    }
                    else
                        rC = false;

                    if(!sVal.isEmpty())
                        ioStatement.replace(sVarname,sVal);
                }
                else
                    rC = false;


            }
        }
    }

    return rC;
}
SGenestaParamInfo AINGTAGenestaParserItf::getParamInfoObjFromParamStr(QString isParamInfo)
{
    SGenestaParamInfo paramInfo;


    QString channelAStr1 = QString("%1=%2").arg(GENESTA_CHANNEL_VAR_IDENTIFIER1,QString("channel A")); //×=channel A
    QString channelBStr1 = QString("%1=%2").arg(GENESTA_CHANNEL_VAR_IDENTIFIER1,QString("channel B"));  //×=channel B
    QString channelAStr2 = QString("%1=%2").arg(GENESTA_CHANNEL_VAR_IDENTIFIER2,QString("channel A")); //×=channel A
    QString channelBStr2 = QString("%1=%2").arg(GENESTA_CHANNEL_VAR_IDENTIFIER2,QString("channel B"));  //×=channel B
    QString Engine1Str = QString("%1=%2").arg(GENESTA_ENGINE_VAR_IDENTIFIER1,QString("engine 1")); //×=channel A
    QString Engine2Str = QString("%1=%2").arg(GENESTA_ENGINE_VAR_IDENTIFIER1,QString("engine 2"));  //×=channel B
    
    if (isParamInfo.contains(channelAStr1))
        paramInfo._Channel = "A";
    if (isParamInfo.contains(channelBStr1))
        paramInfo._Channel = "B";
    if (isParamInfo.contains(channelAStr2))
        paramInfo._Channel = "A";
    if (isParamInfo.contains(channelBStr2))
        paramInfo._Channel = "B";
    if (isParamInfo.contains(Engine1Str))
        paramInfo._Engine = "1";
    if (isParamInfo.contains(Engine2Str))
        paramInfo._Engine = "2";

    
    QString paramWithInfo = isParamInfo;
    replaceGenSymbols(paramWithInfo);
    QString sParameter;
    sParameter = paramWithInfo.split(" ",QString::SkipEmptyParts).at(0);
    if(sParameter.isEmpty()==false)
        sParameter=sParameter.replace("_G.","."); 
    /*if (AINGTAGenestaParserItf::isTriplet(sParameter))
    {
        sParameter = sParameter.toUpper();
    }*/
    paramInfo._Parameter=sParameter;

    
    return paramInfo;
}
bool AINGTAGenestaParserItf::isTriplet(const QString& iParam)
{
    if(iParam.count(".")>=2)
        return true;

    return false;
}
//QMap<QString,QString> AINGTAGenestaParserItf::getGenestaVarMap()
//{
//    AINGTAAppController* pController = AINGTAAppController::getGTAAppController(0);
//    QMap<QString,QString> varSymbolMap;
//    return varSymbolMap;
//}
