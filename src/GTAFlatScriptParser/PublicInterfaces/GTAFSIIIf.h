#ifndef AINGTAFSIIIF_H
#define AINGTAFSIIIF_H
#include "AINGTAFSIICommandBase.h"
#include <QString>
#include <QList>
#include "AINGTAFlatScriptParser.h"


class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIIf:public AINGTAFSIICommandBase
{
    QString _LogicalOperator;
    QList<AINGTAFSIICondition> _Conditions;
    QList<AINGTAFSIICommandBase*> _CommandListOnTrue;
    QList<AINGTAFSIICommandBase*> _CommandListOnFalse;

public:
    AINGTAFSIIIf();
    AINGTAFSIIIf(const QString &iCommandName);

    void setLogicalOperator(const QString &iLogicalOp);
    QString getLogicalOperator()const;

    QList<AINGTAFSIICondition> getConditions();
    void addCondition(const AINGTAFSIICondition &iCondition);

    QList<AINGTAFSIICommandBase*> getCommandListOnTrue();
    void addCommandOnTrue(AINGTAFSIICommandBase* pCmd);

    QList<AINGTAFSIICommandBase*> getCommandListOnFalse();
    void addComamndOnFalse(AINGTAFSIICommandBase* pCmd);






};

#endif // AINGTAFSIIIF_H
