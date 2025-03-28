#include "GTASqlTableModel.h"

#pragma warning(push, 0)
#include <QDebug>
#include <QSqlRecord>
#pragma warning(pop)


GTASqlTableModel::GTASqlTableModel(QObject *parent,QSqlDatabase &iDb)
    : QSqlQueryModel(parent)
{
    _Database = iDb;
    dropIndex();
    createIndex ();
}

GTASqlTableModel::~GTASqlTableModel()
{
    //clear();
}

void GTASqlTableModel::createIndex()
{
    //QString indexQ = QString("CREATE INDEX EQ_IDX ON PARAMETERS(EQUIPNAME, FILE, FILETYPE, APPLICATION, MEDIA, TOOL_NAME)");
	QString indexQ = QString("CREATE INDEX EQ_IDX ON PARAMETERS(EQUIPNAME, FILE, NAME, SIGNALTYPE, TOOL_NAME)");
    QSqlQueryModel::setQuery(indexQ,_Database);
    qDebug()<<"EQ_IDX: "<<lastError().text();
}
void GTASqlTableModel::dropIndex()
{
    QString dropIdxQ = QString("DROP INDEX EQ_IDX");
    QSqlQueryModel::setQuery(dropIdxQ,_Database);
    qDebug()<<"EQ_IDX: "<<lastError().text();
}

void GTASqlTableModel::createEquipmentTable()
{
    //QSqlQueryModel::setQuery("SELECT EQUIPNAME, FILE, FILETYPE, APPLICATION, MEDIA, TOOL_NAME FROM PARAMETERS GROUP BY EQUIPNAME, FILE, FILETYPE, APPLICATION, MEDIA;",_Database);
	QSqlQueryModel::setQuery("SELECT EQUIPNAME, FILE, NAME, SIGNALTYPE, TOOL_NAME, TOOL_TYPE FROM PARAMETERS GROUP BY EQUIPNAME, FILE, NAME, SIGNALTYPE, TOOL_TYPE;", _Database);
    qDebug()<<"EQ_TABLE: "<<lastError().text();
}

void GTASqlTableModel::updateToolName(const QString &iCriteria, const QString &iCriteriaVal, const QString &iToolName)
{
    QString query = QString("UPDATE PARAMETERS SET TOOL_NAME = '%1' WHERE %2 ='%3' OR %2 = '' ").arg(iToolName,iCriteria,iCriteriaVal);
    QSqlQueryModel::setQuery(query,_Database);
    qDebug()<<"EQ_UPDATE: "<<lastError().text();
}

void GTASqlTableModel::refresh()
{
    createEquipmentTable();
}

void GTASqlTableModel::updateToolName(const QString &iToolName)
{
    QString query = QString("UPDATE PARAMETERS SET TOOL_NAME = '%1'").arg(iToolName);
    QSqlQueryModel::setQuery(query,_Database);
    qDebug()<<"EQ_UPDATE: "<<lastError().text();
}

void GTASqlTableModel::search(const QString &iColName, const QString &iSrchStr)
{
	if(!iSrchStr.isEmpty())
	{
		//QSqlQueryModel::setQuery(QString("SELECT EQUIPNAME, FILE, FILETYPE, APPLICATION, MEDIA, TOOL_NAME FROM PARAMETERS WHERE %1 LIKE '%%2%' GROUP BY EQUIPNAME, FILE, FILETYPE, APPLICATION, MEDIA;").arg(iColName,iSrchStr),_Database);
		QSqlQueryModel::setQuery(QString("SELECT EQUIPNAME, FILE, NAME, SIGNALTYPE, TOOL_NAME FROM PARAMETERS WHERE %1 LIKE '%%2%' GROUP BY EQUIPNAME, FILE, NAME, SIGNALTYPE;").arg(iColName, iSrchStr), _Database);
		qDebug()<<"EQ_TABLE: "<<lastError().text();
	}
	else
		refresh();
}
