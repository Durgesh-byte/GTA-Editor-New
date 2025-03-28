#ifndef GTACHECKAUDIOALARM_H
#define GTACHECKAUDIOALARM_H
#include "GTACheckBase.h"
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACheckAudioAlarm : public GTACheckBase
{
public:
    GTACheckAudioAlarm();
    virtual ~GTACheckAudioAlarm();

    GTACheckAudioAlarm(const GTACheckAudioAlarm& rhs);


    void setValue(const QString & iVal);
    void setTime(const QString & iVal);

    QString getAlarmName() const;
    QString getTime() const;

    virtual QString getStatement() const;

    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    GTACommandBase* getClone() const;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;
    QString getSCXMLStateName()const;
     bool hasChannelInControl()const {return false;}

private:
    QString _AlarmType;
    QString _Value;
    QString _Time;
};

#endif // GTACHECKAUDIOALARM_H
