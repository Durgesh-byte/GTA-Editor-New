#include "GTAActionTitleWidget.h"
#include "ui_GTAActionTitleWidget.h"
#include "GTAParamValidator.h"
#include "GTAGenSearchWidget.h"
GTAActionTitleWidget::GTAActionTitleWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionTitleWidget)
{
    ui->setupUi(this);

    ui->ImageNameLE->setVisible(false);
    ui->label_3->setVisible(false);
    ui->SearchPB->setVisible(false);

    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    GTAParamValidator* pValidator2 = new GTAParamValidator(ui->TitleLE);
    ui->TitleLE->setValidator(pValidator2);
    connect(ui->CommentLE,SIGNAL(textChanged()),this,SLOT(validateText()));
}

GTAActionTitleWidget::~GTAActionTitleWidget()
{
    delete ui;
}

void GTAActionTitleWidget::clear()
{
    ui->TitleLE->clear();
    ui->CommentLE->clear();
    ui->ImageNameLE->clear();
}

void GTAActionTitleWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
	clear();
    GTAActionTitle * pTitleCmd = dynamic_cast<GTAActionTitle *>((GTAActionBase*)ipActionCmd);
    if(pTitleCmd != NULL)
    {
        ui->TitleLE->setText(pTitleCmd->getTitle());
        ui->CommentLE->setText(pTitleCmd->getComment());
        ui->ImageNameLE->setText(pTitleCmd->getImageName());
    }
}

bool GTAActionTitleWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    QString title = ui->TitleLE->text();
    QString comment = ui->CommentLE->toPlainText();
    QString image = ui->ImageNameLE->text();
    bool rc=false;

    GTAActionTitle * pActionCmd =  NULL;
    if(isValid())
    {
        pActionCmd = new GTAActionTitle(QString());
        pActionCmd->setTitle(title);
        pActionCmd->setComment(comment);
        pActionCmd->setImageName(image);
        rc=true;
    }
    opCmd = pActionCmd;
	return rc;
}
bool GTAActionTitleWidget::isValid()const
{
    QString title = ui->TitleLE->text();
    QString comment = ui->CommentLE->toPlainText();
    QString image = ui->ImageNameLE->text();
    if(title.isEmpty() )
        return false;
   /* if (comment.isEmpty() && image.isEmpty())
        return false;*/

    return true;
}

void GTAActionTitleWidget::onSearchPBClicked()
{
    emit searchObject(ui->ImageNameLE);
}
void GTAActionTitleWidget::validateText()
{
    GTAParamValidator txtValidatorObj;
    QString currentText = ui->CommentLE->toPlainText();
    int txtSize = currentText.size();
    if (txtSize > 500)
    {
        ui->CommentLE->clear();
        ui->CommentLE->setToolTip("Only 500 characters allowed");
        return;
    }
    if (txtSize !=0 && txtValidatorObj.validate(currentText, txtSize) != QValidator::Acceptable)
    {
        // Get the forbidden character
        QTextCursor cursor = ui->CommentLE->textCursor();
        const int pos = cursor.position();
        const QString forbiddenChar = ui->CommentLE->toPlainText()[pos - 1];

        // Remove the forbidden character
        ui->CommentLE->setText(currentText.remove(pos - 1, 1));

        // Set cursor to last edited position
        ui->CommentLE->moveCursor(QTextCursor::End);              
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, txtSize - pos);
        ui->CommentLE->setTextCursor(cursor);
        
        // Inform the user 
        ui->CommentLE->setToolTip(forbiddenChar + " is a forbidden character and has been removed");
    }
}
int GTAActionTitleWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::IMAGE;
    return retVal;
}

bool GTAActionTitleWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    for (auto param : params)
    {
        if (param.first == "TitleLE")
        {
            if (param.second != "") {
                ui->TitleLE->setText(param.second);
            }
            else {
                errorLogs.push_back({ "Value for TitleLE is empty" });
                return false;
            }
        }
        else if (param.first == "CommentLE")
        {
            if (param.second != "") {
                ui->CommentLE->setText(param.second);
            }
        }
    }

    return true;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_TITLE,GTAActionTitleWidget,obj)
