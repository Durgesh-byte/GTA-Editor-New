//GTAInvalidCommand.h

#ifndef GTAINVALIDCOMMAND_H
#define GTAINVALIDCOMMAND_H
#include <GTAActionBase.h>
class GTACommands_SHARED_EXPORT GTAInvalidCommand : public GTAActionBase
{
public:
	GTAInvalidCommand();
	virtual ~GTAInvalidCommand();
	GTAInvalidCommand(const GTAInvalidCommand& rhs);
	
	virtual QString getStatement() const;
	virtual QList<GTACommandBase*>& getChildren(void) const;
	virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
	virtual void setParent(GTACommandBase* iParent);
	virtual GTACommandBase* getParent(void) const;

	virtual GTACommandBase*getClone() const;
     /**
       * This function return false as it can't be copied and pasted independently
       * Only along with parent this command can be copied and pasted
       */
     virtual bool canBeCopied();
     /**
       * This function return false as it can't be delete independently
       * Only along with parent in can be deleted
       */
     virtual bool IsDeletable();

     /**
     * This function return true if command can be updated by the user independently
     * otherwise false, example end statements for while and if.
     */
     virtual bool  IsUserEditable();


     virtual bool canHaveChildren() const;

     bool logCanExist()const;
     virtual QStringList getVariableList() const;
     virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap);
     bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;


     void setMessage(const QString& iMessage);
     QString getMessage()const;

     QColor getForegroundColor() const;
     QColor getBackgroundColor() const;
     QFont getFont() const;

     bool hasTimeOut() const{return false;}
     bool hasPrecision() const{return false;}
     bool hasChannelInControl()const {return false;}
     void setStateExists(bool ibHasState){_createState = ibHasState;}
     bool getStateExists()const{return _createState;}

private:

    QString _Message;
    bool _createState;
	
};

#endif // GTAInvalidCommand_H
