#include "GTAOneClickApplications.h"

GTAOneClickApplications::GTAOneClickApplications()
{

}

GTAOneClickApplications::~GTAOneClickApplications()
{

}

void GTAOneClickApplications::setAppCommType(const QString& iCommType)
{
    _CommType = iCommType;
}

void GTAOneClickApplications::setAppName(const QString& iAppName)
{
    _AppName = iAppName;
}
void GTAOneClickApplications::setOptions(const GTAOneClickOptions iOptions)
{
//    QStringList optionNames = iOptions.getAllOptions();
//    foreach(QString optName,optionNames)
//    {
//        _Options.addOptionToList(optName);
//        GTAOneClickOption opt1 = iOptions.getOptionDefMap(optName);
//        if(opt1.getOptionCondition().isEmpty())
//            _Options.setDefOption(optName,opt1);
//        QList<GTAOneClickOption> opts = iOptions.getOptionCondMap(optName);
//        foreach(GTAOneClickOption opt,opts)
//        {
//            if(!opt.getOptionCondition().isEmpty())
//                _Options.setCondOptions(optName,opt);
//        }
//        _Options.setOptionType(optName,iOptions.getOptionTypeMap(optName));
//    }
    _Options = iOptions;
}

void GTAOneClickApplications::setDescription(const QString &iDescription)
{
    _Description = iDescription;
}
QString GTAOneClickApplications::getDescription()    const
{
    return _Description;
}
void GTAOneClickApplications::setAppVersion(const QString& iVersion)
{
    _Version = iVersion;
}

GTAOneClickOptions GTAOneClickApplications::getAppOptions() const
{
    return _Options;
}

QString GTAOneClickApplications::getAppCommType() const
{
    return _CommType;
}

QString GTAOneClickApplications::getAppName() const
{
    return _AppName;
}

QString GTAOneClickApplications::getAppVersion() const
{
    return _Version;
}


