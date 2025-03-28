#ifndef GTAFAILPROFILESAWTOOTH_H
#define GTAFAILPROFILESAWTOOTH_H
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileSawTooth : public GTAFailureProfileBase
{

public:
    //enum rampMode{INVERTED,RAMPUP};
    GTAFailProfileSawTooth();
    virtual ~GTAFailProfileSawTooth();
    GTAFailProfileSawTooth(const GTAFailProfileSawTooth& rhs);

    void setPeriod(const QString &iVal);
    void setOffset(const QString &iVal);    
    void setPhase(const QString &iVal);
   

    QString getPeriod() const;    
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
    QString getSimulationName()const{return EQ_SAWTOOTH;}

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
   
private:
    QString  _Period;
    QString  _Offset;
    QString  _Phase;
  
   
};

#endif // GTAFailProfileSawTooth_H
