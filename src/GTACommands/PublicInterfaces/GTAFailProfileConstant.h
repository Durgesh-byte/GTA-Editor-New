#ifndef GTAFailProfileConstant_H
#define GTAFailProfileConstant_H
#include "GTAFailureProfileBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileConstant : public GTAFailureProfileBase
{

public:
    //enum rampMode{INVERTED,RAMPUP};
    GTAFailProfileConstant();
    virtual ~GTAFailProfileConstant();
    GTAFailProfileConstant(const GTAFailProfileConstant& rhs);

    void setConst(const QString &iVal);
    QString getConst() const;    
   
   

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
    QString getSimulationName()const{return FEQ_CONST;}

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
   
private:
    QString  _Const;
   
  
   
};

#endif // GTAFailProfileConstant_H
