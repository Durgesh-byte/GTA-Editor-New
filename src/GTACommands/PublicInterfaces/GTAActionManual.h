#ifndef GTAACTIONMANUAL_H
#define GTAACTIONMANUAL_H
#include "GTAActionBase.h"
/**
  * This class is define the manual action
  */
class GTACommands_SHARED_EXPORT GTAActionManual : public GTAActionBase
{
public:
    GTAActionManual();
    GTAActionManual(const QString &iAction);
    GTAActionManual(const GTAActionManual& rhs);
    virtual ~GTAActionManual();

    void setHTMLMessage(const QString & iHTMLMessage);
    QString getHtmlMessage()const;

    void setMessage(const QString & iMessage);
    QString getMessage() const;

    void setAcknowledgement(bool iVal);
    bool getAcknowledgetment() const;

    void setUserFeedback(bool iVal);
    bool getUserFeedback() const;

    void setReturnType(const QString & iVal);
    QString getReturnType() const;

    void setTargetMachine(const QString & iMachine);
    QString getTargetMachine() const;

    void setParameter(const QString & iParam);
    QString getParameter() const;

    void setImageName(const QString & iImgName);
    QString getImageName() const;

    inline bool IsLocalParameter() const{return _IsLocalParam;}
    inline void setParameterType(bool iLocal){_IsLocalParam = iLocal;}
    virtual QString getStatement() const;
    virtual QString getLTRAStatement()  const;

    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;

    bool logCanExist() const{return false;}
    QString getSCXMLStateName()const;

    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const {return false;}


private:
    //QString _ManualAction;
    QString _htmlMessage;
    QString _Message;
    bool _AcknowledgeMent;
    bool _UserFeedback;
    QString _ReturnType;
    QString _TargetMachine;
    QString _Parameter;
    bool _IsLocalParam;
    QString _Image;


};

#endif // GTAACTIONMANUAL_H
