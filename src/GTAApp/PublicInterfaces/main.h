#pragma once
#pragma warning(push, 0)
#include <qstring.h>
#include <qcoreapplication.h>
#pragma warning(pop)

const QString& kAppConfigFilename = "AppConfig.ini";
const QString& kSplashScreen = ":/images/SplashScreen";

void createConfigFiles(QString& appConfPath, QString& folder, QString& name);
void appendSlash(QString& path);
