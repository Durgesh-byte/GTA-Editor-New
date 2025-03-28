#ifndef GTAEQUATIONSTEP_H
#define GTAEQUATIONSTEP_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationStep : public GTAEquationBase
{

public:
    GTAEquationStep();
	virtual ~GTAEquationStep();
	GTAEquationStep(const GTAEquationStep& rhs);

    void setPeriod(const QString &iVal);
    void setStartValue(const QString &iVal);
    void setEndValue(const QString &iVal);
    
    QString getPeriod() const;
    QString getStartValue() const;
    QString getEndValue() const;

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
    QString getSimulationName()const{return EQ_STEP;}

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

	virtual void stringReplace(const QRegExp&, const QString&) {}
	virtual bool searchString(const QRegExp&, bool) const { return false; }


private:
    QString  _Period;
    QString  _startValue;
    QString  _endValue;
};

#endif // GTAEquationStep_H
