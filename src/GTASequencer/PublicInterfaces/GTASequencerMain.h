#ifndef GTASEQUENCERMAIN_H
#define GTASEQUENCERMAIN_H

#include "GTASequencer_global.h"
#include "GTASequencerMainWindow.h"

#pragma warning(push, 0)
#include <QHash>
#include <QObject>
#include <QStringList>
#pragma warning(pop)

class GTASequencer_SHARED_EXPORT GTASequencerMain : public QObject
{

    Q_OBJECT

public:

    GTASequencerMain(QObject* ipObject = NULL);
    ~GTASequencerMain();
    void startSequencer(const QString &iLogFilePath, const QString &iScxmlPath,
                        bool isMultipleLogs,const QStringList &iScxmlList,
                        const QStringList &iLogLst ,const QStringList &iFailedList,
                        const QStringList &iProcList,
                        const QString &iToolDataPath);


signals:
    void sequencerWindowClosed();               // from sequencer to main window
//   void sequencingComplete(const QDateTime &iTimeStopped, const QMap<QString,QString> &iLogStatusLst);

    void closeSequencer(bool &oValue);                    //from GTA to sequencer
private:
    GTASequencerMainWindow *_pSeqMainWindow;
};


#endif // GTASEQUENCERMAIN_H
