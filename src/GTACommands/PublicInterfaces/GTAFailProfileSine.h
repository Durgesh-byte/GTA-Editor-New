#ifndef GTAFAILPROFILESINE_H
#define GTAFAILPROFILESINE_H
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileSine : public GTAFailureProfileBase
{
public:
    GTAFailProfileSine();
    virtual ~GTAFailProfileSine();
    GTAFailProfileSine(const GTAFailProfileSine& rhs);

    void setPeriod(const QString &iVal);
    void setAmplitude(const QString &iVal);
    void setOffset(const QString &iVal);
    void setPhase(const QString &iVal);


    QString getPeriod() const;
    QString getAmplitude() const;
    QString getOffset() const;
    QString getPhase() const;


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
    QString getSimulationName()const{return EQ_SINUS;}
private:
    QString  _Period;
    QString  _Amplitude;
    QString  _Offset;
    QString  _Phase;
};

#endif // GTAEquationSinus_H
//Period
//Amplitude
//Offset
//Phase
