#ifndef GTAIMPORTDIALOG_H
#define GTAIMPORTDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class GTAImportDialog;
}

class GTAImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GTAImportDialog(QWidget *parent = 0);
    ~GTAImportDialog();

    void lockWidget();
    void unLockWidget();
    void show();

    void setDefaultExportPath(const QString &iDirPath);
    QString getExportPath()const;
    QString getImportPath()const;
    QString getErrorPath()const;
    bool getOverWriteFileStatus()const;
    QString getPluginType()const;
    void setPluginTypes(const QStringList &iList);

signals:
     void startImport();
     void widgetclosed();
private:
    void controlWidget(bool iControlVal);

private slots:
    void accepted();
    void onBrowseExportPathClicked();
    void onBrowseImportPathClicked();
    void onBrowseErrorPathClicked();

private:
    Ui::GTAImportDialog *ui;
};

#endif // GTAIMPORTDIALOG_H
