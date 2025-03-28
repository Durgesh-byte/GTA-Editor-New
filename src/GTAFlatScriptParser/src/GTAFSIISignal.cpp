#include "AINGTAFSIISignal.h"

AINGTAFSIISignal::AINGTAFSIISignal():_isSignalFS(false)
{
}


void AINGTAFSIISignal::setName(const QString &iName)
{
    _Name = iName;
}

QString AINGTAFSIISignal::getName()const
{
    return _Name;
}

void AINGTAFSIISignal::setRecord(const QString &iRecord)
{
    _Record = iRecord;
}

QString AINGTAFSIISignal::getRecord()const
{
    return _Record;
}

void AINGTAFSIISignal::setApplication(const QString &iApplication)
{
    _Application = iApplication;
}

QString AINGTAFSIISignal::getApplication()const
{
    return _Application;
}


void AINGTAFSIISignal::setEquipment(const QString &iEquipment)
{
    _Equipment = iEquipment;
}

QString AINGTAFSIISignal::getEquipment()const
{
    return _Equipment;
}

void AINGTAFSIISignal::setMedia(const QString &iMedia)
{
    _Media = iMedia;

}

QString AINGTAFSIISignal::getMedia()const
{
    return _Media;
}

void AINGTAFSIISignal::setDirection(const QString &iDirection)
{
    _Direction = iDirection;
}

QString AINGTAFSIISignal::getDirection()const
{
    return _Direction;
}

void AINGTAFSIISignal::setBusName(const QString &iBusName)
{
    _BusName = iBusName;
}

QString AINGTAFSIISignal::getBusName()const
{
    return _BusName;
}

void AINGTAFSIISignal::setFSName(const QString &iFSName)
{
    _FSName = iFSName;
}

QString AINGTAFSIISignal::getFSName()const
{
    return _FSName;
}

void AINGTAFSIISignal::setMessage(const QString &iMessage)
{
    _Message = iMessage;
}

QString AINGTAFSIISignal::getMessage()const
{
    return _Message;
}

void AINGTAFSIISignal::setSignal(const QString &iSignal)
{
    _Signal = iSignal;
}

QString AINGTAFSIISignal::getSignal()const
{
    return _Signal;
}

void AINGTAFSIISignal::setIsSignalFS(const bool &iVal)
{
    _isSignalFS = iVal;
}

bool AINGTAFSIISignal::IsSignalFS()const
{
    return _isSignalFS;
}

void AINGTAFSIISignal::setGTAName(const QString &iName)
{
    _GTAName = iName;
}

QString AINGTAFSIISignal::getGTAName()const
{
    return _GTAName;
}

void AINGTAFSIISignal::setMediaType(const QString &iMediaType)
{
    _MediaType = iMediaType;
}

QString AINGTAFSIISignal::getMediaType()const
{
    return _MediaType;
}
