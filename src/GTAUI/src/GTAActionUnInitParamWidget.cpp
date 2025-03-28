#include "GTAActionUnInitParamWidget.h"
#include "ui_GTAActionUnInitParamWidget.h"
#include "GTAActionUnSubscribe.h"
#include "GTAParamValidator.h"
#include "GTAUtil.h"
#include "GTAGenSearchWidget.h"
#include "GTAAppController.h"
#include <QCompleter>

GTAActionUnInitParamWidget::GTAActionUnInitParamWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionUnInitParamWidget)
{
    ui->setupUi(this);
    connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->addParamPB,SIGNAL(clicked()),this,SLOT(addToParamList()));
    connect(ui->editParamPB,SIGNAL(clicked()),this,SLOT(editFromParamList ()));
    connect(ui->removeParamPB,SIGNAL(clicked()),this,SLOT(removeFromParamList()));
    connect(ui->parameterListLW,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamLEFromLst()));
    connect(ui->ReleaseAllCB, SIGNAL(toggled(bool)),this, SLOT(onReleaseAllClicked(bool)));
    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->parameterLE);
    ui->parameterLE->setValidator(pValidator1);

    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
        QStringList wordList = pController->getVariablesForAutoComplete();
        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->parameterLE->setCompleter(completer);
    }
}

GTAActionUnInitParamWidget::~GTAActionUnInitParamWidget()
{
    delete ui;
}
QStringList GTAActionUnInitParamWidget::getValues() const
{
    QStringList sParameterList;
    int items = ui->parameterListLW->count();
    for(int i=0;i<items;i++)
    {
        QListWidgetItem* pWidgetItem = ui->parameterListLW->item(i);
        if (NULL!=pWidgetItem)
        {            
            const QString sParameter = pWidgetItem->isSelected() ? ui-> parameterLE->text() : pWidgetItem->text(); // update automatically if selected
            if (!sParameter.isEmpty())
            {
                sParameterList.append(sParameter);
            }                
        }
    }
    const QString current_parameter = ui->parameterLE->text();
    if (!sParameterList.contains(current_parameter) && !current_parameter.isEmpty())
    {
        sParameterList.append(current_parameter); // add automatically if not in list
    }
    return sParameterList;
}
void GTAActionUnInitParamWidget::setValue(const QString & iVal)
{
    ui->parameterLE->setText(iVal);
}

void GTAActionUnInitParamWidget::onSearchPBClicked()
{
    emit searchObject(ui->parameterLE);
}

void GTAActionUnInitParamWidget::clear()
{
    ui->parameterLE->clear();
    ui->parameterListLW->clear();
}
bool GTAActionUnInitParamWidget::isValid()const
{
    bool rc = true;
    if(ui->ReleaseAllCB->isChecked())
    {
        rc = true;
    }
    else
    {
        if(ui->parameterLE->text().isEmpty() && (ui->parameterListLW->count() == 0))
        {
            rc = false;
        }
    }

    return rc;
}
void GTAActionUnInitParamWidget::addToParamList()
{
    QString sParameter = ui->parameterLE->text();
    sParameter=sParameter.trimmed();
    QStringList processedParameters = GTAUtil::getProcessedParameterForChannel(sParameter,true);
    foreach(QString sParam,processedParameters)
    {
        if(!sParam.isEmpty())
        {            
            QListWidgetItem* item = new QListWidgetItem(sParam.trimmed());
            ui->parameterListLW->addItem(item);
            item->setSelected(true);
        }
    }

}
void GTAActionUnInitParamWidget::removeFromParamList()
{
    QList<QListWidgetItem*> selectedItems = ui->parameterListLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        if (NULL!= pSelectedFirstItem)

            delete pSelectedFirstItem;
    }

}
void GTAActionUnInitParamWidget::editFromParamList()
{

    QList<QListWidgetItem*> selectedItems = ui->parameterListLW->selectedItems();
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
void GTAActionUnInitParamWidget::setActionCommand(const GTAActionBase *iActCommand )
{

    GTAActionUnSubscribe* pUnSubActionCmd = NULL;
    pUnSubActionCmd = dynamic_cast<GTAActionUnSubscribe*> ((GTAActionBase *)iActCommand);
    if (NULL!=pUnSubActionCmd)
    {
        if(pUnSubActionCmd->isReleaseAll())
        {
            ui->ReleaseAllCB->setChecked(true);
        }
        else
        {
            QStringList lstSParameters = pUnSubActionCmd->getValues();
            ui->parameterListLW->clear();
            foreach(QString sParameter,lstSParameters)
            {
                ui->parameterListLW->addItem(sParameter);
            }
        }
    }

}
void GTAActionUnInitParamWidget::updateParamLEFromLst( )
{
    QList<QListWidgetItem*> items = ui->parameterListLW->selectedItems();
    if(!items.isEmpty())
    {
        QListWidgetItem* pFirstItem = items.at(0);
        if(NULL!=pFirstItem)
            ui->parameterLE->setText(pFirstItem->text());
    }

}
bool GTAActionUnInitParamWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;

    GTAActionUnSubscribe *pUnSubActionCmd = new GTAActionUnSubscribe(ACT_UNSUBSCRIBE,COM_UNSUBSCRIBE_PARAM);
    if(ui->ReleaseAllCB->isChecked())
    {
        pUnSubActionCmd->setReleaseAll(true);
        opCmd = pUnSubActionCmd;
        rc = true;
    }
    else
    {
        if(isValid())
        {

            QStringList paramList = getValues();
            if(! paramList.isEmpty())
            {
                pUnSubActionCmd->setValues(paramList);
                opCmd = pUnSubActionCmd;
                rc = true;
            }

        }
        else
        {
           opCmd = NULL;
           rc = false;
        }

    }
    return rc;
}

int GTAActionUnInitParamWidget::getSearchType()
{

    int retVal = GTAGenSearchWidget::RELEASE_PIR_SEARCH;
    return retVal;
}

void GTAActionUnInitParamWidget::processSearchInput(const QStringList &iSearchItemList)
{
    _paramList.clear();
    _paramList = iSearchItemList;
    foreach(QString sParameter,iSearchItemList)
    {
        ui->parameterListLW->addItem(sParameter);
    }
    ui->parameterLE->clear();
}

void GTAActionUnInitParamWidget::processSearchInput(const QList<GTAICDParameter> &iSearchItemList)
{
    _benchParamList.clear();
    _benchParamList = iSearchItemList;
    foreach(GTAICDParameter sParameter,iSearchItemList)
    {
        ui->parameterListLW->addItem(sParameter.getName());
    }
    ui->parameterLE->clear();
}

void GTAActionUnInitParamWidget::onReleaseAllClicked(bool iVal)
{
    ui->searchPB->setDisabled(iVal);
    ui->parameterLE->setDisabled(iVal);
    ui->parameterListLW->setDisabled(iVal);
    ui->addParamPB->setDisabled(iVal);
    ui->editParamPB->setDisabled(iVal);
    ui->removeParamPB->setDisabled(iVal);
    if(iVal)
    {
        ui->parameterListLW->clear();
    }
    else
    {
        foreach(QString sParameter,_paramList)
        {
            ui->parameterListLW->addItem(sParameter);
        }
    }
    ui->parameterLE->clear();

}

bool GTAActionUnInitParamWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    ui->ReleaseAllCB->setChecked(true);
    return true;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_UNSUBSCRIBE,COM_UNSUBSCRIBE_PARAM),GTAActionUnInitParamWidget,obj)
