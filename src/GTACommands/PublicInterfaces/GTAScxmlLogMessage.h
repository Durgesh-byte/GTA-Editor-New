#ifndef GTASCXMLLOGMESSAGE_H
#define GTASCXMLLOGMESSAGE_H
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAScxmlLogMessage
{
public:
    enum StatusType{Success,Fail,TimeOut,None};
    enum ResultType{OK,KO,kNa};
    enum LevelOfDetail{Main=0,Detail};

    GTAScxmlLogMessage();
    GTAScxmlLogMessage(const QString & iLogMessage);

    QString Identifier;
    ResultType Result;
    QString ExpectedValue;
    QString CurrentValue;
    StatusType Status;
    QString FunctionStatus;
    LevelOfDetail LOD;
    bool isCurrentValParam;
    QString getLogMessage(bool isGenericLog = false);


    /**
      * This variable must be used for analyzing the log
      * The variable should not be used while loging in scxml
      */
    QString TimeStamp;
    /**
      * This variable must be used for analyzing the log
      * The variable should not be used while loging in scxml
      */
    QString StatID;

    QString StatusTypeToString(StatusType iVal) const;
    StatusType StringToStatusType(const QString & iVal) const;

    QString ResultTypeToString(ResultType iVal) const;
    ResultType StringToResultType(const QString & iVal) const;

    QString LevelOfDetailToString(LevelOfDetail iVal) const;
    LevelOfDetail StringToLevelOfDetail(const QString & iVal) const;

    int getLOD() const;
    QString getIdentifier() const;

private:
    int _LogLevel;
    void initializeMember();



};

#endif // GTASCXMLLOGMESSAGE_H
