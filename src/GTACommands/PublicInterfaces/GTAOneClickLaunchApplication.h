#ifndef GTAOneClickLaunchApplication_H
#define GTAOneClickLaunchApplication_H
#include "GTAActionBase.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAOneClickLaunchApplicationItemStruct
{
public:


    void setTestRig(const QString &iTestRig);
    void setToolId(const QString &iToolId);
    void setToolType(const QString &iToolType);
    void setAppName(const QString &iAppName);
    void insertOption(const QString &iOptionName, const QString &iOptionValue);
    void insertValArgs(const QString &iOptionName, const QString &iArgs);
    void insertOptionType(const QString &iOptionName, const QString &iType);
    void setConfigFile(const QString &iConfigFile);
    void setAppItem(const QString &iAppItem);
    void setKillMode(bool iKillMode);
    bool getKillMode() const;

    QString getTestRig() const;
    QString getToolId() const;
    QString getToolType() const;
    QString getAppName() const;
    QStringList getOptions() const;
    QString getConfigFile() const;
    QString getArgs(const QString &iOptionName) const;
    QString getValue(const QString &iOptionName) const;
    QString getType(const QString &iOptionName) const;
    QString getAppItem() const;

    GTAOneClickLaunchApplicationItemStruct();
    GTAOneClickLaunchApplicationItemStruct(const GTAOneClickLaunchApplicationItemStruct& rhs);

   

private:
    QString _TestRig;
    QString _ToolId;
    QString _ToolType;
    QString _AppName;
    QString _ConfFile;
    QString _AppItem;
    bool _KillMode;
    QStringList _Options;
    QHash<QString,QString> _OptionTypeMap;
    QHash<QString,QString> _OptionValMap;
    QHash<QString,QString> _OptionArgMap;
};

class GTACommands_SHARED_EXPORT GTAOneClickLaunchApplication : public GTAActionBase 
{
public:

    GTAOneClickLaunchApplication();
    GTAOneClickLaunchApplication(const GTAOneClickLaunchApplication & iCmd);
    ~GTAOneClickLaunchApplication();

    void insertLAStruct( GTAOneClickLaunchApplicationItemStruct * iLAStruct);
    void insertLAStruct( QList<GTAOneClickLaunchApplicationItemStruct *> iLstLAStruct);

    QList<GTAOneClickLaunchApplicationItemStruct *> getLaStruct() const;

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
     bool isOneClick(){return true;}
     QString getSCXMLStateName()const;

     bool hasTimeOut() const{return false;}
     bool hasPrecision() const{return false;}
     bool hasChannelInControl()const {return false;}

	 QString getApplicationName();

private:

    QList<GTAOneClickLaunchApplicationItemStruct *> _LAItemStructs;


};



#endif // GTAOneClickLaunchApplication_H
