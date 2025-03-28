#pragma once

#include <QDialog>
#include "ui_GTAPurposeAndConclusionDialog.h"
#include "GTAEditorWindow.h"

class GTAPurposeAndConclusionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GTAPurposeAndConclusionDialog(GTAEditorWindow* editorParent, QWidget *parent = nullptr);
	~GTAPurposeAndConclusionDialog();

	void setPurpose(const QString& purpose);
	void setConclusion(const QString& conclusion);

private:
	Ui::GTAPurposeAndConclusionDialogClass ui;

private slots:
	void onOkPBClicked();

private:
	GTAEditorWindow* _editorParent;
};
