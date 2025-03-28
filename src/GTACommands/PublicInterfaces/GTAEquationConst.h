#ifndef GTAEQUATIONCONST_H
#define GTAEQUATIONCONST_H
#include "GTAEquationBase.h"

#pragma warning(push, 0)
#include <QString>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationConst : public GTAEquationBase
{
public:

    enum ConstType{ICD,LOCAL,NUM};

    GTAEquationConst():GTAEquationBase(GTAEquationBase::CONST){}

    GTAEquationConst(const GTAEquationConst& rhs);
    virtual ~GTAEquationConst();
    void setConstant(const QString & iConst);
    QString getConstant() const;
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
    QString getSimulationName()const{return EQ_CONST;}
    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    void setConstType(ConstType iConstType);
    ConstType getConstType() const;
    virtual void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    virtual bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    virtual bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
	


private:
    QString _ConstValue;
    ConstType _ConstType;
};

#endif // GTAEQUATIONCONST_H
