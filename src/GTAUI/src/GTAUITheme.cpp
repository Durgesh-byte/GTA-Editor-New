#include "GTAUITheme.h"
#include "ui_GTAUITheme.h"
#include "GTAUserSettings.h"
#include "GTAUtil.h"
GTAUITheme::GTAUITheme(QWidget *parent) :
QWidget(parent),
ui(new Ui::GTAUITheme)
{
	ui->setupUi(this);

	QList<QComboBox*> comboBoxList;
	comboBoxList<<ui->BGColorCB<<ui->WindowTxtColorCB<<ui->ButtonTextColorCB<<ui->UserTextColorCB;
	const QStringList colorNames = QColor::colorNames();
	int index = 0;
	foreach(QString colorName, colorNames)
	{
		QColor objColor(colorName);

		for(int i = 0; i < comboBoxList.count(); i++)
		{
			comboBoxList.at(i)->addItem(colorName,objColor);
			const QModelIndex idx = comboBoxList.at(i)->model()->index(index++,0);
			comboBoxList.at(i)->model()->setData(idx,objColor,Qt::BackgroundColorRole);
		}
	}
	setDefualt();
	connect(ui->BGColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorChange(int)));
	connect(ui->WindowTxtColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorChange(int)));
	connect(ui->ButtonTextColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorChange(int)));
	connect(ui->UserTextColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorChange(int)));
	connect(ui->ResetPB,SIGNAL(clicked()),this,SLOT(setDefualt()));
	connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKClicked()));
	connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(onCancelClicked()));

     loadTheme();
}

GTAUITheme::~GTAUITheme()
{
	delete ui;
}
QColor GTAUITheme::getBackGroundColor() const
{
	return QColor(ui->BGColorCB->currentText());
}
QColor GTAUITheme::getWindowTextColor() const
{
	return QColor(ui->WindowTxtColorCB->currentText());
}
QColor GTAUITheme::getButtonTextColor() const
{
	return QColor(ui->ButtonTextColorCB->currentText());
}
QColor GTAUITheme::getUserTextColor() const
{
	return QColor(ui->UserTextColorCB->currentText());
}
void GTAUITheme::onColorChange(const int & )
{
	QColor background(ui->BGColorCB->currentText());
	QColor windowText(ui->WindowTxtColorCB->currentText());
	QColor buttonText(ui->ButtonTextColorCB->currentText());
	QColor userText(ui->UserTextColorCB->currentText());

	QPalette pallate = qApp->palette();
	pallate.setColor(QPalette::Background,background);
	pallate.setColor(QPalette::WindowText,windowText);
	pallate.setColor(QPalette::Button,buttonText);
	pallate.setColor(QPalette::Text,userText);

	qApp->setPalette( pallate);

}
void GTAUITheme::setDefualt()
{
    //---------------------------------
    // Reset to the basic color scheme
    //---------------------------------
	QString backGround ="aliceblue";
	QString windowText = "black";
	QString buttonText = "aliceblue";
	QString userText = "black";
	int bgIdx = ui->BGColorCB->findText(backGround);
	ui->BGColorCB->setCurrentIndex(bgIdx);

	int wtIdx = ui->WindowTxtColorCB->findText(windowText);
	ui->WindowTxtColorCB->setCurrentIndex(wtIdx);

	int btIdx = ui->ButtonTextColorCB->findText(buttonText);
	ui->ButtonTextColorCB->setCurrentIndex(btIdx);

	int utIdx = ui->UserTextColorCB->findText(userText);
	ui->UserTextColorCB->setCurrentIndex(utIdx);
}
void GTAUITheme::onOKClicked()
{
	hide();

     saveTheme();

     deleteLater();
}
void GTAUITheme::onCancelClicked()
{
	hide();
        setDefualt();
	deleteLater();
}
void GTAUITheme::saveTheme()
{
    //---------------------------------
    // save the last chosen color scheme
    //---------------------------------
    GTAUserSettings userSettings;
    QMap<QString, QString> colorContextMap;
    colorContextMap.insert(THEME_BACKGROUND,ui->BGColorCB->currentText());
    colorContextMap.insert(THEME_WINDOWTEXT,ui->WindowTxtColorCB->currentText());
    colorContextMap.insert(THEME_BUTTON,ui->ButtonTextColorCB->currentText());
    colorContextMap.insert(THEME_USERTEXT,ui->UserTextColorCB->currentText());
    userSettings.setTheme(colorContextMap);
}

void GTAUITheme::loadTheme()
{
    //---------------------------------
    //load the previously saved theme
    //---------------------------------
    GTAUserSettings userSettings;
    QMap<QString, QString> colorContextMap = userSettings.getTheme();
    QString backGround = colorContextMap.value(THEME_BACKGROUND);
    QString windowText = colorContextMap.value(THEME_WINDOWTEXT);
    QString button = colorContextMap.value(THEME_BUTTON);
    QString userText = colorContextMap.value(THEME_USERTEXT);

    if(!backGround.isEmpty())
    {
       int bgIdx = ui->BGColorCB->findText(backGround);
       ui->BGColorCB->setCurrentIndex(bgIdx);
    }

    if(!windowText.isEmpty())
    {
       int wtIdx = ui->WindowTxtColorCB->findText(windowText);
       ui->WindowTxtColorCB->setCurrentIndex(wtIdx);
    }

    if(!button.isEmpty())
    {
       int btIdx = ui->ButtonTextColorCB->findText(button);
       ui->ButtonTextColorCB->setCurrentIndex(btIdx);
    }

    if(! userText.isEmpty())
    {
       int utIdx = ui->UserTextColorCB->findText(userText);
       ui->UserTextColorCB->setCurrentIndex(utIdx);
    }
}
void GTAUITheme::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}
