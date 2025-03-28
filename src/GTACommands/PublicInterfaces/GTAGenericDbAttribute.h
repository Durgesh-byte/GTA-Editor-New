#ifndef GTAGENERICDBATTRIBUTE_H
#define GTAGENERICDBATTRIBUTE_H

#include "GTACommands.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAGenericDbAttribute
{

private :
    int _Id;
    QString _Name;
    int _Column;

public:
    GTAGenericDbAttribute();
    GTAGenericDbAttribute(const int &iId, const QString &iName, const int &iCol);

    void setId(const int &iVal);
    int getId()const;

    void setName(const QString &iVal);
    QString getName()const;

    void setColumn(const int &iVal);
    int getColumn()const;



};

#endif // GTAGENERICDBATTRIBUTE_H
