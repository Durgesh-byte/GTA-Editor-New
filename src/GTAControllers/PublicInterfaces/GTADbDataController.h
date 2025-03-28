#ifndef GTADBDATACONTROLLER_H
#define GTADBDATACONTROLLER_H

#include "GTATreeItem.h"

#pragma warning(push, 0)
#include <QDateTime>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include "sqlite3.h"
#pragma warning(pop)

class  GTADbDataController : public QObject
{
	Q_OBJECT
public:
	GTADbDataController();
	~GTADbDataController();

	/**
	Opens the database located at path.
	@param path the path to the database.
	@return true if success, false if failure.
	*/
	bool GTADbDataController::openDb(QString);

	/**
	Adds a file to the database.
	@param pItem the file to add to the database.
	@return true if success, false if failure.
	*/
	bool GTADbDataController::addFile(GTATreeItem*);

	/**
	Gets the status of the DB.
	@return isOpened true if db is opened, false otherwise.
	*/
	bool  GTADbDataController::getIsOpened();

	/**
	Gets all files information contained in db.
	@return Qlist containing GTATreeItem*.
	*/
	QList<GTATreeItem*> GTADbDataController::getAllFiles();

	/**
	Deletes a file from the database.
	@param pItem the file to remove from the database.
	@return true if success, false if failure.
	*/
	bool deleteFile(QString filePath);

	/**
	Adds all children belonging to pParent pointer.
	*/
	void getAllChildren(GTATreeItem* pParent);

	/**
	Closes the database _db.
	*/
	void GTADbDataController::closeDb();

	/**
	Gets _isNew value defining if DB has been created during current execution.
	@return bool containing _isNew.
	*/
	bool GTADbDataController::getIsNew();

	/**
	Gets _allFolders containing all the Tree Items that are folders.
	@return QList<GTATreeItem*> containing _allFolders.
	*/
	QList<GTATreeItem*> GTADbDataController::getAllFolders();

	/**
	Deletes database file.
	@param iDataDirectoryPath the path to the database.
	@return bool containing QFile::remove return status
	*/
	bool GTADbDataController::deleteDb(QString iDataDirectoryPath);

	QDateTime getLastModified(QString iDataDirectoryPath);

	int isFileInDB(QString filePath);

	/**
	Updates already existing file.
	@param pItem the item to update.
	@param newName new file name if renamed (default empty QString).
	@return bool true if success fals if error.
	*/
	bool GTADbDataController::updateFile(GTATreeItem* pItem, QString oldPath = QString());

	/**
	Updates SVN status of file.
	@param iNAme the name of the file.
	@param iUuid the UUID of file.
	@return bool true if success false if error.
	*/
	bool GTADbDataController::updateSvnStatus(QString iName, QString iUuid);

	QHash<QString, QString> GTADbDataController::getLastModifiedByParent(QString parentPath);

	void GTADbDataController::setIsNew(bool isNew);

	QString GTADbDataController::getPathFromUUID(QString UUID);

	/**
	Updates relative_path and parent valuesfor the file in the database.
	@param newRelPath the new relative path of the file.
	@param newRoot the new parent path for the file.
	@param fileName the name of the file.
	@return bool if success and false if error.
	*/
	bool GTADbDataController::updateRelPath(QString newRelPath, QString newRoot, QString fileName);

	/**
	Updates relative_path and parent values for the folder in the database.
	@param newRelPath the new relative path of the folder.
	@param newRoot the new parent path for the folder.
	@param folderName the name of the folder.
	@return bool if success and false if error.
	*/
	bool GTADbDataController::updateFolderRelPath(QString newRelPath, QString newRoot, QString folderName);

private:

	/**
	Sets the status of the DB.
	@param isOpened the status to set
	*/
	void GTADbDataController::setIsOpened(bool);

	bool _isOpened = false;
	bool _isNew = false;
	QList<GTATreeItem*> _allFolders;
	QSqlDatabase _db;

signals:
	void onDuplicateUUIDDetected(QString oldPath, QString newPath);
};

#endif