#ifndef AINGTALIVEPALLETECOMMANDS_H
#define AINGTALIVEPALLETECOMMANDS_H
#include "AINGTADataModel.h"
#include "AINGTACommandBase.h"
#include "AINGTAICDParameter.h"
#include "AINGTAElement.h"

#pragma warning(push, 0)
#include <QObject>
#include <QThread>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QDebug>
#include <QTimer>
#include <QtScript/QScriptEngine>
#pragma warning(pop)

struct LiveParams {
    QString toolName;
    QString toolIP;
    int port;

    bool operator==(const LiveParams& other) const { return (toolName == other.toolName); }
};

class AINGTALivePalleteCommandsThread : public QThread
{
public:
    static void sleep(unsigned long msecs)
    {
        QThread::sleep(msecs);
    }
};

class AINGTADataModel_SHARED_EXPORT AINGTALivePalleteCommands : public QObject
{
    Q_OBJECT
public:
    explicit AINGTALivePalleteCommands(AINGTAElement* ipElem);
    ~AINGTALivePalleteCommands();
    void populateLiveTools(QString itoolName,QString itoolIP, int itoolPort);
    void init();
    void clearToolList() { _liveTools.clear(); }
    static AINGTACommandBase* manageSetListfromGenerationWindow(QList<AINGTAICDParameter> &iParamList);

signals:
    bool getParameterValue(AINGTAICDParameter&);
    QVariant setParameterValue(const QList<AINGTAICDParameter> &iparamList, QTimer* ipTimer);
    QVariant sendBlockingExtToolRequest(QString iIP, int iPort,QVariantList iArgs,QString iMethodname,const QTimer* ipTimer);
    bool getICDParameterList(QStringList &iVariableList, QList<AINGTAICDParameter> & oParameterList);
    void finished();

private slots:
    void onTimeout();

public slots:
    void executeAndAnalyzeElement();

private:
    QHash<QString,LiveParams> _liveTools;
    void manageExternalToolCommand(AINGTACommandBase *&ipCmd);
    void manageWaitCommand(AINGTACommandBase *&ipCmd,const QString iComplement);
    void manageCheckValueCommand(AINGTACommandBase*& ipCmd);
    void managePrintCommand(AINGTACommandBase*& ipCmd,const QString iComplement);
    void manageMathsCommands(AINGTACommandBase*& ipCmd);
    void manageSetListCommands(AINGTACommandBase*& ipCmd);
	void manageManualActCommands(AINGTACommandBase*& ipCmd);

    //utility functions
    bool valueComparator(QString ilhsVal,QString irhsVal,QString iCondition);
    bool getGlobalStatusofCheckLists(QHash<QString,AINGTAICDParameter> &iLhsList,QHash<QString,AINGTAICDParameter> &iRhsList);
    void updateCmdResultBasedOnResponse(AINGTACommandBase*& ipCmd,QVariant iResponse);
    /**
      * Author: Swarup
      * @brief: replace/update expresiion with required Math function for evaluation
      * @&oExpression: Expression which values need to be updated/modified
      * @mathCmd: command for which the math expression values need to be updated/modified
      */
    void updateMathExpression(QString &oExpression, QString mathCmd);
    /**
      * Author: Saurav
      * @brief: Set/retrieve the local parameter value
      * @iParam: Local parameter which values need to be updated/retrieve
      * @iValue: Value of the local parameter
      * @iSetTheValue: command to know whether to set/ retrieve param values
      */
    void getOrSetValueForLocalParams(QString& iParam, QString& iValue, bool iSetTheValue = false);

private:
    QTimer* _pTimer;
    bool _timedOut;
    AINGTAElement* _pElem;
    QVariant _blockingResponse;
    QHash<QString,QString> _localParams;
};

#endif // AINGTALIVEPALLETECOMMANDS_H
