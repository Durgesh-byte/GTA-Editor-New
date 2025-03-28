#include "GTAActionForEach.h"
#include "GTAActionForEachEnd.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QHash>
#include <QFile>
#include <QDebug>
#pragma warning(pop)

#pragma warning(disable:4239)
//defining a macro to supress qdebugs during execution
//comment the line below to see qdebug messages (only on release)
//DEV: add a message handler class for qdebug messages and use this same macro to manipulate messages
#define QT_NO_DEBUG_OUTPUT

GTAActionForEach::GTAActionForEach()
{
    setAction(ACT_CONDITION);
    setComplement(COM_CONDITION_FOR_EACH);
	if (!_OverwriteTable) {
		readCSVContent();
	}
}

GTAActionForEach::GTAActionForEach(const GTAActionForEach& rhs):GTAActionBase(rhs)
{
    this->setParent(NULL);
    

    //from GTAActionForEach
    this->setRepositoryPath(rhs.getRepositoryPath());
    this->setComment(rhs.getComment());
	this->setRelativePathChkB(rhs.getRelativePathChkB());
    if (!getRelativePathChkB()) {
        this->setPath(rhs.getPath());
    }
    this->setRelativePath(rhs.getRelativePath());
    this->setDelimiterIndex(rhs.getDelimiterIndex());
    this->setOverWriteResults(rhs.getOverWriteResults());
    setColumnMap();

    setDumpList(rhs.getDumpList());

    QList<GTACommandBase*> lstChildrens = rhs.getChildren();
    for (int i=0;i<lstChildrens.size();++i)
    {
        GTACommandBase* pCurrentChild = lstChildrens.at(i);
        GTACommandBase* pClone = nullptr;
        if (pCurrentChild!=nullptr)
        {
            pClone=pCurrentChild->getClone();
            if (pClone)
                pClone->setParent(this);
        }
        this->insertChildren(pClone,i);
    }
    readCSVContent();
}

GTACommandBase* GTAActionForEach::getClone() const
{
	GTACommandBase* pBase = new GTAActionForEach(*this);
    return pBase;
}

QString GTAActionForEach::getGlobalResultStatus()
{
    QString data = "NA";

    QList<GTACommandBase *> childrens = getChildren();

    for(int i=0;i<childrens.count();i++)
    {
        GTACommandBase *pCmd = childrens.at(i);
        QString data1 = pCmd->getGlobalResultStatus();
        if(!data1.trimmed().isEmpty())
        {
            if(data1 == "KO")
            {
                data = data1;
                break;
            }
            else if(data1 != "NA")
            {
                data = data1;
            }
        }
    }
    return data;
}


GTAActionForEach::~GTAActionForEach()
{
    if (!_lstChildren->isEmpty())
    {
        for(int i=0;i<_lstChildren->size();++i)
        {
            GTACommandBase* pCommand = _lstChildren->at(i);
            if (NULL!=pCommand)
            {
                delete pCommand;
                pCommand=NULL;
            }
        }
        _lstChildren->clear();
    }
}

void GTAActionForEach::setPath(const QString & iPath)
{
    _Path = iPath;
}

void GTAActionForEach::setRelativePath(const QString & iPath)
{
		_relativePath = iPath;
}

void GTAActionForEach::setDelimiterIndex(const int & iDelimiter)
{
    _DelimiterIndex = iDelimiter;
}

QString GTAActionForEach::getPath() const
{
    return _Path;
}

QString GTAActionForEach::getRelativePath() const
{
    return _relativePath;
}

int GTAActionForEach::getDelimiterIndex() const
{
    return _DelimiterIndex;
}

void GTAActionForEach::setOverWriteResults(const bool& iVal)
{
    _OverwriteTable = iVal;
}

bool GTAActionForEach::getOverWriteResults()const
{
    return _OverwriteTable;
}

void GTAActionForEach::setRelativePathChkB(const bool& iVal)
{
	_relativePathChkB = iVal;
}

bool GTAActionForEach::getRelativePathChkB()const
{
	return _relativePathChkB;
}

void GTAActionForEach::setRepositoryPath(QString& path)
{
    _repositoryPath = path;
}

QString GTAActionForEach::getRepositoryPath()const
{
    return _repositoryPath;
}

QStringList GTAActionForEach::getHeaderColumns()
{
    return _Columns;
}

bool GTAActionForEach::setColumnMap()
{
    bool rc = false;
    if (getPath()!= "")
    {
        rc = setColumnList(false);
        return rc;
    }
    else if (getRelativePath() != "")
    {
        rc = setColumnList(true);
        return rc;
    }
    return rc;
}

QString GTAActionForEach::getDelimiterValue() const
{
    int index = getDelimiterIndex();
    switch (index)
    {
    case 0: return ";";
    case 1: return ",";
    case 2: return ":";
    case 3: return "\t";
    default: return " ";
        break;
    }
}

bool GTAActionForEach::setColumnList(bool isRelative)
{
    bool rc = false;
    QString FileToParse;
    if (isRelative)
    {
        QString repoPath = getRepositoryPath();
        FileToParse = repoPath + getRelativePath();
    }
    else
    {
        FileToParse = getPath();
    }
	
    QString delim = getDelimiterValue();

    QFile fileObj(FileToParse);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);

    if (rc)
    {
        int lineCount = 0;
        while(!fileObj.atEnd())
        {
            lineCount++;
            QString line = fileObj.readLine();
            if(! line.isEmpty() && rc)
            {
                if (lineCount == 1)
                {
                    //_Columns = line.split(delim);
                    foreach(QString header,line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), ""))
                    {
                        // the column header should not be empty
                        if (header.isEmpty())
                        {
                            rc = false;
                            break;
                        }
                        //replacing '.' with '_' to manage triplets causing ECMA error during run
                        _Columns.append(header.replace(".","_").simplified().trimmed());
                    }
                    continue;
                }
                QHash<QString,QString> columnMap;
                QStringList columns = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");
                //ignore rows of unequal length to the header
                if (_Columns.count() == columns.count())
                {
                    for (int i=0;i<columns.count();i++)
                    {
                        QString column = columns.at(i);
                        columnMap.insertMulti(_Columns.at(i),column.simplified().trimmed());
                    }
                    _ColumnMap.append(columnMap);
                }                
            }
        }
        fileObj.close();
    }
    return rc;
}

bool GTAActionForEach::validateTable(QString &path)
{
    bool rc = false;
    QFile fileObj(path);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);
    
	//Updated to remove - and " and '
	QRegExp rx("([!/`%@&~\\^\\*\\|\\[\\]\\(\\)\\{\\}\\?\\]+)");
    if (rc)
    {
        while (!fileObj.atEnd())
        {
            QString line = fileObj.readLine();
            if (!line.isEmpty())
            {
                if (!line.contains(rx))
                {
                    rc =true;
                }
                else
                {    
                    rc =false;
                    break;
                }
            }
        }
        QString filepath = path;
        QRegExp expr("(^[a-zA-Z0-9_]+[.]{1}[A-Za-z]{3}$)");
        QStringList filenameparts = filepath.split("/");
        QString filename = filenameparts.last();

        //testing any filename in the pattern "xyz.abc"
        if (!filename.contains(expr))
            rc = false;
    }
    return rc;
}

void GTAActionForEach::getColumnMap(QList<QHash<QString, QString> > &iColumns) const
{
    iColumns = _ColumnMap;
}

QString GTAActionForEach::getStatement() const
{
    QString oAction;
    QString path;
    if(!getRelativePath().isEmpty())
    {
         path = getRelativePath();
    }
    else
    {
        path = getPath();
    }
    //path.remove(".csv");
    QStringList filename = path.split("/");
    oAction = QString("%1 row in %2").arg(getComplement(),filename.at(filename.count()-1));
    return oAction;
}

QString GTAActionForEach::getLTRAStatement() const
{
    QString oAction = "";
    if(getPath().isEmpty())
    {
         oAction = QString("#c#%1#c# row in %2").arg(getComplement().toUpper(),getRelativePath().split("/").last());
    }
    else
    {
        oAction = QString("#c#%1#c# row in %2").arg(getComplement().toUpper(),getPath().split("/").last());
    }
    return oAction;
}
QString GTAActionForEach::getSCXMLStateName()const
{
    //QHash<QString,QString> mapEqnStr = GTAUtil::getNomenclatureOfArithSymbols();
    QString oAction;
    QString path;
    if(!getRelativePath().isEmpty())
    {
         path = getRelativePath();
    }
    else
    {
        path = getPath();
    }
    path.remove(".csv");
    QStringList filename = path.split("/");
    oAction = QString("%1 row in %2").arg(getComplement(),filename.at(filename.count()-1));
    return oAction;
}
QList<GTACommandBase*>& GTAActionForEach::getChildren(void) const
{
    return *_lstChildren;
}
bool GTAActionForEach::insertChildren(GTACommandBase* pBase,const int& idPos)
{
    bool rc = false;
    int currSize=_lstChildren->size();
    if (idPos<=currSize && currSize>=0)
    {
        _lstChildren->insert(idPos,pBase);
        if (pBase!=NULL)
        {
            pBase->setParent(this);
            if(this->isIgnoredInSCXML() == true)
            {
                pBase->setIgnoreInSCXML(this->isIgnoredInSCXML());
            }
            if(this->getReadOnlyState() == true)
            {
                pBase->setReadOnlyState(this->getReadOnlyState());
            }
        }
        rc = true;
    }
    return rc;
}

void  GTAActionForEach::setParent(GTACommandBase* iParent)
{
    _parent=iParent;
}
GTACommandBase* GTAActionForEach::getParent(void) const
{
    return _parent;
}

bool GTAActionForEach:: hasChildren()
{
    return !(_lstChildren->isEmpty());
}
bool GTAActionForEach::canHaveChildren() const
{
    return true;
}
QStringList GTAActionForEach::getVariableList() const
{
    QStringList lstOfVars,lstOfParams;
	if (!_OverwriteTable) {
		for (int i = 0; i < _lstChildren->size(); i++) {
			if (_lstChildren->at(i) != nullptr)
				lstOfVars.append(_lstChildren->at(i)->getVariableList());
		}
		lstOfVars.removeDuplicates();
		for (auto var : lstOfVars) {
			if (var.startsWith("line")) {
				QString columnName = var.split("__").at(1);
				for (auto inCSV : getCSVColumnByTitle(columnName)) {
					if (inCSV.split('.').size()==3)
					lstOfParams.append(inCSV);
				}
			}
		}
		lstOfParams.removeDuplicates();
		
	}
	return lstOfParams;
}


void GTAActionForEach::ReplaceTag(const QMap<QString,QString>& iTagValueMap)
{
    QMapIterator<QString,QString> iterTag(iTagValueMap);
    while (iterTag.hasNext())
    {
        iterTag.next();
        QString tag = QString("%1%2%3").arg(QChar(TAG_IDENTIFIER),iterTag.key(),QChar(TAG_IDENTIFIER));
        _Path.replace(tag,iterTag.value());
        //_DelimiterIndex.replace(tag,iterTag.value());

    }
}
void GTAActionForEach::stringReplace(const QRegExp& iStrToFind,const QString& iStringToReplace)
{
    _Path.replace(iStrToFind,iStringToReplace);
    //_ParamValue.replace(iStrToFind,iStringToReplace);

}
bool GTAActionForEach::equipmentReplace(const QRegExp& iStrToFind, const QString& iStringToReplace)
{
    QString temp = _Path;
    bool rc = false;
    if (temp.count('.') == 2) // triplet detection
    {
        temp.chop(temp.size() - temp.indexOf('.'));
        if (temp.contains(iStrToFind))
        {
            rc = true;
            temp.replace(iStrToFind, iStringToReplace);
            _Path.remove(0, _Path.indexOf('.'));
            _Path.prepend(temp);
        }
    }
    return rc;
}
bool GTAActionForEach::searchString(const QRegExp& iRegExp,bool ibTextSearch)const
{
    bool rc = false;

    if (_Path.contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getComment().contains(iRegExp))
        rc = true;
    else if (ibTextSearch && this->getStatement().contains(iRegExp))
        rc = true;
    else
        rc=false;

    return rc;
}
bool GTAActionForEach::expandReferences()
{
    int currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-2;i>=0;i--)
    {
        GTACommandBase* pCurrentChild = _lstChildren->at(i);
        if (pCurrentChild->hasReference())
        {
            _lstChildren->removeAt(i);
            QList<GTACommandBase*>& referenceChildrens = pCurrentChild->getChildren();
            for (int j=0;j<referenceChildrens.size();j++)
            {
                _lstChildren->insert(i+j,referenceChildrens.at(j)->getClone());
            }

        }
    }
    currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-2;i>=0;i--)
    {
        GTACommandBase* pCurrentChild = _lstChildren->at(i);
        pCurrentChild->expandReferences();
    }
    return true;
}
bool GTAActionForEach::setDumpList(const QStringList& iDumpList)
{
    _dumpList.clear();
    _dumpList.append(iDumpList);
    return true;
}
QStringList GTAActionForEach::getDumpList()const
{
    return _dumpList;
}


bool GTAActionForEach::hasLoop() const
{
    return true;

}

void GTAActionForEach::replaceUntagged()
{
    int currChildrenSize = _lstChildren->size();
    for (int i=currChildrenSize-2;i>=0;i--)
    {
        GTACommandBase* pCurrentChild = _lstChildren->at(i);

        if (pCurrentChild!=NULL)
        {
            //if (pCurrentChild->canHaveChildren())
            //{
            pCurrentChild->replaceUntagged();
            //}

            QStringList varList = pCurrentChild->getVariableList();
            bool contains = false;
            foreach(QString varName,varList)
            {
                if(varName.contains(TAG_IDENTIFIER))
                {
                    contains = true;
                }
            }
            if(contains )
            {
                delete pCurrentChild ;
                _lstChildren->removeAt(i);
            }
        }

    }
}
void GTAActionForEach::setLogMessageList(const QList<GTAScxmlLogMessage> &iList)
{
    GTAScxmlLogMessage mainLogMsg;
    foreach(GTAScxmlLogMessage logMsg,iList)
    {
        if((logMsg.LOD == GTAScxmlLogMessage::Main)&& (mainLogMsg.Result!=GTAScxmlLogMessage::KO))
        {
            mainLogMsg=logMsg;
        }
        else if(logMsg.LOD == GTAScxmlLogMessage::Detail)
        {
            _LogMessageList.append(logMsg);
        }
    }
    _LogMessageList.append(mainLogMsg);
    //add a checkbox in command to write back into the command
    if (getOverWriteResults())
    {
        CheckAndWriteToTable(_LogMessageList);
    }
}

/**
 * This function writes back the modified table into the foreach table if the user wants to write back the results
 * The detail messages are read and the table is re-created
 * @imsgList: List of GTAScxmlLogMessage
 * @return: true/false boolean
*/
bool GTAActionForEach::CheckAndWriteToTable(QList<GTAScxmlLogMessage>& imsgList)
{
    bool rc = false;
    //recreate the table and then write into the file
    // hash (rownumber, hash (column name, value) )
    QMap<QString, QHash<QString, QString> > table;
    foreach(GTAScxmlLogMessage logMsg,imsgList)
    {
        QHash<QString,QString> map;
        if (logMsg.getLOD() == GTAScxmlLogMessage::Detail)
        {
            //fetch the column name from expected value and place the current value accordingly
            //logMsg.ExpectedValue sample --> line.IN_Alt_target[index: 0]
            QString currentValue = logMsg.CurrentValue == "" ? "_empty_cell_" : logMsg.CurrentValue;
            QString expectedValue = logMsg.ExpectedValue.split(".").last();
            expectedValue.replace("index: ","").replace("]","");

            map.insert(expectedValue.split("[").first(), currentValue);
            table.insertMulti(expectedValue.split("[").last().trimmed(), map);
        }
    }

    if (table.isEmpty())
        return rc;

    // open the file to be updated
    QString FileToParse = getRelativePathChkB() ? getRepositoryPath() + getRelativePath() : getPath();
    QFile fileObj(FileToParse);
    rc = fileObj.open(QFile::WriteOnly | QFile::Text);
    if (!rc) {
        fileObj.close();
        return rc;
    }    
    QTextStream outputStream(&fileObj);
	QString delim = getDelimiterValue();        

    //write the column headers into the file first
    QStringList ColumnHeaders = getHeaderColumns();
    outputStream << ColumnHeaders.join(delim);
        
    //keys will come according to row index
    QStringList keys = table.keys();
    keys.removeDuplicates();    
    std::sort(keys.begin(), keys.end(), [](QString const& a, QString const& b) { return a.toInt() < b.toInt(); });
        
    //write each row into the file
    //individual keys are sorted to be identified according to row numbers
    foreach(QString key, keys)
    {
        //fetch all hash values of a specific row number
        QList<QHash<QString, QString> > values = table.values(key);

        //iterate through the values until none are left
        //this is done to ensure the table is created with proper column values
        int colcount = 0;
        QString line = "\n";
        while (!values.empty())
        {
            //iterate through the values and remove the entry if found
            for(int k=0; k<values.count(); k++)            
            {                
                auto &item = values.at(k);
                QString value = item.value(ColumnHeaders.at(colcount));
                if (value.isEmpty())
                    continue;
                QString cell = value == "_empty_cell_" ? "": value;
                line += cell;
                colcount++;

                //remove the value once you have got it
                values.removeOne(item);
                if (!values.isEmpty())
                    line += delim;                
                break;                    
            }
        }          
        outputStream << line;
    }    
    fileObj.close();
    return rc;
}

bool GTAActionForEach::hasChannelInControl()const
{
    return false;
}

bool GTAActionForEach::createEndCommand(GTACommandBase* & opCmd)const
{
    opCmd = new GTAActionForEachEnd;
    return true;
}

void GTAActionForEach::setCSVContent(const QList<QStringList> CSVContent)
{
	_CSVContent = CSVContent;
}

QList<QStringList> GTAActionForEach::getCSVContent() const
{
	return _CSVContent;
}

void GTAActionForEach::insertColumnInCSVContent(int columnIndex,const QStringList data)
{
	if (_CSVContent.size() > columnIndex) {
		_CSVContent.replace(columnIndex, data);
	}
	else if (columnIndex == _CSVContent.size()) {
		_CSVContent.append(data);
	}
}

void GTAActionForEach::insertLineInCSVContent(const QStringList dataList)
{
	int columnIndex = 0;
	if(_CSVContent.size() == dataList.size())
	{
		foreach(QString data, dataList) 
		{
			QStringList tempList;
			tempList = _CSVContent.at(columnIndex);
			tempList.append(data);
			insertColumnInCSVContent(columnIndex, tempList);
			columnIndex++;
		}
	}
	else if (_CSVContent.size() == 0)
	{
		foreach(QString data, dataList)
		{
			QStringList tempList;
			tempList.append(data);
			insertColumnInCSVContent(columnIndex, tempList);
			columnIndex++;
		}
	}
}

void GTAActionForEach::readCSVContent()
{
	QString fileToRead = QString();
	if (!getRelativePathChkB())
		fileToRead = getPath();
	else
		fileToRead = getRepositoryPath() + getRelativePath();
	
	QFile fileObj(fileToRead);
	bool rc = fileObj.open(QFile::ReadOnly | QFile::Text);
	if (rc)
	{
		QString delim = getDelimiterValue();

		QString line;
		while (!fileObj.atEnd())
		{
			line = fileObj.readLine();
			if (line.endsWith("\n"))
			{
				line.remove("\n");
			}
            QStringList lineList = line.split(delim).replaceInStrings(QRegExp("^\\s+|\\s+$"), "");
			insertLineInCSVContent(lineList);
		}
	}
	fileObj.close();
}

QStringList GTAActionForEach::getCSVColumnByTitle(const QString title) const {
	int columnIndex = -1;
	for (int i = 0; i < _CSVContent.size(); i++) {
		if (_CSVContent.at(i).at(0) == title) {
			columnIndex = i;
			break;
		}
	}
	if (columnIndex != -1 && !_CSVContent.at(columnIndex).isEmpty()) {
		QStringList toRet = _CSVContent.at(columnIndex);
		toRet.removeFirst();
		return toRet;
	}
	else
	{
		QStringList emptyList;
		return emptyList;
	}
}

