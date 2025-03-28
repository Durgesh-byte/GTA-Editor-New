#include "GTAUtil.h"
#include <iostream>
#include <string>

#pragma warning(push, 0)
#include <QHash>
#include <QDir>
#include <QProcess>
#include "GTAZip.h"
#include <QtWidgets/QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#pragma warning(pop)

QString GTAUtil::_LibraryPath;
QString GTAUtil::_SchedulerType = S2I_SCHEDULER;
QString GTAUtil::_ToolDataPath;
QString GTAUtil::_memExportPath;
GTAUtil::GTAUtil()
{
}

QStringList GTAUtil::getParamChannelIdentifiers()
{
    return QStringList()<<"CHA"<<"CHB"<<"CH_A"<<"CH_B";
}
QList<QRegExp> GTAUtil::getParameterChannelIdentifiers()
{
    QList<QRegExp> paramChannelIdentifiers;
    
    paramChannelIdentifiers << QRegExp("_CHA.");
    paramChannelIdentifiers << QRegExp("_CHB.");
    
    // paramChannelIdentifiers << QRegExp("_CHA");
    //paramChannelIdentifiers << QRegExp("_CHA\\b");
    //paramChannelIdentifiers << QRegExp("_CH_A_");
    //paramChannelIdentifiers << QRegExp("_CH_A\\b");
    //paramChannelIdentifiers << QRegExp("_CHB_");
    //paramChannelIdentifiers << QRegExp("_CHB\\b");
    //paramChannelIdentifiers << QRegExp("_CH_B_");
    //paramChannelIdentifiers << QRegExp("_CH_B\\b");
    //  paramChannelIdentifiers << QRegExp("_A");
    //  paramChannelIdentifiers << QRegExp("_B");
    return paramChannelIdentifiers;
}
QList<QRegExp> GTAUtil::getParameterEngineIdentifiers()
{
    QList<QRegExp> paramEngineIdentifiers;
    paramEngineIdentifiers << QRegExp("EEC1_A");
    paramEngineIdentifiers << QRegExp("EEC2_A");
    paramEngineIdentifiers << QRegExp("EEC1_B");
    paramEngineIdentifiers << QRegExp("EEC2_B");
    return paramEngineIdentifiers;
}
QStringList GTAUtil::getParamEngIdentifiers()
{
    return QStringList()<<"EEC1_A"<<"EEC1_B"<<"EEC2_A"<<"EEC2_B";
}

QString GTAUtil::getListOfForbiddenChars()
{
    QString lstOfchars("-+*/^&~%!|(){}[]");
    
    return lstOfchars;
}
void GTAUtil::clearDirectory(const QString& iFolderPath)
{
    
    QFileInfo dirInfo(iFolderPath);
    if (dirInfo.exists()&& dirInfo.isDir())
    {
        QDir directoryToClean(iFolderPath);
        QFileInfoList lstOfEntries = directoryToClean.entryInfoList(QStringList("*"),QDir::Files);
        QFileInfoList lstOfEntries1 = directoryToClean.entryInfoList(QStringList("*"),QDir::Dirs);
        foreach(QFileInfo fileInfo, lstOfEntries)
        {
            
            QFile file(fileInfo.absoluteFilePath());
            file.remove(fileInfo.absoluteFilePath());
        }
        foreach(QFileInfo fileInfo, lstOfEntries1)
        {
            QDir dir(fileInfo.absoluteFilePath());
            dir.rmdir(fileInfo.absoluteFilePath());
        }
    }
    
    
}
QHash<QString,QString> GTAUtil::getNomenclatureOfArithSymbols()
{
    QHash<QString,QString> mapEqnStr;
    mapEqnStr[QString(ARITH_EQ)]=QString("EQUAL_TO");
    mapEqnStr[QString (ARITH_NOTEQ)]=QString("NOT_EQUAL_TO");
    mapEqnStr[QString (ARITH_GT)]=QString("GREATER_THAN");
    mapEqnStr[QString (ARITH_LTEQ)]=QString("LESSER_THAN_EQ_TO");
    mapEqnStr[QString (ARITH_LT)]=QString("LESSER_THAN");;
    mapEqnStr[QString (ARITH_GTEQ)]=QString("GREATER_THAN_EQ_TO");
    return mapEqnStr;
}
QStringList GTAUtil::getProcessedParameterForChannel(const QString& iParameter , bool bTreatCICAsBoth, bool bTreatCNICAsBoth)
{
    QStringList parameters;
    QString parameter(iParameter);
    if (parameter.contains(GTA_CIC_IDENTIFIER) && parameter.contains(":"))
    {
        
        QStringList varIdentifiers,varValues;
        getChannelSelectionVariable(parameter,varIdentifiers,varValues);
        
        if(varIdentifiers.isEmpty() == false && varValues.isEmpty() == false)
        {
            QString varValue = varValues.at(0);
            QString paramName = parameter.trimmed();
            
            if(((varValue == CHANNEL_IN_CONTROL )|| (varValue == CHANNEL_NOT_IN_CONTROL))&& (bTreatCICAsBoth == true || bTreatCNICAsBoth == true))
                varValue=BOTH_CHANNEL;
            
            if (varValue==BOTH_CHANNEL)
            {
                
                parameters<<paramName<<paramName;
                foreach(QString varIdentifier,varIdentifiers)
                {
                    QString varIdentifierl = varIdentifier;
                    
                    QString chA,chB;
                    if(varIdentifier.contains("B"))
                    {
                        chB = varIdentifierl;
                        chA = varIdentifierl.replace("B","A");
                    }
                    else if (varIdentifier.contains("A"))
                    {
                        chA = varIdentifierl;
                        chB = varIdentifierl.replace("A","B");
                    }
                    
                    QString paramNameToAppend = parameters.takeAt(0);
                    paramNameToAppend.replace( QString("%1%2%1").arg(GTA_CIC_IDENTIFIER,varIdentifier),chA );
                    parameters.append(paramNameToAppend);
                    
                    paramNameToAppend = parameters.takeAt(0);
                    paramNameToAppend.replace( QString("%1%2%1").arg(GTA_CIC_IDENTIFIER,varIdentifier),chB );
                    parameters.append(paramNameToAppend);
                    
                }
                
                
            }
            else if (varValue == CHANNEL_IN_CONTROL )
            {
                
                parameters.append(iParameter);
            }
            else if (varValue == CHANNEL_NOT_IN_CONTROL)
            {
                parameters.append(iParameter);
            }
            else if (varValue.isEmpty()==false)
            {
                parameters.append(paramName);
                parameters.append(paramName);
                
                foreach(QString varIdentifier,varIdentifiers)
                {
                    QString varIdentifierl = varIdentifier;
                    
                    QString chA,chB;
                    if(varIdentifier.contains("B"))
                    {
                        chB = varIdentifierl;
                        chA = varIdentifierl.replace("B","A");
                    }
                    else if (varIdentifier.contains("A"))
                    {
                        chA = varIdentifierl;
                        chB = varIdentifierl.replace("A","B");
                    }
                    
                    QString paramNameToAppend = parameters.takeAt(0);
                    paramNameToAppend.replace( QString("%1%2%1").arg(GTA_CIC_IDENTIFIER,varIdentifier),chA );
                    parameters.append(paramNameToAppend);
                    
                    paramNameToAppend = parameters.takeAt(0);
                    paramNameToAppend.replace( QString("%1%2%1").arg(GTA_CIC_IDENTIFIER,varIdentifier),chB );
                    parameters.append(paramNameToAppend);
                    
                }
                if(varValue.contains("A"))
                    parameters.removeAt(1);
                else
                    parameters.removeAt(0);
            }
        }
        
    }
    
    if (parameters.isEmpty())
        parameters.append(iParameter);
    return parameters;
    
}
void GTAUtil::getChannelSelectionVariable(QString& ioParameter,QStringList& oChvar,QStringList& oChVal)
{
    
    
    bool startAppendingVar = false;
    bool stopAppendingVar = false;
    bool startAppendingVal = false;
    bool stopAppendingVal = false;
    QString sChVal,sChvar;
    
    if (!ioParameter.isEmpty())
    {
        foreach (QChar ch, ioParameter)
        {
            if (startAppendingVar ==false && QString(ch) == GTA_CIC_IDENTIFIER)
            {
                startAppendingVar = true;
                continue;
            }
            
            if (ch == ':' )
            {
                stopAppendingVar = true;
                startAppendingVal = true;
                continue;
            }
            if(startAppendingVal==true && QString(ch) == GTA_CIC_IDENTIFIER)
            {
                /*stopAppendingVar = true;
                stopAppendingVal = true;
                break;*/
                oChvar.append(sChvar);
                oChVal.append(sChVal);
                sChVal.clear();sChvar.clear();
                startAppendingVar = false;
                stopAppendingVar = false;
                startAppendingVal = false;
                stopAppendingVal = false;
                
            }
            
            if (startAppendingVar==true && stopAppendingVar == false)
                sChvar.append(ch);
            
            if (startAppendingVal==true && stopAppendingVal == false)
                sChVal.append(ch);
            
        }
        
        if(oChVal.isEmpty()==false)
            foreach(QString chValue, oChVal)
            { ioParameter.remove(QString(":%1").arg(chValue));}
        
    }
    
    
}
bool GTAUtil::resolveChannelParam(const QString &iParamName, QStringList &oResolvedParams)
{

    bool rc = false;
    QString cicAIdentifier = QString("A:CIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cicBIdentifier = QString("B:CIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cnicAIdentifier = QString("A:CNIC%1").arg(GTA_CIC_IDENTIFIER);
    QString cnicBIdentifier = QString("B:CNIC%1").arg(GTA_CIC_IDENTIFIER);
    QString bothIdentifier = QString(":both%1").arg(GTA_CIC_IDENTIFIER);

    if((iParamName.contains(cicAIdentifier)) ||
       (iParamName.contains(cicBIdentifier)) ||
       (iParamName.contains(cnicAIdentifier)) ||
       (iParamName.contains(cnicBIdentifier)) ||
       (iParamName.contains(bothIdentifier)))
    {
        rc = true;
    }
    else
    {
        rc = false;
    }
    oResolvedParams = getProcessedParameterForChannel(iParamName,true,true);
    oResolvedParams.removeDuplicates();
    return rc;
}

/**
  * This function provides a custom element grouping by managing Calls under titles for multiple logs
  * @return: List of all engines available
  */
QStringList GTAUtil::getAvailableEngines()
{

    QStringList engineList;
    QString engine = "";

    QString appPath = QApplication::applicationDirPath();
    QString ToolData = getToolDataPath();
    QDir dir(ToolData);
    if(dir.exists())
    {
        QFileInfo file(QDir::cleanPath(QString("%1%2%3").arg(ToolData,QDir::separator(),"ChannelSelectionParamInfo.xml")));
        if(file.exists())
        {
            QString channelSelectionParamFile = file.absoluteFilePath();

            QFile xmlFile(channelSelectionParamFile);
            bool rc = xmlFile.open(QFile::ReadOnly | QFile::Text);
            QDomDocument * pDomDoc = NULL;
            if (rc)
            {
                pDomDoc = new QDomDocument();
                if(pDomDoc != NULL)
                {
                    if(pDomDoc->setContent(&xmlFile))
                    {
                        QDomElement rootElem = pDomDoc->documentElement();
                        QDomNodeList enginesNodeLst = rootElem.elementsByTagName("ENGINES");
                        for(int j = 0; j < enginesNodeLst.count(); j++)
                        {
                            QDomNode enginesNode = enginesNodeLst.at(j);
                            if(!enginesNode.isNull())
                            {
                                QDomNodeList engineNodeList = enginesNode.childNodes();
                                for(int k=0;k<engineNodeList.count();k++)
                                {
                                    QDomNode engineNode = engineNodeList.at(k);
                                    if(engineNode.nodeName() == "ENGINE")
                                    {
                                        QDomNamedNodeMap engineAttributes = engineNode.attributes();
                                        engine = engineAttributes.namedItem("id").nodeValue();
                                        engineList.append(engine);
                                    }
                                }
                            }
                        }
                    }
                    delete pDomDoc;
                    pDomDoc = NULL;
                }
                else
                {
                    delete pDomDoc;
                    pDomDoc = NULL;

                }
            }
            xmlFile.close();
        }
    }
    return engineList;
}

QStringList GTAUtil::getChannelSelectionSignals(const QString &iEngine,QHash<QString,QString> &oEngineVarMap, const QString &iChannelID)
{

    QStringList vars;

    QString appPath = QApplication::applicationDirPath();
    QString ToolData = getToolDataPath();
    QDir dir(ToolData);
    if(dir.exists())
    {
        QFileInfo file(QDir::cleanPath(QString("%1%2%3").arg(ToolData,QDir::separator(),"ChannelSelectionParamInfo.xml")));
        if(file.exists())
        {
            QString channelSelectionParamFile = file.absoluteFilePath();

            QFile xmlFile(channelSelectionParamFile);
            bool rc = xmlFile.open(QFile::ReadOnly | QFile::Text);
            QDomDocument * pDomDoc = NULL;
            if (rc)
            {
                pDomDoc = new QDomDocument();
                if(pDomDoc != NULL)
                {
                    if(pDomDoc->setContent(&xmlFile))
                    {
                        //iEngine
                        QDomElement rootElem = pDomDoc->documentElement();
                        QDomNodeList enginesNodeLst = rootElem.elementsByTagName("ENGINES");
                        for(int j = 0; j < enginesNodeLst.count(); j++)
                        {
                            QDomNode enginesNode = enginesNodeLst.at(j);
                            if(!enginesNode.isNull())
                            {
                                QDomNodeList engineNodeList = enginesNode.childNodes();
                                for(int k=0;k<engineNodeList.count();k++)
                                {
                                    QDomNode engineNode = engineNodeList.at(k);
                                    if(engineNode.nodeName() == "ENGINE")
                                    {
                                        QDomNamedNodeMap engineAttributes = engineNode.attributes();
                                        if(engineAttributes.namedItem("id").nodeValue() == iEngine)
                                        {
                                            if(engineNode.hasChildNodes())
                                            {
                                                QDomNodeList channelNodeLst = engineNode.childNodes();
                                                for(int cnt = 0; cnt < channelNodeLst.count(); cnt++)
                                                {
                                                    QDomNode channelNode = channelNodeLst.at(cnt);
                                                    if(channelNode.nodeName() == "CH")
                                                    {
                                                        QDomNamedNodeMap channelAttributes = channelNode.attributes();
                                                        if((channelAttributes.namedItem("id").nodeValue() == iChannelID))
                                                        {
                                                            QString sEngineNumber = channelAttributes.namedItem("engine").nodeValue();
                                                            QString sParam = channelAttributes.namedItem("param").nodeValue();
                                                            bool ok;
                                                            sEngineNumber.toInt(&ok);
                                                            if(ok)
                                                            {
                                                                if(!sParam.trimmed().isEmpty())
                                                                {
                                                                    vars.append(sParam);
                                                                    oEngineVarMap.insert(sEngineNumber,sParam);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                        }

                    }
                    delete pDomDoc;
                    pDomDoc = NULL;

                }
                else
                {
                    delete pDomDoc;
                    pDomDoc = NULL;

                }
            }
            xmlFile.close();

        }


    }
    return vars;
}
bool GTAUtil::copyFilesRecursively(QString isSourcePath, QString isDestPath,const  bool iReplaceFile)
{
    QDir dir;
    dir.setPath(isSourcePath);

    isSourcePath += QDir::separator();
    isDestPath += QDir::separator();

    foreach (QString copy_file, dir.entryList(QDir::Files))
    {
        QString sSource = isSourcePath + copy_file;
        QString sDestination = isDestPath + copy_file;

        if (QFile::exists(sDestination))
        {
            if (iReplaceFile)
            {
                if (QFile::remove(sDestination) == false)
                {
                    return false;
                }
            }
            else
            {
                continue;
            }
        }

        if (QFile::copy(sSource, sDestination) == false)
        {
            return false;
        }
    }

    foreach (QString copy_dir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString from = isSourcePath + copy_dir;
        QString to = isDestPath + copy_dir;

        if (dir.mkpath(to) == false)
        {
            return false;
        }

        if (copyFilesRecursively(from, to, iReplaceFile) == false)
        {
            return false;
        }
    }

    return true;
}
QString GTAUtil::getNewInstanceName(const QString &iInstanceName)
{
    QString newInstanceName = iInstanceName;

    QRegExp rx("+*/^&~%!|(){}[]~,");
    newInstanceName.replace("\n","");
    newInstanceName.replace("\r","");
    newInstanceName.replace(" ","_Space_");
    newInstanceName.replace("/","_bckSlash_");
    newInstanceName.replace("*","_Star_");
    newInstanceName.replace("[","_OpenSqBct_");
    newInstanceName.replace("]","_CloseSqBct_");
    newInstanceName.replace("?","_Quest_");
    newInstanceName.replace("(","_CloseBct_");
    newInstanceName.replace(")","_OpenBct_");
    newInstanceName.replace(":","_Quolon_");
    newInstanceName.replace("#","_Hash_");
    newInstanceName.replace("-","_MINUS_");
    newInstanceName.replace("+","_PLUS_");
    newInstanceName.replace("^","_POWER_");
    newInstanceName.replace("{","_CloseFLOBct_");
    newInstanceName.replace("}","_OpenFLOBct_");
    newInstanceName.replace("!","_exc_");
    newInstanceName.replace("@","_AT_");
    newInstanceName.replace("$","_dOLLAR_");
    newInstanceName.replace("&","_amp_");
    newInstanceName.replace("=","_Equal_");
    newInstanceName.replace("|","_pipe_");
    newInstanceName.replace("\\","_forSlash_");
    newInstanceName.replace(";","_semicolon_");
    newInstanceName.replace("\"","_quotes_");
    newInstanceName.replace("'","_squote_");
    newInstanceName.replace("<","_lThan_");
    newInstanceName.replace(">","_gThan_");
    newInstanceName.replace(".","_dot_");
    newInstanceName.replace("`","_exec_");
    newInstanceName.replace("~","_tild_");


    return newInstanceName;
}
bool GTAUtil::CompressDirectory(const QString &iOutDirectory,const QString &iDirectoryToCompress)
{
    //  //  QProcess * pCompressProcess = new QProcess();
    //    QStringList argument;
    //
    //    QString arg("\""+iOutDirectory+ "\" \"" + iDirectoryToCompress+"\"");
    //
    iOutDirectory.toLatin1().data();
    iDirectoryToCompress.toLatin1().data();
    GTAZip::zipData(iOutDirectory,iDirectoryToCompress);

    return true;
}
void GTAUtil::setRepositoryPath(const QString &iLibPath)
{
    _LibraryPath = iLibPath;
}
QString GTAUtil::getRepositoryPath()
{
    return _LibraryPath;
}
void GTAUtil::updateWithGenToolParam(QString &ioParam,QHash<QString, QString> ioGenToolReturnMap)
{
    QStringList iTokens = ioParam.split("$");
    QStringList keys = ioGenToolReturnMap.keys();
    for(int i=0;i<keys.count();i++)
    {
        QString key = keys.at(i);

        QStringList tokens = key.split("$");
        QString loc = tokens.at(0);
        if((iTokens.count()>1) && (tokens.count()>1))
        {
            if(loc == iTokens.at(0))
            {
                if(iTokens.at(1).contains(tokens.at(1)))
                {
                    QString returnParam = ioGenToolReturnMap.value(key);

                    ioParam = iTokens.at(1);
                    ioParam = ioParam.replace(tokens.at(1),returnParam);
                    ioGenToolReturnMap.insert(ioParam,returnParam);
                    break;
                }
            }

        }

    }
}

void GTAUtil::setSchedulerType(bool isUSCXML)
{
    if(isUSCXML)
        _SchedulerType = USCXML_SCHEDULER;
    else
        _SchedulerType = S2I_SCHEDULER;
}

bool GTAUtil::isUSCXMLScheduler()
{

    return (_SchedulerType == USCXML_SCHEDULER) ? true : false;
}

void GTAUtil::setToolDataPath(const QString &iToolDataPath)
{
    _ToolDataPath = iToolDataPath;
}

QString GTAUtil::getToolDataPath()
{
    return _ToolDataPath;
}

void GTAUtil::setGlobalPathExportScxml(QString& _path)
{
    _memExportPath = _path;
}
QString GTAUtil::getGlobalPathExportScxml()
{
    return _memExportPath;
}


/* This function returns the minimum acceptable value of TimeOut if the Confirmation time is visible
* * The TimeOut has to be greater by delta seconds to Confirmation Time
* @input timeOutUnit:   QString unit of Time Out
* @input confTimeUnit:  QString unit of Confirmation Time
* @input confTime:      QString value of Confirmation Time
* @input delta:         double minimum difference between TimeOut and ConfTime
* @return minTimeOut:   double minimum acceptable value of TimeOut
*/
double GTAUtil::getMinTimeOutValue(QString timeOutUnit, QString confTimeUnit, QString confTime, double delta)
{
    double minTimeOut;

    if (timeOutUnit == ACT_TIMEOUT_UNIT_SEC)
    {
        // TimeOut in sec, ConfTime in sec
        if (confTimeUnit == ACT_CONF_TIME_UNIT_SEC) {
            minTimeOut = confTime.toDouble() + delta;
        }
        // TimeOut in sec, ConfTime in msec
        else {
            minTimeOut = confTime.toDouble() / 1000 + delta;
        }
    }
    else
    {
        // TimeOut in msec, ConfTime in sec
        if (confTimeUnit == ACT_CONF_TIME_UNIT_SEC) {
            minTimeOut = (confTime.toDouble() + delta) * 1000;
        }
        // TimeOut in msec, ConfTime in msec
        else {
            minTimeOut = confTime.toDouble() + delta * 1000;
        }
    }

    return minTimeOut;
}

/**
 * @brief. The goal of this function is to give QColor with a QString name color value. 
 * 
 * @param textColor Name of color find in cellule procedure and in file FWC_Database.csv found in GenericDatabase 
 * @return QColor correponding at the string textColor
 */

QColor GTAUtil::getTextColor(const QString &textColor){
    if (textColor == "AMBER") return QColor(255, 191, 0);
    if (textColor == "RED") return QColor(255, 0, 0);
    if (textColor == "GREEN") return QColor(0, 255, 0);
    if (textColor == "WHITE") return QColor(0, 0, 0);
    if (textColor == "AMBER") return QColor(0, 255, 255);
    if (textColor == "BLACK") return QColor(0, 0, 0);
    return QColor(0, 0, 0);
}

/**
 * .
 * @brief This function give the background color of cell. 
 * @param fontColor Name of color corresponding at the frond color in cellule. You should add specific front color for specific color.
 * @return QColor corresponding at the string color give in parameter. 
 */
QColor GTAUtil::getBackGroundColorCellule(const QString &fontColor){
    if (fontColor.isEmpty()) return QColor(255, 255, 255);
    return QColor(0, 0, 0);
}
