#ifndef AINGTAFSIIELEMENT_H
#define AINGTAFSIIELEMENT_H
#include <QString>
#include "AINGTAFlatScriptParser.h"

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIElement
{
public:
    AINGTAFSIIElement();
    void setElementName(const QString &iName);
    QString getElementName();
private:
    QString _Name;

};

#endif // AINGTAFSIIELEMENT_H
