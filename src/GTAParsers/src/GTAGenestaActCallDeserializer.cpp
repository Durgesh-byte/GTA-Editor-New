#include "AINGTAGenestaActCallDeserializer.h"
#include "AINGTAUtil.h"
#include "AINGTAActionCall.h"
#include "AINGTAActionPrint.h"
#include "AINGTAActionPrintTable.h"
#include "AINGTAGenestaFileParser.h"
#include <QFileInfo>
#include <QDir>
#include "AINGTAGenestaFileParser.h"

#if _MSC_VER >= 1600   //MVSC2015>1899, MSVC_VER=14.0
#pragma execution_character_set("utf-8")
#endif

AINGTAGenestaActCallDeserializer::AINGTAGenestaActCallDeserializer(const QString& iStmt, const QString& isEquipment, const QString& isAircraft, const QStringList &iFileList,const QString & iExportFolder):AINGTAGenestaParserItf(iStmt)
{
    _sEquipment = isEquipment;
    _sAircraft =isAircraft;
    _sFileList = iFileList;
    _exportFolder = iExportFolder;
}
bool AINGTAGenestaActCallDeserializer::getCommands(QList<AINGTACommandBase*>& oLstCmds)
{
    bool rC = false;

    if (_statement.isEmpty())
    {
        _lastError="Nothing to evaluate with empty statement";
        return rC;
    }


    QStringList sList;
    QString sCallParameter = _statement.remove(QString("%1 -").arg(GENESTA_CMD_CALL));
    QString sCalledFileName = _statement.split("(").at(0);
    if (sCalledFileName.isEmpty())
    {
        sCalledFileName = _statement;

    }
    sCalledFileName=sCalledFileName.trimmed();
    AINGTAGenestaParserItf::getPreprocessedStatements(_statement,sList);
    bool found = false;
    QString genestaFileName;
    foreach(QString statement, sList)
    {
        if (!statement.isEmpty())
        {



            QStringList scallParamInfoList = statement.split(" ", QString::SkipEmptyParts);
            if (!scallParamInfoList.isEmpty())
            {
                QString sCallParameter = scallParamInfoList.at(0);


                QMap<QString,QString> mapOfSymbols = AINGTAGenestaFileParser::getGenstaSymbolMap();

                foreach(QString symbol, mapOfSymbols.keys())
                {
                    sCallParameter.replace(symbol,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),mapOfSymbols.value(symbol)));
                }
                //                    sCallParameter.replace(GENESTA_ENGINE_VAR_IDENTIFIER1,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_ENGINE_VARIABLE));
                //sCallParameter.replace(GENESTA_ENGINE_VAR_IDENTIFIER2,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_ENGINE_VARIABLE));
                //sCallParameter.replace(GENESTA_ENGINE_VAR_IDENTIFIER3,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_ENGINE_VARIABLE));

                //sCallParameter.replace(GENESTA_CHANNEL_VAR_IDENTIFIER1,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_CHANNEL_VARIABLE));
                //sCallParameter.replace(GENESTA_CHANNEL_VAR_IDENTIFIER2,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_CHANNEL_VARIABLE));

                //sCallParameter.replace(GENESTA_VAR1_IDENTIFIER,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_VAR1_VARIABLE));
                //sCallParameter.replace(GENESTA_VAR2_IDENTIFIER,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),GTA_VAR2_VARIABLE));

                QString localAircraft = _sAircraft;
                QString localEquipment = _sEquipment;

                QString sCalledFunc = QString("%1_%2_%3.fun").arg(sCallParameter,localAircraft,localEquipment);

                if (sCallParameter.startsWith(GENESTA_PRINT_TABLE_IDENTIFIER))
                {
                    found = true;
                    AINGTAActionPrintTable* printTable = NULL;
                    QString sPrintFileName=sCallParameter;

                    if(!_sAircraft.isEmpty())
                        sPrintFileName.append(QString("_%1").arg(_sAircraft));
                    if(!_sEquipment.isEmpty())
                        sPrintFileName.append(QString("_%1").arg(_sEquipment));

                    printTable = getPrintTableCommand(QString("%1.obj").arg(sCallParameter));
                    if (printTable==NULL)
                    {
                        _lastError = "";
                        printTable = getPrintTableCommand(QString("%1.obj").arg(sPrintFileName));

                    }
                    if(printTable!=NULL)
                    {
                        _lastError = "";
                        rC = true;
                        oLstCmds.append(printTable);
                    }
                    else
                        return rC;
                }
                else
                {
                    AINGTAActionCall* pCall = new AINGTAActionCall;
                    pCall->setAction(ACT_CALL);

                    pCall->setComplement(COM_CALL_FUNC);


                    oLstCmds.append(pCall);

                    QMap <QString,QString> tagValueMap;
                    getTagValueMap(statement,tagValueMap);

                    if (!tagValueMap.isEmpty())
                    {
                        QMapIterator<QString,QString> iterTag(mapOfSymbols);
                        while (iterTag.hasNext())
                        {
                            iterTag.next();
                            QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
                            sCalledFunc.replace(tag,iterTag.value());

                        }
                    }

                    pCall->setElement(sCalledFunc);
                    pCall->setTagValue(tagValueMap);




                    //Check if the file exists in the export list.
                    genestaFileName = QString("%1_%2_%3.obj").arg(sCalledFileName,localAircraft,localEquipment);
                    foreach(QString  objFile,_sFileList)
                    {
                        if(objFile.contains(genestaFileName))
                        {
                            QFileInfo info(objFile);
                            if(info.fileName() == genestaFileName)
                            {
                                found = true;
                                break;
                            }
                        }
                        else
                        {

                            foreach(QString symbol, mapOfSymbols.keys())
                            {
                                objFile.replace(symbol,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),mapOfSymbols.value(symbol)));
                            }
                            //QFileInfo info(objFile);
                            if(objFile.contains(genestaFileName))
                            {

                                found = true;
                                break;


                            }
                        }
                    }

                    foreach(QString symbol, mapOfSymbols.keys())
                    {
                        genestaFileName.replace(symbol,QString("%1%2%1").arg(QString(TAG_IDENTIFIER),mapOfSymbols.value(symbol)));
                    }
                    //Check if the file has already been exported.
                    if(!found)
                    {
                        QDir exportDir;
                        exportDir.setPath(_exportFolder);
                        QStringList fileList = exportDir.entryList(QStringList()<<"*.fun",QDir::Files);

                        foreach(QString  objFile,fileList)
                        {

                            QFileInfo info(objFile);
                            if(info.fileName() == sCalledFunc)
                            {
                                found = true;
                                break;
                            }
                            else if ( genestaFileName.contains(info.baseName(),Qt::CaseInsensitive))
                            {
                                found = true;
                                break;
                            }


                        }
                    }
                }

            }

        }

    }
    //if (oLstCmds.size()!=sList.size())
    //{
    //	_lastError="could not evaluate one or more call command";
    //	rC =false;
    //}	else
    if(found == false)
    {
        _lastError=QString("Call Object (%1) not found.").arg(genestaFileName);
        rC =false;
    }
    else
        rC=true;

    return rC;
}

AINGTAActionPrintTable* AINGTAGenestaActCallDeserializer::getPrintTableCommand(const QString& iPrintFileName)
{
    AINGTAActionPrintTable* pPrintTable = NULL;

    QString printFilePath;
    bool found=false;
    foreach(QString  objFile,_sFileList)
    {
        if(objFile.contains(iPrintFileName))
        {
            QFileInfo info(objFile);
            if(info.fileName() == iPrintFileName)
            {
                found = true;
                printFilePath=info.absoluteFilePath();
                break;
            }
        }
    }
    if (found)
    {

        AINGTAGenestaFileParser obj(printFilePath,AINGTAElement::OBJECT,_sFileList);

        AINGTAElement* pElement = obj.getElement();

        QStringList parameters;
        if (pElement!=NULL)
        {
            for (int i=0;i<pElement->getAllChildrenCount();i++)
            {
                AINGTACommandBase* pCurCmd = NULL;
                pElement->getCommand(i,pCurCmd);
                if (pCurCmd)
                {
                    AINGTAActionPrint* pPrint = dynamic_cast<AINGTAActionPrint*>(pCurCmd);
                    if(pPrint!=NULL && pPrint->getComplement()==COM_PRINT_PARAM)
                        parameters.append(pPrint->getValues());

                }
            }

            parameters.removeDuplicates();
            delete pElement;
            pElement = NULL;
            pPrintTable = new AINGTAActionPrintTable();
            pPrintTable->setValues(parameters);
            QString tableName = iPrintFileName;
            tableName.remove(".obj");
            tableName.remove(QString("%1_").arg(GENESTA_PRINT_TABLE_IDENTIFIER));
            pPrintTable->setTableName(tableName);
        }

    }
    else
        _lastError = "print file not found";

    return pPrintTable;

}
bool AINGTAGenestaActCallDeserializer::getTagValueMap(const QString& iStatement,QMap <QString,QString>& ioTagValueMap)
{
    bool rC = true;
    QRegExp rgExp;
    QString sPattern = QString("\\(\\s+[%1%2%3%4%5%6%7]|\\([%1%2%3%4%5%6%7]").arg(GENESTA_ENGINE_VAR_IDENTIFIER1,
                                                                                  GENESTA_ENGINE_VAR_IDENTIFIER2,
                                                                                  GENESTA_ENGINE_VAR_IDENTIFIER3,
                                                                                  GENESTA_CHANNEL_VAR_IDENTIFIER1,
                                                                                  GENESTA_CHANNEL_VAR_IDENTIFIER2,
                                                                                  GENESTA_VAR1_IDENTIFIER,
                                                                                  GENESTA_VAR2_IDENTIFIER);
    rgExp.setPattern(sPattern);
    int dPos = rgExp.indexIn(iStatement,0);
    QString sStatementParam = iStatement.mid(0,dPos);
    if (dPos!=-1)
    {
        int dEndPos= iStatement.indexOf(")",dPos);
        QString varValues = iStatement.mid(dPos+1,dEndPos-dPos-1); //(Ø=both engines, ×=channel A, ¤=1)

        QStringList newVarValueList;//should hold (Ø=engine 1, ×=channel A, ¤=1),(Ø=engine 2, ×=channel A, ¤=1)
        QStringList varVaueLst = varValues.split(",");


        int i=0;
        foreach(QString sVarVal,varVaueLst)
        {
            QString sNth = QString::number(i++);
            QStringList sVarValInfoLst = sVarVal.split("=");
            if (sVarValInfoLst.size()==2)
            {
                QString sVarname = AINGTAGenestaParserItf::getGTAVarName(sVarValInfoLst.at(0),-1);
                if (sVarname.size())
                {
                    QString sVarValue = sVarValInfoLst.at(1);
                    sVarValue = sVarValue.replace("engine","");
                    sVarValue = sVarValue.replace("channel","");
                    ioTagValueMap.insert(sVarname,sVarValue.trimmed());
                }

                else
                    rC = false;


            }
        }
    }

    return rC;
}


void AINGTAGenestaActCallDeserializer::getPreprocessedStatements(const QString& iStatement, QStringList& oList)
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

    if (bFADEC)
    {   sStatement.replace("channel in control","both channels");
	sStatement.replace("channel not in control","both channels");
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
            if((vals.count() > 1) && (vals.at(1).contains("both channels")))
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
        mapReplaceItems.insert(QString(GENESTA_CHANNEL_VAR_IDENTIFIER2),QString("@%1@").arg(GTA_CHANNEL_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_VAR1_IDENTIFIER),QString("@%1@").arg(GTA_VAR1_VARIABLE));
        mapReplaceItems.insert(QString(GENESTA_VAR2_IDENTIFIER),QString("@%1@").arg(GTA_VAR1_VARIABLE));
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
        oList.append(currentStatement);
    }
}
