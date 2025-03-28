#ifndef GTAEXPORTDOCWIDGET_H
#define GTAEXPORTDOCWIDGET_H

#include <QDialog>

namespace Ui {
class GTAExportDocWidget;
}

class GTAExportDocWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GTAExportDocWidget(QDialog *parent = 0);
    ~GTAExportDocWidget();

    void setErrorInformation(const QString& iError);
    void setPath(const QString&);
    void setSaveFolder(const QString&);
    void setLstOfFilesToExport(const QStringList& ilstFiles);
    void setExportFileName(const QString &iFileName);
    void scxmlExportDisable(bool);
    void setDefaultLogFolderPath(const QString &iPath);
    void show();
    //QString getValue() const;
signals:
    void startExporting(const QString& iSaveFolder,const QString& iFolderName,const QStringList& ilstOfDocuments,bool ibIncludeBDB,bool exportSCXML, bool compress, const QString &iFileName,const QString &iLogFile);
private slots:
    void onExportAccepted(void);
    void onBrowseFolder(void);
    void onBrowseLogFile();
private:
    Ui::GTAExportDocWidget *ui;
    QStringList _lstOfFiles;
    QString _saveFolderName;
    QString _ExportFileName;
    QString _LogFilePath;
    QString _LogFolderPath;
};

#endif // GTAExportDocWidget_H
