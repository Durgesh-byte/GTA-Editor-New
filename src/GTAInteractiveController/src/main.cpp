#include "AINGTAICMain.h"
#if QT_VERSION_MAJOR > 4
    #include <QApplication>
#else
    #include <QtGui/QApplication>
#endif

#include <QFile>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication gtaICApp(argc, argv);
    AINGTAICMain gtaIC;

    QString appPath = QApplication::applicationDirPath();
    QString settingsPath = QDir::cleanPath( QString("%1%2dark_style.txt").arg(appPath,QDir::separator()));

    QFile styleFile(settingsPath);
    if(styleFile.open(QFile::ReadOnly))
        gtaIC.setStyleSheet(QString(styleFile.readAll()));
    gtaIC.show();

    return gtaICApp.exec();
}
