#ifndef GTAFAILPROFILENOISE_H
#define GTAFAILPROFILENOISE_H
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileNoise : public GTAFailureProfileBase
{
   
public:
    GTAFailProfileNoise();
	virtual ~GTAFailProfileNoise();
	GTAFailProfileNoise(const GTAFailProfileNoise& rhs);

    void setMean(const QString &iVal);
    void setSeed(const QString &iVal);
    void setSigma(const QString &iVal);
    

    QString getMean() const;
    QString getSeed() const;
    QString getSigma() const;
   

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
    QString getSimulationName()const{return EQ_NOISE;}

    QStringList getVariableList() const  ;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;


private:
    QString  _mean;
    QString  _Seed;
    QString  _Sigma;
   
};

#endif // GTAFailProfileNoise_H
