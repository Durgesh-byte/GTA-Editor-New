#ifndef GTASAVEPROMPT_H
#define GTASAVEPROMPT_H

#include <QDialog>
//#include <QMessageBox>

namespace Ui {
    class GTASavePrompt;
}

class GTASavePrompt : public QDialog
{
    Q_OBJECT

public:

enum DialogCode{Rejected,Accepted,Ignored};
    explicit GTASavePrompt(QWidget *parent = 0);
    ~GTASavePrompt();

    void setFiles(QStringList &iFiles);


private slots:

    void onSaveAllClicked();
    void onNoPBClicked();
    void onCancelPBclicked();

private:
    Ui::GTASavePrompt *ui;
};

#endif // GTASAVEPROMPT_H
