#include "GTASearchWidgetInterface.h"

GTAGenSearchWidget* GTASearchWidgetInterface::_pGenSearchWidget=NULL;
GTASearchWidgetInterface::GTASearchWidgetInterface(QWidget* iParent):QWidget(iParent)
{

}
GTASearchWidgetInterface::~GTASearchWidgetInterface()
{

}
GTAGenSearchWidget* GTASearchWidgetInterface::getSearchWidget()
{
    if (_pGenSearchWidget==NULL)
    {
        _pGenSearchWidget = new GTAGenSearchWidget;
    }

    return _pGenSearchWidget;
}

void GTASearchWidgetInterface::resetSearchWidget()

{
   if (NULL!=_pGenSearchWidget)
   {
       
       ;
   }
    
}