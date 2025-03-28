#ifndef GTASVNCONTROLLER_H
#define GTASVNCONTROLLER_H

#include "GTAControllers.h"
#include "GTAElement.h"
#include "GTACommandBase.h"
#include "GTAActionBase.h"

#pragma warning(push, 0)
#include <QObject>
#include <qdir.h>
#include <qprocess.h>
#include <QDomElement>
#include <QDomDocument>
#pragma warning(pop)


struct SvnWcStatus
{
    bool isUnversioned;
    bool isAdded;
    bool isConflict;
    bool isIgnored;
    bool isReplaced;
    bool isModified;
    bool isDeleteScheduled;
    bool isOtherProp;
    bool isNone;
    bool isNormal;
    bool isLocked;
    bool isMissing;
    QString lockComment;
    QString lockOwner;

};

struct SvnRemoteStatus
{
    bool isAdded;
    bool isReplaced;
    bool isModified;
    bool isDeleteScheduled;
    bool isNone;
    bool isLocked;
    QString lockComment;
    QString lockOwner;

};

struct SvnFileStatus
{
    bool isWorkingCopy;
    SvnWcStatus wcStatus;
    SvnRemoteStatus remoteStatus;
};

class GTAControllers_SHARED_EXPORT GTASVNController : public QObject
{
    Q_OBJECT

public:
    //explicit GTASVNController(QObject *parent = 0);
    GTASVNController(QObject *parent = 0);
    ~ GTASVNController();

    /**
      Add the selected file to Tortoise SVN repo
      @ipathToAdd: take the file complete path which need to be added to SVN repo
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnAdd(const QString &ipathToAdd, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode);

    /**
     * Remove the missing file to Tortoise SVN repo
     * @param isFileMissing give sFileStat.wcStatus.isMissing bool state
     * @param completeFilePath take the file complete path which need to be removed to SVN repo
     * @param iSvnUsrCredential List containing user credentials i.e. usrName, password & options to select whether to save credentials or
     *                     for subsequent SVN actions.
     * @param oErrorCode Contains the error message, if any while performing Tortoise SVN action.
    */
    bool GTASVNController::SVNRemoveMissingFiles(bool isFileMissing, QString completeFilePath, const QStringList iSvnUsrCredential, QString& oErrorCode);

    /**
      Commit the selected file to Tortoise SVN repo
      @ipathToCommit: take the file complete path which need to be commited to SVN repo
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnCommit(const QString &ipathToCommit, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode, QString commitMessage);

    /**
      Delete the selected file from Tortoise SVN repo
      @ipathToDelete: take the file complete path which need to be deleted from SVN repo
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnDelete(const QString &ipathToDelete, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode);

    /**
      Update the selected file to Tortoise SVN repo to latest revision
      @ipathToUpdate: take the file complete path which need to be updated to latest revision in SVN repo
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnUpdate(const QString &ipathToUpdate, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode);

    /**
      Lock the selected file in Tortoise SVN repo to stop other users from performing other actions.
      @iFileToLock: take the file complete path which need to be locked in SVN repo
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnLock(const QString &iFileToLock, QString dataDir, const QStringList iSvnUsrCredential,QString &oErrorCode);

    /**
      Unlock the selected locked file in Tortoise SVN
      @iFileToUnlock: take the locked file complete path which need to be unlocked in SVN repo
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnUnLock(const QString &iFileToUnlock, QString dataDir, const QStringList iSvnUsrCredential,QString &oErrorCode);

    /**
      Resolve the conflict of selected file while commiting by giving user options to take changes as per their choice.
      @iPathToResolve: take the file complete path for which conflicts need to be resolved
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @iResolveOption: Gives user the option to take changes as per their choice
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnResolve(const QString & iPathToResolve,QString dataDir,const QStringList iSvnUsrCredential,QString iResolveOption, QString &oErrorCode);

    /**
      Update the complete mapped Tortoise SVN directory
      @dataDir: SVN mapped folder which need to be updated
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnUpdateDir(QString dataDir,const QStringList iSvnUsrCredential,QString &oErrorCode);

    /**
      Execute the framed SVN command
      @iCommandArgs: SVN command argument which need to be executed
      @oCmdOpuput: Process output after executing svn command
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool executeSVNCommand(const QStringList &iCommandArgs, QString& oCmdOpuput, QString& oErrorCode);

    /**
      Clean SVN authorisation from cache by deleting SVN Simple file
      @gtaProcPath: Procedure folder path to check whether it is mapped to SVN repo
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool cleanSVNauthenticationData(const QString gtaProcPath, QString &oErrorCode);


    /**
      Revert the selected file to previous revision in Tortoise SVN
      @iFileToRevert: take the file complete path which need to be reverted in SVN repo
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnRevert(const QString &iFileToRevert, QString dataDir, const QStringList iSvnUsrCredential, QString &oErrorCode);

    /**
      Check for modification of svn GTA scripts
      @ipathToCheck: path of the file for which check for modification is requested
      @pElem: GTAElement of ipathToCheck
      @pElemTempFile: GTAElement of temp file(svn copy of ipathTocheck file)
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @tempDir: Folder in which svn exported file and temporary .txt (text files of working copy and previous version files) files are present.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    bool svnCheckForModification(const QString &ipathToCheck, const GTAElement *pElem, const GTAElement *pElemTempFile, QString dataDir, QString tempDir, const QStringList iSvnUsrCredential, QString &oErrorCode);

    /**
      Exporting a file from svn with revision number
      @ipathToCheck: path of the file for which check for modification is requested
      @dataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @tempDir: Folder in which svn exported file and temporary .txt (text files of working copy and previous version files) files are present.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
    */
    QString exportSVNFile(const QString &ipathToCheck, QString dataDir,QString tempDir, QString &oErrorCode, const QStringList iSvnUsrCredential);

private:

    bool executeSVNMergeCommand(const QStringList &iCommandArgs, QString &oCmdOpuput,QString &oErrorCode);

    /**
      Get the SVN state of particular file/folder
      @iPathToCheck: complete path of file/folder for which SVN status need to be checked.
      @iFileStatus: checked file SVN status
      @oErrorCode: Contains the error message, if any while performing Tortoise SVN action.
      @idataDir: Folder in which the concerned file is present, need to be checked whether it is mapped to SVN repo.
      @iSvnUsrCredential: List containing user credentials i.e. usrName, password & options to select whether to save credentials or
                          for subsequent SVN actions.

    */
    bool getSVNStateOfFileFolder(const QString & iPathToCheck, SvnFileStatus &iFileStatus,QString &oErrorCode, const QString & idataDir, const QStringList iSvnUsrCredential);

    /**
      Clean up if any hook present in the SVN folder
      @idataDir: Folder on which SVN cleaning need to be done
    */
    bool svnCleanUp(const QString & idataDir);

/**
      Cleans up the temporary directory
      itempDir: path of the GTA temporary folder
    */
    void svnTempCleanUp(const QString & itempDir);

signals:

public slots:

};

#endif // GTASVNCONTROLLER_H
