#ifndef GTAONECLICKTESTRIG_H
#define GTAONECLICKTESTRIG_H

#include "GTAOneClickConfiguration.h"
#include "GTADataModel.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTADataModel_SHARED_EXPORT GTAOneClickTestRig
{
private:
    QString _TestRigName;
    QString _TestRigType;
    QList<GTAOneClickConfiguration> _Configurations;
public:
    void setTestRigType(const QString & iTestRigType);
    void setTestRigName( const QString & iTestRigName);
    void setConfigurations(const QList<GTAOneClickConfiguration> & iConfigs);
    QString getTestRigName() const;
    QString getTestRigType() const;
    QList<GTAOneClickConfiguration> getConfigurations() const;

    GTAOneClickTestRig();
    ~GTAOneClickTestRig();
};

#endif // GTAONECLICKTESTRIG_H
