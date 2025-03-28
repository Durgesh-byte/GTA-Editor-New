#ifndef GTACMDSCXMLBASE_H
#define GTACMDSCXMLBASE_H


#include "GTACMDSCXML.h"
#include "GTACommandBase.h"
#include "GTAICDParameter.h"
#include "GTASCXMLState.h"
#include "GTAUtil.h"


class GTACMDSCXML_SHARED_EXPORT GTACMDSCXMLBase
{

public:
    GTACMDSCXMLBase();
    GTACMDSCXMLBase(GTACommandBase *pCmdNode, const QString &iStateId, const QString &iTargetId);
    ~GTACMDSCXMLBase();

    QString getStateId() const;
    QString getTargetId() const;
    QString getPreviousStateId() const;
    void setStateId(const QString & iStateId);
    void setTargetId(const QString & iTargetId);
    void setPreviousStateId(const QString & iPrevStateId);

    GTACommandBase * getCommand() const;

    void setCommandName(const QString &iCmdName);
    QString getCommandName();
    virtual void append(GTACMDSCXMLBase* ipCmd);
    QList<GTACMDSCXMLBase*> getChildren() const;

    virtual void insertParamInfoList(const GTAICDParameter &iParamInfo);
	void clearParamInfoList();
    void insertLoopInfoList(const QString loopVar,const GTAICDParameter &iParamInfo);

    virtual QString getSCXMLString() const;
    virtual GTASCXMLState getSCXMLState()const;

    bool getDebugModeStatus() const;
    bool getRepGenStatus()const;
    void enableReportLogGen(bool &iGenReportLog);
    void setDebugModeStatus(bool iIsDebugModeEn = false);
    void setEngSettings(const QString &iEngSettings);
    QString getEngSettings() const;
    void enableRepitativeLogging(bool iRepeatLogs = false);
    void getParamInfoList(QList<GTAICDParameter> &iParamList,bool iGetForChildren = false)const;
    void setUSCXMLEnStatus(bool iEnUSCXML = false){_isUSCXMLEn = iEnUSCXML;}
    void setLocation(const QString &iLocation){_CmdLocation = iLocation;}

    void setHiddenDelay(const QString &iVal)
    {
        _HiddenDelay = iVal;
    }

private:

protected:
    GTACommandBase * _BaseCmd;

    QList<GTAICDParameter> _ParamInfoList;
    QHash<QString,GTAICDParameter> _LoopInfoList;
    QList<GTACMDSCXMLBase*> _Children;
    QString _CmdLocation;
    bool _isUSCXMLEn;
    QString _CmdName;
    QString _CurrentEngSettings;
    QString _PrevStateId;
    QString _StateId;
    QString _TargetId;
    bool _isDebugModeEn;
    bool _GenNewLogFormat;
    bool _RepeatLogs;
    QString _HiddenDelay;
	QList<QString> _ParamNameInfoList;
protected:
    bool resolveParams(GTAICDParameter &oiICDParam) const;
};

#endif // GTACMDSCXMLBASE_H

