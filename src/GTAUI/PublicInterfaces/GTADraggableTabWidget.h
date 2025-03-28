// Copyright (c) 2019 Akihito Takeuchi
// https://github.com/akihito-takeuchi/qt-draggable-tab-widget
// Distributed under the MIT License : http://opensource.org/licenses/MIT
//
// Amended for GTA purposes by Florian Lardeux (@epsyl-alcen.com).

#pragma once

#include <QTabWidget>
#include <QIcon>

/*! @class DraggableTabWidget
    @brief Class designed to hold a custom QTabBar (see DraggableTabBar) to handle drag and drop behaviors.
*/
class DraggableTabWidget : public QTabWidget {
    Q_OBJECT
public:
    class TabInfo {
    public:
        TabInfo() = default;
        TabInfo(QWidget* widget, const QString& text, const QIcon& icon,
            const QString& tool_tip, const QString& whats_this, const int index)
            : widget_(widget), text_(text), icon_(icon),
            tool_tip_(tool_tip), whats_this_(whats_this), index_(index) {}
        ~TabInfo() = default;

        QWidget* widget() const { return widget_; }
        QString text() const { return text_; }
        QIcon icon() const { return icon_; }
        QString toolTip() const { return tool_tip_; }
        QString whatsThis() const { return whats_this_; }
        int index() const { return index_; }

    private:
        QWidget* widget_ = nullptr;
        QString text_;
        QIcon icon_;
        QString tool_tip_;
        QString whats_this_;
        int index_;
    };

    DraggableTabWidget(QWidget* parent = nullptr);
    ~DraggableTabWidget() = default;
};
