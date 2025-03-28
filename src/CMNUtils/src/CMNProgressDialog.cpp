#include "AINCMNProgressDialog.h"
using namespace AINCMN::Dialogs;

AINCMNProgressDialog::AINCMNProgressDialog(QString title, int minRange, int maxRange, bool allowedToAbort)
{
	QLabel * textLabel = new QLabel(title);
	textLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	setLabel(textLabel);
	
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/img/Icon"), QSize(), QIcon::Normal, QIcon::Off);
	setWindowIcon(icon);
	
	if(allowedToAbort)
	{
		setCancelButtonText("Abort");
		setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	}
	else{
		setCancelButton(0);
		setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowTitleHint );
	}
	
	setWindowTitle(title + ": Please wait...");
	setWindowModality(Qt::ApplicationModal);
	QProgressBar * bar = new QProgressBar(this);
	bar->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	setBar(bar);
	setRange(minRange, maxRange);
	setMinimumDuration(2000);
	//show();
}

AINCMNProgressDialog::~AINCMNProgressDialog() 
{
	close();
}

void AINCMNProgressDialog::setProBarLabel(const QString& text)
{
	QLabel * label = new QLabel(text);
	label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	setLabel(label);
}

