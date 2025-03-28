#ifndef GTAFAILPROFILERAMP_H
#define GTAFAILPROFILERAMP_H
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileRamp : public GTAFailureProfileBase
{
   
public:
    GTAFailProfileRamp();
	virtual ~GTAFailProfileRamp();
	GTAFailProfileRamp(const GTAFailProfileRamp& rhs);

    void setStop(const QString &iVal);
    void setStart(const QString &iVal);
   
    QString getStop() const;
    QString getStart() const;
    
   

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
    QString getSimulationName()const{return EQ_RAMP;}

    QStringList getVariableList() const  ;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;


private:
    QString  _Stop;
    QString  _Start;
   
};

#endif // GTAFailProfileNoise_H
