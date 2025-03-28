//#include "AINGTADatabaseManager.h"
////using namespace AINGTA::DbUtils;
////#include "qxml.h"
//#include <QSqlError>
//#include <QSqlQuery>
//#include <QSqlRecord>
//#include <QSqlField>
//#include <QFile>
//#include "AINGTAHeader.h"
//AINGTADatabaseManager::AINGTADatabaseManager()
//{
//    //_DatabaseFilePath = iDatabaseFilePath;
//    //openDatabase(_DatabaseFilePath);
//}
//
//bool AINGTADatabaseManager::openDatabase(const QString &iDatabaseFilePath)
//{
//    bool rc = false;
//	if(QFile::exists(iDatabaseFilePath))
//    {
//        _pSqlDb = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", iDatabaseFilePath));
//        _pSqlDb->setDatabaseName(iDatabaseFilePath);
//        _pSqlDb->open();
//        if (_pSqlDb->isValid() &&  _pSqlDb->isOpen())
//        {
//            rc = true;
//        }
//        else
//        {
//            _LastErrorMsg =  _pSqlDb->lastError().text();
//        }
//    }
//    return rc;
//}
//void AINGTADatabaseManager::closeDatabase()
//{
//    _pSqlDb->close();
//    if(_pSqlDb != NULL)
//    {
//        delete _pSqlDb;
//        _pSqlDb = NULL;
//    }
//}
//bool AINGTADatabaseManager::isOpen()
//{
//	bool rc = false;
//	if(_pSqlDb != NULL && _pSqlDb->isOpen())
//		rc = true;
//
//	return rc;
//}
//QString AINGTADatabaseManager::getLastError()
//{
//    return _LastErrorMsg;
//}
//
//AINGTAHeader * AINGTADatabaseManager::getHeader(long DBID)
//{
//    AINGTAHeader * pHeader = NULL;
//    if(_pSqlDb != NULL )
//    {
//        QSqlQuery * selQuery = new QSqlQuery(*_pSqlDb);
//        QString queryStr = "SELECT T1.ID_HEADER, T1.NAME, T1.CREATION_DATE, T1.DESCRIPTION, T1.DESIGNER,T1.EQUIPMENT_SUPPLIER ";
//                queryStr += ",T2.PROGRAM AS PROG, T2.EQUIPMENT AS EQUIP,T2.STANDARD AS STD FROM HEADER AS T1 ";
//                queryStr += " INNER JOIN PROGCONF AS T2 ON T1.ID_PROGCONF = T2.ID_PROGCONF ";
//                queryStr += " WHERE T1.ID_HEADER = " + QString::number(DBID);
//
//        bool rc = selQuery->exec(queryStr);
//        if(rc)
//        {
//            pHeader= new AINGTAHeader();
//            selQuery->next();
//
//            QSqlRecord iRecord = selQuery->record();
//            QString name = iRecord.field("NAME").value().toString();
//            QString date = iRecord.field("CREATION_DATE").value().toString();
//            QString description = iRecord.field("DESCRIPTION").value().toString();
//            QString designer = iRecord.field("DESIGNER").value().toString();
//            QString supplier = iRecord.field("EQUIPMENT_SUPPLIER").value().toString();
//            QString headerID = iRecord.field("ID_HEADER").value().toString();
//            QString keyWords = iRecord.field("KEYWORDS").value().toString();;
//            QString prog = iRecord.field("PROG").value().toString();
//            QString equip = iRecord.field("EQUIP").value().toString();
//            QString std = iRecord.field("STD").value().toString();
//
//            pHeader->setCreationDate(date);
//            pHeader->setDescription(description);
//            pHeader->setDesignerName(designer);
//            pHeader->setEquipment(equip);
//            pHeader->setEquipmentSupplier(supplier);
//            pHeader->setKeyWords(keyWords);
//            pHeader->setName(name);
//            pHeader->setProgram(prog);
//            pHeader->setStandard(std);
//            pHeader->DBID = headerID.toLong();
//        }
//        else
//            _LastErrorMsg = _pSqlDb->lastError().text();
//    }
//    return pHeader;
//}
//
//QList<AINGTAHeader*> AINGTADatabaseManager::getHeaderList()
//{
//    QList<AINGTAHeader*> oHeaderList;
//    if(_pSqlDb != NULL )
//    {
//        QSqlQuery * selQuery = new QSqlQuery(*_pSqlDb);
//        QString queryStr = "SELECT T1.ID_HEADER, T1.NAME, T1.CREATION_DATE, T1.DESCRIPTION, T1.DESIGNER,T1.EQUIPMENT_SUPPLIER";
//				queryStr += ",T2.PROGRAM AS PROG, T2.EQUIPMENT AS EQUIP,T2.STANDARD AS STD FROM HEADER AS T1 ";
//				queryStr += " INNER JOIN PROGCONF AS T2 ON T1.ID_PROGCONF = T2.ID_PROGCONF";
//
//        bool rc = selQuery->exec(queryStr);
//        if(rc)
//        {
//            while(selQuery->next())
//            {
//                AINGTAHeader * pHeader= new AINGTAHeader();
//                QSqlRecord iRecord = selQuery->record();
//				QString name = iRecord.field("NAME").value().toString();
//				QString date = iRecord.field("CREATION_DATE").value().toString();
//				QString description = iRecord.field("DESCRIPTION").value().toString();
//				QString designer = iRecord.field("DESIGNER").value().toString();
//				QString supplier = iRecord.field("EQUIPMENT_SUPPLIER").value().toString();
//				QString headerID = iRecord.field("ID_HEADER").value().toString();
//				QString keyWords = iRecord.field("KEYWORDS").value().toString();;
//				QString prog = iRecord.field("PROG").value().toString();
//				QString equip = iRecord.field("EQUIP").value().toString();
//				QString std = iRecord.field("STD").value().toString();
//
//
//                pHeader->setCreationDate(date);
//                pHeader->setDescription(description);
//                pHeader->setDesignerName(designer);
//                pHeader->setEquipment(equip);
//                pHeader->setEquipmentSupplier(supplier);
//                pHeader->setKeyWords(keyWords);
//                pHeader->setName(name);
//                pHeader->setProgram(prog);
//                pHeader->setStandard(std);
//                pHeader->DBID = headerID.toLong();
//
//                oHeaderList.append(pHeader);
//            }
//        }
//        else
//            _LastErrorMsg = _pSqlDb->lastError().text();
//    }
//    return oHeaderList;
//}
//
//bool AINGTADatabaseManager::saveHeader(AINGTAHeader *& ipHeader)
//{
//	bool rc = false;
//	return rc;
//}
//
//QList<QString> AINGTADatabaseManager::getColumnNameList(const QString & iTableName)
//{
//	QList<QString> oColumnList;
//	if(_pSqlDb != NULL)
//	{
//		QSqlQuery query = QSqlQuery(*_pSqlDb);
//		QString sqlQuery="PRAGMA table_info(" + iTableName + ")";
//		bool rc = query.exec(sqlQuery);
//		if(rc == true)
//		{
//			while(query.next())
//			{
//				QSqlRecord record = query.record();
//				if(record.count() > 2)
//				{
//					QString columnName = record.field(1).value().toString();
//					oColumnList.append(columnName);
//				}
//			}
//		}
//	}
//	return oColumnList;
//}
//QList<QHash<QString,QString>>  AINGTADatabaseManager::getObject(const QString & iObjectName)
//{
//
//	QList<QHash<QString,QString>> objectData;
//	if(iObjectName == "")
//	{
//		QList<QString> columnList = getColumnNameList("OBJECTEDIT");
//
//		foreach(QString columnName, columnList)
//		{
//			QHash<QString, QString> row;
//			row.insert(columnName,"");
//			objectData.append(row);
//		}
//	}
//	else
//	{
//		if(_pSqlDb != NULL)
//		{
//			QSqlQuery query = QSqlQuery(*_pSqlDb);
//			QString queryStr = "SELECT T1.ACTION, T1.COMMENTS, T1.CONTROL, T1.DUMP_LIST, T1.PRECISION,T1.ON_FAIL,T1.TIMEOUT, T2.ID_OBJECT";
//			queryStr += " FROM OBJECTEDIT AS T1 ";
//			queryStr += " INNER JOIN OBJECT AS T2 ON T1.ID_OBJECT = T2.ID_OBJECT";
//			queryStr += " WHERE T2.NAME = " + iObjectName;
//
//			bool rc = query.exec(queryStr);
//			if(rc == true)
//			{
//				while(query.next())
//				{
//					QSqlRecord row = query.record();
//					QString action = row.field("ACTION").value().toString();
//					QString control = row.field("CONTROL").value().toString();
//					QString comments = row.field("COMMENTS").value().toString();
//					QString timeout = row.field("TIMEOUT").value().toString();
//					QString precision = row.field("PRECISION").value().toString();
//					QString onFail = row.field("ON_FAIL").value().toString();
//					QString dumpList = row.field("DUMP_LIST").value().toString();
//					QString objectID = row.field("ID_OBJECT").value().toString();
//
//					QHash<QString, QString> rowItem;
//					rowItem.insert("ACTION",action);
//					rowItem.insert("CONTROL",control);
//					rowItem.insert("COMMENTS",comments);
//					rowItem.insert("TIMEOUT",timeout);
//					rowItem.insert("PRECISION",precision);
//					rowItem.insert("ON_FAIL",onFail);
//					rowItem.insert("DUMP_LIST",dumpList);
//					rowItem.insert("ID_OBJECT",objectID);
//					objectData.append(rowItem);
//				}
//			}
//		}
//	}
//	return objectData;
//}