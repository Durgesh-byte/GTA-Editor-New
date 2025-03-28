#ifndef AINCMNDBQUERIES_H_
#define AINCMNDBQUERIES_H_

#include <QObject>
#include <QString>
#include <QStringList>

#include "AINCMNDbUtils.h"

namespace AINCMN
{
    namespace DbUtils
    {
        class AINCMNDbUtils_SHARED_EXPORT AINCMNDbQueries : public QObject
        {
            Q_OBJECT
        public:
            AINCMNDbQueries(QObject *parent = 0);
            ~AINCMNDbQueries();

            QString createSelectString(const QString& iTableName, const QString& iSelectAttr, 
						const QString& iWhereAttr, const QString& iWhereAttrVal, bool bWhrAttrValNum=true);
						
            QString createSelectString(const QString& iTableName, const QString& iSelectAttr);

            QString createSelectString(const QString& iTableName, const QStringList& iSelectAttrList, 
						const QString& iWhereAttr, const QString& iWhereAttrVal, bool bWhrAttrValNum=true);

			QString createSelectString(const QString& iTableName, const QStringList& iSelectAttrList, 
						const QStringList& iWhereAttrList, const QStringList& iWhereAttrValList,const QList<bool>& ibWhrAttrValNumList);
			
			QString createSelectString(const QString& iTableName, const QStringList& iSelectAttrList, 
								const QHash<QString, QString>& iHshWhereAttrVals, const QHash<QString, bool>& iHshWhrAttrValNum);

            QString createSelectString(const QString& iTableName, const QStringList& iSelectAttrList);

			QString createSelectString(const QString& iTableName, const QStringList& iSelectAttrList, 
				const QString& iWhereAttr, const QStringList& iWhereAttrValList, bool bWhrAttrValNum=true);
            
			QString createTableString(const QStringList& iListofColumnNames, const QString& iNewTblName);

			QString createTableStringPk(const QString& iNewTblName, const QStringList& iListofColumnNames,
				const QString& iPrimaryKey);
			
			QString createTableString(const QString& iNewTblName, const QStringList& iListOfColNames, 
																	const QStringList& iListOfColDataType);
			
			//this function will also create one more col, auto incremented, with the name $(TABLENAME)_ID	
			QString createTableString( const QStringList& iListofColumnNames, 
									const QString& iNewTblName, const QString& iUniqueNames);

			QString createInsertIntoQryString(const QHash<int, QString>& ihashOfColumns, 
				const QString& iTblName, const QStringList& iListofKeys);
												
			QString createInsertIntoQryString(const QStringList& iListofColumnNames, 
									const QString& iNewTblName, const QStringList& iListofKeys);
								
			QStringList createKeyNamesList(int iNumOfCols);
			
			QString alterTableNameQryStr(const QString& iTableName, const QString& iNewTableName);

			QString createDeleteString(const QString& iTableName, const QStringList& iWhereAttrList,
									const QStringList& iWhereAttrValList, const QList<bool> ibWhrAttrValNumList);
			
			QString createDeleteString(const QString& iTableName, 
				const QHash<QString, QString>& iHshWhereAttrVals, const QHash<QString, bool>& iHshWhrAttrValNum);
			QString createTableStringPkDT(const QString& iNewTblName, const QStringList& iListofColumnNames,const QStringList& iListofColumnTypes,
				const QString& iPrimaryKey);
        signals:

        public slots:

        };
    }
}
#endif // AINCMNDBQUERIES_H_
