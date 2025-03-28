#include "GTAActionCallProceduresWgt.h"
#include "ui_GTAActionCallProceduresWgt.h"
#include "GTAActionCallProcedures.h"
#include "GTAParamValidator.h"
#include "GTAUtil.h"
#include "GTAGenSearchWidget.h"
#include "GTAAppController.h"
#include "GTAParamValueValidator.h"

#pragma warning (push, 0)
#include <QFileInfo>
#include <QCompleter>
#include <QMessageBox>
#include <QDir>
#pragma warning (pop)
GTAActionCallProceduresWgt::GTAActionCallProceduresWgt(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionCallProceduresWgt)
{
    ui->setupUi(this);
    connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->addPB,SIGNAL(clicked()),this,SLOT(prepareToAddCall()));
    connect(this,SIGNAL(addCall()),this,SLOT(addCallProcedures()));
    connect(this,SIGNAL(editCall()),this,SLOT(editFromCallList ()));
    connect(ui->editPB,SIGNAL(clicked()),this,SLOT(prepareToEditCall ()));
    //connect(ui->procLW,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamLEFromLst()));
    //connect(ui->procLW,SIGNAL(itemClicked (QListWidgetItem*)),this,SLOT(updateParamLEFromLst(QListWidgetItem*)));
    connect(ui->removePB,SIGNAL(clicked()),this,SLOT(removeFromCallList())); 
    connect(ui->procLE,SIGNAL(textChanged( const QString & )),this,SLOT(callProcedureLEChanged( const QString &))); 
    connect(ui->procLW,SIGNAL(currentRowChanged (int)),this,SLOT(updateParamLEFromLst(int)));

    ui->parallelCB->hide();
    ui->parallelCB->setChecked(true);
    //     connect(ui->parameterListLW,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamLEFromLst()));
    //     GTAParamValidator* pValidator1 = new GTAParamValidator(ui->parameterLE);
    //     ui->parameterLE->setValidator(pValidator1);
    //
    //     GTAAppController* pController = GTAAppController::getGTAAppController();
    //     if (NULL!=pController)
    //     {
    //         QStringList wordList = pController->getVariablesForAutoComplete();
    //         QCompleter *completer = new QCompleter(wordList, this);
    //         completer->setCaseSensitivity(Qt::CaseInsensitive);
    //         ui->parameterLE->setCompleter(completer);
    //     }

    _pSearchWidget = new GTAGenSearchWidget();
    if(_pSearchWidget)
    {
        _pSearchWidget->hide();
        connect(_pSearchWidget,SIGNAL(okPressed()),this,SLOT(onSearchOKPBClick()));
    }
}

GTAActionCallProceduresWgt::~GTAActionCallProceduresWgt()
{
    if(_pSearchWidget != NULL)
    {
        delete _pSearchWidget;
        _pSearchWidget = NULL;
    }

    delete ui;
}
QStringList GTAActionCallProceduresWgt::getValues() const
{
    QStringList sParameterList;
    /*int items = ui->parameterListLW->count();
    for(int i=0;i<items;i++)
    {
        QListWidgetItem* pWidgetItem = ui->parameterListLW->item(i);
        if (NULL!=pWidgetItem)
        {
            QString sParameter = pWidgetItem->text();
            if (!sParameter.isEmpty())
                sParameterList.append(sParameter);
        }
        
    }*/
    return sParameterList;
}
void GTAActionCallProceduresWgt::setValue(const QString & )
{
    // ui->parameterLE->setText(iVal);
}


void GTAActionCallProceduresWgt::onSearchOKPBClick()
{
    GTAICDParameter item = _pSearchWidget->getSelectedItems();

    ui->procLE->setText(item.getName());
    _RefFileUUID = item.getUUID();
    _pSearchWidget->hide();
}

void GTAActionCallProceduresWgt::onSearchPBClicked()
{
    //    emit searchObject(ui->procLE);
    GTAGenSearchWidget::ElementType CurrSearchType = (GTAGenSearchWidget::ElementType)getSearchType();
    _pSearchWidget->setSearchType(CurrSearchType);
    _pSearchWidget->show();
}

void GTAActionCallProceduresWgt::clear()
{
    //     ui->parameterLE->clear();
    //     ui->parameterListLW->clear();
}
bool GTAActionCallProceduresWgt::isValid()const
{
    QString sCalledItem = ui->procLE->text();
    if(_CallProcedures.isEmpty())
    {
        _LastError = "Empty Call procedures";
        return false;
    }

    return true;
    
}
void GTAActionCallProceduresWgt::prepareToAddCall()
{
        if(ui->tagTableWidget->rowCount()>0)
            ui->tagTableWidget->selectRow(0);
    emit addCall();
    ui->tagTableWidget->clearSelection();
}
void GTAActionCallProceduresWgt::prepareToEditCall()
{
        if(ui->tagTableWidget->rowCount()>0)
            ui->tagTableWidget->selectRow(0);
    emit editCall();
    ui->tagTableWidget->clearSelection();
}
bool  GTAActionCallProceduresWgt::tagValuePairsOK()
{
    bool rc = true;
    //QModelIndexList indexList = ui->tagTableWidget->selectedIndexes();
    //if(indexList.isEmpty()==false)
    //    ui->tagTableWidget->update(indexList.at(0));
        int noOfRows= ui->tagTableWidget->rowCount();
        for (int i=0;i<noOfRows;i++)
        {
            QTableWidgetItem *pValueItem =NULL;
            pValueItem = ui->tagTableWidget->item(i,1);
            if (pValueItem!=NULL)
            {
                QString sVal = pValueItem->text(); //TODO:are empty spaces allowed??
                sVal.remove(QRegExp("\\s+"));
                GTAParamValueValidator validatorObj;
                int lastChar = 0;
                lastChar = sVal.size()-1;

                if (sVal.isEmpty() )
                {
                    _LastError = QString("Value against tag : %1 is empty").arg(ui->tagTableWidget->item(i,0)->text());
                    return false;
                }
                else if (  validatorObj.validate(sVal,lastChar) == QValidator::Invalid) 
                {
                    _LastError = QString("Value against tag : %1 contains forbidden charater(s)").arg(ui->tagTableWidget->item(i,0)->text());
                    return false;
                }
            }
        }
    _LastError = "";
    return rc;

}
void GTAActionCallProceduresWgt::addCallProcedures()
{   
    
    if(ui->procLE->text().isEmpty())
    {
        QMessageBox::critical(this,"Error","Provide procedure/function name");
        return;
    }
    if(!tagValuePairsOK())
    {
        QMessageBox::critical(this,"Error",_LastError);
        return;
    }

    QString sCallProcedureName = ui->procLE->text();
    sCallProcedureName = sCallProcedureName.trimmed();
    if (sCallProcedureName.isEmpty() == false)
    {
        if (sCallProcedureName.endsWith(".pro",Qt::CaseInsensitive)||sCallProcedureName.endsWith(".fun",Qt::CaseInsensitive))
        {
            GTASystemServices* pSysServices = GTASystemServices::getSystemServices();
            QString sDirectory = pSysServices->getDataDirectory();
            QString sProcPath = QString("%1%2%3").arg(sDirectory,QDir::separator(),sCallProcedureName);

            QFileInfo fileInfo(QDir::cleanPath(sProcPath));
            //fileInfo.setFile(sDirectory,sCallProcedureName);

            if (fileInfo.exists())
            {
                QMap<QString,QString> tagValueMap;

                if(ui->elementTypeCB->currentIndex()==0)
                {

                    int noOfRows = ui->tagTableWidget->rowCount();
                    for (int i=0;i<noOfRows;i++)
                    {
                        QTableWidgetItem *pVarItem =NULL;
                        pVarItem = ui->tagTableWidget->item(i,0);

                        QTableWidgetItem *pValueItem =NULL;
                        pValueItem = ui->tagTableWidget->item(i,1);

                        if (NULL!=pValueItem && NULL!=pVarItem)
                        {
                            QString sVar = pVarItem->text();
                            QString sVal = pValueItem->text();
                            if (!sVal.isEmpty() && !sVar.isEmpty())
                            {
                                tagValueMap[sVar]=sVal;
                            }
                        }

                    }

                    foreach(GTACallProcItem item,_CallProcedures)
                    {
                        if (item._elementName == sCallProcedureName)
                        {
                            _RefFileUUID = item._UUID;
                            break;
                        }
                    }

                    GTACallProcItem item(GTACallProcItem::PROCEDURE,sCallProcedureName,tagValueMap,_RefFileUUID);
                    _CallProcedures.append(item);
                    ui->procLW->addItem(sCallProcedureName);
                    ui->procLE->clear();
                }
                else
                {
                    
                    int noOfRows = ui->tagTableWidget->rowCount();
                    for (int i=0;i<noOfRows;i++)
                    {
                        QTableWidgetItem *pVarItem =NULL;
                        pVarItem = ui->tagTableWidget->item(i,0);

                        QTableWidgetItem *pValueItem =NULL;
                        pValueItem = ui->tagTableWidget->item(i,1);

                        if (NULL!=pValueItem && NULL!=pVarItem)
                        {
                            QString sVar = pVarItem->text();
                            QString sVal = pValueItem->text();
                            if (!sVal.isEmpty() && !sVar.isEmpty())
                            {
                                tagValueMap[sVar]=sVal;
                            }
                        }

                    }

                    foreach(GTACallProcItem item,_CallProcedures)
                    {
                        if (item._elementName == sCallProcedureName)
                        {
                            _RefFileUUID = item._UUID;
                            break;
                        }
                    }

                    GTACallProcItem item(GTACallProcItem::FUNCTION,sCallProcedureName,tagValueMap,_RefFileUUID);
                    _CallProcedures.append(item);
                    ui->procLW->addItem(sCallProcedureName);

                    ui->procLE->clear();
                    resetTableWidget();

                }
                
            }
            else
                QMessageBox::information(this,"Invalid file","Call procedure does not exist");

            
        }
        else
            QMessageBox::information(this,"Invalid file","Call procedure is invalid");
    }

}
void GTAActionCallProceduresWgt::removeFromCallList()
{
    QList<QListWidgetItem*> selectedItems = ui->procLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        //ui->procLW->clearSelection();
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);

        int selectedIndex = ui->procLW->row(pSelectedFirstItem);
        
        if(selectedIndex>=0)
        {
            _CallProcedures.removeAt(selectedIndex);

            if(pSelectedFirstItem)
                delete pSelectedFirstItem;
        }

        ui->procLW->clearSelection();
        ui->procLE->clear();
        resetTableWidget();


    }

}
void GTAActionCallProceduresWgt::editFromCallList()
{
    if(ui->procLE->text().isEmpty())
    {
        QMessageBox::critical(this,"Error","Provide procedure/function name");
        return;
    }
    if(!tagValuePairsOK())
    {
        QMessageBox::critical(this,"Error",_LastError);
        return;
    }


    QString sCallProcedureName = ui->procLE->text();
    sCallProcedureName=sCallProcedureName.trimmed();
    if (sCallProcedureName.isEmpty() == false)
    {
        if (sCallProcedureName.endsWith(".pro",Qt::CaseInsensitive)||sCallProcedureName.endsWith(".fun",Qt::CaseInsensitive))
        {
            GTASystemServices* pSysServices = GTASystemServices::getSystemServices();
            QString sDirectory = pSysServices->getDataDirectory();
            QString sProcPath = QString("%1%2%3").arg(sDirectory,QDir::separator(),sCallProcedureName);

            QFileInfo fileInfo(QDir::cleanPath(sProcPath));
            //fileInfo.setFile(sDirectory,sCallProcedureName);

            if (fileInfo.exists())
            {
                QList<QListWidgetItem*> selectedItems = ui->procLW->selectedItems();
                if(!selectedItems.isEmpty())
                {
                    QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
                    QString sSubCallProcedureName = ui->procLE->text();
                    sSubCallProcedureName = sSubCallProcedureName.trimmed();
                    int selectedIndex = ui->procLW->row(pSelectedFirstItem);
                    if(!sSubCallProcedureName.isEmpty())
                    {
                        QMap<QString,QString> tagValueMap;
                        if(ui->elementTypeCB->currentIndex()==0)
                        {
                            pSelectedFirstItem->setText(sSubCallProcedureName);
                            _CallProcedures.removeAt(selectedIndex);

                            int noOfRows = ui->tagTableWidget->rowCount();
                            for (int i=0;i<noOfRows;i++)
                            {
                                QTableWidgetItem *pVarItem =NULL;
                                pVarItem = ui->tagTableWidget->item(i,0);

                                QTableWidgetItem *pValueItem =NULL;
                                pValueItem = ui->tagTableWidget->item(i,1);

                                if (NULL!=pValueItem && NULL!=pVarItem)
                                {
                                    QString sVar = pVarItem->text();
                                    QString sVal = pValueItem->text();
                                    if (!sVal.isEmpty() && !sVar.isEmpty())
                                    {
                                        tagValueMap[sVar]=sVal;
                                    }
                                }
                            }

                            GTACallProcItem item(GTACallProcItem::PROCEDURE, sSubCallProcedureName,tagValueMap,_RefFileUUID);
                            _CallProcedures.insert(selectedIndex,item);
                        }
                        else
                        {
                            pSelectedFirstItem->setText(sSubCallProcedureName);
                            _CallProcedures.removeAt(selectedIndex);

                            int noOfRows = ui->tagTableWidget->rowCount();
                            for (int i=0;i<noOfRows;i++)
                            {
                                QTableWidgetItem *pVarItem =NULL;
                                pVarItem = ui->tagTableWidget->item(i,0);

                                QTableWidgetItem *pValueItem =NULL;
                                pValueItem = ui->tagTableWidget->item(i,1);

                                if (NULL!=pValueItem && NULL!=pVarItem)
                                {
                                    QString sVar = pVarItem->text();
                                    QString sVal = pValueItem->text();
                                    if (!sVal.isEmpty() && !sVar.isEmpty())
                                    {
                                        tagValueMap[sVar]=sVal;
                                    }
                                }

                            }

                            GTACallProcItem item(GTACallProcItem::FUNCTION, sSubCallProcedureName,tagValueMap,_RefFileUUID);
                            _CallProcedures.insert(selectedIndex,item);

                        }
                    }

                }

            }
            else
                QMessageBox::information(this,"Invalid file","Call procedure does not exist");


        }
        else
            QMessageBox::information(this,"Invalid file","Call procedure is invalid");
    }



}
void GTAActionCallProceduresWgt::setActionCommand(const GTAActionBase *iActCommand )
{
    //
    GTAActionCallProcedures* pCallProcedures = NULL;
    _CallProcedures.clear();
    pCallProcedures = dynamic_cast<GTAActionCallProcedures*> ((GTAActionBase *)iActCommand);
    if (NULL!=pCallProcedures)
    {
        QStringList lstSParameters = pCallProcedures->getProcedureNames();
        ui->procLW->clear();
        resetTableWidget();

        int dProcedureCount = pCallProcedures->procedureCount();
        for (int i=0;i<dProcedureCount ;i++)
        {
            GTACallProcItem::call_type callType;
            QString elementName;
            QMap<QString,QString> tagValuePair;
			QString UUID;
            bool rc = pCallProcedures->getCallProcedure(i,callType,elementName,tagValuePair,UUID);
            if (rc)
            {
                GTACallProcItem item(callType,elementName,tagValuePair,UUID);
                _CallProcedures.append(item);
                ui->procLW->addItem(elementName);
            }

        }


        bool bPrallelExec = pCallProcedures->getPrallelExecution();
        ui->parallelCB->setChecked(bPrallelExec);
    }
    
}
void GTAActionCallProceduresWgt::updateParamLEFromLst( )
{
    QList<QListWidgetItem*> items = ui->procLW->selectedItems();

    if(!items.isEmpty())
    {
        QListWidgetItem* pFirstItem = items.at(0);
        if(NULL!=pFirstItem)
        {
            ui->procLE->setText(pFirstItem->text());
            /* int selectedRow = ui->procLW->row(pFirstItem);
           resetTableWidget();

            if(selectedRow<_CallProcedures.count())
            {
                GTACallProcItem item =  _CallProcedures.at(selectedRow);
                if (item._type==GTACallProcItem::FUNCTION)
                {
                    ui->tagTableWidget->show();
                    foreach(QString tag, item._tagValue.keys())
                    {
                        addTag(tag,item._tagValue.value(tag));

                    }
                }
                else
                    ui->tagTableWidget->hide();
            }*/
        }
    }
    
}
void GTAActionCallProceduresWgt::updateParamLEFromLst(QListWidgetItem* item )
{

    QListWidgetItem* pFirstItem = item;
    if(NULL!=pFirstItem)
    {
        resetTableWidget();
        ui->procLE->setText(pFirstItem->text());
        int selectedRow = ui->procLW->row(pFirstItem);


        if(selectedRow<_CallProcedures.count())
        {
            GTACallProcItem subItem =  _CallProcedures.at(selectedRow);

                ui->tagTableWidget->show();
                QList<TagVariablesDesc> taglist = getTagListForElement(subItem._UUID,true);
                for (const auto& tag : subItem._tagValue.keys())
                {
                    for (const auto& tagVarStruct : taglist)
                    {
                        if(tagVarStruct.name.compare(tag))
                        {
                            addTag(tagVarStruct, subItem._tagValue.value(tag));
                        }
                    }

                }
        }
    }


}
void GTAActionCallProceduresWgt::updateParamLEFromLst(int idx )
{

    ui->procLE->clear();

    if(idx<_CallProcedures.size())
    {
        GTACallProcItem::call_type callType = _CallProcedures.at(idx)._type;
        if (callType == GTACallProcItem::PROCEDURE)
        {
            ui->elementTypeCB->setCurrentIndex(0);
        }
        else
            ui->elementTypeCB->setCurrentIndex(1);


    }
    QListWidgetItem* pFirstItem = ui->procLW->item(idx); 
    if(NULL!=pFirstItem)
    {
        resetTableWidget();
        disconnect(ui->procLE,SIGNAL(textChanged( const QString & )),this,SLOT(callProcedureLEChanged( const QString &)));
        ui->procLE->setText(pFirstItem->text());
        connect(ui->procLE,SIGNAL(textChanged( const QString & )),this,SLOT(callProcedureLEChanged( const QString &)));
        int selectedRow = ui->procLW->row(pFirstItem);

        if(selectedRow<_CallProcedures.count())
        {
            GTACallProcItem item =  _CallProcedures.at(selectedRow);
            _RefFileUUID = item._UUID;
            QList<TagVariablesDesc> taglist = getTagListForElement(item._UUID,true);

            foreach(QString tag, item._tagValue.keys())
            {
                bool found = false;
                // foreach(TagVariablesDesc tagVarStruct,taglist)
                for(int i=0 ; i<taglist.count() ; i++)
                {
                    TagVariablesDesc tagVarStruct = taglist.at(i);
                    if(tagVarStruct.name.compare(tag) == 0) //if the current taglist of the element has that tag present
                    {
                        addTag(tagVarStruct,item._tagValue.value(tag));
                        taglist.removeAt(i);
                        i--;
                        found = true;
                        break;
                    }
                }
                if(!found) //do not show it in the tag table
                {
                    item._tagValue.remove(tag);
                }
            }
            //dynamically check if a new tag is added to the releated procedure/function
            //update it with blank input
            if (!taglist.isEmpty())
            {
                foreach (TagVariablesDesc tag, taglist)
                {
                    addTag(tag);
                }
            }
            if (item._type == GTACallProcItem::FUNCTION)
            {
                GTACallProcItem editItem(GTACallProcItem::FUNCTION,item._elementName,item._tagValue,item._UUID);
                _CallProcedures.replace(selectedRow,editItem);
            }
            else if (item._type == GTACallProcItem::PROCEDURE)
            {
                GTACallProcItem editItem(GTACallProcItem::PROCEDURE,item._elementName,item._tagValue,item._UUID);
                _CallProcedures.replace(selectedRow,editItem);
            }
        }

    } 


}



bool GTAActionCallProceduresWgt::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;
    if(isValid())
    {
        if(! _CallProcedures.isEmpty())
        {
            GTAActionCallProcedures *pCallProcedures = new GTAActionCallProcedures;
            pCallProcedures->setCallProcedures(_CallProcedures);
            if (ui->parallelCB->isChecked())
                pCallProcedures->setParallelExecution(true);
            else
                pCallProcedures->setParallelExecution(false);

            opCmd = pCallProcedures;
            rc = true;
        }
    }

    return rc;
}
int GTAActionCallProceduresWgt::getSearchType()
{

    int retVal = GTAGenSearchWidget::PROCEDURE;
    if (ui->elementTypeCB->currentIndex()!=0)
        retVal=GTAGenSearchWidget::FUNCTION;
    
    return retVal;
}
void GTAActionCallProceduresWgt::toggleTagTableView(int iIndex)
{
    if (iIndex==0)
        ui->tagTableWidget->hide();
    else
        ui->tagTableWidget->show();
}
void GTAActionCallProceduresWgt::callProcedureLEChanged( const QString & iText)
{
    resetTableWidget();
        setTagTable(iText);

}
QList<TagVariablesDesc> GTAActionCallProceduresWgt::getTagListForElement(QString isFileName,bool isUUID)
{
    QList<TagVariablesDesc> tagList;
    GTAAppController* pController = GTAAppController::getGTAAppController();
    GTAElement* pElement =NULL;
    bool rC = pController->getElementFromDocument(isFileName,pElement,isUUID);

    if (rC && NULL!=pElement)
    {
        tagList = pElement->getTagList();
        delete pElement;pElement=NULL;
    }
    else
    {
        _LastError=pController->getLastError();
    }
    return tagList;
}

void GTAActionCallProceduresWgt::setTagTable(QString isFileName)
{
    QList<TagVariablesDesc> tagList = getTagListForElement(isFileName,false);
        if (!tagList.isEmpty())
        {
            ui->tagTableWidget->show();
            foreach(TagVariablesDesc sTagName, tagList)
            {
                if (!sTagName.name.isEmpty())
                {
                    addTag(sTagName);
                }
            }
        }
        else
        {
            QString sText = QString("No variables found in %1").arg(isFileName);
        }
}

void GTAActionCallProceduresWgt::addTag( const TagVariablesDesc& iTagVar , const QString& iTagVal)
{
    int noOfRows = ui->tagTableWidget->rowCount();
    ui->tagTableWidget->setColumnCount(2);
    ui->tagTableWidget->setRowCount(noOfRows+1);


    QTableWidgetItem * pTableHeader = new QTableWidgetItem();
    ui->tagTableWidget->setVerticalHeaderItem(noOfRows, pTableHeader);

    QTableWidgetItem *pNewItem = new QTableWidgetItem();
    pNewItem->setText(iTagVar.name);
    pNewItem->setToolTip(QString("Type : %1 \n Unit : %2 \n Supported Value : %3 \n Max : %4 \n Min : %5 Description : %6").arg(iTagVar.type,iTagVar.unit,iTagVar.supportedValue,iTagVar.max,iTagVar.min,iTagVar.Desc));
    ui->tagTableWidget->setItem(noOfRows,0,pNewItem);
    pNewItem->setFlags(Qt::ItemIsSelectable); // make it selectable only

    QTableWidgetItem *pNewItemValue = new QTableWidgetItem();
    ui->tagTableWidget->setItem(noOfRows,1,pNewItemValue);
    ui->tagTableWidget->show();
    if (!iTagVal.isEmpty())
        pNewItemValue->setText(iTagVal);



}
void GTAActionCallProceduresWgt::resetTableWidget()
{
    ui->tagTableWidget->clear();
    ui->tagTableWidget->setColumnCount(2);
    ui->tagTableWidget->setRowCount(0);
    QStringList headerLabels;
    headerLabels<<"Variable"<<"Value";
    ui->tagTableWidget->setHorizontalHeaderLabels(headerLabels);

}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_CALL_PROCS,GTAActionCallProceduresWgt,obj)
