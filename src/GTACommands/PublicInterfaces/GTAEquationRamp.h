#ifndef GTAEQUATIONRAMP_H
#define GTAEQUATIONRAMP_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationRamp : public GTAEquationBase
{
public:
    GTAEquationRamp();
    ~GTAEquationRamp();
    //GTAEquationRamp(const QString & iRamp, const QString & iOffset); 
	GTAEquationRamp(const GTAEquationRamp& rhs);
    void setRiseTime(const QString &iVal);
    void setStart(const QString &iVal);
    void setEnd(const QString &iVal);


    QString getRiseTime() const;
    QString getStart() const;
    QString getEnd() const;


    void setEndValueOffset(bool iVal);
    bool isEndValueOffset()const;


    virtual QString getStatement() const;
    virtual GTAEquationBase* getClone()const;
    bool operator ==(GTAEquationBase*& pObj) const;

    void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const;
    /**
      * This function retrieve the variable name - variable value pair
      */
    void getEquationArguments(QHash<QString,QString>& iSerialzationMap)const;
    /**
      * Thid function set the value of equation from the variable name- variable value pair
      */
    void setEquationArguments(const QHash<QString,QString>& iSerialzationMap);
    QString getSimulationName()const{return EQ_RAMP;}

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap);
    virtual void stringReplace(const QRegExp&, const QString&) {}
    virtual bool searchString(const QRegExp&, bool) const { return false; }
private:
    QString     _Start;
    QString     _RiseTime;
    QString     _End;
    bool        _isEndValueOffset;
};

#endif // GTAEQUATIONRAMP_H
