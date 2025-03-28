#include "GTAResultMdiArea.h"

GTAResultMdiArea* GTAResultMdiArea::_resultMdiArea = nullptr;
GTAResultMdiArea *GTAResultMdiArea::createResultMdiArea()
{
    if(!_resultMdiArea)
    {
		_resultMdiArea = new GTAResultMdiArea();
    }
    return _resultMdiArea;
}

GTAResultMdiArea::GTAResultMdiArea()
{
    _lastActiveWindow = nullptr;
    connect(this,&GTAResultMdiArea::subWindowActivated,this,&GTAResultMdiArea::onSubWindowActivated);
}

void GTAResultMdiArea::onSubWindowActivated(QMdiSubWindow* pSubWin)
{
    _lastActiveWindow = pSubWin;
}

/**
 * This getter function returns last active result page subwindow
 * @return: last active result page
*/
QMdiSubWindow* GTAResultMdiArea::lastActiveSubWindow()
{
    return _lastActiveWindow;
}
