#ifndef GTAPORTINGDIALOG_H
#define GTAPORTINGDIALOG_H

#include <QDialog>

namespace Ui {
class GTAPortingDialog;
}

class GTAPortingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GTAPortingDialog(QWidget *parent = 0);
    ~GTAPortingDialog();

    void lockWidget();
    void unLockWidget();
    void show();


    QString getPortingPath()const;

signals:
    void startPortingFiles(const QString &iPortDirPath);
    void portingDone();


public slots:
     void close();

private slots:
    void accepted();

    void onBrowsePortingPathClicked();

private:
    Ui::GTAPortingDialog *ui;
};

#endif // GTAPORTINGDIALOG_H
