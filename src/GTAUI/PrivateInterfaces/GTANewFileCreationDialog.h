#ifndef GTANEWFILECREATIONDIALOG_H
#define GTANEWFILECREATIONDIALOG_H

#include <QDialog>
#include "GTAAppController.h"

namespace Ui {
    class GTANewFileCreationDialog;
}

class GTANewFileCreationDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogMode { NEWFILE, SAVEAS, SEQUENCE, SAVE_AS_DOCX, NEW_CSV2PRO_FILE };
    explicit GTANewFileCreationDialog(QWidget* parent = 0);
    ~GTANewFileCreationDialog();
    void setDataDirPath(const QString& iPath);

    QString getFileType();
    QString getFileName();
    QString getFilePath();
    QString getAuthorName(); // V26 editor view column enhancement #322480 -- to get author name from UI
    QString getMaxTimeEstimated();
    void setFileTypes(const QStringList& iFileTypesList = {});
    void setFileType(const QString& iFileType);
    void setFileName(const QString& iFileName);
    void setAuthorName(const QString& iAuthorName);
    void setFilePath(const QString& iFilePath = "");
    void setMaxTimeEstimated(const QString& iMaxTimeEstimated);
    void clickOk();

    void setWindowTitle(const QString& iTitle);
    void show();
    void setMode(DialogMode mode);
    DialogMode getMode() { return _DialogMode; }

    bool isFileValid();

    void setElement(GTAElement* iElement);
    GTAElement* getElement()const;
    QString getPath() const;
    bool isSaveAsValid();

    bool _isConsole = false;
    bool _forceOverwrite = false;

signals:
    void newFileAccepted();
    void saveAsAccepted();
    void sequenceCreated();
    void saveAsDocxAccepted();

    void restore_file_watcher();
    void disconnect_file_watcher();
    void newCsvToProConvertedFileAccepted();


private slots:
    void onBrowserTBClicked();
    void onOkPBClicked();

private:
    Ui::GTANewFileCreationDialog* ui;
    QString _GtaDataDir;
    DialogMode _DialogMode;
    bool _isFileValid;
    QString _currExtension;
    GTAElement* _pElement;
};

#endif // GTANEWFILECREATIONDIALOG_H