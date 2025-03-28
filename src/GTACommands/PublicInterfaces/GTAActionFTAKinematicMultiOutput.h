#ifndef GTAActionFTAKinematicMultiOutput_H
#define GTAActionFTAKinematicMultiOutput_H

#include "GTAActionBase.h"
#include "GTAActionPrintTable.h"

#pragma warning(push, 0)
#include <QMap>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAActionFTAKinematicMultiOutput : public GTAActionBase
{
public:

    GTAActionFTAKinematicMultiOutput();

    GTAActionFTAKinematicMultiOutput(const GTAActionFTAKinematicMultiOutput& rhs);
    
    
    void setEpsilon1(const QString & ipVar);
    void setEpsilon2(const QString & ipVar);
    void setNoOfLines(const QString & ipVar);
    void setWaitPilOrder(const QString & ipVar);
    void insertInputVariable(const int& order, const QString& iInput);
    void insertMonitoredVariable(const int& order, const QString& iOutput);


    void insertTagValuepairForTemplates(const QString &iTag,QString &iVal);

    QMap<QString, QString > getTagValuePairForTemapltes() const;



    QString getEpsilon1( )const;
    QString getEpsilon2( )const;
    QString getNoOfLines( )const;
    QString getWaitPilOrder( )const;


    QString getRefFilePath() const;
    void setRefFilePath(const QString &iRefFilePath);

    QStringList getInputVariableList() const;
    QString getInputVariable(const int& iIndex) const;

    QStringList getOutputVariableList() const;
    QString getOutputVariable(const int& iIndex) const;


    QString getStatement() const;

    QStringList getRefFileData() const;
    void setRefFileData(const QStringList &iRefFileData);

    void setCurrentName(const QString &iName);
    QString getCurrentName() const;


    QList<GTACommandBase*>& getChildren(void) const;
    bool insertChildren(GTACommandBase* pBase,const int& idPos);
    void setParent(GTACommandBase* iParent);
    GTACommandBase* getParent(void) const;

    GTACommandBase*getClone() const;
    bool canHaveChildren() const;
    QStringList getVariableList() const;
    void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    QString getSCXMLStateName()const;

    void setGlobalResultStatus(const QString &iResult);
    QString getGlobalResultStatus() const;

    bool createsCommadsFromTemplate()const;
    void insertPrintTableCmd(GTAActionPrintTable *iPrintTable);
    QList<GTAActionPrintTable *> getPrintTableCmdList() const;
    bool hasPrecision() const{return false;}
    bool hasTimeOut() const{return false;}
    bool hasChannelInControl()const {return false;}

private:
    QList<GTAActionPrintTable *> _PrintTableCmdList;
    QMap<int, QString> _mapInputVars;
    QMap<int, QString> _mapMonitoredVars;
    QMap<QString, QString> _tagValuePair;
    QString _Epsilon1 ;
    QString _Epsilon2 ;
    QString _NoOfLines ;
    QString _WaitPILOrder ;
    QString _RefFilePath;    
    QStringList _RefFileData;
    QString _Name;
    QString _globalResult;
};

#endif // GTAActionFTAKinematicMultiOutput_H
