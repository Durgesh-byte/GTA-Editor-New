#include "AINGTAICUserFeedbackWidget.h"
#include "ui_AINGTAICUserFeedbackWidget.h"
#include <QFile>
#include <QKeyEvent>

AINGTAICUserFeedbackWidget::AINGTAICUserFeedbackWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AINGTAICUserFeedbackWidget)
{
    ui->setupUi(this);
    ui->okPB->setEnabled(false);
    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    connect(ui->okPB,SIGNAL(clicked()),this,SLOT(onOKPBClicked()));
    connect(ui->trueRB,SIGNAL(toggled(bool)),ui->okPB,SLOT(setEnabled(bool)));
    connect(ui->falseRB,SIGNAL(toggled(bool)),ui->okPB,SLOT(setEnabled(bool)));
    connect(ui->userInputLE,SIGNAL(textChanged(QString)),this,SLOT(okPBEnabled(QString)));
    connect(ui->okButton,SIGNAL(clicked()),this,SLOT(onOkPromptPBClicked()));
    connect(ui->koButton,SIGNAL(clicked()),this,SLOT(onKoPromptPBClicked()));

    ui->okButton->setStyleSheet("QPushButton { width: 40px; background-color: green; } QPushButton:focus { border-color: white;} QPushButton:hover { border-color: white;}");
    ui->koButton->setStyleSheet("QPushButton {width: 40px; background-color: red; } QPushButton:focus { border-color: white;} QPushButton:hover { border-color: white;}");
    _mousePressed = false;
    _mousePosition = QPoint();
}

AINGTAICUserFeedbackWidget::~AINGTAICUserFeedbackWidget()
{
    delete ui;
}

bool AINGTAICUserFeedbackWidget::showWidget(QString msg, QString expectedReturnVal)
{
    ui->okButton->setVisible(false);
    ui->koButton->setVisible(false);
    ui->textMsgLB->setText(msg);
    bool rc = true;
    ui->userInputLE->setVisible(true);
    ui->expectedTypeLB->setText("Expected Type : " + expectedReturnVal);
    ui->expectedTypeLB->setVisible(true);
    ui->trueRB->setVisible(false);
    ui->falseRB->setVisible(false);
    validateText(expectedReturnVal);
    if(!(expectedReturnVal.compare("int",Qt::CaseInsensitive) == 0 || expectedReturnVal.compare("float",Qt::CaseInsensitive) == 0 || expectedReturnVal.compare("string",Qt::CaseInsensitive) == 0 || expectedReturnVal.compare("bool",Qt::CaseInsensitive) == 0))
    {
        rc = false;
    }
    else if(expectedReturnVal.compare("bool",Qt::CaseInsensitive) == 0)
    {
        ui->trueRB->setVisible(true);
        ui->falseRB->setVisible(true);
        ui->userInputLE->setVisible(false);
        ui->expectedTypeLB->setVisible(false);
    }
    show();
    this->activateWindow();
    return rc;
}

void AINGTAICUserFeedbackWidget::showPrompt(QString msg, bool iWaitAck)
{
    ui->textMsgLB->setText(msg);
    ui->expectedTypeLB->setVisible(false);
    ui->userInputLE->setVisible(false);
    ui->trueRB->setVisible(false);
    ui->falseRB->setVisible(false);
    if(iWaitAck)
    {
        ui->okButton->setVisible(true);
        ui->koButton->setVisible(true);
        ui->okPB->setVisible(false);
    }
    else
    {
        ui->okButton->setVisible(false);
        ui->okButton->setAutoDefault(false);
        ui->koButton->setVisible(false);
        ui->okPB->setVisible(true);
        ui->okPB->setEnabled(true);
    }
    show();
    this->activateWindow();
}

void AINGTAICUserFeedbackWidget::validateText(QString expectedReturnType)
{
    if(expectedReturnType.compare("int",Qt::CaseInsensitive) == 0)
    {
        QIntValidator *validator = new QIntValidator(ui->userInputLE);
        ui->userInputLE->setValidator(validator);
    }
    else if(expectedReturnType.compare("float",Qt::CaseInsensitive) == 0)
    {
        QDoubleValidator * validator =  new QDoubleValidator(ui->userInputLE);
        ui->userInputLE->setValidator(validator);
    }
}

void AINGTAICUserFeedbackWidget::onOKPBClicked()
{
    if(!ui->expectedTypeLB->text().isEmpty())
    {
        if(ui->expectedTypeLB->text().contains("bool"))
        {
            if(ui->trueRB->isChecked())
            {
                emit sendUserInput("1");
            }
            else
                emit sendUserInput("0");
        }
        else
        {
            emit sendUserInput(ui->userInputLE->text());
        }
    }
    close();
}

void AINGTAICUserFeedbackWidget::onOkPromptPBClicked()
{
    emit sendAcknowledgment("true");
    close();
}
void AINGTAICUserFeedbackWidget::onKoPromptPBClicked()
{
    emit sendAcknowledgment("false");
    close();
}

void AINGTAICUserFeedbackWidget::okPBEnabled(QString text)
{
    if(text.length() != 0)
    {
        ui->okPB->setEnabled(true);
    }
    else
    {
        ui->okPB->setEnabled(false);
    }
}
void AINGTAICUserFeedbackWidget::keyPressEvent(QKeyEvent *event)
{

    if (event->key()==Qt::Key_Enter && ui->okPB->isEnabled())
    {
        onOKPBClicked();
    }
    else if(event->key()==Qt::Key_Enter && ui->okButton->isVisible())
    {
        onOkPromptPBClicked();
    }
    else
        QWidget::keyPressEvent(event);
}

void AINGTAICUserFeedbackWidget::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton)
    {
        _mousePressed = true;
        _mousePosition = event->pos();
    }
}

void AINGTAICUserFeedbackWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (_mousePressed)
    {
        move(mapToParent(event->pos() - _mousePosition));
    }
}

void AINGTAICUserFeedbackWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _mousePressed = false;
        _mousePosition = QPoint();
    }
}
