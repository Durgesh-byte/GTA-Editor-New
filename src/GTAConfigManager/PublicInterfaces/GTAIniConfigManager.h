#ifndef GTAINICONFIGMANAGER_H
#define GTAINICONFIGMANAGER_H

/// <summary>
/// This class handles .ini configuration files. The strategy of this class to read the file is to consider
/// it as an array of lines. Each line has its own property and thus the methods are able to respect as truthfully
/// as possible the shape and integrity of the file.
/// </summary>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "GTAConfigManagerShared.h"


#pragma warning(push, 0)
#include <qstring.h>
#include "qvector.h"
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)

/*Structure to handle a single line of the configuration file.*/
struct IniLine
{
	std::string _type;
	std::string _content;
	std::string _value;

	IniLine(): _type(""), _content(""), _value ("") {}
	IniLine(std::string type, std::string content, std::string value)
		: _type(type), _content(content), _value(value)
	{}
};

/*Class to handle the reading and writing of the configuration file.*/
class GTAConfigManager_SHARED_EXPORT IniConfigManager
{
public:
	IniConfigManager();
	IniConfigManager(QString path);
	~IniConfigManager();

	void parse();
	void write();
	void write(QString path);

	void setPath(QString path);

	void reset();

	bool get(const std::string& section, const std::string& key, std::string& value);
	bool set(std::string section, std::string key, std::string value);

	static void parse_ini_path(const QString& path, QString& folder, QString& name);

private:
	QString _path;
	std::vector<IniLine> _iniArray;
};


#endif