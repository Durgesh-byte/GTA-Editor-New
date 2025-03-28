#include "GTAEquationRampWidget.h"
#include "GTAEquationRamp.h"
#include "ui_GTAEquationRampWidget.h"
#define STR_OFFSET "Offset"
#define STR_END "End Value"

GTAEquationRampWidget::GTAEquationRampWidget(QWidget *parent) :
GTAEquationUIInterface(parent),
ui(new Ui::GTAEquationRampWidget)
{
    ui->setupUi(this);
    onSolidCBToggled(true);
    connect(ui->SearchTB,SIGNAL(clicked()),this,SLOT(onSearchStartTimeClicked()));
    connect(ui->SearchTB2, SIGNAL(clicked()), this, SLOT(onSearchRiseTimeClicked()));
    connect(ui->SearchTB3, SIGNAL(clicked()), this, SLOT(onSearchEndTimeClicked()));

    _pSearchWidgetStartTime = new GTAGenSearchWidget();
    _pSearchWidgetEndTime = new GTAGenSearchWidget();
    _pSearchWidgetRiseTime = new GTAGenSearchWidget();

    if(_pSearchWidgetStartTime)
    {
        _pSearchWidgetStartTime->hide();
        connect(_pSearchWidgetStartTime,SIGNAL(okPressed()),this,SLOT(onSearchWidgetStartOKClicked()));

    }
    if(_pSearchWidgetEndTime)
    {
        _pSearchWidgetEndTime->hide();
        connect(_pSearchWidgetEndTime, SIGNAL(okPressed()), this, SLOT(onSearchWidgetEndOKClicked()));
    }
    if (_pSearchWidgetRiseTime)
    {
        _pSearchWidgetRiseTime->hide();
        connect(_pSearchWidgetRiseTime, SIGNAL(okPressed()), this, SLOT(onSearchWidgetRiseOKClicked()));
    }

	_graphEndValue.load(":/images/forms/img/RampGraph_EndValue.png");
	_graphOffset.load(":/images/forms/img/RampGraph_Offset.png");
	connect(ui->EndTimeCB, SIGNAL(currentIndexChanged(int)), this, SLOT(endCBChanged(int)));
}

GTAEquationRampWidget::~GTAEquationRampWidget()
{
    if(_pSearchWidgetStartTime != NULL)
    {
        delete _pSearchWidgetStartTime;
        _pSearchWidgetStartTime = NULL;
    }
    if (_pSearchWidgetEndTime != NULL)
    {
        delete _pSearchWidgetEndTime;
        _pSearchWidgetEndTime = NULL;
    }
    if (_pSearchWidgetRiseTime != NULL)
    {
        delete _pSearchWidgetRiseTime;
        _pSearchWidgetRiseTime = NULL;
    }
    delete ui;
}

QString GTAEquationRampWidget::getRiseTime() const
{
    return ui->riseTimeLE->text();
}
QString GTAEquationRampWidget::getStart() const
{
    return ui->startLE->text();
}
QString GTAEquationRampWidget::getEnd() const
{
    return ui->endLE->text();

}

void GTAEquationRampWidget::setRiseTime(const QString & iRiseTime)
{
    ui->riseTimeLE->setText(iRiseTime);

}
void GTAEquationRampWidget::setStart(const QString & iStart)
{
    ui->startLE->setText(iStart);
}
void GTAEquationRampWidget::setEnd(const QString & iEnd)
{
    ui->endLE->setText(iEnd);
}
bool GTAEquationRampWidget::isValid()const
{
    if( ui->riseTimeLE->text().isEmpty()     ||
        ui->startLE->text().isEmpty()        ||
        ui->endLE->text().isEmpty()
        )
        return false;
    else
        return true;
}
void GTAEquationRampWidget::clear() const
{
    ui->riseTimeLE->clear();
    ui->startLE->clear();
    ui->endLE->clear();
}

bool GTAEquationRampWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    bool rc = false;
    GTAEquationRamp* pEqnRamp= NULL;
    if (NULL!=pEqBase)
    {
        pEqnRamp = dynamic_cast<GTAEquationRamp*> (pEqBase);

    }
    else
    {
        pEqnRamp = new GTAEquationRamp();

    }
    if (NULL!=pEqnRamp)
    {

        pEqnRamp->setRiseTime(getRiseTime());
        pEqnRamp->setStart(getStart());
        bool isOffset = isEndValueOffset();
        pEqnRamp->setEndValueOffset(isOffset);
        if(isOffset)
        {
            bool isNumber;
            double number = getEnd().toDouble(&isNumber);
            if (isNumber)
            {
                QString endValue = QString::number(getStart().toDouble() + getEnd().toDouble());
                pEqnRamp->setEnd(endValue);
            }
            else
            {
                QString offsetFormule = getStart() + "+" + getEnd();
                pEqnRamp->setEnd(getEnd());
            }
        }
        else
            pEqnRamp->setEnd(getEnd());

        rc = true;
    }
    pEqBase = pEqnRamp;
    return rc;

}

bool GTAEquationRampWidget::setEquation(GTAEquationBase*& pEqBase)
{
    GTAEquationRamp* pEqnRamp= NULL;
    if (NULL!=pEqBase)
    {
        pEqnRamp = dynamic_cast<GTAEquationRamp*> (pEqBase);
        if (NULL!=pEqnRamp)
        {
            setRiseTime(pEqnRamp->getRiseTime());
            setEnd(pEqnRamp->getEnd());
            setStart(pEqnRamp->getStart());
            if(pEqnRamp->isEndValueOffset())
            {
                int index = ui->EndTimeCB->findText(STR_OFFSET);
                if(index > 0 && index < ui->EndTimeCB->count())
                    ui->EndTimeCB->setCurrentIndex(index);

                QString str = pEqnRamp->getEnd();
                QStringList strList = str.split("+");
                str = strList.last();
                str = str.trimmed();
                setEnd(str);
            }
            else
            {
                int index = ui->EndTimeCB->findText(STR_END);
                if(index > 0 && index < ui->EndTimeCB->count())
                    ui->EndTimeCB->setCurrentIndex(index);
            }
        }
    }
    else
        clear();
    return true;
}


void GTAEquationRampWidget::onSolidCBToggled(bool iVal)
{
    if(iVal)
    {
        ui->startLE->clear();
    }
    else
    {
        QString startValue = getStart();
        if(startValue == EQ_CURRENT_VALUE || startValue == "")
            ui->startLE->setText(EQ_CURRENT_VALUE);

    }
//    ui->SearchTB->setVisible(!iVal);
}

bool GTAEquationRampWidget::isEndValueOffset()const
{
    if(ui->EndTimeCB->currentText() == STR_OFFSET)
        return true;
    else
        return false;
}

void GTAEquationRampWidget::onSearchStartTimeClicked()
{
    if(_pSearchWidgetStartTime != NULL)
    {
        _pSearchWidgetStartTime->setSearchType(GTAGenSearchWidget::PARAMETER);
        _pSearchWidgetStartTime->show();
    }
}

void GTAEquationRampWidget::onSearchEndTimeClicked()
{
    if (_pSearchWidgetEndTime != NULL)
    {
        _pSearchWidgetEndTime->setSearchType(GTAGenSearchWidget::PARAMETER);
        _pSearchWidgetEndTime->show();
    }
}

void GTAEquationRampWidget::onSearchRiseTimeClicked()
{
    if (_pSearchWidgetRiseTime != NULL)
    {
        _pSearchWidgetRiseTime->setSearchType(GTAGenSearchWidget::PARAMETER);
        _pSearchWidgetRiseTime->show();
    }
}

void GTAEquationRampWidget::onSearchWidgetStartOKClicked()
{
    GTAICDParameter selectedItem = _pSearchWidgetStartTime->getSelectedItems();
    if(!selectedItem.getName().isEmpty())
    {
        ui->startLE->setText(selectedItem.getName());
    }
    _pSearchWidgetStartTime->hide();
}

void GTAEquationRampWidget::onSearchWidgetEndOKClicked()
{
    GTAICDParameter selectedItem = _pSearchWidgetEndTime->getSelectedItems();
    if (!selectedItem.getName().isEmpty())
    {
        ui->endLE->setText(selectedItem.getName());
    }
    _pSearchWidgetEndTime->hide();
}

void GTAEquationRampWidget::onSearchWidgetRiseOKClicked()
{
    GTAICDParameter selectedItem = _pSearchWidgetRiseTime->getSelectedItems();
    if (!selectedItem.getName().isEmpty())
    {
        ui->riseTimeLE->setText(selectedItem.getName());
    }
    _pSearchWidgetRiseTime->hide();
}

void GTAEquationRampWidget::endCBChanged(int index)
{
	if (index == 0)
	{
		ui->ImageLabel->setPixmap(_graphEndValue);
	}
	else
	{
		ui->ImageLabel->setPixmap(_graphOffset);
	}
}


#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_RAMP,GTAEquationRampWidget)
