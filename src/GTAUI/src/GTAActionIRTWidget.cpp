#include "GTAActionIRTWidget.h"
#include "ui_GTAActionIRTWidget.h"
#include "GTAAppController.h"
#include "GTAActionIRT.h"
#include "GTAGenSearchWidget.h"
#include <QMessageBox>
#include <QListWidgetItem>
#include <QDebug>
GTAActionIRTWidget::GTAActionIRTWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionIRTWidget)
{
    ui->setupUi(this);

    _CurrentFunctionCode.clear();

    connect(ui->SequenceFileLE,SIGNAL(textChanged(QString)),this,SLOT(onSequenceFileChange(QString)));
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->SequenceListLW,SIGNAL(currentRowChanged(int)),this,SLOT(onFunctionSelectionChange(int)));

    _IsEditMode = false;
}

GTAActionIRTWidget::~GTAActionIRTWidget()
{
    delete ui;

}
QString GTAActionIRTWidget::getFunction() const
{
    QString oSelectedItem = ui->SequenceListLW->currentItem() != NULL ? ui->SequenceListLW->currentItem()->text() : QString();
    return oSelectedItem;
}

void GTAActionIRTWidget::setFunction(const QString & iFunctionName)
{
    ui->SequenceListLW->addItem(iFunctionName);
}
QString GTAActionIRTWidget::getSequenceFile() const
{
    return ui->SequenceFileLE->text();
}
void GTAActionIRTWidget::setSquenceFile(const QString & iSequenceFile)
{
    ui->SequenceFileLE->setText(iSequenceFile);
}

void GTAActionIRTWidget::setActionCommand(const GTAActionBase * ipActCommand)
{
    disconnect(ui->SequenceListLW,SIGNAL(currentRowChanged(int)),this,SLOT(onFunctionSelectionChange(int)));
    disconnect(ui->SequenceFileLE,SIGNAL(textChanged(QString)),this,SLOT(onSequenceFileChange(QString)));

    clear();
    _IsEditMode = false;

    GTAActionIRT* pActIRT = dynamic_cast<GTAActionIRT*>((GTAActionBase * )ipActCommand);
    if(pActIRT != NULL)
    {
        _IsEditMode = true;
        QString seqFile = pActIRT->getSequenceFile();
        QString functionName = pActIRT->getFunction();
        _CurrentFunctionCode = pActIRT->getFunctionCode();

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
            pAppCtrl->getIRTFunctionContents(domNodeList,contents);

        QString previewText;
        foreach(QString line, contents)
        {
            previewText += line + "\n";
        }
        ui->PreviewTE->setText(previewText);

    }
    connect(ui->SequenceFileLE,SIGNAL(textChanged(QString)),this,SLOT(onSequenceFileChange(QString)));
    connect(ui->SequenceListLW,SIGNAL(currentRowChanged(int)),this,SLOT(onFunctionSelectionChange(int)));

}

bool GTAActionIRTWidget::getActionCommand(GTAActionBase *& opActCmd)const
{
    GTAActionIRT  * pCmd = NULL;
    QString sequenceFile = getSequenceFile();
    QString functionName = getFunction();
    if(sequenceFile.isEmpty() ||  functionName.isEmpty())
        return false;

    pCmd = new GTAActionIRT(ACT_IRT,QString(),functionName);
    pCmd->setSequenceFile(sequenceFile);
    if(! _IsEditMode)
    {
        GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
        if(pAppCtrl != NULL)
        {
            //QList<QDomNode> xmlRpcList;
            //pAppCtrl->getIRTFunctionContents(sequenceFile,functionName,xmlRpcList);
            QByteArray oXmlDocRep;
            /*bool rc = */pAppCtrl->getIRTFunctionContents(sequenceFile,functionName,oXmlDocRep);
            pCmd->setFunctionCode(oXmlDocRep);
        }
    }
    else
        pCmd->setFunctionCode(_CurrentFunctionCode);

    opActCmd=pCmd;

	_IsEditMode = false;
    return true;
}

void GTAActionIRTWidget::onSequenceFileChange(const QString &iSequnceFileName)
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

            pAppCtrl->getIRTFunctionList(iSequnceFileName,FunctionList);
            ui->SequenceListLW->addItems(FunctionList);
            if(FunctionList.count() > 0 )
            {
                ui->SequenceListLW->setCurrentItem(ui->SequenceListLW->item(0));
            }
        }
    }
}
void GTAActionIRTWidget::onSearchPBClicked()
{
    emit searchObject(ui->SequenceFileLE);
}
void GTAActionIRTWidget::clear()
{
    ui->SequenceFileLE->clear();
    ui->SequenceListLW->clear();
    ui->PreviewTE->clear();
	
}

void GTAActionIRTWidget::onFunctionSelectionChange(int iSelectedRow)
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
                bool rc = pAppCtrl->getIRTFunctionContents(seqFile,functionName,xmlRpcFunc);
                if(rc && xmlRpcFunc.count() >0)
                {
                    pAppCtrl->getIRTFunctionContents(xmlRpcFunc,xmlRpcFuncInStrList);
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
int GTAActionIRTWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::IRT_OBJECT;
    return retVal;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_IRT,GTAActionIRTWidget,obj)
