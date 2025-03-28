#ifndef GTASVNLISTALLWIDGET_H
#define GTASVNLISTALLWIDGET_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class GTASVNListAllWidget;
}

class GTASVNListAllWidget : public QDialog
{
    Q_OBJECT
public:
    explicit GTASVNListAllWidget(QWidget *parent = 0);

    ~GTASVNListAllWidget();
    void setTitle(const QString &iTitleText);
    void clear();
    void setColumnHeaders(const QStringList &iHeaderItems);
    QStringList getSelectedList();
    void displayList(bool isCheckable, const QHash<QString,QString> &iFileList);
    void showUpdateDirCB(bool iVal);
    bool isUpdateDirchecked() const;
    int getStatus(){return _status;}
    void showCommitMessage(bool iVal);
    QString getCommitMessage();
public slots:
    void onClosePBClicked();
    void onOkPBClicked();
    void onSelectAllChecked(bool isSelected);
private:
    Ui::GTASVNListAllWidget *ui;
    QStringList _FileList;
    QStringList _SelectedFiles;
    void getAllFilesinPath(const QString &iPath);
    void displayList(bool isCheckable);

    int _status;

};

#endif // GTASVNLISTALLWIDGET_H
