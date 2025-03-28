#ifndef GTAFSIIImportWidget_H
#define GTAFSIIImportWidget_H

#include <QDialog>

namespace Ui {
class GTAFSIIImportWidget;
}

class GTAFSIIImportWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GTAFSIIImportWidget(QWidget *parent = 0);
    ~GTAFSIIImportWidget();

    QString getExportPath()const;
    QString getImportPath()const;
    QString getErrorPath()const;
    bool getOverWriteFileStatus();
    void lockWidget();
    void unLockWidget();
    void setDefaultExportPath(const QString &iDefPath);
    void show();

private slots:
    void onBrowseExportPathClicked();
    void onBrowseImportPathClicked();
    void onBrowseErrorPathClicked();
    void accepted();
signals:
    void startFSIIImport();
private:
    Ui::GTAFSIIImportWidget *ui;

};

#endif // GTAFSIIImportWidget_H
