#ifndef GTAFAILUREPROFILEBASE_H
#define GTAFAILUREPROFILEBASE_H
#include "GTACommands.h"
#include "GTACommandBase.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAFailureProfileBase : public GTACommandBase
{
public:
    enum EquationType{IDLE,CONST,SINUS,EXPONENT,SAWTOOTH,PULSE,NOISE,STOPTRIGGER,RAMP,RAMPSLOPE};
    enum Mode{STOP_TRIGGER,DURATION};

    GTAFailureProfileBase(EquationType iEquationType);
	virtual ~GTAFailureProfileBase();
    EquationType getEquationType()const;

    virtual QString getStatement() const = 0;
	virtual GTAFailureProfileBase* getClone()const=0;
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
    virtual void getFailureProfileArguments(QHash<QString,QString>& iSerialzationMap)const=0;

    /**
      * Thid function set the value of equation from the variable name- variable value pair
      */
    virtual void setFailureProfileArguments(const QHash<QString,QString>& iSerialzationMap)=0;
    virtual QString getSimulationName()const=0;
    virtual bool canHaveChildren() const;

    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool logCanExist() const{return false;}
    QString getCommandTypeForDisplay() const {return QString("Failure Profile");}

    virtual Mode getMode()const{return DURATION;}
    bool hasChannelInControl()const {return false;}
    
private:
    EquationType _EquationType;
    
};

#endif // GTAEQUATIONBASE_H
