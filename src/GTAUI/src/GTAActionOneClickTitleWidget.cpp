#include "GTAActionOneClickTitleWidget.h"
#include "ui_GTAActionOneClickTitleWidget.h"
#include "GTAParamValidator.h"
#include "GTAUtil.h"
#include "GTAGenSearchWidget.h"
GTAActionOneClickTitleWidget::GTAActionOneClickTitleWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionOneClickTitleWidget)
{
    ui->setupUi(this);

    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    //connect(ui->CommentLE,SIGNAL(textChanged()),this,SLOT(validateText()));
}

GTAActionOneClickTitleWidget::~GTAActionOneClickTitleWidget()
{
    delete ui;
}

void GTAActionOneClickTitleWidget::clear()
{
    ui->TitleLE->clear();
    ui->CommentLE->clear();
    ui->ImageNameLE->clear();
}

void GTAActionOneClickTitleWidget::setActionCommand(const GTAActionBase* ipActionCmd)
{
    clear();
    
    GTAActionOneClickTitle* pOneClickTitle= dynamic_cast<GTAActionOneClickTitle*>((GTAActionBase*)ipActionCmd);
	if(pOneClickTitle != NULL)
    {
        ui->TitleLE->setText(pOneClickTitle->getTitle());
        ui->CommentLE->setText(pOneClickTitle->getComment());
        ui->ImageNameLE->setText(pOneClickTitle->getImageName());
    }
}

bool GTAActionOneClickTitleWidget::getActionCommand(GTAActionBase*& opActionCmd)const
{
    

    bool rc = false;
    GTAActionOneClickTitle * pActionCmd =  NULL;
    if(isValid())
    {
        QString title = ui->TitleLE->text();
        QString comment = ui->CommentLE->toPlainText();
        QString image = ui->ImageNameLE->text();

        pActionCmd = new GTAActionOneClickTitle(ACT_ONECLICK,COM_ONECLICK_TITLE);
        pActionCmd->setTitle(title);
        pActionCmd->setComment(comment);
        pActionCmd->setImageName(image);
        rc = true;
    }
    opActionCmd=pActionCmd;   
	return rc;
}
bool GTAActionOneClickTitleWidget::isValid()const
{
    QString title = ui->TitleLE->text();
    QString comment = ui->CommentLE->toPlainText();
    QString image = ui->ImageNameLE->text();
    if(title.isEmpty() && comment.isEmpty() && image.isEmpty())
        return false;

    return true;
}

void GTAActionOneClickTitleWidget::onSearchPBClicked()
{
    emit searchImage(ui->ImageNameLE);
}
void GTAActionOneClickTitleWidget::validateText()
{
    GTAParamValidator txtValidatorObj;
    QString currentText = ui->CommentLE->toPlainText();
    int txtSize = currentText.size();
    if (txtSize !=0 && txtValidatorObj.validate(currentText,txtSize) != QValidator::Acceptable)
    {

        ui->CommentLE->setText(currentText.remove(txtSize-1,1));
        ui->CommentLE->moveCursor(QTextCursor::End);

    }
}
int GTAActionOneClickTitleWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::IMAGE;
    return retVal;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_ONECLICK,COM_ONECLICK_TITLE),GTAActionOneClickTitleWidget,obj)
