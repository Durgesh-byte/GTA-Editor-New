#ifndef GTAActionElse_H
#define GTAActionElse_H
#include <GTAActionBase.h>
class GTACommands_SHARED_EXPORT GTAActionElse : public GTAActionBase
{
public:
	GTAActionElse(const QString & iActionName, const QString & iComplmentName);
	virtual ~GTAActionElse();
	GTAActionElse (const GTAActionElse& rhs);
	virtual GTACommandBase*getClone() const;

	virtual QString getStatement() const;
	virtual QList<GTACommandBase*>& getChildren(void) const;
	virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
	virtual void setParent(GTACommandBase* iParent);
	virtual GTACommandBase* getParent(void) const;
	bool isMutuallExclusiveStatement()const{return true;}

     virtual bool canBeCopied();
     virtual bool IsDeletable();

     virtual bool hasChildren();
     virtual bool canHaveChildren() const;
    
     virtual QStringList getVariableList() const;
     virtual void ReplaceTag(const QMap<QString,QString>& iTagValueMap);
     bool expandReferences();
     bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
     bool hasPrecision() const{return false;}
     bool hasTimeOut() const{return false;}
     bool hasChannelInControl()const {return false;}
     bool logCanExist() const {return false;}
     virtual QString getGlobalResultStatus();
private:
	

};

#endif // GTAActionElse_H
