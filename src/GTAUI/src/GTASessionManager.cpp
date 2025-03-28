#include "GTASessionManager.h"
#include "ui_GTASessionManager.h"

#include <QMessageBox>

GTASessionManager::GTASessionManager(GTAWidget *parent) :
    GTAWidget(parent),
    ui(new Ui::GTASessionManager)
{
    ui->setupUi(this);
    ui->SessionNameLE->setVisible(false);
    ui->SessionNameLE->setToolTip("Enter session name");
    ui->SavePB->setVisible(false);
    ui->SavePB->setToolTip("Save Session");
    ui->NewPB->setToolTip("Create New session");
    ui->DeletePB->setToolTip("Delete selected Session");
    connect(ui->ClosePB,SIGNAL(clicked()),this,SIGNAL(widgetClosed()));
    connect(ui->NewPB,SIGNAL(clicked()),this,SLOT(onNewPBClicked()));
    connect(ui->DeletePB,SIGNAL(clicked()),this,SLOT(onDeletePBClicked()));
    connect(ui->SavePB,SIGNAL(clicked()),this,SLOT(onSavePBClicked()));
    connect(ui->ClearTB,SIGNAL(clicked()),this,SLOT(onClearClicked()));
    connect(ui->SwitchPB,SIGNAL(clicked()),this,SLOT(onSwitchSessionClicked()));
    connect(ui->SessionLW,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onSessionSelected(QListWidgetItem*)));
}

GTASessionManager::~GTASessionManager()
{
    delete ui;
}

void GTASessionManager::onSessionSelected(QListWidgetItem *item)
{
    if(item != NULL)
    {
        QString text = item ->text();
        if(text == "Default")
            ui->DeletePB->setEnabled(false);
        else
             ui->DeletePB->setEnabled(true);
    }

}

void GTASessionManager::show()
{
    ui->SessionNameLE->setVisible(false);
    ui->SavePB->setVisible(false);
    ui->ClearTB->setVisible(false);
    this->QWidget::show();
}

void GTASessionManager::close()
{
    this->QWidget::close();
}

void GTASessionManager::closeEvent(QCloseEvent *)
{
    emit widgetClosed();
}

void GTASessionManager::onNewPBClicked()
{
    ui->SessionNameLE->setVisible(true);
    ui->SavePB->setVisible(true);
    ui->ClearTB->setVisible(true);
    ui->SessionNameLE->clear();

}

void GTASessionManager::onDeletePBClicked()
{
    QStringList selectedItems;
    QList<QListWidgetItem *> lstSelectedItems = ui->SessionLW->selectedItems();
    for(int i = 0; i < lstSelectedItems.count(); i++)
    {
        QListWidgetItem *pItem = lstSelectedItems.at(i);
        if(pItem)
        {
            QString file = pItem->text();
            selectedItems.append(file);
            _sessions.removeAll(file);

        }
    }
    emit deleteSession(selectedItems);
    lstSelectedItems.clear();
    qDeleteAll(ui->SessionLW->selectedItems());
}

void GTASessionManager::onSavePBClicked()
{
    QString sessionName = ui->SessionNameLE->text();
    if(_sessions.contains(sessionName))
    {
        QMessageBox::warning(this,"Error","The session name already exists. Please provide a different session name",QMessageBox::Ok);
    }
    else
    {
        emit saveSession(sessionName);
        _sessions.append(sessionName);
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setText(sessionName);
        ui->SessionLW->addItem(pItem);
    }
}

void GTASessionManager::setSessions(const QStringList &iLstSessions)
{
    _sessions.clear();
    _sessions = iLstSessions;
    ui->SessionLW->clear();
    foreach(QString session, iLstSessions)
    {
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setText(session);
        ui->SessionLW->addItem(pItem);

    }


}

void GTASessionManager::onClearClicked()
{
    ui->SessionNameLE->clear();
}


void GTASessionManager::onSwitchSessionClicked()
{
    QString sessionName;
    if(ui->SessionLW->count() > 0 && ui->SessionLW->selectedItems().count() > 0)
    {
        QListWidgetItem *pItem = ui->SessionLW->selectedItems().at(0);
        sessionName = pItem->text();
    }
    emit switchSession(sessionName);
}
