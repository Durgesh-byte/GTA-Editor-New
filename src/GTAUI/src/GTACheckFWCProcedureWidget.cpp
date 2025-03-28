#include "GTACheckFWCProcedureWidget.h"
#include "ui_GTACheckFWCProcedureWidget.h"
#include "GTACheckFwcProcedure.h"

GTACheckFWCProcedureWidget::GTACheckFWCProcedureWidget(QWidget *parent) :
    GTACheckWidgetInterface(parent),
    ui(new Ui::GTACheckFWCProcedureWidget)
{
    ui->setupUi(this);

    _ProcedureTable.insert(0,ui->Proc1LE);
    _ProcedureTable.insert(1,ui->Proc2LE);
    _ProcedureTable.insert(2,ui->Proc3LE);
    _ProcedureTable.insert(3,ui->Proc4LE);
    _ProcedureTable.insert(4,ui->Proc5LE);
    _ProcedureTable.insert(5,ui->Proc6LE);
    _ProcedureTable.insert(6,ui->Proc7LE);
    _ProcedureTable.insert(7,ui->Proc8LE);
    _ProcedureTable.insert(8,ui->Proc9LE);
    _ProcedureTable.insert(9,ui->Proc10LE);
    _SearchButtonEditorMap.insert(ui->Search1PB,ui->Proc1LE);
    _SearchButtonEditorMap.insert(ui->Search2PB,ui->Proc2LE);
    _SearchButtonEditorMap.insert(ui->Search3PB,ui->Proc3LE);
    _SearchButtonEditorMap.insert(ui->Search4PB,ui->Proc4LE);
    _SearchButtonEditorMap.insert(ui->Search5PB,ui->Proc5LE);
    _SearchButtonEditorMap.insert(ui->Search6PB,ui->Proc6LE);
    _SearchButtonEditorMap.insert(ui->Search7PB,ui->Proc7LE);
    _SearchButtonEditorMap.insert(ui->Search8PB,ui->Proc8LE);
    _SearchButtonEditorMap.insert(ui->Search9PB,ui->Proc9LE);
    _SearchButtonEditorMap.insert(ui->Search10PB,ui->Proc10LE);

    foreach(QPushButton * pButton, _SearchButtonEditorMap.keys())
    {
       connect(pButton,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    }

}

GTACheckFWCProcedureWidget::~GTACheckFWCProcedureWidget()
{
    delete ui;
}
int GTACheckFWCProcedureWidget::getProcedureCount() const
{
    return _ProcedureTable.count();
}
QString GTACheckFWCProcedureWidget::getProcedure(int index) const
{
    QString oProcedure;
    if(_ProcedureTable.contains(index))
    {
        QLineEdit *pEditor = _ProcedureTable.value(index);
        if(pEditor != NULL)
            oProcedure = pEditor->text();
    }
    return oProcedure;
}
QString GTACheckFWCProcedureWidget::getCondition() const
{
    return ui->OperationCB->currentText();
}
void GTACheckFWCProcedureWidget::onSearchPBClicked()
{
    QPushButton * pButton = dynamic_cast<QPushButton *> (sender());
    if(_SearchButtonEditorMap.contains(pButton))
    {
		QLineEdit * pEditor = _SearchButtonEditorMap.value(pButton);
        emit searchProcedure(pEditor);
    }
}

void GTACheckFWCProcedureWidget::insertProcedure(int index, const QString & iVal)
{
    if(_ProcedureTable.contains(index))
    {
       QLineEdit * pEdit =  _ProcedureTable.value(index);
       if(pEdit != NULL)
       {
           pEdit->setText(iVal);
       }
    }
}

void GTACheckFWCProcedureWidget::setCheckCommand(const GTACheckFwcProcedure * ipCheckCmd)
{
    if(ipCheckCmd != NULL)
    {
        int itemCount = ipCheckCmd->getCount();
        for(int i = 0 ; i < itemCount; i++)
        {
            QString val = ipCheckCmd->getItem(i);
            insertProcedure(i,val);
        }
    }
}

GTACheckFwcProcedure * GTACheckFWCProcedureWidget::getCheckCommand()
{
    GTACheckFwcProcedure * pCheckCmd = new GTACheckFwcProcedure();
    int itemCount = getProcedureCount();

    for(int i = 0; i <itemCount; i++)
    {
        QString val = getProcedure(i);
        pCheckCmd->insertItem(i,val);
    }

    return pCheckCmd;
}
void GTACheckFWCProcedureWidget::clear()
{
    foreach(QLineEdit * pLineEdit, _ProcedureTable.values())
    {
        if(pLineEdit != NULL)
            pLineEdit->clear();
    }
}

bool GTACheckFWCProcedureWidget::isValid()
{
    bool rc = true;
    foreach(QLineEdit * pLineEdit, _ProcedureTable.values())
    {
        if(pLineEdit != NULL && pLineEdit->text().isEmpty())
        {
            rc = false;
            break;
        }
    }
    return rc;
}

bool GTACheckFWCProcedureWidget::getCommand(GTACheckBase *& pCheckCmd)
{
    bool rc = isValid();
    if(rc)
    {
        pCheckCmd = getCheckCommand();
    }
    return rc;
}

bool GTACheckFWCProcedureWidget::setCommand(const GTACheckBase * pCheckCmd)
{
    bool rc = false;
    if(pCheckCmd != NULL && pCheckCmd->getCheckType() == GTACheckBase::FWC_PROCEDURE)
    {
        GTACheckFwcProcedure * pCmd = dynamic_cast<GTACheckFwcProcedure*>((GTACheckBase*)pCheckCmd);
        if(pCmd != NULL)
        {
            setCheckCommand(pCmd);
            rc = true;
        }

    }
    return rc;
}
bool GTACheckFWCProcedureWidget::hasTimeOut()const
{
    return false;
}
bool GTACheckFWCProcedureWidget::hasDumpList()const
{
    return true;
}
bool GTACheckFWCProcedureWidget::hasPrecision()const     
{
    return false;
}
bool GTACheckFWCProcedureWidget::hasSearchItem()const
{
    return true;
}
bool GTACheckFWCProcedureWidget::hasConfigTime() const   
{
    return false;
}
bool GTACheckFWCProcedureWidget::hasActionOnFail()const
{
    return true;
}