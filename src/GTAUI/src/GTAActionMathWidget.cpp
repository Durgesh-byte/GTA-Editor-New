#include "GTAActionMathWidget.h"
#include "ui_GTAActionMathWidget.h"
#include "GTAUtil.h"

GTAActionMathWidget::GTAActionMathWidget(QWidget *parent) :GTAActionWidgetInterface(parent),
ui(new Ui::GTAActionMathWidget)
{
    ui->setupUi(this);
    connect(ui->param1SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->param2SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->LocationSearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->OperatorCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeNoteText(QString)));
}

GTAActionMathWidget::~GTAActionMathWidget()
{
    delete ui;
}
void GTAActionMathWidget::setComplement(const QString &compVal)
{

    _isType = compVal;
    if (compVal == COM_MATHS_POW)
    {

        ui->Parameter1Frame->show();
        ui->parameter2Frame->show();
        ui->locationFrame->show();
        ui->powNoteLB->show();


        ui->operatorFrame->hide();        
        ui->absNoteLB->hide();
        ui->incNoteLB->hide();
        ui->binaryNoteFrame->hide();


    }
    else if (compVal == COM_MATHS_BINARY)
    {

        ui->Parameter1Frame->show();
        ui->parameter2Frame->show();
        ui->locationFrame->show();
        ui->operatorFrame->show();  
        ui->binaryNoteFrame->show();


        ui->absNoteLB->hide();
        ui->incNoteLB->hide();
        ui->powNoteLB->hide();




    }
    else if (compVal == COM_MATHS_ABS)
    {

        ui->Parameter1Frame->show();
        ui->parameter2Frame->hide();
        ui->locationFrame->show();
        ui->operatorFrame->hide();  
        ui->binaryNoteFrame->hide();


        ui->absNoteLB->show();
        ui->incNoteLB->hide();
        ui->powNoteLB->hide();

    }
    else if (compVal == COM_MATHS_INCREMENT)
    {

        ui->Parameter1Frame->show();
        ui->incNoteLB->show();

        ui->parameter2Frame->hide();
        ui->locationFrame->hide();
        ui->operatorFrame->hide();  
        ui->binaryNoteFrame->hide();
        ui->absNoteLB->hide();
        ui->powNoteLB->hide();

    }

}
void GTAActionMathWidget::clear()
{
    ui->parameter1LE->clear();
    ui->Parameter2LE->clear();
    ui->LocationLE->clear();

}
bool GTAActionMathWidget::isValid()const
{
    bool isParam1Empty =    ui->parameter1LE->text().isEmpty();
    bool isParam2Empty =    ui->Parameter2LE->text().isEmpty();
    bool isLocationEmpty =  ui->LocationLE->text().isEmpty();

    if (isParam1Empty)
        return false;

    if (_isType == COM_MATHS_BINARY || _isType == COM_MATHS_POW)
    {
        if (isParam2Empty || isLocationEmpty)
            return false;


    }


    return true;
}
bool GTAActionMathWidget::getActionCommand(GTAActionBase *& opCmd)const
{
    bool rc = false;
    opCmd = NULL;
    if(isValid())
    {

        rc =true;
        GTAActionMath *pMathsCmd = new GTAActionMath();
        pMathsCmd->setFirstVariable(ui->parameter1LE->text());
        pMathsCmd->setSecondVariable(ui->Parameter2LE->text());
        pMathsCmd->setLocation(ui->LocationLE->text());
        pMathsCmd->setOperator(ui->OperatorCB->currentText());
        opCmd = pMathsCmd;
    }
    return rc;
}
void GTAActionMathWidget::setActionCommand(const GTAActionBase * pCmd)
{
   if(pCmd != NULL)
    {
       
        GTAActionMath * pMathCmd =dynamic_cast<GTAActionMath*>((GTAActionBase*)pCmd);
        ui->parameter1LE->setText(pMathCmd->getFirstVariable());
        ui->Parameter2LE->setText(pMathCmd->getSecondVariable());
        ui->LocationLE->setText(pMathCmd->getLocation());
        int idx = ui->OperatorCB->findText(pMathCmd->getOperator());
        if(idx>-1)
            ui->OperatorCB->setCurrentIndex(idx);
    }
    
}
void GTAActionMathWidget::onSearchPBClicked()
{
    QToolButton * pTB = dynamic_cast<QToolButton*>(sender());
    if(pTB == ui->param1SearchPB)
        emit searchObject(ui->parameter1LE);
    if(pTB == ui->param2SearchPB)
        emit searchObject(ui->Parameter2LE);
    if(pTB == ui->LocationSearchPB)
        emit searchObject(ui->LocationLE);
}


void GTAActionMathWidget::changeNoteText(QString iOperator)
{
    QString txt = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Cambria Math'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'MS Shell Dlg 2'; font-size:16pt; font-weight:600; font-style:italic;\">####</span></p></body></html>";
    txt.replace("####",iOperator);

    ui->bin2NoteLB->setTextFormat(Qt::RichText);
    ui->bin2NoteLB->setText(txt);


}
#include "GTAGenSearchWidget.h"
int GTAActionMathWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::PARAMETER;
    return retVal;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_MATH,GTAActionMathWidget,obj)
