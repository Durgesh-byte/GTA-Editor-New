#include "GTACommentImageWidget.h"
#include "ui_GTACommentImageWidget.h"

GTACommentImageWidget::GTACommentImageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTACommentImageWidget)
{
    ui->setupUi(this);
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
}

GTACommentImageWidget::~GTACommentImageWidget()
{
    delete ui;
}
QString GTACommentImageWidget::getImageFileName() const
{
    return ui->ImageNameLE->text();
}
QString GTACommentImageWidget::getImageFullFilePath() const
{
    return QString();
}
void GTACommentImageWidget::setImageFileName(const QString & iFileName)
{
    ui->ImageNameLE->setText(iFileName);
}

void GTACommentImageWidget::setImageFullFilePath(const QString &)
{

}

void GTACommentImageWidget::setPreview(const QString & iImagePath)
{
    QPixmap image(iImagePath);
    ui->ImagePreviewLabel->setPixmap(image);
}
void GTACommentImageWidget::onSearchPBClicked()
{
    emit searchImage(ui->ImageNameLE);
}
