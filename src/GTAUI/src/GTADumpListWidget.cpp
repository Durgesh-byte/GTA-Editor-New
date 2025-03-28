#include "GTADumpListWidget.h"
#include "ui_GTADumpListWidget.h"
#include "GTAActionPrint.h"
#include "GTAParamValidator.h"
#include <QMessageBox.h>
GTADumpListWidget::GTADumpListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTADumpListWidget)
{
    ui->setupUi(this);
    connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->addPB,SIGNAL(clicked()),this,SLOT(addToParamList()));
    connect(ui->cancelPB,SIGNAL(clicked()),this,SLOT(onEditingCancel()));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(editFinished()));
    connect(ui->editPB,SIGNAL(clicked()),this,SLOT(editFromParamList ()));
    connect(ui->deletePB,SIGNAL(clicked()),this,SLOT(removeFromParamList()));
    hideInformation();
    connect(ui->parameterListLW,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamLEFromLst()));
    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->parameterLE);
    ui->parameterLE->setValidator(pValidator1);
}

GTADumpListWidget::~GTADumpListWidget()
{
    delete ui;
}
QStringList GTADumpListWidget::getDumpList() const
{

    return _dumpParameterLst;
}


void GTADumpListWidget::onSearchPBClicked()
{
    emit searchObject(ui->parameterLE);
}
//
//void GTADumpListWidget::clear()
//{
//    ui->parameterLE->clear();
//    ui->parameterListLW->clear();
//}
//bool GTADumpListWidget::isValid()
//{
//    return ui->parameterLE->text().isEmpty() ? false : true;
//}
void GTADumpListWidget::addToParamList()
{
    QString sParameter = ui->parameterLE->text();
    sParameter=sParameter.trimmed();
    if(!sParameter.isEmpty())
    {
        if (!_currentDumpParameterLst.contains(sParameter))
        {
            ui->parameterListLW->addItem(sParameter);
            _currentDumpParameterLst.append(sParameter);
            hideInformation();
        }
        else
            showInformation(QString("parameter: \"%1\" already exists in list").arg(sParameter));
        
    }
}
void GTADumpListWidget::removeFromParamList()
{
    QList<QListWidgetItem*> selectedItems = ui->parameterListLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        if (NULL!= pSelectedFirstItem)
        {
            _currentDumpParameterLst.removeAll(pSelectedFirstItem->text());
            delete pSelectedFirstItem;
        }
    }
     hideInformation();

}
void GTADumpListWidget::editFinished()
{
    _dumpParameterLst.clear();
    _dumpParameterLst = _currentDumpParameterLst;
    setDumpList(_currentDumpParameterLst);
     hideInformation();
    this->hide();
}
void GTADumpListWidget::onEditingCancel()
{
    if (_currentDumpParameterLst!=_dumpParameterLst)
    {
        QMessageBox msgBox (this);
        QString text = QString("Current changes will be lost, continue with exit?");
        msgBox.setText(text);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("File Load warning");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret==QMessageBox::Yes)
        {
            _currentDumpParameterLst.clear();
            _currentDumpParameterLst = _dumpParameterLst;
            setDumpList(_dumpParameterLst);
            this->hide();

        }
    }
    else
    {
        this->hide();
    }
     hideInformation();
    
    
}
void GTADumpListWidget::setDumpList(QStringList iDumpList)
{
    hideInformation();
    clearDumpList();
    ui->parameterListLW->addItems(iDumpList);
    _currentDumpParameterLst.clear();
    _dumpParameterLst.clear();
    _dumpParameterLst.append(iDumpList);
    _currentDumpParameterLst.append(iDumpList);
    ui->parameterLE->clear();
}
void GTADumpListWidget:: clearDumpList(bool bClearData)
{
    ui->parameterListLW->clear();
    hideInformation();
    ui->parameterLE->clear();
    if(bClearData)
    {
        _currentDumpParameterLst.clear();
        _dumpParameterLst.clear();
    }

}
void GTADumpListWidget::editFromParamList()
{

    hideInformation();
    QList<QListWidgetItem*> selectedItems = ui->parameterListLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        QString sParameter = ui->parameterLE->text();
        sParameter=sParameter.trimmed();

        if (NULL!=pSelectedFirstItem)
        {
            QString sCurrentText = pSelectedFirstItem->text();
            if(!sParameter.isEmpty() && _currentDumpParameterLst.contains(sCurrentText) )
            {
                if (_currentDumpParameterLst.contains(sParameter))
                {
                    showInformation(QString("parameter: \"%1\" already exists in list").arg(sParameter));
                }
                else
                {
                    _currentDumpParameterLst.replace(_currentDumpParameterLst.indexOf(sCurrentText),sParameter);
                    pSelectedFirstItem->setText(sParameter);
                }    
            }

        }
        
    }

}
void GTADumpListWidget::hideInformation()
{
    //ui->informationLE->hide();
    ui->infoGB->hide();
    ui->informationLE->hide();
}
void GTADumpListWidget::showInformation(const QString& isInformation)
{
    ui->informationLE->setText(isInformation);
    ui->infoGB->show();
    ui->informationLE->show();
}

//void GTADumpListWidget::setActionCommand(GTAActionPrint *ipPrintActionCmd )
//{
//
//    QStringList lstSParameters = ipPrintActionCmd->getValues();
//    ui->parameterListLW->clear();
//    foreach(QString sParameter,lstSParameters)
//    {
//        ui->parameterListLW->addItem(sParameter);
//    }
//}
void GTADumpListWidget::updateParamLEFromLst( )
{
    QList<QListWidgetItem*> items = ui->parameterListLW->selectedItems();
    if(!items.isEmpty())
    {
        QListWidgetItem* pFirstItem = items.at(0);
        if(NULL!=pFirstItem)
            ui->parameterLE->setText(pFirstItem->text());
    }

    
}
