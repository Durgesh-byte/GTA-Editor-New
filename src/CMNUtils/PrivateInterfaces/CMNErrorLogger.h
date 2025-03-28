#ifndef AINCMNERRORLOGGER_H_
#define AINCMNERRORLOGGER_H_

#include <QObject>
#include <QString>
#include <QFile>

#include "AINCMNUtils.h"

class AINCMNUtils_SHARED_EXPORT AINCMNErrorLogger : public QObject
{
public:
    AINCMNErrorLogger(const QString& iLogFileAbsPath, QObject *parent = 0);
    ~AINCMNErrorLogger();
    
    bool writeLine(const QString& iLine);


    QFile * _pLogFile;
};

#endif // AINCMNERRORLOGGER_H_
