#include "main.h"

#include "GTAMainWindow.h"
#include "GTATestConfig.h"
#include "GTAAppConfig.h"
#include "GTAAppController.h"
#include "ArgumentHandler.h"
#include "GTAUiUtils.h"
#include <fstream>

#pragma warning(push, 0)
#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QMessageBox>
#include <QThread>
#include <QStyleFactory>
#include <QTextStream>
#pragma warning(pop)




int main(int argc, char* argv[])
{
    QApplication gtaApp(argc, argv);
    gtaApp.setStyle("cleanlooks");

    // dealing with application configurations

    QString folder, name;
    const QString& kCurrentDir = QCoreApplication::applicationDirPath() + "/";

    QString appConfPath = kCurrentDir + kAppConfigFilename;
    GTAAppController* pAppController = GTAAppController::getGTAAppController();
    ArgumentHandler argHandler(*pAppController, appConfPath);
    argHandler.handleConfigInit(gtaApp.arguments());
    IniConfigManager::parse_ini_path(appConfPath, folder, name);
    UiUtils::DoNotShowAgainMesageBox::clearAll();
    if (!argHandler.wasActionPerformed(ArgumentHandler::Action::kSetInitFile)) {
        AppConfig appConf(appConfPath);
        bool parametersPresent = appConf.loadConf();
        if (!parametersPresent) {
            QApplication::beep();
            QMessageBox::StandardButton reply = QMessageBox::question(nullptr,
                "Invalid configuration file",
                "The application configuration file does not exist or is incomplete.\nDo you want to (re)create it?");

            if (reply == QMessageBox::Yes) {
                createConfigFiles(appConfPath, folder, name);
            }
            else {
                QMessageBox::warning(nullptr, "Closing GTA", "GTA will now close.");
                return 2;
            }
        }
        else {
            folder = QString::fromStdString(appConf.getTestConfFolderPath());
            appendSlash(folder);
            name = QString::fromStdString(appConf.getTestConfFileName());
        }
    }

    // finish loadings and starting the application
    QPixmap pixmap(kSplashScreen);
    QSplashScreen splashScreen(pixmap);
    gtaApp.processEvents();
    argHandler.registerArgumetns(gtaApp.arguments());
    GTAMainWindow gtaMainWin = GTAMainWindow(appConfPath, folder, name);
    auto* mainWindowPtr = &gtaMainWin;
    pAppController->setDisplayConsole(argHandler.doesNeedConfig());
    gtaMainWin.setAppController(pAppController);
    gtaMainWin.initialize(argHandler.shouldLaunchGui());
    argHandler.setMainWindow(mainWindowPtr);
    argHandler.executeArguments();

    UiUtils::DoNotShowAgainMesageBox::clearAll();

    if (argHandler.shouldLaunchGui()) {
        splashScreen.show();
        splashScreen.showMessage("Loading....");

        QThread::sleep(2);
        splashScreen.finish(mainWindowPtr);
        gtaMainWin.show();
        return gtaApp.exec();
    }
    else {
        QTextStream out(stdout);
        out << "Finished all actions, exiting.\n";
        return 0;
    }
}

void createConfigFiles(QString& appConfPath, QString& folder, QString& name) {
    QMessageBox::information(nullptr, "Configuration files", "AppConfig.ini and BenchConfig.ini will be created at the root of the application.\nPlease add your settings next.");
    folder = appConfPath;
    name = QString("BenchConfig.ini");

    std::ofstream appconf(appConfPath.toStdString(), std::ofstream::out | std::ofstream::trunc);
    appconf << "[PATH]\n";
    appconf << "BENCHFOLDERPATH = " << folder.toStdString() << "\n";
    appconf << "BENCHFILENAME = " << name.toStdString() << "\n";
    appconf.close();

    std::fstream benchconf((folder + name).toStdString(), std::ofstream::out | std::ofstream::trunc);
    benchconf << "[PATH]\n";
    benchconf << "LIBRARYPATH = \n";
    benchconf << "REPOSITORYPATH = \n";
    benchconf << "TOOLDATAPATH = \n";
    benchconf.close();
}

void appendSlash(QString& path) {
    if (!path.endsWith("/") && !path.endsWith("\\"))
    {
        if (path.contains("/"))
            path += "/";
        else
            path += "\\";
    }
}
