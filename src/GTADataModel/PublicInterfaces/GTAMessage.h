#ifndef GTAMESSAGE_H
#define GTAMESSAGE_H

#pragma warning(push, 0)
#include <QString>
#include <QHash>
#pragma warning(pop)

class GTAMessage
{
public:
    enum MessageType{kWarning, DISPLAY,BITEMSG};
    GTAMessage(MessageType iType);

    void setMessage(const QString &iVal);
    QString getMessage() const;

    void setDescription(const QString &iVal);
    QString getDescription() const;

    void setLabel(const QString &iVal);
    QString getLabel() const;

    void setDisplayColor(const QString &iVal);
    QString getDisplayColor() const;

    void setDisplayLocation(const QString &iVal);
    QString getDisplayLocation() const;

    void setScreen(const QString &iVal);
    QString getScreen() const;

    void setEngineType(const QString &iVal);
    QString getEngineType() const;

    void setFWCMessage(const QString &iVal);
    QString getFWCMessage() const;

    void setFWCLabel(const QString &iVal);
    QString getFWCLabel() const;

private:
    QString     _Message;
    QString     _Description;
    QString     _LabelBit;
    QString     _DisplayColor;
    QString     _DisplayLocation;
    QString     _Screen;
    QString     _EngineType;
    QString     _FwcMessage;
    QString     _FwcLabelBit;
    MessageType _MsgType;

};

#endif // GTAMESSAGE_H
