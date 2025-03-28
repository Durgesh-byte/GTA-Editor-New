#include "GTAActionPrintValWidget.h"
#include "ui_GTAActionPrintValWidget.h"
#include "GTAActionPrint.h"
#include "GTAParamValidator.h"
#include "GTAUtil.h"
#include "GTAGenSearchWidget.h"
#include "GTAAppController.h"
#include <QCompleter>
GTAActionPrintValWidget::GTAActionPrintValWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionPrintValWidget)
{
    ui->setupUi(this);
    connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->addParamPB,SIGNAL(clicked()),this,SLOT(addToParamList()));
    connect(ui->editParamPB,SIGNAL(clicked()),this,SLOT(editFromParamList ()));
    connect(ui->removeParamPB,SIGNAL(clicked()),this,SLOT(removeFromParamList()));
    connect(ui->parameterListLW,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamLEFromLst()));
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

GTAActionPrintValWidget::~GTAActionPrintValWidget()
{
    delete ui;
}
QStringList GTAActionPrintValWidget::getValues() const
{
    QStringList sParameterList;
    int items = ui->parameterListLW->count();
    for(int i=0;i<items;i++)
    {
        QListWidgetItem* pWidgetItem = ui->parameterListLW->item(i);
        if (NULL!=pWidgetItem)
        {
            QString sParameter = pWidgetItem->text();
            if (!sParameter.isEmpty())
                sParameterList.append(sParameter);
        }
        
    }
    return sParameterList;
}
void GTAActionPrintValWidget::setValue(const QString & iVal)
{
    ui->parameterLE->setText(iVal);
}

void GTAActionPrintValWidget::onSearchPBClicked()
{
    emit searchObject(ui->parameterLE);
}

void GTAActionPrintValWidget::clear()
{
    ui->parameterLE->clear();
    ui->parameterListLW->clear();
}
bool GTAActionPrintValWidget::isValid()const
{
    return ui->parameterLE->text().isEmpty() ? false : true;
}
void GTAActionPrintValWidget::addToParamList()
{
    QString sParameter = ui->parameterLE->text();
    sParameter=sParameter.trimmed();
    QStringList processedParameters = GTAUtil::getProcessedParameterForChannel(sParameter,true,true);
    foreach(QString sParam,processedParameters)
    {
        if(!sParam.isEmpty())
        {
            ui->parameterListLW->addItem(sParam.trimmed());
        }
    }

}
void GTAActionPrintValWidget::removeFromParamList()
{
    QList<QListWidgetItem*> selectedItems = ui->parameterListLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        if (NULL!= pSelectedFirstItem)

            delete pSelectedFirstItem;
    }

}
void GTAActionPrintValWidget::editFromParamList()
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
void GTAActionPrintValWidget::setActionCommand(const GTAActionBase *iActCommand )
{

    GTAActionPrint* pPrintActionCmd = NULL;
    pPrintActionCmd = dynamic_cast<GTAActionPrint*> ((GTAActionBase *)iActCommand);
    if (NULL!=pPrintActionCmd)
    {
        QStringList lstSParameters = pPrintActionCmd->getValues();
        ui->parameterListLW->clear();
        foreach(QString sParameter,lstSParameters)
        {
            ui->parameterListLW->addItem(sParameter);
        }
    }
    
}
void GTAActionPrintValWidget::updateParamLEFromLst( )
{
    QList<QListWidgetItem*> items = ui->parameterListLW->selectedItems();
    if(!items.isEmpty())
    {
        QListWidgetItem* pFirstItem = items.at(0);
        if(NULL!=pFirstItem)
            ui->parameterLE->setText(pFirstItem->text());
    }

    
}
bool GTAActionPrintValWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;
    QStringList paramList = getValues();
    if(! paramList.isEmpty())
    {
        GTAActionPrint *pPrintActionCmd = new GTAActionPrint(ACT_PRINT,COM_PRINT_PARAM);
        pPrintActionCmd->setValues(paramList);
        pPrintActionCmd->setPrintType(GTAActionPrint::PARAMETER);
        opCmd = pPrintActionCmd;
        rc = true;
    }

    return rc;
}
int GTAActionPrintValWidget::getSearchType()
{

    int retVal = GTAGenSearchWidget::PARAMETER;
    return retVal;
}

bool GTAActionPrintValWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    for (auto param : params)
    {
        if (param.first == "parameterLE")
        {
            setValue(param.second);
            addToParamList();
        }
        else {
            errorLogs.append(QString("Expected parameterLE, got %1").arg(param.first));
            return false;
        }
    }
    return true;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_PARAM),GTAActionPrintValWidget,obj)
