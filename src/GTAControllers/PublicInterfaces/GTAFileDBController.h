#ifndef AINGTAFILEDBCONTROLLER_H
#define AINGTAFILEDBCONTROLLER_H

#include "AINGTATreeItem.h"
#include <QSqlDatabase>
class AINGTAFileDBController : public QObject
{
public:
	bool insertFile(AINGTATreeItem *pItem);

private:
	QSqlDatabase _db;
	QString _dbPath;
};

#endif