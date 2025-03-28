/*****************************************************************//**
 * \file   GTASCXMLControllerNew.h
 * \brief  
 * 
 * \author 
 * \date   August 2022
 *********************************************************************/
#ifndef GTASCXMLCONTROLLERNEW_H
#define GTASCXMLCONTROLLERNEW_H

#include "GTAElement.h"
#include "GTASCXMLUtils.h"
#include "GTASCXMLSCXML.h"
#include "GTACommandList.h"
#include "GTAICDParameter.h"
#include "GTAAppController.h"
#include "GTACMDSCXMLBase.h"
#include "GTACMDSCXMLTitle.h"

#pragma warning(push, 0)
#include <QList>
#include <QStringList>
#pragma warning(pop)

class GTASCXLMParamInfo
{
    QString _appName;
    QHash<QString,QString> busMediaInfo;
    bool isInitNeeded;

public:
    void insertParamInfo(const QString &iAppName, const QString &iBusName, const QString &iMedia, bool iIsInitNeeded)
    {
        _appName = iAppName;
        isInitNeeded = iIsInitNeeded;
        if(!busMediaInfo.contains(iBusName))
        {
            busMediaInfo.insert(iBusName,iMedia);
        }
    }
    bool operator ==(const GTASCXLMParamInfo & iObj) const
    {
        return (this->_appName == iObj._appName);
    }
    QString getId(){return _appName;}
    bool getIsInitNeeded(){return isInitNeeded;}
    QString getJsonExpr() const
    {
        QStringList buses = busMediaInfo.keys();
        QStringList content;
        content.append(QString("'Name':'%1'").arg(_appName));
        for(int i=0;i<buses.count();i++)
        {
            QString bus = buses.at(i);
            QString media = busMediaInfo.value(bus);
            content.append(QString("'%1':{'Media':'%2'}").arg(bus,media));
        }

        return QString("{%1}").arg(content.join(","));
    }
};

class GTASCXMLControllerNew
{

    GTAElement *_pElement;
    QString _LastError;
    QString _elementName;
    QString _ScxmlTemplatePath;

    QHash<QString, GTAICDParameter> _icdParamList;
    QList<QString> _CmdParameterList;

    QList<GTACMDSCXMLBase *> _pCmdSCXMLList;

    GTAAppController *_pAppCtrl;
    QStringList _EngineChannelSignals;
    int stateIdCntr;
    int titleIdCntr;
    QList<QHash<QString,QString> > _LoopID;
    QHash<QString,QString> _InternalCmdID;
    QHash<QString,QString> _InternalStateID;
    void getFilteredVariableList(QStringList variablesTemp,QStringList &variables, const QString &iInstanceName);


    enum paramType {PARAM_ICD=1,PARAM_LOCAL=2, PARAM_NUM=3,PARAM_STRING=4,PARAM_UNKNOWN=5};

public:
    GTASCXMLControllerNew(const QString &iTemplatePath);
    ~GTASCXMLControllerNew();

    bool exportToScxml(const GTAElement * ipElement,const QString &iFilePath,bool iHasUnsubscibeAtStart,
                       bool iHasUnsubscibeAtEnd,QStringList& iVarList, GTAAppController::ExecutionMode mode = GTAAppController::NORMAL_MODE);
    QString getLastError(){return _LastError;}

    void setUnsubscibeFileParamOnly(const bool unsubscibeFileParamOnly);
    bool getUnsubscribeFileParamOnly() const { return _unsubscribeFileParamOnly; }


private:

	bool getExecutionMode();
    void updatedVariableList(QStringList &iVarlist, QStringList &oVarList);

    void setElementName(GTAElement* pElement,QString iSaveFilePath);

    void GetParamList(QStringList &iVariableList);
    void GroupElementByTitleComplete(const GTAElement *pElem, QStringList &oVariableList, GTAElement *oGroupedElem, QStringList &checkforRep);
    void createCommandGroupedByTitle(GTACommandBase*& ipCmd, GTAActionParentTitle *&ipTitleCmd,QStringList &oVariableList, QStringList &checkforRep);

    bool getCMDStateChartForElement(const GTAElement *pElement);
    bool getCMDStateChartForCommand(const QList<GTACommandBase*> ipCmdList, GTACMDSCXMLBase *ioCurrentTitle, QString &iPreviousStateID);

private:
	GTAElement* pTitleBasedElement;
    bool _executionMode;
    QStringList _VarList;
    /*
    * \var QStringList CurrentsubscribedPIR
    * \brief Contains used PIR Paramaters for initialize/release 
    */
    QStringList CurrentsubscribedPIR;

    /**
    * \var QHash<QString, QString> LinePIRParameters
    * \brief Contains each line.PIR and the PIR Parameters inside
    */
    QHash<QString, QString> LinePIRParameters;
    bool _unsubscribeFileParamOnly;
};

#endif // GTASCXMLCONTROLLERNEW_H
