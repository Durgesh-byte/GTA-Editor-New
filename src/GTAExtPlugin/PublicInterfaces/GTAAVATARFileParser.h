#ifndef AINGTAAVATARFILEPARSER_H
#define AINGTAAVATARFILEPARSER_H

#include <QStringList>
#include "AINGTAICDParameter.h"
#include "AINGTAExtPlugin.h"
#include <QFileInfo>
#include <qdom.h>
#include "AINGTAElement.h"
#include <QHash>
#include "AINGTAPluginFileParserBase.h"
#include "AINGTAICDParameter.h"

class AINGTAEXTPLUGIN_SHARED_EXPORT AINGTAAVATARFileParser : public AINGTAPluginFileParserBase
{
    Q_OBJECT
public:
    AINGTAAVATARFileParser();
    ~AINGTAAVATARFileParser();
    virtual bool parseAndSaveFile(const QFileInfo &iAVATARFile);
    QString getElementDirPath()const {return _ElementDirPath;}
    QStringList getErrors(){return _ErrorMessages;}
private:

    enum PARAM_DIR{INPUT,OUTPUT,INVALID};
    QString getCommandType(const QString &iLine) const;
    bool makeCommandFromText(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeInvalidCommand(const QString &iText, const QString &iReason, AINGTACommandBase *&oCmd);
    bool makeSetCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeCheckCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeAckCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeWaitForCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeMsgCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeCallCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeVerCommand(const QString &iText, AINGTACommandBase *&oCmd);
    void resolveFSValue(const QString &iFSVal, QString &oFSVal);
    bool resolveParameter(const QString &iParam, AINGTAICDParameter &oResolvedParam);
    bool getParameterFromDB(const QString &iParam, AINGTAICDParameter &oDBParam);
	bool getParameterFromDBNew(const QString &iParam, AINGTAICDParameter &oDBParam);
	bool resolveParameterValue(const QString &iVal, bool isFs, AINGTAICDParameter iParam, QString &oResolvedValue);
private:
//    QString _CurrentDataDir;
    QString _CurrentDir;
    QString _ElementDirPath;
    QHash<QString, AINGTAICDParameter> _ResolvedParams;
    QHash<QString, QString> _FunctionUUIDMap;
	QHash<QString, QString> _ProcedureUUIDMap;

    struct GenericFunctionArguments
    {
        QString HMILabel;
        QString name;
        QString ID;
        QString UserSelVal;
    };


};

#endif // AINGTAAVATARFILEPARSER_H
