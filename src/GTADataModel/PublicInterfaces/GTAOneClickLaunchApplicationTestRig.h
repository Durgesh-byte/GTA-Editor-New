#ifndef GTAOneClickLaunchApplicationTestRig_H
#define GTAOneClickLaunchApplicationTestRig_H

#include "GTAOneClickLaunchApplicationApplications.h"
#include "GTADataModel.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTADataModel_SHARED_EXPORT GTAOneClickLaunchApplicationTestRig
{
private:
    QList<GTAOneClickLaunchApplicationApplications> _Applications;
    QString _TestRigName;
public:

    void setTestRigName( const QString & iTestRigName);
    QString getTestRigName() const;

    void  insertApplication(const GTAOneClickLaunchApplicationApplications &iApp);
    QList<GTAOneClickLaunchApplicationApplications> getApplications() const ;

    void clear();
    GTAOneClickLaunchApplicationTestRig();
    ~GTAOneClickLaunchApplicationTestRig();
};

#endif // GTAOneClickLaunchApplicationTestRig_H
