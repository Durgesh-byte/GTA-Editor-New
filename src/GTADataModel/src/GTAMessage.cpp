#include "GTAMessage.h"

GTAMessage::GTAMessage(MessageType iType)
{
    _MsgType = iType;

    _Message= QString();
    _Description= QString();
    _LabelBit= QString();
    _DisplayColor= QString();
    _DisplayLocation= QString();
    _Screen= QString();
    _EngineType= QString();
    _FwcMessage= QString();
    _FwcLabelBit= QString();
}
void GTAMessage::setMessage(const QString &iVal)
{
    _Message = iVal;
}

QString GTAMessage::getMessage() const
{
    return _Message;
}

void GTAMessage::setDescription(const QString &iVal)
{
    _Description = iVal;
}

QString GTAMessage::getDescription() const
{
    return _Description;
}

void GTAMessage::setLabel(const QString &iVal)
{
    _LabelBit = iVal;
}

QString GTAMessage::getLabel() const
{
    return _LabelBit;
}

void GTAMessage::setDisplayColor(const QString &iVal)
{
    _DisplayColor = iVal;
}

QString GTAMessage::getDisplayColor() const
{
    return _DisplayColor;
}

void GTAMessage::setDisplayLocation(const QString &iVal)
{
    _DisplayLocation = iVal;
}

QString GTAMessage::getDisplayLocation() const
{
    return _DisplayLocation;
}

void GTAMessage::setScreen(const QString &iVal)
{
    _Screen = iVal;
}

QString GTAMessage::getScreen() const
{
    return _Screen;
}

void GTAMessage::setEngineType(const QString &iVal)
{
    _EngineType = iVal;
}

QString GTAMessage::getEngineType() const
{
    return _EngineType;
}

void GTAMessage::setFWCMessage(const QString &iVal)
{
    _FwcMessage = iVal;
}

QString GTAMessage::getFWCMessage() const
{
    return _FwcMessage;
}

void GTAMessage::setFWCLabel(const QString &iVal)
{
    _FwcLabelBit = iVal;
}

QString GTAMessage::getFWCLabel() const
{
    return _FwcLabelBit;
}
