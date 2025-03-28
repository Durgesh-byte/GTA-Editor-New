#include "GTASaveEditorWidget.h"
#include "ui_GTASaveEditorWidget.h"
GTASaveEditorWidget::GTASaveEditorWidget(QDialog *parent) : QDialog(parent),
    ui(new Ui::GTASaveEditorWidget)
{
    ui->setupUi(this);
	connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(onSaveAccepted(void)));
}
GTASaveEditorWidget::~GTASaveEditorWidget()
{
	delete ui;
}

void GTASaveEditorWidget::setSaveName(const QString& isName)
{
	ui->fileNameLineEdit->setText(isName);

}
void GTASaveEditorWidget::onSaveAccepted()
{
	emit  startSaveDocument();
}