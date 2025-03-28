#ifndef GTAEQUATIONBASE_H
#define GTAEQUATIONBASE_H
#include "GTACommands.h"
#include "GTACommandBase.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAEquationBase : public GTACommandBase
{
public:
    enum EquationType{CONST, GAIN, RAMP, TREPEZE, CRENELS, SINECRV, SINUS, TRIANGLE, SQUARE, STEP, SAWTOOTH};


    GTAEquationBase(EquationType iEquationType);
//    GTAEquationBase(const GTAEquationBase &Rhs);
    virtual ~GTAEquationBase();
    
	// We need to have these functions to configure Const Equations
	EquationType getEquationType()const;
	void setEquationType(const EquationType &iEqnType) { _EquationType = iEqnType; }

    virtual QString getStatement() const = 0;
    virtual GTAEquationBase* getClone()const=0;
    virtual void getFunctionStatement(QString& osFuncType,QString& osFunctionStatement)const=0;

    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual QList<GTACommandBase*>& getChildren(void) const;

    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);

    //Visual Property
    virtual QColor getForegroundColor() const;
    virtual QColor getBackgroundColor() const;
    virtual bool isForegroundColor() const { return false; };
    virtual QFont getFont() const;

    //serialization/deserialization interfaces
    /**
      * This function retrieve the variable name - variable value pair
      */
    virtual void getEquationArguments(QHash<QString,QString>& iSerialzationMap)const=0;

    /**
      * Thid function set the value of equation from the variable name- variable value pair
      */
    virtual void setEquationArguments(const QHash<QString,QString>& iSerialzationMap)=0;
    virtual QString getSimulationName()const=0;
    virtual bool canHaveChildren() const;

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool logCanExist() const{return false;}
    QString getCommandTypeForDisplay() const {return QString("Equation");}
    bool hasChannelInControl()const {return false;}

    virtual void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    virtual bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace);
    virtual bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    
    virtual void setSolidState(const QString &iIsSolid){sIsSolid = iIsSolid;}
    virtual void setContinousState(const QString &iIsContinous){sIsContinous = iIsContinous;}
    virtual QString getSolidState()const {return sIsSolid;}
    virtual QString getContinousState()const {return sIsContinous;}

    virtual bool operator ==(GTAEquationBase*& pObj) const = 0;

	bool getIsVmac() const { return _isVmac; }
	void setIsVmac(const bool &iIsVmac) { _isVmac = iIsVmac; }

private:
    EquationType _EquationType;
    QString sIsSolid;
    QString sIsContinous;
	bool _isVmac;    
};

#endif // GTAEQUATIONBASE_H
