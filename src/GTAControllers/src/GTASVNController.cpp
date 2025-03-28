#pragma warning(disable: 4996) //alow use of getenv (warning issued by MSVC)

#include "GTASVNController.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QDebug>
#pragma warning(pop)

GTASVNController::GTASVNController(QObject *parent) :
    QObject(parent)
{
}

GTASVNController::~GTASVNController(){

}

bool GTASVNController::svnAdd(const QString &ipathToAdd, QString dataDir, const QStringList iSvnUsrCredential,QString &oErrorCode)
{
    bool rc;
    QString usrName,password;
    QString completePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),ipathToAdd));
    QFileInfo fileInfo(completePath);

    if(!fileInfo.isFile() && !fileInfo.isDir())
    {
        oErrorCode = "The selected path is not a file or directory\nCan Not be added.";
        return false;
    }
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completePath, sFileStat, oErrorCode, dataDir, iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode = "The selected path is not a SVN working copy";
            return rc;
        }

        if((sFileStat.wcStatus.isAdded != true) ||(sFileStat.wcStatus.isUnversioned == true))
        {
            QStringList commandArgs;
            usrName = iSvnUsrCredential.first();
            password= iSvnUsrCredential.at(1);
            commandArgs << "add" << "--depth=empty" << "--username" << usrName << "--password" << password << completePath;
            QString output;
            rc = executeSVNCommand(commandArgs,output,oErrorCode);
            if(oErrorCode.isEmpty() == true && rc == true)
            {
                SvnFileStatus sFileStat1;
                rc = getSVNStateOfFileFolder(completePath,sFileStat1,oErrorCode, dataDir,iSvnUsrCredential);
                if(rc && (sFileStat1.wcStatus.isAdded || sFileStat1.remoteStatus.isAdded))
                    rc= true;
                else
                {
                    oErrorCode = "Could not add the file to SVN";
                    rc =false;
                }
            }
            else
            {
                rc =false;
                if(oErrorCode.isEmpty())
                    oErrorCode = "Could not add the file to SVN";
                else
                    oErrorCode.prepend("\nCould not add file to SVN.\n \n");
            }
        }
        else
        {
            rc = false;
            oErrorCode = "File is already added to SVN";
        }
    }
    else
    {
        rc = false;
        oErrorCode.prepend("\nCould not add file to SVN.\n \n");
    }
    svnCleanUp(dataDir);
    return rc;
}

bool GTASVNController::SVNRemoveMissingFiles(bool isFileMissing, QString completeFilePath, const QStringList iSvnUsrCredential, QString& oErrorCode) {
    QStringList commandArgs;
    QString usrName = iSvnUsrCredential.first();
    QString password = iSvnUsrCredential.at(1);
    QString output;
    if (isFileMissing) {
        commandArgs << "rm" << "--username" << usrName << "--password" << password << completeFilePath;
        return executeSVNCommand(commandArgs, output, oErrorCode);
    }
    return false;
}

bool GTASVNController::svnCommit(const QString &ipathToCommit, QString dataDir, const QStringList iSvnUsrCredential,QString &oErrorCode, QString commitMessage)
{
    bool rc = false;
    QStringList commandArgs;
    QString output,usrName,password;
    QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),ipathToCommit));
    QFileInfo info(completeFilePath);
    if(!info.isFile() && info.exists() && !info.isDir())
    {
       oErrorCode = "The selected path is not a file or directory.\nCan Not commit." ;
       return rc;
    }
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completeFilePath, sFileStat, oErrorCode, dataDir, iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode = "The selected path is not a SVN working copy.";
            return rc;
        }
        else if(sFileStat.wcStatus.isAdded != true && sFileStat.wcStatus.isUnversioned == true)
        {
            rc = svnAdd(ipathToCommit,dataDir,iSvnUsrCredential,oErrorCode);
            if(!rc)
            {
                rc = false;
                oErrorCode.prepend("Could not add file to SVN. \n");
                return rc;
            }
            else
                rc = getSVNStateOfFileFolder(completeFilePath,sFileStat,oErrorCode,dataDir, iSvnUsrCredential);
        }
        else if (sFileStat.wcStatus.isAdded == false
            && !sFileStat.wcStatus.isModified
            && !sFileStat.remoteStatus.isModified
            && !sFileStat.wcStatus.isDeleteScheduled
            && !sFileStat.remoteStatus.isDeleteScheduled
            && !sFileStat.wcStatus.isMissing)
        {
            rc = false;
            oErrorCode.prepend("No Modifcations found since last commit. \nSVN commit is not required\n");
            return rc;
        }
        else if(sFileStat.wcStatus.isModified == true && sFileStat.remoteStatus.isModified == true)
        {
            rc =false;
            oErrorCode.prepend("Could not commit the file in SVN.\nA new Version of file is available in SVN.\nPlease perform an update and then commit.\n");
            return rc;
        }
        svnUnLock(completeFilePath, dataDir, iSvnUsrCredential, oErrorCode);
        if (rc)
        {
            usrName = iSvnUsrCredential.first();
            password = iSvnUsrCredential.at(1);
            rc = SVNRemoveMissingFiles(sFileStat.wcStatus.isMissing, completeFilePath, iSvnUsrCredential, oErrorCode);
            commandArgs << "update" << "--username" << usrName << "--password" << password << dataDir;
            rc = executeSVNCommand(commandArgs, output, oErrorCode);
            commandArgs.clear();
            commandArgs << "ci" << "--username" << usrName << "--password" << password << "-m" << commitMessage << completeFilePath;
            rc = executeSVNCommand(commandArgs, output, oErrorCode);
            if (oErrorCode.isEmpty() == true && rc == true)
            {
                SvnFileStatus sFileStat1;
                rc = getSVNStateOfFileFolder(completeFilePath, sFileStat1, oErrorCode, dataDir, iSvnUsrCredential);
                if (rc && !sFileStat1.wcStatus.isModified)
                    rc = true;
                else
                {
                    oErrorCode.prepend("Could not commit the file to SVN. \n");
                    rc = false;
                }
            }
            else
            {
                oErrorCode.prepend("Could not commit the file to SVN. \n");
                rc = false;
            }
        }
    }
    else
    {
        rc = false;
        oErrorCode.prepend("Could not commit the file to SVN. \n");
    }
    svnCleanUp(dataDir);
    return rc;
}

bool GTASVNController::svnUpdate(const QString &ipathToUpdate, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode)
{
    bool rc = false;
    QStringList commandArgs;
    QString output,usrName,password;
    QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),ipathToUpdate));
    QFileInfo fileInfo(completeFilePath);
    if(!fileInfo.isFile())
        return rc;
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completeFilePath, sFileStat, oErrorCode, dataDir, iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode.prepend("The selected path is not a SVN working copy.\n");
            rc= false;
            return rc;
        }
        else if(sFileStat.wcStatus.isUnversioned == true)
        {
            oErrorCode.prepend("The Selected file is not under version control.\n");
            rc = false;
            return rc;
        }
        else if((sFileStat.wcStatus.isModified == true) && (sFileStat.remoteStatus.isModified == true))
        {
            oErrorCode.prepend("File has been modified in the SVN repository and local as well.\nThere are chances of Conflict.\nPlease Perform Check for modification\n or perform run Resolve Conflict option.\n");
            rc = false;
            return rc;
        }
        else if((sFileStat.wcStatus.isModified == true) || (sFileStat.remoteStatus.isModified == true))
        {
            usrName = iSvnUsrCredential.first();
            password = iSvnUsrCredential.at(1);
            commandArgs<<"update"<<"--username"<<usrName<<"--password"<<password<<completeFilePath;
            rc = executeSVNCommand(commandArgs,output,oErrorCode);
            if(oErrorCode.isEmpty() == true && rc == true)
            {
                SvnFileStatus sFileStat1;
                rc = getSVNStateOfFileFolder(completeFilePath,sFileStat1,oErrorCode,dataDir,iSvnUsrCredential);
                if(!rc)
                {
                    oErrorCode.prepend("Could not update file from SVN.\n");
                    rc = false;
                }
            }
            else
            {
                oErrorCode.prepend("Could not update file from SVN.\n");
                rc = false;
            }
        }
    }
    else
    {
        rc = false;
        oErrorCode.prepend("Could not update file from SVN.\n");
    }
    svnCleanUp(dataDir);
    return rc;
}

bool GTASVNController::svnLock(const QString &iFileToLock, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode)
{
    bool rc = false;
    QStringList commandArgs;
    QString output,usrName,password;
    QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),iFileToLock));
    QFileInfo fileInfo(completeFilePath);
    if(!fileInfo.isFile())
        return rc;
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completeFilePath, sFileStat, oErrorCode, dataDir, iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode.prepend("The selected path is not a SVN working copy.\n");
            rc= false;
            return rc;
        }
        else if(sFileStat.wcStatus.isUnversioned == true)
        {
            oErrorCode.prepend("The Selected file is not under version control.\n");
            rc= false;
            return rc;
        }
        else if((sFileStat.wcStatus.isLocked == true) || (sFileStat.remoteStatus.isLocked == true))
        {
            oErrorCode.prepend("The Selected file is already locked and can not be locked again.\n");
            rc= false;
            return rc;
        }
        usrName= iSvnUsrCredential.first();
        password = iSvnUsrCredential.at(1);

        commandArgs<<"lock"<<"--force"<<"--username"<<usrName<<"--password"<<password<<completeFilePath;
        rc = executeSVNCommand(commandArgs,output, oErrorCode);
        if(oErrorCode.isEmpty() == true && rc == true)
        {
            SvnFileStatus sFileStat1;
            rc = getSVNStateOfFileFolder(completeFilePath,sFileStat1,oErrorCode,dataDir,iSvnUsrCredential);
            if(rc && sFileStat1.wcStatus.isLocked)
                rc = true;
            else
            {
                oErrorCode.prepend("Could not Lock the file in SVN.\n");
                rc= false;
            }
        }
        else
        {
            oErrorCode.prepend("Could not Lock the file in SVN.\n");
            rc= false;
        }
    }
    else
    {
        rc = false;
        oErrorCode.prepend("Could not Lock the file in SVN.\n");
    }
    svnCleanUp(dataDir);
    return rc;
}

bool GTASVNController::svnUnLock(const QString &iFileToUnlock, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode)
{
    bool rc = false;
    QStringList commandArgs;
    QString output,usrName,password;
    QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),iFileToUnlock));
    QFileInfo fileInfo(completeFilePath);
    if(!fileInfo.isFile())
        return rc;
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completeFilePath, sFileStat, oErrorCode, dataDir, iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode.prepend("The selected path is not a SVN working copy.\n");
            rc= false;
            return rc;
        }
        else if(sFileStat.wcStatus.isUnversioned == true)
        {
            oErrorCode.prepend("The Selected file is not under version control.\n");
            rc= false;
            return rc;
        }
        else if((sFileStat.wcStatus.isLocked == false) && (sFileStat.remoteStatus.isLocked == false))
        {
            oErrorCode.prepend("The Selected file is already unlocked.\n");
            rc= false;
            return rc;
        }
        else if(sFileStat.remoteStatus.isLocked == true && sFileStat.wcStatus.isLocked == false)
        {
            oErrorCode = QString("The Selected file is locked by other user(%1) and can not be unlocked.").arg(sFileStat.remoteStatus.lockOwner);
            rc= false;
            return rc;
        }
        usrName = iSvnUsrCredential.first();
        password = iSvnUsrCredential.at(1);

        commandArgs<<"unlock"<<"--username"<<usrName<<"--password"<<password<<completeFilePath;
        rc = executeSVNCommand(commandArgs,output,oErrorCode);
        if(oErrorCode.isEmpty() == true && rc == true)
        {
            SvnFileStatus sFileStat1;
            rc = getSVNStateOfFileFolder(completeFilePath,sFileStat1,oErrorCode,dataDir, iSvnUsrCredential);
            if(rc && !sFileStat1.wcStatus.isLocked && !sFileStat1.remoteStatus.isLocked)
                rc = true;
            else
            {
                oErrorCode.prepend("Could not Unlock the file in SVN.\n");
                rc =false;
            }
        }
        else
        {
            oErrorCode.prepend("Could not Unlock the file in SVN.\n");
            rc =false;
        }
    }
    else
    {
        rc = false;
        oErrorCode.prepend("Could not Unlock the file in SVN.\n");
    }
    svnCleanUp(dataDir);
    return rc;
}

bool GTASVNController::svnDelete(const QString &ipathToDelete, QString dataDir, const QStringList iSvnUsrCredential,QString &oErrorCode)
{
    bool rc = false;
    QString usrName,password;
    QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),ipathToDelete));
    QFileInfo info(completeFilePath);
    if(!info.isFile()&& info.exists() && !info.isDir())
    {
        oErrorCode = "The selected path is not a file or directory.\nCannot delete.";
        return rc;
    }
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completeFilePath,sFileStat,oErrorCode,dataDir,iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode = "The selected path is not a SVN working copy.";
            return rc;
        }
        else
        {
            if((sFileStat.wcStatus.isLocked != true) && (sFileStat.remoteStatus.isLocked != true))
            {
                QStringList commandArgs;
                QString output;
                usrName = iSvnUsrCredential.first();
                password = iSvnUsrCredential.at(1);

                commandArgs<<"delete"<<"--username"<<usrName<<"--password"<<password<<completeFilePath;
                rc = executeSVNCommand(commandArgs,output,oErrorCode);
                if(oErrorCode.isEmpty() == true && rc == true)
                {
                    QFileInfo fileInfo(completeFilePath);
                    if(fileInfo.exists())
                    {
                        rc = false;
                        oErrorCode.prepend("Could not delete the file from SVN. \n");
                    }
                    else
                        rc = true;
                }
                else
                {
                    rc = false;
                    oErrorCode.prepend("Could not delete the file from SVN. \n");
                }
            }
            else
            {
                rc = false;
                if(sFileStat.wcStatus.isLocked == true)
                    oErrorCode = "File is locked in SVN, and can not be modified.\nPlease Unlock the File to perform any modifications";
                else if(sFileStat.remoteStatus.isLocked == true)
                {
                    QString user = sFileStat.remoteStatus.lockOwner;
                    QString comment = sFileStat.remoteStatus.lockComment;
                    if(!comment.isEmpty())
                        oErrorCode = QString("File is locked in SVN by other user(%1), reason(%2), and can not be modified.\nPlease Request the user to unlock the file for any modifications.").arg(user,comment);
                    else
                        oErrorCode = QString("File is locked in SVN by other user(%1), and can not be modified.\nPlease Request the user to unlock the file for any modifications.").arg(user);
                }
            }
        }
    }
    else
    {
        rc = false;
        oErrorCode.prepend("Could not delete the file from SVN. \n");
    }
    svnCleanUp(dataDir);
    return rc;
}

bool GTASVNController::svnRevert(const QString &iFileToRevert, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode)
{
    bool rc = false;
    QString usrName,password;
    QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),iFileToRevert));
    QFileInfo info(completeFilePath);
    if(!info.isFile()&& info.exists())
    {
        oErrorCode = "The selected path is not a file\nCan not revert.";
        return rc;
    }
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completeFilePath,sFileStat,oErrorCode,dataDir,iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode = "The selected path is not a SVN working copy.";
            return rc;
        }
        else
        {
            if((sFileStat.wcStatus.isModified == true) || sFileStat.remoteStatus.isModified == true)
            {
                QStringList commandArgs;
                QString output;
                usrName = iSvnUsrCredential.first();
                password = iSvnUsrCredential.at(1);

                commandArgs<<"revert"<<"--username"<<usrName<<"--password"<<password<<completeFilePath;
                rc = executeSVNCommand(commandArgs,output,oErrorCode);

                if(oErrorCode.isEmpty()==true && rc == true)
                {
                    SvnFileStatus sFileStat1;
                    rc = getSVNStateOfFileFolder(completeFilePath,sFileStat1,oErrorCode,dataDir,iSvnUsrCredential);
                    if(rc && !sFileStat1.wcStatus.isModified && !sFileStat1.remoteStatus.isModified)
                        rc = true;
                    else
                    {
                        oErrorCode.prepend("Could not revert the file.\n");
                        rc = false;
                    }
                }
                else
                {
                    oErrorCode.prepend("Could not revert the file.\n");
                    rc =false;
                }

            }
            else
            {
                oErrorCode.prepend("Can't revert the file, since it's not modified \n");
                rc =false;
            }
        }
    }
    else
    {
        oErrorCode.prepend("Could not revert the file.\n");
        rc =false;
    }
    svnCleanUp(dataDir);
    return rc;
}

bool GTASVNController::svnResolve(const QString &iPathToResolve, QString dataDir, const QStringList iSvnUsrCredential, QString iResolveOption, QString &oErrorCode)
{
    bool rc = false;
    QString usrName,password;
    QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),iPathToResolve));
    QFileInfo info(completeFilePath);
    if(!info.isFile()&& info.exists())
    {
        oErrorCode = "The selected path is not a file\nCan Not resolve.";
        return rc;
    }
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completeFilePath,sFileStat,oErrorCode,dataDir,iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode = "The selected path is not a SVN working copy.";
            rc= false;
            return rc;
        }
        else if(sFileStat.wcStatus.isUnversioned == true)
        {
            oErrorCode = "The Selected file is not under version control.";
            rc = false;
            return rc;
        }
        //if the file is not revisioned/added already then add the file using hte svn command
        else if(sFileStat.wcStatus.isModified^sFileStat.remoteStatus.isModified)
        {
            if(sFileStat.wcStatus.isModified)
                oErrorCode = "File has been modified locally, but not on server, No need to perform resolve operation.";
            if(sFileStat.remoteStatus.isModified)
                oErrorCode = "File has been modified on server, but not on local, No need to perform resolve operation.";
            rc = false;
            return rc;
        }
        else if((sFileStat.wcStatus.isModified == true) && (sFileStat.remoteStatus.isModified == true))
        {
            QStringList commandArgs;
            QString output;
            usrName = iSvnUsrCredential.first();
            password = iSvnUsrCredential.at(1);

            commandArgs<<"update"<<"--accept"<<iResolveOption<<"--username"<<usrName<<"--password"<<password<<completeFilePath;
            rc = executeSVNCommand(commandArgs,output,oErrorCode);
            if(oErrorCode.isEmpty() == true && rc == true)
            {
                SvnFileStatus sFileStat1;
                rc = getSVNStateOfFileFolder(completeFilePath,sFileStat1,oErrorCode,dataDir,iSvnUsrCredential);
                if(rc)
                {
                    oErrorCode.prepend("Could not resolve file from SVN.\n");
                    rc =false;
                }
            }
            else
            {
                rc =false;
                oErrorCode.prepend("Could not resolve file from SVN.\n");
            }
        }
    }
    else
    {
        rc = false;
        oErrorCode.prepend("Could not resolve file from SVN.\n");
    }
    svnCleanUp(dataDir);
    return rc;
}

bool GTASVNController::svnUpdateDir(QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode)
{
    bool rc;
    QString output, usrName,password;
    QString completeDirectoryPath = QDir::cleanPath(QString("%1").arg(dataDir));
    QStringList commandArgs;
    usrName = iSvnUsrCredential.first();
    password = iSvnUsrCredential.at(1);
    commandArgs<<"update"<<"--force"<<"--username"<<usrName<<"--password"<<password<<completeDirectoryPath;
    rc = executeSVNCommand(commandArgs,output,oErrorCode);
    svnCleanUp(dataDir);

    return rc;
}


//bool GTASVNController::svnCheckForModification(const QString &ipathToCheck, QString dataDir, QString tempDir, const QStringList iSvnUsrCredential,QString &oErrorCode)
bool GTASVNController::svnCheckForModification(const QString &ipathToCheck, const GTAElement *pElem, const GTAElement *pElemTempFile, QString dataDir, QString tempDir, const QStringList iSvnUsrCredential, QString &oErrorCode)
{
    bool rc = false;
    QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),ipathToCheck));

    QFileInfo info(completeFilePath);
    if(!info.isFile()&& info.exists())
    {
        oErrorCode = "The selected path is not a file\nCan Not check for modifications.";
        return rc;
    }
    SvnFileStatus sFileStat;
    rc = getSVNStateOfFileFolder(completeFilePath,sFileStat,oErrorCode,dataDir,iSvnUsrCredential);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        if(!sFileStat.isWorkingCopy)
        {
            oErrorCode = "The selected path is not a SVN working copy.";
            rc= false;
            svnTempCleanUp(tempDir);
            return rc;
        }
        else if(sFileStat.wcStatus.isUnversioned == true)
        {
            oErrorCode = "The Selected file is not under version control.";
            rc = false;
            svnTempCleanUp(tempDir);
            return rc;
        }
        else if((sFileStat.wcStatus.isModified == false) && (sFileStat.remoteStatus.isModified == false))
        {
            oErrorCode = "The Selected file is Unmodified.";
            rc =false;
            svnTempCleanUp(tempDir);
            return rc;
        }
        else
        {


            QString workingFile = QDir::cleanPath(QString("%1%2WorkingCopy.txt").arg(tempDir,QDir::separator()));

            QFile file(workingFile);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream in(&file);
            QString cmdStatement;
            for(int i=0;i<pElem->getDirectChildrenCount();i++)
            {
                GTACommandBase * cmd;
                pElem->getDirectChildren(i,cmd);
                cmdStatement = cmd->getStatement();
                GTAActionBase * pActCmd = dynamic_cast<GTAActionBase *>(cmd);
                if((pActCmd != NULL) && (pActCmd->getAction() == ACT_PRINT) && (pActCmd->getComplement() == COM_PRINT_MSG))
                {
                    in<<"<ACT_PRINT>";
                    cmdStatement.remove("print message to execution log: ");
                }
                cmdStatement.replace("\n","<NEXTLINE>");
                in<<cmdStatement;
                if(cmd->isTitle())
                    in<<"<TITLE>";
                if(cmd->isIgnoredInSCXML())
                    in<<"<isIgnoredInSCXML>";

                GTACommandBase::CommandType cmdType = cmd->getCommandType();
                if (cmdType==GTACommandBase::CHECK)
                    in<<"<CHECK>";



                in<<endl;
                if(cmd->canHaveChildren())
                {
                    QList<GTACommandBase*> children = cmd->getChildren();
                    for (auto* child : children)
                    {
                        cmdStatement = child->getStatement();
                        GTAActionBase* pSubActCmd = dynamic_cast<GTAActionBase *>(child);
                        if((pSubActCmd != NULL) && (pSubActCmd->getAction() == ACT_PRINT) && (pSubActCmd->getComplement() == COM_PRINT_MSG))
                        {
                            in<<"<ACT_PRINT>";
                            cmdStatement.remove("print message to execution log: ");
                        }
                        cmdStatement.replace("\n","<NEXTLINE>");
                        in<<cmdStatement;
                        if(child->isTitle())
                            in<<"<TITLE>";
                        if(child->isIgnoredInSCXML())
                        {
                            in<<"<isIgnoredInSCXML>";
                        }

                        GTACommandBase::CommandType subCmdType = child->getCommandType();
                        if (subCmdType ==GTACommandBase::CHECK)
                            in<<"<CHECK>";

                        in<<endl;
                    }
                }


            }
            file.close();

            QString PreviousFile = QDir::cleanPath(QString("%1%2PreviousVersionCopy.txt").arg(tempDir,QDir::separator()));

            QFile prevSVNfile(PreviousFile);
            prevSVNfile.open(QIODevice::ReadWrite | QIODevice::Text);
            QTextStream input(&prevSVNfile);
            for(int i=0 ; i < pElemTempFile->getDirectChildrenCount(); i++)
            {
                GTACommandBase * cmd;
                pElemTempFile->getDirectChildren(i,cmd);
                if (cmd != NULL)
                {
                    cmdStatement = cmd->getStatement();
                    GTAActionBase * pActCmd = dynamic_cast<GTAActionBase *>(cmd);
                    if((pActCmd != NULL) && (pActCmd->getAction() == ACT_PRINT) && (pActCmd->getComplement() == COM_PRINT_MSG))
                    {
                        input<<"<ACT_PRINT>";
                        cmdStatement.remove("print message to execution log: ");
                    }
                    cmdStatement.replace("\n","<NEXTLINE>");
                    input<<cmdStatement;
                    if(cmd->isTitle())
                    {
                        input<<"<TITLE>";
                    }
                    if(cmd->isIgnoredInSCXML())
                    {
                        input<<"<isIgnoredInSCXML>";
                    }

                    GTACommandBase::CommandType cmdType = cmd->getCommandType();
                    if (cmdType==GTACommandBase::CHECK)
                    {
                        input<<"<CHECK>";
                    }

                    input<<endl;
                    if(cmd->canHaveChildren())
                    {
                        QList<GTACommandBase*> children = cmd->getChildren();
                        for(auto *child : children)
                        {
                            cmdStatement = child->getStatement();
                            GTAActionBase * pSubActCmd = dynamic_cast<GTAActionBase *>(child);
                            if((pSubActCmd != NULL) && (pSubActCmd->getAction() == ACT_PRINT) && (pSubActCmd->getComplement() == COM_PRINT_MSG))
                            {
                                input<<"<ACT_PRINT>";
                                cmdStatement.remove("print message to execution log: ");
                            }
                            cmdStatement.replace("\n","<NEXTLINE>");
                            input<<cmdStatement;
                            if(child->isTitle())
                            {
                                input<<"<TITLE>";
                            }
                            if(child->isIgnoredInSCXML())
                                input<<"<isIgnoredInSCXML>";

                            GTACommandBase::CommandType subCmdType = child->getCommandType();
                            if (subCmdType ==GTACommandBase::CHECK)
                                input<<"<CHECK>";
                            input<<endl;
                        }
                    }
                }
            }
            prevSVNfile.close();
            QStringList commandArgs;
            QString oOutput;
            PreviousFile = "--old="+PreviousFile;
            workingFile = "--new="+workingFile;

            commandArgs<<"diff"<<PreviousFile<<workingFile;
            rc  = executeSVNCommand(commandArgs,oOutput,oErrorCode);
            if(rc && oErrorCode.isEmpty())
            {
                qDebug()<<oErrorCode;
                oErrorCode.clear();
                oErrorCode.append("<SVN Check For Modifications SUCCESS:>\n");
                oErrorCode.append(oOutput);
                svnTempCleanUp(tempDir);
                return rc;
            }
            else
            {
                oErrorCode.clear();
                oErrorCode.append("Could not perform SVN Check for modification .\n");
                rc = false;
            }


        }

    }

    svnTempCleanUp(tempDir);
    svnCleanUp(dataDir);
    return rc;
}

QString GTASVNController::exportSVNFile(const QString &ipathToCheck, QString dataDir, QString tempDir, QString &oErrorCode, const QStringList iSvnUsrCredential)
{
    QStringList commandArgs;
    QString output;
    QString fileName = ipathToCheck;
     QString completeFilePath = QDir::cleanPath(QString("%1%2%3").arg(dataDir,QDir::separator(),ipathToCheck));
    commandArgs<<"status"<<"-u"<<"--username"<<iSvnUsrCredential.at(0)<<"--password"<<iSvnUsrCredential.at(1)<<completeFilePath;
    bool rc =false;
    rc = executeSVNCommand(commandArgs,output,oErrorCode);
    if(!output.isEmpty() && oErrorCode.isEmpty() == true && rc == true)
    {
        QString rev;
        output = output.replace(" ",">");
        QStringList items = output.split("\n",QString::SkipEmptyParts);
        for(int i=0;i<items.count();i++)
        {
            QString val = items.at(i);
            if(val.startsWith("Status>against>revision:"))
            {
                QStringList vals = val.split(":",QString::SkipEmptyParts);
                if(vals.count()>1)
                {
                    rev = vals.last();
                    rev = rev.trimmed();
                    rev = rev.remove(">");
                    break;
                }
            }
        }
        if(!rev.isEmpty())
        {
            QString tempFileName = QDir::cleanPath(QString("%1%2%4_%3").arg(tempDir,QDir::separator(),fileName.remove("/"),rev));
            QString tempDirectory = QDir::cleanPath(QString("%1%2").arg(tempDir,QDir::separator()));
            QFile tempFile(tempFileName);
            if(tempFile.exists())
                rc = tempFile.remove(tempFileName);
            QDir().mkdir(tempDirectory);
            commandArgs.clear();
            commandArgs <<"export"<<"--username"<<iSvnUsrCredential.at(0)<<"--password"<<iSvnUsrCredential.at(1)<<"--force"<<"-r"<<rev<<completeFilePath<<tempFileName;
            rc = executeSVNCommand(commandArgs,output,oErrorCode);
            if(rc)
            {
                //                commandArgs.clear();
                //                commandArgs<<tempFileName<<completeFilePath;
                //                QString out;
                //                rc = executeSVNMergeCommand(commandArgs,out,oErrorCode);
                return tempFileName;
            }
            //QDir().rmdir(tempDirectory);
        }
        else
        {
            if(!rc)
            {
                oErrorCode = "Could not be checked for modification.\n"+oErrorCode+"\nCommand Failed.";
            }
        }
    }
    else
    {
        rc = false;
        oErrorCode.prepend("Could not perform SVN Check for modification .\n");
    }
    return QString();
}
bool GTASVNController::executeSVNMergeCommand(const QStringList &iCommandArgs, QString &oCmdOpuput, QString &)
{
    bool rc=true;
    QProcess myProcess;
    myProcess.start("tortoisemerge",iCommandArgs);
    rc= myProcess.waitForFinished(-1);
    QString output = myProcess.readAllStandardOutput();
    if(output.isEmpty())
        oCmdOpuput = myProcess.readAllStandardError();
    else
        oCmdOpuput = output.replace(" ",">").trimmed();

    return rc;
}

bool GTASVNController::cleanSVNauthenticationData(const QString gtaProcPath, QString &oErrorCode)
{
    bool rc, deleteFile = true;
    QString svnRepo, svnCachePath, oCmdOpuput;
    QStringList commandArgs;
    commandArgs<<"info"<<"--xml"<<gtaProcPath;
    rc = executeSVNCommand(commandArgs,oCmdOpuput,oErrorCode);
    if(rc == true && oErrorCode.isEmpty() == true)
    {
        QDomDocument domDoc;
        rc = domDoc.setContent(oCmdOpuput);
        if(!rc)
            return false;
        QDomElement rootElem = domDoc.documentElement();
        QDomNodeList currentStatusNodeList = rootElem.elementsByTagName("root");
        QDomElement currentStatusElemNode;
        if(!currentStatusNodeList.isEmpty())
            currentStatusElemNode = currentStatusNodeList.at(0).toElement();
        if(!currentStatusElemNode.isNull())
            svnRepo = currentStatusElemNode.text();
    }
    QString RepoName = svnRepo.mid(svnRepo.lastIndexOf("/"), (svnRepo.length() - svnRepo.lastIndexOf("/")));

    svnCachePath = std::getenv("APPDATA");
    svnCachePath.append("/Subversion/auth/svn.simple");
    QDir svnAuthPath(svnCachePath);
    if(svnAuthPath.exists())
    {
        QStringList fileList = svnAuthPath.entryList(QStringList(),QDir::Files);
        QStringList fileContent;
        QString filePath;
        foreach(QString file, fileList)
        {
            filePath = svnCachePath + "/" + file;
            filePath = QDir::cleanPath(filePath);
            QFile authFile(filePath);
            if (!authFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
               qDebug()<<"Not able to open the file";
               return false;
            }
            while(!authFile.atEnd())
                fileContent.append(authFile.readLine().trimmed());
            qint16 svnRepoIndex = static_cast<qint16>(fileContent.indexOf(QRegExp("^<https://.+")));
            
			QString svnRepoName;

			//Added BVU 
			if (svnRepoIndex != -1)
			{
				QString simpleFileSvnRepo = fileContent.at(svnRepoIndex);
				svnRepoName = simpleFileSvnRepo.mid(simpleFileSvnRepo.lastIndexOf("/"), (simpleFileSvnRepo.indexOf(" realm") - simpleFileSvnRepo.lastIndexOf("/")));
			}
			
			//QString simpleFileSvnRepo = fileContent.at(svnRepoIndex);
            //QString svnRepoName = simpleFileSvnRepo.mid(simpleFileSvnRepo.lastIndexOf("/"), (simpleFileSvnRepo.indexOf(" realm") - simpleFileSvnRepo.lastIndexOf("/")));

            if(RepoName == svnRepoName)
            {
                deleteFile = authFile.remove();
                filePath = QString();
            }
            else
            {
                authFile.close();
                filePath = QString();
            }
            fileContent.clear();
        }
    }
    if(deleteFile)
        return true;
    else
        return false;
}


bool GTASVNController::getSVNStateOfFileFolder(const QString &iPathToCheck, SvnFileStatus &iFileStatus, QString &oErrorCode, const QString &idataDir, const QStringList iSvnUsrCredential)
{
    bool rc = false;
    QString usrName,password;
    QStringList commandArgs;
    QString output = QString();

    iFileStatus.remoteStatus.isAdded = false;
    iFileStatus.remoteStatus.isDeleteScheduled = false;
    iFileStatus.remoteStatus.isModified = false;
    iFileStatus.remoteStatus.isNone = false;
    iFileStatus.remoteStatus.isReplaced = false;
    iFileStatus.remoteStatus.isLocked = false;
    iFileStatus.wcStatus.isAdded = false;
    iFileStatus.wcStatus.isConflict = false;
    iFileStatus.wcStatus.isDeleteScheduled = false;
    iFileStatus.wcStatus.isIgnored = false;
    iFileStatus.wcStatus.isModified = false;
    iFileStatus.wcStatus.isNone = false;
    iFileStatus.wcStatus.isNormal = false;
    iFileStatus.wcStatus.isReplaced = false;
    iFileStatus.wcStatus.isUnversioned = false;
    iFileStatus.wcStatus.isLocked = false;
    iFileStatus.wcStatus.isMissing = false;
    iFileStatus.wcStatus.isOtherProp = false;

    usrName = iSvnUsrCredential.first();
    password= iSvnUsrCredential.at(1);

    commandArgs<<"status"<<"--xml"<<"--username"<<usrName<<"--password"<<password<<"-u"<<iPathToCheck;
    rc = executeSVNCommand(commandArgs, output, oErrorCode);

    if(!oErrorCode.isEmpty())
        return false;

    if(!output.contains("is not a working copy"))
        iFileStatus.isWorkingCopy = true;
    else
    {
        iFileStatus.isWorkingCopy = false;
        return  rc;
    }

    if(output.contains("Failed to create new lock"))
    {
        iFileStatus.wcStatus.isLocked = false;
        iFileStatus.remoteStatus.isLocked = false;
        return rc;
    }

    QDomDocument domDoc;
    rc = domDoc.setContent(output);

    if(!rc)
        return false;

    QDomElement rootElem = domDoc.documentElement();
    QDomNodeList currentStatusNodeList = rootElem.elementsByTagName(SVN_WC_STATUS);
    QDomNodeList remoteStatusNodeList = rootElem.elementsByTagName(SVN_REMOTE_STATUS);
    QDomElement currentStatusElemNode;
    QDomElement remoteStatusElemNode;
    if(!currentStatusNodeList.isEmpty())
        currentStatusElemNode  = currentStatusNodeList.at(0).toElement();
    if(!remoteStatusNodeList.isEmpty())
        remoteStatusElemNode  = remoteStatusNodeList.at(0).toElement();


    if(!currentStatusElemNode.isNull())
    {

        QDomNode itemAttr = currentStatusElemNode.attributes().namedItem(SVN_ITEM_ATTR);
        if(!itemAttr.isNull())
        {
            QString itemVal = itemAttr.nodeValue();

            if (itemVal == SVN_WC_STATUS_MISSING)
                iFileStatus.wcStatus.isMissing = true;
            else
                iFileStatus.wcStatus.isMissing = false;

            if(itemVal == SVN_WC_STATUS_UNVERSIONED)
                iFileStatus.wcStatus.isUnversioned = true;
            else
                iFileStatus.wcStatus.isUnversioned = false;

            if((itemVal == SVN_WC_STATUS_EXTERNAL) || (itemVal == SVN_WC_STATUS_INCOMPLETE) || (itemVal == SVN_WC_STATUS_OBSTRUCTED))
                iFileStatus.wcStatus.isOtherProp = true;
            else
                iFileStatus.wcStatus.isOtherProp = false;

            if(itemVal == SVN_WC_STATUS_IGNORED)
                iFileStatus.wcStatus.isIgnored = true;
            else
                iFileStatus.wcStatus.isIgnored = false;

            if(itemVal == SVN_WC_STATUS_CONFLICTED)
                iFileStatus.wcStatus.isConflict = true;
            else
                iFileStatus.wcStatus.isConflict = false;

            if(itemVal == SVN_WC_STATUS_DELETED)
                iFileStatus.wcStatus.isDeleteScheduled = true;
            else
                iFileStatus.wcStatus.isDeleteScheduled = false;

            if(itemVal == SVN_WC_STATUS_NONE)
                iFileStatus.wcStatus.isNone = true;
            else
                iFileStatus.wcStatus.isNone = false;

            if(itemVal == SVN_WC_STATUS_ADDED)
                iFileStatus.wcStatus.isAdded= true;
            else
                iFileStatus.wcStatus.isAdded = false;

            if(itemVal == SVN_WC_STATUS_REPLACED)
                iFileStatus.wcStatus.isReplaced= true;
            else
                iFileStatus.wcStatus.isReplaced = false;

            if(itemVal == SVN_WC_STATUS_NORMAL)
                iFileStatus.wcStatus.isNormal = true;
            else
                iFileStatus.wcStatus.isNormal = false;

            if(itemVal == SVN_WC_STATUS_MODIFIED)
                iFileStatus.wcStatus.isModified = true;
            else
                iFileStatus.wcStatus.isModified = false;

            QDomNodeList lockNodeList = currentStatusElemNode.elementsByTagName(SVN_LOCK_TYPE_NODE);
            if(!lockNodeList.isEmpty())
            {
                QDomNode lockNode = lockNodeList.at(0);
                if(!lockNode.isNull())
                {
                    iFileStatus.wcStatus.isLocked = true;
                    QDomNode commentNode = lockNode.namedItem(SVN_LOCK_TYPE_COMMENT);
                    QDomNode ownerNode = lockNode.namedItem(SVN_LOCK_TYPE_OWNER);

                    if(!commentNode.isNull())
                    {
                        QDomNodeList cmntList = commentNode.childNodes();
                        if(!cmntList.isEmpty())
                        {
                            iFileStatus.wcStatus.lockComment = cmntList.at(0).nodeValue();
                        }
                    }
                    if(!ownerNode.isNull())
                    {
                        QDomNodeList ownerList = ownerNode.childNodes();
                        if(!ownerList.isEmpty())
                        {
                            iFileStatus.wcStatus.lockOwner = ownerList.at(0).nodeValue();
                        }
                    }
                }
            }
        }
    }

    if(!remoteStatusElemNode.isNull())
    {
        QDomNode itemAttr = remoteStatusElemNode.attributes().namedItem(SVN_ITEM_ATTR);
        if(!itemAttr.isNull())
        {
            QString itemVal = itemAttr.nodeValue();

            if(itemVal == SVN_WC_STATUS_DELETED)
                iFileStatus.remoteStatus.isDeleteScheduled = true;
            else
                iFileStatus.remoteStatus.isDeleteScheduled = false;

            if(itemVal == SVN_WC_STATUS_NONE)
                iFileStatus.remoteStatus.isNone = true;
            else
                iFileStatus.remoteStatus.isNone = false;

            if(itemVal == SVN_WC_STATUS_ADDED)
                iFileStatus.remoteStatus.isAdded= true;
            else
                iFileStatus.remoteStatus.isAdded = false;

            if(itemVal == SVN_WC_STATUS_REPLACED)
                iFileStatus.remoteStatus.isReplaced= true;
            else
                iFileStatus.remoteStatus.isReplaced = false;

            if(itemVal == SVN_WC_STATUS_MODIFIED)
                iFileStatus.remoteStatus.isModified = true;
            else
                iFileStatus.remoteStatus.isModified = false;

            QDomNodeList lockNodeList = remoteStatusElemNode.elementsByTagName(SVN_LOCK_TYPE_NODE);

            if(!lockNodeList.isEmpty())
            {
                QDomNode lockNode = lockNodeList.at(0);
                if(!lockNode.isNull())
                {
                    iFileStatus.remoteStatus.isLocked = true;
                    QDomNode commentNode = lockNode.namedItem(SVN_LOCK_TYPE_COMMENT);
                    QDomNode ownerNode = lockNode.namedItem(SVN_LOCK_TYPE_OWNER);


                    if(!commentNode.isNull())
                    {
                        QDomNodeList cmntList = commentNode.childNodes();
                        if(!cmntList.isEmpty())
                        {
                            iFileStatus.remoteStatus.lockComment = cmntList.at(0).nodeValue();
                        }
                    }
                    if(!ownerNode.isNull())
                    {
                        QDomNodeList ownerList = ownerNode.childNodes();
                        if(!ownerList.isEmpty())
                        {
                            iFileStatus.remoteStatus.lockOwner = ownerList.at(0).nodeValue();
                        }
                    }
                }
            }
        }
    }
    svnCleanUp(idataDir);
    return rc;
}

bool GTASVNController::executeSVNCommand(const QStringList& iCommandArgs, QString& oCmdOpuput, QString& oErrorCode)
{
    bool rc = false;
    QProcess myProcess;
    myProcess.start("svn", iCommandArgs);
    rc = myProcess.waitForFinished();
    oCmdOpuput = myProcess.readAllStandardOutput();
    oErrorCode = myProcess.readAllStandardError();
    return rc;
}

bool GTASVNController::svnCleanUp(const QString & idataDir)
{
    bool rc = false;
    QString output;
    QStringList commandArgs;
    commandArgs<<"cleanup"<<"--non-interactive"<<idataDir;
    QString error;
    rc = executeSVNCommand(commandArgs,output, error);
    return rc;
}

void GTASVNController::svnTempCleanUp(const QString &itempDir)
{
    QDir dir(itempDir);
    dir.setNameFilters(QStringList() << "*.*");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
}
