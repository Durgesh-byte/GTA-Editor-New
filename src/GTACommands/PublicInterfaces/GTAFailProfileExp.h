#ifndef GTAFAILPROFILEEXP_H
#define GTAFAILPROFILEEXP_H
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileExp : public GTAFailureProfileBase
{

public:
    //enum rampMode{INVERTED,RAMPUP};
    GTAFailProfileExp();
    virtual ~GTAFailProfileExp();
    GTAFailProfileExp(const GTAFailProfileExp& rhs);

    void setStart	(const QString &iVal);
    void setStop	(const QString &iVal);
    void setTau		(const QString &iVal);
   

    QString getStart()	const;
    QString getStop()	const;
    QString getTau()	const;
   

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
    QString getSimulationName()const{return EQ_SAWTOOTH;}

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
   
private:
    QString  _start;
    QString  _stop;
    QString  _tau;
   
    //QString getRampModeType(void)const;
};

#endif // GTAEQUATIONSAWTOOTH_H
