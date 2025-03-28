#include "GTAEditorFrame.h"
#include "GTAEditorWindow.h"

GTAEditorFrame::GTAEditorFrame(QWidget* parent)
	: QFrame(parent)
{
    setContentsMargins(0, 0, 0, 0);

    _views = new QSplitter();
    _views->setHandleWidth(0);
    _views->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* frameLayout = new QVBoxLayout();
    frameLayout->addWidget(_views);
    setLayout(frameLayout);

    _currentTabIndices = TabIndices();

    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
}

/**
* @brief Adds a view to the split view frame only if required. Requirements are based on the desired view index and the maximum number of views authorized.
*/
void GTAEditorFrame::addView()
{
    QFrame* tabFrame = new QFrame();
    QVBoxLayout* tabLayout = new QVBoxLayout();
    DraggableTabWidget* tabWidget = new DraggableTabWidget(tabFrame);

    tabWidget->setElideMode(Qt::ElideRight);
    tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    tabWidget->setTabsClosable(true);
    
    connect(tabWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(triggerMenu(QPoint)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(registerCurrentTabIndices(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequested()));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SIGNAL(informAboutTabChange()));
    connect(tabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(registerCurrentTabIndices(int)));
    connect(tabWidget, SIGNAL(tabBarClicked(int)), this, SIGNAL(informAboutTabChange()));

    tabLayout->addWidget(tabWidget);

    tabFrame->setLayout(tabLayout);
    tabFrame->setStyleSheet("QFrame:focus{border: solid rgb(50, 50, 238);  border-width: 1px;}");
    tabFrame->setFocusPolicy(Qt::ClickFocus);
    
    _views->addWidget(tabFrame);
}

/**
* @brief Adds a widget to the tab widget of the procedure editor frame. It is general, but will mainly handle EditorWindows.
* @param window QWidget to include.
* @param windowName Name of the window.
* @param viewIndex Index of the view (for split view) to add the window to.
*/
void GTAEditorFrame::addWindow(QWidget* window, const QString& windowName, uint viewIndex)
{
    int count = getViewsCount();
    if (viewIndex == 0 && _views->count() == 0)
        addView();
    else if (viewIndex >= static_cast<uint>(_views->count()) && static_cast<uint>(_views->count()) < kMaxViews)
    {
        addView();
        viewIndex = static_cast<uint>(_views->count() - 1);
    }
    else if (viewIndex >= static_cast<uint>(_views->count()) && static_cast<uint>(_views->count()) >= kMaxViews)
        viewIndex = static_cast<uint>(_views->count() - 1);

    DraggableTabWidget* tabWidget = getTabWidget(viewIndex);
    connect(window, SIGNAL(fileHasChanged(TabIndices, QString)), this, SLOT(updateTabName(TabIndices, QString)));
    tabWidget->addTab(window, windowName);
    tabWidget->setCurrentIndex(tabWidget->count() - 1);

    _currentTabIndices = TabIndices(viewIndex, tabWidget->count() - 1);
}

/**
* @brief Closes a single tab as per its indices.
* @param viewIndex Index of the tab widget (= view).
* @param tabIndex Index within the tab widget.
*/
void GTAEditorFrame::closeTab(uint viewIndex, uint tabIndex)
{   
    QWidget* window = getTabWidget(viewIndex)->widget(tabIndex);
    delete window;
}

/**
* @brief Closes all the tabs in a tab widget designated by its view index.
* @param viewIndex The index of the tab widget (view).
* @param except Index of the (potentially) excluded tab.
*/
void GTAEditorFrame::closeAllTabs(uint viewIndex, int except)
{
    DraggableTabWidget* currentTabWidget = getTabWidget(viewIndex);
    QWidget* activeWindow = getCurrentWindow();
    uint activeWindowIndex = getTabWidget(viewIndex)->currentIndex();
    bool closeActiveWindow = false;
    for (int i = currentTabWidget->count() - 1; i >= 0; i--)
    {
        if (i == except)
            continue;
        else if (i == activeWindowIndex)
        {
            closeActiveWindow = true;
            continue;
        }
        closeTab(viewIndex, i);
    }
    int count = currentTabWidget->count();
    if ((closeActiveWindow && except <= -1) || (except > activeWindowIndex))
    {
        closeTab(viewIndex, 0);
    } 
    else if (except < activeWindowIndex)
    {         
        closeTab(viewIndex, 1);
    }      
}

/**
* @brief Checks if a view needs to be closed and closes it.
*/
void GTAEditorFrame::closeView()
{
    if (getViewsCount() > 1)
    {
        if (getTabWidget(0)->count() == 0)
            closeView(0);
        else if (getTabWidget(1)->count() == 0)
            closeView(1);
    }
}

/**
* @brief Closes a selected view in the frame.
* @param viewIndex The index of the view to close.
*/
void GTAEditorFrame::closeView(int viewIndex)
{
    QWidget* tabWidgetFrame = getTabWidget(viewIndex)->parentWidget();
    tabWidgetFrame->close();
    delete tabWidgetFrame;
    _currentTabIndices = TabIndices(0, getTabWidget(0)->currentIndex());
}

/**
* @brief Returns the current tab widget in focus.
*/
DraggableTabWidget* GTAEditorFrame::getTabWidget(uint viewIndex) const
{
    QWidget* widget = _views->widget(viewIndex)->layout()->itemAt(0)->widget();
    DraggableTabWidget* tabWidget = qobject_cast<DraggableTabWidget*>(widget);
    return tabWidget;
}

/**
* @brief Returns the current tab widget.
*/
DraggableTabWidget* GTAEditorFrame::getCurrentTabWidget() const
{
    if (_currentTabIndices.viewIndex.has_value())
    {
        QWidget* current = _views->widget(*_currentTabIndices.viewIndex)->layout()->itemAt(0)->widget();
        return qobject_cast<DraggableTabWidget*>(current);
    }
    else
        return nullptr;
}

/**
* @brief Returns the current window (widget).
*/
QWidget* GTAEditorFrame::getCurrentWindow() const
{
    DraggableTabWidget* currentTab = getCurrentTabWidget();
    if (currentTab != nullptr && _currentTabIndices.tabIndex.has_value())
        return currentTab->widget(*_currentTabIndices.tabIndex);
    else
        return nullptr;
}

/**
* @brief Returns all the window widgets in a specific view.
*/
QList<GTAEditorWindow*> GTAEditorFrame::getAllWindows(int viewIndex) const
{
    DraggableTabWidget* currentTabWidget = getTabWidget(viewIndex);
    QList<GTAEditorWindow*> windowsList;
    for (int i = 0; i < currentTabWidget->count(); ++i)
        windowsList.append(qobject_cast<GTAEditorWindow*>(currentTabWidget->widget(i)));
    return windowsList;
}

/**
* @brief Updates the current window indices in the editor frame.
* @param viewIndex Index of the view (tab widget).
* @param refTabIndex Pivotal tab index made to adjust the indexing. If this value is -1, the view index is set to 0.
*/
void GTAEditorFrame::updateWindowsPosition(uint viewIndex, int refTabIndex)
{
    for (uint view = 0; view < getViewsCount(); view++)
    {
        DraggableTabWidget* tabWidget = getTabWidget(view);
        QList<GTAEditorWindow*> windowsList = getAllWindows(view);
        for (uint i = 0; i < tabWidget->count(); i++)
        {
            QString tabName = tabWidget->tabText(i);
            const auto it = std::find_if(windowsList.begin(), windowsList.end(), [&](GTAEditorWindow* w) {return w->windowTitle() == tabName; });
            GTAEditorWindow* window = *it;
            (*it)->setFramePosition(TabIndices(view, i));
        }
    }
    refTabIndex = refTabIndex == -1 ? getTabWidget(viewIndex)->count() - 1 : refTabIndex;
    setCurrentTabIndices(TabIndices(viewIndex, refTabIndex));
    getTabWidget(viewIndex)->setCurrentIndex(refTabIndex);
    getTabWidget(viewIndex)->parentWidget()->setFocus();
}

/**
* @brief Returns the current tab indices (current view index + current tab index).
* @return current tab indices (pair of ints).
*/
GTAEditorFrame::TabIndices GTAEditorFrame::getCurrentTabIndices() const
{
    return _currentTabIndices;
}

/**
* @brief Sets the tab indices (view index + tab index).
* @param tab indices (pair).
*/
void GTAEditorFrame::setCurrentTabIndices(const TabIndices& indices)
{
    _currentTabIndices = indices;
}

/**
* @brief Returns current number of views in the frame.
* @return Number of views as an unsigned int.
*/
uint GTAEditorFrame::getViewsCount() const
{
    return _views->count();
}

/**
 * @brief Checks if any window in any view is opened
*/
bool GTAEditorFrame::isAnyWindowOpened() const {
    size_t windowsCount = 0;
    for (size_t i = 0; i < getViewsCount(); ++i) {
        windowsCount += getAllWindows(i).size();
    }

    return windowsCount;
}

/**
* @brief Emits a signal to trigger custom menu from mainwindow.
*/
void GTAEditorFrame::triggerMenu(QPoint pt)
{
    emit displayTabContextMenu(pt);
}

/**
* @brief We need to send a signal that triggers the proper closing of the window via the close tab button.
*/
void GTAEditorFrame::onTabCloseRequested()
{
    emit closeWindowAndTab();
}

/**
* @brief Saves the tab indices (view + tab index) when requested (connected to click-on-tab signals).
* @param tabIndex Index of the tab being clicked on.
*/
void GTAEditorFrame::registerCurrentTabIndices(int tabIndex)
{
    _currentTabIndices.viewIndex = _views->indexOf(_views->focusWidget());
    _currentTabIndices.tabIndex = tabIndex;
    focusCurrent();
}

/**
* @brief Updates the tab name according to changes made to the file.
* @param tabPos Position of the window (view index + tab index).
* @param fileName New name to assign to the tab.
*/
void GTAEditorFrame::updateTabName(TabIndices tabPos, QString fileName)
{
    QTabWidget* tabWidget = getTabWidget(*tabPos.viewIndex);
    tabWidget->setTabText(*tabPos.tabIndex, fileName);
}

/**
* @brief Set focus on current tab widget & window.
*/
void GTAEditorFrame::focusCurrent()
{
	QWidget* pCurrentWnd = getCurrentWindow();
	if (!pCurrentWnd)
	{
        return;
	}

    pCurrentWnd->setFocus();
    getCurrentTabWidget()->parentWidget()->setFocus();
}

/**
 * @brief deletes all views and nullopts the tab/view indexes, to be used after last window is closed
*/
void GTAEditorFrame::deleteAllViews()
{
    for (size_t i = 0; i < kMaxViews; ++i) {
        auto* view = _views->widget(i);
        if (view) {
            delete view;
        }
    }
    _currentTabIndices.tabIndex = std::nullopt;
    _currentTabIndices.viewIndex = std::nullopt;
}

/**
* @brief Checks if the newly focused widget is one child of the tab widgets, sets the current tab indices and set focus.
* @param newWidget New widget forwarded by the signal.
*/
void GTAEditorFrame::focusChanged(QWidget*, QWidget* newWidget)
{
    for (int viewIndex = 0; viewIndex < _views->count(); viewIndex++)
    {
        if (newWidget != nullptr)
        {
            QWidget* viewWidget = _views->widget(viewIndex)->layout()->itemAt(0)->widget();
            QWidget* daddy = newWidget->parentWidget()->parentWidget();
            QWidget* granny = nullptr;
            if (daddy != nullptr)
                granny = newWidget->parentWidget()->parentWidget()->parentWidget();
            if (newWidget != nullptr && daddy != nullptr && granny != nullptr && (viewWidget == daddy || viewWidget == granny))
            {
                _currentTabIndices.viewIndex = viewIndex;
                _currentTabIndices.tabIndex = qobject_cast<QTabWidget*>(viewWidget)->currentIndex();
                focusCurrent();
                break;
            }
            
        }
    }
}

