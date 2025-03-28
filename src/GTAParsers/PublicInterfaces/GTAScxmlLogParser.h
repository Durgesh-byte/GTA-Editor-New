#ifndef GTASCXMLLOGPARSER_H
#define GTASCXMLLOGPARSER_H
#include "GTAScxmlLogMessage.h"
#include "GTAParsers.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

typedef QList<GTAScxmlLogMessage> LogMessageList;
class GTAParsers_SHARED_EXPORT GTAScxmlLogParser
{
public:
    GTAScxmlLogParser(const QString & iLogFile);
    bool getLogResult( QHash<QString,LogMessageList *> & oLogResult, bool iRepetativeLog = false);
    QString getExecEndTime() const {return _ExecEnd;}
    QString getExecStartTime() const {return _ExecStart;}
private:
    QString _LogFile;
    QString _ExecStart;
    QString _ExecEnd;
};

#endif // GTASCXMLLOGPARSER_H
