#ifndef GTACHECKFWCPROCEDURE_H
#define GTACHECKFWCPROCEDURE_H
#include "GTACheckBase.h"
#include "GTACommands.h"

#pragma warning(push, 0)
#include <QHash>
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTACheckFwcProcedure : public GTACheckBase
{
public:
    GTACheckFwcProcedure();
	GTACheckFwcProcedure(const GTACheckFwcProcedure& rhs);
	virtual ~GTACheckFwcProcedure();
    void setCondition(const QString & iVal);
    /*void setOnDisplay(const QString & iVal);*/
    void insertItem(int index, const QString & iVal);
    int getCount() const;

    QString getCondition() const;
   /* QString getOnDisplay() const;*/
    QString getItem(int index) const;


    virtual QString getStatement() const;

	virtual QList<GTACommandBase*>& getChildren(void) const;
	virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
	virtual void setParent(GTACommandBase* iParent);
	virtual GTACommandBase* getParent(void) const;
	/*GTACommandBase* getClone();*/
    GTACommandBase*getClone() const;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool hasChannelInControl()const {return false;}

private:
    QHash<int, QString> _ProcTable;
    QString _Condition;
    QString _OnDisplay;


};

#endif // GTACHECKFWCPROCEDURE_H
