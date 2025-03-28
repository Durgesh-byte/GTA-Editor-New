#include "GTAActionWidgetInterface.h"

GTAActionWidgetInterface::GTAActionWidgetInterface(QWidget * Parent): QWidget(Parent)
{
    _newPrecison=QString();
}
QString GTAActionWidgetInterface::getLastError()
{
    return _LastError;
}
void GTAActionWidgetInterface::connectExternalItemsWithLocalWidget(QWidget *)
{


}