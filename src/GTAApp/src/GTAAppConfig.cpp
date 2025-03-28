#include "GTAAppConfig.h"

/**
* @brief Constructor
* @param path The path to the configuration file.
*/
AppConfig::AppConfig(QString path)
{
	setPath(path);
	_configManager = new IniConfigManager(path);

	_params._bench_conf_folder_path = new Parameter_app<std::string>("", true);
	_params._bench_conf_file_name = new Parameter_app<std::string>("", true);
}

/**
* @brief Destructor
* @param None
*/
AppConfig::~AppConfig()
{
	delete _configManager;
}

/**
* @brief Set a parameter which is a string by finding it in the correct section/key.
* @param param The parameter to be set.
* @param section The section of the parameter.
* @param key The key of the parameter.
* @return boolean Returns true if the parameter does not currently exist in the configManager array and is mandatory.
*/
bool AppConfig::setParamString(Parameter_app<std::string>* param, const std::string& section, const std::string& key)
{
	bool error = false;
	std::string value;
	bool exists = _configManager->get(section, key, value);

	if (exists)
	{
		param->_value = value;
		param->_useDefault = false;
	}
	else
	{
		param->_useDefault = true;
		if (param->_isMandatory)
			error = true;
	}

	return error;
}

/**
* @brief Converts std::string to bool, i.e. "false" -> false.
* @param str std::string of an attributes.
* @return boolean value corresponding to the string.
*/
inline bool str2bool(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
}

/**
* @brief Set a parameter which is a bool by finding it in the correct section/key.
* @param param The parameter to be set.
* @param section The section of the parameter.
* @param key The key of the parameter.
* @return boolean Returns true if the parameter does not currently exist in the configManager array and is mandatory.
*/
bool AppConfig::setParamBool(Parameter_app<bool>* param, const std::string& section, const std::string& key)
{
	bool error = false;
	std::string value;
	bool exists = _configManager->get(section, key, value);

	if (exists)
	{
		param->_value = str2bool(value);
		param->_useDefault = false;
	}
	else
	{
		param->_useDefault = true;
		if (param->_isMandatory)
			error = true;
	}

	return error;
}

/** (So far not used)
* @brief Resets the configuration manager array.
* @param None
* @return void
*/
void AppConfig::reset()
{
	_configManager->reset();
}

/**
* @brief Checks the validity of the parameters.
* @param None
* @return boolean Returns true if at least the parameters exist (paths).
*/
bool AppConfig::checkValidity() const
{
	QString benchConfFolder = QString::fromStdString(_params._bench_conf_folder_path->get());
	QString benchConfName = QString::fromStdString(_params._bench_conf_file_name->get());
	QString benchConfPath = benchConfFolder + benchConfName;
	QFileInfo benchConfInfo(benchConfPath);

	bool validity = true;
	validity = validity && !benchConfPath.isEmpty();
	validity = validity && benchConfInfo.exists();

	return validity;
}

/**
* @brief Main method that loads the configuration file by storing all the relevant parameters.
* @param None
* @return boolean Returns true if at least one mandatory parameter is missing from the configuration file.
*/
bool AppConfig::loadConf()
{
	// parse
	_configManager->parse();
	bool has_errors = false;

	if (setParamString(_params._bench_conf_folder_path, APP_SECTION_PATH, APP_KEY_BENCHFOLDERPATH))
		_error_log.push_back(std::make_pair(APP_SECTION_PATH, APP_KEY_BENCHFOLDERPATH));
	if (setParamString(_params._bench_conf_file_name, APP_SECTION_PATH, APP_KEY_BENCHFILENAME))
		_error_log.push_back(std::make_pair(APP_SECTION_PATH, APP_KEY_BENCHFILENAME));

	// deal with error_log
	if (_error_log.size() > 0)
		has_errors = true;

	return !has_errors;
}

/**
* @brief Main method for saving the configuration file.
* @param None
* @return void
*/
void AppConfig::saveConf()
{
	// bench folder path
	if (_params._bench_conf_folder_path->_hasChanged)
		_configManager->set(APP_SECTION_PATH, APP_KEY_BENCHFOLDERPATH, _params._bench_conf_folder_path->_value);

	// bench file name
	if (_params._bench_conf_file_name->_hasChanged)
		_configManager->set(APP_SECTION_PATH, APP_KEY_BENCHFILENAME, _params._bench_conf_file_name->_value);

	_configManager->write();
}

/**
* @brief Set the configuration file path.
* @param path The path to the configuration file.
* @return void
*/
void AppConfig::setPath(QString path)
{
	_path = path;
}

/**
* @brief Get the bench configuration folder path.
* @param None
* @return Full path to the bench folder.
*/
std::string AppConfig::getTestConfFolderPath()
{
	return _params._bench_conf_folder_path->get();
}

/**
* @brief Get the bench configuration file name
* @param None 
* @return Name of the bench configuration file.
*/
std::string AppConfig::getTestConfFileName()
{
	return _params._bench_conf_file_name->get();
}

/**
* @brief Set the bench configuration folder path.
* @param value Full folder path.
* @return void
*/
void AppConfig::setTestConfFolderPath(const std::string& value)
{
	_params._bench_conf_folder_path->_value = value;
}

/**
* @brief Set the bench configuration file name.
* @param value File name.
* @return void
*/
void AppConfig::setTestConfFileName(const std::string& value)
{
	_params._bench_conf_file_name->_value = value;
}


