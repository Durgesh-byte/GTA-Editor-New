#ifndef GTAFAILPROFILEIDLE_H
#define GTAFAILPROFILEIDLE_H
#include "GTAFailureProfileBase.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailProfileIdle : public GTAFailureProfileBase
{
public:
    GTAFailProfileIdle(); 
	GTAFailProfileIdle(const GTAFailProfileIdle& rhs);
	virtual ~GTAFailProfileIdle();
	GTAFailureProfileBase* getClone()const;

    

    QString getStatement() const;
    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const;
    void getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const;
    void setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap);
    QString getSimulationName()const{return EQ_IDLE;}

    QStringList getVariableList() const  ;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

private:
  
  
};

#endif // GTAFailProfileIdle_H
