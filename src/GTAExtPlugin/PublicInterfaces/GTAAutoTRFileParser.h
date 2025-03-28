#ifndef AINGTAAUTOTRFILEPARSER_H
#define AINGTAAUTOTRFILEPARSER_H

#include <QStringList>
#include "AINGTAICDParameter.h"
#include "AINGTAExtPlugin.h"
#include <QFileInfo>
#include <qdom.h>
#include "AINGTAElement.h"
#include <QHash>
#include "AINGTAPluginFileParserBase.h"

class AINGTAEXTPLUGIN_SHARED_EXPORT AINGTAAutoTRFileParser : public AINGTAPluginFileParserBase
{
    Q_OBJECT
public:
    AINGTAAutoTRFileParser();
    ~AINGTAAutoTRFileParser();
    bool parseAndSaveFile(const QFileInfo &iAutoTRFile);
    QString getElementDirPath()const {return _ElementDirPath;}
    QStringList getErrors(){return _ErrorMessages;}
private:
    QString getCommandType(const QString &iLine) const;
    bool makeCommandFromText(const QString &iText, AINGTACommandBase *&oCmd);
    QString getValueType(const QString &iVal);

    bool makeSetCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeMatlabCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeCheckCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeWaitCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeTextCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeObserveCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeInvalidCommand(const QString &iText, const QString &iReason, AINGTACommandBase *&oCmd);
    bool makeLogCommand(const QString &iText, AINGTACommandBase *&oCmd);
    bool makeTitleCommand(const QString &iTitleText, AINGTACommandBase *&oCmd);
    bool makeCallCommand(const QString &iText, AINGTACommandBase *&oCmd);
private:
//    QString _CurrentDataDir;
    QString _CurrentDir;
    QStringList _EnumsUsed;
    QString _ElementDirPath;
    QHash<QString, QString> _FunctionUUIDMap;
};

#endif // AINGTAAUTOTRFILEPARSER_H
