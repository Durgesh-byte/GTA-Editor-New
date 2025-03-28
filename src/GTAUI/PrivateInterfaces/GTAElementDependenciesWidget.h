#ifndef GTAELEMENTDEPENDENCIESWIDGET_H
#define GTAELEMENTDEPENDENCIESWIDGET_H

#pragma warning (push, 0)
#include <QWidget>
#include <QLineEdit>
#include <QAbstractItemModel.h>
#include <QtCore>
#include <QtGui>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#pragma warning (pop)

#include "GTAActionCall.h"
#include "GTAAppController.h"
#include "GTACommandBase.h"
#include "GTAElement.h"

namespace Ui {
class GTAElementDependenciesWidget;
}

class GTAElementDependenciesWidget : public QWidget
{
	Q_OBJECT

public:
    struct FileInfo
    {
        QString fileName;
        bool isIgnored = false;
    };
    explicit GTAElementDependenciesWidget(QString fileName, GTAAppController* appController, QWidget *parent = 0);
    ~GTAElementDependenciesWidget();

    void displayName();    
    void displayUsedDocTree();
    void displayUsedTool();
    void displayUsedCsv();

private:
    Ui::GTAElementDependenciesWidget* ui;
    QMap<QString, QString> _elementType;
    QString _fileName;
    QString _pGTADataDirectory;  
    QString _pGTATableDirectory;
    std::vector<FileInfo> _allUsedDoc;
    QStringList _allUsedTool;
    QStringList _allUsedCsv;
    QString sep = ";";
    GTAAppController* _pAppController;
    
    std::vector<FileInfo> getUsedDoc(QString file);
    QStringList getUsedTool(int & nbTool);
    QStringList getUsedCsv(int& nbCsv);

    QTreeWidgetItem* addRoot(QString path);
    QTreeWidgetItem* addChild(QTreeWidgetItem* parent, QString path);
    void insertChildren(QTreeWidgetItem* item, QString file, bool isIgnored = false);

    bool isValid(QString file);
    QIcon iconOf(QString file);
    QColor colorOf(QString file);
    QPixmap assignIcon(GitStatus status) const;

    void getUsedAndIgnoredFileCount(int& usedFilesCount, int& ignoredFilesCount);
private slots:
    void onExportClicked();
};

#endif // GTAElementDependenciesWidget_H
