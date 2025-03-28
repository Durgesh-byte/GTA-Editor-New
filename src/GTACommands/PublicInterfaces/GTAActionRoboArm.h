#ifndef GTAACTIONROBOARM_H
#define GTAACTIONROBOARM_H

#include "GTAActionBase.h"
#pragma warning(push, 0)
#include <QtXml/QDomNodeList>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAActionRoboArm : public GTAActionBase
{
public:
    GTAActionRoboArm(const QString & iActionName, const QString & iComplement, const QString & iFunctionName = QString());
    GTAActionRoboArm(const GTAActionRoboArm & iCmd);

    virtual ~GTAActionRoboArm();

    void setSequenceFile(const QString & iSequenceFile);
    QString getSequenceFile() const;

    void setFunction(const QString & iFunctionName);
    QString getFunction() const;

    void setFunctionCode(const QByteArray & iFuncodeCode);
    QByteArray getFunctionCode() const;
//    void setFunctionCode(const QList<QDomNode> & iCode);
//    QList<QDomNode> getFunctionCode() const;

    virtual QString getStatement() const;
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);

    virtual GTACommandBase*getClone() const;
    virtual bool canHaveChildren() const;

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;

    QString getSCXMLStateName()const;
    bool hasTimeOut() const{return false;}
    bool hasPrecision() const{return false;}
    bool hasChannelInControl()const {return false;}

private:
    QString _SequenceFile;
    QString _FunctionName;
    QByteArray _XmlRpcFunction;

};

#endif // GTAACTIONROBOARM_H
