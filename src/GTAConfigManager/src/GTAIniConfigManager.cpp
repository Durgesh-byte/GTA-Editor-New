#include "GTAIniConfigManager.h"

/**
* @brief Default constructor
* @param path the path to the .ini file.
*/
IniConfigManager::IniConfigManager()
{
}

/**
* @brief Constructor overload
* @param path the path to the .ini file.
*/
IniConfigManager::IniConfigManager(QString path)
{
	_path = path;
}

/**
* @brief Default destructor.
* @param None
*/
IniConfigManager::~IniConfigManager()
{
}

/**
* @brief Parse the .ini file, line by line and store the content in an array.
* @param None
* @return void
*/
void IniConfigManager::parse()
{
	std::ifstream indata;
	indata.open(_path.toStdString());
	QString a = _path;
	std::string line;

	while (std::getline(indata, line))
	{
		// check if the line is empty
		if (line.find_first_not_of(' ') == std::string::npos)
		{
			IniLine ini_line("blank", "", "");
			_iniArray.push_back(ini_line);
		}
		// else check if it's a comment
		else if (line.find(";") != std::string::npos)
		{
			line = line.substr(line.find_first_not_of(';'));
			IniLine ini_line("comment", "", line);
			_iniArray.push_back(ini_line);
		}
		// else check if it's a section
		else if (line.find("[") != std::string::npos)
		{
			size_t first_bracket_pos = line.find("[");
			size_t last_bracket_pos = line.find("]");
			IniLine ini_line("section", line.substr(first_bracket_pos + 1, first_bracket_pos + last_bracket_pos - 1), "");
			_iniArray.push_back(ini_line);
		}
		// else, it's a key
		else
		{
			size_t equal_pos = line.find("=");
			std::string key = line.substr(0, equal_pos);
			std::string value = line.substr(equal_pos + 1, std::string::npos);
			key = key.substr(0, key.find_first_of(' '));
			if (!value.empty() && value != " ")
				value = value.substr(value.find_first_not_of(' '), std::string::npos);
			IniLine ini_line("key", key, value);
			_iniArray.push_back(ini_line);
		}
	}
	indata.close();
}

/**
* @brief Given a path to a file, returns the directory in which is stored that file.
* @param path path of a file.
* @return string corresponding to the directory.
*/
inline std::string find_directory(std::string path)
{
	std::string delimiter1 = "/";
	std::string delimiter2 = "\\";
	size_t pos = 0;
	std::string token;
	std::string res = "";
	while ((pos = path.find(delimiter1)) != std::string::npos || (pos = path.find(delimiter2)) != std::string::npos)
	{
		token = path.substr(0, pos);
		path.erase(0, pos + 1);
		res.append(token);
		res.append("/");
	}
	return res;
}

/**
* @brief Overwrites the configuration file, based on the stored array of lines.
* @param None
* @return void
*/
void IniConfigManager::write()
{
	std::ofstream outdata;
	std::remove(_path.toStdString().c_str());
	outdata.open(_path.toStdString());

	for (const auto& ini_line : _iniArray)
	{
		if (ini_line._type == "blank")
			outdata << std::endl;
		else if (ini_line._type == "comment")
			outdata << ";" << ini_line._value << std::endl;
		else if (ini_line._type == "section")
			outdata << "[" << ini_line._content << "]" << std::endl;
		else
			outdata << ini_line._content << "=" << ini_line._value << std::endl;
	}
	outdata.close();
}

/**
* @brief Overload of write. The configuration file is written in another file.
* @param path Full path to the new file.
* @return void
*/
void IniConfigManager::write(QString path)
{
	std::ofstream outdata;
	std::remove(path.toStdString().c_str());
	outdata.open(path.toStdString());

	for (const auto& ini_line : _iniArray)
	{
		if (ini_line._type == "blank")
			outdata << std::endl;
		else if (ini_line._type == "comment")
			outdata << ";" << ini_line._value << std::endl;
		else if (ini_line._type == "section")
			outdata << "[" << ini_line._content << "]" << std::endl;
		else
			outdata << ini_line._content << "=" << ini_line._value << std::endl;
	}
	outdata.close();
}

/**
* @brief General getter of the class. For given section and key, the method simultenously evaluates if the line exists and stored the corresponding value.
* @param section The section of the configuration file.
* @param key The key of the configuration file (under the section).
* @param value The value of the section/key. This argument is evaluated on the spot by reference.
* @return boolean representing the existence of the section/key in the array.
*/
bool IniConfigManager::get(const std::string& section, const std::string& key, std::string& value)
{
	bool found_section = false;
	bool found_key = false;
	size_t pos = 0;
	IniLine ini_line;
	while ((!found_section || !found_key) && pos != _iniArray.size())
	{
		ini_line = _iniArray.at(pos);
		if (ini_line._type == "section" && !found_section)
		{
			found_section = (ini_line._content == section);
		}
		else if (ini_line._type == "key" && !found_key)
		{
			found_key = (ini_line._content == key);
		}
		pos++;
	}
	if (found_section && found_key)
		value = ini_line._value;
	else
		value = std::string();
	return found_section && found_key;
}

/**
* @brief Sets the path to the configuration file.
* @param path Full path to the configuration file.
* @return void
*/
void IniConfigManager::setPath(QString path)
{
	_path = path;
}

/**
* @brief Reset the configuration array.
* @param None
* @return void
*/
void IniConfigManager::reset()
{
	_iniArray.clear();
}

/**
* @brief Sets the value of a given section and key. If the section/key does not exists, write it at the appropriate location.
* @param section The section where to store the value.
* @param key The key where to store the value.
* @param value The value to be stored.
* @return void
*/
bool IniConfigManager::set(std::string section, std::string key, std::string value)
{
	bool found_section = false;
	bool found_key = false;
	bool inserted = false;

	size_t pos = 0;
	size_t last_key_pos = 0;
	IniLine* line = nullptr;
	while ((!found_section || !found_key) && pos != _iniArray.size())
	{
		line = &_iniArray.at(pos);
		if (line->_type == "section")
		{
			if (line->_content == section)
			{
				found_section = true;
				pos++;
				continue;
			}
		}
		
		if (line->_type == "key")
			last_key_pos = pos;

		if (found_section && line->_type == "key" && line->_content == key)
			found_key = true;

		if (found_section && line->_type == "section" && line->_content != section)
		{
			IniLine new_key_line("key", key, value);
			_iniArray.insert(_iniArray.begin() + last_key_pos + 1, new_key_line);
			found_key = true;
			inserted = true;
		}
		pos++;
	}

	if (found_section && found_key)
		line->_value = value;
	else if (found_section && !found_key)
	{
		IniLine new_key_line("key", key, value);
		_iniArray.push_back(new_key_line);
		found_key = true;
		inserted = true;
	}
	else
	{
		IniLine new_blank_line("blank", "", "");
		IniLine new_section_line("section", section, "");
		IniLine new_key_line("key", key, value);
		_iniArray.push_back(new_blank_line);
		_iniArray.push_back(new_section_line);
		_iniArray.push_back(new_key_line);
		inserted = true;
	}

	return inserted;
}

/**
* @brief Parses a path to .ini file so that the folder and the name of the file are separated.
* @param path The complete file path
* @param path Folder containing the file (should be an empty reference)
* @param path Name of the file (should be an empty reference)
* @return void (writes directly the QStrings)
*/
void IniConfigManager::parse_ini_path(const QString& path, QString& folder, QString& name)
{
	char delimiter1 = '/';
	char delimiter2 = '\\';
	std::string path_ = path.toStdString();
	size_t pos = path.size() - 1;
	std::string name_ = "";
	std::string folder_ = "";
	bool read_folder = false;

	while (pos > 0)
	{
		if (path_[pos] != delimiter1 && path_[pos] != delimiter2 && !read_folder)
			name_.insert(0, 1, path_[pos]);
		else
		{
			read_folder = true;
			folder_.insert(0, 1, path_[pos]);
		}
		pos--;
	}
	folder_.insert(0, 1, path_[0]);
	folder = QString::fromStdString(folder_);
	name = QString::fromStdString(name_);
}