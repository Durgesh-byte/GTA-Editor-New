#ifndef GTASVNCHECKFORMODIFICATIONSWIDGET_H
#define GTASVNCHECKFORMODIFICATIONSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include "GTAUtil.h"

namespace Ui {
    class GTASVNCheckForModificationsWidget;
}

class GTASVNCheckForModificationsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTASVNCheckForModificationsWidget(QWidget *parent = 0);
    ~GTASVNCheckForModificationsWidget();

    /**
      Displays the differences in a tablewidget
      @iOutputDiff: differences of files to be displayed
    */
    void displayList(QStringList iOutputDiff);
private:
    Ui::GTASVNCheckForModificationsWidget *ui;

    /**
      Updates the QtblewidgetItem style based on the command type.
      @oitem: tablewidgetitem whose style needs to be updateed
	  @text: Item text
    */
    void updateItemStyle(QTableWidgetItem *& oitem,QString & text);
};

#endif // GTASVNCHECKFORMODIFICATIONSWIDGET_H
