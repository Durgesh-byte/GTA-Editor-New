#include "GTADbDataController.h"



GTADbDataController::GTADbDataController()
{
}


GTADbDataController::~GTADbDataController()
{
}

bool GTADbDataController::deleteDb(QString iDataDirectoryPath)
{
	/*if(_isOpened){
		closeDb();
	}*/
	QString path = iDataDirectoryPath;
	path.append("/../WORK_DB.db");
	path.replace("/", "\\");
	bool rc = false;
	QFileInfo fileInfo(path);
	if (fileInfo.exists())
	{
		QFile dbFile(path);
		dbFile.setPermissions(QFile::WriteOther);
		rc = QFile::remove(path);
	}
	if (!rc)
	{
		QSqlQuery query(_db);
		query.exec("DELETE FROM DATA;");
	}
	
	_isOpened = false;
	//return rc;
	return true;
}

QHash<QString, QString> GTADbDataController::getLastModifiedByParent(QString parentPath)
{
	QHash<QString, QString> FileLastModifTable;
	QSqlQuery query(_db);
	QString select = QString("SELECT relative_path, last_modified "
		"FROM DATA "
		"WHERE parent = '" + parentPath + "'");
	if (!query.exec(select)) {
		QSqlError error = query.lastError();
		QString toto = error.text();
	}

	while (query.next())
	{
		QSqlRecord SqlRecord = query.record();
		QString relative_path = SqlRecord.value("relative_path").toString();
		QString last_modified = SqlRecord.value("last_modified").toString();
		FileLastModifTable.insert(relative_path, last_modified);
	}
	return FileLastModifTable;
}

bool GTADbDataController::updateRelPath(QString newRelPath, QString newRoot, QString fileName)
{
	bool rc = false;
	if (_isOpened)
	{
		QSqlQuery query(_db);
		QString update = "UPDATE DATA";
		if (newRoot.isEmpty())
			newRoot = "root";
		update.append(" SET relative_path = '" + newRelPath + "', parent = '" + newRoot + "' WHERE name = '" + fileName + "'");
		query.prepare(update);
		rc = query.exec();
		if (!rc) {
			QSqlError error = query.lastError();
		}
	}
	return rc;
}

bool GTADbDataController::updateFolderRelPath(QString newRelPath, QString newRoot, QString folderName)
{
	bool rc = false;
	if (_isOpened)
	{
		QSqlQuery query(_db);
		QString update = "UPDATE DATA";
		if (newRoot.isEmpty())
			newRoot = "root";
		update.append(" SET relative_path = '" + newRelPath + "', parent = '" + newRoot + "' WHERE name = '" + folderName + "'");
		query.prepare(update);
		rc = query.exec();
		if (!rc) {
			QSqlError error = query.lastError();
		}
	}
	return rc;
}


QDateTime GTADbDataController::getLastModified(QString iDataDirectoryPath = QString())
{
	QString path = iDataDirectoryPath;
	path.append("/../WORK_DB.db");
	path.replace("/", "\\");
	bool rc = false;
	QFileInfo fileInfo(path);
	rc = fileInfo.exists();
	if (rc)
		return fileInfo.lastModified();
	else
		return QDateTime();
}

int GTADbDataController::isFileInDB(QString filePath)
{

	int present = -1;
	if (_isOpened)
	{
		QSqlQuery query(_db);
		QString select = QString("SELECT COUNT(1) "
			"FROM DATA "
			"WHERE relative_path = '" + filePath + "'");
		if (!query.exec(select)) {
			QSqlError error = query.lastError();
			QString toto = error.text();
		}
		while (query.next())
		{
			present = query.value(0).toInt();
		}
	}
	return present;
}


bool GTADbDataController::openDb(QString iDataDirectoryPath = QString())
{
	const QString DRIVER("QSQLITE");
	if (QSqlDatabase::isDriverAvailable(DRIVER)) {
		_db = QSqlDatabase::addDatabase(DRIVER);
		QString path = iDataDirectoryPath;
		path.append("/../WORK_DB.db");
		path.replace("/", "\\");
		bool rc;
		QFileInfo fileInfo(path);
		rc = fileInfo.exists();
		_db.setDatabaseName(path);
		if (rc) {
			//path.replace("\/", "\\");
			if (!_db.open())
			{
				setIsOpened(false);
				QSqlError error = _db.lastError();
				QString toto = error.text();
				return false;
			}
			else
			{
				setIsOpened(true);
				return true;
			}
		}
		else
		{
			_isNew = true;
			_db.open();
			QSqlQuery query(_db);
			query.exec("create table if not exists DATA "
				"(name string not null, "
				"description string, "
				"gta_version string not null, "
				"uuid string unique, "
				"date_created date not null, "
				"author string, "
				"last_modified date not null, "
				"validation_status string, "
				"validator string, "
				"date_validated date , "
				"relative_path string primary key not null unique, "
				"is_favourite boolean, "
				"extension string, "
				"parent string, "
				"svn_status string)");

			return openDb(iDataDirectoryPath);
		}

	}
	return false;
}

bool GTADbDataController::updateFile(GTATreeItem* pItem, QString oldPath)
{
	bool rc = false;
	if (_isOpened)
	{
		QSqlQuery query(_db);
		QString insert = QString("UPDATE DATA "
			"SET name = '%1', "
			"description = '%2', "
			"gta_version = '%3', "
			"uuid = '%4', "
			"date_created = '%5', "
			"author = '%6', "
			"last_modified = '%7', "
			"validation_status = '%8', "
			"validator = '%9', "
			"date_validated = '%10'");
		if (pItem->getParent() != nullptr)
		{
			insert.append(" ,"
				"parent ='" + pItem->getParent()->getAbsoluteRelPath() + "'");
		}
		if (!oldPath.isEmpty())
		{
			insert.append(" ,"
				"relative_path ='" + pItem->getAbsoluteRelPath() + "'");
		}
		else
		{
			oldPath = pItem->getAbsoluteRelPath();
		}
		if (pItem->getSvnStatus() != nullptr)
		{
			insert.append(" ,"
				"svn_status ='" + pItem->getSvnStatus() + "'");
		}
		insert.append(" WHERE "
			"relative_path = '" + oldPath + "'");
		insert = insert.arg(pItem->getName()).arg(pItem->getDescription()).arg(pItem->getGtaVersion()).arg(pItem->getUUID()).arg(pItem->getCreatedDate()).arg(pItem->getAuthorName()).arg(pItem->getModifiedDate()).arg(pItem->getValidationStatus()).arg(pItem->getValidatedBy()).arg(pItem->getValidationDate());
		query.prepare(insert);
		rc = query.exec();
	}

	return rc;
}


bool GTADbDataController::updateSvnStatus(QString iName, QString iUuid)
{
	bool rc = false;
	if (_isOpened)
	{
		QSqlQuery query(_db);
		QString update = QString("UPDATE DATA "
			"SET svn_status = 'M'");

		update.append(" WHERE "
			"name = '" + iName + "' AND "
			"uuid = '" + iUuid + "'");

		query.prepare(update);

		rc = query.exec();
		if (!rc) {
			QSqlError error = query.lastError();
		}
	}
	return rc;
}

bool GTADbDataController::addFile(GTATreeItem* pItem)
{
	bool rc = false;
	if (_isOpened)
	{
		QSqlQuery query(_db);
		QString parent = "root";
		QString insert = "INSERT INTO DATA (name,description,gta_version,uuid,date_created,author,last_modified,validation_status,validator,date_validated,relative_path,is_favourite,extension,parent,svn_status)";
		QString UUID = pItem->getUUID();
		if (UUID.isEmpty())
			UUID = "NULL";
		else
			UUID = '\'' + UUID + '\'';
		if (pItem->getParent() != nullptr)
			parent = pItem->getParent()->getAbsoluteRelPath();
		QString values = QString("VALUES ('%1','%2','%3',%4,'%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15')").arg(pItem->getName()).arg(pItem->getDescription()).arg(pItem->getGtaVersion()).arg(UUID).arg(pItem->getCreatedDate()).arg(pItem->getAuthorName()).arg(pItem->getModifiedDate()).arg(pItem->getValidationStatus()).arg(pItem->getValidatedBy()).arg(pItem->getValidationDate()).arg(pItem->getAbsoluteRelPath()).arg(QString::number(pItem->isFavourite())).arg(pItem->getExtension()).arg(parent).arg(pItem->getSvnStatus());
		insert.append(values);
		query.prepare(insert);
		//query.first();
		rc = query.exec();
		if (!rc) {
			QSqlError error = query.lastError();
			QString errorText = error.text();
			if (errorText == "UNIQUE constraint failed: DATA.uuid Unable to fetch row") {
				QString oldPath = getPathFromUUID(UUID);
				emit onDuplicateUUIDDetected(oldPath, pItem->getAbsoluteRelPath());
			}
		}
	}
	return rc;
}

QString GTADbDataController::getPathFromUUID(QString UUID)
{
	if (_isOpened)
	{
		QSqlQuery query(_db);
		QString select = QString("SELECT relative_path "
			"FROM DATA "
			"WHERE uuid = " + UUID);
		if (!query.exec(select))
		{
			QSqlError error = query.lastError();
			QString toto = error.text();
		}
		QString relPath;
		while (query.next())
		{
			relPath = query.value(0).toString();
		}
		return relPath;
	}
	return QString();

}

bool GTADbDataController::deleteFile(QString filePath)
{
	bool rc = false;
	if (_isOpened)
	{
		QSqlQuery query(_db);
		QString remove = "DELETE FROM DATA";
		remove.append(" WHERE "
			"relative_path = '" + filePath + "'");
		query.prepare(remove);
		rc = query.exec();
		if (!rc) {
			QSqlError error = query.lastError();
			QString toto = error.text();
		}
	}
	return rc;
}

void GTADbDataController::setIsOpened(bool isOpened)
{
	_isOpened = isOpened;

}


bool GTADbDataController::getIsOpened()
{
	return true;
}

void GTADbDataController::getAllChildren(GTATreeItem* pParent)
{
	QString parentName = pParent->getAbsoluteRelPath();
	if (_isOpened)
	{
		pParent->clearChildren();
		QSqlQuery query(_db);
		query.prepare(QString("SELECT * FROM DATA WHERE parent = '%1'").arg(parentName));
		if (!query.exec()) {
			QSqlError error = query.lastError();
			QString toto = error.text();
		}
		while (query.next()) {
			GTATreeItem* pItem = new GTATreeItem;
			pItem->setName(query.value(0).toString());
			pItem->setDescription(query.value(1).toString());
			pItem->setGtaVersion(query.value(2).toString());
			pItem->setUUID(query.value(3).toString());
			pItem->setCreatedDate(query.value(4).toString());
			pItem->setAuthorName(query.value(5).toString());
			pItem->setModifiedDate(query.value(6).toString());
			pItem->setValidationStatus(query.value(7).toString());
			pItem->setValidatedBy(query.value(8).toString());
			pItem->setValidationDate(query.value(9).toString());
			pItem->setAbsoluteRelPath(query.value(10).toString());
			pItem->setIsFavourite(query.value(11).toInt());
			pItem->setExtension(query.value(12).toString());
			//pItem->setParent(query.value(13).toString());
			pItem->setSvnStatus(query.value(14).toString()); //("Not SVN");
			pItem->setMaxTimeEstimated(query.value(15).toString());
			if (pItem->getExtension() == "folder")
			{
				getAllChildren(pItem);
			}
			pParent->addChild(pItem);
		}
	}
}

QList<GTATreeItem*> GTADbDataController::getAllFiles()
{/*
	QList<GTATreeItem*> allFiles;
	QStringList allParents;
	if (_isOpened)
	{
		QSqlQuery query(_db);
		query.prepare("SELECT DISTINCT parent FROM DATA");
		if (!query.exec()) {
			QSqlError error = query.lastError();
			QString toto = error.text();
		}
		while (query.next()) {
			allParents.append(query.value(0).toString());
		}
		if (query.isActive())
			query.finish();
		foreach(QString parent, allParents) {

		}
		query.prepare(QString("SELECT * FROM DATA"));
		if (!query.exec()) {
			QSqlError error = query.lastError();
			QString toto = error.text();
		}

		while (query.next()) {
			pItem->setName(query.value(0).toString());
			pItem->setDescription(query.value(1).toString());
			pItem->setGtaVersion(query.value(2).toString());
			pItem->setUUID(query.value(3).toString());
			pItem->setCreatedDate(query.value(4).toString());
			pItem->setAuthorName(query.value(5).toString());
			pItem->setModifiedDate(query.value(6).toString());
			pItem->setValidationStatus(query.value(7).toString());
			pItem->setValidatedBy(query.value(8).toString());
			pItem->setValidationDate(query.value(9).toString());
			pItem->setAbsoluteRelPath(query.value(10).toString());
			pItem->setIsFavourite(query.value(11).toInt());
			pItem->setExtension(query.value(12).toString());
		}
		if (query.isActive())
			query.finish();
	}*/
	QList<GTATreeItem*> allFiles;
	return allFiles;

}

void GTADbDataController::closeDb()
{
	_db.close();
}

bool GTADbDataController::getIsNew()
{
	return _isNew;
}

void GTADbDataController::setIsNew(bool isNew)
{
	_isNew = isNew;
}

QList<GTATreeItem*> GTADbDataController::getAllFolders()
{
	return _allFolders;
}