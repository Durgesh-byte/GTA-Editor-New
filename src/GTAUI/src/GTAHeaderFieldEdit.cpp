#include "GTAHeaderFieldEdit.h"
#include "ui_GTAHeaderFieldEdit.h"
#include <QTableWidgetItem>
#include "GTACommandBase.h"
#include "GTAAppController.h"
#include "GTAActionCall.h"
GTAHeaderFieldEdit::GTAHeaderFieldEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTAHeaderFieldEdit)
{
    ui->setupUi(this);
    _mode=mode::ADDMODE;
   // connect(this,SIGNAL(accept()),this,SLOT(addFieldEmit()));
}

GTAHeaderFieldEdit::~GTAHeaderFieldEdit()
{
    delete ui;
} 
QString GTAHeaderFieldEdit::getFieldName()const
{
    return ui->fieldNameLE->text();

}
QString GTAHeaderFieldEdit::getFieldVal()const
{
    return ui->fieldValueLE->text();
}
QString GTAHeaderFieldEdit::getIsMandatory()const
{
    return ui->mandatoryCB->currentText();
}
QString GTAHeaderFieldEdit::getFieldDescription()const
{
    return ui->descriptionTE->toPlainText();
}

/**
 * @brief get the showLTRA value from dialog
 * @return bool : true if field will be displayed in LTRA
*/
bool GTAHeaderFieldEdit::getShowInLTRA()const
{
    return _showInLTRA;
}

void GTAHeaderFieldEdit::accept()
{
    if (_mode==mode::ADDMODE)
        emit signalAddField();
    else
        emit signalEditField();
}
void GTAHeaderFieldEdit::clear()
{
    ui->fieldNameLE->clear();
    ui->fieldValueLE->clear();
    ui->mandatoryCB->setCurrentIndex(0);
    ui->descriptionTE->clear();
}
void GTAHeaderFieldEdit::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}
void GTAHeaderFieldEdit::setMode(mode iMode)
 {
     _mode =iMode;
 }

void GTAHeaderFieldEdit::show()
{
    if (_mode==ADDMODE)
    {
        this->setWindowTitle("Add Header Field");
    }
    else
        this->setWindowTitle("Edit Header Field");

    QDialog::show();
}
void GTAHeaderFieldEdit::setHeaderInfo(const QString& iFieldName,const QString& iFieldVal,QString& isMandatory,QString& iDescription, bool& iShowInLTRA)
{

    ui->fieldNameLE->setText(iFieldName);
    ui->fieldValueLE->setText(iFieldVal);

    int mandatoryFieldIndex = ui->mandatoryCB->findText(isMandatory);
    if (mandatoryFieldIndex!=-1)
        ui->mandatoryCB->setCurrentIndex(mandatoryFieldIndex);
   
    ui->descriptionTE->setPlainText(iDescription);
    _showInLTRA = iShowInLTRA;
}