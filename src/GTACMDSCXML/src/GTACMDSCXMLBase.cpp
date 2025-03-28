#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXMLUtils.h"

GTACMDSCXMLBase::GTACMDSCXMLBase(GTACommandBase *pCmdNode, const QString &iStateId, const QString &iTargetId)
{
    _BaseCmd = pCmdNode;
    _StateId = iStateId;
    _TargetId = iTargetId;
}

GTACMDSCXMLBase::GTACMDSCXMLBase()
{
    _BaseCmd = nullptr;
}

GTACMDSCXMLBase::~GTACMDSCXMLBase()
{
    for (auto& child : _Children)
    {
        if (child != nullptr)
        {
            delete child;
            child = nullptr;
        }
    }
}

void GTACMDSCXMLBase::setEngSettings(const QString &iEngSettings)
{
    _CurrentEngSettings = iEngSettings;
}
void GTACMDSCXMLBase::setDebugModeStatus(bool iIsDebugModeEn)
{
    _isDebugModeEn = iIsDebugModeEn;
}

void GTACMDSCXMLBase::setPreviousStateId(const QString & iPrevStateId)
{
    _PrevStateId = iPrevStateId;
}
QString GTACMDSCXMLBase::getStateId() const
{
    return _StateId;
}
QString GTACMDSCXMLBase::getTargetId() const
{
    return _TargetId;
}
QString GTACMDSCXMLBase::getPreviousStateId() const
{
    return _PrevStateId;
}
void GTACMDSCXMLBase::setStateId(const QString & iStateId)
{
    _StateId = iStateId;
}
void GTACMDSCXMLBase::setTargetId(const QString & iTargetId)
{
    _TargetId = iTargetId;
}

GTACommandBase * GTACMDSCXMLBase::getCommand() const
{
    return _BaseCmd;
}

void GTACMDSCXMLBase::setCommandName(const QString &iCmdName)
{
    _CmdName = iCmdName;
}
QString GTACMDSCXMLBase::getCommandName()
{
    return _CmdName;
}
void GTACMDSCXMLBase::append(GTACMDSCXMLBase* ipCmd)
{
    _Children.append(ipCmd);
}
QList<GTACMDSCXMLBase*> GTACMDSCXMLBase::getChildren() const
{
    return _Children;
}

void GTACMDSCXMLBase::insertParamInfoList(const GTAICDParameter &iParamInfo)
{
    _ParamInfoList.append(iParamInfo);
	if(!iParamInfo.getName().isEmpty())
		_ParamNameInfoList.append(iParamInfo.getName());
}

void GTACMDSCXMLBase::clearParamInfoList()
{
	_ParamInfoList.clear();
}


void GTACMDSCXMLBase::insertLoopInfoList(const QString loopVar,const GTAICDParameter &iParamInfo)
{
    _LoopInfoList.insert(loopVar,iParamInfo);
}

QString GTACMDSCXMLBase::getSCXMLString() const
{
    return "";
}

GTASCXMLState GTACMDSCXMLBase::getSCXMLState() const
{
    GTASCXMLState state;
    return state;
}
bool GTACMDSCXMLBase::getDebugModeStatus()const
{
    return _isDebugModeEn;
}
bool GTACMDSCXMLBase::getRepGenStatus()const
{
    return _GenNewLogFormat;
}








void GTACMDSCXMLBase::enableReportLogGen(bool &iGenReportLog)
{
    _GenNewLogFormat = iGenReportLog;
}
bool GTACMDSCXMLBase::resolveParams(GTAICDParameter &ioICDParam) const
{
    bool rc = false;
     QString tempParam = ioICDParam.getTempParamName();
	//QString tempParam = ioICDParam.getName();
    GTACMDSCXMLUtils::updateWithGenToolParam(tempParam);

    if(tempParam != ioICDParam.getTempParamName())
	//if(tempParam != ioICDParam.getName())
    {
        rc= true;
        //ioICDParam.setSignalName(tempParam);
        ioICDParam.setTempParamName(tempParam);
		//ioICDParam.setName(tempParam);
    }
	int idx = 0;
	foreach(GTAICDParameter icdParam, _ParamInfoList)
	{
		//QString paramName = QString();
		if (icdParam.getName() == tempParam)
		{
			rc = true;
			ioICDParam = _ParamInfoList[idx];
		}
		idx++;
	}
    /*if(_ParamInfoList.getName().contains(ioICDParam))
    {
        rc =true;
        int idx = _ParamInfoList.indexOf(ioICDParam);
        if(idx>=0)
        {
            ioICDParam = _ParamInfoList[idx];
        }
    }*/
    if (_LoopInfoList.keys().contains(ioICDParam.getSignalName().remove("line.").remove(QString("%2%1").arg(ioICDParam.getSignalName().split(INTERNAL_PARAM_SEP).last(),INTERNAL_PARAM_SEP))))
    {
        rc =true;
        GTAICDParameter value = _LoopInfoList.value(ioICDParam.getSignalName().remove("line.").remove(QString("%2%1").arg(ioICDParam.getSignalName().split(INTERNAL_PARAM_SEP).last(),INTERNAL_PARAM_SEP)));
        if(value.getTempParamName()!="")
        {
            ioICDParam = value;
        }
    }
    return rc;
}

QString GTACMDSCXMLBase::getEngSettings() const
{
    return _CurrentEngSettings;
}
void GTACMDSCXMLBase::enableRepitativeLogging(bool iRepeatLogs)
{
    _RepeatLogs = iRepeatLogs;
}

void GTACMDSCXMLBase::getParamInfoList(QList<GTAICDParameter> &iParamList, bool iGetForChildren)const
{
    for(const auto& param : _ParamInfoList)
    {
        iParamList.append(param);
    }
    if(iGetForChildren)
    {
        for(int i=0;i<_Children.count();i++)
        {
            GTACMDSCXMLBase *pCmdBase = _Children.at(i);
            if(pCmdBase != nullptr)
            {
                pCmdBase->getParamInfoList(iParamList,iGetForChildren);
            }
        }
    }
}
