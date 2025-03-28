#include "GTAActionInitParamWidget.h"
#include "ui_GTAActionInitParamWidget.h"
#include "GTAActionSubscribe.h"
#include "GTAParamValidator.h"
#include "GTAUtil.h"
#include "GTAGenSearchWidget.h"
#include "GTAAppController.h"
#include <QCompleter>

GTAActionInitParamWidget::GTAActionInitParamWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionInitParamWidget)
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

GTAActionInitParamWidget::~GTAActionInitParamWidget()
{
    delete ui;
}
QStringList GTAActionInitParamWidget::getValues() const
{
    QStringList sParameterList;
    int items = ui->parameterListLW->count();
    for(int i=0;i<items;i++)
    {
        QListWidgetItem* pWidgetItem = ui->parameterListLW->item(i);
        if (NULL!=pWidgetItem)
        {
            const QString sParameter = pWidgetItem->isSelected() ? ui->parameterLE->text() : pWidgetItem->text(); // update automatically if selected
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
void GTAActionInitParamWidget::setValue(const QString & iVal)
{
    ui->parameterLE->setText(iVal);
}

void GTAActionInitParamWidget::onSearchPBClicked()
{
    emit searchObject(ui->parameterLE);
}

void GTAActionInitParamWidget::clear()
{
    ui->parameterLE->clear();
    ui->parameterListLW->clear();
}
bool GTAActionInitParamWidget::isValid()const
{
    return ui->parameterLE->text().isEmpty() ? false : true;
}
void GTAActionInitParamWidget::addToParamList()
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
void GTAActionInitParamWidget::removeFromParamList()
{
    QList<QListWidgetItem*> selectedItems = ui->parameterListLW->selectedItems();
    if(!selectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedFirstItem = selectedItems.at(0);
        if (NULL!= pSelectedFirstItem)

            delete pSelectedFirstItem;
    }

}
void GTAActionInitParamWidget::editFromParamList()
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
void GTAActionInitParamWidget::setActionCommand(const GTAActionBase *iActCommand )
{

    GTAActionSubscribe* pSubActionCmd = NULL;
    pSubActionCmd = dynamic_cast<GTAActionSubscribe*> ((GTAActionBase *)iActCommand);
    if (NULL!=pSubActionCmd)
    {
        QStringList lstSParameters = pSubActionCmd->getValues();
        ui->parameterListLW->clear();
        foreach(QString sParameter,lstSParameters)
        {
            ui->parameterListLW->addItem(sParameter);
        }
    }
    
}
void GTAActionInitParamWidget::updateParamLEFromLst( )
{
    QList<QListWidgetItem*> items = ui->parameterListLW->selectedItems();
    if(!items.isEmpty())
    {
        QListWidgetItem* pFirstItem = items.at(0);
        if(NULL!=pFirstItem)
            ui->parameterLE->setText(pFirstItem->text());
    }

    
}
bool GTAActionInitParamWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;
    QStringList paramList = getValues();
    if(! paramList.isEmpty())
    {
        GTAActionSubscribe *pSubActionCmd = new GTAActionSubscribe(ACT_SUBSCRIBE,COM_SUBSCRIBE_PARAM);
        pSubActionCmd->setValues(paramList);
        opCmd = pSubActionCmd;
        rc = true;
    }

    return rc;
}

int GTAActionInitParamWidget::getSearchType()
{

    //int retVal = GTAGenSearchWidget::PARAMETER;
    int retVal = GTAGenSearchWidget::INIT_PIR_SEARCH;
    return retVal;

}

bool GTAActionInitParamWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    for (auto param : params)
    {
        if (param.first == "parameterLE" && param.second != "")
        {
            param.second = param.second.trimmed();
            QStringList processedParameters = GTAUtil::getProcessedParameterForChannel(param.second, true);
            for(auto sParam : processedParameters)
            {
                if (!sParam.isEmpty())
                {
                    ui->parameterListLW->addItem(sParam.trimmed());
                }
            }
        }
    }
    
    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_SUBSCRIBE,COM_SUBSCRIBE_PARAM),GTAActionInitParamWidget,obj)
