#ifndef AINGTAFSIISIGNAL_H
#define AINGTAFSIISIGNAL_H
#include <QString>
#include "AINGTAFlatScriptParser.h"

class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIISignal
{

    QString _Name;
    QString _Record;
    QString _Application;

    QString _Equipment;
    QString _Media;
    QString _Direction;
    QString _BusName;
    QString _FSName;
    QString _Message;
    QString _Signal;
    QString _GTAName;
    bool    _isSignalFS;
    QString _MediaType;

public:
    AINGTAFSIISignal();

    void setName(const QString &iName);
    QString getName()const;

    void setRecord(const QString &iRecord);
    QString getRecord()const;

    void setApplication(const QString &iApplication);
    QString getApplication()const;

    void setEquipment(const QString &iEquipment);
    QString getEquipment()const;

    void setMedia(const QString &iMedia);
    QString getMedia()const;

    void setDirection(const QString &iDirection);
    QString getDirection()const;

    void setBusName(const QString &iBusName);
    QString getBusName()const;

    void setFSName(const QString &iFSName);
    QString getFSName()const;

    void setMessage(const QString &iMessage);
    QString getMessage()const;

    void setSignal(const QString &iSignal);
    QString getSignal()const;

    void setGTAName(const QString &iName);
    QString getGTAName()const;

    void setIsSignalFS(const bool &iVal);
    bool IsSignalFS()const;

    void setMediaType(const QString &iMediaType);
    QString getMediaType()const;


};

#endif // AINGTAFSIISIGNAL_H
