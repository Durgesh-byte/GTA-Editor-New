#include "AINCMNErrorLogger.h"

#include "AINCMNCommonServices.h"
using namespace AINCMN::CMNUtils;

#include <QTextStream>

AINCMNErrorLogger::AINCMNErrorLogger(const QString& iLogFileAbsPath, QObject *parent) :
    QObject(parent)
{
	_pLogFile = new QFile(iLogFileAbsPath);
	bool bOk = _pLogFile->open(QIODevice::WriteOnly | QIODevice::Text);
	if (! bOk || ! _pLogFile->isOpen())
	{
		AINCMNCommonServices::display("Error", "LogFile(" + iLogFileAbsPath+ ") Opening");
	}	
}

AINCMNErrorLogger::~AINCMNErrorLogger()
{
	if (_pLogFile)
	{			
		if (_pLogFile->isOpen())
		{
			_pLogFile->close();
		}
		delete _pLogFile;
		_pLogFile = NULL;
	}
}

bool AINCMNErrorLogger::writeLine(const QString& iLine)
{
	bool bOk(false);
	if (! _pLogFile->isOpen())
	{
		bOk = _pLogFile->open(QIODevice::WriteOnly | QIODevice::Text);
	}
	
	if (_pLogFile->isOpen())
	{		
		QTextStream in(_pLogFile);
		in << iLine << "\n";
		bOk = true;
	}
	
	return bOk;
}
