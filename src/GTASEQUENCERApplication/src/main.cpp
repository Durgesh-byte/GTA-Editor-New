#include "AINGTASEQUENCERAppMainWindow.h"
#if QT_VERSION_MAJOR > 4
    #include <QApplication>
#else
    #include <QtGui/QApplication>
#endif

#include <QMessageBox>
#include <AINGTAUtil.h>
int main(int argc, char *argv[])
{
    QApplication gtaSequencerApp(argc,argv);
    /** Single instance of application using QShared memory*/
    /* QSystemSemaphore sema("key",1);
    sema.acquire();

    QSharedMemory shg;
    shg.setKey("Key2");
    bool is_running;
    if (shg.attach() == true)
    {
        is_running = true;
    }
    else
    {
        shg.create(1);
        is_running = false;
    }
    sema.release();

    if(is_running){
        QMessageBox::warning(nullptr,"Error"," Cannot start the application, an another instance of sequencer application is already running.",QMessageBox::Ok);
        gtaSequencerApp.exit();
        return 1;
    } */

    /** Single instance */
    /*QString applicationName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    QProcess tasklist;
    tasklist.start(
            "tasklist",
            QStringList() << "/NH"
            << "/FO" << "CSV"
            << "/FI" << QString("IMAGENAME eq %1").arg(applicationName));
    tasklist.waitForFinished();
    QString output = tasklist.readAllStandardOutput();
    QStringList outputlist = output.split(applicationName);
    if(outputlist.count()>2)
    {
        QMessageBox::warning(nullptr,"Error"," Cannot start the application, an another instance of sequencer application is already running.",QMessageBox::Ok);
         gtaSequencerApp.exit();
         return 0;
    }
    else
    { */
	AINGTASEQUENCERAppMainWindow sequencerApp;
    sequencerApp.show();
    return gtaSequencerApp.exec();
//#if MAJOR_TOOL_VERSION > 26
//    AINGTASEQUENCERAppMainWindow sequencerApp;
//    sequencerApp.show();
//    return gtaSequencerApp.exec();
//#else
//    QMessageBox::warning(nullptr,"Error"," GTA Sequencer Application is not supported with GTA Versions lower than V27",QMessageBox::Ok);
//    gtaSequencerApp.exit();
//    return 0;
//#endif
    //}
}
