#ifndef GTACHECKUIINTERFACE_H
#define GTACHECKUIINTERFACE_H
#include "GTACheckBase.h"
#include "GTACore.h"
#include "GTAActionWidgetInterface.h"
//#include "GTAAppController.h"

#pragma warning(push, 0)
#include <QWidget>
#pragma warning(pop)

class GTACore_SHARED_EXPORT GTACheckWidgetInterface : public QWidget
{
public:
    GTACheckWidgetInterface(QWidget * Parent);
    virtual void clear() = 0;
    virtual bool isValid() = 0;
    virtual bool getCommand(GTACheckBase *& pCheckCmd) = 0;
    virtual bool setCommand(const GTACheckBase * pCheckCmd) = 0;

    virtual void processSearchInput(const QStringList&){}
    virtual void setAutoCompleteItems(const QStringList &){};
    virtual bool setWidgetFields([[maybe_unused]] const QList<QPair<QString, QString>>& params, [[maybe_unused]] ErrorMessageList& errorLogs) { return false; }


    virtual bool hasTimeOut()const       =0;
    virtual bool hasDumpList()const      =0;
    virtual bool hasPrecision()const     =0;
    virtual bool hasSearchItem()const    =0;
    virtual bool hasConfigTime() const   =0;
    virtual bool hasActionOnFail()const  =0;

  


};

#endif // GTACHECKUIINTERFACE_H
