#ifndef GTAFWCDATA_H
#define GTAFWCDATA_H
#include "GTADataModelInterface.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTADataModel_SHARED_EXPORT GTAFWCData
{

private:
    QString _Header;
    QString _Message;
    QString _Color;

public:
    GTAFWCData();
    QString getHeader() const;
    QString getMessage() const;
    QString getFWCColor() const;

    void setFWCColor(const QString &iColor);
    void setHeader(const QString  &iHeader);
    void setMessage(const QString & iMessage);
};

#endif // GTAFWCDATA_H
