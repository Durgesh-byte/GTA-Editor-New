#ifndef AINGTAFSIISET_H
#define AINGTAFSIISET_H
#include "AINGTAFSIICommandBase.h"
#include <QString>

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIISet:public AINGTAFSIICommandBase
{
public:
    AINGTAFSIISet();
    AINGTAFSIISet(const QString &iCommandType);
    virtual void setParamName(const QString &iName);
    virtual QString getParamName();
    virtual void setParamValue(const QString &iVal);
    virtual QString getParamValue();

    void setFSOnly(const bool &iVal = false);
    bool isFSOnly()const;
    void setValueOnly(const bool &iVal =false);
    bool isValueOnly()const;
private:

    QString _ParamName;
    QString _Value;
    bool _FSOnly;
    bool _ValueOnly;


};

#endif // AINGTAFSIISET_H
