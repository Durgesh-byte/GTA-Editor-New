#ifndef GTAEQUATIONSINUS_H
#define GTAEQUATIONSINUS_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QString>
#include <QtMath>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationSinus : public GTAEquationBase
{

public:
    GTAEquationSinus();
    virtual ~GTAEquationSinus();
    GTAEquationSinus(const GTAEquationSinus& rhs);

    void setPeriod(const QString &iVal);
    void setMaxValue(const QString &iVal);
    void setMinValue(const QString &iVal);
    void setStartValue(const QString &iVal);
    void setDirection(const QString &iVal);
	void setFrequency(const QString &iVal);
	void setAmplitude(const QString &iVal);
	void setOffset(const QString &iVal);
	void setPhase(const QString &iVal);
	void setIsDegrees(const bool isDegrees);

    QString getPeriod() const;
    QString getMaxValue() const;
    QString getMinValue() const;
    QString getStartValue() const;
    QString getDirection() const;
	QString getFrequency() const;
	QString getAmplitude() const;
	QString getOffset() const;
	QString getPhase() const;
	bool getIsMinMax() const;
	bool getIsPeriod() const;
	bool getIsStartDir() const;
	bool getIsDegrees() const;

	void phaseToStartDir();
	void startDirToPhase();
	void ampOffToMinMax();
	void minMaxToAmpOff();
	void periodToFreq();
	void freqToPeriod();

    virtual QString getStatement() const;
    virtual GTAEquationBase* getClone()const;
    bool operator ==(GTAEquationBase*& pObj) const;

    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const;
    /**
      * This function retrieves the variable name - variable value pair
      */
    void getEquationArguments(QHash<QString,QString>& iSerialzationMap)const;
    /**
      * This function sets the value of equation from the variable name- variable value pair
      */
    void setEquationArguments(const QHash<QString,QString>& iSerialzationMap);
    QString getSimulationName()const{return EQ_SINUS;}

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    virtual void stringReplace(const QRegExp&, const QString&) {}
    virtual bool searchString(const QRegExp&, bool) const { return false; }


private:
	bool _isMinMax;
	bool _isPeriod;
	bool _isStartDir;
	bool _isDegrees;
    QString  _period;
	QString  _frequency;
    QString  _maxValue;
    QString  _minValue;
	QString  _amplitude;
	QString  _offset;
    QString  _startValue;
    QString  _direction;
	QString  _phase;
};

#endif // GTAEquationSinus_H
