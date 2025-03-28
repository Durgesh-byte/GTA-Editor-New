#ifndef GTAEQUATIONTRIANGLE_H
#define GTAEQUATIONTRIANGLE_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationTriangle : public GTAEquationBase
{

public:
    GTAEquationTriangle();
	virtual ~GTAEquationTriangle();
    GTAEquationTriangle(const GTAEquationTriangle& rhs);

    void setPeriod(const QString &iVal);
    void setMinValue(const QString &iVal);
    void setMaxValue(const QString &iVal);
    void setStartValue(const QString &iVal);
    void setDirection(const QString &iVal);

    QString getPeriod() const;
    QString getMinValue() const;
    QString getMaxValue() const;
    QString getStartValue() const;
    QString getDirection() const;

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
    QString getSimulationName()const{return EQ_TRIANGLE;}

    QStringList getVariableList() const  ;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    virtual void stringReplace(const QRegExp&, const QString&) {}
	virtual bool searchString(const QRegExp&, bool) const { return false; }

private:
    QString  _Period;
    QString  _maxValue;
    QString  _minValue;
    QString  _startValue;
    QString  _direction;
};

#endif // GTAEquationTriangle_H
//Period
//Amplitude
//Offset
//Phase
