#ifndef GTAHEADERTEMPLATEPARSER_H
#define GTAHEADERTEMPLATEPARSER_H

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea,
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)

#include "GTAParsers.h"

#pragma warning(push, 0)
#include <QString>
#include <QStringList>
#pragma warning(pop)

#define MAXHEADERCOLS 3;

class GTAParsers_SHARED_EXPORT GTAHeaderTemplateParser
{
public:

    GTAHeaderTemplateParser(const QString& isTextFilePath,const QString& isColSeperator=QString("/"));
    bool parse();
	int getMaxCols (){return _dMaxRows;}
	int getMaxRows(){return _sLines.size();}
	bool getParsedLines(QStringList& isLines);


private:

	QString _sFilePath;
	QString _sColSeperator;
	QStringList _sLines;
	int _dMaxRows;


};

#endif
