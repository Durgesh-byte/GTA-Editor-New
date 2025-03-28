#include "GTATestConfig.h"

#pragma warning(push, 0)
#include "qtextstream.h"
#pragma warning(pop)

TestConfig* TestConfig::instance = 0;

/**
* @brief Creates an instance of the class. The method is static and returns a static instance.
* @param None
* @return void
*/
TestConfig* TestConfig::getInstance()
{
	if (instance == nullptr)
		instance = new TestConfig();
	return instance;
}

/**
* @brief Frees the memory allocated for the singleton. This should not be called more than once, when closing the application.
* @param None
* @return void
*/
void TestConfig::resetInstance()
{
	delete instance;
	instance = nullptr;
}

/**
* @brief Constructor
* @param path The path to the configuration file.
*/
TestConfig::TestConfig()
{
	_configManager = new IniConfigManager();
}

/**
* @brief Destructor
* @param None
*/
TestConfig::~TestConfig()
{
	delete _configManager;
}

/**
* @brief Initializes the configuration by initializing all parameters with their default value and the mandatory indicator.
* @param path Full path to the bench configuration file.
* @return None
*/
void TestConfig::initialize(const QString& path)
{
	setPath(path);
	_configManager->reset();
	_error_log.clear();

	_params._library_path = new Parameter_test<std::string>("", true);
	_params._log_dir_path = new Parameter_test<std::string>("/LOG", false);
	_params._repository_path = new Parameter_test<std::string>("", true);
	_params._tool_data_path = new Parameter_test<std::string>("", true);
	_params._git_db_path = new Parameter_test<std::string>("", false);
	_params._gta_controller_path = new Parameter_test<std::string>("", false);
	_params._auto_launcher_path = new Parameter_test<std::string>("", false);
	_params._auto_launcher_config_path = new Parameter_test<std::string>("", false);
	_params._gta_controller_output_log_path = new Parameter_test<std::string>("", false);

	_params._recording_tool = new Parameter_test<std::string>("",false);

	_params._channel_selection = new Parameter_test<bool>(false, false);
	_params._bench_db_startup = new Parameter_test<bool>(true, false);
	_params._proc_db_startup = new Parameter_test<bool>(false, false);
	_params._debug_mode_ip = new Parameter_test<std::string>("169.254.61.95", false);
	_params._debug_mode_port = new Parameter_test<int>(65432, false);
	_params._decimal_places = new Parameter_test<std::string>("Default", false);
	_params._dv_saved_list = new Parameter_test<std::string>("Author:Validation Status:", false);
	_params._engine_selection = new Parameter_test<std::string>("CFM", false);
	_params._en_invoke_local_var = new Parameter_test<bool>("", false);
	_params._generic_scxml = new Parameter_test<bool>(false, false);
	_params._init_subscribe_end = new Parameter_test<bool>(true, false);
	_params._init_subscribe_start = new Parameter_test<bool>(true, false);
	_params._last_launch_elem_path = new Parameter_test<std::string>("", false);
	_params._last_log_path = new Parameter_test<std::string>("/LOG", false);
	_params._log_enable = new Parameter_test<bool>(true, false);
	_params._log_messages = new Parameter_test<std::string>("Info", false);
	_params._loop_timeout_substeps = new Parameter_test<bool>(false, false);
	_params._manual_action_timeout_status = new Parameter_test<bool>(false, false);
	_params._manual_action_timeout_value = new Parameter_test<std::string>("", false);
	_params._new_log_format = new Parameter_test<bool>(false, false);
	_params._new_scxml_struct = new Parameter_test<bool>(true, false);
	_params._pir_invoke_for_scxml = new Parameter_test<bool>(false, false);
	_params._pir_wait_time = new Parameter_test<bool>(false, false);
	_params._pir_wait_time_val = new Parameter_test<std::string>("", false);
	_params._print_table_status = new Parameter_test<bool>(false, false);
	_params._resize_row = new Parameter_test<bool>(false, false);
	_params._result_view_saved_list = new Parameter_test<std::string>("Expected Value : Result:", false);
	_params._save_before_export = new Parameter_test<bool>(false, false);
	_params._stand_alone_exec_mode = new Parameter_test<bool>(false, false);
	_params._subscribe_unsubscribe_timeout_status = new Parameter_test<bool>(false, false);
	_params._subscribe_unsubscribe_timeout_value = new Parameter_test<std::string>("", false);
	_params._svn_add_on_exit = new Parameter_test<bool>(false, false);
	_params._svn_commit_on_exit = new Parameter_test<bool>(false, false);
	_params._svn_update_on_start = new Parameter_test<bool>(false, false);
	_params._uscxml_based_scxml_en = new Parameter_test<bool>(false, false);
	_params._validation_enable = new Parameter_test<bool>(false, false);
	_params._virtual_pmr = new Parameter_test<bool>(false, false);

	_params._datatv_geometry = new Parameter_test<std::string>(R"(\x1\xd9\xd0\xcb\0\x2\0\0\0\0\0\xc0\0\0\0;\0\0\x6\x84\0\0\x3\xe4\0\0\0\xc9\0\0\0\x61\0\0\x6{\0\0\x3\xdb\0\0\0\0\0\0\0\0\a\x80)", false);
	_params._result_page_geometry = new Parameter_test<std::string>(R"(\0\0\0\xff\0\0\0\0\xfd\0\0\0\x2\0\0\0\0\0\0\x1\xc6\0\0\x2\x4\xfc\x2\0\0\0\x1\xfb\0\0\0\x1a\0\x44\0\x61\0t\0\x61\0\x42\0r\0o\0w\0s\0\x65\0r\0\x44\0W\x1\0\0\0\x34\0\0\x2\x4\0\0\x1\x16\0\a\xff\xff\0\0\0\x3\0\0\x5\xb3\0\0\x1%\xfc\x1\0\0\0\x1b\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x1\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x2\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x2\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x2\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x2\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x2\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x2\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x2\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfc\0\0\0\0\0\0\x5\xb3\0\0\0\0\0\xff\xff\xff\xfa\0\0\0\0\x1\0\0\0\x2\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0,\0\x41\0p\0p\0l\0i\0\x63\0\x61\0t\0i\0o\0n\0 \0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\0\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\xfb\0\0\0\x14\0L\0o\0g\0 \0W\0i\0n\0\x64\0o\0w\x1\0\0\0\0\0\0\x5\xb3\0\0\x1\n\0\xff\xff\xff\0\0\x3\xe8\0\0\x2\x4\0\0\0\x4\0\0\0\x4\0\0\0\b\0\0\0\b\xfc\0\0\0\x1\0\0\0\x2\0\0\0\v\0\0\0\"\0P\0r\0o\0g\0r\0\x61\0m\0S\0\x65\0l\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\0\0\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x1c\0G\0\x65\0n\0\x65\0r\0\x61\0l\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\0<\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x1a\0\x45\0\x64\0i\0t\0o\0r\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\0\xbd\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\"\0\x45\0\x64\0i\0t\0o\0r\0\x46\0i\0l\0\x65\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\x2R\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x1a\0R\0\x65\0s\0u\0l\0t\0T\0o\0o\0l\0\x42\0\x61\0r\0\0\0\x3)\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\"\0\x45\0\x64\0i\0t\0o\0r\0V\0i\0\x65\0w\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\x3)\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x14\0S\0V\0N\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\x3\x93\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x1a\0S\0V\0N\0\x44\0i\0r\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\x4p\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0\x1a\0\x45\0x\0p\0o\0r\0t\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\x4\xc3\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0 \0S\0u\0\x62\0W\0i\0n\0\x64\0o\0w\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\x5\x16\xff\xff\xff\xff\0\0\0\0\0\0\0\0\0\0\0 \0\x64\0\x65\0\x62\0u\0g\0g\0i\0n\0g\0T\0o\0o\0l\0\x42\0\x61\0r\x1\0\0\x5i\xff\xff\xff\xff\0\0\0\0\0\0\0\0)", false);
	_params._geometry = new Parameter_test<std::string>(R"(\0\0\0\xff\0\0\0\0\0\0\0\x1\0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\v\\\a\0\0\0\a\0\0\0\x4\0\0\0}\0\0\0\n\0\0\0}\0\0\0\t\0\0\0}\0\0\0\b\0\0\0}\0\0\0\x3\0\0\0}\0\0\0\x2\0\0\0}\0\0\0\x6\0\0\0}\0\0\x1\xae\0\0\0\v\x1\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\0}\xff\xff\xff\xff\0\0\0\x81\0\0\0\0\0\0\0\v\0\0\0}\0\0\0\x1\0\0\0\0\0\0\0Z\0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\x1\0\0\0\0\0\0\0 < \0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\x1\0\0\0\0\0\0\0\x9b\0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\x1\0\0\0\0\0\0\x3\xe8\0\0\0\0\x80)", false);
	_params._window_state = new Parameter_test<std::string>(R"(\0\0\0\xff\0\0\0\0\0\0\0\x1\0\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x1\0\0\x1\0\0\0\0\0\0\0\0\0\0\0\0}\xff\xff\xff\xff\0\0\0\x81\0\0\0\0\0\0\0\0\0\0\x3\xe8\0\0\0\0s)", false);

	_params._bisg_ip = new Parameter_test<std::string>("127.0.0.1", false);
	_params._bisg_port = new Parameter_test<int>(2405, false);
	_params._tool_list = new Parameter_test<std::string>("", false);
}

/**
* @brief Check if the (mandatory) parameters are valid.
* @param None
* @return boolean Returns true if all mandatory parameters are valid.
*/
bool TestConfig::checkValidity(bool printErrors)
{
	QString sLibraryPath = QString::fromStdString(_params._library_path->get());
	QString sRepoPath = QString::fromStdString(_params._repository_path->get());
	QString sToolDataPath = QString::fromStdString(_params._tool_data_path->get());
	QString sLogPath = QString::fromStdString(_params._log_dir_path->get());
	QFileInfo sLibraryInfo(sLibraryPath);
	QFileInfo sRepoInfo(sRepoPath);
	QFileInfo sToolDataPathInfo(sToolDataPath);
	std::vector<QString> logs;
	bool validity = true;
	if (sLibraryPath.isEmpty()) {
		logs.push_back("[Error] Library (bench database) path is empty");
		validity = false;
	}
	if (!sLibraryInfo.exists()) {
		logs.push_back("[Error] Library (bench database) info doesn't exist");
		validity = false;
	}
	if (sRepoPath.isEmpty()) {
		logs.push_back("[Error] (Procedure) Repository path is empty");
		validity = false;
	}
	if (!sRepoInfo.exists()) {
		logs.push_back("[Error] (Procedure) Repository path info doesn't exist");
		validity = false;
	}
	if (sToolDataPath.isEmpty()) {
		logs.push_back("[Error] Tool data path is empty");
		validity = false;
	}
	if (!sToolDataPathInfo.exists()) {
		logs.push_back("[Error] Tool data path info doesn't exist");
		validity = false;
	}

	if (sLogPath.isEmpty()) {
		logs.push_back("[Error] Log data directory is empty");
		validity = false;
	} 
	else if (!sLogPath.compare("/LOG")) {
		logs.push_back("[Warning] Log data directory is set to default: " + sLogPath);
	}

	if (printErrors) {
		for (const auto& log : logs) {
			QTextStream  out(stdout);
			out << log << "\n";
		}
	}

	return validity;
}

/**
* @brief Set a parameter which is a string by finding it in the correct section/key.
* @param param The parameter to be set.
* @param section The section of the parameter.
* @param key The key of the parameter.
* @return boolean Returns true if the parameter does not currently exist in the configManager array and is mandatory.
*/
bool TestConfig::setParamString(Parameter_test<std::string>* param, const std::string& section, const std::string& key)
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
* @brief Converts bool to std::string, i.e. false -> "false".
* @param b boolean
* @return string corresponding to the boolean.
*/
inline std::string bool2str(const bool& b)
{
	return b ? "true" : "false";
}

/**
* @brief Set a parameter which is a bool by finding it in the correct section/key.
* @param param The parameter to be set.
* @param section The section of the parameter.
* @param key The key of the parameter.
* @return boolean Returns true if the parameter does not currently exist in the configManager array and is mandatory.
*/
bool TestConfig::setParamBool(Parameter_test<bool>* param, const std::string& section, const std::string& key)
{
	bool error = false;
	std::string value;
	bool exists = _configManager->get(section, key, value);
	std::transform(value.begin(), value.end(), value.begin(),
		[](unsigned char c) { return std::tolower(c); });

	// check the nature of the value (true/false, yes/no, ON/OFF)
	if (value == "on" || value == "yes")
		value = "true";
	else if (value == "off" || value == "no")
		value = "false";

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

/**
* @brief Set a parameter which is a int by finding it in the correct section/key.
* @param param The parameter to be set.
* @param section The section of the parameter.
* @param key The key of the parameter.
* @return boolean Returns true if the parameter does not currently exist in the configManager array and is mandatory.
*/
bool TestConfig::setParamInt(Parameter_test<int>* param, const std::string& section, const std::string& key)
{
	bool error = false;
	std::string value;
	bool exists = _configManager->get(section, key, value);

	if (exists)
	{
		param->_value = std::stoi(value);
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
* @brief Main method that loads the configuration file by storing all the relevant parameters.
* @return boolean Returns true if at least one mandatory parameter is missing from the configuration file.
*/
bool TestConfig::loadConf(const QString& path, bool printErrors)
{
	// initialize
	initialize(path);
	// parse
	_configManager->parse();
	bool has_errors = false;
	
	// PATH
	if (setParamString(_params._library_path, TEST_SECTION_PATH, TEST_KEY_LIBRARYPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_LIBRARYPATH));
	if (setParamString(_params._log_dir_path, TEST_SECTION_PATH, TEST_KEY_LOGDIRPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_LOGDIRPATH));
	if (setParamString(_params._repository_path, TEST_SECTION_PATH, TEST_KEY_REPOSITORYPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_REPOSITORYPATH));
	if (setParamString(_params._tool_data_path, TEST_SECTION_PATH, TEST_KEY_TOOLDATAPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_TOOLDATAPATH));
	if (setParamString(_params._git_db_path, TEST_SECTION_PATH, TEST_KEY_GITDBPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_GITDBPATH));
	if (setParamString(_params._gta_controller_path, TEST_SECTION_PATH, TEST_KEY_GTACONTROLLER))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_GTACONTROLLER));
	if (setParamString(_params._auto_launcher_path, TEST_SECTION_PATH, TEST_KEY_AUTOLAUNCHERPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_AUTOLAUNCHERPATH));
	if (setParamString(_params._auto_launcher_config_path, TEST_SECTION_PATH, TEST_KEY_AUTOLAUNCHERCONFIGPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_AUTOLAUNCHERCONFIGPATH));
	if (setParamString(_params._gta_controller_output_log_path, TEST_SECTION_PATH, TEST_KEY_GTACONTROLLEROUTPUTLOGPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_PATH, TEST_KEY_GTACONTROLLEROUTPUTLOGPATH));

	

	// RECORDING SETTINGS
	if (setParamString(_params._recording_tool, RECORDING_SETTINGS, VIDEO_RECORDING_TOOL))
		_error_log.push_back(std::make_pair(RECORDING_SETTINGS, VIDEO_RECORDING_TOOL));

	// USER SETTINGS
	if (setParamBool(_params._channel_selection, TEST_SECTION_USERSETTINGS, TEST_KEY_CHANNELSELECTION))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_CHANNELSELECTION));
	if (setParamBool(_params._bench_db_startup, TEST_SECTION_USERSETTINGS, TEST_KEY_BENCHDBSTARTUP))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_BENCHDBSTARTUP));
	if (setParamBool(_params._proc_db_startup, TEST_SECTION_USERSETTINGS, TEST_KEY_PROCDBSTARTUP))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_PROCDBSTARTUP));
	if (setParamString(_params._debug_mode_ip, TEST_SECTION_USERSETTINGS, TEST_KEY_DEBUGMODEIP))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_DEBUGMODEIP));
	if (setParamInt(_params._debug_mode_port, TEST_SECTION_USERSETTINGS, TEST_KEY_DEBUGMODEPORT))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_DEBUGMODEPORT));
	if (setParamString(_params._decimal_places, TEST_SECTION_USERSETTINGS, TEST_KEY_DECIMALPLACES))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_DECIMALPLACES));
	if (setParamString(_params._dv_saved_list, TEST_SECTION_USERSETTINGS, TEST_KEY_DVSAVEDLIST))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_DVSAVEDLIST));
	if (setParamString(_params._engine_selection, TEST_SECTION_USERSETTINGS, TEST_KEY_ENGINESELECTION))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_ENGINESELECTION));
	if (setParamBool(_params._en_invoke_local_var, TEST_SECTION_USERSETTINGS, TEST_KEY_ENINVOKELOCALVAR))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_ENINVOKELOCALVAR));
	if (setParamBool(_params._generic_scxml, TEST_SECTION_USERSETTINGS, TEST_KEY_GENERICSCXML))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_GENERICSCXML));
	if (setParamBool(_params._init_subscribe_end, TEST_SECTION_USERSETTINGS, TEST_KEY_INITSUBSCRIBEEND))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_INITSUBSCRIBEEND));
	if (setParamBool(_params._init_subscribe_start, TEST_SECTION_USERSETTINGS, TEST_KEY_INITSUBSCRIBESTART))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_INITSUBSCRIBESTART));
	if (setParamString(_params._last_launch_elem_path, TEST_SECTION_USERSETTINGS, TEST_KEY_LASTLAUNCHELEMPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_LASTLAUNCHELEMPATH));
	if (setParamString(_params._last_log_path, TEST_SECTION_USERSETTINGS, TEST_KEY_LASTLOGPATH))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_LASTLOGPATH));
	if (setParamBool(_params._log_enable, TEST_SECTION_USERSETTINGS, TEST_KEY_LOGENABLE))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_LOGENABLE));
	if (setParamString(_params._log_messages, TEST_SECTION_USERSETTINGS, TEST_KEY_LOGMESSAGES))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_LOGMESSAGES));
	if (setParamBool(_params._loop_timeout_substeps, TEST_SECTION_USERSETTINGS, TEST_KEY_LOOPTIMEOUTSUBSTEPS))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_LOOPTIMEOUTSUBSTEPS));
	if (setParamBool(_params._manual_action_timeout_status, TEST_SECTION_USERSETTINGS, TEST_KEY_MANUALACTIONTIMEOUTSTATUS))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_MANUALACTIONTIMEOUTSTATUS));
	if (setParamString(_params._manual_action_timeout_value, TEST_SECTION_USERSETTINGS, TEST_KEY_MANUALACTIONTIMEOUTVALUE))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_MANUALACTIONTIMEOUTVALUE));
	if (setParamBool(_params._new_log_format, TEST_SECTION_USERSETTINGS, TEST_KEY_NEWLOGFORMAT))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_NEWLOGFORMAT));
	if (setParamBool(_params._new_scxml_struct, TEST_SECTION_USERSETTINGS, TEST_KEY_NEWSCXMLSTRUCT))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_NEWSCXMLSTRUCT));
	if (setParamBool(_params._pir_invoke_for_scxml, TEST_SECTION_USERSETTINGS, TEST_KEY_PIRINVOKEFORSCXML))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_PIRINVOKEFORSCXML));
	if (setParamBool(_params._pir_wait_time, TEST_SECTION_USERSETTINGS, TEST_KEY_PIRWAITTIME))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_PIRWAITTIME));
	if (setParamString(_params._pir_wait_time_val, TEST_SECTION_USERSETTINGS, TEST_KEY_PIRWAITTIMEVAL))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_PIRWAITTIMEVAL));
	if (setParamBool(_params._print_table_status, TEST_SECTION_USERSETTINGS, TEST_KEY_PRINTTABLESTATUS))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_PRINTTABLESTATUS));
	if (setParamBool(_params._resize_row, TEST_SECTION_USERSETTINGS, TEST_KEY_RESIZEROW))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_RESIZEROW));
	if (setParamString(_params._result_view_saved_list, TEST_SECTION_USERSETTINGS, TEST_KEY_RESULTVIEWSAVEDLIST))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_RESULTVIEWSAVEDLIST));
	if (setParamBool(_params._save_before_export, TEST_SECTION_USERSETTINGS, TEST_KEY_SAVEBEFOREEXPORT))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_SAVEBEFOREEXPORT));
	if (setParamBool(_params._stand_alone_exec_mode, TEST_SECTION_USERSETTINGS, TEST_KEY_STANDALONEEXECMODE))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_STANDALONEEXECMODE));
	if (setParamBool(_params._subscribe_unsubscribe_timeout_status, TEST_SECTION_USERSETTINGS, TEST_KEY_SUBSCRIBEUNSUBSCRIBETIMEOUTSTATUS))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_SUBSCRIBEUNSUBSCRIBETIMEOUTSTATUS));
	if (setParamString(_params._subscribe_unsubscribe_timeout_value, TEST_SECTION_USERSETTINGS, TEST_KEY_SUBSCRIBEUNSUBSCRIBETIMEOUTVALUE))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_SUBSCRIBEUNSUBSCRIBETIMEOUTVALUE));
	if (setParamBool(_params._svn_add_on_exit, TEST_SECTION_USERSETTINGS, TEST_KEY_SVNADDONEXIT))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_SVNADDONEXIT));
	if (setParamBool(_params._svn_commit_on_exit, TEST_SECTION_USERSETTINGS, TEST_KEY_SVNCOMMITONEXIT))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_SVNCOMMITONEXIT));
	if (setParamBool(_params._svn_update_on_start, TEST_SECTION_USERSETTINGS, TEST_KEY_SVNUPDATEONSTART))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_SVNUPDATEONSTART));
	if (setParamBool(_params._uscxml_based_scxml_en, TEST_SECTION_USERSETTINGS, TEST_KEY_USCXMLBASEDSCXMLEN))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_USCXMLBASEDSCXMLEN));
	if (setParamBool(_params._validation_enable, TEST_SECTION_USERSETTINGS, TEST_KEY_VALIDATIONENABLE))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_VALIDATIONENABLE));
	if (setParamBool(_params._virtual_pmr, TEST_SECTION_USERSETTINGS, TEST_KEY_VIRTUALPMR))
		_error_log.push_back(std::make_pair(TEST_SECTION_USERSETTINGS, TEST_KEY_VIRTUALPMR));

	// GENERAL
	if (setParamString(_params._datatv_geometry, TEST_SECTION_GENERAL, TEST_KEY_DATATVGEOMETRY))
		_error_log.push_back(std::make_pair(TEST_SECTION_GENERAL, TEST_KEY_DATATVGEOMETRY));
	if (setParamString(_params._result_page_geometry, TEST_SECTION_GENERAL, TEST_KEY_RESULTPAGEGEOMETRY))
		_error_log.push_back(std::make_pair(TEST_SECTION_GENERAL, TEST_KEY_RESULTPAGEGEOMETRY));
	if (setParamString(_params._geometry, TEST_SECTION_GENERAL, TEST_KEY_GEOMETRY))
		_error_log.push_back(std::make_pair(TEST_SECTION_GENERAL, TEST_KEY_GEOMETRY));
	if (setParamString(_params._window_state, TEST_SECTION_GENERAL, TEST_KEY_WINDOWSTATE))
		_error_log.push_back(std::make_pair(TEST_SECTION_GENERAL, TEST_KEY_WINDOWSTATE));

	// LIVEVIEW USER SETTINGS
	if (setParamString(_params._bisg_ip, TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_BISGIP))
		_error_log.push_back(std::make_pair(TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_BISGIP));
	if (setParamInt(_params._bisg_port, TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_BISGPORT))
		_error_log.push_back(std::make_pair(TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_BISGPORT));
	if (setParamString(_params._tool_list, TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_TOOLLIST))
		_error_log.push_back(std::make_pair(TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_TOOLLIST));
	
	// deal with error_log
	if (_error_log.size() > 0)
		has_errors = true;

	if (printErrors) {
		for (const auto& log : _error_log) {
			QTextStream out(stdout);
			out << "Init config invalid field: " << 
			QString::fromStdString(log.first) << " - " << QString::fromStdString(log.second) << "\n";
		}
	}

	return !has_errors && checkValidity(printErrors);
}

/**
* @brief reset the 'has changed' variables of all parameters.
* @param None
* @return void
*/
void TestConfig::resetChanges()
{
	_params._library_path->_hasChanged = false;
	_params._log_dir_path->_hasChanged = false;
	_params._repository_path->_hasChanged = false;
	_params._tool_data_path->_hasChanged = false;
	_params._git_db_path->_hasChanged = false;

	_params._recording_tool->_hasChanged = false;

	_params._channel_selection->_hasChanged = false;
	_params._bench_db_startup->_hasChanged = false;
	_params._proc_db_startup->_hasChanged = false;
	_params._debug_mode_ip->_hasChanged = false;
	_params._debug_mode_port->_hasChanged = false;
	_params._decimal_places->_hasChanged = false;
	_params._dv_saved_list->_hasChanged = false;
	_params._engine_selection->_hasChanged = false;
	_params._en_invoke_local_var->_hasChanged = false;
	_params._generic_scxml->_hasChanged = false;
	_params._init_subscribe_end->_hasChanged = false;
	_params._init_subscribe_start->_hasChanged = false;
	_params._last_launch_elem_path->_hasChanged = false;
	_params._last_log_path->_hasChanged = false;
	_params._log_enable->_hasChanged = false;
	_params._log_messages->_hasChanged = false;
	_params._loop_timeout_substeps->_hasChanged = false;
	_params._manual_action_timeout_status->_hasChanged = false;
	_params._manual_action_timeout_value->_hasChanged = false;
	_params._new_log_format->_hasChanged = false;
	_params._new_scxml_struct->_hasChanged = false;
	_params._pir_invoke_for_scxml->_hasChanged = false;
	_params._pir_wait_time->_hasChanged = false;
	_params._pir_wait_time_val->_hasChanged = false;
	_params._print_table_status->_hasChanged = false;
	_params._resize_row->_hasChanged = false;
	_params._result_view_saved_list->_hasChanged = false;
	_params._save_before_export->_hasChanged = false;
	_params._stand_alone_exec_mode->_hasChanged = false;
	_params._subscribe_unsubscribe_timeout_status->_hasChanged = false;
	_params._subscribe_unsubscribe_timeout_value->_hasChanged = false;
	_params._svn_add_on_exit->_hasChanged = false;
	_params._svn_commit_on_exit->_hasChanged = false;
	_params._svn_update_on_start->_hasChanged = false;
	_params._uscxml_based_scxml_en->_hasChanged = false;
	_params._validation_enable->_hasChanged = false;
	_params._virtual_pmr->_hasChanged = false;

	_params._datatv_geometry->_hasChanged = false;
	_params._result_page_geometry->_hasChanged = false;
	_params._geometry->_hasChanged = false;
	_params._window_state->_hasChanged = false;

	_params._bisg_ip->_hasChanged = false;
	_params._bisg_port->_hasChanged = false;
	_params._tool_list->_hasChanged = false;
}

/**
* @brief Main method for saving the configuration file.
* @param None
* @return void
*/
void TestConfig::saveConf()
{
	// PATH
	if (_params._library_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_LIBRARYPATH, _params._library_path->_value);
	if (_params._log_dir_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_LOGDIRPATH, _params._log_dir_path->_value);
	if (_params._repository_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_REPOSITORYPATH, _params._repository_path->_value);
	if (_params._tool_data_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_TOOLDATAPATH, _params._tool_data_path->_value);
	if (_params._git_db_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_GITDBPATH, _params._git_db_path->_value);
	if (_params._gta_controller_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_GTACONTROLLER, _params._gta_controller_path->_value);
	if (_params._auto_launcher_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_AUTOLAUNCHERPATH, _params._auto_launcher_path->_value);
	if (_params._auto_launcher_config_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_AUTOLAUNCHERCONFIGPATH, _params._auto_launcher_config_path->_value);
	if (_params._gta_controller_output_log_path->_hasChanged)
		_configManager->set(TEST_SECTION_PATH, TEST_KEY_GTACONTROLLEROUTPUTLOGPATH, _params._gta_controller_output_log_path->_value);
	// RECORDING SETTINGS
	if (_params._recording_tool->_hasChanged)
		_configManager->set(RECORDING_SETTINGS, VIDEO_RECORDING_TOOL, _params._recording_tool->_value);

	// USER SETTINGS
	if (_params._channel_selection->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_CHANNELSELECTION, bool2str(_params._channel_selection->_value));
	if (_params._bench_db_startup->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_BENCHDBSTARTUP, bool2str(_params._bench_db_startup->_value));
	if (_params._proc_db_startup->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_PROCDBSTARTUP, bool2str(_params._proc_db_startup->_value));
	if (_params._debug_mode_ip->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_DEBUGMODEIP, _params._debug_mode_ip->_value);
	if (_params._debug_mode_port->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_DEBUGMODEPORT, std::to_string(_params._debug_mode_port->_value));
	if (_params._decimal_places->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_DECIMALPLACES, _params._decimal_places->_value);
	if (_params._dv_saved_list->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_DVSAVEDLIST, _params._dv_saved_list->_value);
	if (_params._engine_selection->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_ENGINESELECTION, _params._engine_selection->_value);
	if (_params._en_invoke_local_var->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_ENINVOKELOCALVAR, bool2str(_params._en_invoke_local_var->_value));
	if (_params._generic_scxml->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_GENERICSCXML, bool2str(_params._generic_scxml->_value));
	if (_params._init_subscribe_end->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_INITSUBSCRIBEEND, bool2str(_params._init_subscribe_end->_value));
	if (_params._init_subscribe_start->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_INITSUBSCRIBESTART, bool2str(_params._init_subscribe_start->_value));
	if (_params._last_launch_elem_path->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_LASTLAUNCHELEMPATH, _params._last_launch_elem_path->_value);
	if (_params._last_log_path->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_LASTLOGPATH, _params._last_log_path->_value);
	if (_params._log_enable->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_LOGENABLE, bool2str(_params._log_enable->_value));
	if (_params._log_messages->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_LOGMESSAGES, _params._log_messages->_value);
	if (_params._loop_timeout_substeps->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_LOOPTIMEOUTSUBSTEPS, bool2str(_params._loop_timeout_substeps->_value));
	if (_params._manual_action_timeout_status->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_MANUALACTIONTIMEOUTSTATUS, bool2str(_params._manual_action_timeout_status->_value));
	if (_params._manual_action_timeout_value->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_MANUALACTIONTIMEOUTVALUE, _params._manual_action_timeout_value->_value);
	if (_params._new_log_format->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_NEWLOGFORMAT, bool2str(_params._new_log_format->_value));
	if (_params._new_scxml_struct->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_NEWSCXMLSTRUCT, bool2str(_params._new_scxml_struct->_value));
	if (_params._pir_invoke_for_scxml->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_PIRINVOKEFORSCXML, bool2str(_params._pir_invoke_for_scxml->_value));
	if (_params._pir_wait_time->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_PIRWAITTIME, bool2str(_params._pir_wait_time->_value));
	if (_params._pir_wait_time_val->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_PIRWAITTIMEVAL, _params._pir_wait_time_val->_value);
	if (_params._print_table_status->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_PRINTTABLESTATUS, bool2str(_params._print_table_status->_value));
	if (_params._resize_row->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_RESIZEROW, bool2str(_params._resize_row->_value));
	if (_params._result_view_saved_list->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_RESULTVIEWSAVEDLIST, _params._result_view_saved_list->_value);
	if (_params._save_before_export->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_SAVEBEFOREEXPORT, bool2str(_params._save_before_export->_value));
	if (_params._stand_alone_exec_mode->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_STANDALONEEXECMODE, bool2str(_params._stand_alone_exec_mode->_value));
	if (_params._subscribe_unsubscribe_timeout_status->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_SUBSCRIBEUNSUBSCRIBETIMEOUTSTATUS, bool2str(_params._subscribe_unsubscribe_timeout_status->_value));
	if (_params._subscribe_unsubscribe_timeout_value->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_SUBSCRIBEUNSUBSCRIBETIMEOUTVALUE, _params._subscribe_unsubscribe_timeout_value->_value);
	if (_params._svn_add_on_exit->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_SVNADDONEXIT, bool2str(_params._svn_add_on_exit->_value));
	if (_params._svn_commit_on_exit->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_SVNCOMMITONEXIT, bool2str(_params._svn_commit_on_exit->_value));
	if (_params._svn_update_on_start->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_SVNUPDATEONSTART, bool2str(_params._svn_update_on_start->_value));
	if (_params._uscxml_based_scxml_en->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_USCXMLBASEDSCXMLEN, bool2str(_params._uscxml_based_scxml_en->_value));
	if (_params._validation_enable->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_VALIDATIONENABLE, bool2str(_params._validation_enable->_value));
	if (_params._virtual_pmr->_hasChanged)
		_configManager->set(TEST_SECTION_USERSETTINGS, TEST_KEY_VIRTUALPMR, bool2str(_params._virtual_pmr->_value));

	// GENERAL
	if (_params._datatv_geometry->_hasChanged)
		_configManager->set(TEST_SECTION_GENERAL, TEST_KEY_DATATVGEOMETRY, _params._datatv_geometry->_value);
	if (_params._result_page_geometry->_hasChanged)
		_configManager->set(TEST_SECTION_GENERAL, TEST_KEY_RESULTPAGEGEOMETRY, _params._result_page_geometry->_value);
	if (_params._geometry->_hasChanged)
		_configManager->set(TEST_SECTION_GENERAL, TEST_KEY_GEOMETRY, _params._geometry->_value);
	if (_params._window_state->_hasChanged)
		_configManager->set(TEST_SECTION_GENERAL, TEST_KEY_WINDOWSTATE, _params._window_state->_value);

	// LIVEVIEW USER SETTINGS
	if (_params._bisg_ip->_hasChanged)
		_configManager->set(TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_BISGIP, _params._bisg_ip->_value);
	if (_params._bisg_port->_hasChanged)
		_configManager->set(TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_BISGPORT, std::to_string(_params._bisg_port->_value));
	if (_params._tool_list->_hasChanged)
		_configManager->set(TEST_SECTION_LIVEVIEWUSERSETTINGS, TEST_KEY_TOOLLIST, _params._tool_list->_value);

	_configManager->write();
	resetChanges();
}

/**
* @brief Set the configuration file path.
* @param path The path to the configuration file.
* @return void
*/
void TestConfig::setPath(QString path)
{
	_path = path;
	_configManager->setPath(path);
}

// parameters getters
std::string TestConfig::getLibraryPath()
{
	return _params._library_path->get();
}

std::string TestConfig::getLogDirPath()
{
	return _params._log_dir_path->get();
}

std::string TestConfig::getRepositoryPath()
{
	return _params._repository_path->get();
}

std::string TestConfig::getToolDataPath()
{
	return _params._tool_data_path->get();
}

std::string TestConfig::getGitDbPath() {
	return _params._git_db_path->get();
}

/**
* The function return the GTA Controller exe path from app configuration ini file
* return GTA Controller exe path if exists in configuration ini file otherwise empty string is returned
*/
std::string TestConfig::getGTAControllerPath()
{
	return _params._gta_controller_path->get();
}

/**
* The function return the AutoLauncher Watch Folder path from app configuration ini file
* return AutoLauncher Watch folder path if exists in configuration ini file otherwise empty string is returned
*/
std::string TestConfig::getAutoLauncherPath()
{
	return _params._auto_launcher_path->get();
}

/**
* The function return the AutoLauncher Config file path from configuration settings
* @return AutoLauncher Config file path from configuration settings if exists otherwise empty string is returned
*/
std::string TestConfig::getAutoLauncherConfigPath()
{
	return _params._auto_launcher_config_path->get();
}

std::string TestConfig::getGTAControllerOutputLogPath()
{
	return _params._gta_controller_output_log_path->get();
}

std::string TestConfig::getRecordingToolName()
{
	return _params._recording_tool->get();
}

bool TestConfig::getInitSubscribeEnd()
{
	return _params._init_subscribe_end->get();
}

bool TestConfig::getInitSubscribeStart()
{
	return _params._init_subscribe_start->get();
}

std::string TestConfig::getLastLaunchElemPath()
{
	return _params._last_launch_elem_path->get();
}

std::string TestConfig::getLastLogPath()
{
	return _params._last_log_path->get();
}

bool TestConfig::getLogEnable()
{
	return _params._log_enable->get();
}

std::string TestConfig::getLogMessages()
{
	return _params._log_messages->get();
}

bool TestConfig::getLoopTimeoutSubsteps()
{
	return _params._loop_timeout_substeps->get();
}

bool TestConfig::getManualActionTimeoutStatus()
{
	return _params._manual_action_timeout_status->get();
}

std::string TestConfig::getManualActionTimeoutValue()
{
	return _params._manual_action_timeout_value->get();
}

bool TestConfig::getNewLogFormat()
{
	return _params._new_log_format->get();
}

bool TestConfig::getNewSCXMLStruct()
{
	return _params._new_scxml_struct->get();
}

bool TestConfig::getPIRInvokeForSCXML()
{
	return _params._pir_invoke_for_scxml->get();
}

bool TestConfig::getPIRWaitTime()
{
	return _params._pir_wait_time->get();
}

std::string TestConfig::getPIRWaitTimeVal()
{
	return _params._pir_wait_time_val->get();
}

bool TestConfig::getPrintTableStatus()
{
	return _params._print_table_status->get();
}

bool TestConfig::getResizeRow()
{
	return _params._resize_row->get();
}

std::string TestConfig::getResultViewSavedList()
{
	return _params._result_view_saved_list->get();
}

bool TestConfig::getSaveBeforeExport()
{
	return _params._save_before_export->get();
}

bool TestConfig::getStandAloneExecMode()
{
	return _params._stand_alone_exec_mode->get();
}

bool TestConfig::getSubscribeUnsubscribeTimeoutStatus()
{
	return _params._subscribe_unsubscribe_timeout_status->get();
}

std::string TestConfig::getSubscribeUnsubscribeTimeoutValue()
{
	return _params._subscribe_unsubscribe_timeout_value->get();
}

bool TestConfig::getSVNAddOnExit()
{
	return _params._svn_add_on_exit->get();
}

bool TestConfig::getSVNCommitOnExit()
{
	return _params._svn_commit_on_exit->get();
}

bool TestConfig::getSVNUpdateOnStart()
{
	return _params._svn_update_on_start->get();
}

bool TestConfig::getUSCXMLBasedSCXMLEN()
{
	return _params._uscxml_based_scxml_en->get();
}

bool TestConfig::getValidationEnable()
{
	return _params._validation_enable->get();
}

bool TestConfig::getVirtualPMR()
{
	return _params._virtual_pmr->get();
}

bool TestConfig::getChannelSelection()
{
	return _params._channel_selection->get();
}

bool TestConfig::getBenchDBStartup()
{
	return _params._bench_db_startup->get();
}

bool TestConfig::getProcDBStartup()
{
	return _params._proc_db_startup->get();
}

std::string TestConfig::getDebugModeIP()
{
	return _params._debug_mode_ip->get();
}

int TestConfig::getDebugModePort()
{
	return _params._debug_mode_port->get();
}

std::string TestConfig::getDecimalPlaces()
{
	return _params._decimal_places->get();
}

std::string TestConfig::getDVSavedList()
{
	return _params._dv_saved_list->get();
}

std::string TestConfig::getEngineSelection()
{
	return _params._engine_selection->get();
}

bool TestConfig::getENInvokeLocalVar()
{
	return _params._en_invoke_local_var->get();
}

bool TestConfig::getGenericSCXML()
{
	return _params._generic_scxml->get();
}

std::string TestConfig::getDataTVGeometry()
{
	std::string datatv = _params._datatv_geometry->get();
	if (datatv.find("@ByteArray(") != std::string::npos)
	{
		size_t pos = datatv.find("(");
		datatv = datatv.substr(pos + 1, datatv.size() - pos - 2);
	}
	return datatv;
}

std::string TestConfig::getResultPageGeometry()
{
	std::string resultPageGeometry = _params._result_page_geometry->get();
	if (resultPageGeometry.find("@ByteArray(") != std::string::npos)
	{
		size_t pos = resultPageGeometry.find("(");
		resultPageGeometry = resultPageGeometry.substr(pos + 1, resultPageGeometry.size() - pos - 2);
	}
	return resultPageGeometry;
}

std::string TestConfig::getGeometry()
{
	std::string geometry = _params._geometry->get();
	if (geometry.find("@ByteArray(") != std::string::npos)
	{
		size_t pos = geometry.find("(");
		geometry = geometry.substr(pos + 1, geometry.size() - pos - 2);
	}
	return geometry;
}

std::string TestConfig::getWindowState()
{
	std::string windowState = _params._window_state->get();
	if (windowState.find("@ByteArray(") != std::string::npos)
	{
		size_t pos = windowState.find("(");
		windowState = windowState.substr(pos + 1, windowState.size() - pos - 2);
	}
	return windowState;
}

std::string TestConfig::getBISGIP()
{
	return _params._bisg_ip->get();
}

int TestConfig::getBISGPort()
{
	return _params._bisg_port->get();
}

std::string TestConfig::getToolList()
{
	return _params._tool_list->get();
}

// parameters setters
void TestConfig::setLibraryPath(const std::string& value)
{
	_params._library_path->_value = value;
	if (value != _params._library_path->_default || !_params._library_path->_useDefault)
	{
		_params._library_path->_hasChanged = true;
		_params._library_path->_useDefault = false;
	}
}

void TestConfig::setLogDirPath(const std::string& value)
{
	_params._log_dir_path->_value = value;
	if (value != _params._log_dir_path->_default || !_params._log_dir_path->_useDefault)
	{
		_params._log_dir_path->_hasChanged = true;
		_params._log_dir_path->_useDefault = false;
	}
}


void TestConfig::setRepositoryPath(const std::string& value)
{
	_params._repository_path->_value = value;
	if (value != _params._repository_path->_default || !_params._repository_path->_useDefault)
	{
		_params._repository_path->_hasChanged = true;
		_params._repository_path->_useDefault = false;
	}
}

void TestConfig::setToolDataPath(const std::string& value)
{
	_params._tool_data_path->_value = value;
	if (value != _params._tool_data_path->_default || !_params._tool_data_path->_useDefault)
	{
		_params._tool_data_path->_hasChanged = true;
		_params._tool_data_path->_useDefault = false;
	}
}

void TestConfig::setGitDbPath(const std::string& value)
{
	_params._git_db_path->_value = value;
	if (value != _params._git_db_path->_default || !_params._git_db_path->_useDefault)
	{
		_params._git_db_path->_hasChanged = true;
		_params._git_db_path->_useDefault = false;
	}
}

/**
* The function used to set the GTA Controller exe path which will be saved to app configuration ini file
* @param1 : value -> path of GTA Controller exe
*/
void TestConfig::setGTAControllerPath(const std::string& value)
{
	_params._gta_controller_path->_value = value;
	if (value != _params._gta_controller_path->_default || !_params._gta_controller_path->_useDefault)
	{
		_params._gta_controller_path->_hasChanged = true;
		_params._gta_controller_path->_useDefault = false;
	}
}

/**
* The function used to set the AutoLauncher watch folder path which will be saved to app configuration ini file
* @param1 : value -> path of AutoLauncher watch folder
*/
void TestConfig::setAutoLauncherPath(const std::string& value)
{
	_params._auto_launcher_path->_value = value;
	if (value != _params._auto_launcher_path->_default || !_params._auto_launcher_path->_useDefault)
	{
		_params._auto_launcher_path->_hasChanged = true;
		_params._auto_launcher_path->_useDefault = false;
	}
}

/**
* The function used to set the AutoLauncher config file path which will be saved to app configuration ini file
* @param1 : value -> path of AutoLauncher config file path
*/
void TestConfig::setAutoLauncherConfigPath(const std::string& value)
{
	_params._auto_launcher_config_path->_value = value;
	if (value != _params._auto_launcher_config_path->_default || !_params._auto_launcher_config_path->_useDefault)
	{
		_params._auto_launcher_config_path->_hasChanged = true;
		_params._auto_launcher_config_path->_useDefault = false;
	}
}

void TestConfig::setGTAControllerOutputLogPath(const std::string& value)
{
	_params._gta_controller_output_log_path->_value = value;
	if (value != _params._gta_controller_output_log_path->_default || !_params._gta_controller_output_log_path->_useDefault)
	{
		_params._gta_controller_output_log_path->_hasChanged = true;
		_params._gta_controller_output_log_path->_useDefault = false;
	}
}

void TestConfig::setRecordingToolName(const std::string& value)
{
	_params._recording_tool->_value = value;
	if (value != _params._recording_tool->_default || !_params._recording_tool->_useDefault)
	{
		_params._recording_tool->_hasChanged = true;
		_params._recording_tool->_useDefault = false;
	}
}

void TestConfig::setChannelSelection(const bool& value)
{
	_params._channel_selection->_value = value;
	if (value != _params._channel_selection->_default || !_params._channel_selection->_useDefault)
	{
		_params._channel_selection->_hasChanged = true;
		_params._channel_selection->_useDefault = false;
	}
}

void TestConfig::setBenchDBStartup(const bool& value)
{
	_params._bench_db_startup->_value = value;
	if (value != _params._bench_db_startup->_default || !_params._bench_db_startup->_useDefault)
	{
		_params._bench_db_startup->_hasChanged = true;
		_params._bench_db_startup->_useDefault = false;
	}
}

void TestConfig::setProcDBStartup(const bool& value)
{
	_params._proc_db_startup->_value = value;
	if (value != _params._proc_db_startup->_default || !_params._proc_db_startup->_useDefault)
	{
		_params._proc_db_startup->_hasChanged = true;
		_params._proc_db_startup->_useDefault = false;
	}
}

void TestConfig::setDebugModeIP(const std::string& value)
{
	_params._debug_mode_ip->_value = value;
	if (value != _params._debug_mode_ip->_default || !_params._debug_mode_ip->_useDefault)
	{
		_params._debug_mode_ip->_hasChanged = true;
		_params._debug_mode_ip->_useDefault = false;
	}
}

void TestConfig::setDebugModePort(const int& value)
{
	_params._debug_mode_port->_value = value;
	if (value != _params._debug_mode_port->_default || !_params._debug_mode_port->_useDefault)
	{
		_params._debug_mode_port->_hasChanged = true;
		_params._debug_mode_port->_useDefault = false;
	}
}

void TestConfig::setDecimalPlaces(const std::string& value)
{
	_params._decimal_places->_value = value;
	if (value != _params._decimal_places->_default || !_params._decimal_places->_useDefault)
	{
		_params._decimal_places->_hasChanged = true;
		_params._decimal_places->_useDefault = false;
	}
}

void TestConfig::setDVSavedList(const std::string& value)
{
	_params._dv_saved_list->_value = value;
	if (value != _params._dv_saved_list->_default || !_params._dv_saved_list->_useDefault)
	{
		_params._dv_saved_list->_hasChanged = true;
		_params._dv_saved_list->_useDefault = false;
	}
}

void TestConfig::setEngineSelection(const std::string& value)
{
	_params._engine_selection->_value = value;
	if (value != _params._engine_selection->_default || !_params._engine_selection->_useDefault)
	{
		_params._engine_selection->_hasChanged = true;
		_params._engine_selection->_useDefault = false;
	}
}

void TestConfig::setENInvokeLocalVar(const bool& value)
{
	_params._en_invoke_local_var->_value = value;
	if (value != _params._en_invoke_local_var->_default || !_params._en_invoke_local_var->_useDefault)
	{
		_params._en_invoke_local_var->_hasChanged = true;
		_params._en_invoke_local_var->_useDefault = false;
	}
}

void TestConfig::setGenericSCXML(const bool& value)
{
	_params._generic_scxml->_value = value;
	if (value != _params._generic_scxml->_default || !_params._generic_scxml->_useDefault)
	{
		_params._generic_scxml->_hasChanged = true;
		_params._generic_scxml->_useDefault = false;
	}
}

void TestConfig::setInitSubscribeEnd(const bool& value)
{
	_params._init_subscribe_end->_value = value;
	if (value != _params._init_subscribe_end->_default || !_params._init_subscribe_end->_useDefault)
	{
		_params._init_subscribe_end->_hasChanged = true;
		_params._init_subscribe_end->_useDefault = false;
	}
}

void TestConfig::setInitSubscribeStart(const bool& value)
{
	_params._init_subscribe_start->_value = value;
	if (value != _params._init_subscribe_start->_default || !_params._init_subscribe_start->_useDefault)
	{
		_params._init_subscribe_start->_hasChanged = true;
		_params._init_subscribe_start->_useDefault = false;
	}
}

void TestConfig::setLastLaunchElemPath(const std::string& value)
{
	_params._last_launch_elem_path->_value = value;
	if (value != _params._last_launch_elem_path->_default || !_params._last_launch_elem_path->_useDefault)
	{
		_params._last_launch_elem_path->_hasChanged = true;
		_params._last_launch_elem_path->_useDefault = false;
	}
}

void TestConfig::setLastLogPath(const std::string& value)
{
	_params._last_log_path->_value = value;
	if (value != _params._last_log_path->_default || !_params._last_log_path->_useDefault)
	{
		_params._last_log_path->_hasChanged = true;
		_params._last_log_path->_useDefault = false;
	}
}

void TestConfig::setLogEnable(const bool& value)
{
	_params._log_enable->_value = value;
	if (value != _params._log_enable->_default || !_params._log_enable->_useDefault)
	{
		_params._log_enable->_hasChanged = true;
		_params._log_enable->_useDefault = false;
	}
}

void TestConfig::setLogMessages(const std::string& value)
{
	_params._log_messages->_value = value;
	if (value != _params._log_messages->_default || !_params._log_messages->_useDefault)
	{
		_params._log_messages->_hasChanged = true;
		_params._log_messages->_useDefault = false;
	}
}

void TestConfig::setLoopTimeoutSubsteps(const bool& value)
{
	_params._loop_timeout_substeps->_value = value;
	if (value != _params._loop_timeout_substeps->_default || !_params._loop_timeout_substeps->_useDefault)
	{
		_params._loop_timeout_substeps->_hasChanged = true;
		_params._loop_timeout_substeps->_useDefault = false;
	}
}

void TestConfig::setManualActionTimeoutStatus(const bool& value)
{
	_params._manual_action_timeout_status->_value = value;
	if (value != _params._manual_action_timeout_status->_default || !_params._manual_action_timeout_status->_useDefault)
	{
		_params._manual_action_timeout_status->_hasChanged = true;
		_params._manual_action_timeout_status->_useDefault = false;
	}
}

void TestConfig::setManualActionTimeoutValue(const std::string& value)
{
	_params._manual_action_timeout_value->_value = value;
	if (value != _params._manual_action_timeout_value->_default || !_params._manual_action_timeout_value->_useDefault)
	{
		_params._manual_action_timeout_value->_hasChanged = true;
		_params._manual_action_timeout_value->_useDefault = false;
	}
}

void TestConfig::setNewLogFormat(const bool& value)
{
	_params._new_log_format->_value = value;
	if (value != _params._new_log_format->_default || !_params._new_log_format->_useDefault)
	{
		_params._new_log_format->_hasChanged = true;
		_params._new_log_format->_useDefault = false;
	}
}

void TestConfig::setNewSCXMLStruct(const bool& value)
{
	_params._new_scxml_struct->_value = value;
	if (value != _params._new_scxml_struct->_default || !_params._new_scxml_struct->_useDefault)
	{
		_params._new_scxml_struct->_hasChanged = true;
		_params._new_scxml_struct->_useDefault = false;
	}
}

void TestConfig::setPIRInvokeForSCXML(const bool& value)
{
	_params._pir_invoke_for_scxml->_value = value;
	if (value != _params._pir_invoke_for_scxml->_default || !_params._pir_invoke_for_scxml->_useDefault)
	{
		_params._pir_invoke_for_scxml->_hasChanged = true;
		_params._pir_invoke_for_scxml->_useDefault = false;
	}
}

void TestConfig::setPIRWaitTime(const bool& value)
{
	_params._pir_wait_time->_value = value;
	if (value != _params._pir_wait_time->_default || !_params._pir_wait_time->_useDefault)
	{
		_params._pir_wait_time->_hasChanged = true;
		_params._pir_wait_time->_useDefault = false;
	}
}

void TestConfig::setPIRWaitTimeVal(const std::string& value)
{
	_params._pir_wait_time_val->_value = value;
	if (value != _params._pir_wait_time_val->_default || !_params._pir_wait_time_val->_useDefault)
	{
		_params._pir_wait_time_val->_hasChanged = true;
		_params._pir_wait_time_val->_useDefault = false;
	}
}

void TestConfig::setPrintTableStatus(const bool& value)
{
	_params._print_table_status->_value = value;
	if (value != _params._print_table_status->_default || !_params._print_table_status->_useDefault)
	{
		_params._print_table_status->_hasChanged = true;
		_params._print_table_status->_useDefault = false;
	}
}

void TestConfig::setResizeRow(const bool& value)
{
	_params._resize_row->_value = value;
	if (value != _params._resize_row->_default || !_params._resize_row->_useDefault || !_params._resize_row->_useDefault)
	{
		_params._resize_row->_hasChanged = true;
		_params._resize_row->_useDefault = false;
	}
}

void TestConfig::setResultViewSavedList(const std::string& value)
{
	_params._result_view_saved_list->_value = value;
	if (value != _params._result_view_saved_list->_default || !_params._result_view_saved_list->_useDefault)
	{
		_params._result_view_saved_list->_hasChanged = true;
		_params._result_view_saved_list->_useDefault = false;
	}
}

void TestConfig::setSaveBeforeExport(const bool& value)
{
	_params._save_before_export->_value = value;
	if (value != _params._save_before_export->_default || !_params._save_before_export->_useDefault)
	{
		_params._save_before_export->_hasChanged = true;
		_params._save_before_export->_useDefault = false;
	}
}

void TestConfig::setStandAloneExecMode(const bool& value)
{
	_params._stand_alone_exec_mode->_value = value;
	if (value != _params._stand_alone_exec_mode->_default || !_params._stand_alone_exec_mode->_useDefault)
	{
		_params._stand_alone_exec_mode->_hasChanged = true;
		_params._stand_alone_exec_mode->_useDefault = false;
	}
}

void TestConfig::setSubscribeUnsubscribeTimeoutStatus(const bool& value)
{
	_params._subscribe_unsubscribe_timeout_status->_value = value;
	if (value != _params._subscribe_unsubscribe_timeout_status->_default || !_params._subscribe_unsubscribe_timeout_status->_useDefault)
	{
		_params._subscribe_unsubscribe_timeout_status->_hasChanged = true;
		_params._subscribe_unsubscribe_timeout_status->_useDefault = false;
	}
}

void TestConfig::setSubscribeUnsubscribeTimeoutValue(const std::string& value)
{
	_params._subscribe_unsubscribe_timeout_value->_value = value;
	if (value != _params._subscribe_unsubscribe_timeout_value->_default || !_params._subscribe_unsubscribe_timeout_value->_useDefault)
	{
		_params._subscribe_unsubscribe_timeout_value->_hasChanged = true;
		_params._subscribe_unsubscribe_timeout_value->_useDefault = false;
	}
}

void TestConfig::setSVNAddOnExit(const bool& value)
{
	_params._svn_add_on_exit->_value = value;
	if (value != _params._svn_add_on_exit->_default || !_params._svn_add_on_exit->_useDefault)
	{
		_params._svn_add_on_exit->_hasChanged = true;
		_params._svn_add_on_exit->_useDefault = false;
	}
}

void TestConfig::setSVNCommitOnExit(const bool& value)
{
	_params._svn_commit_on_exit->_value = value;
	if (value != _params._svn_commit_on_exit->_default || !_params._svn_commit_on_exit->_useDefault)
	{
		_params._svn_commit_on_exit->_hasChanged = true;
		_params._svn_commit_on_exit->_useDefault = false;
	}
}

void TestConfig::setSVNUpdateOnStart(const bool& value)
{
	_params._svn_update_on_start->_value = value;
	if (value != _params._svn_update_on_start->_default || !_params._svn_update_on_start->_useDefault)
	{
		_params._svn_update_on_start->_hasChanged = true;
		_params._svn_update_on_start->_useDefault = false;
	}
}

void TestConfig::setUSCXMLBasedSCXMLEN(const bool& value)
{
	_params._uscxml_based_scxml_en->_value = value;
	if (value != _params._uscxml_based_scxml_en->_default || !_params._uscxml_based_scxml_en->_useDefault)
	{
		_params._uscxml_based_scxml_en->_hasChanged = true;
		_params._uscxml_based_scxml_en->_useDefault = false;
	}
}

void TestConfig::setValidationEnable(const bool& value)
{
	_params._validation_enable->_value = value;
	if (value != _params._validation_enable->_default || !_params._validation_enable->_useDefault)
	{
		_params._validation_enable->_hasChanged = true;
		_params._validation_enable->_useDefault = false;
	}
}

void TestConfig::setVirtualPMR(const bool& value)
{
	_params._virtual_pmr->_value = value;
	if (value != _params._virtual_pmr->_default || !_params._virtual_pmr->_useDefault)
	{
		_params._virtual_pmr->_hasChanged = true;
		_params._virtual_pmr->_useDefault = false;
	}
}

void TestConfig::setDataTVGeometry(const std::string& value)
{
	_params._datatv_geometry->_value = value;
	if (value != _params._datatv_geometry->_default || !_params._datatv_geometry->_useDefault)
	{
		_params._datatv_geometry->_hasChanged = true;
		_params._datatv_geometry->_useDefault = false;
	}
}

void TestConfig::setResultPageGeometry(const std::string& value)
{
	_params._result_page_geometry->_value = value;
	if (value != _params._result_page_geometry->_default || !_params._result_page_geometry->_useDefault)
	{
		_params._result_page_geometry->_hasChanged = true;
		_params._result_page_geometry->_useDefault = false;
	}
}

void TestConfig::setGeometry(const std::string& value)
{
	_params._geometry->_value = value;
	if (value != _params._geometry->_default || !_params._geometry->_useDefault)
	{
		_params._geometry->_hasChanged = true;
		_params._geometry->_useDefault = false;
	}
}

void TestConfig::setWindowState(const std::string& value)
{
	_params._window_state->_value = value;
	if (value != _params._window_state->_default || !_params._window_state->_useDefault)
	{
		_params._window_state->_hasChanged = true;
		_params._window_state->_useDefault = false;
	}
}

void TestConfig::setBISGIP(const std::string& value)
{
	_params._bisg_ip->_value = value;
	if (value != _params._bisg_ip->_default || !_params._bisg_ip->_useDefault)
	{
		_params._bisg_ip->_hasChanged = true;
		_params._bisg_ip->_useDefault = false;
	}
}

void TestConfig::setBISGPort(const int& value)
{
	_params._bisg_port->_value = value;
	if (value != _params._bisg_port->_default || !_params._bisg_port->_useDefault)
	{
		_params._bisg_port->_hasChanged = true;
		_params._bisg_port->_useDefault = false;
	}
}

void TestConfig::setToolList(const std::string& value)
{
	_params._tool_list->_value = value;
	if (value != _params._tool_list->_default || !_params._tool_list->_useDefault)
	{
		_params._tool_list->_hasChanged = true;
		_params._tool_list->_useDefault = false;
	}
}
