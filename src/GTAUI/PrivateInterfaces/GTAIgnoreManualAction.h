#ifndef GTAIGNOREMANUALACTION_H
#define GTAIGNOREMANUALACTION_H

#include "GTAEditorWindow.h"
#include <QDialog>
#include <QString>

namespace Ui {
class GTAIgnoreManualAction;
}

class GTAIgnoreManualAction : public QDialog
{
    Q_OBJECT

public:
    explicit GTAIgnoreManualAction(GTAEditorWindow* window, QWidget *parent = 0);
    ~GTAIgnoreManualAction();
    void updateTable(GTAEditorWindow* window);
    QList<int> getSelection();

private:
    void updateCheckBox();
    Ui::GTAIgnoreManualAction *ui;
    QList <int> _initialIndexes;
    QList <int> _finalIndexes;
    QList<int> _manActRows;
    GTAEditorWindow* _window;

private slots:
    void onIgnoreAll(bool state = false);
    void onItemChanged(int,int);
    void onCancelled();
};

#endif // GTAIGNOREMANUALACTION_H
