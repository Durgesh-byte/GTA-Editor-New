#pragma once
#include <QFrame>
#include <QSplitter>
#include <qboxlayout.h>
#include <qtabwidget.h>
#include <qtabbar.h>
#include <qmenu.h>
#include "GTADraggableTabWidget.h"
#include "GTAUI.h"

#include <QApplication>
#include <optional>

class GTAEditorWindow;

constexpr uint kMaxViews = 2;

/*! @class GTAEditorFrame
	@brief Class handling the behavior of the procedure editor UI.

	It inherits QFrame and defines some functions for dealing with open procedures.
	It allows the use of a split view.
*/
class GTAUI_SHARED_EXPORT GTAEditorFrame : public QFrame
{
	Q_OBJECT;

public:
struct GTAEditorFrame::TabIndices
{
	TabIndices() : viewIndex(std::nullopt), tabIndex(std::nullopt) {}
	TabIndices(uint vIndex, uint tIndex)
		: viewIndex(vIndex), tabIndex(tIndex) {}
	std::optional<uint> viewIndex;
	std::optional<uint> tabIndex;
};

private:
	QSplitter* _views;
	TabIndices _currentTabIndices;

public:
	GTAEditorFrame(QWidget* parent = (QWidget*)nullptr);
	virtual ~GTAEditorFrame() = default;

	void addWindow(QWidget* window, const QString& windowName, uint viewIndex);
	void closeTab(uint viewIndex, uint tabIndex);
	void closeAllTabs(uint viewIndex, int except=-1);
	void closeView();
	void closeView(int viewIndex);

	TabIndices getCurrentTabIndices() const;
	DraggableTabWidget* getTabWidget(uint viewIndex) const;
	DraggableTabWidget* getCurrentTabWidget() const;
	QWidget* getCurrentWindow() const;
	QList<GTAEditorWindow*> getAllWindows(int viewIndex) const;

	void setCurrentTabIndices(const TabIndices& indices);
	uint getViewsCount() const;
	bool isAnyWindowOpened() const;
	void updateWindowsPosition(uint viewIndex, int refTabIndex);
	void focusCurrent();
	void deleteAllViews();

private:
	void addView();

private slots:
	void triggerMenu(QPoint);
	void onTabCloseRequested();
	void registerCurrentTabIndices(int tabIndex);
	void updateTabName(TabIndices tabPos, QString fileName);
	void focusChanged(QWidget*, QWidget* newWidget);

signals:
	void displayTabContextMenu(QPoint);
	void closeWindowAndTab();
	void informAboutTabChange();
};