#include "GTATagVariableDescWidget.h"
#include "ui_GTATagVariableDescWidget.h"

#define TAGVAR_TABLE_COL_CNT 7
#define TAGVAR_TABLE_NAME_COL 0
#define TAGVAR_TABLE_TYPE_COL 1
#define TAGVAR_TABLE_DESC_COL 2
#define TAGVAR_TABLE_UNIT_COL 3
#define TAGVAR_TABLE_VALUE_COL 4
#define TAGVAR_TABLE_MAX_COL 5
#define TAGVAR_TABLE_MIN_COL 6

GTATagVariableDescWidget::GTATagVariableDescWidget(QString ielementPath,GTAWidget *parent) :
    GTAWidget(parent),
    ui(new Ui::GTATagVariableDescWidget)
{
    ui->setupUi(this);
    ui->tagVarTW->verticalHeader()->setVisible(false);
    ui->tagVarTW->horizontalHeader()->setStretchLastSection(true);
    connect(ui->tagVarTW->horizontalHeader(),SIGNAL(sectionResized(int, int, int)),ui->tagVarTW,SLOT(resizeRowsToContents()));

    connect(ui->okPB,SIGNAL(clicked(bool)),this,SLOT(onOKPBClicked()));
    _elementPath = ielementPath;
}

GTATagVariableDescWidget::~GTATagVariableDescWidget()
{
    delete ui;
}

void GTATagVariableDescWidget::showWidget(QList<TagVariablesDesc> tagList)
{
    ui->tagVarTW->setRowCount(tagList.count());
    ui->tagVarTW->setColumnCount(TAGVAR_TABLE_COL_CNT);
    int i=0;
    foreach(TagVariablesDesc tag, tagList)
    {

        QTableWidgetItem *pItemName =  new QTableWidgetItem(tag.name);
        pItemName->setFlags(pItemName->flags() ^ Qt::ItemIsEditable);
        pItemName->setTextAlignment(Qt::AlignCenter);
        ui->tagVarTW->setItem(i,TAGVAR_TABLE_NAME_COL,pItemName);

        QTableWidgetItem *pItemType =  new QTableWidgetItem(tag.type);
        pItemType->setTextAlignment(Qt::AlignCenter);
        ui->tagVarTW->setItem(i,TAGVAR_TABLE_TYPE_COL,pItemType);

        QTableWidgetItem *pItemDesc =  new QTableWidgetItem(tag.Desc);
        pItemDesc->setTextAlignment(Qt::AlignCenter);
        ui->tagVarTW->setItem(i,TAGVAR_TABLE_DESC_COL,pItemDesc);

        QTableWidgetItem *pItemUnit =  new QTableWidgetItem(tag.unit);
        pItemUnit->setTextAlignment(Qt::AlignCenter);
        ui->tagVarTW->setItem(i,TAGVAR_TABLE_UNIT_COL,pItemUnit);

        QTableWidgetItem *pItemValue =  new QTableWidgetItem(tag.supportedValue);
        pItemValue->setTextAlignment(Qt::AlignCenter);
        ui->tagVarTW->setItem(i,TAGVAR_TABLE_VALUE_COL,pItemValue);

        QTableWidgetItem *pItemMax =  new QTableWidgetItem(tag.max);
        pItemMax->setTextAlignment(Qt::AlignCenter);
        ui->tagVarTW->setItem(i,TAGVAR_TABLE_MAX_COL,pItemMax);

        QTableWidgetItem *pItemMin =  new QTableWidgetItem(tag.min);
        pItemMin->setTextAlignment(Qt::AlignCenter);
        ui->tagVarTW->setItem(i,TAGVAR_TABLE_MIN_COL,pItemMin);
        i++;

    }
    show();

    this->activateWindow();
}

void GTATagVariableDescWidget::onOKPBClicked()
{
    QList<TagVariablesDesc> tagList;
    for(int i=0 ; i < ui->tagVarTW->rowCount() ; i++)
    {
        TagVariablesDesc tagVar;
        tagVar.name = ui->tagVarTW->item(i,TAGVAR_TABLE_NAME_COL)->text();
        tagVar.type = ui->tagVarTW->item(i,TAGVAR_TABLE_TYPE_COL)->text();
        tagVar.Desc = ui->tagVarTW->item(i,TAGVAR_TABLE_DESC_COL)->text();
        tagVar.unit = ui->tagVarTW->item(i,TAGVAR_TABLE_UNIT_COL)->text();
        tagVar.supportedValue = ui->tagVarTW->item(i,TAGVAR_TABLE_VALUE_COL)->text();
        tagVar.max = ui->tagVarTW->item(i,TAGVAR_TABLE_MAX_COL)->text();
        tagVar.min = ui->tagVarTW->item(i,TAGVAR_TABLE_MIN_COL)->text();
        tagList.append(tagVar);
    }
    emit(sendUserInput(_elementPath, tagList));
    close();

}
