#include "GTAActionLineEditor.h"
#include "ui_GTAActionLineEditor.h"
#include "GTAParamValidator.h"

GTAActionLineEditor::GTAActionLineEditor(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionLineEditor)
{
    ui->setupUi(this);

    showMessageTypeCB(false);
    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->LineEdtorLE);
    ui->LineEdtorLE->setValidator(pValidator1);
}

GTAActionLineEditor::~GTAActionLineEditor()
{
    delete ui;
}

void GTAActionLineEditor::clear()
{
    ui->LineEdtorLE->clear();
}

bool GTAActionLineEditor::isValid()
{
    return ui->LineEdtorLE->text().isEmpty() ? false : true;
}

void GTAActionLineEditor::setLabel(const QString & iVal)
{
    ui->LineEditLabel->setText(iVal);
}

void GTAActionLineEditor::setText(const QString & iVal)
{
    ui->LineEdtorLE->setText(iVal);
}

void GTAActionLineEditor::setSuffiexLabel(const QString & iVal)
{
    ui->LineEditSuffixLabel->setText(iVal);
}


QString GTAActionLineEditor::getText() const
{
    return ui->LineEdtorLE->text();
}

void GTAActionLineEditor::showMessageTypeCB(bool iVal)
{
    ui->MessageTypeCB->setVisible(iVal);
    ui->MessageTypeLabel->setVisible(iVal);
}
void GTAActionLineEditor::setMessageTypeList(const QStringList & iMsgTypes)
{
    ui->MessageTypeCB->addItems(iMsgTypes);
}
void GTAActionLineEditor::setCurrentMessageType(const QString & iMsg)
{
    int index =  ui->MessageTypeCB->findText(iMsg);
    ui->MessageTypeCB->setCurrentIndex(index);
}
QString GTAActionLineEditor::getCurrentMessageType() const
{
    return ui->MessageTypeCB->currentText();
}
void GTAActionLineEditor::setValidator(QValidator* pVAlidator)
{
    if (pVAlidator!=NULL)
    {
        ui->LineEdtorLE->setValidator(pVAlidator);
        pVAlidator->setParent(ui->LineEdtorLE);
    }
    

}  
void GTAActionLineEditor:: clearValidator()
{
    ui->LineEdtorLE->setValidator(0);
}   