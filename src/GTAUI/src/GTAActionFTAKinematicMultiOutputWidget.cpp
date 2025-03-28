#include "GTAActionFTAKinematicMultiOutputWidget.h"
#include "GTAActionFTAKinematicMultiOutput.h"
#include "ui_GTAActionFTAKinematicMultiOutputWidget.h"
#include "GTAAppController.h"
#include <QFileDialog>
#include <QMessageBox>


GTAActionFTAKinematicMultiOutputWidget::GTAActionFTAKinematicMultiOutputWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionFTAKinematicMultiOutputWidget)
{
    ui->setupUi(this);
    connect(ui->BrowseRefFilePB,SIGNAL(clicked()),this,SLOT(onBrowseRefFilePBClicked()));
    
    //input variable search connections

    _hshToolButtonLE.insert(ui->input1SearchPB,ui->InputParam1LE);
    _hshToolButtonLE.insert(ui->input2SearchPB,ui->InputParam2LE);
    _hshToolButtonLE.insert(ui->input3SearchPB,ui->InputParam3LE);
   
    _hshToolButtonLE.insert(ui->output1SearchPB,ui->OutputParam1LE);
    _hshToolButtonLE.insert(ui->output2SearchPB,ui->OutputParam2LE);
    _hshToolButtonLE.insert(ui->output3SearchPB,ui->OutputParam3LE);
    _hshToolButtonLE.insert(ui->output4SearchPB,ui->OutputParam4LE);
    _hshToolButtonLE.insert(ui->output5SearchPB,ui->OutputParam5LE);

    connect(ui->input1SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->input2SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->input3SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));

    connect(ui->output1SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->output2SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->output3SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->output4SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->output5SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->RefFilePathLE,SIGNAL(textChanged(const QString &)),this,SLOT(onRefFileTextChanged(const QString &)));
  
}
void GTAActionFTAKinematicMultiOutputWidget::onRefFileTextChanged(const QString &)
{
    _RefFileData.clear();
    if(!ui->RefFilePathLE->text().isEmpty())
    {
        GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
        bool rc =  pAppCtrl->parseKinematicReferenceFile(ui->RefFilePathLE->text(),_RefFileData);
        if(rc)
        {
            ui->NumLinesLE->setText(QString::number(_RefFileData.count()));
        }
        else
            QMessageBox::information(this,"Invalid input","Reference file is not valid.");
    }
}
GTAActionFTAKinematicMultiOutputWidget::~GTAActionFTAKinematicMultiOutputWidget()
{
    delete ui;
}

void GTAActionFTAKinematicMultiOutputWidget::clear()
{
    ui->InputParam1LE->clear();
    ui->InputParam2LE->clear();
    ui->InputParam3LE->clear();

    ui->OutputParam1LE->clear();
    ui->OutputParam2LE->clear();
    ui->OutputParam3LE->clear();
    ui->OutputParam4LE->clear();
    ui->OutputParam5LE->clear();

    ui->Epsilon2LE->clear();
    ui->Epsilon1LE->clear();
    ui->NumLinesLE->clear();
    ui->WaitPilOrderLE->clear();
    ui->NameLE->clear();


    ui->RefFilePathLE->clear();

}
bool GTAActionFTAKinematicMultiOutputWidget::isValid()const
{
    bool rc = false;
    bool isInputPresent = false;
    bool isOutputPresent = false;
    rc = !ui->Epsilon1LE->text().isEmpty();
    rc = rc & !ui->Epsilon2LE->text().isEmpty();
    rc = rc & !ui->NumLinesLE->text().isEmpty();
    rc = rc & !ui->WaitPilOrderLE->text().isEmpty();
    rc = rc & !ui->RefFilePathLE->text().isEmpty();
    rc = rc & !ui->NameLE->text().isEmpty();
    
    isInputPresent = isInputPresent | !ui->InputParam1LE->text().isEmpty();
    isInputPresent = isInputPresent | !ui->InputParam2LE->text().isEmpty();
    isInputPresent = isInputPresent | !ui->InputParam3LE->text().isEmpty();


    isOutputPresent = isOutputPresent | !ui->OutputParam1LE->text().isEmpty();
    isOutputPresent = isOutputPresent | !ui->OutputParam2LE->text().isEmpty();
    isOutputPresent = isOutputPresent | !ui->OutputParam3LE->text().isEmpty();
    isOutputPresent = isOutputPresent | !ui->OutputParam4LE->text().isEmpty();
    isOutputPresent = isOutputPresent | !ui->OutputParam5LE->text().isEmpty();




    GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();

    _RefFileData.clear();
    if(rc && !ui->RefFilePathLE->text().isEmpty())
    {
        rc =  rc & pAppCtrl->parseKinematicReferenceFile(ui->RefFilePathLE->text(),_RefFileData);
        if(QString::number(_RefFileData.count()) != ui->NumLinesLE->text())
        {
            _LastError = "Number of line mismatch with reference file data\n";
           rc = false;
        }
    }

    rc = rc & isInputPresent & isOutputPresent;

    return rc;
}

bool GTAActionFTAKinematicMultiOutputWidget::getActionCommand(GTAActionBase *& pCmd)const
{
    bool rc = false;

    if(isValid())
    {
        rc =true;

        GTAActionFTAKinematicMultiOutput *pKinematicCmd = new GTAActionFTAKinematicMultiOutput();
        pKinematicCmd->setWaitPilOrder(ui->WaitPilOrderLE->text());
        pKinematicCmd->setEpsilon1(ui->Epsilon1LE->text());
        pKinematicCmd->setEpsilon2(ui->Epsilon2LE->text());
        pKinematicCmd->setNoOfLines(ui->NumLinesLE->text());

        QString Var  = ui->InputParam1LE->text();
        pKinematicCmd->insertInputVariable(0,Var);

        Var  = ui->InputParam2LE->text();
        pKinematicCmd->insertInputVariable(1,Var);

        QString iTag = ui->NumLinesLbl->text();
        QString iVal = ui->NumLinesLE->text();
        pKinematicCmd->insertTagValuepairForTemplates(iTag,iVal);
        

        iTag = ui->WaitPilOrderLbl->text();
        iVal = ui->WaitPilOrderLE->text();
        pKinematicCmd->insertTagValuepairForTemplates(iTag,iVal);

        iTag = ui->Epsilon1Lbl->text();
        iVal = ui->Epsilon1LE->text();
        pKinematicCmd->insertTagValuepairForTemplates(iTag,iVal);

        iTag = ui->Epsilon2Lbl->text();
        iVal = ui->Epsilon2LE->text();
        pKinematicCmd->insertTagValuepairForTemplates(iTag,iVal);



        Var  = ui->InputParam3LE->text();
        pKinematicCmd->insertInputVariable(2,Var);


        Var  = ui->OutputParam1LE->text();
        pKinematicCmd->insertMonitoredVariable(0,Var);

        Var  = ui->OutputParam2LE->text();
        pKinematicCmd->insertMonitoredVariable(1,Var);


        Var  = ui->OutputParam3LE->text();
        pKinematicCmd->insertMonitoredVariable(2,Var);

        Var  = ui->OutputParam4LE->text();
        pKinematicCmd->insertMonitoredVariable(3,Var);  
        
        
        Var  = ui->OutputParam5LE->text();
        pKinematicCmd->insertMonitoredVariable(4,Var);

        pKinematicCmd->setCurrentName(ui->NameLE->text());

        pKinematicCmd->setRefFileData(_RefFileData);
     
        pKinematicCmd->setRefFilePath(ui->RefFilePathLE->text());
        pCmd = pKinematicCmd;
    }
    return rc;
}

void GTAActionFTAKinematicMultiOutputWidget::setActionCommand(const GTAActionBase * pCmd)
{
    

    if(pCmd!= NULL)
    {
        
        GTAActionFTAKinematicMultiOutput *pKinematicCmd = dynamic_cast<GTAActionFTAKinematicMultiOutput* >((GTAActionBase*)pCmd);
        ui->WaitPilOrderLE->setText(pKinematicCmd->getWaitPilOrder());
        ui->Epsilon1LE->setText(pKinematicCmd->getEpsilon1());
        ui->Epsilon2LE->setText(pKinematicCmd->getEpsilon2());
        ui->NumLinesLE->setText(pKinematicCmd->getNoOfLines());


        QStringList sInputItems = pKinematicCmd->getInputVariableList();
        for(int i=0;i<sInputItems.count();i++)
        {
            QString sVar = pKinematicCmd->getInputVariable(i);
            switch(i)
            {
                case 0:ui->InputParam1LE->setText(sVar);break;
                case 1:ui->InputParam2LE->setText(sVar);break;
                case 2:ui->InputParam3LE->setText(sVar);break;
            }
        }

        QStringList sOutputItems = pKinematicCmd->getOutputVariableList();
        for(int i=0;i<sOutputItems.count();i++)
        {
            QString sVar = pKinematicCmd->getOutputVariable(i);
            switch(i)
            {
                case 0:ui->OutputParam1LE->setText(sVar);break;
                case 1:ui->OutputParam2LE->setText(sVar);break;
                case 2:ui->OutputParam3LE->setText(sVar);break;
                case 3:ui->OutputParam4LE->setText(sVar);break;
                case 4:ui->OutputParam5LE->setText(sVar);break;
            }
        }
        ui->RefFilePathLE->setText(pKinematicCmd->getRefFilePath());
        _RefFileData = pKinematicCmd->getRefFileData();
        ui->NameLE->setText(pKinematicCmd->getCurrentName());
       
    }
   

}
void GTAActionFTAKinematicMultiOutputWidget::onSearchPBClicked()
{

    QObject* pSender = sender();
    QToolButton* pToolButton = dynamic_cast<QToolButton*> (pSender);

    if (pToolButton!=NULL && _hshToolButtonLE.contains(pToolButton))
    { 
        QLineEdit* pLineEdit = _hshToolButtonLE.value(pToolButton);
        emit searchObject(pLineEdit);
    }
   
}

void GTAActionFTAKinematicMultiOutputWidget::onBrowseRefFilePBClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"Select Reference File",QDir::currentPath(),"*.csv");
    if (!filePath.isEmpty())
        ui->RefFilePathLE->setText(filePath);
}
#include "GTAGenSearchWidget.h"
int GTAActionFTAKinematicMultiOutputWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::PARAMETER;
    return retVal;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_FCTL,COM_FCTL_KINEMATIC_MULTI_OUTPUT),GTAActionFTAKinematicMultiOutputWidget,obj)