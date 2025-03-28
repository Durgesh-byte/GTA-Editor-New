#ifndef GTAOneClickLaunchApplicationApplications_H
#define GTAOneClickLaunchApplicationApplications_H

#include "GTADataModel.h"
#include "GTAOneClickLaunchApplicationOptions.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTADataModel_SHARED_EXPORT GTAOneClickLaunchApplicationApplications
{

private:
    QList<GTAOneClickLaunchApplicationOptions> _Options;
    QString _AppName;
    QString _AppType;
    QString _AppToolId;
    QString _AppToolType;
    QString _AppDescription;
    bool    _isKillMode;
public:
    GTAOneClickLaunchApplicationApplications();
    ~GTAOneClickLaunchApplicationApplications();

    void insertOption(const GTAOneClickLaunchApplicationOptions &iOption){_Options<<iOption;}
    void setAppName(const QString &iAppName){_AppName = iAppName;}
    void setAppType(const QString &iAppType){_AppType = iAppType;}
    void setAppToolId(const QString &iAppToolId){_AppToolId = iAppToolId;}
    void setAppToolType(const QString &iAppToolType){_AppToolType = iAppToolType;}
    void setAppDescription(const QString &iAppDescription){_AppDescription = iAppDescription;}
    void setKillMode(bool iKillMode){_isKillMode = iKillMode;}

    QList<GTAOneClickLaunchApplicationOptions> getOptions() const {return _Options;}
    QString getAppName() const {return _AppName;}
    QString getAppType() const {return _AppType;}
    QString getAppToolId() const {return _AppToolId;}
    QString getAppToolType() const {return _AppToolType;}
    QString getAppDescription() const {return _AppDescription;}
    bool getKillMode() const {return _isKillMode;}
    void clear();
};

#endif // GTAOneClickLaunchApplicationApplications_H
