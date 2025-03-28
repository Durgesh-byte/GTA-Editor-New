#ifndef AINCMNSqlDbLoader_H_
#define AINCMNSqlDbLoader_H_

#include "AINCMNDbUtils.h"

#include <QString>
#include <QFile>
#include <QList>

class QSqlDatabase;
class QSqlQuery;

namespace AINCMN
{
    namespace DbUtils
    {
        class AINCMNDbUtils_SHARED_EXPORT AINCMNSqlDbLoader
        {
        public:
            bool importFromSqlFile(const QString& iPath, QSqlDatabase* iopDb);

            bool importFromSqlFile(QFile& iFile, QSqlDatabase* iopDb);

            QSqlQuery* fireSql(const QString& iSqlCmd, QSqlDatabase* iopDb);
						
            QSqlDatabase* createInMemoryDbFromDbFile(const QString& FileName);

            bool loadFromDbFile(QSqlDatabase*& iopFromDb, const QString& FileName);

            bool writeToDbFile(QSqlDatabase* pFromDb, const QString& FileName);
        };
    }
}

#endif
