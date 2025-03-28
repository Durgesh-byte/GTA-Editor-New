#ifndef GTAActionForEach_H
#define GTAActionForEach_H
#include <GTAActionBase.h>
#include "GTAActionSetList.h"
#include "GTAActionSet.h"
#include "GTAActionSubscribe.h"
#include "GTAActionUnSubscribe.h"
#include "GTACheckValue.h"
#include "GTAStructCheck.h"

#pragma warning(push, 0)
#include <QHash>
#include <QList>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAActionForEach : public GTAActionBase
{
public:
    GTAActionForEach();
    GTAActionForEach(const QString &iAction, const QString & iComplement) : GTAActionBase(iAction,iComplement){}
    virtual ~GTAActionForEach();

    enum Delimiter {SEMICOLON,COMMA,COLON,TAB,SPACE};

    GTAActionForEach(const GTAActionForEach& rhs);
    void setPath(const QString & iPath);
    void setRelativePath(const QString & iPath);
    void setDelimiterIndex(const int &iDelimiter);
    QString getPath() const;
    QString getRelativePath() const;
    int getDelimiterIndex()const;
    QString getDelimiterValue()const;

    void setHeaderColumns(QStringList & iColumns);
    QStringList getHeaderColumns();

    bool setColumnMap();
    void getColumnMap(QList<QHash<QString, QString> > &iColumns) const;

    bool createEndCommand(GTACommandBase* & opCmd)const;
    virtual QString getLTRAStatement() const;
    virtual QString getStatement() const;
    virtual QList<GTACommandBase*>& getChildren(void) const;
    virtual bool insertChildren(GTACommandBase* pBase,const int& idPos);
    virtual void setParent(GTACommandBase* iParent);
    virtual GTACommandBase* getParent(void) const;

    virtual GTACommandBase*getClone() const;

    virtual bool hasChildren();
    virtual bool canHaveChildren() const;

    virtual QStringList getVariableList() const;
    virtual void  ReplaceTag(const QMap<QString,QString>& iTagValueMap) ;
    //bool getSCXMLConditionStatement(const QStringList &iIcdParamList, QString &oCondtionStatement, const QHash<QString, QString> &iGenToolReturnMap, const QString &iEngine = QString(), bool isGenericSCXML = false, const QStringList iLocalVarList = QStringList()) const;
    //QString getSCXMLCondition(const QString &iLhs, const QString &iRhs,const QStringList &iIcdParamList, bool isGenericSCXML, const QStringList iLocalVarList = QStringList())const;
    bool stringReplaceApplicable()const{return true;}
    void stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace);
    bool equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace) override;
    bool searchString(const QRegExp& iRegExp,bool ibTextSearch)const;
    bool expandReferences();
    bool setDumpList(const QStringList& iDumpList);
    QStringList getDumpList()const;

    QString getSCXMLStateName()const;
    bool hasLoop() const;
    void replaceUntagged();
    inline void setLogMessageList(const QList<GTAScxmlLogMessage> & iList);
    bool hasTimeOut() const{return true;}
    bool hasPrecision() const{return false;}
    bool hasChannelInControl()const;
    virtual QString getGlobalResultStatus();
    virtual bool isCompoundState(){return true;}
    virtual QString getCompounsStateId(){return QString("UserDef_While");}
    bool validateTable(QString &path);
    void setOverWriteResults(const bool& iVal);
    bool getOverWriteResults()const;
	void setCSVContent(const QList<QStringList>);
	QList<QStringList> getCSVContent() const;
	void insertColumnInCSVContent(int, const QStringList);
	void insertLineInCSVContent(const QStringList);
	void readCSVContent();
	QStringList getCSVColumnByTitle(const QString) const;
	void setRelativePathChkB(const bool& iVal);
	bool getRelativePathChkB()const;
    void setRepositoryPath(QString& path);
    QString getRepositoryPath()const;

private:
    bool setColumnList(bool isRelative);
    bool CheckAndWriteToTable(QList<GTAScxmlLogMessage>& imsgList);
    QStringList _Columns;
    QList<QHash<QString, QString> > _ColumnMap;
	QList<QStringList> _CSVContent;
    QString     _Delimiter;
    int         _DelimiterIndex;
    QString     _Path;
    bool        _OverwriteTable;
    QString     _relativePath;
	bool		_relativePathChkB;
    QString     _repositoryPath;
};

#endif // GTAActionForEach_H
