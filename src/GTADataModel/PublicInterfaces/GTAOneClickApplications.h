#ifndef GTAONECLICKAPPLICATIONS_H
#define GTAONECLICKAPPLICATIONS_H

#include "GTADataModel.h"
#include "GTAOneClickOptions.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)


class GTADataModel_SHARED_EXPORT GTAOneClickApplications
{
private:
    QString _AppName;
    QString _Version;
    QString _CommType;
    QString _Description;
    GTAOneClickOptions _Options;
public:
    GTAOneClickApplications();
    ~GTAOneClickApplications();
    void setAppName(const QString& iAppName);
    void setAppVersion(const QString& iVersion);
    void setAppCommType(const QString & iCommType);
    void setDescription(const QString &iDescription);
    void setOptions(const GTAOneClickOptions iOptions);

    QString getDescription() const;
    QString getAppName() const;
    QString getAppVersion() const;
    QString getAppCommType() const;
    GTAOneClickOptions getAppOptions() const;

};

#endif // GTAONECLICKAPPLICATIONS_H
