#ifndef GTAFAILPROFILESTOPTRIGGER_H
#define GTAFAILPROFILESTOPTRIGGER_H
#include "GTAFailureProfileBase.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileStopTrigger : public GTAFailureProfileBase
{
public:
    GTAFailProfileStopTrigger(); 
	GTAFailProfileStopTrigger(const GTAFailProfileStopTrigger& rhs);
	virtual ~GTAFailProfileStopTrigger();
	GTAFailureProfileBase* getClone()const;

    void setTimeout(const QString& iTimeout){_timeOut=iTimeout;}
    QString getTimeout()const{return _timeOut;}

    QString getStatement() const;
    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const;
    void getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const;
    void setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap);
    QString getSimulationName()const{return EQ_STOP_TRIGGER;}

    QStringList getVariableList() const  ;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    Mode getMode()const{return GTAFailureProfileBase::STOP_TRIGGER;}

private:
  
      QString _timeOut;
};

#endif // GTAFailProfileStopTrigger_H
