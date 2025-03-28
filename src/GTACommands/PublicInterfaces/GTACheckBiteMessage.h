#ifndef GTACHECKBITEMESSAGE_H
#define GTACHECKBITEMESSAGE_H
#include "GTACheckBase.h"
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACheckBiteMessage : public GTACheckBase
{
public:
    GTACheckBiteMessage();
	GTACheckBiteMessage(const GTACheckBiteMessage& rhs);
	virtual ~GTACheckBiteMessage();
    void setMessage(const QString & iVal);
    void setCondition(const QString & iVal);
    void setOnReport(const QString & iVal);
    void setWaitStatus(const bool & iVal);
    void setBuffDepth(const QString & iVal);

    QString getMessage() const;
    QString getCondition() const;
    QString getOnReport() const;
    bool getWaitStatus() const;
    QString getBuffDepth() const;

    virtual QString getStatement() const;

	virtual QList<GTACommandBase*>& getChildren(void) const;
	virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
	virtual void setParent(GTACommandBase* iParent);
	virtual GTACommandBase* getParent(void) const;
	virtual GTACommandBase*getClone() const; 
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;
    QString getSCXMLStateName()const;
    bool hasChannelInControl()const {return false;}
   
	
private:
    QString _Message;
    QString _Condition;
    QString _OnReport;
    bool _waitForAck;
    QString _bufferDepth;

};

#endif // GTACHECKBITEMESSAGE_H
