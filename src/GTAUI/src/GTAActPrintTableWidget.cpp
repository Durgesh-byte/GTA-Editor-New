#include "GTAActPrintTableWidget.h"
#include "ui_GTAActPrintTableWidget.h"
#include "GTAParamValidator.h"
#include "GTAAppController.h"
#include "GTAUtil.h"
#include "GTAAppController.h"
#include "GTAGenSearchWidget.h"
#include <QCompleter>
#include <QMessageBox>

GTAActPrintTableWidget::GTAActPrintTableWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActPrintTableWidget)
{
    ui->setupUi(this);
    searchType = 0;
    connect(ui->searchParamPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->addPB,SIGNAL(clicked()),this,SLOT(addToParamList()));
//    connect(ui->printTableRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
    //connect(ui->parameterRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
    connect(ui->editPB,SIGNAL(clicked()),this,SLOT(editFromParamList ()));
    connect(ui->deletePB,SIGNAL(clicked()),this,SLOT(removeFromParamList()));
    connect(ui->printTableLW,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamLEFromLst()));
    //connect(ui->parameterLE,SIGNAL(textChanged(const QString& )),this,SLOT(updateTableName(const QString&)));
    connect(ui->tableNameLE,SIGNAL(textChanged(const QString& )),this,SLOT(onTableNameEdited(const QString&)));    
    connect(ui->searchTableCB,SIGNAL(toggled(bool)),this,SLOT(onSearchCBToggled(bool)));
    connect(ui->searchTablePB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));

    //GTAParamValidator* pValidator1 = new GTAParamValidator(ui->parameterLE);
    //ui->parameterLE->setValidator(pValidator1);
    _bUpdatePrintCommand= false;
    ui->warningLB->hide();

    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
        QStringList wordList = pController->getVariablesForAutoComplete();
        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->parameterLE->setCompleter(completer);
    }

}

GTAActPrintTableWidget::~GTAActPrintTableWidget()
{
    delete ui;
}
QStringList GTAActPrintTableWidget::getValues() const
{
    QStringList sParameterList;
    /*int items = ui->printTableLW->count();
    for(int i=0;i<items;i++)
    {
        QListWidgetItem* pWidgetItem = ui->printTableLW->item(i);
        if (NULL!=pWidgetItem)
        {
            QString sParameter = pWidgetItem->text();
            if (!sParameter.isEmpty())
                sParameterList.append(sParameter);
        }
        
    }*/
    return sParameterList;
}
void GTAActPrintTableWidget::onTableNameEdited(const QString& iTableName)
{
     GTAAppController* pCtrl = GTAAppController::getGTAAppController(0);
     if (pCtrl !=NULL)
     {
         if(ui->searchTableCB->isChecked())
         {
            ui->warningLB->hide();
            

            QStringList tableItems =  pCtrl->getPrintTableItem(iTableName);
            ui->printTableLW->clear();
            ui->printTableLW->addItems(tableItems);

         }
         else
         {
             
             QStringList tableNames = pCtrl->getPrintTables();
             if (tableNames.contains(iTableName))
             {
                 ui->warningLB->show();
             }
             else
                 ui->warningLB->hide();

         }
        
     }
}
 void  GTAActPrintTableWidget::onSearchCBToggled(bool isChecked)
 {
    if (isChecked)
    {
        ui->addPB->setDisabled(true);
        ui->editPB->setDisabled(true);
        ui->deletePB->setDisabled(true);
        ui->tableNameLE->setReadOnly(true);
        QString tableName = ui->tableNameLE->text();
        GTAAppController* pCtrl = GTAAppController::getGTAAppController(0);
        if (pCtrl !=NULL)
        {
            if(ui->searchTableCB->isChecked())
            {
                ui->warningLB->hide();


                QStringList tableItems =  pCtrl->getPrintTableItem(tableName);
                ui->printTableLW->clear();
                ui->printTableLW->addItems(tableItems);

            }
        }
    }
    else
    {

        ui->addPB->setDisabled(false);
        ui->editPB->setDisabled(false);
        ui->deletePB->setDisabled(false);
        ui->tableNameLE->setReadOnly(false);
        onTableNameEdited(ui->tableNameLE->text());
    }
 }
void GTAActPrintTableWidget::setValue(const QString & )
{
    //ui->parameterLE->setText(iVal);
}

void GTAActPrintTableWidget::onSearchPBClicked()
{
    QObject* pSender = sender();

    if (pSender == ui->searchParamPB)
    {
        searchType=0;
        emit searchObject(ui->parameterLE);
    }
    if (pSender == ui->searchTablePB)
    {
        searchType=1;
        emit searchObject(ui->tableNameLE);
    }
        
    
}

void GTAActPrintTableWidget::clear()
{
    ui->parameterLE->clear();
    ui->printTableLW->clear();
    ui->tableNameLE->clear();
    ui->titleNameLE->clear();
    
}
bool GTAActPrintTableWidget::isValid()const
{
    bool rc = true;
    if(ui->printTableLW->count()==0)
    {
        _LastError = "List of print table items Empty";
        return false;
    }


    rc = rc & !ui->tableNameLE->text().isEmpty();
    rc = rc & !ui->titleNameLE->text().isEmpty();

    return rc;
}
void GTAActPrintTableWidget::addToParamList()
{
    //if (ui->parameterRB->isChecked())
    {
        QString sParameter = ui->parameterLE->text();
        QStringList parameters = GTAUtil::getProcessedParameterForChannel(sParameter,true);
        foreach(QString sParam, parameters)
        {
            sParam=sParam.trimmed();
            if(!sParam.isEmpty())
            {
                ui->printTableLW->addItem(sParam);
            }
        }
    }
   /* else
    {
       
        QMessageBox msgBox (this);
        msgBox.setText(QString("This would empty the current items in list and add contents of table:%1 continue?").arg(ui->parameterLE->text()));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Add Warning");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret==QMessageBox::Yes)
        {
            GTAAppController* pCtrl = GTAAppController::getGTAAppController(0);
            if (NULL!=pCtrl)
            {
                ui->printTableLW->clear();
                QStringList lstOfParams = pCtrl->getPrintTableParameters(ui->parameterLE->text());

                foreach(QString sParam, lstOfParams)
                {
                    ui->printTableLW->addItem(sParam);
                }
            }
           
        }
    }*/
    
}
void GTAActPrintTableWidget::removeFromParamList()
{
   /* if (ui->parameterRB->isChecked()==true)
    {*/
        QList<QListWidgetItem*> selectedItems = ui->printTableLW->selectedItems();
        if(!selectedItems.isEmpty())
        {
            QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
            if (NULL!= pSelectedFirstItem)

                delete pSelectedFirstItem;
        }
    //}
    

}
void GTAActPrintTableWidget::editFromParamList()
{

    QList<QListWidgetItem*> selectedItems = ui->printTableLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        QString sParameter = ui->parameterLE->text();
        sParameter=sParameter.trimmed();
        if(!sParameter.isEmpty())
        {
            pSelectedFirstItem->setText(sParameter);
        }

    }

}
void GTAActPrintTableWidget::setActionCommand(const GTAActionBase * ipActionCmd )
{
    GTAActionPrintTable* pPrintActionCmd = dynamic_cast<GTAActionPrintTable*> ((GTAActionBase *)ipActionCmd);
    ui->parameterLE->clear();
    if (NULL!=pPrintActionCmd)
    {
        QStringList lstSParameters = pPrintActionCmd->getValues();
        ui->printTableLW->clear();
        foreach(QString sParameter,lstSParameters)
        {
            ui->printTableLW->addItem(sParameter);
        }
        ui->tableNameLE->setText(pPrintActionCmd->getTableName());
        ui->titleNameLE->setText(pPrintActionCmd->getTitleName());
    }
    else
        clear();


    
}
bool GTAActPrintTableWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    /*if (ui->printTableRB->isChecked() == true)
    {
    _bUpdatePrintCommand = true;
    }*/
    bool rc = false;
    if(!isValid())
        return rc;

    rc = true;
    GTAActionPrintTable* pAct = new GTAActionPrintTable;
    QString tableName = ui->tableNameLE->text();
    pAct->setTableName(tableName);

    QStringList sParameterList;
    int items = ui->printTableLW->count();
    
        
    
    for(int i=0;i<items;i++)
    {
        QListWidgetItem* pWidgetItem = ui->printTableLW->item(i);
        if (NULL!=pWidgetItem)
        {
            QString sParameter = pWidgetItem->text();
            if (!sParameter.isEmpty())
                sParameterList.append(sParameter);
        }

    }
    pAct->setValues(sParameterList);
    pAct->setTitleName(ui->titleNameLE->text());

    GTAAppController* pCtrl = GTAAppController::getGTAAppController(0);
    pCtrl->editPrintTables(tableName,  sParameterList);


    opCmd= pAct;

    return rc;

}
void GTAActPrintTableWidget::updateParamLEFromLst( )
{
    QList<QListWidgetItem*> items = ui->printTableLW->selectedItems();
    if(!items.isEmpty())
    {
        QListWidgetItem* pFirstItem = items.at(0);
        if(NULL!=pFirstItem)
            ui->parameterLE->setText(pFirstItem->text());
    }

    
}
void GTAActPrintTableWidget::clearContents(bool )
{
   /* if(iChecked)
    {
        if ( ui->printTableLW->count()==0)
        {
            return;
        }
        QObject* pSender = sender();

        QMessageBox msgBox (this);
        msgBox.setText("This would empty the current items in list continue?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Add Warning");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret==QMessageBox::Yes)
        {
            ui->printTableLW->clear();
        }
        else
        {
            if (pSender==ui->printTableRB)
            {
                disconnect(ui->parameterRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
                ui->parameterRB->setChecked(true);
                connect(ui->parameterRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
               
            }

            if (pSender==ui->parameterRB)
            {
                disconnect(ui->printTableRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
                ui->printTableRB->setChecked(true);
                connect(ui->printTableRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
               
            }

           
        }
       
    }*/
   
}

 bool GTAActPrintTableWidget::updatePrintTableInElement()
 {
     /* if (ui->printTableRB->isChecked()==true)
        return true;
      else*/
          return false;
 }
 void GTAActPrintTableWidget::updateTableName(const QString& )
 {
     /*if (ui->printTableRB->isChecked())
     {
         ui->tableNameLE->setText(iText);
     }*/

 }
 int  GTAActPrintTableWidget::getSearchType()
 {
     int retVal = -1;
     if (searchType)
     {
         retVal = GTAGenSearchWidget::PRINT_TABLE;
     }
     else
         retVal = GTAGenSearchWidget::PARAMETER;

     return retVal;
 }

 bool GTAActPrintTableWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
 {
     for (auto param : params) {
         if (param.first == "tableNameLE")
         {
             ui->tableNameLE->setText(param.second);
         }
         else if (param.first == "titleNameLE")
         {
             ui->titleNameLE->setText(param.second);
         }
         else if (param.first == "parameterLE")
         {
             ui->parameterLE->setText(param.second);
             addToParamList();
         }
         else
         {
             errorLogs.append(QString("Invalid print table first value: %1").arg(param.first));
             return false;
         }
     }
     return true;
 }

#include "GTAWgtRegisterDef.h"
 //REGISTER_ACTION_WIDGET(ACT_PRINT_TABLE,GTAActPrintTableWidget,obj)
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_TABLE),GTAActPrintTableWidget,obj)
