#include "GTAProgress.h"
#include "ui_GTAProgress.h"

GTAProgress::GTAProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTAProgress)
{
    ui->setupUi(this);
    setVisible(false);
    setWindowFlags(Qt::CustomizeWindowHint|Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    this->setFixedSize(this->width(),this->height());
}

GTAProgress::~GTAProgress()
{
    delete ui;
}

void GTAProgress::setPosition(const QRect & iRect)
{
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignTop, size(), iRect));
}

void GTAProgress::setLabelText(const QString &iText)
{
    ui->label->setText(iText);
}

void GTAProgress::startProgressBar()
{
    setVisible(true);
}

void GTAProgress::GTAProgress::onStopProgressBar()
{
    setVisible(false);
    emit stopProgressBar();
}
