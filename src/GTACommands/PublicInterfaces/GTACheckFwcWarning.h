#ifndef GTAFWCWARNING_H
#define GTAFWCWARNING_H
#include "GTACommands.h"
#include "GTACheckBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACheckBase;
class GTACommands_SHARED_EXPORT GTACheckFwcWarning : public GTACheckBase
{
public:
    GTACheckFwcWarning();
	virtual ~GTACheckFwcWarning();
	GTACheckFwcWarning(const GTACheckFwcWarning& rhs);
    void setMessage(const QString & iVal);
    void setCondition(const QString & iVal);
    void setOnDisplay(const QString & iVal);
    void setHeader(const QString & iVal);
    void setWaitForBuffer(const QString & iVal);
    void setWaitAck(const bool& iVal);
    void setFWCWarningColor(const QString &iColor);

    QString getMessage() const;
    QString getCondition() const;
    QString getOnDisplay() const;
    QString getHeader() const;
    QString getWaitForBuffer()const;
    QString getFWCWarningColor() const;
    bool getWaitAck()const;
    virtual QString getLTRAStatement() const;

    virtual QString getStatement() const;

	virtual QList<GTACommandBase*>& getChildren(void) const;
	virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
	virtual void setParent(GTACommandBase* iParent);
	virtual GTACommandBase* getParent(void) const;
	GTACommandBase* getClone()const;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;

    QString getSCXMLStateName() const;
    bool hasChannelInControl()const {return false;}
    bool hasConfirmatiomTime() {return true;}

private:
    QString _Message;
    QString _Condition;
    QString _OnDisplay;
    QString _Header;
    QString _WaitForBuffer;
    QString _WarningColor;
    bool    _WaitAck;
};

#endif // GTAFWCWARNING_H
