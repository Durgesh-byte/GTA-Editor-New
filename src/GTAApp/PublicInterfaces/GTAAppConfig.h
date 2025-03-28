#ifndef GTAAPPCONFIG_H
#define GTAAPPCONFIG_H

/// <summary>
/// This class is designed to handle the application configuration file (AppConfig.ini), which is supposed to be with the exe file.
/// It uses the GTAIniConfigManager to work, but act as a curtain behind which the reading and writing operation run.
/// </summary>

#include <string>
#include "GTAIniConfigManager.h"
#include <sstream>

#pragma warning(push, 0)
#include <qstring.h>
#include <qfileinfo.h>
#pragma warning(pop)


const std::string APP_SECTION_PATH = "PATH";
const std::string APP_KEY_BENCHFOLDERPATH = "BENCHFOLDERPATH";
const std::string APP_KEY_BENCHFILENAME = "BENCHFILENAME";

/*Structure for the parameters in the configuration file.
Stores its value, its default value, if it is a mandatory parameter and if the parameter has changed during the application.*/
template <typename T>
struct Parameter_app
{
	Parameter_app(T default_value, bool isMandatory)
		: _default(default_value), _isMandatory(isMandatory), _hasChanged(false) {}

	T get()
	{
		if (_useDefault)
			return _default;
		else
			return _value;
	}

	T _value;
	T _default;
	bool _isMandatory;
	bool _useDefault;
	bool _hasChanged;
};

/*Structure which store all the parameters.*/
struct AppParameters
{
	AppParameters() {};
	~AppParameters()
	{
		delete _bench_conf_folder_path;
		delete _bench_conf_file_name;
	}
	Parameter_app<std::string>* _bench_conf_folder_path;
	Parameter_app<std::string>* _bench_conf_file_name;
};

/*Class handling the application configuration file.*/
class AppConfig
{
public:
	AppConfig(QString path);
	~AppConfig();

	bool loadConf();
	void saveConf();

	void setPath(QString path);

	bool setParamString(Parameter_app<std::string>* param, const std::string& section, const std::string& key);
	bool setParamBool(Parameter_app<bool>* param, const std::string& section, const std::string& key);

	void reset();

	bool checkValidity() const;

	std::string getTestConfFolderPath();
	std::string getTestConfFileName();

	void setTestConfFolderPath(const std::string& value);
	void setTestConfFileName(const std::string& value);

private:
	IniConfigManager* _configManager;
	AppParameters _params;
	QString _path;
	std::vector<std::pair<std::string, std::string> > _error_log;
};

#endif