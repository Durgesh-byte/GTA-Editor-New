#include "GTAOneClickTestRig.h"

GTAOneClickTestRig::GTAOneClickTestRig()
{
}

GTAOneClickTestRig::~GTAOneClickTestRig()
{
}

void GTAOneClickTestRig::setConfigurations(const QList<GTAOneClickConfiguration> &iConfigs)
{
    _Configurations = iConfigs;
}


void GTAOneClickTestRig::setTestRigName(const QString &iTestRigName)
{
    _TestRigName = iTestRigName;
}
void GTAOneClickTestRig::setTestRigType(const QString &iTestRigType)
{
    _TestRigType = iTestRigType;
}

QString GTAOneClickTestRig::getTestRigName() const
{
    return _TestRigName;
}


QString GTAOneClickTestRig::getTestRigType() const
{
    return _TestRigType;
}

QList<GTAOneClickConfiguration> GTAOneClickTestRig::getConfigurations() const
{
    return _Configurations;
}
