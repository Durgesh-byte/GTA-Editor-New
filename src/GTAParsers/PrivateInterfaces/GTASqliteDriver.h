#ifndef GTASQLITEDRIVER_H
#define GTASQLITEDRIVER_H
#pragma warning(push, 0)
#include <QtCore/QCoreApplication>
#pragma warning(pop)

#include "sqlite3.h"
#include "GTAParsers.h"

class GTAParsers_SHARED_EXPORT GTASqliteDriver
{

public :

    GTASqliteDriver();
    static int executeSqliteQuery(const QString &iDbFilePath, const QStringList &iQueries);
    static int myCallback(void *, int argc, char **argv, char **azColName);

};



#endif // GTASQLITEDRIVER_H
