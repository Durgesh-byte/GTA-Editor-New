#include "GTAHeaderTemplateParser.h"

#pragma warning(push, 0)
#include <QFile>
#include <QFile>
#include <QTextStream>
#pragma warning(pop)


GTAHeaderTemplateParser:: GTAHeaderTemplateParser(const QString& isTextFilePath,const QString& isColSeperator):_sFilePath(isTextFilePath)
{
	_sColSeperator=isColSeperator;
	_dMaxRows=MAXHEADERCOLS;

}

 bool GTAHeaderTemplateParser:: parse()
 {
    bool bOK = true;

    QFile file(_sFilePath);
    bOK = file.setPermissions(QFile::ReadOther);
    bOK = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (! bOK)
    {
            //TODO: display, "Error: File Opening", isTemplatePath

            return bOK;
    }


    QTextStream in(&file);
    int dRow = 0;
    QStringList listOfColumns;

    while(! in.atEnd())
    {
            ++dRow;
            QString sLine = in.readLine();
            _sLines.append(sLine);
    }
    file.close();

    return bOK;


 }
 bool GTAHeaderTemplateParser::getParsedLines(QStringList& isLines)
 {
	 bool bOK =false;
	 if(_sLines.isEmpty())
		 return bOK;
	 else
	 {  isLines=_sLines;
	 return true;
	 }
 }