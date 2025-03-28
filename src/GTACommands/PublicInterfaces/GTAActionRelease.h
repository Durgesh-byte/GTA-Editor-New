#ifndef GTAACTIONRELEASE_H
#define GTAACTIONRELEASE_H
#include "GTAActionBase.h"
#include "GTACommandBase.h"
class GTACommands_SHARED_EXPORT GTAActionRelease : public GTAActionBase
{
public:
    GTAActionRelease();
    GTAActionRelease(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    GTAActionRelease(const GTAActionRelease & iObj);

    void setParameterList(const QStringList & iParam);
    QStringList getParameterList() const;
    virtual QString getStatement() const;

    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual bool canHaveChildren() const;
    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
        bool getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &iGenToolReturnMap, const QString &iEngine = QString(), bool isGenericSCXML = false) const;

    virtual GTACommandBase*getClone() const;
    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;

    QString getSCXMLStateName()const;
    bool hasTimeOut() const{return false;}
    bool hasPrecision() const{return false;}
    bool hasChannelInControl() const;
	

private:
    QStringList _Parameter;
};

#endif // GTAACTIONRELEASE_H
