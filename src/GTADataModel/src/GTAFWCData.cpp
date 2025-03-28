#include "GTAfwcdata.h"

GTAFWCData::GTAFWCData()
{

    _Header = QString();
    _Message = QString();
}

void GTAFWCData::setHeader(const QString &iHeader)
{
    _Header = iHeader;
}


void GTAFWCData::setMessage(const QString &iMessage)
{
    _Message = iMessage;
}


QString GTAFWCData::getHeader() const
{
    return _Header;
}


QString GTAFWCData::getMessage() const
{
    return _Message;
}

QString GTAFWCData::getFWCColor() const
{
    return _Color;
}

void GTAFWCData::setFWCColor(const QString &iColor)
{
    _Color = iColor;
}
