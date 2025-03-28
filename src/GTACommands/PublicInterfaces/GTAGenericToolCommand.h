#ifndef GTAGENERICTOOLCOMMAND_H
#define GTAGENERICTOOLCOMMAND_H

#include "GTAActionBase.h"
#include "GTAGenericFunction.h"
#include "GTAActionBase.h"
#include "GTACommands.h"
#include "GTAGenericDB.h"
#include "GTAUtil.h"
#define RETURN_CODE_TRUE "true"

class GTACommands_SHARED_EXPORT GTAGenericToolCommand :public GTAActionBase
{
public:
    GTAGenericToolCommand();
    GTAGenericToolCommand(const QString &iAction, const QString & iComplement);
    GTAGenericToolCommand(const GTAGenericToolCommand& rhs);

    QString getStatement() const;
    bool hasPrecision() const;
    bool hasTimeOut() const;
    void setHasTimeOut(const bool &val);
    // void setStatement();
    void insertFunction(GTAGenericFunction iFuncObj);
    void setDefinitions(const QList<GTAGenericParamData> &iDefinitions);
    QList<GTAGenericParamData> getDefinitions() const;
    void removeFunction(GTAGenericFunction iFuncObj);
    void setFunctions(const QList<GTAGenericFunction> &iFuncs){_SelectedFunctions = iFuncs;}
    QList<GTAGenericFunction> getFunctions()const{return _SelectedFunctions;}
    bool isWaitUntilEnabled(const QString &iReturnParamName,const QString &iReturnType) const;
    QString getElement()const;
    QList<GTAGenericFunction> getCommand() const;
    virtual bool hasChannelInControl()const {return false;}

    virtual  QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual GTACommandBase *getClone() const;
    virtual bool hasReference();
    virtual bool canHaveChildren() const;
    virtual QString getLTRAStatement()  const;
    QString getGenericArgumentInfos(QList<GTAGenericArgument>& iArgList, int j) const;
    QString getCommandPath() const ;
    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    void setToolName(const QString &iToolName);
    QString getToolName()const;

    void setToolDisplayName(const QString &iToolName);
    QString getToolDisplayName()const;
    void setSCXMLModSelected(const bool &iVal);
    bool getSCXMLModSelected() const;


    virtual bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    void setLocationInElem(const int iLocation){_Location = iLocation;}

    bool getSCXMLConditionStatement(const QString &iStateNameWithReturnParamName, const QString &iReturnParamName, const bool &iCondition,QString &oReturnCode, const QString &iReturnType) const;
    QString getSCXMLStateName() const;

    void setUserDbList(const QList<GTAGenericDB> &iDbList);
    QList<GTAGenericDB> getUserDbList()const;

    void setMismatch(bool iVal);
    bool isMismatch();

    virtual QColor getForegroundColor() const;
    void setForegroundColor(const QString& iColor);
    virtual bool isForegroundColor() const;
    virtual QColor getBackgroundColor() const;

    const QColor kBackgroudColor = QColor(Qt::white);

private:
    QList<GTAGenericFunction> _SelectedFunctions;
    QString _ToolName;
    QString _ToolDisplayName;
    QString _ToolId;
    int _Location;
    bool _hasTimeOut;
    QList<GTAGenericParamData> _Definitions;
    QList<GTAGenericDB> _UserDb;
    bool _isMismatched;
    bool _IsNew;
    bool _isSCXMLModSelected;
    QString _Color;
    bool isColor;
};

#endif // GTAGENERICTOOLCOMMAND_H
