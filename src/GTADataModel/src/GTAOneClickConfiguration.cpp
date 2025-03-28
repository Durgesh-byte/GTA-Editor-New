#include "GTAOneClickConfiguration.h"

GTAOneClickConfiguration::GTAOneClickConfiguration()
{
}
GTAOneClickConfiguration::~GTAOneClickConfiguration()
{
}

void GTAOneClickConfiguration::setApplications(const QList<GTAOneClickApplications> &iApplications)
{
    _Applications = iApplications;
}

void GTAOneClickConfiguration::setConfigurationName(const QString &iConfigName)
{
    _ConfigName = iConfigName;
}

QList<GTAOneClickApplications> GTAOneClickConfiguration::getApplications() const
{
    return _Applications;
}

QString GTAOneClickConfiguration::getConfigurationName() const
{
    return _ConfigName;
}
