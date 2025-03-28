#include "GTAActionManualWidget.h"
#include "GTAActionManual.h"
#include "GTAParamValidator.h"
#include "GTAParamValidator.h"
#include "GTAAppController.h"

#pragma warning(push, 0)
#include "ui_GTAActionManualWidget.h"
#include <QMessageBox>
#include <QCompleter>
#include <QPixmap>
#include <QColorDialog>
#include <QByteArray>
#include <QTextCodec>
#include <QDebug>
#pragma warning(pop)

#define DFLT_USR_FEEDBACK "UserFeedback"
GTAActionManualWidget::GTAActionManualWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionManualWidget)
{
    ui->setupUi(this);

    _pManualAction =NULL;// new GTAActionManual();
    _IsLocalParam = false;
    ui->UserFeedbackGB->setEnabled(true);

    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->UserFeedbackChkB,SIGNAL(toggled(bool)),this,SLOT(onUserFeedbackValueChange(bool)));
    connect(ui->AcknowledgChkB,SIGNAL(toggled(bool)),this,SLOT(onAcknowledgementValueChange(bool)));

    ui->UserFeedbackChkB->setChecked(false);
    ui->AcknowledgChkB->setChecked(true);

    GTAParamValidator* pValidator2 = new GTAParamValidator(ui->ParameterLE);
    ui->ParameterLE->setValidator(pValidator2);
    connect(ui->TitleTE,SIGNAL(textChanged()),this,SLOT(validateText()));


    GTAAppController* pController = GTAAppController::getGTAAppController();
    if (NULL!=pController)
    {
        QStringList wordList = pController->getVariablesForAutoComplete();
        QCompleter *completer = new QCompleter(wordList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->ParameterLE->setCompleter(completer);
    }

    ui->BoldTB->setCheckable(true);
    connect(ui->BoldTB,SIGNAL(clicked()),this,SLOT(textBold()));

    ui->ItalicsTB->setCheckable(true);
    connect(ui->ItalicsTB,SIGNAL(clicked()),this,SLOT(textItalic()));

    ui->UnderlineTB->setCheckable(true);
    connect(ui->UnderlineTB,SIGNAL(clicked()),this,SLOT(textUnderline()));

    connect(ui->FontCB,SIGNAL(activated(QString)),this,SLOT(textFamily(QString)));

    ui->FontSizeCB->clear();
    QFontDatabase db;
    foreach(int size, db.standardSizes())
        ui->FontSizeCB->addItem(QString::number(size));

    connect(ui->FontSizeCB, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
    ui->FontSizeCB->setCurrentIndex(ui->FontSizeCB->findText(QString::number(QApplication::font()
                                                                   .pointSize())));

    ui->LeftTB->setCheckable(true);
    connect(ui->LeftTB,SIGNAL(clicked()),this,SLOT(textAlign()));
    ui->RightTB->setCheckable(true);
    connect(ui->RightTB,SIGNAL(clicked()),this,SLOT(textAlign()));
    ui->CentreTB->setCheckable(true);
    connect(ui->CentreTB,SIGNAL(clicked()),this,SLOT(textAlign()));
    ui->JustifyTB->setCheckable(true);
    connect(ui->JustifyTB,SIGNAL(clicked()),this,SLOT(textAlign()));

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    ui->ColorTB->setIcon(pix);
    connect(ui->ColorTB,SIGNAL(clicked()),this,SLOT(textColor()));


    QPixmap pic(16, 16);
    pic.fill(Qt::white);
    ui->BackgroundTB->setIcon(pic);
    connect(ui->BackgroundTB,SIGNAL(clicked()),this,SLOT(backgroundColor()));


    connect(ui->TitleTE, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(ui->TitleTE, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));


    fontChanged(ui->TitleTE->font());
    colorChanged(ui->TitleTE->textColor());
    alignmentChanged(ui->TitleTE->alignment());
    backgroundColorChanged(ui->TitleTE->textBackgroundColor());

    connect(ui->searchParamPB1, SIGNAL(clicked()), this, SLOT(onSearchPBClicked1()));
    connect(ui->addToExpressionPB, SIGNAL(clicked()), this, SLOT(onAddExpPBClicked()));
    connect(ui->addCondition, SIGNAL(toggled(bool)), this, SLOT(onAddCondition(bool)));
}

GTAActionManualWidget::~GTAActionManualWidget()
{
    //if(pManualAction != NULL)
    //{
    //    delete pManualAction;
    //    pManualAction = NULL;
    //}
    /*const QValidator* pValidator = ui->TitleLE->validator();
    if (pValidator!=NULL)
        delete pValidator;*/
    delete ui;
}

void GTAActionManualWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
    if(ipActionCmd != NULL)
    {
        GTAActionManual * pCmd = (GTAActionManual*)ipActionCmd;

        QString message = pCmd->getMessage();
        QString returnType =pCmd->getReturnType();
        QString targetMachine =pCmd->getTargetMachine();
        bool acknowledgement = pCmd->getAcknowledgetment();
        bool userFeedback = pCmd->getUserFeedback();
        _IsLocalParam = pCmd->IsLocalParameter();
        QString parameter = pCmd->getParameter();
        QString htmlMessage = pCmd->getHtmlMessage();

        QByteArray subData = htmlMessage.toLocal8Bit();
        QTextCodec *codec = Qt::codecForHtml(subData);
        QString str = codec->toUnicode(subData);
        if (Qt::mightBeRichText(str))
        {
            ui->TitleTE->setHtml(str);
        }
        else
        {
            str = QString::fromLocal8Bit(subData);
            ui->TitleTE->setPlainText(str);
        }

//        ui->TitleTE->setText(message);
//        ui->TitleTE->setHtml(htmlMessage);
        ui->AcknowledgChkB->setChecked(acknowledgement);
        ui->UserFeedbackChkB->setChecked(userFeedback);
        ui->ParameterLE->setText(parameter);

        for( int i = 0 ; i < ui->ReturnTypeCB->count(); i++)
        {
            if( ui->ReturnTypeCB->itemText(i) == returnType)
            {
                ui->ReturnTypeCB->setCurrentIndex(i);
                break;
            }
        }

        _pManualAction = pCmd;
        GTACommandBase* parentCmd = pCmd->getParent();
        QString statement = parentCmd == NULL? QString() : parentCmd->getPrintStatement();
        statement = statement.toLower();
        ui->addCondition->setDisabled(statement.isEmpty() || statement.contains(COM_CONDITION_ELSE) || statement.contains(COM_CONDITION_FOR_EACH)); 
    }
    else
    {
        clear();
        ipActionCmd=NULL;
    }

}

bool GTAActionManualWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;
    GTAActionManual * pCmd = NULL;
    

    if(isValid()==false)
    {
        opCmd=pCmd;
        return rc;
    }

    QString htmlMessage;
    QString TitleHtmlMessage = ui->TitleTE->toHtml();
    QString sMessage = ui->TitleTE->toPlainText();
    sMessage.replace("\""," ");
    sMessage.replace("'"," ");
    QString returnType = ui->ReturnTypeCB->currentText();
//    QString targetMachine = ui->TargetMachineCB->currentText();
    bool acknowledgement = ui->AcknowledgChkB->isChecked();
    bool userFeedback = ui->UserFeedbackChkB->isChecked();
    QString parameter = ui->ParameterLE->text();


    pCmd = new GTAActionManual();

    pCmd->setMessage(ampersandEncode(sMessage));
    htmlMessage = ampersandEncode(TitleHtmlMessage);
    pCmd->setHTMLMessage(htmlMessage);
    pCmd->setReturnType(returnType);
    pCmd->setAcknowledgement(acknowledgement);
    pCmd->setUserFeedback(userFeedback);
//    pCmd->setTargetMachine(targetMachine);
    pCmd->setParameterType(_IsLocalParam);
    pCmd->setParameter(parameter);
    
    opCmd=pCmd;
    rc = true;
    
    return rc;
}
void GTAActionManualWidget::onSearchPBClicked()
{
    emit searchObject(ui->ParameterLE);
}
void GTAActionManualWidget::onSearchPBClicked1()
{
    ui->addCondition->setChecked(false);
    emit searchObject(ui->ParameterLE1);
}
void GTAActionManualWidget::onAddExpPBClicked()
{
    int cursorPosition = ui->TitleTE->textCursor().position();
    QString currTxt = ui->TitleTE->toPlainText();
    QString separator = currTxt.isEmpty() ? "" : ",";
    QString userExpression = ui->ParameterLE1->text();
    ui->TitleTE->setText(currTxt.append(separator + userExpression));

    QTextCursor currentCursor = ui->TitleTE->textCursor();
    currentCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, cursorPosition);
    ui->TitleTE->setTextCursor(currentCursor);
}
void GTAActionManualWidget::onAddCondition(bool iValue)
{
    if (iValue)
    {         
        ui->ParameterLE1->setText(_pManualAction->getParent()->getPrintStatement().split(" ").at(1));
    }
    else
    {
        ui->ParameterLE1->clear();
    }
}
void GTAActionManualWidget::onUserFeedbackValueChange(bool iValue)
{
    if(iValue)
    {
        ui->ParameterLE->setText(DFLT_USR_FEEDBACK);
        ui->UserFeedbackGB->setEnabled(true);
        ui->AcknowledgChkB->setChecked(false);
    }
    else
        ui->UserFeedbackGB->setEnabled(false);
}

void GTAActionManualWidget::onAcknowledgementValueChange(bool iValue)
{
    if(iValue)
        ui->UserFeedbackChkB->setChecked(false);
}
void GTAActionManualWidget::clear()
{
    ui->AcknowledgChkB->setChecked(true);
//    ui->TargetMachineCB->setCurrentIndex(0);
    ui->TitleTE->setText(QString());
    ui->ParameterLE->setText(DFLT_USR_FEEDBACK);
}


void GTAActionManualWidget::setParametertype(const QString &iParamType)
{
    QString sParamType = "";

    if(iParamType == "boolean")
        sParamType = "bool";
    else if(iParamType == "integer")
        sParamType = "int";
    else if(iParamType == "enumerate")
        sParamType = "int";
    else if(iParamType == "float")
        sParamType = "float";
    else if(iParamType == "string")
        sParamType = "string";
    else if(iParamType == "char")
        sParamType = "string";
    else sParamType = "string";


    int indx = ui->ReturnTypeCB->findText(sParamType);
    if(indx != -1)
        ui->ReturnTypeCB->setCurrentIndex(indx);
    else
        QMessageBox::warning(this,"Manual Action","Invalid Signal!\nSignal Type: "+sParamType+" is not supported.");
}
void GTAActionManualWidget::validateText()
{
    GTAParamValidator txtValidatorObj;
    QString currentText = ui->TitleTE->toPlainText();
    int txtSize = currentText.size();

    //    QRegExp rx("([`'\"]+)");
    //    if(currentText.contains(rx))
    //    {
    //        currentText.remove(rx);
    //        disconnect(ui->TitleTE,SIGNAL(textChanged()),this,SLOT(validateText()));
    //        ui->TitleTE->setText(currentText);
    //        connect(ui->TitleTE,SIGNAL(textChanged()),this,SLOT(validateText()));
    //        ui->TitleTE->moveCursor(QTextCursor::End);
    //        ui->TitleTE->setToolTip("Contained forbidden charachter(s) which has been removed");

    //    }

	QRegExp rx("([!/`%&~|,\\^\\*\\(\\)\\|\\[\\]\\(\\)\\{\\}\\.\\?]+)");
	
    /*if(currentText.contains(rx))
	{
		ui->TitleTE->setText(currentText.remove(rx));
		ui->TitleTE->moveCursor(QTextCursor::End);
	}*/
    if (txtSize !=0 && txtValidatorObj.validate(currentText,txtSize) != QValidator::Acceptable && !currentText.contains(rx))
    {
        ui->TitleTE->setText(currentText.remove(txtSize-1,1));
        ui->TitleTE->moveCursor(QTextCursor::End);
    }
    else
        ui->TitleTE->setToolTip("");

}
void GTAActionManualWidget::processSearchInput(const QStringList& iSearchItemList)
{
    if(iSearchItemList.size() > 4)
    {
        setParametertype(iSearchItemList.at(4).toLower());
    }
}
bool GTAActionManualWidget::isValid() const 
{
    QString message = ui->TitleTE->toPlainText();
    QString returnType = ui->ReturnTypeCB->currentText();
//    QString targetMachine = ui->TargetMachineCB->currentText();
    //if(message.isEmpty() || targetMachine.isEmpty() || returnType.isEmpty())
	if(message.isEmpty() || returnType.isEmpty())
    {
        return false;
    }
    if (ui->UserFeedbackChkB->isChecked())
    {
        QString parameterName = ui->ParameterLE->text();
        if (parameterName.isEmpty())
        {
            return false;
        }
    }
    return true;
}
#include "GTAGenSearchWidget.h"
int GTAActionManualWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::PARAMETER;
    return retVal;
}
void GTARestrictedLineWidget::keyPressEvent(QKeyEvent * event )
{
    QString text = event->text();
    if (text!="`")
    {
        QLineEdit::keyPressEvent(event);
    }
}


void GTAActionManualWidget::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->BoldTB->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void GTAActionManualWidget::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->ItalicsTB->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}
void GTAActionManualWidget::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->UnderlineTB->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void GTAActionManualWidget::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{

    QTextCursor cursor = ui->TitleTE->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->TitleTE->mergeCurrentCharFormat(format);
}

void GTAActionManualWidget::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void GTAActionManualWidget::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void GTAActionManualWidget::textAlign()
{
    if (sender() == ui->LeftTB)
        ui->TitleTE->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (sender() == ui->CentreTB)
        ui->TitleTE->setAlignment(Qt::AlignHCenter);
    else if (sender() == ui->RightTB)
        ui->TitleTE->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (sender() == ui->JustifyTB)
        ui->TitleTE->setAlignment(Qt::AlignJustify);
}

void GTAActionManualWidget::backgroundColor()
{
    QColor col = QColorDialog::getColor(ui->TitleTE->textBackgroundColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setBackground(col);
    mergeFormatOnWordOrSelection(fmt);
    backgroundColorChanged(col);
}
void GTAActionManualWidget::textColor()
{
    QColor col = QColorDialog::getColor(ui->TitleTE->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void GTAActionManualWidget::backgroundColorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    ui->BackgroundTB->setIcon(pix);
}


void GTAActionManualWidget::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    ui->ColorTB->setIcon(pix);
}


void GTAActionManualWidget::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
    backgroundColorChanged(format.background().color());
}

void GTAActionManualWidget::cursorPositionChanged()
{
    alignmentChanged(ui->TitleTE->alignment());
}


void GTAActionManualWidget::fontChanged(const QFont &f)
{
    ui->FontCB->setCurrentIndex(ui->FontCB->findText(QFontInfo(f).family()));
    ui->FontSizeCB->setCurrentIndex(ui->FontSizeCB->findText(QString::number(f.pointSize())));
    ui->BoldTB->setChecked(f.bold());
    ui->ItalicsTB->setChecked(f.italic());
    ui->UnderlineTB->setChecked(f.underline());
}

void GTAActionManualWidget::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        ui->LeftTB->setChecked(true);
    else if (a & Qt::AlignHCenter)
        ui->CentreTB->setChecked(true);
    else if (a & Qt::AlignRight)
        ui->RightTB->setChecked(true);
    else if (a & Qt::AlignJustify)
        ui->JustifyTB->setChecked(true);
}


QString GTAActionManualWidget::ampersandEncode(QString string)const
{
    QString encoded;
    for(int i=0;i<string.size();++i)
    {

        QChar ch = string.at(i);
//		qDebug() << ch.row()<<":" << ch.cell()<<":"<<ch.unicode()<<":"<<ch.toAscii() << ":" <<ch;

        if(ch.unicode() > 127)
            encoded += QString("&#%1;").arg((int)ch.unicode());
        else
            encoded += ch;
    }
    return encoded;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_MANUAL,GTAActionManualWidget,obj);
