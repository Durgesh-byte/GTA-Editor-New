#ifndef GTAEQUATIONPULSE_H
#define GTAEQUATIONPULSE_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationPulse : public GTAEquationBase
{
   
public:
    GTAEquationPulse();
	virtual ~GTAEquationPulse();
	GTAEquationPulse(const GTAEquationPulse& rhs);

    void setPeriod(const QString &iVal);
    void setMinValue(const QString &iVal);
    void setMaxValue(const QString &iVal);
    void setDirection(const QString &iVal);
    void setDutyCycle(const QString &iVal);

    QString getPeriod() const;
    QString getMinValue() const;
    QString getMaxValue() const;
    QString getDirection() const;
    QString getDutyCycle() const;

    virtual QString getStatement() const;
	virtual GTAEquationBase* getClone()const;
    bool operator ==(GTAEquationBase*& pObj) const;

    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const;
    /**
      * This function retrieve the variable name - variable value pair
      */
    void getEquationArguments(QHash<QString,QString>& iSerializationMap)const;
    /**
      * Thid function set the value of equation from the variable name- variable value pair
      */
    void setEquationArguments(const QHash<QString,QString>& iSerializationMap);
    QString getSimulationName()const{return EQ_SQUARE;}

    QStringList getVariableList() const  ;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

	virtual void stringReplace(const QRegExp&, const QString&) {}
	virtual bool searchString(const QRegExp&, bool) const { return false; }

private:
    QString  _Period;
    QString  _maxValue;
    QString  _minValue;
    QString  _direction;
    QString  _dutyCycle;
};

#endif // GTAEQUATIONPULSE_H
