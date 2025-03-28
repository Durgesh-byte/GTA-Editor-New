#ifndef AINGTAFSIIINCREASEDECREASE_H
#define AINGTAFSIIINCREASEDECREASE_H
#include "AINGTAFSIISet.h"
#include <QString>
#include "AINGTAFlatScriptParser.h"

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIIncreaseDecrease: public AINGTAFSIISet
{
public:

    AINGTAFSIIIncreaseDecrease(const QString &iCommandName);
    void setParamValue(const QString &iVal);


private:
    AINGTAFSIIIncreaseDecrease();
    bool _IsCmdDecrease;
    QString _Value;


};

#endif // AINGTAFSIIINCREASEDECREASE_H
