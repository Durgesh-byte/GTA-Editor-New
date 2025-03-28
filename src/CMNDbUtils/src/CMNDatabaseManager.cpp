#include "AINCMNDatabaseManager.h"
#include "AINCMNVariables.h"
#include "AINCMNProgressDialog.h"
#include "AINCMNCommonServices.h"


#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QVariant>
#include <QSet>
#include <QList>
#include <QStringList>


using namespace AINCMN::DbUtils;
using namespace AINCMN::CMNUtils;


#define D_QUOTE QString("\"")
#define INDEX_NAME_SPLITTER ("_____")
#define TABLE_INFO_COL_NAME_INDEX 1
#define TABLE_INFO_COL_TYPE_INDEX 2
#define TABLE_INFO_COL_PRIMARY_KEY_INDEX 5
#define TABLE_INFO_IS_PRIMARY_KEY_IDENTIFIER "1"
#define TABLE_INFO_IS_NOT_PRIMARY_KEY_IDENTIFIER "0"
#define TABLEINDEX_LIST_COL_NAME_INDEX 1

AINCMNDatabaseManager::AINCMNDatabaseManager(const QString& iDbFilePath, QObject *parent) 
:QObject(parent), _pDb(NULL)
{
	_bCloseDb = true;

	_sDbFilePath = iDbFilePath;
	
	_pDbQueries = new AINCMNDbQueries();

	bool bOk = openDatabase();
	Q_ASSERT(bOk);
}

AINCMNDatabaseManager::AINCMNDatabaseManager(QSqlDatabase * ipSqlDatabase, QObject *parent)
:QObject(parent)
{
    _pDb = ipSqlDatabase;

    _bCloseDb = false;
		
    _pDbQueries = new AINCMNDbQueries();
}

AINCMNDatabaseManager::~AINCMNDatabaseManager()
{
    if (_bCloseDb && _pDb)
		_pDb->close();

    if (_pDbQueries)
    {
        delete _pDbQueries;
        _pDbQueries = NULL;
    }
}


bool AINCMNDatabaseManager::openDatabase(QString iDbFilePath)
{
	bool bOk(false);
	
	if (iDbFilePath.isEmpty())
		iDbFilePath = _sDbFilePath;
			
	_pDb = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", iDbFilePath));
	
	_pDb->setDatabaseName(iDbFilePath);
	_pDb->open();
	
	bOk = _pDb->isValid();
	bool bOk2 = _pDb->isOpen();
	
	if (!bOk || !bOk2)
	{
		QSqlError error = _pDb->lastError();
		AINCMNCommonServices::display("Error: DbOpen", error.text() + QString("\n") + iDbFilePath);
	}
	return bOk;	
}

QSqlDatabase * AINCMNDatabaseManager::getDatabase()
{
	return _pDb;
}

bool AINCMNDatabaseManager::startTransaction()
{
	bool bOk = _pDb->transaction();
	
	return bOk;
}

bool AINCMNDatabaseManager::endTransaction()
{
	bool bOk = _pDb->commit();
	if (! bOk)
		bOk = _pDb->rollback();
	
	return bOk;
}

void AINCMNDatabaseManager::getListOfTables(QStringList & ioListofTables)
{
	if (_pDb->isOpen())
	{
		ioListofTables = _pDb->tables(QSql::Tables);
	}
}

bool AINCMNDatabaseManager::isTablePresent(const QString& iTableName) const
{
	QStringList listofTables = _pDb->tables(QSql::Tables);
	
	bool bOk = listofTables.contains(iTableName, Qt::CaseInsensitive);
	
	return bOk;
}

QString AINCMNDatabaseManager::getDatabaseAbsFile()
{
	return _sDbFilePath;
}

QList<QStringList> AINCMNDatabaseManager::getTableContent(const QString& iTableName, 
													   const QStringList& iTableColumns)
{
	QList<QStringList> listOfTableContent;
	
	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);
	
	bool bOk = qry.exec("SELECT * FROM " + iTableName);

	if ( bOk )
	{
		while(qry.next())
		{			
			QSqlRecord iRecord = qry.record();
			int iRecCnt = iRecord.count();
			if (iRecCnt >= iTableColumns.count())
			{
				QStringList listOfLineItems;
				for (int i=0; i<iTableColumns.count(); i++)
				{
					QString sColValue = iRecord.value(iTableColumns.at(i)).toString();
					listOfLineItems.append(sColValue);
				}

				listOfTableContent.append(listOfLineItems);
			}
		}
	}

	return listOfTableContent;
}

QList<QHash<QString, QString>> AINCMNDatabaseManager::getTableContent(const QString& iTableName)
{
	QList<QHash<QString, QString>> listOfTableContent;

	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);
	
	QStringList lstOfCols = getListOfColumns(iTableName);
	bool bOk = qry.exec("SELECT * FROM " + iTableName);
	if ( bOk && lstOfCols.count())
	{		
		while(qry.next())
		{
			QHash<QString, QString> hshOfRow;
			QSqlRecord iRecord = qry.record();
			for (int i=0; i<lstOfCols.count(); i++)
			{
				QString sColValue = iRecord.value(lstOfCols.at(i)).toString();
				hshOfRow.insert(lstOfCols.at(i), sColValue);
			}
			listOfTableContent.append(hshOfRow);
		}
	}
	
	return listOfTableContent;
}

QStringList AINCMNDatabaseManager::getListOfColumns(const QString& iTableName)
{
	QStringList listOfColumns;
	
	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);
	
	bool bOk = qry.exec("pragma table_info (" + iTableName + ")");
	if ( bOk )
	{
		while(qry.next())
		{			
			QSqlRecord iRecord = qry.record();
			QString sColumnName = iRecord.value(1).toString();
			listOfColumns.append(sColumnName);	
		}
	}
	
	return listOfColumns;
}

QList<QVariant> AINCMNDatabaseManager::getValues(const QString& iTableName, const QString& iSelectAttr, 
					const QString& iWhereAttr, const QString& iWhereAttrVal, bool bWhrAttrValNum)
{
    QList<QVariant> listOfValues;
	
	if (isTablePresent(iTableName))
    {
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);
		
		QString qryStr = _pDbQueries->createSelectString(iTableName, iSelectAttr, iWhereAttr, iWhereAttrVal, bWhrAttrValNum);
		bool bOk = selectQry.exec(qryStr);

		if ( bOk )
		{
			while (selectQry.next())
			{
				QSqlRecord record = selectQry.record();
				if (! record.isEmpty())
				{                   
					QVariant vId = record.value(iSelectAttr);
					if (! vId.isNull() && vId.isValid())
					{
						listOfValues.append(vId);
					}
				}
			}
		}
		else {
			showLastError("Error: getValues1 for "+iTableName);
		}
	}
	
    return listOfValues;
}

QList<QVariant> AINCMNDatabaseManager::getValues(const QString& iTableName, const QString& iSelectAttr)
{
    QList<QVariant> listOfValues;
	if (isTablePresent(iTableName))
    {
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);
	    
		QString qryStr = _pDbQueries->createSelectString(iTableName, iSelectAttr);
		bool bOk = selectQry.exec(qryStr);

		if ( bOk )
		{
			while (selectQry.next())
			{
				QSqlRecord record = selectQry.record();
				if (! record.isEmpty())
				{                   
					QVariant vId = record.value(iSelectAttr);
					if (! vId.isNull() && vId.isValid())
					{
						listOfValues.append(vId);
					}
				}
			}
		}
		else {
			showLastError("Error: getValues2 for "+iTableName);
		}
	}

    return listOfValues;
}

QList<QHash<QString, QVariant>> AINCMNDatabaseManager::getValues(const QString& iTableName,
			const QStringList& iSelectAttrList, const QString& iWhereAttr, 
			const QString& iWhereAttrVal, bool bWhrAttrValNum)
{
    QList<QHash<QString, QVariant>> listOfValues;
	
	if (isTablePresent(iTableName))
    {
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);
	    
		QString qryStr = _pDbQueries->createSelectString(iTableName, iSelectAttrList, iWhereAttr, iWhereAttrVal, bWhrAttrValNum);
		bool bOk = selectQry.exec(qryStr);

		if ( bOk )
		{
			while (selectQry.next())
			{
				QHash<QString, QVariant> hashOfValues;
				for (int i=0; i<iSelectAttrList.count(); i++)
				{
					QVariant vId = selectQry.record().value(iSelectAttrList.at(i));
					hashOfValues.insert(iSelectAttrList.at(i), vId);
				}			
				listOfValues.append(hashOfValues);
			}
		}
		else {
			showLastError("Error: getValues3 for "+iTableName);
		}
	}

    return listOfValues;
}

QList<QHash<QString, QVariant>> AINCMNDatabaseManager::getValues(const QString& sTable, const QStringList& iLstOfSeletctAttrs,
							 const QHash<QString, QString>& ihshOfWhereAttrs, const QHash<QString, bool>& iHshOfWhrAttrDTNum)
{
	QList<QHash<QString, QVariant>> lstOfValues;
	if (iLstOfSeletctAttrs.count())
	{
		QStringList lstWhereAttr = ihshOfWhereAttrs.keys();
		QStringList lstWhereAttrDT = iHshOfWhrAttrDTNum.keys();
		if (lstWhereAttrDT.count() != lstWhereAttr.count())
			return lstOfValues;

		QStringList lstWhereAttrVal;
		QList<bool> lstWhereAttrDTVals;
		for (int i=0; i<lstWhereAttr.count(); i++)
		{
			QString sKey = lstWhereAttr.at(i);

			lstWhereAttrVal.append(ihshOfWhereAttrs.value(sKey));
			lstWhereAttrDTVals.append(iHshOfWhrAttrDTNum.value(sKey));
		}
		
		lstOfValues = getValues(sTable, iLstOfSeletctAttrs, lstWhereAttr, lstWhereAttrVal, lstWhereAttrDTVals);
	}

	return lstOfValues;
}

QList<QHash<QString, QVariant>> AINCMNDatabaseManager::getValues(const QString& sTable, 
			const QStringList& iLstOfSeletctAttrs, const QHash<QString, QString>& ihshOfWhereAttrs, bool bWhereAttrNum)
{
	QList<QHash<QString, QVariant>> lstOfValues;
	if (iLstOfSeletctAttrs.count())
	{
		QStringList lstWhereAttrVal;
		QList<bool> lstWhereAttrDT;
		QStringList lstWhereAttr = ihshOfWhereAttrs.keys();
		for (int i=0; i<lstWhereAttr.count(); i++)
		{
			QString sKey = lstWhereAttr.at(i);
			
			lstWhereAttrVal.append(ihshOfWhereAttrs.value(sKey));
			
			if ( bWhereAttrNum )
				lstWhereAttrDT.append(true);
			else
				lstWhereAttrDT.append(false);
		}

		lstOfValues = getValues(sTable, iLstOfSeletctAttrs, lstWhereAttr, lstWhereAttrVal, lstWhereAttrDT);
	}
	
	return lstOfValues;
}

QList<QHash<QString, QVariant>> AINCMNDatabaseManager::getValues(const QString& iTableName, const QStringList& iSelectAttrList, 
					const QStringList& iWhereAttrList, const QStringList& iWhereAttrValList, const QList<bool>& ibWhrAttrValNumList)
{
	QList<QHash<QString, QVariant>> listOfValues;
	if (isTablePresent(iTableName))
    {
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);
	    
		if (iWhereAttrList.count() == iWhereAttrValList.count() && iWhereAttrValList.count() == ibWhrAttrValNumList.count())
		{			
			QString qryStr = _pDbQueries->createSelectString(iTableName, iSelectAttrList, iWhereAttrList, iWhereAttrValList, ibWhrAttrValNumList);
			bool bOk = selectQry.exec(qryStr);

			if ( bOk )
			{
				while (selectQry.next())
				{
					QHash<QString, QVariant> hashOfValues;
					for (int i=0; i<iSelectAttrList.count(); i++)
					{
						QVariant vId = selectQry.record().value(iSelectAttrList.at(i));
						hashOfValues.insert(iSelectAttrList.at(i), vId);
					}			
					listOfValues.append(hashOfValues);
				}
			}
			else {
				showLastError("Error: getValues3 for "+iTableName);
			}
		}
		else {
			//Error Log
		}
	}
    return listOfValues;

}

QList<QHash<QString, QVariant>> AINCMNDatabaseManager::getValues(
                                        const QString& iTableName, const QStringList& iSelectAttrList)
{
    QList<QHash<QString, QVariant>> listOfValues;
	
	if (isTablePresent(iTableName))
    {
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);
	    
		QString qryStr = _pDbQueries->createSelectString(iTableName, iSelectAttrList);
		bool bOk = selectQry.exec(qryStr);
		if ( bOk )
		{
			while (selectQry.next())
			{
				QHash<QString, QVariant> hashOfValues;
				for (int i=0; i<iSelectAttrList.count(); i++)
				{
					QVariant vId = selectQry.record().value(iSelectAttrList.at(i));
					hashOfValues.insert(iSelectAttrList.at(i), vId);
				}
				listOfValues.append(hashOfValues);
			}
		}
		else {
			showLastError("Error: getValues4 for "+iTableName);
		}
	}
    return listOfValues;
}

bool AINCMNDatabaseManager::createTableInDb(const QString& iTableName, 
									const QList<QHash<QString, QString>>& iListOfTableRowsWithHeader)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;
		
	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	QStringList listOfColNames = iListOfTableRowsWithHeader.at(0).keys();
	bOk = createEmptyTable(iTableName, listOfColNames);		
	if (! bOk )
	{
		endTransaction();
		return bOk;
	}
	
	if (iListOfTableRowsWithHeader.count()<1)
	{
		endTransaction();
		return bOk;
	}
		
	QStringList listOfKeys = _pDbQueries->createKeyNamesList(listOfColNames.count());
	QString sInsertQry = _pDbQueries->createInsertIntoQryString(listOfColNames, iTableName, listOfKeys);
	bOk = qry.prepare(sInsertQry);
	if (! bOk)
	{
		endTransaction();
		return bOk;
	}	
	
	for (int i=0; i<iListOfTableRowsWithHeader.count(); i++)
	{
		QHash<QString, QString> curRow = iListOfTableRowsWithHeader.at(i);
		for (int j=0; j<listOfColNames.count(); j++)
		{
			QString sKeyValue = curRow.value(listOfColNames.at(j));
			
			qry.bindValue(listOfKeys.at(j),sKeyValue);			
		}
		
		bOk = qry.exec();
	}
	
	endTransaction();
	
	return bOk;
}

bool AINCMNDatabaseManager::createTableInDb(const QString& iTableName, 
				const QList<QStringList>& iListOfTableRowsWithHeader, AINCMNProgressDialog *pProgBar)
{	
	bool bOk(false);
		
	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);
	
	int iMaxRange=iListOfTableRowsWithHeader.count();
	
	if (pProgBar)
	{
		//pProgBar->setWindowTitle("Table Creation: "+iTableName);
		pProgBar->setRange(0,iMaxRange);
	}
	QStringList listOfColNames = iListOfTableRowsWithHeader.at(0);
	bOk = createEmptyTable(iTableName, listOfColNames);
	
	if (! bOk )
		return bOk;
	
	if (iListOfTableRowsWithHeader.count()<=1)
		return bOk;
	
	bOk = startTransaction();	
	if (! bOk)
		return bOk;
	
	QStringList listOfKeys = _pDbQueries->createKeyNamesList(listOfColNames.count());
	QString sInsertQry = _pDbQueries->createInsertIntoQryString(listOfColNames, iTableName, listOfKeys);
	
	bOk = qry.prepare(sInsertQry);
	if (! bOk)
	{
		endTransaction();
		return bOk;
	}
		
	for (int i=1; i<iListOfTableRowsWithHeader.count(); i++)
	{
		QStringList curRow = iListOfTableRowsWithHeader.at(i);
		for (int j=0; j<curRow.count(); j++)
		{
			QString sKeyValue = curRow.at(j);

			qry.bindValue(listOfKeys.at(j),sKeyValue);			
		}
		
		bOk = qry.exec();
		
		if (pProgBar && i<=iMaxRange)
		{
			/*if (pProgBar->wasCanceled())
			{
				bOk = false;
				break;
			}*/
			pProgBar->setValue(i);
		}	
	}
	
	endTransaction();
	
	return bOk;
}

bool AINCMNDatabaseManager::alterTableName(const QString& iTableName, const QString& iNewTableName)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;
		
	QSqlQuery selectQry(*_pDb);
	selectQry.setForwardOnly(true);
	
	QString qryStr = _pDbQueries->alterTableNameQryStr(iTableName, iNewTableName); 
	bOk = selectQry.exec(qryStr);
	
	endTransaction();
	
	return bOk;
}

void AINCMNDatabaseManager::showLastError(const QString& iMsgTitle)
{
	QSqlError error = _pDb->lastError();
	AINCMNCommonServices::display(iMsgTitle, error.text() + QString("\n") + _sDbFilePath);
}

bool AINCMNDatabaseManager::createEmptyTable(const QString& iTableName, const QStringList& iListOfColNames)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;
		
	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	bOk = qry.exec("DROP TABLE IF EXISTS " + iTableName);
	if (! bOk )
	{
		endTransaction();
		return bOk;
	}
	QString sCreateTable = _pDbQueries->createTableString(iListOfColNames, iTableName);
	bOk = qry.exec(sCreateTable);
	
	endTransaction();

	qry.clear();
		
	return bOk;
}

bool AINCMNDatabaseManager::createEmptyTablePk(const QString& iTableName, 
											   const QStringList& iListOfColNames, const QString& sPK)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;

	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	bOk = qry.exec("DROP TABLE IF EXISTS " + iTableName);
	if (! bOk )
	{
		endTransaction();
		return bOk;
	}
	QString sCreateTable = _pDbQueries->createTableStringPk(iTableName, iListOfColNames, sPK);
	bOk = qry.exec(sCreateTable);
	
	endTransaction();

	qry.clear();

	return bOk;
}

bool AINCMNDatabaseManager::insertRowsInTable(const QString& iTableName, 
											  const QStringList& iListOfColNames, const QList<QStringList>& iListOfRows)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;

	int iNbColumns = iListOfColNames.count();

	QStringList listOfKeys = _pDbQueries->createKeyNamesList(iNbColumns);
	QString sInsertStr = _pDbQueries->createInsertIntoQryString(iListOfColNames, iTableName, listOfKeys);
	QSqlQuery qry(*_pDb);
	qry.setForwardOnly(true);
	bOk = qry.prepare(sInsertStr);
	if (! bOk)
	{
		endTransaction();
		return bOk;
	}

	for (int i=0; i<iListOfRows.count(); i++)
	{
		QStringList curRow = iListOfRows.at(i);
		if (curRow.count() != iNbColumns)
		//if (curRow.count() > iNbColumns) //Major change done for EF Application
			continue;

		for (int j=0; j<iNbColumns; j++)
		{
			qry.bindValue(listOfKeys.at(j), curRow.at(j));			
		}

		bOk = qry.exec();
	}

	endTransaction();

	return bOk;
}

bool AINCMNDatabaseManager::createEmptyTableDT(const QString& iTableName, const QStringList& iListOfColNames, 
					  const QStringList& iListOfColDataType)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;

	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	bOk = qry.exec("DROP TABLE IF EXISTS " + iTableName);
	if (! bOk )
	{
		endTransaction();
		return bOk;
	}
	QString sCreateTable = _pDbQueries->createTableString(iTableName, iListOfColNames, iListOfColDataType);
	bOk = qry.exec(sCreateTable);
	
	endTransaction();
	
	qry.clear();

	return bOk;
}
bool AINCMNDatabaseManager::createEmptyTablePKDT(const QString& iTableName, const QStringList& iListOfColNames, 
											   const QStringList& iListOfColDataType,const QString& sPK)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;

	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	bOk = qry.exec("DROP TABLE IF EXISTS " + iTableName);
	if (! bOk )
	{
		endTransaction();
		return bOk;
	}
	QString sCreateTable = _pDbQueries->createTableStringPkDT(iTableName, iListOfColNames, iListOfColDataType,sPK);
	bOk = qry.exec(sCreateTable);

	endTransaction();

	qry.clear();

	return bOk;
}

bool AINCMNDatabaseManager::insertRowsInTable(const QString& iTableName, 
					   const QStringList& iListOfColNames, const QStringList& iListOfColDataType,
					   const QList<QStringList>& iListOfRows)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;

	int iNbColumns = iListOfColNames.count();

	QStringList listOfKeys = _pDbQueries->createKeyNamesList(iNbColumns);
	QString sInsertStr = _pDbQueries->createInsertIntoQryString(iListOfColNames, iTableName, listOfKeys);
	QSqlQuery qry(*_pDb);
	qry.setForwardOnly(true);
	bOk = qry.prepare(sInsertStr);
	if (! bOk)
	{
		endTransaction();
		return bOk;
	}
	
	for (int i=0; i<iListOfRows.count(); i++)
	{
		QStringList curRow = iListOfRows.at(i);
		if (curRow.count() != iNbColumns)
		//if (curRow.count() < iNbColumns) //Major change done for EF Application
			continue;
		
		for (int j=0; j<iNbColumns; j++)
		{
			QVariant vCurCol(curRow.at(j));
			QString sValueDataType=iListOfColDataType.at(j);
			if (vCurCol.toString()=="")
				qry.bindValue(listOfKeys.at(j), vCurCol.toString());
			else if(DT_DOUBLE ==sValueDataType )
				qry.bindValue(listOfKeys.at(j), vCurCol.toDouble());
			else if (DT_INT == sValueDataType)
				qry.bindValue(listOfKeys.at(j), vCurCol.toInt());
			else if (DT_BOOL == sValueDataType)
				qry.bindValue(listOfKeys.at(j), vCurCol.toBool());
			else
				qry.bindValue(listOfKeys.at(j), vCurCol.toString());	
		}

		bOk = qry.exec();
	}
	
	endTransaction();

	return bOk;
}


bool AINCMNDatabaseManager::createEmptyTable(const QString& iTableName, 
						const QStringList& iListOfColNames, const QStringList& iListOfUniqueCols)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;
	
	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	bOk = qry.exec("DROP TABLE IF EXISTS " + iTableName);
	if (! bOk )
	{
		endTransaction();
		return bOk;
	}
	
	QString sUniqueColStr;
	for (int i=0; i<iListOfUniqueCols.count(); i++)
	{
		if (i)
			sUniqueColStr += ",";
			
		sUniqueColStr += iListOfUniqueCols.at(i);
	}
	
	QString sCreateTable = _pDbQueries->createTableString(iListOfColNames, iTableName, sUniqueColStr);
	bOk = qry.exec(sCreateTable);
		
	endTransaction();

	qry.clear();
		
	return bOk;
}

bool AINCMNDatabaseManager::createColumn(const QString& iTableName, const QString& iColumnName,
				 const QString& iDataType)
{
	bool bOk = startTransaction();
	if (! bOk)
		return bOk;
	QSqlQuery query = QSqlQuery(*_pDb);
	query.setForwardOnly(true);
	
	QString alterTblStr ="" ;

	
	alterTblStr = "ALTER TABLE " + D_QUOTE + iTableName + D_QUOTE + " add column ";
	alterTblStr = alterTblStr +  D_QUOTE+iColumnName+D_QUOTE + QString(" ")+D_QUOTE+iDataType +D_QUOTE;
	
	bOk = query.exec(alterTblStr);
	endTransaction();
	return bOk;
}

void AINCMNDatabaseManager::createColumns(const QString& iTableName, const QStringList& iListofColumnNames,
				 const QStringList& iDataTypes)
{
	QStringList lstOfCreatedCols = getListOfColumns(iTableName);
	for (int i=0; i<iListofColumnNames.count(); i++)
	{
		if (! lstOfCreatedCols.contains(iListofColumnNames.at(i)))
		{
			bool bOk = createColumn(iTableName, iListofColumnNames.at(i),iDataTypes.at(i) );
			if (bOk)
				lstOfCreatedCols.append(iListofColumnNames.at(i));
		}
	}	
}

bool AINCMNDatabaseManager::updateRecords(const QString& iTblName, const QHash<QString, QString> &ihashWhereAttributes,
										  const QHash<QString, QString>& iHshOfColsToUpdate)
{
	bool bOk = startTransaction();
	if (! bOk)
		return bOk;

	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	QString sUpdateStr="UPDATE " + iTblName + " SET "; 
	
	QStringList lstOfColsToUpdate = iHshOfColsToUpdate.keys();
	for (int i=0; i<lstOfColsToUpdate.count(); i++)
	{
		QString sColToUpdate = lstOfColsToUpdate.at(i);
		QString sColVal = iHshOfColsToUpdate.value(sColToUpdate);
		
		if (i > 0)
			sUpdateStr += ", ";

		sUpdateStr += (D_QUOTE + sColToUpdate + D_QUOTE + "=" + D_QUOTE + sColVal + D_QUOTE);
	}
	
	sUpdateStr += " WHERE ";

	QStringList lstOfWhrCols = ihashWhereAttributes.keys();
	for (int i=0; i<lstOfWhrCols.count(); i++)
	{
		QString sWhereCol = lstOfWhrCols.at(i);
		QString sWhereColVal = ihashWhereAttributes.value(sWhereCol);

		if (i > 0)
			sUpdateStr += " AND ";

		sUpdateStr += (D_QUOTE + sWhereCol + D_QUOTE + "=" + D_QUOTE + sWhereColVal + D_QUOTE);
	}
	
	bOk = qry.exec(sUpdateStr);

	qry.clear();

	endTransaction();
	return bOk;
}

bool AINCMNDatabaseManager::isDbOpen()
{
	bool bOk(false);
	bOk = _pDb->isOpen();
	return bOk;
}

QList<QHash<QString, QVariant>> AINCMNDatabaseManager::getValues(const QString& iTableName, const QStringList& iSelectAttrList, 
										  const QString& iWhereAttr, const QStringList& iWhereAttrValList,
										  bool bWhrAttrValNum)
{
	QList<QHash<QString, QVariant>> listOfValues;

	if (isTablePresent(iTableName))
	{
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);

		QString qryStr = _pDbQueries->createSelectString(iTableName, iSelectAttrList, iWhereAttr, iWhereAttrValList, bWhrAttrValNum);
		bool bOk = selectQry.exec(qryStr);

		if ( bOk )
		{
			while (selectQry.next())
			{
				QHash<QString, QVariant> hashOfValues;
				for (int i=0; i<iSelectAttrList.count(); i++)
				{
					QVariant vId = selectQry.record().value(iSelectAttrList.at(i));
					hashOfValues.insert(iSelectAttrList.at(i), vId);
				}			
				listOfValues.append(hashOfValues);
			}
		}
		else
		{
			showLastError("Error: getValues3 for "+iTableName);
		}
	}

	return listOfValues;
}

bool AINCMNDatabaseManager::isEntryExisting(const QString& iTableName, const QStringList& iListofColumnNames,
											const QStringList& iListofColumnValues, const QList<bool> ibColValNumList)
{
	bool bIsIt(false);
	
	if (isTablePresent(iTableName))
	{
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);

		if (iListofColumnNames.count() == iListofColumnValues.count())
		{			
			QString qryStr = _pDbQueries->createSelectString(iTableName, iListofColumnNames, iListofColumnNames, iListofColumnValues, ibColValNumList);
			bool bOk = selectQry.exec(qryStr);

			if ( bOk )
			{
				while (selectQry.next())
				{
					bIsIt = true;
					break;
				}
			}
			else {
				bIsIt = false;
				//Error Log
			}
		}
		else {
			//Error Log
		}
	}

	return bIsIt;
}

bool AINCMNDatabaseManager::isEntryExisting(const QString& iTableName, 
									const QHash<QString, QString>& iHshWhereColVals, const QHash<QString, bool>& iHshWhrAttrValNum)
{
	bool bIsIt(false);

	if (isTablePresent(iTableName))
	{
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);
		
		QStringList lstKeys = iHshWhereColVals.keys();
		
		if (lstKeys.count())
		{
			QStringList lstSelCols = QStringList() << lstKeys.at(0);
			QString qryStr = _pDbQueries->createSelectString(iTableName, lstSelCols, iHshWhereColVals, iHshWhrAttrValNum);
			bool bOk = selectQry.exec(qryStr);
			if ( bOk )
			{
				while (selectQry.next())
				{
					bIsIt = true;
					break;
				}
			}
			else {
				//Error Log
			}
		}
		else {
			//Error Log
		}
	}

	return bIsIt;
}

bool AINCMNDatabaseManager::deleteRecords(const QString& iTableName, const QStringList& iListofColumnNames,
															const QStringList& iListofColumnValues, const QList<bool> ibColValNumList)
{
	bool bOk(true);

	if (isTablePresent(iTableName))
	{
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);

		QString qryStr = _pDbQueries->createDeleteString(iTableName, iListofColumnNames, iListofColumnValues, ibColValNumList);
		bOk = selectQry.exec(qryStr);

	}

	return bOk;
}


bool AINCMNDatabaseManager::deleteRecords(const QString& iTableName, 
				const QHash<QString, QString>& iHshWhereAttrVals, const QHash<QString, bool>& iHshWhrAttrValNum)
{
	bool bOk(true);

	if (isTablePresent(iTableName))
	{
		QSqlQuery selectQry(*_pDb);
		selectQry.setForwardOnly(true);

		QString qryStr = _pDbQueries->createDeleteString(iTableName, iHshWhereAttrVals, iHshWhrAttrValNum);
		bOk = selectQry.exec(qryStr);
	}

	return bOk;
}

QList<bool> AINCMNDatabaseManager::getDefaultValOfColDTLst(const QStringList & listofCols)
{
	QList<bool> listOfValNum;

	for (int i=0; i<listofCols.count(); i++)
	{
		listOfValNum.append(false);
	}

	return listOfValNum;
}

bool AINCMNDatabaseManager::insertRowsInTable(const QString& iTableName, 
											  const QList<QHash<QString, QString>>& iListOfRows)
{
	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;

	if (iListOfRows.count())
	{
		QHash<QString,QString> hashOfColumns = iListOfRows.at(0);
		QStringList listOfColNames = hashOfColumns.keys();
		int iNbColumns = listOfColNames.count();

		QStringList listOfKeys = _pDbQueries->createKeyNamesList(iNbColumns);
		QString sInsertStr = _pDbQueries->createInsertIntoQryString(listOfColNames, iTableName, listOfKeys);
		QSqlQuery qry(*_pDb);
		qry.setForwardOnly(true);
		bOk = qry.prepare(sInsertStr);
		if (! bOk)
		{
			endTransaction();
			return bOk;
		}

		for (int i=0; i<iListOfRows.count(); i++)
		{
			QHash<QString,QString> hashOfColumnValues = iListOfRows.at(i);

			for (int j=0; j<listOfColNames.count(); j++)
			{
				QString sColumnName = listOfColNames.at(j);
				QString sColumnValue = hashOfColumnValues.value(sColumnName);
				qry.bindValue(listOfKeys.at(j), sColumnValue);			
			}

			bOk = qry.exec();
		}
	}

	bOk = endTransaction();

	return bOk;
}

void AINCMNDatabaseManager::splitIndexName(QString& oTableName, QString& oColName)
{
	
}

QString AINCMNDatabaseManager::formIndexName(const QString& iTableName, const QString& iColName)
{
	QString sIdxName("index" + QString(INDEX_NAME_SPLITTER) + iTableName + INDEX_NAME_SPLITTER + iColName + QString(INDEX_NAME_SPLITTER) + "1");

	return sIdxName;
}

bool AINCMNDatabaseManager::createIndex(const QString& iTableName, const QString& iColName)
{
	if (! _pDb)
		return false;

	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;
	
	QSqlQuery qry(*_pDb);
	qry.setForwardOnly(true);
	
	QString sIndexName = formIndexName(iTableName, iColName);

	QString sQryStr("create index if not exists " + sIndexName + " on " + iTableName + "(" + iColName + ")");
	bOk = qry.exec(sQryStr);

	bOk = endTransaction();

	return bOk;
}

bool AINCMNDatabaseManager::dropIndex(const QString& iTableName, const QString& iColName)
{
	if (! _pDb)
		return false;

	bool bOk = startTransaction();	
	if (! bOk)
		return bOk;

	QSqlQuery qry(*_pDb);
	qry.setForwardOnly(true);

	QString sIndexName = formIndexName(iTableName, iColName);

	QString sQryStr("drop index if exists " + sIndexName);
	bOk = qry.exec(sQryStr);

	bOk = endTransaction();

	return bOk;
}

void AINCMNDatabaseManager::createIndexes(const QHash<QString, QString>& hshOfTblName_ColName)
{
	updateIndexes(hshOfTblName_ColName, true);
}

void AINCMNDatabaseManager::dropIndexes(const QHash<QString, QString>& hshOfTblName_ColName)
{
	updateIndexes(hshOfTblName_ColName, false);
}

void AINCMNDatabaseManager::updateIndexes(const QHash<QString, QString>& hshOfTblName_ColName, bool bCreateIdx)
{
	bool bOk(true);
	if (! _pDb)
		return;

	QStringList lstOfTabels = hshOfTblName_ColName.keys();
	lstOfTabels.removeDuplicates();

	for (int i =0; i<lstOfTabels.count(); i++)
	{
		QString sTableName = lstOfTabels.at(i);
		QStringList lstOfCols = hshOfTblName_ColName.values(sTableName);
		lstOfCols.removeDuplicates();
		for (int j=0; j<lstOfCols.count(); j++)
		{
			QString sColName = lstOfCols.at(j);
			if ( bCreateIdx )
				bOk = createIndex(sTableName, sColName);
			else
				bOk = dropIndex(sTableName, sColName);
		}
	}
}

bool AINCMNDatabaseManager::dropTable(const QString& sTableName)
{
	bool bOk(true);
	if (! _pDb)
		return false;
	
	bOk = startTransaction();	
	if (! bOk)
		return bOk;

	QSqlQuery qry(*_pDb);
	qry.setForwardOnly(true);

	QString sQryStr("drop table if exists " + sTableName);
	bOk = qry.exec(sQryStr);

	bOk = endTransaction();


	return bOk;
}

void AINCMNDatabaseManager::dropAllTables()
{
	bool bOk(true);
	
	QStringList lstOfTables;
	getListOfTables(lstOfTables);

	for (int i=0; i<lstOfTables.count(); i++)
	{
		QString sTableName = lstOfTables.at(i);
		bOk = dropTable(sTableName);
	}
}
QString AINCMNDatabaseManager::getPrimaryKey(const QString& iTableName)
{
	QString sPrimaryKey="";

	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	QString sQuertyString = "pragma table_info(" + iTableName +")";
	bool bOk = qry.exec( sQuertyString);

	while(qry.next())
	{
		QString sPrimaryKeyStatus = qry.value(TABLE_INFO_COL_PRIMARY_KEY_INDEX).toString();
		if (sPrimaryKeyStatus==TABLE_INFO_IS_PRIMARY_KEY_IDENTIFIER)
		{
			sPrimaryKey = qry.value(1).toString();
		}
	}
	return sPrimaryKey;
}
QStringList AINCMNDatabaseManager::getIndexedColoumns(const QString& iTableName)
{
	QStringList IndexedColoums;

	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	QString sQuertyString = "pragma index_list(" + iTableName +")";
	bool bOk = qry.exec( sQuertyString);

	//get list of index name.
	QStringList lstIndexName;
	while(qry.next())
	{
		QString sIndexName = qry.value(TABLEINDEX_LIST_COL_NAME_INDEX).toString();
		if (!sIndexName.isEmpty())
		{
			if (!sIndexName.contains("sqlite_autoindex"))
				lstIndexName<<sIndexName;
			
		}
	}

	sQuertyString = "pragma table_info(" + iTableName +")";
	bOk = qry.exec( sQuertyString);
	while(qry.next())
	{
		QString colname = qry.value(TABLE_INFO_COL_NAME_INDEX).toString();
		if (!colname.isEmpty())
		{
			
			foreach(QString sIndexName, lstIndexName)
			{
				if (sIndexName.contains(colname))
					IndexedColoums<<colname;

			}
				

		}
	}

	return IndexedColoums;

}

QString AINCMNDatabaseManager::getColumnDataType(const QString& iTableName,const QString& iColName )
{
	QString sDataType="";

	QSqlQuery qry = QSqlQuery(*_pDb);
	qry.setForwardOnly(true);

	QString sQuertyString = "pragma table_info(" + iTableName +")";
	bool bOk = qry.exec( sQuertyString);

	while(qry.next())
	{
		QString sCurrCol = qry.value(TABLE_INFO_COL_NAME_INDEX).toString();
		if (sCurrCol==iColName)
		{
			sDataType = qry.value(TABLE_INFO_COL_TYPE_INDEX).toString();
		}
	}
	return sDataType;
}

