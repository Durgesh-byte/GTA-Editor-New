#include "AINGTAFSIIFile.h"

AINGTAFSIIFile::AINGTAFSIIFile()
{
    _FunctionNameMap.clear();
}


void AINGTAFSIIFile::setICDDatabaseList(const QStringList &iICDList)
{
    _ICDDatabase = iICDList;
}

QStringList AINGTAFSIIFile::getICDDatabaseList()const
{
    return _ICDDatabase;
}

void AINGTAFSIIFile::setTestProcedure(const AINGTAFSIITestProcedure &iTestProc)
{
    _TestProc = iTestProc;
}

AINGTAFSIITestProcedure AINGTAFSIIFile::getTestProcedure()
{
    return _TestProc;
}

void AINGTAFSIIFile::setFunctionList(const QList<AINGTAFSIIFunction> &iFunctionList)
{
    _Functions = iFunctionList;
}

QList<AINGTAFSIIFunction> AINGTAFSIIFile::getFunctionList()
{
    return _Functions;
}

void AINGTAFSIIFile::addFunction(const AINGTAFSIIFunction &iFunction)
{
    _Functions.append(iFunction);
}

void AINGTAFSIIFile::setName(const QString &iName)
{
    _Name = iName;
}

QString AINGTAFSIIFile::getName()const
{
    return _Name;
}


void AINGTAFSIIFile::setDescription(const QString &iDesc)
{
    _Description = iDesc;
}

QString AINGTAFSIIFile::getDescription()const
{
    return _Description;
}

void AINGTAFSIIFile::insertIntoFuncNameMap(const QString &iLongName, const QString &iShorName)
{
    _FunctionNameMap.insert(iLongName,iShorName);
}

QHash<QString,QString> AINGTAFSIIFile::getFuncNameMap()const
{
    return _FunctionNameMap;
}

QString AINGTAFSIIFile::getValueFromFuncNameMap(const QString &iKey)const
{
    QString value;
    value = _FunctionNameMap.value(iKey);
	return value;
}

bool AINGTAFSIIFile::isPresentInFuncNameMap(const QString &iKey)
{
    if(_FunctionNameMap.contains(iKey))
    {
        return true;
    }
    else
        return false;
}

void AINGTAFSIIFile::setExternalXmlFilePath(const QString &iPath)
{
    _ExternalXmlFilePath = iPath;
}

QString AINGTAFSIIFile::getExternalXmlFilePath()const
{
    return _ExternalXmlFilePath;
}

bool AINGTAFSIIFile::operator == (const AINGTAFSIIFile &iFSIIFile) const
{
	if(iFSIIFile.getDescription() == this->getDescription())
		return true;
	else
		return false;
}
void AINGTAFSIIFile::clear()
{
	_Name = "";
	_Description = "";
	_ICDDatabase.clear();
	_Functions.clear();
	_FunctionNameMap.clear();
	_ExternalXmlFilePath= "";
}
