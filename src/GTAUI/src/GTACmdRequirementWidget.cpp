#include "GTACmdRequirementWidget.h"
#include "ui_GTACmdRequirementWidget.h"
#include <QString>
#include <QStringList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QModelIndex>


GTACmdRequirementWidget::GTACmdRequirementWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTACmdRequirementWidget)
{
    ui->setupUi(this);

    connect(ui->RequirementLW,SIGNAL(clicked(QModelIndex)),this,SLOT(onRequirementLWClicked(QModelIndex)));
    connect(ui->addToListPB,SIGNAL(clicked()),this,SLOT(onAddPBClicked()));
    connect(ui->removeFromListPB,SIGNAL(clicked()),this,SLOT(onRemovePBClicked()));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKPBClicked()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(hide()));
    connect(ui->EditPB,SIGNAL(clicked()),this,SLOT(onEditPBClicked()));

}

void GTACmdRequirementWidget::show()
{
    ui->RequirementTE->clear();
    this->QWidget::show();
}

void GTACmdRequirementWidget::clear()
{
    ui->RequirementTE->clear();
    ui->RequirementLW->clear();
}

void GTACmdRequirementWidget::setRequirements(const QStringList &iList)
{
    for(int i = 0; i < iList.count(); i++)
    {
        QString sReq = iList.at(i);
        QListWidgetItem *pItem = new QListWidgetItem(ui->RequirementLW);
        pItem->setText(sReq);
        ui->RequirementLW->addItem(pItem);
    }
}

QStringList GTACmdRequirementWidget::getRequirements()const
{
    QStringList lstRequirements;

    for(int i =0; i < ui->RequirementLW->count(); i++)
    {
        QListWidgetItem *pItem = ui->RequirementLW->item(i);
        if(pItem)
        {
            QString sReq = pItem->text();
            lstRequirements.append(sReq);
        }
    }

    return lstRequirements;

}




void GTACmdRequirementWidget::onOKPBClicked()
{
    this->hide();
}


void GTACmdRequirementWidget::onEditPBClicked()
{
    QList<QListWidgetItem *> lstOfSelecteItems = ui->RequirementLW->selectedItems();
    if(lstOfSelecteItems.count() > 0)
    {
        QListWidgetItem * pItem = lstOfSelecteItems.at(0);
        if(pItem)
        {
            pItem->setText(ui->RequirementTE->toPlainText());

        }

    }
}

void GTACmdRequirementWidget::onRemovePBClicked()
{
    QList<QListWidgetItem *> lstOfSelecteItems = ui->RequirementLW->selectedItems();
    if(lstOfSelecteItems.count() > 0)
    {
        for(int i = 0; i < lstOfSelecteItems.count(); i++)
        {
            QListWidgetItem * pItem = lstOfSelecteItems.at(i);
            ui->RequirementLW->removeItemWidget(pItem);
            delete pItem;

        }
    }
}

void GTACmdRequirementWidget::onAddPBClicked()
{
    QString sRequirement = ui->RequirementTE->toPlainText();
    if(!sRequirement.isEmpty())
    {
        QListWidgetItem* pItem = new QListWidgetItem(ui->RequirementLW);
        pItem->setText(sRequirement);
        ui->RequirementLW->addItem(pItem);
        ui->RequirementLW->setCurrentIndex(QModelIndex());
    }
}

void GTACmdRequirementWidget::onRequirementLWClicked(const QModelIndex &)
{
    QList<QListWidgetItem *> lstOfSelecteItems = ui->RequirementLW->selectedItems();
    if(lstOfSelecteItems.count() > 0)
    {
          QListWidgetItem * pItem = lstOfSelecteItems.at(0);
          if(pItem)
          {
              QString sReq = pItem->text();
              ui->RequirementTE->setText(sReq);
          }
    }
}

//void GTACmdRequirementWidget::

GTACmdRequirementWidget::~GTACmdRequirementWidget()
{
    delete ui;
}
