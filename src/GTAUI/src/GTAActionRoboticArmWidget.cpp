#include "GTAActionRoboticArmWidget.h"
#include "ui_GTAActionRoboticArmWidget.h"
#include "GTAAppController.h"
#include "GTAGenSearchWidget.h"
#include <QMessageBox>
#include <QListWidgetItem>
#include <QDebug>
GTAActionRoboticArmWidget::GTAActionRoboticArmWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionRoboticArmWidget)
{
    ui->setupUi(this);

    _CurrentFunctionCode.clear();

    connect(ui->roboArmFileLE,SIGNAL(textChanged(QString)),this,SLOT(onSequenceFileChange(QString)));
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->SequenceListLW,SIGNAL(currentRowChanged(int)),this,SLOT(onFunctionSelectionChange(int)));
    ui->roboArmFileLE->setReadOnly(true);
    _IsEditMode = false;
}

GTAActionRoboticArmWidget::~GTAActionRoboticArmWidget()
{
    delete ui;

}
QString GTAActionRoboticArmWidget::getFunction() const
{
    QString oSelectedItem = ui->SequenceListLW->currentItem() != NULL ? ui->SequenceListLW->currentItem()->text() : QString();
    return oSelectedItem;
}

void GTAActionRoboticArmWidget::setFunction(const QString & iFunctionName)
{
    ui->SequenceListLW->addItem(iFunctionName);
}
QString GTAActionRoboticArmWidget::getSequenceFile() const
{
    return ui->roboArmFileLE->text();
}
void GTAActionRoboticArmWidget::setSquenceFile(const QString & iSequenceFile)
{
    ui->roboArmFileLE->setText(iSequenceFile);
}

void GTAActionRoboticArmWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
    disconnect(ui->SequenceListLW,SIGNAL(currentRowChanged(int)),this,SLOT(onFunctionSelectionChange(int)));
    disconnect(ui->roboArmFileLE,SIGNAL(textChanged(QString)),this,SLOT(onSequenceFileChange(QString)));

    GTAActionRoboArm * pRoboCmd = dynamic_cast<GTAActionRoboArm *>((GTAActionBase *)ipActionCmd);
    clear();
    _IsEditMode = false;

    if(pRoboCmd != NULL)
    {
        _IsEditMode = true;
        QString seqFile = pRoboCmd->getSequenceFile();
        QString functionName = pRoboCmd->getFunction();
        _CurrentFunctionCode = pRoboCmd->getFunctionCode();

        setSquenceFile(seqFile);

        ui->SequenceListLW->addItems(QStringList(functionName));
        if(ui->SequenceListLW->count() > 0)
            ui->SequenceListLW->setCurrentItem(ui->SequenceListLW->item(0));

        QDomDocument domDoc;
        domDoc.setContent(_CurrentFunctionCode);
        QDomElement domRootElem = domDoc.documentElement();
        QList<QDomNode> domNodeList;
        if(! domRootElem.isNull())
        {
            QDomNodeList listOfChild = domRootElem.childNodes();
            for(int i = 0; i < listOfChild.count(); i++)
                domNodeList.append(listOfChild.item(i));
        }
        GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
        QStringList contents;
        if(pAppCtrl != NULL)
            pAppCtrl->getRoboArmFunctionContents(domNodeList,contents);

        QString previewText;
        foreach(QString line, contents)
        {
            previewText += line + "\n";
        }
        ui->PreviewTE->setText(previewText);

    }
    connect(ui->roboArmFileLE,SIGNAL(textChanged(QString)),this,SLOT(onSequenceFileChange(QString)));
    connect(ui->SequenceListLW,SIGNAL(currentRowChanged(int)),this,SLOT(onFunctionSelectionChange(int)));

}

bool GTAActionRoboticArmWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    GTAActionRoboArm  * pCmd = NULL;
   
    if(isValid()==false)
    {
        opCmd = pCmd;
        return false;
    }
   
    QString sequenceFile = getSequenceFile();
    QString functionName = getFunction();
    pCmd = new GTAActionRoboArm(QString(),QString(),functionName);
    pCmd->setSequenceFile(sequenceFile);
    if(! _IsEditMode)
    {
        GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
        if(pAppCtrl != NULL)
        {
            //QList<QDomNode> xmlRpcList;
            //pAppCtrl->getIRTFunctionContents(sequenceFile,functionName,xmlRpcList);
            QByteArray oXmlDocRep;
            /*bool rc = */pAppCtrl->getRoboArmFunctionContents(sequenceFile,functionName,oXmlDocRep);
            pCmd->setFunctionCode(oXmlDocRep);
        }
    }
    else
        pCmd->setFunctionCode(_CurrentFunctionCode);

	_IsEditMode = false;
    opCmd= pCmd;

    return true;
}

void GTAActionRoboticArmWidget::onSequenceFileChange(const QString &iSequnceFileName)
{
	_IsEditMode = false;
    ui->PreviewTE->clear();
    ui->SequenceListLW->clear();
    if(!iSequnceFileName.isEmpty())
    {
        GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
        if(pAppCtrl != NULL)
        {
            QStringList FunctionList;

            pAppCtrl->getRoboArmFunctionList(iSequnceFileName,FunctionList);
            ui->SequenceListLW->addItems(FunctionList);
            if(FunctionList.count() > 0 )
            {
                ui->SequenceListLW->setCurrentItem(ui->SequenceListLW->item(0));
            }
        }
    }
}
void GTAActionRoboticArmWidget::onSearchPBClicked()
{
    emit searchObject(ui->roboArmFileLE);
}
void GTAActionRoboticArmWidget::clear()
{
    ui->roboArmFileLE->clear();
    ui->SequenceListLW->clear();
    ui->PreviewTE->clear();
	
}
bool GTAActionRoboticArmWidget::isValid() const 
{
    QString sequenceFile = getSequenceFile();
    QString functionName = getFunction();
    if(sequenceFile.isEmpty() ||  functionName.isEmpty())
        return false;

    return true;
}
void GTAActionRoboticArmWidget::onFunctionSelectionChange(int iSelectedRow)
{
    if(iSelectedRow >= 0)
    {
        QStringList xmlRpcFuncInStrList;
        QString functionName = ui->SequenceListLW->item(iSelectedRow) != NULL ? ui->SequenceListLW->item(iSelectedRow)->text() : QString();
        if(! functionName.isEmpty())
        {
            GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
            if(pAppCtrl != NULL)
            {
                QString seqFile = getSequenceFile();
                QList<QDomNode> xmlRpcFunc;
                bool rc = pAppCtrl->getRoboArmFunctionContents(seqFile,functionName,xmlRpcFunc);
                if(rc && xmlRpcFunc.count() >0)
                {
                    pAppCtrl->getRoboArmFunctionContents(xmlRpcFunc,xmlRpcFuncInStrList);
                }

            }
        }


        QString previewText;
        foreach(QString line, xmlRpcFuncInStrList)
        {
            previewText += line + "\n";
        }
        ui->PreviewTE->setText(previewText);
    }
}
int GTAActionRoboticArmWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::ROBO_ARM_OBJECT;
    return retVal;
}
//#include "GTAWgtRegisterDef.h"
//REGISTER_ACTION_WIDGET(ACT_ROBO_ARM,GTAActionRoboticArmWidget,obj)
