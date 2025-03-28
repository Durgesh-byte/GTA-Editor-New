//#ifndef AINGTADATABASEMANAGER_H
//#define AINGTADATABASEMANAGER_H
//
//#include "AINGTADbUtils.h"
//
////#include "AINCMNDatabaseManager.h"
//#include <QSqlDatabase>
//#include <QHash>
//#include <QList>
//class AINGTAHeader;
//class AINGTADbUtils_SHARED_EXPORT AINGTADatabaseManager
//{
//
//public:
//    AINGTADatabaseManager();
//    bool openDatabase(const QString & iDatabaseFilePath);
//    QString getLastError();
//    void closeDatabase();
//	bool isOpen();
//    AINGTAHeader * getHeader(long DBID);
//    QList<AINGTAHeader*> getHeaderList();
//    bool saveHeader(AINGTAHeader *& ipHeader);
//
//	QList<QHash<QString,QString>>  AINGTADatabaseManager::getObject(const QString & iObjectName = "");
//
//	
//
//
//private:
//	QList<QString> getColumnNameList(const QString & iTableName);
//		
//    QSqlDatabase * _pSqlDb;
//    QString _LastErrorMsg;
//	QString _DatabaseFilePath;
//
//};
//#endif //AINGTADBMANAGER_H
