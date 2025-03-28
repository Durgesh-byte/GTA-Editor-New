#include "GTAActPrintMsgWidget.h"
#include "GTAActionPrint.h"
#include "GTAUtil.h"
#include "GTAParamValidator.h"
#include "GTARestrictedTextEdit.h"
GTAActPrintMsgWidget::GTAActPrintMsgWidget(QWidget *parent) :
GTAActionWidgetInterface(parent)
{
    _pBoxLayout = new QVBoxLayout(this);
    _pLineEdit = new GTARestrictedTextEdit(this);
    _pLineEdit->setToolTip("Enter message in this text editor");
    _pBoxLayout->addWidget(_pLineEdit);
    connect(_pLineEdit,SIGNAL(textChanged()),this,SLOT(validateText()));
}
GTAActPrintMsgWidget::~GTAActPrintMsgWidget()
{

}

void GTAActPrintMsgWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
    GTAActionPrint* pPrntMsg = dynamic_cast<GTAActionPrint*>((GTAActionBase *)ipActionCmd);
    if (NULL!=pPrntMsg)
    {
        _pLineEdit->setText(pPrntMsg->getValues().at(0));
    }
}
bool GTAActPrintMsgWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false; 
    GTAActionPrint *pPrintActionCmd = NULL;
    QString text = _pLineEdit->toPlainText();
    text.replace("\""," ");
    text.replace("'"," ");

    if(!text.isEmpty())
    {
        rc = true;
        pPrintActionCmd = new GTAActionPrint(ACT_PRINT,COM_PRINT_MSG);
        pPrintActionCmd->setPrintType(GTAActionPrint::MESSAGE);
        pPrintActionCmd->setValues(QStringList()<<text);
    }
   
    
    opCmd = pPrintActionCmd;
    
   
   
    return rc;

}
void GTAActPrintMsgWidget::clear() 
{
    _pLineEdit->clear();
}
bool GTAActPrintMsgWidget::isValid()const 
{
    QString text = _pLineEdit->toPlainText();

    if(!text.isEmpty())
        return true;

    return false;
}
void GTAActPrintMsgWidget::validateText()
{
    QString currentText = _pLineEdit->toPlainText();
    int txtSize = currentText.size();
    if (txtSize > 500)
    {
        _pLineEdit->clear();
        _pLineEdit->setToolTip("Only 500 characters allowed");
        return;
    }
    QRegExp rx("([A-Za-z0-9.?\\\\@#$_:=\\-<>\\s]+)");
    QRegExpValidator validator(rx, 0);
    QValidator::State rc =  validator.validate(currentText, txtSize);
    if(txtSize !=0 && rc != QValidator::Acceptable)
    {
        // Get the forbidden character
        QTextCursor cursor = _pLineEdit->textCursor();
        const int pos = cursor.position();
        const QString forbiddenChar = _pLineEdit->toPlainText()[pos - 1];

        // Remove the forbidden character
        _pLineEdit->setText(currentText.remove(pos - 1, 1));

        // Set cursor to last edited position
        _pLineEdit->moveCursor(QTextCursor::End);
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, txtSize - pos);
        _pLineEdit->setTextCursor(cursor);

        // Inform the user 
        _pLineEdit->setToolTip(forbiddenChar + " is a forbidden character and has been removed");
    }
    else
        _pLineEdit->setToolTip("");
}

bool GTAActPrintMsgWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    if (params.size() == 1 && params.at(0).first == "_pLineEdit")
    {
        _pLineEdit->setText(params.at(0).second);
        return true;
    }
    else {
        if (params.size() != 1)
            errorLogs.push_back({ QString("Expected print msg params size 1, got %").arg(params.size()) });
        if (params.at(0).first == "_pLineEdit")
            errorLogs.push_back({ QString("First param not expected to be:  %").arg(params.at(0).first) });

        return false;
    }
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_PRINT,COM_PRINT_MSG),GTAActPrintMsgWidget,obj)
