#include "AINCMNDbQueries.h"
using namespace AINCMN::DbUtils;

#include <QHash>
#include <QVariant>

AINCMNDbQueries::AINCMNDbQueries(QObject *parent) :
    QObject(parent)
{
}

AINCMNDbQueries::~AINCMNDbQueries()
{

}

QString AINCMNDbQueries::createSelectString(const QString& iTableName, const QString& iSelectAttr, 
		const QString& iWhereAttr, const QString& iWhereAttrVal, bool bWhrAttrValNum)
{
	QString qryStr;
	if (bWhrAttrValNum)
        qryStr = QString("SELECT DISTINCT " + iSelectAttr + " FROM " + iTableName + " WHERE " + iWhereAttr + "=" + iWhereAttrVal);
	else
		qryStr = QString("SELECT DISTINCT " + iSelectAttr + " FROM " + iTableName + " WHERE " + iWhereAttr + "=\"" + iWhereAttrVal + "\"");
	
	return qryStr;
}

QString AINCMNDbQueries::createSelectString(const QString& iTableName, const QString& iSelectAttr)
{
    QString qryStr("SELECT DISTINCT " + iSelectAttr + " FROM " + iTableName);

    return qryStr;
}

QString AINCMNDbQueries::createSelectString(const QString& iTableName, const QStringList& iSelectAttrList, 
		const QString& iWhereAttr, const QString& iWhereAttrVal, bool bWhrAttrValNum)
{
    QString qryStr("SELECT ");
    for (int i=0; i<iSelectAttrList.count(); i++)
    {
            qryStr += iSelectAttrList.at(i);
            if (i < iSelectAttrList.count()-1)
                    qryStr +=", ";
    }
    if (bWhrAttrValNum)
		qryStr = qryStr + " FROM " + iTableName + " WHERE " + iWhereAttr + "=" + iWhereAttrVal; 
    else
		qryStr = qryStr + " FROM " + iTableName + " WHERE " + iWhereAttr + "=\"" + iWhereAttrVal+ "\"";
    
    return qryStr;
}

QString AINCMNDbQueries::createSelectString(const QString& iTableName, const QStringList& iSelectAttrList, 
						const QStringList& iWhereAttrList, const QStringList& iWhereAttrValList, const QList<bool>& ibWhrAttrValNumList)
{
	QString qryStr("SELECT ");
    for (int i=0; i<iSelectAttrList.count(); i++)
    {
            qryStr += iSelectAttrList.at(i);
            if (i < iSelectAttrList.count()-1)
                    qryStr +=", ";
    }
	
	QString sCondition;
	for(int i=0;i<iWhereAttrList.count();i++)
	{
		QString sWhereAttr = iWhereAttrList.at(i);
		QString sWhereAttrVal = iWhereAttrValList.at(i);
		bool bWhrAttrValNum  = ibWhrAttrValNumList.at(i);
		
		if (i > 0)
			sCondition += " AND ";
		if (bWhrAttrValNum)
			sCondition = sCondition + sWhereAttr + "=" + sWhereAttrVal;
		else
			sCondition = sCondition + sWhereAttr + "=\"" + sWhereAttrVal + "\"";	
	}
			
	qryStr = qryStr + " FROM " + iTableName + " WHERE "+ sCondition  ;

	return qryStr;
}

QString AINCMNDbQueries::createSelectString(const QString& iTableName, const QStringList& iSelectAttrList, 
					const QHash<QString, QString>& iHshWhereAttrVals, const QHash<QString, bool>& iHshWhrAttrValNum)
{
	QString qryStr("SELECT ");
	for (int i=0; i<iSelectAttrList.count(); i++)
	{
		qryStr += iSelectAttrList.at(i);
		if (i < iSelectAttrList.count()-1)
			qryStr +=", ";
	}

	QString sCondition("");

	QStringList lstOfWhereAttrs1 = iHshWhereAttrVals.keys();
	QStringList lstOfWhereAttrs2 = iHshWhrAttrValNum.keys();
	if (lstOfWhereAttrs1.count() != lstOfWhereAttrs2.count())
		return sCondition;

	for(int i=0;i<lstOfWhereAttrs1.count();i++)
	{		
		QString sWhereAttr = lstOfWhereAttrs1.at(i);
		QString sWhereAttrVal = iHshWhereAttrVals.value(sWhereAttr);
		bool bWhrAttrValNum  = iHshWhrAttrValNum.value(sWhereAttr);

		if (i > 0)
			sCondition += " AND ";
		if (bWhrAttrValNum)
			sCondition = sCondition + sWhereAttr + "=" + sWhereAttrVal;
		else
			sCondition = sCondition + sWhereAttr + "=\"" + sWhereAttrVal + "\"";	
	}

	qryStr = qryStr + " FROM " + iTableName + " WHERE "+ sCondition  ;

	return qryStr;
}

QString AINCMNDbQueries::createSelectString(const QString& iTableName, const QStringList& iSelectAttrList)
{
        QString qryStr("SELECT DISTINCT ");
        for (int i=0; i<iSelectAttrList.count(); i++)
        {
                qryStr += iSelectAttrList.at(i);
                if (i < iSelectAttrList.count()-1)
                        qryStr +=", ";
        }
        qryStr += (" FROM " + iTableName);

        return qryStr;
}

QString AINCMNDbQueries::createTableString(const QStringList& iListofColumnNames, 
																const QString& iNewTblName)
{
	QString createTblStr = "CREATE TABLE " + iNewTblName + "(";
	for (int i=0; i<iListofColumnNames.count(); i++)
	{
		if ( i )
			createTblStr = createTblStr + ", " + iListofColumnNames.at(i) + " varchar(255)";
		else
			createTblStr = createTblStr + iListofColumnNames.at(i) + " varchar(255)";
	}
	createTblStr = createTblStr + ")";

	return createTblStr;
}

QString AINCMNDbQueries::createTableStringPk(const QString& iNewTblName, const QStringList& iListofColumnNames,
										   const QString& iPrimaryKey)
{
	QString createTblStr = "CREATE TABLE " + iNewTblName + "(";
	for (int i=0; i<iListofColumnNames.count(); i++)
	{
		QString sCurCol = iListofColumnNames.at(i);
		
		if ( i )
			createTblStr = createTblStr + ", " + sCurCol + " varchar(255)";
		else
			createTblStr = createTblStr + sCurCol + " varchar(255)";

		if (iPrimaryKey == sCurCol)
			createTblStr += (" PRIMARY KEY ASC");
	}
	createTblStr = createTblStr + ")";

	return createTblStr;
}

QString AINCMNDbQueries::createTableString(const QString& iNewTblName, const QStringList& iListofColumnNames, 
					   const QStringList& iListOfColDataType)
{
	QString createTblStr = "CREATE TABLE " + iNewTblName + "(";
	for (int i=0; i<iListofColumnNames.count(); i++)
	{
		if ( i )
			createTblStr = createTblStr + ", " + iListofColumnNames.at(i) + " " + iListOfColDataType.at(i);
		else
			createTblStr = createTblStr + iListofColumnNames.at(i) + " " + iListOfColDataType.at(i);
	}
	createTblStr = createTblStr + ")";

	return createTblStr;
}

QString AINCMNDbQueries::createTableString(const QStringList& iListofColumnNames, 
									const QString& iNewTblName, const QString& iUniqueNames)
{
	QString createTblStr = "CREATE TABLE " + iNewTblName + "(" + iNewTblName + "_ID INTEGER PRIMARY KEY AUTOINCREMENT";
	for (int i=0; i<iListofColumnNames.count(); i++)
	{
		createTblStr = createTblStr + ", " + iListofColumnNames.at(i) + " varchar(255)";
	}
	createTblStr = createTblStr + ", UNIQUE (" + iUniqueNames +"))";

	return createTblStr;
}

QString AINCMNDbQueries::createInsertIntoQryString(const QHash<int, QString>& ihashOfColumns, 
											const QString& iTblName, const QStringList& iListofKeys)
{
	QString sInsertStr = "INSERT INTO " + iTblName + " (";
	for (int i=0; i<ihashOfColumns.count(); i++)
	{
		if (0 == i)
			sInsertStr = sInsertStr + ihashOfColumns.value(i);
		else
			sInsertStr = sInsertStr + ", " + ihashOfColumns.value(i);
	}
	sInsertStr =  sInsertStr + ") VALUES (";

	for (int i=0; i<iListofKeys.count(); i++)
	{
		if (0 == i)
			sInsertStr = sInsertStr + iListofKeys.at(i);
		else
			sInsertStr = sInsertStr + ", " + iListofKeys.at(i);
	}
	sInsertStr =  sInsertStr + ")";

	return sInsertStr;
}

QString AINCMNDbQueries::createInsertIntoQryString(const QStringList& iListofColumnNames, 
										const QString& iNewTblName, const QStringList& iListofKeys)
{
	QString sInsertStr = "INSERT INTO " + iNewTblName + " (";
	for (int i=0; i<iListofColumnNames.count(); i++)
	{
		if (0==i)
			sInsertStr = sInsertStr + iListofColumnNames.at(i);
		else
			sInsertStr = sInsertStr + ", " + iListofColumnNames.at(i);
	}
	sInsertStr =  sInsertStr + ") VALUES (";

	for (int i=0; i<iListofColumnNames.count(); i++)
	{
		if (0==i)
			sInsertStr = sInsertStr + iListofKeys.at(i);
		else
			sInsertStr = sInsertStr + ", " + iListofKeys.at(i);
	}
	sInsertStr =  sInsertStr + ")";

	return sInsertStr;
}

QStringList AINCMNDbQueries::createKeyNamesList(int iNumOfCols)
{
	QStringList oListofKeys;
	for (int i=0; i<iNumOfCols; i++)
	{
		QVariant vNum(i);
		oListofKeys.append(QString(":Key")+ vNum.toString());
	}	
	return oListofKeys;
}


QString AINCMNDbQueries::alterTableNameQryStr(const QString& iTableName, const QString& iNewTableName)
{
	QString qryStr("ALTER TABLE " + iTableName + " RENAME TO " + iNewTableName);
	
	return qryStr;
}


QString AINCMNDbQueries::createSelectString(const QString& iTableName, const QStringList& iSelectAttrList, 
											 const QString& iWhereAttr, const QStringList& iWhereAttrValList, bool bWhrAttrValNum)
{
	QString qryStr("SELECT DISTINCT ");
	for (int i=0; i<iSelectAttrList.count(); i++)
	{
		qryStr += iSelectAttrList.at(i);
		if (i < iSelectAttrList.count()-1)
			qryStr +=", ";
	}
	QString sCondition;
	for(int i=0;i<iWhereAttrValList.count();i++)
	{
		QString sWhereAttrVal = iWhereAttrValList.at(i);

		if (i > 0)
			sCondition += " OR ";
		if (bWhrAttrValNum)
			sCondition = sCondition + iWhereAttr + "=" + sWhereAttrVal;
		else
			sCondition = sCondition + iWhereAttr + "=\"" + sWhereAttrVal + "\"";	
	}

	qryStr = qryStr + " FROM " + iTableName + " WHERE "+ sCondition  ;

	return qryStr;
}

QString AINCMNDbQueries::createDeleteString(const QString& iTableName, const QStringList& iWhereAttrList, 
											const QStringList& iWhereAttrValList,const QList<bool> ibWhrAttrValNumList)
{
	QString qryStr("DELETE");

	QString sCondition("");
	for(int i=0;i<iWhereAttrList.count();i++)
	{
		QString sWhereAttr = iWhereAttrList.at(i);
		QString sWhereAttrVal = iWhereAttrValList.at(i);
		bool bWhrAttrValNum  = ibWhrAttrValNumList.at(i);

		if (i > 0)
			sCondition += " AND ";
		if (bWhrAttrValNum)
			sCondition = sCondition + sWhereAttr + "=" + sWhereAttrVal;
		else
			sCondition = sCondition + sWhereAttr + "=\"" + sWhereAttrVal + "\"";	
	}

	qryStr = qryStr + " FROM " + iTableName + " WHERE "+ sCondition  ;

	return qryStr;

}


QString AINCMNDbQueries::createDeleteString(const QString& iTableName, 
			const QHash<QString, QString>& iHshWhereAttrVals, const QHash<QString, bool>& iHshWhrAttrValNum)
{
	QString sCondition("");
	
	QStringList lstOfWhereAttrs1 = iHshWhereAttrVals.keys();
	QStringList lstOfWhereAttrs2 = iHshWhrAttrValNum.keys();
	if (lstOfWhereAttrs1.count() == lstOfWhereAttrs2.count())
	{	
		for(int i=0;i<lstOfWhereAttrs1.count();i++)
		{
			QString sWhereAttr = lstOfWhereAttrs1.at(i);
			QString sWhereAttrVal = iHshWhereAttrVals.value(sWhereAttr);
			bool bWhrAttrValNum  = iHshWhrAttrValNum.value(sWhereAttr);

			if (i > 0)
				sCondition += " AND ";
			if (bWhrAttrValNum)
				sCondition = sCondition + sWhereAttr + "=" + sWhereAttrVal;
			else
				sCondition = sCondition + sWhereAttr + "=\"" + sWhereAttrVal + "\"";	
		}
	}
	else {
		//Error Log
	}

	QString qryStr = "DELETE FROM " + iTableName + " WHERE "+ sCondition  ;

	return qryStr;
}

QString AINCMNDbQueries::createTableStringPkDT(const QString& iNewTblName, const QStringList& iListofColumnNames,const QStringList& iListofColumnTypes,
											 const QString& iPrimaryKey)
{
	QString createTblStr = "CREATE TABLE " + iNewTblName + "(";
	for (int i=0; i<iListofColumnNames.count(); i++)
	{
		QString sCurCol = iListofColumnNames.at(i);
		QString sCurDT  = iListofColumnTypes.at(i);

		if ( i )
			createTblStr = createTblStr + ", " + sCurCol + " " + sCurDT;
		else
			createTblStr = createTblStr + sCurCol + " " + sCurDT;

		if (iPrimaryKey == sCurCol)
			createTblStr += (" PRIMARY KEY ASC");
	}
	createTblStr = createTblStr + ")";

	return createTblStr;
}