#include "GTAActionOneClickSetWidget.h"
#include "ui_GTAActionOneClickSetWidget.h"
#include "GTAEquationWidget.h"
#include "GTAActionOneClickSet.h"
#include "GTAEquationConst.h"
#include "GTAUtil.h"
#include "GTAParamValidator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include "GTAAppController.h"
#include "GTAICDParameter.h"
#include "GTAGenSearchWidget.h"
GTAActionOneClickSetWidget::GTAActionOneClickSetWidget(QWidget *parent) :
        GTAActionWidgetInterface(parent)/*,_pSetActionCmd(NULL)*/,_pEquationWgt(NULL),
        ui(new Ui::GTAActionOneClickSetWidget)
{
    //    _pSetCmd = NULL;
    ui->setupUi(this);
    _pEquationWgt = new GTAEquationWidget();
    _pEquationWgt->hide();

    //    _pSetActionCmd = new GTAActionOneClickSet();

    QStringList vmacForceTypeList;
    vmacForceTypeList<<SET_VMACFORCE_SOL<<SET_VMACFORCE_LIQ;
    ui->VmacForcingTypeCB->addItems(vmacForceTypeList);

    _sFSList =  QStringList() << ACT_FSSSM_NO<<ACT_FSSSM_ND<<ACT_FSSSM_FW<<ACT_FSSSM_NCD<<ACT_FSSSM_FT<<ACT_FSSSM_MINUS<<ACT_FSSSM_PLUS<<ACT_FSSSM_LOST<<ACT_FSSSM_NOCHANGE;
    ui->functionStatusCB->addItems(_sFSList);
    
    connect(ui->EquationPB,SIGNAL(clicked()),this,SLOT(onEquationButtonClick()));
    connect(_pEquationWgt,SIGNAL(okPressed()),this,SLOT(onEquationWgtOKButtonClick()));
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->SearchPB2,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->ValueLE,SIGNAL(textChanged(QString)),this,SLOT(onParameterValueEdited(QString)));
    connect(ui->addToListPB,SIGNAL(clicked()),this,SLOT(onAddSetCommand()));
    connect(ui->removeFromListPB,SIGNAL(clicked()),this,SLOT(onRemoveSetCommand()));
    connect(ui->setActionListLV,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamValueLineEdits()));
    connect(ui->editSelectedPB,SIGNAL(clicked()),this,SLOT(editSetCommand()));
    connect(ui->VmacForcingTypeCB,SIGNAL(currentIndexChanged(int)),SLOT(updateVmacForceType(int)));
    connect(ui->importFromCSVPB,SIGNAL(clicked()),this,SLOT(importParametersFromCSV()));
    connect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
    //connect(ui->ParamNameLE,SIGNAL(textChanged( const QString &)),this,SLOT(disableFSCB( const QString &)));
    
    
    //connect(ui->CancelPB,SIGNAL(clicked()),SLOT(clear()));
    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->ParamNameLE);
    ui->ParamNameLE->setValidator(pValidator1);
    GTAParamValidator* pValidator2 = new GTAParamValidator(ui->ValueLE);

    ui->ValueLE->setValidator(pValidator2);
    ui->noteLB->hide();
}



GTAActionOneClickSetWidget::~GTAActionOneClickSetWidget()
{
    //     if(_pSetCmd != NULL)
    //     {
    //         delete _pSetCmd;
    //         _pSetCmd = NULL;
    //     }
    
    if(_pEquationWgt != NULL)
    {
        delete _pEquationWgt;
        _pEquationWgt = NULL;
    }
    delete ui;
}
void GTAActionOneClickSetWidget::onEquationButtonClick()
{

    if (!isParameterFromDB())
    {
        if (this->isVisible())
            QMessageBox::information(this,"Invalid input parameter","Equation are valid for ICD/PMR/PIR parameters only");
        return;
    }
    if(_pEquationWgt != NULL)
    {

        QList<QListWidgetItem *> lstOfSelecteItems =  ui->setActionListLV->selectedItems();
        // QModelIndexList selectedIndex = ui->setActionListLV->selectedIndexes();
        //Only one selection is allowed, take the top selection
        if(!lstOfSelecteItems.isEmpty())
        {
            QListWidgetItem* pItem = lstOfSelecteItems[0];

            //selectedIndexes () const : QModelIndexLis
            if(NULL!=pItem)
            {
                //selected indexed are not directly available for qlistwidget.
                int totoalItems = ui->setActionListLV->count();

                for (int i=0;i<totoalItems;i++)
                {
                    QListWidgetItem* pItemTocompare = ui->setActionListLV->item(i);
                    if (pItem == pItemTocompare)
                    {
                        if (_SetCommandList.size()>i)
                        {
                            GTAActionOneClickSet* pActionSet = _SetCommandList.at(i);
                            if (NULL!=pActionSet)
                            {
                                GTAEquationBase* pEqn = pActionSet->getEquation();
                                if (NULL!=pEqn)
                                {
                                    _pEquationWgt->setEquation(pEqn);
                                }
                            }



                        }
                    }
                }


            }
        }

        _pEquationWgt->show();
    }
}
void GTAActionOneClickSetWidget::onEquationWgtOKButtonClick()
{
    ui->ValueLE->setReadOnly(false);
    if(_pEquationWgt != NULL)
    {
        _pEquationWgt->hide();
        GTAEquationBase * pEquation = NULL;
        _pEquationWgt->getEquation(pEquation);
        if(pEquation != NULL)
        {
           silentSetValueText(pEquation->getStatement());
            //_pEquation = pEquation;
            if(pEquation->getEquationType() != GTAEquationBase::CONST )
            {
                ui->ValueLE->setReadOnly(true);
            }
        }
    }
}

void GTAActionOneClickSetWidget::onSearchPBClicked()
{
    if (sender()==ui->SearchPB)
        emit searchObject(ui->ParamNameLE);
    else if (sender() == ui->SearchPB2)
    {
        QLineEdit* pValLE = ui->ValueLE;
        GTAEquationConst objConst;
        _pEquationWgt->setEquation(&objConst);
        emit searchObject(pValLE);
    }
}

 bool GTAActionOneClickSetWidget::isValid() const 
 {
        return !_SetCommandList.isEmpty();
 }
void GTAActionOneClickSetWidget::setActionCommand(const GTAActionBase* ipActionCmd)
{
    
    clear();
    _SetCommandList.clear();

    ui->ValueLE->setReadOnly(false);
    GTAActionOneClickSetList* pActSetLstCmd= dynamic_cast<GTAActionOneClickSetList*>((GTAActionBase*)ipActionCmd);
    if(pActSetLstCmd != NULL)
    {
        QList<GTAActionOneClickSet*> listOfSetActions;
        pActSetLstCmd->getSetCommandlist(listOfSetActions);
        QString vmacVal = pActSetLstCmd->getVmacForceType();
        foreach(GTAActionOneClickSet* pbase , listOfSetActions)
            _SetCommandList.append(dynamic_cast<GTAActionOneClickSet*> (pbase->getClone()));


        
        QStringList exprList;
        for(int i=0;i<_SetCommandList.size();i++)
        {
            GTAActionOneClickSet* pSetCmd = _SetCommandList.at(i);
            if (NULL!=pSetCmd)
            {
                QString sParameter = pSetCmd->getParameter();
                QString sValue="";
                GTAEquationBase* pEqn = pSetCmd->getEquation();
                QString sFunctionStatus= pSetCmd->getFunctionStatus();
                if (NULL!=pEqn)
                {
                    sValue=pEqn->getStatement();
                }
                if (sValue.isEmpty() && !pSetCmd->getFunctionStatus().isEmpty())
                {
                     exprList.append(QString("%1(FS:%2) [Set Functional Status]").arg(sParameter,sFunctionStatus));
                }
                else
                    exprList.append(QString("%1(FS:%2) = %3 ").arg(sParameter,sFunctionStatus,sValue));
            }
        }
        ui->setActionListLV->addItems(exprList);
        int index = ui->VmacForcingTypeCB->findText(vmacVal);
        ui->VmacForcingTypeCB->setCurrentIndex(index);
        
        
    }
    //    _pSetCmd = pOneClickSetCmd;

}

bool GTAActionOneClickSetWidget::getActionCommand(GTAActionBase*& opCmd)const
{

    bool rc = false;
    GTAActionOneClickSetList * pSetCommand = NULL;
    int nbOfItems = ui->setActionListLV->count();
    if (nbOfItems > 0)
    {
        if (!_SetCommandList.isEmpty())
        {
            pSetCommand = new GTAActionOneClickSetList(QString(ACT_SET),"");
            pSetCommand->setSetCommandlist(_SetCommandList);
            pSetCommand->setVmacForceType(getVmacForceType());
            rc=true;
        }
    }
    opCmd=pSetCommand;
    return rc;;
}
void GTAActionOneClickSetWidget::onParameterValueEdited(const QString &iValue)
{
    GTAEquationConst * pConstEq = NULL;

    GTAEquationBase * pEq = NULL;
    _pEquationWgt->getEquation(pEq);
    if(pEq != NULL && pEq->getEquationType() == GTAEquationBase::CONST)
    {
        pConstEq = dynamic_cast<GTAEquationConst*>(pEq);
    }
    else
    {
        if (ui->ValueLE->isReadOnly()==false)//if its readonly then the LE has non constant eqn value.
        {
            delete pEq;
            pEq = NULL;
            pConstEq = new GTAEquationConst();
        }
      
    }

    if(pConstEq != NULL)
    {
        pConstEq->setConstant(iValue);
        _pEquationWgt->setEquation(pConstEq);
    }

}
void GTAActionOneClickSetWidget::clear()
{
    ui->ParamNameLE->clear();
    ui->ValueLE->clear();
    _pEquationWgt->clear();
    _pEquationWgt->setEquation(NULL);
    ui->setActionListLV->clear();
    ui->functionStatusCB->setCurrentIndex(0);
    ui->SetOnlyFSCB->setCheckState(Qt::Unchecked);
}
void GTAActionOneClickSetWidget::onAddSetCommand()
{
    QString sParameter = ui->ParamNameLE->text();
    sParameter.remove(QRegExp("\\s+"));
    QString sFunctionStatus = ui->functionStatusCB->currentText();//(ui->functionStatusCB->currentIndex());

    if (sParameter.isEmpty())
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label->text()));
        return ;
    }


    QString sValue = ui->ValueLE->text();
    sValue.remove(QRegExp("\\s+"));

    if (sValue.isEmpty() && ui->SetOnlyFSCB->isChecked()==false)
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label_2->text()));
        return ;
    }

   /* if (ui->SetOnlyFSCB->isChecked())
    {
        if (isParameterFromDB() == false)
        {
            
            QMessageBox::critical(this,"Invalid",QString("Set only FS is not valid for Local variables").arg(ui->label_2->text()));
            return ;
        }
    }*/

    QString vmacForceType=ui->VmacForcingTypeCB->currentText();
    GTAEquationBase* pEquation = NULL;
    bool rc = _pEquationWgt->getEquation(pEquation);
    if (pEquation)
    {
        if(pEquation->getEquationType() == GTAEquationBase::CONST)
        {
            GTAEquationConst * pConst = dynamic_cast<GTAEquationConst*> (pEquation);
            if (pConst)
            {
                QString constantVal = pConst->getConstant();
                if (constantVal.isEmpty())
                {
                    pConst->setConstant(sValue);
                }
            }
        }
    }

    if (rc)
    {
        GTAActionOneClickSet* pSetCmd = new GTAActionOneClickSet(ACT_ONECLICK_SET_INSTANCE,"");

       
        pSetCmd->setParameter(sParameter);
        pSetCmd->setEquation(pEquation);
        pSetCmd->setFunctionStatus(sFunctionStatus);
        bool setFsStatus = ui->SetOnlyFSCB->isChecked();

        if (setFsStatus)
            pSetCmd->setIsSetOnlyFSFixed(true);
               

        _SetCommandList.push_back(pSetCmd);

        QListWidgetItem* pItem = new QListWidgetItem(ui->setActionListLV);
        if (ui->SetOnlyFSCB->isChecked())
        {
            delete pEquation;pEquation=NULL;
            pSetCmd->setEquation(pEquation);
            pItem->setText(QString("%1(FS:%2) [Set Functional Status]").arg(sParameter,sFunctionStatus));
        }
        else
            pItem->setText(QString("%1(FS:%2) = %3 ").arg(sParameter,sFunctionStatus,sValue));
        ui->setActionListLV->addItem(pItem);

        ui->setActionListLV->setCurrentIndex(QModelIndex());
    }
    
}
void GTAActionOneClickSetWidget::onRemoveSetCommand()
{
    QList<QListWidgetItem *> lstOfSelecteItems =  ui->setActionListLV->selectedItems();


    // QModelIndexList selectedIndex = ui->setActionListLV->selectedIndexes();
    //Only one selection is allowed, take the top selection
    if(!lstOfSelecteItems.isEmpty())
    {
        
        QListWidgetItem* pItem = lstOfSelecteItems[0];

        //selectedIndexes () const : QModelIndexLis
        if(NULL!=pItem)
        {
            //selected indexed are not directly available for qlistwidget.
            int totoalItems = ui->setActionListLV->count();
            
            for (int i=0;i<totoalItems;i++)
            {
                QListWidgetItem* pItemTocompare = ui->setActionListLV->item(i);
                if (pItem == pItemTocompare)
                {
                    if (_SetCommandList.size()>i)
                    {
                        GTAActionOneClickSet* pActionSet = _SetCommandList.takeAt(i);
                        if (NULL!=pActionSet)
                        {
                            delete pActionSet;pActionSet=NULL;
                        }

                        ui->setActionListLV->removeItemWidget(pItem);
                        delete pItem;pItem=NULL;
                        ui->setActionListLV->clearSelection();
                        ui->ParamNameLE->setText("");
                        silentSetValueText("");

                    }
                }
            }

            
        }
    }
    else
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Nothing selected for removal"));
    }

}

void GTAActionOneClickSetWidget::updateParamValueLineEdits()
{
    QList<QListWidgetItem *> lstOfSelecteItems =  ui->setActionListLV->selectedItems();
    //Only one selection is allowed, take the top selection

    if(!lstOfSelecteItems.isEmpty())
    {
        QListWidgetItem* pItem = lstOfSelecteItems[0];

        //selectedIndexes () const : QModelIndexLis
        if(NULL!=pItem)
        {
            //selected indexed are not directly available for qlistwidget.
            int totoalItems = ui->setActionListLV->count();

            for (int i=0;i<totoalItems;i++)
            {
                QListWidgetItem* pItemTocompare = ui->setActionListLV->item(i);
                if (pItem == pItemTocompare)
                {
                    if (_SetCommandList.size()>i)
                    {
                        GTAActionOneClickSet* pActionSet = _SetCommandList.at(i);
                        if (NULL!=pActionSet)
                        {
                            ui->ParamNameLE->setText(pActionSet->getParameter());

                            GTAEquationBase* pEqn;
                            pEqn=pActionSet->getEquation();
                            if (NULL!=pEqn)
                            {
                                silentSetValueText(pEqn->getStatement());
                                ui->SetOnlyFSCB->setChecked(pActionSet->getIsSetOnlyFSFixed());
                                _pEquationWgt->setEquation(pEqn);
                                if(pEqn->getEquationType() == GTAEquationBase::CONST)
                                {
                                    ui->ValueLE->setReadOnly(false);
                                }
                                else
                                    ui->ValueLE->setReadOnly(true);

                                ui->ValueLE->setDisabled(false);
                            }
                            else
                            {
                                silentSetFSCB();
                                ui->ValueLE->setDisabled(true);
                                ui->EquationPB->setDisabled(true);
                                ui->SearchPB2->setDisabled(true);
                            }
                            int dIndex = ui->functionStatusCB->findText(pActionSet->getFunctionStatus());
                            if(dIndex>=0)
                                ui->functionStatusCB->setCurrentIndex(dIndex);
                        }
                    }
                }
            }


        }
    }



    /*if(!lstOfSelecteItems.isEmpty())
    {
        QListWidgetItem* pItem = lstOfSelecteItems.at(0);
        if(NULL!=pItem)
        {
            QString sText = pItem->text();
            QStringList listOfparams = sText.split("=",QString::SplitBehavior::SkipEmptyParts);
            if (listOfparams.size()==2)
            {
                ui->ParamNameLE->setText(listOfparams.at(0));
                ui->ValueLE->setText(listOfparams.at(1));
            }
        }
    }*/
}
void GTAActionOneClickSetWidget::editSetCommand()
{
    QString sParameter = ui->ParamNameLE->text();
    sParameter.remove(QRegExp("\\s+"));

    if (sParameter.isEmpty())
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label->text()));
        return ;
    }


    QString sValue = ui->ValueLE->text();
    sValue.remove(QRegExp("\\s+"));

    if (sValue.isEmpty() && ui->SetOnlyFSCB->isChecked()==false)
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Enter valid value for %1 field").arg(ui->label_2->text()));
        return ;
    }

  /*  if (ui->SetOnlyFSCB->isChecked())
    {
        if (isParameterFromDB() == false)
        {
            
            QMessageBox::critical(this,"Invalid",QString("Set only FS is not valid for Local variables").arg(ui->label_2->text()));
            return ;
        }
    }*/
    QList<QListWidgetItem *> lstOfSelecteItems =  ui->setActionListLV->selectedItems();
    //Only one selection is allowed, take the top selection
    if(!lstOfSelecteItems.isEmpty())
    {
        QListWidgetItem* pItem = lstOfSelecteItems[0];

        if(NULL!=pItem)
        {
            //selected indexed are not directly available for qlistwidget.
            int totoalItems = ui->setActionListLV->count();

            for (int i=0;i<totoalItems;i++)
            {
                QListWidgetItem* pItemTocompare = ui->setActionListLV->item(i);
                if (pItem == pItemTocompare)
                {
                    if (_SetCommandList.size()>i)
                    {
                        GTAActionOneClickSet* pActionSet = _SetCommandList.at(i);
                        if (NULL!=pActionSet)
                        {
                            pActionSet->setParameter(sParameter);
                            ui->ParamNameLE->setText(pActionSet->getParameter());
                            GTAEquationBase* pEqn=NULL;
                            _pEquationWgt->getEquation(pEqn);
                            if (NULL!=pEqn)
                            {
                                pActionSet->setEquation(pEqn);
                               silentSetValueText(pEqn->getStatement());
                                _pEquationWgt->setEquation(pEqn);

                                if(pEqn->getEquationType() == GTAEquationBase::CONST)
                                {
                                    ui->ValueLE->setReadOnly(false);
                                }
                                else
                                    ui->ValueLE->setReadOnly(true);

                                QString sFunctionStatus =ui->functionStatusCB->currentText();
                                pActionSet->setFunctionStatus(sFunctionStatus);


                                pActionSet->setIsSetOnlyFSFixed(ui->SetOnlyFSCB->isChecked());

                                if (ui->SetOnlyFSCB->isChecked())
                                {
                                    delete pEqn;pEqn=NULL;
                                    pActionSet->setEquation(pEqn);
                                    pItem->setText(QString("%1(FS:%2) [Set Functional Status]").arg(sParameter,sFunctionStatus));
                                }
                                else
                                    pItem->setText(QString("%1(FS:%2) = %3 ").arg(sParameter,sFunctionStatus,sValue));
                                
                            }
                        }

                    }
                    break;
                }
            }


        }

    }
    else
    {
        if (this->isVisible())
            QMessageBox::critical(this,"Empty",QString("Nothing selected for removal"));
    }

    



}

void GTAActionOneClickSetWidget::updateVmacForceType(int)
{
    _vMacForceType=ui->VmacForcingTypeCB->currentText();
}
QString GTAActionOneClickSetWidget::getVmacForceType()const
{
    return ui->VmacForcingTypeCB->currentText();

}
void GTAActionOneClickSetWidget::onCancel()
{
    foreach(GTAActionOneClickSet* pSet,_SetCommandList)
    {
        if (pSet)
        {delete pSet;pSet=NULL;}
        
    }
    _SetCommandList.clear();
}
void GTAActionOneClickSetWidget::importParametersFromCSV()
{

    QString sCsvFilePath = QFileDialog::getOpenFileName(this, tr("Select Database"),"", tr("Excel File (*.csv)"));
    fillExpLWFromCSV(ui->setActionListLV,sCsvFilePath);


}

void GTAActionOneClickSetWidget::fillExpLWFromCSV(QListWidget* , const QString& iFilePath)
{

    QFileInfo iFileInfo(iFilePath);
    bool rC = true;

    QStringList validLines;
    if (iFileInfo.exists())
    {
        QFile csvFileObj(iFilePath);
        bool fOpenRC = csvFileObj.open(QIODevice::Text| QFile::ReadOnly);
        int lineNo=0;
        QStringList erroLines;
        if(fOpenRC)
        {
            while(!csvFileObj.atEnd())
            {
                lineNo++;
                QString line = csvFileObj.readLine();
                line = line.trimmed();
                if (validateRowFromCSV(line))
                    validLines.append(line);
                else
                {
                    rC = false;;
                    erroLines.append(QString::number(lineNo));
                }
            }
            
        }


        if (rC)
        {
            foreach(QString line, validLines)
            {
                QStringList paramInfoLst = line.split(";");


                QString sParameter = paramInfoLst.at(0);
                sParameter=sParameter.trimmed();

                QString sValue = paramInfoLst.at(1);
                sValue=sValue.trimmed();
                //associate current value to const.
                GTAActionOneClickSet* pSetCmd = new GTAActionOneClickSet(ACT_ONECLICK_SET_INSTANCE,"");
                if (sValue.isEmpty())
                {
                    pSetCmd->setIsSetOnlyFSFixed(true);
                    
                }
                else
                {
                    GTAEquationConst* pEquationConst = new GTAEquationConst;
                    GTAEquationBase* pEquation = pEquationConst;
                    pEquationConst->setConstant(sValue);
                    pSetCmd->setIsSetOnlyFSFixed(false);
                    pSetCmd->setEquation(pEquation);

                }
               
                QString sFS = paramInfoLst.at(2);
                sFS=sFS.trimmed();

                

                



                pSetCmd->setParameter(sParameter);
                pSetCmd->setFunctionStatus(sFS);

                _SetCommandList.push_back(pSetCmd);
                
                //update UI - set listlist widget
                QString sStatement;
                if (pSetCmd->getIsSetOnlyFSFixed()==true)
                {
                    sStatement = QString("%1(FS:%2) [Set Functional Status] ").arg(sParameter,sFS);
                }
                else
                 sStatement = QString("%1(FS:%2) = %3 ").arg(sParameter,sFS,sValue);
                
                ui->setActionListLV->addItem(sStatement);
                

            }
        }
        else
            if (this->isVisible())
                QMessageBox::critical(0,"Parse Error",QString("Error encountered in line(s) [%1]").arg(erroLines.join(",")));
        
        
        if(fOpenRC)
            csvFileObj.close();
    }
}

bool GTAActionOneClickSetWidget::validateRowFromCSV(const QString& iLine)
{
    bool rC = true;
    if (!iLine.isEmpty())
    {
        QStringList paramInfoLst = iLine.split(";");
        if (paramInfoLst.size()==3)
        {
            
            QString sParameter = paramInfoLst.at(0);
            sParameter=sParameter.trimmed();

            QString sValue = paramInfoLst.at(1);
            sValue=sValue.trimmed();

            QString sFS = paramInfoLst.at(2);
            sFS=sFS.trimmed();



            if (sParameter.contains(" ")||sFS.contains(" "))
            {
                rC = false;
            }
            else           
                if (!_sFSList.contains(sFS))
                {
                rC=false;

            }
            


        }
        else
            rC = false;
    }

    return rC;
}

QList<QWidget*>  GTAActionOneClickSetWidget::getWidgetsInTabOrder()const
{
    QList<QWidget*> lstOfWidgets;
    lstOfWidgets <<ui->ParamNameLE <<ui->functionStatusCB <<ui->importFromCSVPB <<ui->SearchPB
            <<ui->ValueLE <<ui->EquationPB<<ui->setActionListLV<<ui->addToListPB
            <<ui->removeFromListPB <<ui->editSelectedPB <<ui->VmacForcingTypeCB;
    return lstOfWidgets;

}
QWidget*  GTAActionOneClickSetWidget::getLastWidgetForTabOrder()const
{
    return ui->VmacForcingTypeCB;
}

bool GTAActionOneClickSetWidget::isParameterFromDB()
{/*
    bool rc = false;
    QString paramText = ui->ParamNameLE->text();
    if(paramText.isEmpty())
        return rc;
    GTAAppController* pCtrlr = GTAAppController::getGTAAppController();
    if (NULL!=pCtrlr)
    {

        QStringList paramList = QStringList()<<paramText;
        QList<GTAICDParameter> icdDetailList;
        pCtrlr->getICDDetails(paramList,icdDetailList);
        if (!icdDetailList.isEmpty())
            rc = true;

        
    }
    
    return rc;
    */


    // do not use this function right now.
    return true;

    
}
void GTAActionOneClickSetValueLineEdit::keyPressEvent(QKeyEvent * event)
{
    emit keyPressedForValueLE(this->text());
    this->QLineEdit::keyPressEvent(event);
}
GTAActionOneClickSetValueLineEdit::GTAActionOneClickSetValueLineEdit(QWidget* parent):QLineEdit(parent)
{

}
void GTAActionOneClickSetWidget::disableLineEdits(bool iStatus)
{

    if (iStatus==true)
    {
        if (isParameterFromDB())
        {
            ui->ValueLE->setDisabled(iStatus);
            ui->SearchPB2->setDisabled(iStatus);
            ui->EquationPB->setDisabled(iStatus);
            ui->noteLB->show();

        }
        else
        {

            if (this->isVisible())
                QMessageBox::information(this,"Invalid input parameter","Set only FS is not valid for Local variables");

            silentReSetFSCB();
            
        }

    }
    else
    {
        ui->ValueLE->setDisabled(false);
        ui->SearchPB2->setDisabled(false);
        ui->EquationPB->setDisabled(false);
         ui->noteLB->hide();
    }
    

}
void GTAActionOneClickSetWidget::silentSetValueText(const QString& iString)
{
    disconnect(ui->ValueLE,SIGNAL(textChanged(QString)),this,SLOT(onParameterValueEdited(QString)));
    ui->ValueLE->setText(iString);
    connect(ui->ValueLE,SIGNAL(textChanged(QString)),this,SLOT(onParameterValueEdited(QString)));
}
void GTAActionOneClickSetWidget::silentSetFSCB()
{

    disconnect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
    ui->SetOnlyFSCB->setChecked(true);
    connect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
}
void GTAActionOneClickSetWidget::silentReSetFSCB()
{

    disconnect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
    ui->SetOnlyFSCB->setChecked(false);
    connect(ui->SetOnlyFSCB,SIGNAL(toggled(bool)),this,SLOT(disableLineEdits(bool)));
}
void GTAActionOneClickSetWidget::disableFSCB( const QString & )
{
    if (ui->SetOnlyFSCB->isChecked())
    {
        silentReSetFSCB();
    }
    ui->ValueLE->setDisabled(false);
}
int GTAActionOneClickSetWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::PARAMETER;
    return retVal;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_ONECLICK_SET,GTAActionOneClickSetWidget,obj)
