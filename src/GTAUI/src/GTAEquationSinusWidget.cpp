#include "GTAEquationSinusWidget.h"
#include "GTAEquationSinus.h"
#include "ui_GTAEquationSinusWidget.h"

GTAEquationSinusWidget::GTAEquationSinusWidget(QWidget *parent) :
GTAEquationUIInterface(parent),
ui(new Ui::GTAEquationSinusWidget)
{
    ui->setupUi(this);

	_newDirections = new QList<QString>();

	connect(ui->buttonFreq, SIGNAL(clicked()), this, SLOT(isFreqClicked()));
	connect(ui->buttonPeriod, SIGNAL(clicked()), this, SLOT(isPeriodClicked()));
	connect(ui->buttonMinMax, SIGNAL(clicked()), this, SLOT(isMinMaxClicked()));
	connect(ui->buttonOffAmp, SIGNAL(clicked()), this, SLOT(isOffAmpClicked()));
	connect(ui->buttonPhase, SIGNAL(clicked()), this, SLOT(isPhaseClicked()));
	connect(ui->buttonStartValueDir, SIGNAL(clicked()), this, SLOT(isStartValDirClicked()));
	connect(ui->buttonDegrees, SIGNAL(clicked()), this, SLOT(isDegreesClicked()));
	connect(ui->buttonRadians, SIGNAL(clicked()), this, SLOT(isRadiansClicked()));
	connect(ui->frequencyLE, SIGNAL(textEdited(QString)), this, SLOT(frequencyValueChanged()));
	connect(ui->periodLE, SIGNAL(textEdited(QString)), this, SLOT(periodValueChanged()));
	connect(ui->minValueLE, SIGNAL(textEdited(QString)), this, SLOT(minimunValueChanged()));
	connect(ui->maxValueLE, SIGNAL(textEdited(QString)), this, SLOT(maximumValueChanged()));
	connect(ui->ampLE, SIGNAL(textEdited(QString)), this, SLOT(amplitudeValueChanged()));
	connect(ui->offsetLE, SIGNAL(textEdited(QString)), this, SLOT(offsetValueChanged()));
	connect(ui->phaseLE, SIGNAL(textEdited(QString)), this, SLOT(phaseValueChanged()));
	connect(ui->startValueLE, SIGNAL(textEdited(QString)), this, SLOT(startValueChanged()));
	connect(ui->directionCB, SIGNAL(currentIndexChanged(int)), this, SLOT(directionChanged(int)));

	connect(ui->SearchMinVal, SIGNAL(clicked()), this, SLOT(onSearchMinimumClicked()));
	connect(ui->SearchMaxVal, SIGNAL(clicked()), this, SLOT(onSearchMaximumClicked()));
	connect(ui->SearchOffset_2, SIGNAL(clicked()), this, SLOT(onSearchOffsetClicked()));
	connect(ui->SearchAmplitude_2, SIGNAL(clicked()), this, SLOT(onSearchAmplitudeClicked()));
	connect(ui->SearchPeriod, SIGNAL(clicked()), this, SLOT(onSearchPeriodClicked()));
	connect(ui->SearchFrequency, SIGNAL(clicked()), this, SLOT(onSearchFrequencyClicked()));
	connect(ui->SearchStartVal, SIGNAL(clicked()), this, SLOT(onSearchStartValueClicked()));
	connect(ui->SearchPhase, SIGNAL(clicked()), this, SLOT(onSearchPhaseClicked()));
	connect(ui->SearchDirection, SIGNAL(clicked()), this, SLOT(onSearchDirectionClicked()));

	_pSearchWidgetMinimum = new GTAGenSearchWidget();
	_pSearchWidgetMaximum = new GTAGenSearchWidget();
	_pSearchWidgetOffset = new GTAGenSearchWidget();
	_pSearchWidgetAmplitude = new GTAGenSearchWidget();
	_pSearchWidgetPeriod = new GTAGenSearchWidget();
	_pSearchWidgetFrequency = new GTAGenSearchWidget();
	_pSearchWidgetStartValue = new GTAGenSearchWidget();
	_pSearchWidgetPhase = new GTAGenSearchWidget();
	_pSearchWidgetDirection = new GTAGenSearchWidget();
	
	if (_pSearchWidgetMinimum)
	{
		_pSearchWidgetMinimum->hide();
		connect(_pSearchWidgetMinimum, SIGNAL(okPressed()), this, SLOT(onSearchWidgetMinimumOKClicked()));

	}
	if (_pSearchWidgetMaximum)
	{
		_pSearchWidgetMaximum->hide();
		connect(_pSearchWidgetMaximum, SIGNAL(okPressed()), this, SLOT(onSearchWidgetMaximumOKClicked()));

	}
	if (_pSearchWidgetOffset)
	{
		_pSearchWidgetOffset->hide();
		connect(_pSearchWidgetOffset, SIGNAL(okPressed()), this, SLOT(onSearchWidgetOffsetOKClicked()));

	}
	if (_pSearchWidgetAmplitude)
	{
		_pSearchWidgetAmplitude->hide();
		connect(_pSearchWidgetAmplitude, SIGNAL(okPressed()), this, SLOT(onSearchWidgetAmplitudeOKClicked()));

	}
	if (_pSearchWidgetPeriod)
	{
		_pSearchWidgetPeriod->hide();
		connect(_pSearchWidgetPeriod, SIGNAL(okPressed()), this, SLOT(onSearchWidgetPeriodOKClicked()));

	}
	if (_pSearchWidgetFrequency)
	{
		_pSearchWidgetFrequency->hide();
		connect(_pSearchWidgetFrequency, SIGNAL(okPressed()), this, SLOT(onSearchWidgetFrequencyOKClicked()));

	}
	if (_pSearchWidgetStartValue)
	{
		_pSearchWidgetStartValue->hide();
		connect(_pSearchWidgetStartValue, SIGNAL(okPressed()), this, SLOT(onSearchWidgetStartValueOKClicked()));

	}
	if (_pSearchWidgetPhase)
	{
		_pSearchWidgetPhase->hide();
		connect(_pSearchWidgetPhase, SIGNAL(okPressed()), this, SLOT(onSearchWidgetPhaseOKClicked()));

	}
	if (_pSearchWidgetDirection)
	{
		_pSearchWidgetDirection->hide();
		connect(_pSearchWidgetDirection, SIGNAL(okPressed()), this, SLOT(onSearchWidgetDirectionOKClicked()));

	}

	readOnly = ui->frequencyLE->palette();
	QColor col = readOnly.color(QPalette::Button);
	readOnly.setColor(QPalette::Base, col);
	readOnly.setColor(QPalette::Text, Qt::black);
	graphMinMaxStart.load(":/images/forms/img/Sinus_MinMaxStart.png");
	graphMinMaxPhase.load(":/images/forms/img/Sinus_MinMaxPhase.png");
	graphOffAmpStart.load(":/images/forms/img/Sinus_OffAmpStart.png");
	graphOffAmpPhase.load(":/images/forms/img/Sinus_OffAmpPhase.png");
	equationMinMaxPeriod.load(":/images/forms/img/EQUATION_SINUS_MINMAX_PERIOD.png");
	equationMinMaxFreq.load(":/images/forms/img/EQUATION_SINUS_MINMAX_FREQ.png");
	equationOffAmpPeriod.load(":/images/forms/img/EQUATION_SINUS_OFFAMP_PERIOD.png");
	equationOffAmpFreq.load(":/images/forms/img/EQUATION_SINUS_OFFAMP_FREQ.png");
	allDisabled();
}

GTAEquationSinusWidget::~GTAEquationSinusWidget()
{
	if (_pSearchWidgetMinimum)
	{
		delete _pSearchWidgetMinimum;
		_pSearchWidgetMinimum = NULL;
	}
	if (_pSearchWidgetMaximum)
	{
		delete _pSearchWidgetMaximum;
		_pSearchWidgetMaximum = NULL;

	}
	if (_pSearchWidgetOffset)
	{
		delete _pSearchWidgetOffset;
		_pSearchWidgetOffset = NULL;

	}
	if (_pSearchWidgetAmplitude)
	{
		delete _pSearchWidgetAmplitude;
		_pSearchWidgetAmplitude = NULL;

	}
	if (_pSearchWidgetPeriod)
	{
		delete _pSearchWidgetPeriod;
		_pSearchWidgetPeriod = NULL;

	}
	if (_pSearchWidgetFrequency)
	{
		delete _pSearchWidgetFrequency;
		_pSearchWidgetFrequency = NULL;

	}
	if (_pSearchWidgetStartValue)
	{
		delete _pSearchWidgetStartValue;
		_pSearchWidgetStartValue = NULL;

	}
	if (_pSearchWidgetPhase)
	{
		delete _pSearchWidgetPhase;
		_pSearchWidgetPhase = NULL;

	}
	if (_pSearchWidgetDirection)
	{
		delete _pSearchWidgetDirection;
		_pSearchWidgetDirection = NULL;

	}
    delete ui;
}

// GET Functions
QString GTAEquationSinusWidget::getPeriod() const
{
    return ui->periodLE->text();
}
QString GTAEquationSinusWidget::getDirection() const
{
    QString sDirection = ui->directionCB->currentText();
    return sDirection;
}
QString GTAEquationSinusWidget::getStartValue() const
{
    return ui->startValueLE->text();
}
QString GTAEquationSinusWidget::getMinValue() const
{
    return ui->minValueLE->text();
}
QString GTAEquationSinusWidget::getMaxValue() const
{
    return ui->maxValueLE->text();
}
QString GTAEquationSinusWidget::getFrequency() const
{
	return ui->frequencyLE->text();
}
QString GTAEquationSinusWidget::getAmplitude() const
{
	return ui->ampLE->text();
}
QString GTAEquationSinusWidget::getOffset() const
{
	return ui->offsetLE->text();
}
QString GTAEquationSinusWidget::getPhase() const
{
	return ui->phaseLE->text();
}

// SET Functions
void GTAEquationSinusWidget::setPeriod(const QString & iPeriod)
{
    ui->periodLE->setText(iPeriod);
	periodValueChanged();
}
void GTAEquationSinusWidget::setDirection(const QString & iDirection)
{
	int index = ui->directionCB->findText(iDirection);
	if (index != -1)
	{
	   ui->directionCB->setCurrentIndex(index);
	   directionChanged(index);
	}
	else
	{
		ui->directionCB->addItem(iDirection);
		ui->directionCB->setCurrentText(iDirection);
	}
}
void GTAEquationSinusWidget::setStartValue(const QString & iStartValue)
{
    ui->startValueLE->setText(iStartValue);
	startValueChanged();
}
void GTAEquationSinusWidget::setMinValue(const QString & iMinValue)
{
    ui->minValueLE->setText(iMinValue);
	minimunValueChanged();
}
void GTAEquationSinusWidget::setMaxValue(const QString & iMaxValue)
{
    ui->maxValueLE->setText(iMaxValue);
	maximumValueChanged();
}
void GTAEquationSinusWidget::setFrequency(const QString & iFrequencyValue)
{
	ui->frequencyLE->setText(iFrequencyValue);
	frequencyValueChanged();
}
void GTAEquationSinusWidget::setAmplitude(const QString & iAmplitudeValue)
{
	ui->ampLE->setText(iAmplitudeValue);
	amplitudeValueChanged();
}
void GTAEquationSinusWidget::setOffset(const QString & iOffsetValue)
{
	ui->offsetLE->setText(iOffsetValue);
	offsetValueChanged();
}
void GTAEquationSinusWidget::setPhase(const QString & iPhaseValue)
{
	ui->phaseLE->setText(iPhaseValue);
	phaseValueChanged();
}

void GTAEquationSinusWidget::clear() const
{
    ui->periodLE->clear();
	if (_newDirections->isEmpty())
		ui->directionCB->setCurrentIndex(0);
	else
	{
		// Iterate through _newDirections and remove each item from the combo box
		foreach(const QString & direction, *_newDirections) {
			int index = ui->directionCB->findText(direction);
			if (index != -1) { // -1 means item not found
				ui->directionCB->removeItem(index);
			}
		}
		_newDirections->clear();
	}    
	ui->startValueLE->clear();
    ui->minValueLE->clear();
    ui->maxValueLE->clear();
}

bool GTAEquationSinusWidget::isValid()const
{
    if( ui->periodLE->text().isEmpty() || ui->startValueLE->text().isEmpty() ||
        ui->minValueLE->text().isEmpty() || ui->maxValueLE->text().isEmpty() )
    {
        return false;
    }
    else
        return true;
}

bool GTAEquationSinusWidget::getEquation(GTAEquationBase*& pEqBase)const
{
    bool rc = false;
    GTAEquationSinus* pEqnSinus = NULL;
    if (NULL != pEqBase)
    {
        pEqnSinus = dynamic_cast<GTAEquationSinus*> (pEqBase);
    }
    else
    {
        pEqnSinus = new GTAEquationSinus();
    }
    if (NULL != pEqnSinus)
    {
        pEqnSinus->setPeriod(getPeriod());
		if (ui->buttonFreq->isChecked())
			pEqnSinus->setFrequency(getFrequency());
        pEqnSinus->setDirection(getDirection());
        pEqnSinus->setStartValue(getStartValue());
		if (ui->buttonPhase->isChecked())
			pEqnSinus->setPhase(getPhase());
        pEqnSinus->setMinValue(getMinValue());
        pEqnSinus->setMaxValue(getMaxValue());
		if (ui->buttonOffAmp->isChecked()) 
		{
			pEqnSinus->setAmplitude(getAmplitude());
			pEqnSinus->setOffset(getOffset());
		}
        rc = true;
    }
    pEqBase = pEqnSinus;
    return rc;
}

bool GTAEquationSinusWidget::setEquation(GTAEquationBase*& pEqBase)
{
    GTAEquationSinus* pEqnSinus= NULL;
    if (NULL!=pEqBase)
    {
        pEqnSinus = dynamic_cast<GTAEquationSinus*> (pEqBase);
        if (NULL!=pEqnSinus)
        {
			if(pEqnSinus->getIsPeriod())
				setPeriod(pEqnSinus->getPeriod());
			else
				setFrequency(pEqnSinus->getFrequency());
			if (pEqnSinus->getIsMinMax()) 
			{
				setMinValue(pEqnSinus->getMinValue());
				setMaxValue(pEqnSinus->getMaxValue());
			}
			else
			{
				setAmplitude(pEqnSinus->getAmplitude());
				setOffset(pEqnSinus->getOffset());
			}
			if (pEqnSinus->getIsStartDir())
			{
				setDirection(pEqnSinus->getDirection());
				setStartValue(pEqnSinus->getStartValue());
			}
			else
				setPhase(pEqnSinus->getPhase());
			
			
        }
    }
    else
        clear();
    return true;
}

void GTAEquationSinusWidget::onSolidCBToggled(bool )
{

}

// UI Functions to change input
void GTAEquationSinusWidget::isPeriodClicked()
{
	ui->frequencyLE->setPalette(readOnly);
	ui->frequencyLE->setReadOnly(true);
	ui->periodLE->setPalette(readWrite);
	ui->periodLE->setReadOnly(false);
	ui->SearchPeriod->setDisabled(false);
	ui->SearchFrequency->setDisabled(true);
	changePixmaps();
}
void GTAEquationSinusWidget::isFreqClicked()
{
	ui->periodLE->setPalette(readOnly);
	ui->periodLE->setReadOnly(true);
	ui->frequencyLE->setPalette(readWrite);
	ui->frequencyLE->setReadOnly(false);
	ui->SearchFrequency->setDisabled(false);
	ui->SearchPeriod->setDisabled(true);
	changePixmaps();
}
void  GTAEquationSinusWidget::isMinMaxClicked()
{
	ui->offsetLE->setPalette(readOnly);
	ui->offsetLE->setReadOnly(true);
	ui->ampLE->setPalette(readOnly);
	ui->ampLE->setReadOnly(true);
	ui->maxValueLE->setPalette(readWrite);
	ui->maxValueLE->setReadOnly(false);
	ui->minValueLE->setPalette(readWrite);
	ui->minValueLE->setReadOnly(false);
	ui->SearchMinVal->setDisabled(false);
	ui->SearchMaxVal->setDisabled(false);
	ui->SearchOffset_2->setDisabled(true);
	ui->SearchAmplitude_2->setDisabled(true);
	changePixmaps();
}
void  GTAEquationSinusWidget::isOffAmpClicked()
{
	ui->maxValueLE->setPalette(readOnly);
	ui->maxValueLE->setReadOnly(true);
	ui->minValueLE->setPalette(readOnly);
	ui->minValueLE->setReadOnly(true);
	ui->offsetLE->setPalette(readWrite);
	ui->offsetLE->setReadOnly(false);
	ui->ampLE->setPalette(readWrite);
	ui->ampLE->setReadOnly(false);
	ui->SearchMinVal->setDisabled(true);
	ui->SearchMaxVal->setDisabled(true);
	ui->SearchOffset_2->setDisabled(false);
	ui->SearchAmplitude_2->setDisabled(false);
	changePixmaps();
}
void  GTAEquationSinusWidget::isPhaseClicked()
{
	ui->startValueLE->setPalette(readOnly);
	ui->startValueLE->setReadOnly(true);
	ui->directionCB->setEnabled(false);
	ui->phaseLE->setPalette(readWrite);
	ui->phaseLE->setReadOnly(false);
	ui->SearchStartVal->setDisabled(true);
	ui->SearchPhase->setDisabled(false);
	ui->SearchDirection->setDisabled(true);
	changePixmaps();
}
void  GTAEquationSinusWidget::isStartValDirClicked()
{
	ui->phaseLE->setPalette(readOnly);
	ui->phaseLE->setReadOnly(true);
	ui->startValueLE->setPalette(readWrite);
	ui->startValueLE->setReadOnly(false);
	ui->directionCB->setEnabled(true);
	ui->SearchDirection->setDisabled(false);
	ui->SearchStartVal->setDisabled(false);
	ui->SearchPhase->setDisabled(true);
	changePixmaps();	
}
void  GTAEquationSinusWidget::isDegreesClicked() 
{
	double newPhase = ui->phaseLE->text().toDouble();
	newPhase = qRadiansToDegrees(newPhase);
	ui->phaseLE->setText(QString::number(newPhase));
	changePixmaps();
}
void  GTAEquationSinusWidget::isRadiansClicked() 
{
	double newPhase = ui->phaseLE->text().toDouble();
	newPhase = qDegreesToRadians(newPhase);
	ui->phaseLE->setText(QString::number(newPhase));
	changePixmaps();
}
void GTAEquationSinusWidget::allDisabled()
{
	ui->frequencyLE->setPalette(readOnly);
	ui->frequencyLE->setReadOnly(true);
	ui->periodLE->setPalette(readOnly);
	ui->periodLE->setReadOnly(true);
	ui->offsetLE->setPalette(readOnly);
	ui->offsetLE->setReadOnly(true);
	ui->ampLE->setPalette(readOnly);
	ui->ampLE->setReadOnly(true);
	ui->maxValueLE->setPalette(readOnly);
	ui->maxValueLE->setReadOnly(true);
	ui->minValueLE->setPalette(readOnly);
	ui->minValueLE->setReadOnly(true);
	ui->startValueLE->setPalette(readOnly);
	ui->startValueLE->setReadOnly(true);
	ui->directionCB->setEnabled(false);
	ui->phaseLE->setPalette(readOnly);
	ui->phaseLE->setReadOnly(true);
	ui->SearchAmplitude_2->setDisabled(true);
	ui->SearchDirection->setDisabled(true);
	ui->SearchFrequency->setDisabled(true);
	ui->SearchMaxVal->setDisabled(true);
	ui->SearchMinVal->setDisabled(true);
	ui->SearchOffset_2->setDisabled(true);
	ui->SearchPeriod->setDisabled(true);
	ui->SearchPhase->setDisabled(true);
	ui->SearchStartVal->setDisabled(true);
}

void GTAEquationSinusWidget::changePixmaps()
{
	if (ui->buttonMinMax->isChecked() && ui->buttonPeriod->isChecked() && ui->buttonStartValueDir->isChecked())
	{
		ui->equationLB->setPixmap(equationMinMaxPeriod);
		ui->graphLB->setPixmap(graphMinMaxStart);
	}
	else if (ui->buttonMinMax->isChecked() && ui->buttonPeriod->isChecked() && ui->buttonPhase->isChecked())
	{
		ui->equationLB->setPixmap(equationMinMaxPeriod);
		ui->graphLB->setPixmap(graphMinMaxPhase);
	}
	else if (ui->buttonMinMax->isChecked() && ui->buttonFreq->isChecked() && ui->buttonStartValueDir->isChecked())
	{
		ui->equationLB->setPixmap(equationMinMaxFreq);
		ui->graphLB->setPixmap(graphMinMaxStart);
	}
	else if (ui->buttonMinMax->isChecked() && ui->buttonFreq->isChecked() && ui->buttonPhase->isChecked())
	{
		ui->equationLB->setPixmap(equationMinMaxFreq);
		ui->graphLB->setPixmap(graphMinMaxPhase);
	}
	else if (ui->buttonOffAmp->isChecked() && ui->buttonPeriod->isChecked() && ui->buttonStartValueDir->isChecked())
	{
		ui->equationLB->setPixmap(equationOffAmpPeriod);
		ui->graphLB->setPixmap(graphOffAmpStart);
	}
	else if (ui->buttonOffAmp->isChecked() && ui->buttonPeriod->isChecked() && ui->buttonPhase->isChecked())
	{
		ui->equationLB->setPixmap(equationOffAmpPeriod);
		ui->graphLB->setPixmap(graphOffAmpPhase);
	}
	else if (ui->buttonOffAmp->isChecked() && ui->buttonFreq->isChecked() && ui->buttonStartValueDir->isChecked())
	{
		ui->equationLB->setPixmap(equationOffAmpFreq);
		ui->graphLB->setPixmap(graphOffAmpStart);
	}
	else if (ui->buttonOffAmp->isChecked() && ui->buttonFreq->isChecked() && ui->buttonPhase->isChecked())
	{
		ui->equationLB->setPixmap(equationOffAmpFreq);
		ui->graphLB->setPixmap(graphOffAmpPhase);
	}
}

// UI functions to detect a change in value, and to convert accordingly. 
void GTAEquationSinusWidget::minimunValueChanged() 
{
	if (!ui->maxValueLE->text().isEmpty() && !ui->minValueLE->text().isEmpty())
	{
		QString maximum = ui->maxValueLE->text();
		QString minimum = ui->minValueLE->text();
		if (maximum.contains("line.") || minimum.contains("line."))
		{
			ui->offsetLE->setText("(" + maximum + "+" + minimum + ")/2");
			ui->ampLE->setText("(" + maximum + "-" + minimum + ")/2");
		}
		else
		{
			double newOffset = (ui->maxValueLE->text().toDouble() + ui->maxValueLE->text().toDouble()) / 2;
			double newAmp = (ui->maxValueLE->text().toDouble() - ui->minValueLE->text().toDouble()) / 2;
			ui->offsetLE->setText(QString::number(newOffset));
			ui->ampLE->setText(QString::number(newAmp));
		}
		
	}
	else 
	{
		ui->offsetLE->setText("");
		ui->ampLE->setText("");
	}
}
void GTAEquationSinusWidget::maximumValueChanged()
{
	if (!ui->maxValueLE->text().isEmpty() && !ui->minValueLE->text().isEmpty())
	{
		QString maximum = ui->maxValueLE->text();
		QString minimum = ui->minValueLE->text();
		if (maximum.contains("line.") || minimum.contains("line."))
		{
			ui->offsetLE->setText("(" + maximum + "+" + minimum + ")/2");
			ui->ampLE->setText("(" + maximum + "-" + minimum + ")/2");
		}
		else
		{
			double newOffset = (ui->maxValueLE->text().toDouble() + ui->maxValueLE->text().toDouble()) / 2;
			double newAmp = (ui->maxValueLE->text().toDouble() - ui->minValueLE->text().toDouble()) / 2;
			ui->offsetLE->setText(QString::number(newOffset));
			ui->ampLE->setText(QString::number(newAmp));
		}
	}
	else
	{
		ui->offsetLE->setText("");
		ui->ampLE->setText("");
	}
}
void GTAEquationSinusWidget::offsetValueChanged() 
{
	if (!ui->offsetLE->text().isEmpty() && !ui->ampLE->text().isEmpty())
	{
		QString amplitude = ui->ampLE->text();
		QString offset = ui->offsetLE->text();
		if (amplitude.contains("line.") || offset.contains("line."))
		{
			ui->maxValueLE->setText(offset + "+" + amplitude);
			ui->minValueLE->setText(offset + "-" + amplitude);
		}
		else
		{
			double newMaxValue = offset.toDouble() + amplitude.toDouble();
			double newMinValue = offset.toDouble() - amplitude.toDouble();
			ui->maxValueLE->setText(QString::number(newMaxValue));
			ui->minValueLE->setText(QString::number(newMinValue));
		}
	}
	else
	{
		ui->maxValueLE->setText("");
		ui->minValueLE->setText("");
	}
}
void GTAEquationSinusWidget::amplitudeValueChanged() 
{
	if (!ui->offsetLE->text().isEmpty() && !ui->ampLE->text().isEmpty())
	{
		QString amplitude = ui->ampLE->text();
		QString offset = ui->offsetLE->text();
		if (amplitude.contains("line.") || offset.contains("line."))
		{
			ui->maxValueLE->setText(offset + "+" + amplitude);
			ui->minValueLE->setText(offset + "-" + amplitude);
		}
		else 
		{
			double newMaxValue = offset.toDouble() + amplitude.toDouble();
			double newMinValue = offset.toDouble() - amplitude.toDouble();
			ui->maxValueLE->setText(QString::number(newMaxValue));
			ui->minValueLE->setText(QString::number(newMinValue));
		}	
	}
	else
	{
		ui->maxValueLE->setText("");
		ui->minValueLE->setText("");
	}
}
void GTAEquationSinusWidget::periodValueChanged() 
{
	if (!ui->periodLE->text().isEmpty())
	{
		QString period = ui->periodLE->text();
		if (period.contains("line."))
		{
			ui->frequencyLE->setText("1/"+period);
		}
		else if (ui->periodLE->text().toDouble() != 0)
		{
			double newValue = 1 / ui->periodLE->text().toDouble();
			ui->frequencyLE->setText(QString::number(newValue));
		}
	}
	else
	{
		ui->frequencyLE->setText("");
	}
}
void GTAEquationSinusWidget::frequencyValueChanged() 
{
	if (!ui->frequencyLE->text().isEmpty() )
	{
		QString frequency = ui->frequencyLE->text();
		if (frequency.contains("line."))
		{
			ui->periodLE->setText("1/" + frequency);
		}
		else if (ui->frequencyLE->text().toDouble() != 0)
		{
			double newValue = 1 / ui->frequencyLE->text().toDouble();
			ui->periodLE->setText(QString::number(newValue));
		}	
	}
	else
	{
		ui->periodLE->setText("");
	}
}
void GTAEquationSinusWidget::phaseValueChanged()
{
	if (!ui->phaseLE->text().isEmpty())
	{
		QString qPhase = ui->phaseLE->text();
		if (qPhase.contains("line."))
		{
			ui->startValueLE->setText("sin(" + qPhase + ")*" + ui->ampLE->text() + "+" + ui->offsetLE->text());
		}
		else 
		{
			double phase = ui->phaseLE->text().toDouble();
			if (ui->buttonDegrees->isChecked())
			{
				phase = qDegreesToRadians(phase);
			}
			double startValue = (qSin(phase) * ui->ampLE->text().toDouble()) + ui->offsetLE->text().toDouble();
			ui->startValueLE->setText(QString::number(startValue));
			if ((-M_PI_2 <= phase) && (phase < M_PI_2))
			{
				ui->directionCB->setCurrentIndex(0);
			}
			else
			{
				ui->directionCB->setCurrentIndex(1);
			}
		}
		
	}
	else
	{
		ui->startValueLE->setText("");
	}
}
void GTAEquationSinusWidget::startValueChanged() 
{
	if (!ui->startValueLE->text().isEmpty() && !ui->maxValueLE->text().isEmpty() && !ui->minValueLE->text().isEmpty())
	{
		QString startValue = ui->startValueLE->text();
		QString maximum = ui->maxValueLE->text();
		QString minimum = ui->minValueLE->text();
		QString offset = ui->offsetLE->text();
		QString amplitude = ui->ampLE->text();
		if (startValue.contains("line."))
		{
			ui->phaseLE->setText("asin((" + startValue + "-" + offset + ")/" + amplitude + ")");
		}
		else 
		{
			double newPhase = qAsin((ui->startValueLE->text().toDouble() - ui->offsetLE->text().toDouble()) / ui->ampLE->text().toDouble());
			if (ui->directionCB->currentIndex() == 1)
				newPhase = M_PI - newPhase;
			if (newPhase > M_PI)
			{
				newPhase = newPhase - 2 * M_PI;
			}
			else if (newPhase <= -M_PI)
			{
				newPhase = newPhase + 2 * M_PI;
			}
			if (ui->buttonDegrees->isChecked()) {
				newPhase = qRadiansToDegrees(newPhase);
			}
			ui->phaseLE->setText(QString::number(newPhase));
		}
		
	}
	else
	{
		ui->phaseLE->setText("");
	}
}
void GTAEquationSinusWidget::directionChanged(int index)
{
	if (ui->directionCB->isEnabled()) {
		double newPhase = qAsin((ui->startValueLE->text().toDouble() - ui->offsetLE->text().toDouble()) / ui->ampLE->text().toDouble());
		if (index == 0)
		{
			if (newPhase > M_PI)
			{
				newPhase = newPhase - 2 * M_PI;
			}
			else if (newPhase <= -M_PI)
			{
				newPhase = newPhase + 2 * M_PI;
			}
			if (ui->buttonDegrees->isChecked()) {
				newPhase = qRadiansToDegrees(newPhase);
			}
			ui->phaseLE->setText(QString::number(newPhase));
		}
		else
		{
			newPhase = M_PI - newPhase;
			if (newPhase > M_PI)
			{
				newPhase = newPhase - 2 * M_PI;
			}
			else if (newPhase <= -M_PI)
			{
				newPhase = newPhase + 2 * M_PI;
			}
			if (ui->buttonDegrees->isChecked()) {
				newPhase = qRadiansToDegrees(newPhase);
			}
			ui->phaseLE->setText(QString::number(newPhase));
		}
	}
}

void GTAEquationSinusWidget::onSearchMinimumClicked()
{
	if (_pSearchWidgetMinimum != NULL)
	{
		_pSearchWidgetMinimum->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetMinimum->show();
	}
}
void GTAEquationSinusWidget::onSearchMaximumClicked()
{
	if (_pSearchWidgetMaximum != NULL)
	{
		_pSearchWidgetMaximum->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetMaximum->show();
	}
}
void GTAEquationSinusWidget::onSearchOffsetClicked()
{
	if (_pSearchWidgetOffset != NULL)
	{
		_pSearchWidgetOffset->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetOffset->show();
	}
}
void GTAEquationSinusWidget::onSearchAmplitudeClicked()
{
	if (_pSearchWidgetAmplitude != NULL)
	{
		_pSearchWidgetAmplitude->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetAmplitude->show();
	}
}
void GTAEquationSinusWidget::onSearchPeriodClicked()
{
	if (_pSearchWidgetPeriod != NULL)
	{
		_pSearchWidgetPeriod->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetPeriod->show();
	}
}
void GTAEquationSinusWidget::onSearchFrequencyClicked()
{
	if (_pSearchWidgetFrequency != NULL)
	{
		_pSearchWidgetFrequency->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetFrequency->show();
	}
}
void GTAEquationSinusWidget::onSearchStartValueClicked()
{
	if (_pSearchWidgetStartValue != NULL)
	{
		_pSearchWidgetStartValue->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetStartValue->show();
	}
}
void GTAEquationSinusWidget::onSearchPhaseClicked()
{
	if (_pSearchWidgetPhase != NULL)
	{
		_pSearchWidgetPhase->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetPhase->show();
	}
}
void GTAEquationSinusWidget::onSearchDirectionClicked()
{
	if (_pSearchWidgetDirection != NULL)
	{
		_pSearchWidgetDirection->setSearchType(GTAGenSearchWidget::PARAMETER);
		_pSearchWidgetDirection->show();
	}
}

void GTAEquationSinusWidget::onSearchWidgetMinimumOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetMinimum->getSelectedItems();
	if (!selectedItem.getName().isEmpty())
	{
		ui->minValueLE->setText(selectedItem.getName());
	}
	_pSearchWidgetMinimum->hide();
}

void GTAEquationSinusWidget::onSearchWidgetMaximumOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetMaximum->getSelectedItems();
	if (!selectedItem.getName().isEmpty())
	{
		ui->maxValueLE->setText(selectedItem.getName());
	}
	_pSearchWidgetMaximum->hide();
}
void GTAEquationSinusWidget::onSearchWidgetOffsetOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetOffset->getSelectedItems();
	if (!selectedItem.getName().isEmpty())
	{
		ui->offsetLE->setText(selectedItem.getName());
	}
	_pSearchWidgetOffset->hide();
}
void GTAEquationSinusWidget::onSearchWidgetAmplitudeOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetAmplitude->getSelectedItems();
	if (!selectedItem.getName().isEmpty())
	{
		ui->ampLE->setText(selectedItem.getName());
	}
	_pSearchWidgetAmplitude->hide();
}
void GTAEquationSinusWidget::onSearchWidgetPeriodOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetPeriod->getSelectedItems();
	if (!selectedItem.getName().isEmpty())
	{
		ui->periodLE->setText(selectedItem.getName());
	}
	_pSearchWidgetPeriod->hide();
}
void GTAEquationSinusWidget::onSearchWidgetFrequencyOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetFrequency->getSelectedItems();
	if (!selectedItem.getName().isEmpty())
	{
		ui->frequencyLE->setText(selectedItem.getName());
	}
	_pSearchWidgetFrequency->hide();
}
void GTAEquationSinusWidget::onSearchWidgetStartValueOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetStartValue->getSelectedItems();
	if (!selectedItem.getName().isEmpty())
	{
		ui->startValueLE->setText(selectedItem.getName());
	}
	_pSearchWidgetStartValue->hide();
}
void GTAEquationSinusWidget::onSearchWidgetPhaseOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetPhase->getSelectedItems();
	if (!selectedItem.getName().isEmpty())
	{
		ui->phaseLE->setText(selectedItem.getName());
	}
	_pSearchWidgetPhase->hide();
}
void GTAEquationSinusWidget::onSearchWidgetDirectionOKClicked()
{
	GTAICDParameter selectedItem = _pSearchWidgetDirection->getSelectedItems();
	QString parametername = selectedItem.getName();
	if (!parametername.isEmpty())
	{
		if (!_newDirections->contains(parametername))
		{
			_newDirections->append(parametername);
		}
		ui->directionCB->addItem(parametername);
		setDirection(parametername);
	}
	_pSearchWidgetDirection->hide();
}

#include "GTAWgtRegisterDef.h"
REGISTER_EQUATION_WIDGET(EQ_SINUS,GTAEquationSinusWidget)
