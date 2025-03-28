
#ifndef GTAEQUATIONSINUSWIDGET_H
#define GTAEQUATIONSINUSWIDGET_H

#include <QWidget>
#include "GTAEquationUIInterface.h"
#include "GTAGenSearchWidget.h"
#include <QtMath>
#include <QPixmap>

namespace Ui {
    class GTAEquationSinusWidget;
}

class GTAEquationSinusWidget : public GTAEquationUIInterface
{
    Q_OBJECT
    //Q_INTERFACES(GTAEquationUIInterface)
public:
    explicit GTAEquationSinusWidget(QWidget *parent = 0);
    ~GTAEquationSinusWidget();

    QString getPeriod() const;
    QString getDirection() const;
    QString getStartValue() const;
    QString getMinValue() const;
    QString getMaxValue() const;
	QString getFrequency() const;
	QString getAmplitude() const;
	QString getOffset() const;
	QString getPhase() const;

    void setPeriod(const QString & iPeriod);
    void setDirection(const QString & iRampMode);
    void setStartValue(const QString & iStartValue);
    void setMinValue(const QString & iMinValue);
    void setMaxValue(const QString & iMaxValue);
	void setFrequency(const QString & iFrequencyValue);
	void setAmplitude(const QString & iAmplitudeValue);
	void setOffset(const QString & iOffsetValue);
	void setPhase(const QString & iPhaseValue);
	void changePixmaps();

    //interfaces from GTAEquationUIInterface
    bool isValid()const;
    void clear() const ;
    bool getEquation(GTAEquationBase*& pEqBase) const;
    bool setEquation(GTAEquationBase*& pEqBase);

public slots:
     void onSolidCBToggled(bool );
	 void isMinMaxClicked();
	 void isOffAmpClicked();
	 void isPeriodClicked();
	 void isFreqClicked();
	 void isPhaseClicked();
	 void isStartValDirClicked();
	 void isDegreesClicked();
	 void isRadiansClicked();
	 void minimunValueChanged();
	 void maximumValueChanged();
	 void offsetValueChanged();
	 void amplitudeValueChanged();
	 void periodValueChanged();
	 void frequencyValueChanged();
	 void phaseValueChanged();
	 void startValueChanged();
	 void directionChanged(int);

	 void onSearchMinimumClicked();
	 void onSearchMaximumClicked();
	 void onSearchOffsetClicked();
	 void onSearchAmplitudeClicked();
	 void onSearchPeriodClicked(); 
	 void onSearchFrequencyClicked();
	 void onSearchStartValueClicked();
	 void onSearchPhaseClicked();
	 void onSearchDirectionClicked();

	 void onSearchWidgetMinimumOKClicked();
	 void onSearchWidgetMaximumOKClicked();
	 void onSearchWidgetOffsetOKClicked();
	 void onSearchWidgetAmplitudeOKClicked();
	 void onSearchWidgetPeriodOKClicked();
	 void onSearchWidgetFrequencyOKClicked();
	 void onSearchWidgetStartValueOKClicked();
	 void onSearchWidgetPhaseOKClicked();
	 void onSearchWidgetDirectionOKClicked();

private:
    Ui::GTAEquationSinusWidget *ui;
	void allDisabled();
	QPalette  readOnly;
	QPalette  readWrite;
	QPixmap graphMinMaxStart;
	QPixmap graphMinMaxPhase;
	QPixmap graphOffAmpStart;
	QPixmap graphOffAmpPhase;
	QPixmap equationMinMaxPeriod;
	QPixmap equationMinMaxFreq;
	QPixmap equationOffAmpPeriod;
	QPixmap equationOffAmpFreq;

	GTAGenSearchWidget* _pSearchWidgetMinimum;
	GTAGenSearchWidget* _pSearchWidgetMaximum;
	GTAGenSearchWidget* _pSearchWidgetOffset;
	GTAGenSearchWidget* _pSearchWidgetAmplitude;
	GTAGenSearchWidget* _pSearchWidgetPeriod;
	GTAGenSearchWidget* _pSearchWidgetFrequency;
	GTAGenSearchWidget* _pSearchWidgetStartValue;
	GTAGenSearchWidget* _pSearchWidgetPhase;
	GTAGenSearchWidget* _pSearchWidgetDirection;

	QList<QString>* _newDirections;
};

#endif // GTAEquationSinusWidget_H


