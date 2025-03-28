#ifndef GTAEQUATIONSINECRV_H
#define GTAEQUATIONSINECRV_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationSineCrv : public GTAEquationBase
{

public:
    GTAEquationSineCrv();
	virtual ~GTAEquationSineCrv();
	GTAEquationSineCrv(const GTAEquationSineCrv& rhs);

    void setGain(const QString &iVal);
    void setTrignometryOperator(const QString &iVal);
    void setPulsation(const QString &iVal);
    void setPhase(const QString &iVal);
    void setOffset(const QString &iVal);

    QString getGain() const;
    QString getTrignometryOperator() const;
    QString getPulsation() const;
    QString getPhase() const;
    QString getOffset() const;

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
    QString getSimulationName()const{return EQ_SINECRV;}
    
    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
	virtual void stringReplace(const QRegExp&, const QString&) {}
	virtual bool searchString(const QRegExp&, bool) const { return false; }

private:
    QString  _Gain;
    QString  _TrigonometryOper;
    QString  _Pulsation;
    QString  _Phase;
    QString  _Offset;
};

#endif // GTAEQUATIONSINECRV_H
