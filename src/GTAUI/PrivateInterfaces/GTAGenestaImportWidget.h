#ifndef GTAGENESTAIMPORTWIDGET_H
#define GTAGENESTAIMPORTWIDGET_H

#include <QDialog>
#include <QHash>
#include <QCloseEvent>
#include "GTAAppController.h"
#include "GTACompatibilityReport.h"

namespace Ui {
class GTAGenestaImportWidget;
}

class GTAGenestaImportWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GTAGenestaImportWidget(GTACompatibilityReport * iReportWidget,QDialog *parent = 0);
    ~GTAGenestaImportWidget();
    
    QString getErrorFilePath();
    QString getGenestaLibPath();
    QString getObjectPath();
    QString getExportPath();
    void hideProgress();
    bool getOverWriteState();
    void setDefaultExportPath(const QString & iFilePath);
    void updateNoOfFiles(const int& iNoOfFiles);
    void lockWidget();
    void unLockWidget();
    

    QStringList getListOfFiles(){return _fileNames;}
protected:
    void	closeEvent ( QCloseEvent * event ) ;
public slots:
    void onUpdateProgress(int iProgress);
    void show();
signals:
    void startGenestaImport();
    void updateProgress(int );
private slots:
    void onBrowseGenestaLibPBClick();
    void onBrowseErrorFilePBClick();
    void accepted();
    void onBrowseObjectFolderPBClick();
    void onBrowseExportFolderPBClick();

private:
  
    Ui::GTAGenestaImportWidget *ui;
    QStringList _fileNames;
    int _progress;
    bool _bExporProgress;
    GTACompatibilityReport * _ReportWidget;

   



};

#endif // GTAGENESTAIMPORTWIDGET_H
