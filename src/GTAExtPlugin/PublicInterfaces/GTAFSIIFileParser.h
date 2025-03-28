#ifndef AINGTAFSIIFILEPARSER_H
#define AINGTAFSIIFILEPARSER_H

#include <QStringList>
#include "AINGTAICDParameter.h"
#include "AINGTAExtPlugin.h"
#include <QFileInfo>
#include <qdom.h>
#include "AINGTAElement.h"
#include <QHash>
#include "AINGTAActionCall.h"
#include "AINGTAPluginFileParserBase.h"

class AINGTAEXTPLUGIN_SHARED_EXPORT AINGTAFSIIFileParser : public AINGTAPluginFileParserBase
{
    Q_OBJECT
public:
    AINGTAFSIIFileParser();
    ~AINGTAFSIIFileParser();
    bool parseAndSaveFile(const QFileInfo &iFSIIFile);
    QString getElementDirPath()const {return _ElementDirPath;}
    QStringList getErrors();
private:
    QHash<QString, QString> _NameElemMap;
    QList<AINGTAICDParameter> _ParamList;
    QString _CurretFile;
    QString _CurrentSUT;
//    QString _CurrDirPath;
    QString _ElementDirPath;
    QString _CurrentDir;
    int _CurrentFunctionCntr;
    QHash<QString,AINGTAICDParameter> _ResolvedParamList;
    QHash<QString, QString> _FunctionUUIDMap;

private:
    bool makeInvalidCommand(const QString &iText, const QString &iReason, AINGTACommandBase *&oCmd);
    void resolveParams(const QString &iSig, const QString &iAppName);
    void resolveFSValue(const AINGTAICDParameter &iParam, const QString &iUserFS, QString &oFS);
    QString resolveOpaqueSignals(QString &val);
    void createMainTestProcedure(QDomElement &iTPElem);
    void createObjects(QDomElement &iObjElem);

    void createCallCommand(const QString &iCallName, AINGTAActionCall *&oCmd);
};

#endif // AINGTAFSIIFILEPARSER_H
