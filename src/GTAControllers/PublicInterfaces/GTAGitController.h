#pragma once
#include "GTAControllers.h"
#include "GTAUtil.h"

#include <iostream>
#include <optional>

#include "git2.h"

#pragma warning(push, 0)
#include <qfile.h>
#include <qdir.h>
#pragma warning(pop)

#pragma warning (disable : 4251)
/**
 * @brief Controller for handling Git functionalities
*/
class GTAControllers_SHARED_EXPORT GtaGitController {
public:
	/**
	 * @brief C-tor, initializes gitlib library
	*/
	GtaGitController();

	void setRepoPath(const std::string repoPath) { _repoPath = repoPath; }

	/**
	 * @brief Opens repository at given path. 
	          Closes old repo and opens new one if repo was already opened
	 * @return nullopt if success, error string if fail
	*/
	std::optional<std::string> openRepo();

	/**
	 * @brief Checks if repo is opened
	*/
	bool isRepoOpened();

	/**
	 * @brief Checks git status for given filepath
	*/
	GitStatus CheckFileStatus(const std::string& filePath);

	std::string getRepoPath() { return _repoPath; }

	/**
	 * @brief Returns pointer to controller
	*/
	static GtaGitController* getGtaGitController();

	std::optional<std::string> removeFileFromIndex(const std::string& path);

private:
	/**
	 * @brief Converts status returned by gitlib library to an enum
	*/
	GitStatus ConvertToStatus(unsigned int status);

	/**
	 * @brief Returns error string from last action
	*/
	std::string GetError(int errorCode, const std::string& action);

	/**
	 * @brief Opens repository at given path.
	*/
	std::optional<std::string> openRepoInternal(const std::string& repoPath);

	git_repository* _repo = nullptr;
	std::string _repoPath = "";

	static GtaGitController* _gitController;
};

