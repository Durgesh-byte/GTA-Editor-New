#ifndef GTAKeyBoardShortcutsWidgetWIDGET_H
#define GTAKeyBoardShortcutsWidgetWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QDialog>

namespace Ui {
    class GTAKeyBoardShortcutsWidget;
}

class GTAKeyBoardShortcutsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTAKeyBoardShortcutsWidget(QWidget *parent = 0);
    ~GTAKeyBoardShortcutsWidget();

  
signals:
 

private:
    Ui::GTAKeyBoardShortcutsWidget *ui;

private slots:
   
};

#endif // GTAKeyBoardShortcutsWidget_H
