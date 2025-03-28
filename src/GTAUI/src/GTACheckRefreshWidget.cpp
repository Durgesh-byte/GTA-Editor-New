#include "GTACheckRefreshWidget.h"
#include "ui_GTACheckRefreshWidget.h"
#include "GTACheckRefresh.h"
#include "GTAAppController.h"
#include "GTAICDParameter.h"


GTACheckRefreshWidget::GTACheckRefreshWidget(QWidget *parent) : GTACheckWidgetInterface(parent),
    //    QWidget(parent),
    ui(new Ui::GTACheckRefreshWidget)
{
    ui->setupUi(this);
    connect(ui->searchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    
    /*QDoubleValidator* pDoubleValidator1 = new QDoubleValidator(ui->refreshRateLE);
    ui->refreshRateLE->setValidator(pDoubleValidator1);*/

    //QDoubleValidator* pDoubleValidator2 = new QDoubleValidator(ui->TimeOutLE);
    //ui->TimeOutLE->setValidator(pDoubleValidator2);

    /*QDoubleValidator* pDoubleValidator3 = new QDoubleValidator(ui->PrecisionLE);
    ui->PrecisionLE->setValidator(pDoubleValidator3);*/
}

GTACheckRefreshWidget::~GTACheckRefreshWidget()
{
    delete ui;
}


void GTACheckRefreshWidget::setCheckCommand(const GTACheckRefresh * ipCheckCmd)
{
    if(ipCheckCmd != NULL)
    {
        QString sParameter = ipCheckCmd->getParameter();
        QString sRefreshTime = ipCheckCmd->getRefreshTime();
        ui->ParameterLE->setText(sParameter);
        ui->refreshRateLE->setText(sRefreshTime);
        
    }
}

GTACheckRefresh * GTACheckRefreshWidget::getCheckCommand()
{
    GTACheckRefresh * pCheckCmd = new GTACheckRefresh();
    QString sParameter = ui->ParameterLE->text();
    QString sRefreshTime = ui->refreshRateLE->text();

    pCheckCmd->setRefreshTime(sRefreshTime);
    pCheckCmd->setParameter(sParameter);
    
    return pCheckCmd;
}
void GTACheckRefreshWidget::clear()
{
    ui->refreshRateLE->clear();
    ui->ParameterLE->clear();
   // ui->TimeOutLE->setText("0.0");
  /*  ui->PrecisionLE->setText("0.0");*/
//ui->OnFailCB->setCurrentIndex(0);
}

bool GTACheckRefreshWidget::isValid()
{
    bool rc = false;
    if(! ui->refreshRateLE->text().isEmpty())
        rc = true;
    return rc;
}

bool GTACheckRefreshWidget::getCommand(GTACheckBase *& pCheckCmd)
{
    bool rc = isValid();
    if(rc)
    {
        pCheckCmd = getCheckCommand();
    }
    return rc;
}

bool GTACheckRefreshWidget::setCommand(const GTACheckBase * pCheckCmd)
{
    bool rc = false;
    if(pCheckCmd != NULL )
    {
        GTACheckRefresh * pCmd = dynamic_cast<GTACheckRefresh*>((GTACheckBase*)pCheckCmd);
        if(pCmd != NULL)
        {
            setCheckCommand(pCmd);
            rc = true;
        }
    }
    return rc;
}


void GTACheckRefreshWidget::onSearchPBClicked()
{
    emit searchParameter(ui->ParameterLE);
}
void GTACheckRefreshWidget::processSearchInput(const GTAICDParameter& iResult)
{

    if (iResult.getName().isEmpty()==false)
    {
        ui->refreshRateLE->setText(iResult.getRefreshRate());

    }
}

bool GTACheckRefreshWidget::hasTimeOut()const
{
    return true;
}
bool GTACheckRefreshWidget::hasDumpList()const
{
    return false;
}
bool GTACheckRefreshWidget::hasPrecision()const
{
    return true;
}
bool GTACheckRefreshWidget::hasSearchItem()const
{
    return true;
}
bool GTACheckRefreshWidget::hasConfigTime() const
{
    return false;
}
bool GTACheckRefreshWidget::hasActionOnFail()const
{
    return true;
}

//void GTACheckRefreshWidget::fillRefreshRateInfo(const QString & iParameter)
//{
//  
//    QString parameter = iParameter;
//
//    if(parameter.isEmpty())
//        return ;
//    /*GTAAppController* pCtrlr = GTAAppController::getGTAAppController();
//    if (NULL!=pCtrlr)
//    {
//
//        QStringList paramList = QStringList()<<parameter;
//        QList<GTAICDParameter> icdDetailList;
//        pCtrlr->getICDDetails(paramList,icdDetailList);
//        if (icdDetailList.isEmpty())
//            return;
//
//        icdDetailList.clear();
//        GTAICDParameter paramInfo = icdDetailList.at(0);
//       
//
//    }*/
//
//    
//}
