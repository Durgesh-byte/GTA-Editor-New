#ifndef GTAONECLICKCONFIGURATION_H
#define GTAONECLICKCONFIGURATION_H

#include "GTADataModel.h"
#include "GTAOneClickApplications.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTADataModel_SHARED_EXPORT GTAOneClickConfiguration
{
private:
    QString _ConfigName;
    QList<GTAOneClickApplications> _Applications;
public:
    GTAOneClickConfiguration();
    ~GTAOneClickConfiguration();
    void setConfigurationName(const QString & iConfigName);
    void setApplications(const QList<GTAOneClickApplications> & iApplications);

    QString getConfigurationName() const;
    QList<GTAOneClickApplications> getApplications() const;

};

#endif // GTAONECLICKCONFIGURATION_H
