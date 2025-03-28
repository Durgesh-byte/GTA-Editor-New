#include "AINGTASEQUENCERConfigurationManager.h"
#include "ui_AINGTASEQUENCERConfigurationManager.h"
#include <QMessageBox>
#include <QListWidgetItem>


AINGTASEQUENCERConfigurationManager::AINGTASEQUENCERConfigurationManager(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AINGTASEQUENCERConfigurationManager)
{
    ui->setupUi(this);

    QSize si;
    si.setHeight(30);
    si.setWidth(30);

    ui->SavePb->setIconSize(si);
    ui->ClearTb->setIconSize(si);
    ui->ConfigNameLE->setFixedHeight(30);
    ui->ClearTb->setAutoRaise(true);
    ui->SavePb->setAutoRaise(true);

    ui->ConfigNameLE->setVisible(false);
    ui->ConfigNameLE->setToolTip("Enter Configuration name");
    ui->SavePb->setToolTip("Save Configuration");
    ui->NewPb->setToolTip("Create New Configuration");
    ui->DeletePb->setToolTip("Delete selected Configuration");
    ui->DeletePb->setEnabled(false);

    connect(ui->ClosePB,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->NewPb,SIGNAL(clicked()),this,SLOT(onNewPBClicked()));
    connect(ui->DeletePb,SIGNAL(clicked()),this,SLOT(onDeletePBClicked()));
    connect(ui->SavePb,SIGNAL(clicked()),this,SLOT(onSavePBClicked()));
    connect(ui->ClearTb,SIGNAL(clicked()),this,SLOT(onClearClicked()));
    connect(ui->ConfigurationLW,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onConfigurationSelected(QListWidgetItem*)));

}

AINGTASEQUENCERConfigurationManager::~AINGTASEQUENCERConfigurationManager()
{
    _configurations.clear();
    delete ui;
}

void AINGTASEQUENCERConfigurationManager::show()
{
    ui->ConfigNameLE->setVisible(false);
    ui->SavePb->setVisible(false);
    ui->ClearTb->setVisible(false);
    this->QWidget::show();
}

void AINGTASEQUENCERConfigurationManager::close()
{
    this->QWidget::close();
}

void AINGTASEQUENCERConfigurationManager::onNewPBClicked()
{
    ui->ConfigNameLE->clear();
    ui->ConfigNameLE->setVisible(true);
    ui->SavePb->setVisible(true);
    ui->ClearTb->setVisible(true);

}

void AINGTASEQUENCERConfigurationManager::onDeletePBClicked()
{
    QStringList selectedItems;
    QList<QListWidgetItem *> lstSelectedItems = ui->ConfigurationLW->selectedItems();
    for(int i = 0; i < lstSelectedItems.count(); i++)
    {
        QListWidgetItem *pItem = lstSelectedItems.at(i);
        if(pItem)
        {
            QString file = pItem->text();
            selectedItems.append(file);
            _configurations.removeAll(file);

        }
    }
    emit deleteConfig(selectedItems);
    lstSelectedItems.clear();
    qDeleteAll(ui->ConfigurationLW->selectedItems());
}

void AINGTASEQUENCERConfigurationManager::onSavePBClicked()
{
    QString configName = ui->ConfigNameLE->text();
    if(!configName.isEmpty())
    {
        if(_configurations.contains(configName))
        {
            QMessageBox::warning(this,"Error","The configuration name already exists. Please provide a different configuration name",QMessageBox::Ok);
        }
        else
        {
            emit saveConfiguration(configName);
            _configurations.append(configName);
            QListWidgetItem *pItem = new QListWidgetItem;
            pItem->setText(configName);
            ui->ConfigurationLW->addItem(pItem);
            ui->ConfigNameLE->clear();
            ui->ConfigNameLE->setVisible(false);
            ui->SavePb->setVisible(false);
            ui->ClearTb->setVisible(false);

        }
    }
}

void AINGTASEQUENCERConfigurationManager::onClearClicked()
{
    ui->ConfigNameLE->clear();
}

void AINGTASEQUENCERConfigurationManager::onConfigurationSelected(QListWidgetItem *item)
{
    if(item != NULL)
    {
        ui->DeletePb->setEnabled(true);
    }

}

void AINGTASEQUENCERConfigurationManager::setConfigurations(const QStringList &iLstConfigurtaions)
{
    _configurations.clear();
    _configurations = iLstConfigurtaions;
    ui->ConfigurationLW->clear();
    foreach(QString Configurtaion, iLstConfigurtaions)
    {
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setText(Configurtaion);
        ui->ConfigurationLW->addItem(pItem);

    }


}

void AINGTASEQUENCERConfigurationManager::closeEvent(QCloseEvent *event)
{
    emit widgetClosed();
}
