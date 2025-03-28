#include "GTAActionReleaseWidget.h"
#include "ui_GTAActionReleaseWidget.h"
#include "GTAParamValidator.h"
#include "GTAUtil.h"
#include "GTAAppController.h"
#include <QCompleter>
GTAActionReleaseWidget::GTAActionReleaseWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionReleaseWidget)
{
    ui->setupUi(this);

    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->AddPB,SIGNAL(clicked()),this,SLOT(onAddPBClicked()));
    connect(ui->RemovePB,SIGNAL(clicked()),this,SLOT(onRemovePBClicked()));
    connect(ui->ParamLW,SIGNAL(clicked(QModelIndex)),this,SLOT(onParamSelectionChanged(QModelIndex)));

    _pSearchParameter = new GTAGenSearchWidget();
    if (_pSearchParameter)
    {
        _pSearchParameter->hide();
        connect(_pSearchParameter, SIGNAL(okPressed()), this, SLOT(onSearchWidgetOKClicked()));

    }

    GTAParamValidator* pValidator2 = new GTAParamValidator(ui->ParamLE);
    ui->ParamLE->setValidator(pValidator2);

    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
        QStringList wordList = pController->getVariablesForAutoComplete();
        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->ParamLE->setCompleter(completer);
    }

}

GTAActionReleaseWidget::~GTAActionReleaseWidget()
{
    delete ui;
}
void GTAActionReleaseWidget::onAddPBClicked()
{
    QString sParameter = ui->ParamLE->text();
    sParameter=sParameter.trimmed();
    QStringList processedParameters = GTAUtil::getProcessedParameterForChannel(sParameter,true,true);
    foreach(QString sParam,processedParameters)
    {
        if(!sParam.isEmpty())
        {
            ui->ParamLW->addItem(sParam.trimmed());
        }
    }
}
void GTAActionReleaseWidget::onRemovePBClicked()
{
    QModelIndex index = ui->ParamLW->currentIndex();
    if(index.isValid())
    {
        QListWidgetItem * pItem = ui->ParamLW->takeItem(index.row());
        if(pItem != NULL)
        {
            delete pItem; pItem = NULL;
            ui->ParamLE->clear();
        }
    }
}

/**
 * @brief Handles the click event on the Search button.
 *
 * This function is triggered when the Search button is clicked. It sets the search type
 * to RELEASE and shows the search parameter window if it's not null.
 */
void GTAActionReleaseWidget::onSearchPBClicked()
{
    if (_pSearchParameter != NULL)
    {
        _pSearchParameter->setSearchType(GTAGenSearchWidget::RELEASE);
        _pSearchParameter->show();
    }
}

/**
 * @brief Processes the selection made in the search widget.
 *
 * This slot is called when an OK action is triggered in the search widget. It retrieves the
 * selected item and, if not empty, hides the search window. If no item is selected in the
 * parameter list widget, it adds the selected item's name after processing. If an item is
 * already selected, it replaces the selected item's text with the new name or sets the text
 * in the QLineEdit, based on the text's current state.
 */
void GTAActionReleaseWidget::onSearchWidgetOKClicked()
{
    GTAICDParameter selectedItem = _pSearchParameter->getSelectedItems();

    // Check if sParameter is not empty
    if (!selectedItem.getName().isEmpty())
    {
        // Hide the search Window
        _pSearchParameter->hide();
        // Directly add the item from ParamLE to ParamLW
        QString sParameter = selectedItem.getName().trimmed();
        QStringList processedParameters = GTAUtil::getProcessedParameterForChannel(sParameter, true, true);
        // Check if there's any selected item in the QListWidget
        QListWidgetItem* currentSelected = ui->ParamLW->currentItem();
        if (currentSelected == nullptr)
        {
            // No item is selected, add new items
            foreach(const QString & sParam, processedParameters)
            {
                if (!sParam.isEmpty())
                {
                    ui->ParamLW->addItem(sParam);
                }
            }
            // Clear the ParamLE after adding the item(s)
            ui->ParamLE->clear();
        }
        else
        {
            // An item is selected, replace its text with the first processed parameter
            // Assuming processedParameters will have at least one valid entry given the check above
            if (!processedParameters.isEmpty() && !ui->ParamLE->text().isEmpty())
            {
                currentSelected->setText(processedParameters.first().trimmed());
                ui->ParamLW->selectionModel()->clearSelection();
                // Clear the ParamLE after adding the item(s)
                ui->ParamLE->clear();
            }
            else if (!processedParameters.isEmpty() && ui->ParamLE->text().isEmpty())
            {
                QString parameterName = processedParameters.first().trimmed();
                ui->ParamLW->addItem(parameterName);
                ui->ParamLW->selectionModel()->clearSelection();
            }
        }    
    }
}

void GTAActionReleaseWidget::onParamSelectionChanged(const QModelIndex &iIndex)
{
    if(iIndex.isValid())
    {
        QListWidgetItem * pItem = ui->ParamLW->currentItem();
        if(pItem != NULL)
        {
            ui->ParamLE->setText(pItem->text());
        }
    }
}
void GTAActionReleaseWidget::clear()
{
    ui->ParamLE->clear();
    ui->ParamLW->clear();
}
bool GTAActionReleaseWidget::isValid()const
{
    if(ui->ParamLW->count() > 0)
    {
        return true;
    }
    return false;
}
QStringList GTAActionReleaseWidget::getParameterList() const
{
    QStringList oParamList;
    int paramCount = ui->ParamLW->count();
    for(int i = 0; i< paramCount; i++)
    {
        QListWidgetItem * pItem = ui->ParamLW->item(i);
        if(pItem != NULL)
        {
            oParamList.append(pItem->text());
        }
    }
	return oParamList;
}

bool GTAActionReleaseWidget::getActionCommand(GTAActionBase *& pCmd)const
{
    bool rc = false;
    if(isValid())
    {
        GTAActionRelease * pActionCmd = new GTAActionRelease(ACT_REL,COM_REL_PAR);
        pActionCmd->setParameterList(getParameterList());
        pCmd = pActionCmd;
        rc = true;
    }
    return rc;
}

void GTAActionReleaseWidget::setActionCommand(const GTAActionBase * pCmd)
{
    clear();
   
    if(pCmd != NULL )
    {
        GTAActionRelease * pActionCmd = dynamic_cast<GTAActionRelease*>((GTAActionBase*)pCmd);
        if(pActionCmd != NULL)
        {
            QStringList paramList = pActionCmd->getParameterList();
            ui->ParamLW->addItems(paramList);
        }
    }
   
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_REL,COM_REL_PAR),GTAActionReleaseWidget,obj)
