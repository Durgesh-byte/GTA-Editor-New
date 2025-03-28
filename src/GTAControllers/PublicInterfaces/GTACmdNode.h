#ifndef GTACMDNODE_H
#define GTACMDNODE_H
#include "GTACommandBase.h"
#include "GTAControllers.h"

#pragma warning(push, 0)
#include <QString>
#include <QHash>
#pragma warning(pop)

class GTAControllers_SHARED_EXPORT GTACmdNode
{
public:
    GTACmdNode(GTACommandBase * pCmdNode, const QString & iStateId);
   
    ~GTACmdNode();
  
    void setStateId(const QString & iStateId);
    QString getStateId() const;
    GTACommandBase * getCommand() const;

    //static void setUuid(const QString & iID);
    //QString getLogMessage() const;

    GTACmdNode * NextNode;
    GTACmdNode * PreviousNode;
    GTACmdNode * Parent;
    QHash<bool,QList<GTACmdNode*> *> Children;
    bool IsConditionCmd;
    bool IsWhileLoop;

private:
    GTACommandBase * _pCmd;
    QString _StateId;
    QString _LogMessage;
    //static QString _Uuid;
};
#endif // GTACMDNODE_H
