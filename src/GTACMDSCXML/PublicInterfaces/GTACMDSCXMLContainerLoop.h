#ifndef GTACMDSCXMLCONTAINTERLOOP_H
#define GTACMDSCXMLCONTAINTERLOOP_H


#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXML.h"
#include "GTACommandBase.h"


class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLContainerLoop : public GTACMDSCXMLBase
{
public:
    GTACMDSCXMLContainerLoop();
    GTACMDSCXMLContainerLoop(GTACommandBase *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLContainerLoop();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;


private:
    GTACommandBase *_LoopCmd;

private:
};

#endif // GTACMDSCXMLCONTAINTERLOOP_H
