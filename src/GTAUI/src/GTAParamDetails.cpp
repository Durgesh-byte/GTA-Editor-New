#include "GTAParamDetails.h"
#include "ui_GTAParamDetails.h"

GTAParamDetails::GTAParamDetails(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::GTAParamDetails)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
}

GTAParamDetails::~GTAParamDetails()
{
    delete ui;
}
void GTAParamDetails::setTableData(const QHash<QString, QString> &iTableData)
{
    int rowIdx=0;
    ui->tableWidget->setRowCount(iTableData.keys().count());
    foreach(QString feild,iTableData.keys())
    {
        ui->tableWidget->setItem(rowIdx,0,new QTableWidgetItem(feild));
        ui->tableWidget->setItem(rowIdx++,1,new QTableWidgetItem(iTableData.value(feild)));
    }
}

void GTAParamDetails::clearTableData()
{
    ui->tableWidget->clearContents();
}


void GTAParamDetails::setWidgetTitle(QString iTitle)
{
    this->setWindowTitle(iTitle);
}