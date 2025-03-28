#ifndef GTACHECKREFRESH_H
#define GTACHECKREFRESH_H
#include "GTACheckBase.h"
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACheckRefresh : public GTACheckBase
{
public:
    GTACheckRefresh();
    virtual ~GTACheckRefresh();

    GTACheckRefresh(const GTACheckRefresh& rhs);


    void setParameter(const QString & iVal);
    void setRefreshTime(const QString & iVal);

    QString getParameter() const;
    QString getRefreshTime() const;

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
   
    QString _Parameter;
    QString _RefreshValue;
};

#endif // GTACheckRefresh_H
