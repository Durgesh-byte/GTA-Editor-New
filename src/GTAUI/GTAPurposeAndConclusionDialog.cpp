#include "GTAPurposeAndConclusionDialog.h"

GTAPurposeAndConclusionDialog::GTAPurposeAndConclusionDialog(GTAEditorWindow* editorParent, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	_editorParent = editorParent;
	connect(ui.OKBtn, SIGNAL(clicked()), this, SLOT(onOkPBClicked()));
	connect(ui.CancelBtn, SIGNAL(clicked()), this, SLOT(close()));
}

GTAPurposeAndConclusionDialog::~GTAPurposeAndConclusionDialog()
{}

/**
 * @brief slot function trigger when ok button is clicked
*/
void GTAPurposeAndConclusionDialog::onOkPBClicked()
{
	QString purpose = ui.PurposeLE->toPlainText();
	QString conclusion = ui.ConclusionLE->toPlainText();

	_editorParent->setPurposeAndConclusionToElement(purpose, conclusion);
	_editorParent->onSave();
	close();
}

/**
 * @brief Set the purpose to text box
 * @param purpose - saved value of purpose from file
*/
void GTAPurposeAndConclusionDialog::setPurpose(const QString& purpose)
{
	ui.PurposeLE->setText(purpose);
}

/**
 * @brief Set the conclusion to text box
 * @param conclusion - saved value of conclusion from file
*/
void GTAPurposeAndConclusionDialog::setConclusion(const QString& conclusion)
{
	ui.ConclusionLE->setText(conclusion);
}
