#pragma warning (push, 0)
#include "GTAActionFailureConfigValWidget.h"
#include "GTAParamValidator.h"
#include "GTAAppController.h"
#include "GTAUtil.h"
#include "GTAAppController.h"
#include "GTAGenSearchWidget.h"


#include "ui_GTAActionFailureConfigValWidget.h"
#include <QCompleter>
#include <QMessageBox>
#pragma warning (pop)

GTAActionFailureConfigValWidget::GTAActionFailureConfigValWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTAActionFailureConfigValWidget)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog);
    _profileSelectorWidget = new GTAProfileSelectorWidget(this);


    //connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
   
    //connect(ui->printTableRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
    ////connect(ui->parameterRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
    
    
    //connect(ui->printTableLW,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamLEFromLst()));
    //connect(ui->parameterLE,SIGNAL(textChanged(const QString& )),this,SLOT(updateTableName(const QString&)));
    //connect(ui->tableNameLE,SIGNAL(textChanged(const QString& )),this,SLOT(displayWarning(const QString&)));
    ////GTAParamValidator* pValidator1 = new GTAParamValidator(ui->parameterLE);
    ////ui->parameterLE->setValidator(pValidator1);
    //_bUpdatePrintCommand= false;
    //ui->warningLB->hide();

    //GTAAppController* pController = GTAAppController::getGTAAppController();
    //if (NULL!=pController)
    //{
    //    QStringList wordList = pController->getVariablesForAutoComplete();
    //    QCompleter *completer = new QCompleter(wordList, this);
    //    completer->setCaseSensitivity(Qt::CaseInsensitive);
    //    ui->parameterLE->setCompleter(completer);
    //}

    connect(ui->signalProfilePB,SIGNAL(clicked()),this,SLOT(showProfileSelectorWidget()));
    connect(_profileSelectorWidget,SIGNAL(okPressed()),this,SLOT(setCurrentProfile()));
    connect(ui->deletePB,SIGNAL(clicked()),this,SLOT(removeFromParamList()));
    connect(ui->editPB,SIGNAL(clicked()),this,SLOT(editFromParamList ()));
    connect(ui->addPB,SIGNAL(clicked()),this,SLOT(addToParamList()));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOkPBClicked()));
    connect(ui->CancePB,SIGNAL(clicked()),this,SLOT(onCancelPBClicked())); 
    connect(ui->failureLW,SIGNAL(itemClicked( QListWidgetItem * )),this,SLOT(onConfigItemClicked(QListWidgetItem *)));

    ui->vmacTypeCB->addItems(QStringList()<<SET_VMACFORCE_SOL<<SET_VMACFORCE_LIQ);
    
    _pCurrentProfile=NULL;
    _pConfigVal=NULL;

}


GTAActionFailureConfigValWidget::~GTAActionFailureConfigValWidget()
{
    delete ui;

    if (_pCurrentProfile!=NULL)
    {
        delete _pCurrentProfile;
        _pCurrentProfile=NULL;
    }
    if (_pConfigVal!=NULL)
    {
        delete _pConfigVal;
        _pConfigVal=NULL;
    }
}
QStringList GTAActionFailureConfigValWidget::getValues() const
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
void GTAActionFailureConfigValWidget::setCurrentProfile()
{


    GTAFailureProfileBase* pProfile = NULL;
    bool rc  = _profileSelectorWidget->getFailureProfile(pProfile);
    if (rc && pProfile!=NULL)
    {
        QString statement = pProfile->getStatement();

        if (NULL!=pProfile)
        {
            if (_pCurrentProfile!=NULL)
                delete _pCurrentProfile;
            _pCurrentProfile=pProfile;

            ui->configValLE->setText(pProfile->getStatement());
        }

    }
    
    
}
void GTAActionFailureConfigValWidget::displayWarning(const QString& )
{
//      GTAAppController* pCtrl = GTAAppController::getGTAAppController(0);
//      if (pCtrl !=NULL)
//      {
//          QStringList tableNames = pCtrl->getPrintTables();
//          if (tableNames.contains(iTableName))
//          {
//              ui->warningLB->show();
//          }
//          else
//              ui->warningLB->hide();
//      }
}
void GTAActionFailureConfigValWidget::setValue(const QString & )
{
    //ui->parameterLE->setText(iVal);
}

void GTAActionFailureConfigValWidget::onSearchPBClicked()
{
    
        //emit searchObject(ui->configValLE);
    
}

void GTAActionFailureConfigValWidget::clear()
{
//     ui->parameterLE->clear();
//     ui->printTableLW->clear();
//     ui->tableNameLE->clear();
//     ui->titleNameLE->clear();
    
}
bool GTAActionFailureConfigValWidget::isValid()const
{
    bool rc = true;
//     if(ui->printTableLW->count()==0)
//     {
//         _LastError = "List of print table items Empty";
//         return false;
//     }
// 
// 
//     rc = rc & !ui->tableNameLE->text().isEmpty();
//     rc = rc & !ui->titleNameLE->text().isEmpty();

    return rc;
}
void GTAActionFailureConfigValWidget::addToParamList()
{
    bool rc=false;
    GTAFailureProfileBase* profileToAppend = NULL;
    if(NULL!=_pCurrentProfile)
    {
        profileToAppend = _pCurrentProfile->getClone();

    }
    else
    {
        if (ui->configValLE->text().isEmpty())
        {
            QMessageBox::information(this,"Error","Select equation");
            return;
        }
    }

    QString sDuration = ui->durationLE->text();
    if(sDuration.isEmpty())
    {
        QMessageBox::information(this,"Error","Duration is empty, enter valid value");
        return;
    }
    sDuration.toDouble(&rc);
    if (!rc)
    {
        QMessageBox::information(this,"Error","Duration is invalid, enter valid value");
        return;
    }


    if(NULL==_pConfigVal)
        _pConfigVal = new GTAFailureConfig;


    QString sVmacFailure = ui->vmacTypeCB->currentText();
    double duration = sDuration.toDouble();
    _pConfigVal->appendFailure(profileToAppend,duration,sVmacFailure);
    ui->failureLW->addItem(QString("[%1,Duration:%2,VMAC:%3]").arg(profileToAppend->getStatement(),sDuration,sVmacFailure));

    

    
    
       

    
}
void GTAActionFailureConfigValWidget::removeFromParamList()
{

     QList<QListWidgetItem*> selectedItems = ui->failureLW->selectedItems();
     if(!selectedItems.isEmpty())
     {
         QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
         if (NULL!= pSelectedFirstItem)
         {
             int selectedRow = ui->failureLW->row(pSelectedFirstItem);
             delete pSelectedFirstItem;

             if (selectedRow>=0)
             {
                 _pConfigVal->remove(selectedRow);
             }
         }
     }

    
}
void GTAActionFailureConfigValWidget::editFromParamList()
{


    bool rc=false;
    GTAFailureProfileBase* profileToAppend = NULL;
    if(NULL!=_pCurrentProfile)
    {
        profileToAppend = _pCurrentProfile->getClone();

    }
    else
    {
        if (ui->configValLE->text().isEmpty())
        {
            QMessageBox::information(this,"Error","Select equation");
            return;
        }
    }

    QString sDuration = ui->durationLE->text();
    if(sDuration.isEmpty())
    {
        QMessageBox::information(this,"Error","Duration is empty, enter valid value");
        return;
    }
    int dDuration = sDuration.toInt(&rc);
    if (!rc)
    {
        QMessageBox::information(this,"Error","Duration is invalid, enter valid value");
        return;
    }


    QString sVmacFailure = ui->vmacTypeCB->currentText();



    QList<QListWidgetItem*> selectedItems = ui->failureLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        if (NULL!= pSelectedFirstItem)
        {
            int selectedRow = ui->failureLW->row(pSelectedFirstItem);
           

            if (selectedRow>=0)
            {
                rc = _pConfigVal->editFailureProfile(selectedRow,profileToAppend,dDuration,sVmacFailure);
                if(rc)
                    pSelectedFirstItem->setText(QString("[%1,Duration:%2,VMAC:%3]").arg(profileToAppend->getStatement(),sDuration,sVmacFailure));
            }
        }
    }

}
void GTAActionFailureConfigValWidget::updateParamLEFromLst( )
{
//     QList<QListWidgetItem*> items = ui->printTableLW->selectedItems();
//     if(!items.isEmpty())
//     {
//         QListWidgetItem* pFirstItem = items.at(0);
//         if(NULL!=pFirstItem)
//             ui->parameterLE->setText(pFirstItem->text());
//     }

    
}
void GTAActionFailureConfigValWidget::clearContents()
{
    if(_pConfigVal)
        delete _pConfigVal;
    if(_pCurrentProfile)
        delete _pCurrentProfile;

    ui->durationLE->clear();
    ui->configValLE->clear();
    ui->configNameLE->clear();
    ui->failureLW->clear();
    _pConfigVal=NULL;
    _pCurrentProfile=NULL;
   
}

 bool GTAActionFailureConfigValWidget::updatePrintTableInElement()
  {
//       if (ui->printTableRB->isChecked()==true)
//         return true;
//       else
          return false;
 }
 void GTAActionFailureConfigValWidget::updateTableName(const QString& )
  {
//      if (ui->printTableRB->isChecked())
//      {
//          ui->tableNameLE->setText(iText);
//      }

 }
 int  GTAActionFailureConfigValWidget::getSearchType()
 {
     int retVal = -1;
//      if (updatePrintTableInElement())
//      {
//          retVal = GTAGenSearchWidget::PRINT_TABLE;
//      }
//      else
//          retVal = GTAGenSearchWidget::PARAMETER;

     return retVal;
 }
void GTAActionFailureConfigValWidget::showProfileSelectorWidget()
{
    _profileSelectorWidget->show();
    _profileSelectorWidget->initialize();
   
}
void GTAActionFailureConfigValWidget::onOkPBClicked()
{
    QString configName = ui->configNameLE->text();
   
    if (configName.isEmpty() )
    {
        QMessageBox::information(this,"Error","Enter valid Name");
        return;
        
    }
    else if (ui->failureLW->count()==0)
    {
        QMessageBox::information(this,"Error","List of profile is empty");
        return;
    }
    else
    {
        _pConfigVal->setName(configName);
        hide();
        emit okPressed();
    }

}
void GTAActionFailureConfigValWidget::onCancelPBClicked()

{

    //QMessageBox msgBox (this);
    //msgBox.setText("This would empty the current items and reset values?");
    //msgBox.setIcon(QMessageBox::Warning);
    //msgBox.setWindowTitle("Add Warning");
    //msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    //msgBox.setDefaultButton(QMessageBox::No);
    //int ret = msgBox.exec();
    //if(ret==QMessageBox::Yes)
    //{
    //    clearContents();
    //    hide();
    //}
     hide();
}
void GTAActionFailureConfigValWidget::setConfigValue(GTAFailureConfig* ipConfig)
{
    if (ipConfig)
    {
       clearContents();
       setConfigName(ipConfig->getConfigurationName());
       int noOfProfiles = ipConfig->noOfProfiles();
       QString sConfigName = ipConfig->getName();
       _pConfigVal=new GTAFailureConfig;
       
       for (int i=0;i<noOfProfiles;i++)
       {
           double dDuration;
           GTAFailureProfileBase* pFailureProfile = NULL;
           QString sVmacFailure;
           ipConfig->getFailureProfile(i , pFailureProfile , dDuration,sVmacFailure);           
           _pConfigVal->setName(sConfigName);
           _pConfigVal->appendFailure(pFailureProfile,dDuration,sVmacFailure);
           ui->failureLW->addItem(QString("[%1,Duration:%2,VMAC:%3]").arg(pFailureProfile->getStatement(),QString::number(dDuration),sVmacFailure));



       }

       //fordebug
       _pConfigVal->noOfProfiles();
    }

}
void GTAActionFailureConfigValWidget::setConfigName(const QString& iName)
{
    ui->configNameLE->setText(iName);
}
void GTAActionFailureConfigValWidget::onConfigItemClicked(QListWidgetItem * ipItem)
{
    if (ipItem!=NULL)
    {
        int selectedRow = ui->failureLW->row(ipItem);
        if (selectedRow>=0)
        {
            double dDuration;
            QString sVmacForcingType;
            GTAFailureProfileBase* pFailureProfile = NULL;
            _pConfigVal->getFailureProfile(selectedRow , pFailureProfile , dDuration,sVmacForcingType);
            ui->durationLE->setText(QString::number(dDuration));

            int index = ui->vmacTypeCB->findText(sVmacForcingType);
            if (index>=0)
                ui->vmacTypeCB->setCurrentIndex(index);

            if(pFailureProfile)
            {
                if(_pCurrentProfile)
                    delete _pCurrentProfile;
                _pCurrentProfile = pFailureProfile->getClone();
               ui->configValLE->setText(pFailureProfile->getStatement());

                
            }
          
        }
    }

}
//void GTAActionFailureConfigValWidget::setActionCommand(const GTAActionBase * ipActionCmd )
//{
////     GTAActionPrintTable* pPrintActionCmd = dynamic_cast<GTAActionPrintTable*> ((GTAActionBase *)ipActionCmd);
////     ui->parameterLE->clear();
////     if (NULL!=pPrintActionCmd)
////     {
////         QStringList lstSParameters = pPrintActionCmd->getValues();
////         ui->printTableLW->clear();
////         foreach(QString sParameter,lstSParamet~ers)
////         {
////             ui->printTableLW->addItem(sParameter);
////         }
////         ui->tableNameLE->setText(pPrintActionCmd->getTableName());
////         ui->titleNameLE->setText(pPrintActionCmd->getTitleName());
////     }
////     else
////         clear();
//
//
//    
//}
//bool GTAActionFailureConfigValWidget::getActionCommand(GTAActionBase*& opCmd)const
//{
//    /*if (ui->printTableRB->isChecked() == true)
//    {
//    _bUpdatePrintCommand = true;
//    }*/
//    bool rc = false;
////     if(!isValid())
////         return rc;
//// 
////     rc = true;
////     GTAActionPrintTable* pAct = new GTAActionPrintTable;
////     QString tableName = ui->tableNameLE->text();
////     pAct->setTableName(tableName);
//// 
////     QStringList sParameterList;
////     int items = ui->printTableLW->count();
////     
////         
////     
////     for(int i=0;i<items;i++)
////     {
////         QListWidgetItem* pWidgetItem = ui->printTableLW->item(i);
////         if (NULL!=pWidgetItem)
////         {
////             QString sParameter = pWidgetItem->text();
////             if (!sParameter.isEmpty())
////                 sParameterList.append(sParameter);
////         }
//// 
////     }
////     pAct->setValues(sParameterList);
////     pAct->setTitleName(ui->titleNameLE->text());
//// 
////     GTAAppController* pCtrl = GTAAppController::getGTAAppController(0);
////     pCtrl->editPrintTables(tableName,  sParameterList);
//// 
//// 
////     opCmd= pAct;
//
//    return rc;
//
//}