#ifndef GTAONECLICK_H
#define GTAONECLICK_H
#include "GTAActionBase.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAOneClick : public GTAActionBase
{
public:
    virtual ~GTAOneClick();

    GTAOneClick(const QString & iActionName, const QString & iComplement);
    GTAOneClick(const GTAOneClick & iCmd);

    void setConfFile(const QString & iConfFile);
    QString getConfFile() const;


    void setDescription(const QString &iDescription);

    QString getDescription() const;

    void setConfigName(const QString & iConfigName);
    QString getConfigName() const;

    void setTestRigName(const QString & iTestRigName);
    QString getTestRigName() const;

    void setTestRigType(const QString & iTestRigType);
    QString getTestRigType() const;

    void setApplicationName(const QString & iApplicationName);
    QString getApplicationName() const;

    void setOptionValueAndType(const QString &iOptionName,const QStringList & iOptionValue,const QString & iOptionType);
    QStringList getOptionNames() const;
    QStringList getValueForOption(const QString & iOptionName) const;
    QString getTypeofOption(const QString & iOptionName) const;


    virtual QString getStatement() const;
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);

    virtual GTACommandBase* getClone() const;
    bool canHaveChildren() const;

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap);
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;

    bool getIsStartEnv()const;
    bool getIsStopTestConfig()const;
    bool getIsStopAlto() const;

    void setIsStartEnv(bool iVal);
    void setIsStopTestConfig(bool iVal);
    void setIsStopAlto(bool iVal);


    QStringList getDirNamesofOption(const QString &iOptionName) const;
    QString getIniFileNameofOption(const QString &iOptionName) const;
    QHash<QString,bool> getDirChkStatusofOption(const QString &iOptionName) const;

    void setDirNamesofOption(const QString &iOptionName,const QStringList &iDirNames);
    void setIniFileNameofOption(const QString &iOptionName, const QString &iIniFileName);
    void setDirChkStatusofOption(const QString &iOptionName, const QHash<QString,bool> &iDirChkStatus);
    QStringList getSelectedDirNames(const QString &iOptionName) const;

    QString getSCXMLStateName()const;

     bool isOneClick(){return true;}

     bool hasTimeOut() const{return false;}
     bool hasPrecision() const{return false;}
     bool hasChannelInControl()const {return false;}

private:

    QString _ConfFile;
    QString _ConfigName;
    QString _TestRigName;
    QString _ApplicationName;
    QString _TestRigType;
    QString _Description;
    QStringList _OptionNamesList;
    QHash<QString,QString> _OptionTypeMap;
    QHash<QString,QStringList> _OptionValueMap;
    QHash<QString,QStringList> _DirNames;
    QHash<QString,QString> _IniFileName;



    bool _IsStartEnv;
    bool _IsStopTestConfig;
    bool _IsStopAlto;
    QHash<QString,QHash<QString, bool> > _DirChkStatus;

};

#endif // GTAONECLICK_H
