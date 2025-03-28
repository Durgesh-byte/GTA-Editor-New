#ifndef GTACMDLOGNODE_H
#define GTACMDLOGNODE_H
#include "GTACmdNode.h"
#include "GTAControllers.h"
#include "GTAScxmlLogMessage.h"
class GTAControllers_SHARED_EXPORT GTACmdLogNode : public GTACmdNode
{
public:
    enum Result{Pass,Fail,None};
    GTACmdLogNode(GTACommandBase * pCmd);

    QHash<bool,GTACmdNode*> Children;

    Result FinalResult;

    QString Comment;


    QString getInstanceName() const;

    void setLogMessage(const QList<GTAScxmlLogMessage> & iList);

    QList<GTAScxmlLogMessage> getLogDetail() const;

private:
    bool IsConditionCmd;
    bool IsWhileLoop;
    QList<GTAScxmlLogMessage> _LogMessageList;
};

#endif // GTACMDLOGNODE_H
