#pragma warning (push, 0)
#include "GTAActionFailureWidget.h"
#include "ui_GTAActionFailureWidget.h"
#include "GTAParamValidator.h"
#include "GTAAppController.h"
#include "GTAUtil.h"
#include "GTAAppController.h"
#include "GTAGenSearchWidget.h"
#include "GTAActionFailure.h"


#include <QCompleter>
#include <QMessageBox>
#pragma warning (pop)

GTAActionFailureWidget::GTAActionFailureWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionFailureWidget)
{
    ui->setupUi(this);
    connect(ui->failureTypeCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onFailureSelectionChanged(QString))); 
    connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchParameterClicked()));
    connect(ui->searchFailurePB,SIGNAL(clicked()),this,SLOT(onSearchFailureClicked()));
    connect(ui->setConfigValTB,SIGNAL(clicked()),this,SLOT(showConfigWidget()));
    connect(ui->editPB,SIGNAL(clicked()),this,SLOT(editConfiguration()));
    _pConfigWidget = new GTAActionFailureConfigValWidget(this);
    _pConfigWidget->hide();
    connect(_pConfigWidget,SIGNAL(okPressed()),this,SLOT(setConfigValue()));
    _pCurrentConfigVal=NULL;

    searchType=0;

    ui->searchFailureChB->hide();
    

    connect(ui->addPB,SIGNAL(clicked()),this,SLOT(addToParamList()));
    connect(ui->deletePB,SIGNAL(clicked()),this,SLOT(removeFromParamList()));
    connect(ui->paramValLW,SIGNAL(itemDoubleClicked( QListWidgetItem * )),this,SLOT(onParamValDoubleClicked(QListWidgetItem *)));
    connect(ui->paramValLW,SIGNAL(itemClicked( QListWidgetItem * )),this,SLOT(onParamValClicked(QListWidgetItem *)));

    connect(ui->failureNameLE,SIGNAL(textChanged(const QString & )),this,SLOT(updateFailure(const QString & )));
    //connect(ui->printTableRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
    ////connect(ui->parameterRB,SIGNAL(toggled(bool)),this,SLOT(clearContents(bool)));
    //connect(ui->editPB,SIGNAL(clicked()),this,SLOT(editFromParamList ()));

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

    QStringList failureTypes = QStringList()<<FAILURE_START<< FAILURE_STOP<< FAILURE_PAUSE<<FAILURE_RESUME;
    ui->failureTypeCB->clear();
    ui->failureTypeCB->insertItems(0,failureTypes);
    

}
GTAActionFailureWidget::~GTAActionFailureWidget()
{
    delete ui;
}
QStringList GTAActionFailureWidget::getValues() const
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
void GTAActionFailureWidget::displayWarning(const QString& )
{
   /*  GTAAppController* pCtrl = GTAAppController::getGTAAppController(0);
     if (pCtrl !=NULL)
     {
         QStringList tableNames = pCtrl->getPrintTables();
         if (tableNames.contains(iTableName))
         {
             ui->warningLB->show();
         }
         else
             ui->warningLB->hide();
     }*/
}
void GTAActionFailureWidget::setValue(const QString & )
{
    //ui->parameterLE->setText(iVal);
}

void GTAActionFailureWidget::onSearchParameterClicked()
{
    
    searchType=0;
    emit searchObject(ui->parameterNameLE);
    
}
void GTAActionFailureWidget::onSearchFailureClicked()
{

    searchType=1;
    emit searchObject(ui->failureNameLE);

}
void GTAActionFailureWidget::clear()
{
   /* ui->parameterLE->clear();
    ui->printTableLW->clear();
    ui->tableNameLE->clear();
    ui->titleNameLE->clear();*/
    
}
void GTAActionFailureWidget:: setConfigValue()
{
    GTAFailureConfig* pConfigVal = _pConfigWidget->getConfigValue();
    if (pConfigVal!=NULL)
    {
        ui->parameterValLE->setText(pConfigVal->getName());
        _pCurrentConfigVal = new GTAFailureConfig(*pConfigVal);
    }
}
bool GTAActionFailureWidget::isValid()const
{
    bool rc = true;
    if(ui->failureNameLE->text().isEmpty())
    {
        _LastError = "Enter a valid failure name";
        return false;
    }
    
    int itemsInLst = ui->paramValLW->count();
    QString failureText = ui->failureTypeCB->currentText().toUpper();
    if(itemsInLst==0 && failureText=="START")
    {
        _LastError = "List of failure item table Empty";
        return false;
    }


   return rc;
}
void GTAActionFailureWidget::addToParamList()
{
    
    QString sParameterName = ui->parameterNameLE->text();
    if (sParameterName.isEmpty())
    {
        QMessageBox::critical(0,"Empty value","Parameter cannot be empty",QMessageBox::Ok);
        return;
    }
    if (_pCurrentConfigVal == NULL)
    {
        QMessageBox::critical(0,"Empty value","Parameter value be empty, select appropriate value",QMessageBox::Ok);
        return;
    }
    GTAFailureConfig* pConfig = new GTAFailureConfig(*_pCurrentConfigVal);
    _lstParamConfigVal.append(qMakePair(sParameterName, pConfig));
    ui->paramValLW->addItem(QString("[%1 , Configuration: %2]").arg(sParameterName,pConfig->getName()));
       

    
    
}
void GTAActionFailureWidget::removeFromParamList()
{
    QList<QListWidgetItem*> selectedItems = ui->paramValLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        if (NULL!= pSelectedFirstItem)
        {
            int selectedRow = ui->paramValLW->row(pSelectedFirstItem);
            delete pSelectedFirstItem;

            if (selectedRow>=0 && _lstParamConfigVal.size() < selectedRow)
            {
                QPair<QString,GTAFailureConfig* > nameValPair = _lstParamConfigVal.takeAt(selectedRow);
                if(nameValPair.second!=NULL)
                    delete nameValPair.second;
            }
        }
    }


}
void GTAActionFailureWidget::editFromParamList()
{

    //QList<QListWidgetItem*> selectedItems = ui->printTableLW->selectedItems();
    //if(!selectedItems.isEmpty())
    //{
    //    QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
    //    QString sParameter = ui->parameterLE->text();
    //    sParameter=sParameter.trimmed();
    //    if(!sParameter.isEmpty())
    //    {
    //        pSelectedFirstItem->setText(sParameter);
    //    }

    //}

}
void GTAActionFailureWidget::setActionCommand(const GTAActionBase * ipActionCmd )
{
    

    GTAActionFailure* pFailAct = dynamic_cast <GTAActionFailure*>((GTAActionBase *)ipActionCmd);
    if (pFailAct)
    {
        ui->failureNameLE->setText(pFailAct->getFailureName());
        GTAActionFailure::Failure_Type failType = pFailAct->getFailureType();

        int typeIndex = (int)failType;

        ui->failureTypeCB->setCurrentIndex(typeIndex);

        if (failType == GTAActionFailure::START)
        {
             ui->failureTypeCB->setDisabled(true);
            QList <QPair<QString,GTAFailureConfig* >> lstParamCinfigVal = pFailAct->getParamValConfig();
        
            for (int i=0;i<lstParamCinfigVal.size();i++)
            {
                QPair<QString,GTAFailureConfig* > pairItem = lstParamCinfigVal.at(i);
                if (pairItem.second!=NULL)
                {

                    QString sParameterName = pairItem.first;
                    GTAFailureConfig* pConfig = new GTAFailureConfig(*pairItem.second);
                    _lstParamConfigVal.append( qMakePair(sParameterName,pConfig ));
                    ui->paramValLW->addItem(QString("[%1 , Configuration: %2]").arg(sParameterName,pConfig->getName()));


                }
            }
        }
        else
            ui->failureTypeCB->setDisabled(false);


    }



}
bool GTAActionFailureWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;
    if(!isValid() )
    {
        opCmd=NULL;
        return rc;
    }


    rc = true;
    GTAActionFailure* pAct = new GTAActionFailure;
    QString failureName = ui->failureNameLE->text();
    pAct->setFailureName(failureName);


    QString sFailureType = ui->failureTypeCB->currentText();
    GTAActionFailure::Failure_Type failType= GTAActionFailure::START;
    if (sFailureType.toUpper() == "STOP")
    {
        failType=GTAActionFailure::STOP;
    }
    else if (sFailureType.toUpper() == "PAUSE")
    {
        failType=GTAActionFailure::PAUSE;
    }
    else if (sFailureType.toUpper() == "RESUME")
    {
        failType=GTAActionFailure::RESUME;
    }



    pAct->setFailureType(failType);

  
    if(failType == GTAActionFailure::START)
    {
        pAct->setParamValConfig(_lstParamConfigVal);
    }

    _lstParamConfigVal.clear();
    _pCurrentConfigVal=NULL;
    
    opCmd=pAct;
        
   
    return rc;

}
void GTAActionFailureWidget::updateParamLEFromLst( )
{
    /*QList<QListWidgetItem*> items = ui->printTableLW->selectedItems();
    if(!items.isEmpty())
    {
        QListWidgetItem* pFirstItem = items.at(0);
        if(NULL!=pFirstItem)
            ui->parameterLE->setText(pFirstItem->text());
    }*/

    
}
void GTAActionFailureWidget::clearContents(bool )
{/*
    if(iChecked)
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

 bool GTAActionFailureWidget::updatePrintTableInElement()
 {
     /* if (ui->printTableRB->isChecked()==true)
        return true;
      else*/
          return false;
 }
 void GTAActionFailureWidget::updateTableName(const QString& )
 {
    /* if (ui->printTableRB->isChecked())
     {
         ui->tableNameLE->setText(iText);
     }*/

 }
 int  GTAActionFailureWidget::getSearchType()
 {

    int retVal = GTAGenSearchWidget::PARAMETER;
    if (searchType==1)
    {
        retVal=GTAGenSearchWidget::FAILURE_COMMANDS;
    }
   
    return retVal;

 }
 void GTAActionFailureWidget::onFailureSelectionChanged(QString currentSelection)
 {
     
     if (currentSelection==FAILURE_START)
     {
          ui->stopItemFrame->show();
          //ui->searchFailurePB->setEnabled(false);
          //ui->searchFailureChB->show();
           ui->failureNameLE->setReadOnly(false);
           ui->searchFailureChB->show();
     }
     else
     {
       
         //ui->searchFailureChB->show();
         //ui->searchFailureChB->hide();
         ui->searchFailureChB->hide();
         ui->failureNameLE->setReadOnly(true);
         if (NULL!=_pCurrentConfigVal || ui->paramValLW->count()!=0)
         {
            
             QMessageBox msgBox (this);
             msgBox.setText("All the information will be lost Continue?");
             msgBox.setIcon(QMessageBox::Warning);
             msgBox.setWindowTitle("Add Warning");
             msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
             msgBox.setDefaultButton(QMessageBox::No);
             int ret = msgBox.exec();
             if(ret==QMessageBox::Yes)
             {
                  ui->stopItemFrame->hide();
                  if(_pCurrentConfigVal)
                  {
                      delete _pCurrentConfigVal;
                      _pCurrentConfigVal = NULL;
                  }

             }
             else
             {
                  disconnect(ui->failureTypeCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onFailureSelectionChanged(QString))); 
                  ui->failureTypeCB->setCurrentIndex(1);
                  connect(ui->failureTypeCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onFailureSelectionChanged(QString))); 
             }
         }
         else
            ui->stopItemFrame->hide();
          
     }

 }
 void GTAActionFailureWidget::showConfigWidget()
 {
     _pConfigWidget->show();
     ui->parameterValLE->clear();
     _pConfigWidget->clearContents();
     if (_pCurrentConfigVal !=NULL)
     {
         delete _pCurrentConfigVal;
         _pCurrentConfigVal = NULL;
     }

 }
void GTAActionFailureWidget::editConfiguration()
{

    /*
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        if (NULL!= pSelectedFirstItem)
        {
            int selectedRow = ui->paramValLW->row(pSelectedFirstItem);
            QPair<QString,GTAFailureConfig* > paramConfigValpair = _lstParamConfigVal.at(selectedRow);
            GTAFailureConfig* pConfig = paramConfigValpair.second;
            _pConfigWidget->setConfigValue(pConfig);
            
        }
    }

    _pConfigWidget->show();*/


    QString sParameterName = ui->parameterNameLE->text();
    QList<QListWidgetItem*> selectedItems = ui->paramValLW->selectedItems();

    if(selectedItems.isEmpty())
    {
        QMessageBox::critical(0,"Empty selection","Select an item to edit",QMessageBox::Ok);
        return;
    }

    if (sParameterName.isEmpty())
    {
        QMessageBox::critical(0,"Empty value","Parameter cannot be empty",QMessageBox::Ok);
        return;
    }
    if (_pCurrentConfigVal == NULL)
    {
        QMessageBox::critical(0,"Empty value","Parameter value be empty, select appropriate value",QMessageBox::Ok);
        return;
    }


    else
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        if (NULL!= pSelectedFirstItem)
        {
            int selectedRow = ui->paramValLW->row(pSelectedFirstItem);

            GTAFailureConfig* pConfig = new GTAFailureConfig(*_pCurrentConfigVal);
            QPair<QString,GTAFailureConfig* > paramConfigValpair = _lstParamConfigVal.takeAt(selectedRow);
            pSelectedFirstItem->setText(QString("[%1 , Configuration: %2]").arg(sParameterName,_pCurrentConfigVal->getName()));

            delete paramConfigValpair.second;
            paramConfigValpair.second = pConfig;
            paramConfigValpair.first = sParameterName;

            _lstParamConfigVal.insert(selectedRow,paramConfigValpair);




        }
    }

    

}
void GTAActionFailureWidget::onParamValDoubleClicked(QListWidgetItem * ipItem)
{
    if (ipItem!=NULL)
    {
        int selectedRow = ui->paramValLW->row(ipItem);
        if (selectedRow>=0)
        {
            QPair<QString,GTAFailureConfig* > paramConfigValpair = _lstParamConfigVal.at(selectedRow);
            GTAFailureConfig* pConfig = paramConfigValpair.second;
            
            GTAFailureConfig* pNewConfig = new GTAFailureConfig(*pConfig);
            _pConfigWidget->setConfigValue(pNewConfig);
            _pConfigWidget->show();

            QString sConfigName = pConfig->getName();
            _pConfigWidget->setConfigName(sConfigName);
            
        }
    }

}
void GTAActionFailureWidget::onParamValClicked(QListWidgetItem * ipItem)
{
    if (ipItem!=NULL)
    {
        int selectedRow = ui->paramValLW->row(ipItem);
        if (selectedRow>=0)
        {
            QPair<QString,GTAFailureConfig* > paramConfigValpair = _lstParamConfigVal.at(selectedRow);
            GTAFailureConfig* pConfig = paramConfigValpair.second;

            GTAFailureConfig* pNewConfig = new GTAFailureConfig(*pConfig);
            if(_pCurrentConfigVal)
                delete _pCurrentConfigVal;
            _pCurrentConfigVal = pNewConfig;
            ui->parameterValLE->setText(pNewConfig->getName());
            ui->parameterNameLE->setText(paramConfigValpair.first);
            
            

        }
    }

}
void GTAActionFailureWidget::updateFailure(const QString & iText)
{
    if (ui->searchFailureChB->isChecked())
    {
        GTAAppController* pController = GTAAppController::getGTAAppController();
        if (pController)
        {
            clearAll();
            GTACommandBase* pCmd= pController->getFailureCommand(iText);
            if (pCmd!=NULL)
            {
                GTAActionFailure* pFailure = dynamic_cast<GTAActionFailure*>(pCmd);
                if (pFailure)
                {
                   
                    QList <QPair<QString,GTAFailureConfig* >> lstParamCinfigVal = pFailure->getParamValConfig();

                    for (int i=0;i<lstParamCinfigVal.size();i++)
                    {
                        QPair<QString,GTAFailureConfig* > pairItem = lstParamCinfigVal.at(i);
                        if (pairItem.second!=NULL)
                        {

                            QString sParameterName = pairItem.first;
                            GTAFailureConfig* pConfig = new GTAFailureConfig(*pairItem.second);
                            _lstParamConfigVal.append( qMakePair(sParameterName,pConfig ));
                            ui->paramValLW->addItem(QString("[%1 , Configuration: %2]").arg(sParameterName,pConfig->getName()));


                        }
                    }
                }
            }
        }
    }
}
void GTAActionFailureWidget::clearAll()
{
    for (int i=0;i<_lstParamConfigVal.size();i++)
    {
        QPair<QString,GTAFailureConfig* > pairItem = _lstParamConfigVal.at(i);
         GTAFailureConfig* pConfig = pairItem.second;
        if (pConfig!=NULL)
        {
            QString sParameterName = pairItem.first;
            if(pConfig)
                delete pConfig;
        }
    }
    _lstParamConfigVal.clear();
     ui->paramValLW->clear();
}
#include "GTAWgtRegisterDef.h"
 REGISTER_ACTION_WIDGET(ACT_FAILURE,GTAActionFailureWidget,obj)
