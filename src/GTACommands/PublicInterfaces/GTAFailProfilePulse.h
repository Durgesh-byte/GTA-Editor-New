#ifndef GTAFAILPROFILEPULSE_H
#define GTAFAILPROFILEPULSE_H
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfilePulse : public GTAFailureProfileBase
{
   
public:
    GTAFailProfilePulse();
	virtual ~GTAFailProfilePulse();
	GTAFailProfilePulse(const GTAFailProfilePulse& rhs);

    void setPeriod(const QString &iVal);
    void setOffset(const QString &iVal);
    void setPhase(const QString &iVal);
    void setDutyCycle(const QString &iVal);

    QString getPeriod() const;
    QString getOffset() const;
    QString getPhase() const;
    QString getDutyCycle() const;

    virtual QString getStatement() const;
	virtual GTAFailureProfileBase* getClone()const;

    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const;
    /**
      * This function retrieve the variable name - variable value pair
      */
    void getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const;
    /**
      * Thid function set the value of equation from the variable name- variable value pair
      */
    void setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap);
    QString getSimulationName()const{return EQ_SQUARE;}

    QStringList getVariableList() const  ;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;


private:
    QString  _Period;
   
    QString  _Offset;
    QString  _Phase;
    QString  _dutyCycle;
};

#endif // GTAFailProfilePulse_H
