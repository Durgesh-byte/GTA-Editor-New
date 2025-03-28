#include "GTAActionCmdCommentWidget.h"

#pragma warning (push,0)
#include "ui_GTAActionCmdCommentWidget.h"
#pragma warning (pop)

GTAActionCmdCommentWidget::GTAActionCmdCommentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTAActionCmdCommentWidget)
{
    ui->setupUi(this);

    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(okClicked()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(hide()));
    connect(ui->CommentLE,SIGNAL(textChanged()),this,SLOT(validateText()));
}

GTAActionCmdCommentWidget::~GTAActionCmdCommentWidget()
{
    delete ui;
}
void GTAActionCmdCommentWidget::clear()
{
    ui->CommentLE->clear();
    ui->ImageNameLE->clear();
    _comment.clear();
}
void GTAActionCmdCommentWidget::onSearchPBClicked()
{
    emit searchImage(ui->ImageNameLE);
}
void GTAActionCmdCommentWidget::showImagePanel(bool iVal)
{
    ui->ImageNameLE->clear();
    ui->label_3->hide();
    ui->ImageNameLE->hide();
    ui->SearchPB->hide();
    if(iVal)
    {
        ui->label_3->show();
        ui->ImageNameLE->show();
        ui->SearchPB->show();
    }
}
void GTAActionCmdCommentWidget::setComment(const QString & iComment)
{
    ui->CommentLE->setText(iComment);
    _comment=iComment;
}

void GTAActionCmdCommentWidget::setImageName(const QString & iImgName)
{
    ui->ImageNameLE->setText(iImgName);
}

QString GTAActionCmdCommentWidget::getComment() const
{
    return ui->CommentLE->toPlainText();
}
QString GTAActionCmdCommentWidget::getImageName() const
{
    return ui->ImageNameLE->text();
}

void GTAActionCmdCommentWidget::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}

void GTAActionCmdCommentWidget::okClicked()
{
    _comment = ui->CommentLE->toPlainText();
    this->hide();
}
void GTAActionCmdCommentWidget::show()
{
    ui->CommentLE->setText(_comment);
    this->QWidget::show();


}

void GTAActionCmdCommentWidget::validateText()
{
    
    QString currentText = ui->CommentLE->toPlainText();
    int txtSize = currentText.size();
    if (txtSize !=0 && (currentText.endsWith("'") || currentText.endsWith("\"")))
    {
        currentText = currentText.replace("\'","");
        currentText = currentText.replace("\"","");
        ui->CommentLE->setText(currentText);
        ui->CommentLE->moveCursor(QTextCursor::End);
    }
}