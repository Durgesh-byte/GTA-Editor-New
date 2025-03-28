#include "GTALogAttachmentWidget.h"
#include "ui_GTALogAttachmentWidget.h"
#include <QFileDialog>
#include <QModelIndex>
GTALogAttachmentWidget::GTALogAttachmentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTALogAttachmentWidget)
{
    ui->setupUi(this);
    connect(ui->AddPB,SIGNAL(clicked()),this,SLOT(onAddPBClicked()));
    connect(ui->RemovePB,SIGNAL(clicked()),this,SLOT(onRemovePBCliecked()));
}

GTALogAttachmentWidget::~GTALogAttachmentWidget()
{
    delete ui;
}
void GTALogAttachmentWidget::clear()
{
    ui->AttachmentLW->clear();
}
QStringList GTALogAttachmentWidget::getItemList() const
{
    QStringList oList;
    int itemCount = ui->AttachmentLW->count();
    for(int i = 0; i < itemCount; i++)
    {
        QListWidgetItem * pItem = ui->AttachmentLW->item(i);
        if(pItem != NULL)
            oList.append(pItem->text());
    }
    return oList;
}
void GTALogAttachmentWidget::instertItem(const QString & iVal)
{
    ui->AttachmentLW->addItem(new QListWidgetItem(iVal));
}
void GTALogAttachmentWidget::onAddPBClicked()
{
    QString file = QFileDialog::getOpenFileName(this,"Attachment");
    if(! file.isEmpty())
        instertItem(file);
}

void GTALogAttachmentWidget::onRemovePBCliecked()
{
    QModelIndex index = ui->AttachmentLW->currentIndex();
    if(index.isValid())
    {
        QListWidgetItem * pItem = ui->AttachmentLW->takeItem(index.row());
        if(pItem != NULL)
        {
            delete pItem; pItem= NULL;
        }
    }
}
void GTALogAttachmentWidget::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}