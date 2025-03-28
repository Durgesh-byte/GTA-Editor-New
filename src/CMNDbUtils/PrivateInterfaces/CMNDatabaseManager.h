#ifndef AINCMNDATABASEMANAGER_H_
#define AINCMNDATABASEMANAGER_H_

#include "AINCMNDbUtils.h"
#include "AINCMNDbQueries.h"




#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QList>

class QSqlDatabase;
namespace AINCMN
{
    namespace Dialogs
    {
        class AINCMNProgressDialog;
    }
}

using namespace AINCMN::Dialogs;

namespace AINCMN
{
    namespace DbUtils
    {
        class AINCMNDbUtils_SHARED_EXPORT AINCMNDatabaseManager : public QObject
        {
		public:
			AINCMNDatabaseManager(const QString& iDbFilePath, QObject *parent = 0);
            AINCMNDatabaseManager(QSqlDatabase * ipSqlDatabase, QObject *parent = 0);
			virtual ~AINCMNDatabaseManager();
		        							
			QSqlDatabase * getDatabase();
			
			bool startTransaction();
			
			bool endTransaction();
			
			void getListOfTables(QStringList & ioListofTables);
			
			bool isTablePresent(const QString& iTableName) const;
			
			QString getDatabaseAbsFile();
					
			QList<QStringList> getTableContent(const QString& iTableName, 
										const QStringList& iTableColumns);
										
			QList<QHash<QString, QString>> getTableContent(const QString& iTableName);
			
			QStringList getListOfColumns(const QString& iTableName);

			QList<QHash<QString, QVariant>> getValues(const QString& iTableName, const QStringList& iSelectAttrList);
			
			QList<QVariant> getValues(const QString& iTableName, const QString& iSelectAttr);
			
                         //Single Select Attributes + Single Where Attr + All Where Attr either number/string
            QList<QVariant> getValues(const QString& iTableName, const QString& iSelectAttr, 
					const QString& iWhereAttr, const QString& iWhereAttrVal, bool bWhrAttrValNum=true);						
			//Multiple Select Attributes + Single Where Attr + All Where Attr either number/string
            QList<QHash<QString, QVariant>> getValues(const QString& iTableName, const QStringList& iSelectAttrList, 
					const QString& iWhereAttr, const QString& iWhereAttrVal, bool bWhrAttrValNum=true);
			
			//Multiple Select + Single Where Attr & Multiple Values + All Where Attr either number/string
			QList<QHash<QString, QVariant>> getValues(const QString& iTableName, const QStringList& iSelectAttrList, 
				const QString& iWhereAttr, const QStringList& iWhereAttrValList, bool bWhrAttrValNum=true);
			
			//Multiple Select + Hash of Where Attr + All Where Attr either number/string
			QList<QHash<QString, QVariant>> getValues(const QString& sTable, const QStringList& iLstOfSeletctAttrs,
				const QHash<QString, QString>& ihshOfWhereAttrs, bool bWhereAttrNum=true);
			
			//Multiple Select + Hash of Where Attr + Hash of Where Attr DT (string/number)
			QList<QHash<QString, QVariant>> getValues(const QString& sTable, const QStringList& iLstOfSeletctAttrs,
				const QHash<QString, QString>& ihshOfWhereAttrs, const QHash<QString, bool>& iHshOfWhrAttrDTNum);
			
			//Multiple Select & Multiple Where
			QList<QHash<QString, QVariant>> getValues(const QString& iTableName, const QStringList& iSelectAttrList, 
				const QStringList& iWhereAttrList, const QStringList& iWhereAttrValList, const QList<bool>& ibWhrAttrValNumList);
			
			void splitIndexName(QString& oTableName, QString& oColName);
			QString formIndexName(const QString& iTableName, const QString& iColName);
			bool createIndex(const QString& iTableName, const QString& iColName);
			bool dropIndex(const QString& iTableName, const QString& iColName);

			void createIndexes(const QHash<QString, QString>& hshOfTblName_ColName);
			void dropIndexes(const QHash<QString, QString>& hshOfTblName_ColName);
			
			bool dropTable(const QString& sTableName);
			
			void dropAllTables();
		public:

			
			bool createTableInDb(const QString& iTableName, 
							const QList<QHash<QString, QString>>& iListOfTableRowsWithHeader);
			
			bool createTableInDb(const QString& iTableName, 
						const QList<QStringList>& iListOfTableRowsWithHeader, AINCMNProgressDialog *pProgBar = NULL);
						
			bool alterTableName(const QString& iTableName, const QString& iNewTableName);
			
			void showLastError(const QString& iMsgTitle);

			bool createEmptyTable(const QString& iTableName, const QStringList& iListOfColNames);

			bool createEmptyTablePk(const QString& iTableName, const QStringList& iListOfColNames, const QString& sPK);
			
			bool createEmptyTableDT(const QString& iTableName, const QStringList& iListOfColNames, 
															const QStringList& iListOfColDataType);
			bool createEmptyTablePKDT(const QString& iTableName, const QStringList& iListOfColNames, 
									 const QStringList& iListOfColDataType,const QString& sPK);
			bool createEmptyTable(const QString& iTableName, const QStringList& iListOfColNames, 
																const QStringList& iListOfUniqueCols);
			 
			//All inserted value VARCHAR type
			bool insertRowsInTable(const QString& iTableName, 
								const QStringList& iListOfColNames, const QList<QStringList>& iListOfRows);

			bool insertRowsInTable(const QString& iTableName, 
				const QStringList& iListOfColNames, const QStringList& iListOfColDataType,
				const QList<QStringList>& iListOfRows);

			bool insertRowsInTable(const QString& iTableName, const QList<QHash<QString, QString>>& iListOfRows);

			bool createColumn(const QString& iTableName, const QString& iColumnName,
																const QString& iDataType);
			void createColumns(const QString& iTableName, const QStringList& iListofColumnNames,
																const QStringList& iDataTypes);

			bool updateRecords(const QString& iTblName, const QHash<QString,QString> &hashWhereAttributes,
										  const QHash<QString, QString>& ihashOfColumns	);
			bool isDbOpen();

			bool isEntryExisting(const QString& iTableName, const QStringList& iListofColumnNames,
														  const QStringList& iListofColumnValues, const QList<bool> ibColValNumList);

			bool isEntryExisting(const QString& iTableName, 
							const QHash<QString, QString>& iHshWhereColVals, const QHash<QString, bool>& iHshWhrAttrValNum);

			bool deleteRecords(const QString& iTableName, const QStringList& iListofColumnNames,
												const QStringList& iListofColumnValues, const QList<bool> ibColValNumList);

			bool deleteRecords(const QString& iTableName, const QHash<QString, QString>& iHshWhereAttrVals, 
																		const QHash<QString, bool>& iHshWhrAttrValNum);

			QList<bool> getDefaultValOfColDTLst(const QStringList & listofCols);
			QList<QHash<QString, QString>> getTableInfo(const QString& iTableName);
			QString getPrimaryKey(const QString& iTableName);
			QStringList getIndexedColoumns(const QString& iTableName);
			QString getColumnDataType(const QString& iTableName,const QString& iColName );

		private:
			bool openDatabase(QString iDbFilePath=0);
			void updateIndexes(const QHash<QString, QString>& hshOfTblName_ColName, bool bCreateIdx);
			
			QString _sDbFilePath;
            bool _bCloseDb;
            AINCMNDbQueries * _pDbQueries;
		
		public:
			QSqlDatabase * _pDb;
        };
    }
}
#endif // AINCMNDATABASEMANAGER_H_
