#ifndef GTAGENERICPARAMDATA_H
#define GTAGENERICPARAMDATA_H

#include "GTACommands.h"
#include "GTAGenericAttribute.h"

#pragma warning(push, 0)
#include <QString>
#include <QList>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAGenericParamData
{
public:
    GTAGenericParamData();

    QString getParamDataName();
    void setParamDataName(const QString iParamName);

    QString getParamDataType();
    void setParamDataType(const QString iParamDataType);

    QList<GTAGenericAttribute> getParamAttributeList();
    void setParamAttributeList(const QList<GTAGenericAttribute> iAttrList);


private:
        QString _Name;
	QString _Type;
        QList<GTAGenericAttribute> _Attributes;
};

#endif // GTAGENERICPARAMDATA_H
