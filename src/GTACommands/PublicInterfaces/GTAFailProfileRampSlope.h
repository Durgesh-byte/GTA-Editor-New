#ifndef GTAFAILPROFILERAMPSLOPE_H
#define GTAFAILPROFILERAMPSLOPE_H
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileRampSlope : public GTAFailureProfileBase
{
   
public:
    GTAFailProfileRampSlope();
	virtual ~GTAFailProfileRampSlope();
	GTAFailProfileRampSlope(const GTAFailProfileRampSlope& rhs);

    void setOffset(const QString &iVal);
    void setSlope(const QString &iVal);
   
    QString getOffset() const;
    QString getSlope() const;
    
   

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
    QString getSimulationName()const{return FEQ_RAMPSLOPE;}

    QStringList getVariableList() const  ;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;


private:
    QString  _Offset;
    QString  _Slope;
   
};

#endif // GTAFailProfileNoise_H
