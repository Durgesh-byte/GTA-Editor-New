#include "GTASVNListAllWidget.h"
#include "ui_GTASVNListAllWidget.h"
#include <QDir>
#include <QTableWidgetItem>
#include <QFileInfoList>

GTASVNListAllWidget::GTASVNListAllWidget(QWidget *parent) :
        QDialog(parent),ui(new Ui::GTASVNListAllWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint);
    setWindowModality(Qt::ApplicationModal);
    connect(ui->CancelPb,SIGNAL(clicked()),this,SLOT(onClosePBClicked()));
    connect(ui->OkPb,SIGNAL(clicked()),this,SLOT(onOkPBClicked()));
    connect(ui->SelectAllCB,SIGNAL(toggled(bool)),this,SLOT(onSelectAllChecked(bool)));

    clear();
    ui->UpdateDirCB->setVisible(false);
    ui->commitMessageLE->setVisible(false);
    ui->CommitMessagelabel->setVisible(false);
    ui->SVNFileListTW->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}
GTASVNListAllWidget::~GTASVNListAllWidget()
{
    delete ui;
}

void GTASVNListAllWidget::setTitle(const QString &iTitleText)
{
    setWindowTitle(iTitleText);
}

void GTASVNListAllWidget::clear()
{
    ui->SVNFileListTW->clear();
    _FileList.clear();
    _SelectedFiles.clear();
}

void GTASVNListAllWidget::setColumnHeaders(const QStringList &iHeaderItems)
{
    ui->SVNFileListTW->setHorizontalHeaderLabels(iHeaderItems);
    ui->SVNFileListTW->resizeColumnsToContents();
    ui->SVNFileListTW->horizontalHeader()->stretchLastSection();
}

void GTASVNListAllWidget::getAllFilesinPath(const QString &iPath)
{
    QDir dataDir(iPath);
    QStringList namedFilters;
    namedFilters <<"*.pro" <<"*.seq" <<"*.obj"<<"*.fun";
    QFileInfoList fileInfoList = dataDir.entryInfoList(namedFilters);
    foreach(QFileInfo info,fileInfoList)
    {
        _FileList<<info.fileName();
    }
    _FileList.sort();
}
void GTASVNListAllWidget::displayList(bool isCheckable)
{
    for(int fileIdx=0;fileIdx<_FileList.count();fileIdx++)
    {
        QTableWidgetItem item;
        ui->SVNFileListTW->setColumnCount(1);
        item.setText(_FileList.at(fileIdx));
        if(isCheckable)
            item.setCheckState(Qt::Unchecked);
        ui->SVNFileListTW->setItem(fileIdx,0,&item);
    }
}

void GTASVNListAllWidget::displayList(bool isCheckable, const QHash<QString, QString> &iFileList)
{
    ui->SVNFileListTW->setRowCount(iFileList.count());
    QStringList keys = iFileList.keys();
    keys.sort();
    for(int fileIdx=0;fileIdx<keys.count();fileIdx++)
    {
        ui->SVNFileListTW->setColumnCount(2);
        QString key = keys.at(fileIdx);
        if(!key.isEmpty())
        {
            QString val  = iFileList.value(key);
            QTableWidgetItem *fileNameItem = new QTableWidgetItem(key);
            QTableWidgetItem *StatusItem= new QTableWidgetItem(val);
            if(isCheckable)
                fileNameItem->setCheckState(Qt::Unchecked);
            //            int currRowCnt = ui->SVNFileListTW->rowCount();
            //            ui->SVNFileListTW->insertRow(fileIdx);
            ui->SVNFileListTW->setItem(fileIdx,0,fileNameItem);
            ui->SVNFileListTW->setItem(fileIdx,1,StatusItem);

        }
    }
    if(ui->SVNFileListTW->rowCount() >= iFileList.count())
    {
        ui->SVNFileListTW->removeRow(ui->SVNFileListTW->rowCount());
    }
}

void GTASVNListAllWidget::onClosePBClicked()
{
    _status = -1;
    clear();
    hide();
}
void GTASVNListAllWidget::onOkPBClicked()
{
    int rowCnt = ui->SVNFileListTW->rowCount();
    for(int i=0;i<rowCnt;i++)
    {
        QTableWidgetItem *pItem = ui->SVNFileListTW->item(i,0);
        if(pItem != NULL)
        {
            Qt::CheckState chkState = pItem->checkState();
            if(chkState == Qt::Checked)
            {
                _SelectedFiles.append(pItem->text());
            }
        }
    }
    _status=0;
    hide();
}

QString GTASVNListAllWidget::getCommitMessage()
{
    return ui->commitMessageLE->text();
}

QStringList GTASVNListAllWidget::getSelectedList()
{
    return _SelectedFiles;
}

void GTASVNListAllWidget::onSelectAllChecked(bool isSelected)
{
    int rowCnt = ui->SVNFileListTW->rowCount();
    for(int i=0;i<rowCnt;i++)
    {
        QTableWidgetItem *pItem = ui->SVNFileListTW->item(i,0);
        if(pItem != NULL)
        {
            if(isSelected)
                pItem->setCheckState(Qt::Checked);
            else
                pItem->setCheckState(Qt::Unchecked);
        }
    }
}

void GTASVNListAllWidget::showUpdateDirCB(bool iVal)
{
    ui->UpdateDirCB->setVisible(iVal);
}
void GTASVNListAllWidget::showCommitMessage(bool iVal)
{
    ui->commitMessageLE->setVisible(iVal);
    ui->CommitMessagelabel->setVisible(iVal);
}
bool GTASVNListAllWidget::isUpdateDirchecked() const
{
    return ui->UpdateDirCB->isChecked();
}
