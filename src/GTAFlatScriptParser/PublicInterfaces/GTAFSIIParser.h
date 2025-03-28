#ifndef AINGTAFSIIPARSER_H
#define AINGTAFSIIPARSER_H
#include <QFileInfoList>
#include "AINGTAFSIICommandBase.h"
#include "AINGTAFSIIFunctionCall.h"
#include "AINGTAFSIIFile.h"
#include <QDomDocument>
#include <QList>
#include "AINGTAFlatScriptParser.h"
#include <QHash>
#include <QFile>
#include <QIODevice>


class AINGTAFlatScriptParser_LIBRARY_SHARED_EXPORT AINGTAFSIIParser
{
public:
    AINGTAFSIIParser();
//    AINGTAFSIIParser(const QString &iFSIIScriptPath, const QString &iErrorPath);

    bool parseFlatScriptFile(const QFileInfo &iFile);
    void createFunction(QDomNode &funcNode,AINGTAFSIIFunction &oFunction);
    AINGTAFSIICommandBase* createCommand(QDomNode &cmdNode);
    void createTestProcedure(QDomNode &funcNode, QList<AINGTAFSIIFunctionCall> &oFunctionCalls, AINGTAFSIIFile &iObjFile);
    void processSignal(QString &ioName, AINGTAFSIISignal &oSignal);
    QList<AINGTAFSIIFile> getFSIIData();
    QHash<QString, AINGTAFSIISignal> getFSParameters();
    QString getGTAParamName(const QString &iName);
    bool parseFiles(const QString &iFSIIScriptPath, const QString &iErrorPath);
    bool parseFile(const QFileInfo &iFSIIFileInfo);
    bool setLogPath(const QString &iErrorPath);
    AINGTAFSIIFile getFileFromName(QString &iName);
    QString getValidityValue(const QString &iName,const QString &iGTAName, const QString &iFSVal);

private:
    QString getShortName(const QString &iLongName);

private:
    QFileInfoList _FileList;
    QList <AINGTAFSIIFile> _FS2Data;
    QHash <QString,QString> _CRDCToGTAParamMap;
    QHash <QString, AINGTAFSIISignal> _FSParameters;
    QFile _LogFile;
    static int _Counter;
	AINGTAFSIIFile _CurrentObjFile;
};

#endif // AINGTAFSIIPARSER_H
