#include "GTATitleBasedExecution.h"
#include "ui_GTATitleBasedExecution.h"

QPointer<GTATitleBasedExecution> GTATitleBasedExecution::_pTitleBasedWidget = 0;

GTATitleBasedExecution::GTATitleBasedExecution(GTAWidget *parent) :
    GTAWidget(parent),
    ui(new Ui::GTATitleBasedExecution)
{
    ui->setupUi(this);
    _pModel = NULL;
    _varList.clear();
    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->ExpandAllTB,SIGNAL(clicked()),this,SLOT(onExpandAll()));
    connect(ui->CollapseAllTB,SIGNAL(clicked()),this,SLOT(onCollapseAll()));
    connect(ui->SelectAllTB,SIGNAL(clicked()),this,SLOT(onSelectAll()));
    connect(ui->DeSelectAllTB,SIGNAL(clicked()),this,SLOT(onDeSelectAll()));
    connect(ui->ExecuteTB,SIGNAL(clicked()),this,SLOT(onExecute()));
}

GTATitleBasedExecution::~GTATitleBasedExecution()
{
    if(_pModel != NULL)
    {
        delete _pModel;
        _pModel = NULL;
    }

    delete ui;
}


QPointer<GTATitleBasedExecution> GTATitleBasedExecution::getTittleBasedWidget(GTAWidget *parent)
{
    
    if(_pTitleBasedWidget == NULL)
    {
        _pTitleBasedWidget = new GTATitleBasedExecution(parent);
    }
    return _pTitleBasedWidget;
}

void GTATitleBasedExecution::setVariableList(const QStringList &iVarList)
{
    _varList<<iVarList;
}

QStringList& GTATitleBasedExecution::getVariableList()
{
    return _varList;
}

void GTATitleBasedExecution::setModel(QAbstractItemModel *pModel)
{
    if(pModel != NULL)
    {
        onUpdateModel();
        _pModel = pModel;
        ui->ElementTV->setModel(_pModel);
    }
}

void GTATitleBasedExecution::onUpdateModel()
{
    if (NULL != _pModel)
    {
        delete _pModel;
        _pModel = NULL;
        ui->ElementTV->setModel(NULL);
    }
}

void GTATitleBasedExecution::setFileName(const QString &iFileName)
{
    ui->ProcNameLbl->setText(iFileName);
}

void GTATitleBasedExecution::onExpandAll()
{
     ui->ElementTV->expandAll();

}

void GTATitleBasedExecution::onCollapseAll()
{
    ui->ElementTV->reset();
}

void GTATitleBasedExecution::onSelectAll()
{
    selectAll(true);
}

void GTATitleBasedExecution::onDeSelectAll()
{

    selectAll(false);

}

void GTATitleBasedExecution::onExecute()
{
    GTAElement *pTitleElem = NULL;
    QAbstractItemModel *pModel = ui->ElementTV->model();
    GTATitleBasedExecutionModel *pTitleModel = dynamic_cast<GTATitleBasedExecutionModel*>(pModel);
    if(pTitleModel != NULL)
    {
        pTitleElem = pTitleModel->getElement();
    }
    emit executeTitle(_uuid,pTitleElem,_varList);
}


//Dev Comment : you can use _pModel also. Check after replacing with _pModel
/*
  * GTATitleBasedExecutionModel::selectAll(bool isSelectAll)
  * Args : isSelectAll - selects all commands when true, de-selects all commands when false
  */
void GTATitleBasedExecution::selectAll(bool isSelectAll)
{
    QAbstractItemModel *pModel = ui->ElementTV->model();
    GTATitleBasedExecutionModel *pTitleModel = dynamic_cast<GTATitleBasedExecutionModel*>(pModel);
    if(pTitleModel)
    {
        GTAElement *pTitleElem = NULL;
        pTitleElem = pTitleModel->getElement();

        if(pTitleElem != NULL)
        {
            pTitleModel->LayoutAboutToBeChanged();
//            int count = pTitleElem->getDirectChildrenCount();
            int count = pTitleElem->getAllChildrenCount();
            for(int i = 0; i < count; i++)
            {
                GTACommandBase *pCmd = NULL;
                pTitleElem->getCommand(i,pCmd);
                if(pCmd != NULL)
                {
                    pCmd->setIgnoreInSCXML(!isSelectAll);

                }
            }
            pTitleModel->LayoutChanged();
//            ui->ElementTV->reset();
        }

    }
}
