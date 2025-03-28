#include "GTASequencerMain.h"
#include "GTASequencer.h"

GTASequencerMain::GTASequencerMain(QObject* ipObject):QObject(ipObject)
{
    _pSeqMainWindow = NULL;
}

GTASequencerMain::~GTASequencerMain()
{
    if(_pSeqMainWindow != NULL)
    {
        delete _pSeqMainWindow;
        _pSeqMainWindow = NULL;
    }
}

void GTASequencerMain::startSequencer(const QString &iLogFilePath, const QString &iScxmlPath,
                                         bool isMultipleLogs,const QStringList &iScxmlList,
                                         const QStringList &iLogLst ,const QStringList &iFailedList,
                                         const QStringList &iProcList,
                                         const QString &iToolDataPath)
{
        GTASequencer *pSequencer = GTASequencer::getSequencer();
        if(pSequencer)
        {
            pSequencer->setLogFilePath(iLogFilePath);
            pSequencer->setScxmlPath(iScxmlPath);
            pSequencer->setMultipleLogs(isMultipleLogs);
            pSequencer->setGeneratedScxmlFiles(iScxmlList);
            pSequencer->setLogNameLst(iLogLst);
            pSequencer->setFailedList(iFailedList);
            pSequencer->setProcList(iProcList);
        }

//        if(_pSeqMainWindow != NULL)
//        {
//            delete _pSeqMainWindow;
//            _pSeqMainWindow = NULL;
//        }

       _pSeqMainWindow  = new GTASequencerMainWindow(iToolDataPath, pSequencer);
        connect(_pSeqMainWindow,SIGNAL(sequencerWindowClosed()),this,SIGNAL(sequencerWindowClosed()));
        connect(this,SIGNAL(closeSequencer(bool&)),_pSeqMainWindow,SLOT(onCloseSequencer(bool&)));
        _pSeqMainWindow->show();
}

