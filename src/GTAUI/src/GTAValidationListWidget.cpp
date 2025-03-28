#include "GTAValidationListWidget.h"
#include "ui_GTAValidationListWidget.h"
#include <QDebug>
#include "GTAFilterModel.h"

GTAValidationListWidget::GTAValidationListWidget(GTAWidget *parent) :
    GTAWidget(parent),
    ui(new Ui::GTAValidationListWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    _pModel = NULL;
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowModality(Qt::ApplicationModal);

    ui->ValidateClick_PB->setDisabled(true);
    ui->ValidateClick_PB->setToolTip("Vaidates current selections and returns to Editor viewer");
    ui->SKIPClick_PB->setToolTip("Skips the current selections and return to Editor viewer");
    ui->CancelClick_PB->setToolTip("Cancels current selections and continue in log result viewer");
    ui->ExpandAllTB->setToolTip("Expands All");
    ui->CollapseAllTB->setToolTip("Collapse All");
    ui->SelectAllTB->setToolTip("Selects All item");
    ui->DeselectAllTB->setToolTip("Deselect All");

    connect(ui->ExpandAllTB,SIGNAL(clicked()),this,SLOT(onExpandAll()));
    connect(ui->CollapseAllTB,SIGNAL(clicked()),this,SLOT(onCollapseAll()));
    connect(ui->SelectAllTB,SIGNAL(clicked()),this,SLOT(onSelectAll()));
    connect(ui->DeselectAllTB,SIGNAL(clicked()),this,SLOT(onDeSelectAll()));
    connect(ui->ValidateClick_PB,SIGNAL(clicked()),this,SLOT(onValidate_Skip_Cancel_PBClicked()));
    connect(ui->SKIPClick_PB,SIGNAL(clicked()),this,SLOT(onValidate_Skip_Cancel_PBClicked()));
    connect(ui->CancelClick_PB,SIGNAL(clicked()),this,SLOT(onValidate_Skip_Cancel_PBClicked()));
    connect(ui->ValidatorNameLE,SIGNAL(textChanged(QString)),this,SLOT(enableDisableValidate(QString)));

}

GTAValidationListWidget::~GTAValidationListWidget()
{
    onUpdateModel();
    if(_pModel != NULL)
    {
        delete _pModel;
        _pModel = NULL;
    }
    delete ui;
}

void GTAValidationListWidget::onExpandAll()
{
    ui->ValidationElementTV->expandAll();
}

void GTAValidationListWidget::onCollapseAll()
{
    ui->ValidationElementTV->collapseAll();
}

void GTAValidationListWidget::onSelectAll()
{
    selectAll(true);
}

void GTAValidationListWidget::onDeSelectAll()
{
    selectAll(false);
}

void GTAValidationListWidget::setModel(QAbstractItemModel *pModel)
{
    if (NULL != pModel)
    {
        onUpdateModel();
        ui->ValidationElementTV->setModel(pModel);
        if (NULL != ui->ValidationElementTV->model())
        {
            for (int k=1;k<ui->ValidationElementTV->model()->columnCount();k++)
            {
                ui->ValidationElementTV->hideColumn(k);
            }
        }
    }
}

void GTAValidationListWidget::onUpdateModel()
{
    QAbstractItemModel* pModel = ui->ValidationElementTV->model();
    if (NULL != pModel)
    {
        ui->ValidationElementTV->setModel(NULL);
        delete pModel;
        pModel = NULL;
    }
}



void GTAValidationListWidget::onValidate_Skip_Cancel_PBClicked()
{
    QString validatorName;
    QObject* pSender = sender();
    if(pSender == ui->ValidateClick_PB)
    {
        QStringList selectedUUIDList;
        QAbstractItemModel *pModel = ui->ValidationElementTV->model();
        GTAValidationWidgetModel *pValidationModel = dynamic_cast<GTAValidationWidgetModel*>(pModel);
        if(NULL != pValidationModel)
        {
            selectedUUIDList = pValidationModel->getElementUUID();
        }
        validatorName = ui->ValidatorNameLE->text();
        emit selectedFiles(validatorName, selectedUUIDList);
        this->close();
    }
    else if(pSender == ui->SKIPClick_PB)
    {
        emit changeToEditorView(true);
        this->close();
    }
    else if (pSender == ui->CancelClick_PB)
    {
        this->close();
    }

}

void GTAValidationListWidget::setFileName(const QString &iFileName)
{
    ui->FileNameLabel->setText(iFileName);
}

void GTAValidationListWidget::enableDisableValidate(QString text)
{
    if (text.isEmpty())
        ui->ValidateClick_PB->setDisabled(true);
    else
        ui->ValidateClick_PB->setDisabled(false);
}

void GTAValidationListWidget::selectAll(bool isSelectAll)
{
    QAbstractItemModel *pModel = ui->ValidationElementTV->model();
    GTAValidationWidgetModel *pValidationModel = dynamic_cast<GTAValidationWidgetModel*>(pModel);
    if(NULL != pValidationModel)
    {
        GTAElement *pValidationElem = NULL;
        pValidationElem = pValidationModel->getElement();

        if(NULL != pValidationElem)
        {
            pValidationModel->LayoutAboutToBeChanged();
            int count = pValidationElem->getAllChildrenCount();
            for(int i =0; i <count; i++)
            {
                GTACommandBase *pCmd = NULL;
                pValidationElem->getCommand(i, pCmd);
                if(pCmd != NULL)
                {
                    pCmd->setIgnoreInSCXML(!isSelectAll);

                }
            }
            pValidationModel->LayoutChanged();
        }
    }
}
