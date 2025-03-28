#include "GTACmdNode.h"
//QString GTACmdNode::_Uuid = QString();
GTACmdNode::GTACmdNode(GTACommandBase * pCmdNode, const QString & iStateId)
{
    _pCmd = pCmdNode;
    setStateId(iStateId);
    NextNode = NULL;
    PreviousNode = NULL;
    Parent= NULL;
    Children.clear();
    IsConditionCmd = false;
    IsWhileLoop = false;
    QString statment = pCmdNode != NULL ? pCmdNode->getStatement() : QString();
   // _LogMessage = QString("%1#%2;%3").arg(_Uuid,QString(iStateId).remove('s'),statment);
}
GTACmdNode::~GTACmdNode()
{
    if(NextNode != NULL)
    {
        delete NextNode;
        NextNode = NULL;
    }
    QList<GTACmdNode*> * leftChildList = Children.value(true);
    if(leftChildList  != NULL && leftChildList->count())
    {
        GTACmdNode *pLeftChild = leftChildList->at(0);
        if(pLeftChild != NULL)
        {
            delete pLeftChild;
            pLeftChild = NULL;
        }
    }
    QList<GTACmdNode*> * rightChildList = Children.value(false);
    if(rightChildList  != NULL && rightChildList->count())
    {
        GTACmdNode *pRightChild = rightChildList->at(0);
        if(pRightChild != NULL)
        {
            delete pRightChild;
            pRightChild = NULL;
        }
    }

}

void GTACmdNode::setStateId(const QString & iStateId)
{
    _StateId = iStateId;
}
QString GTACmdNode::getStateId() const
{
    return _StateId;
}
GTACommandBase * GTACmdNode::getCommand() const
{
    return _pCmd;
}

//void GTACmdNode::setUuid(const QString & iID)
//{
//    _Uuid = iID;
//}
//QString GTACmdNode::getLogMessage() const
//{
//    return _LogMessage;
//}

