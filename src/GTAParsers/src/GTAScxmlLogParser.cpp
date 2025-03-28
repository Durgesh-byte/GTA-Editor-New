#include "GTAScxmlLogParser.h"

#pragma warning(push, 0)
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#pragma warning(pop)

GTAScxmlLogParser::GTAScxmlLogParser(const QString &iLogFile)
{
    _LogFile =iLogFile;
    _ExecStart = "";
    _ExecEnd = "";
}
bool GTAScxmlLogParser::getLogResult( QHash<QString,LogMessageList *> & oLogResult, bool iRepetativeLog)                           
{
    bool rc = true;
    QFile objLogFile(_LogFile);
    rc = objLogFile.exists();
    bool repeatLogs = iRepetativeLog;
    QStringList loopMessages;
    if(rc)
    {
        rc = objLogFile.open(QFile::Text | QFile::ReadOnly);
        if(rc)
        {
            QTextStream reader(&objLogFile);
            QStringList linesParsed;
            while(! reader.atEnd())
            {
                QString line = reader.readLine();

                //regx and condition introduced to obtain all logs for loops
                //created during foreach action
                //QRegExp regExp("_uuid\\[\\{[a-z0-9]{8}-([a-z0-9]{4}-){3}[a-z0-9]{12}\\}\\]");

//                if(line.contains("line."))
//                    repeatLogs = true;
//                else
//                    repeatLogs = iRepetativeLog;

                //DEV : to ensure all foreach commands are forced with repeated logging treatment
                if (line.contains("]#_START_"))
                {
                    repeatLogs = true;

                    QStringList logpieces = line.split("\t");
                    QStringList logList;
                    for(int i=3;i<logpieces.count();i++)
                        logList.append(logpieces.at(i));

                    QString lineadd = logList.join("\t");

                    //adding the logs to logmessages without state id and timestamp
                    loopMessages.append(lineadd);
                    continue;
                }
                else if (line.contains("]#_END_"))
                {
                    QStringList logpieces = line.split("\t");
                    QStringList logList;
                    for(int i=3;i<logpieces.count();i++)
                        logList.append(logpieces.at(i));

                    QString modline = logList.join("\t");
                    loopMessages.removeOne(modline.replace("]#_END_","]#_START_"));
                    if (loopMessages.isEmpty())
                        repeatLogs = iRepetativeLog;
                    continue;
                }

                
                if(line.contains(":;:"))
                {

                    QStringList logTabList = line.split("\t");
                    if(logTabList.count() >= 4 )
                    {

                        QString timeStamp = logTabList.at(0);
                        QString statID = logTabList.at(2);
                        QString log = "";
                        QStringList logList;
                        for(int i=3;i<logTabList.count();i++)
                            logList.append(logTabList.at(i));

                        log = logList.join("\t");

                        if(_ExecStart.isEmpty())
                            _ExecStart = timeStamp;
                        _ExecEnd = timeStamp;

                        if(!repeatLogs)
                        {
                            if(linesParsed.contains(log))
                                continue;
                            else
                                linesParsed.append(log);
                        }



                        GTAScxmlLogMessage objLogMessage(log);
						
						//check to remove internal array name to user readable name
                        if (repeatLogs)
                        {
                            QString varName = objLogMessage.ExpectedValue;
                            //removing state ID name only for print statement 
                            if (varName.contains("line.") || varName.contains("_line_"))
                            {
                                QStringList VarNamelst = varName.split(".");
                                objLogMessage.ExpectedValue = QString("line.%1").arg(VarNamelst.last());
                            }
                        }

                        if(!objLogMessage.Identifier.trimmed().isEmpty())
                        {
                            objLogMessage.TimeStamp = timeStamp;
                            objLogMessage.StatID = statID;
                            QString objId = objLogMessage.Identifier;
                            LogMessageList * pList = NULL;
                            if(oLogResult.contains(objLogMessage.Identifier))
                            {
                                pList = oLogResult.value(objLogMessage.Identifier);
                                for(int logIdx=0;logIdx<pList->count();logIdx++)
                                {
                                    GTAScxmlLogMessage msg = pList->at(logIdx);
                                    if(msg.LOD == GTAScxmlLogMessage::Main)
                                    {
                                        if((msg.Result == GTAScxmlLogMessage::OK) && (objLogMessage.Result == GTAScxmlLogMessage::KO))
                                        {
                                            pList->removeAt(logIdx);
                                        }
                                        else if((msg.Result == GTAScxmlLogMessage::KO) && (objLogMessage.Result == GTAScxmlLogMessage::OK))
                                        {
                                            pList->removeAt(logIdx);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                pList = new LogMessageList;
                                if (repeatLogs)
                                    oLogResult.insertMulti(objLogMessage.Identifier,pList);
                                else
                                    oLogResult.insert(objLogMessage.Identifier,pList);
                            }

                            if(pList != NULL)
                            {

                                pList->append(objLogMessage);
                            }
                        }
                    }
                }
            }

            objLogFile.close();
        }
    }
    return rc;
}
