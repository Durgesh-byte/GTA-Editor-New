#ifndef GTAACTIONCALL_H
#define GTAACTIONCALL_H
#include "GTAActionBase.h"

#pragma warning(push, 0)
#include <QHash>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAActionCall : public GTAActionBase
{
public:
    GTAActionCall();
    GTAActionCall(const QString &iAction, const QString & iComplement,const QString & iElement);
    GTAActionCall(const GTAActionCall & iObj);

    void setElement(const QString & iParameter);
    QString getElement() const;
    virtual QString getStatement() const;



    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;
    virtual GTACommandBase *getClone() const;
    virtual  bool hasReference();
    virtual bool canHaveChildren() const;
    virtual QString getLTRAStatement()  const;
    QString getFunctionWithArgument() const;
    QMap <QString,QString>  getTagValue() const;
    QMap <QString, QString> getTagParameterValue() const;
    void updateTagValue();
    void setTagValue(QMap <QString,QString> iTagValueMap) ;
    void setTagParameterValue(QMap <QString, QString> iTagParamValueMap);

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;

    GTACommandBase* getFirstNonCallChild( ) ;
    
    QString getGlobalResultStatus( );
    bool hasPrecision() const{return false ;}
    bool hasTimeOut() const{return true;}

    /* bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool replacableStrFound(const QRegExp& iRegExp)const;*/

    void setMonitorName(const QString &iMonitorName);
    QString getMonitorName() const;
    bool logCanExist() const{return false;}
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    double getTimeoutInMilliseconds();
    QString getSCXMLStateName()const;
    void setIsParallelInSCXML(bool isParallel){_isParallelInSCXML=isParallel;}
    bool isParallelInSCXML()const {return _isParallelInSCXML;}
    bool hasChannelInControl()const {return false;}
    void setTagValue(const QHash <QString,QString>& iTagValueHash) ;

    void setContinuousLoopTime(const QString &iContinuousLoopTime);
    QString getContinuousLoopTime() const;
    void setContinuousLoopTimeUnit(const QString &iContinuousLoopTimeUnit);
    QString getContinuousLoopTimeUnit() const;
    void setIsContinuousLoop(bool isContinuousLoop);
    bool isContinuousLoop() const;

private:
    QString     _Parameter;
    QMap    <QString,QString>       _MapOfVarTags;
    QMap    <QString, QString>      _MapOfTagValues;
    QString _MonitorName;
    GTACommandBase* getFirstNonCallChild( GTACommandBase* ipCall) ;
    QString getGlobalResultStatus( GTACommandBase* ipCall) ;
    bool _isParallelInSCXML;
    bool _isContinuousLoop;
    QString _continuousLoopTime;
    QString _continuousLoopTimeUnit;

};

#endif // GTAACTIONCALL_H
