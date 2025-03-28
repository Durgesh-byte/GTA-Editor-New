#include "GTACmdLogNode.h"

GTACmdLogNode::GTACmdLogNode(GTACommandBase *pCmd) : GTACmdNode(pCmd,QString())
{
    Comment = "";
    FinalResult = None;
    _LogMessageList.clear();
}

QString GTACmdLogNode::getInstanceName() const
{
	return (getCommand() != NULL ? getCommand()->getInstanceName(): QString());
}

void GTACmdLogNode::setLogMessage(const QList<GTAScxmlLogMessage> & iList)
{
    _LogMessageList = iList;
}

QList<GTAScxmlLogMessage> GTACmdLogNode::getLogDetail() const
{
    QList<GTAScxmlLogMessage> oList;
    foreach(GTAScxmlLogMessage objLog, _LogMessageList)
    {
        if( objLog.LOD == GTAScxmlLogMessage::Detail)
            oList.append(objLog);
    }
    return oList;
}
