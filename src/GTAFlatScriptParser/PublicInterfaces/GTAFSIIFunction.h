#ifndef AINGTAFSIIFUNCTION_H
#define AINGTAFSIIFUNCTION_H
#include "AINGTAFSIICommandBase.h"
#include "AINGTAFSIISignal.h"
#include <QList>
#include "AINGTAFlatScriptParser.h"

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIFunction
{

    QList<AINGTAFSIICommandBase*> _Commands;
    QList<AINGTAFSIISignal> _Signals;           // <fs:definitions>
    QString _Name;
    QString _Description;
public:
    AINGTAFSIIFunction();

    void addCommand(AINGTAFSIICommandBase* pCmd);
    QList<AINGTAFSIICommandBase*> getCommands()const;

    void setSignals(const QList<AINGTAFSIISignal> &iSignalList);
    QList<AINGTAFSIISignal> getSignal()const;

    void setName(const QString &iName);
    QString getName()const;

    void setDescription(const QString &iDesc);
    QString getDescription();

};

#endif // AINGTAFSIIFUNCTION_H
