#ifndef GTAACTIONSET_H
#define GTAACTIONSET_H
#include "GTAActionBase.h"
class GTAEquationBase;
class  GTACommands_SHARED_EXPORT GTAActionSet : public GTAActionBase
{
public:
    GTAActionSet();
    ~GTAActionSet();
    GTAActionSet(const GTAActionSet& rhs);
    GTAActionSet(const QString &iAction, const QString & iComplement);
    void setParameter(const QString & iParameter);
    void setEquation(GTAEquationBase *& ipEquation);

    void setFunctionStatus(const QString& iFs);
    QString getFunctionStatus()const;

    void setSDIStatus(const QString& iSDIStatus);
    QString getSDIStatus()const;

    QString getParameter() const;
    QString getValue() const;
   
    GTAEquationBase * getEquation() const;
    virtual QString getStatement() const;
    virtual QString getLTRAStatement()  const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;

    virtual QStringList GTAActionSet::getVariableList() const;
    virtual void ReplaceTag(const QMap<QString,QString> & iTagValueMap);
    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;

    bool getIsSetOnlyFSFixed() const;
    void setIsSetOnlyFSFixed(const bool iIsSetOnlyFS = false);

    bool getIsSetOnlyFSVariable() const;
    void setIsSetOnlyFSVariable(const bool iIsSetOnlyFS = false);

    bool getIsSetOnlyValue() const;
    void setIsSetOnlyValue(const bool IsSetOnlyValue = false);

    bool getIsSetOnlySDI() const;
    void setIsSetOnlySDI(const bool iIsSetOnlySDI = false);

    bool hasTimeOut() const{return true;}
    bool hasPrecision() const{return false;}
    bool hasChannelInControl()const;

	// Inline function to configure if the parameter has a FunctionalStatus or not
	inline void setIsParamWithoutFS(const bool& iStatus) { _IsParamWithoutFS = iStatus; }
	inline bool getIsParamWithoutFS() const { return _IsParamWithoutFS; }

    void setAutoAddExternal(bool isAutoAddExt) { _autoExternalAction = isAutoAddExt; };
    bool autoAddExternal() { return _autoExternalAction; };

private:
    GTAEquationBase *  _pEquation;
    QString               _Parameter;
    QString               _VmacForceType;
    QString               _FunctionalStatus;
    bool                  _IsSetOnlyFSFixed; 
    bool                  _IsSetOnlyFSVariable;
    QString               _SDIStatus;
    bool                  _IsSetOnlySDI;
    bool                  _IsSetOnlyValue;
	bool				  _IsParamWithoutFS;
    bool                  _autoExternalAction = false;
};

#endif // GTAACTIONSET_H
