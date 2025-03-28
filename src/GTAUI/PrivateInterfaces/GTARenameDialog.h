#ifndef GTARENAMEDIALOG_H
#define GTARENAMEDIALOG_H

#include <QDialog>

namespace Ui {
    class GTARenameDialog;
}

class GTARenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GTARenameDialog(QWidget *parent = 0);
    ~GTARenameDialog();

    QString getName()const;
    void setFileName(const QString &iFileName);

signals:
    void renameDone(QString);
private slots:

    void ValidateName(QString iName);
    void accepted();

public slots:

    void show();
private:
    Ui::GTARenameDialog *ui;

};

#endif // GTARENAMEDIALOG_H
