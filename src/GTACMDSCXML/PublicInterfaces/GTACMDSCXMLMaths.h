#ifndef GTACMDSCXMLMATHS_H
#define GTACMDSCXMLMATHS_H


#include "GTACMDSCXML.h"
#include "GTACMDSCXMLBase.h"
#include "GTAEquationBase.h"
#include "GTAActionMaths.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLMaths : public GTACMDSCXMLBase
{

public:
    GTACMDSCXMLMaths();
    GTACMDSCXMLMaths(GTAActionMaths *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLMaths();

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;

private:
    GTAActionMaths *_mathCmd;
};

#endif // GTACMDSCXMLMATHS_H
