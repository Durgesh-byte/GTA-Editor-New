#ifndef GTAGENERICATTRIBUTE_H
#define GTAGENERICATTRIBUTE_H

#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTACommands_SHARED_EXPORT GTAGenericAttribute
{
public:
    GTAGenericAttribute();

    QString getAttributeName();
    void setAttributeName(const QString iName);

    QString getAttributeType();
    void setAttributeType(const QString iType);

    QString getAttributeOccurence();
    void setAttributeOccurence(const QString iOccurence);

    void setAttributeDescription(const QString &iDescription);
    QString getAttributeDescription();

    QStringList getAttributeValues();
    void setAttributeValues(const QStringList iValues);

    QString getReturnCode()const;
    void setReturnCode(const QString &returnCode);

    QString getTrueCondition()const;
    void setTrueCondition(const QString &condition);

    QString getFalseCondition()const;
    void setFalseCondition(const QString &condition);

    void setWaitUntil(const QString &iVal);
    bool hasWaitUntil()const;
private:
    QString _Name;
    QString _Type;
    QString _Occurence;
    QString _Description;
    QStringList _Values;
    QString     _ReturnCode;
    QString     _TrueCondition;
    QString     _FalseCondition;
    bool     _WaitUntil;
};

#endif // GTAGENERICATTRIBUTE_H
