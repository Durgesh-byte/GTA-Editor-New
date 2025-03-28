#include "GTAOneClickLaunchApplicationTestRig.h"

GTAOneClickLaunchApplicationTestRig::GTAOneClickLaunchApplicationTestRig()
{
}

GTAOneClickLaunchApplicationTestRig::~GTAOneClickLaunchApplicationTestRig()
{
}


void GTAOneClickLaunchApplicationTestRig::setTestRigName(const QString &iTestRigName)
{
    _TestRigName = iTestRigName;
}

QString GTAOneClickLaunchApplicationTestRig::getTestRigName() const
{
    return _TestRigName;
}


void GTAOneClickLaunchApplicationTestRig::insertApplication(const GTAOneClickLaunchApplicationApplications &iApp)
{
    _Applications <<iApp;
}

QList<GTAOneClickLaunchApplicationApplications> GTAOneClickLaunchApplicationTestRig::getApplications() const
{
    return _Applications;
}
void  GTAOneClickLaunchApplicationTestRig::clear()
{
    _TestRigName = "";
    _Applications.clear();
}