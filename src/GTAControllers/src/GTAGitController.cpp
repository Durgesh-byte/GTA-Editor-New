#include "GtaGitController.h"

GtaGitController* GtaGitController::_gitController = 0;
GtaGitController::GtaGitController() {
	git_libgit2_init();
}

std::optional<std::string> GtaGitController::openRepo() {
	if (!_repo) {
		return openRepoInternal(_repoPath);
	}
	else {
		git_repository_free(_repo);
		return openRepoInternal(_repoPath);
	}
}

std::optional<std::string> GtaGitController::openRepoInternal(const std::string& repoPath) {
	int errorCode = git_repository_open(&_repo, repoPath.c_str());
	if (errorCode) {
		return GetError(errorCode, "Repo initialization");
	}
	else {
		_repoPath = repoPath;
		return std::nullopt;
	}
}

bool GtaGitController::isRepoOpened() {
	return _repo;
}

GitStatus GtaGitController::CheckFileStatus(const std::string& filePath) {
	unsigned int statusFlag = 0;
	if (_repo) {
		git_status_file(&statusFlag, _repo, filePath.c_str());
	}

	return ConvertToStatus(statusFlag);
}

GitStatus GtaGitController::ConvertToStatus(unsigned int status) {
	switch (status) {
	case GIT_STATUS_CURRENT:
		return GitStatus::Current;
	case GIT_STATUS_INDEX_NEW:
		return GitStatus::IndexNew;
	case GIT_STATUS_INDEX_MODIFIED:
		return GitStatus::IndexModified;
	case GIT_STATUS_INDEX_DELETED:
		return GitStatus::IndexDeleted;
	case GIT_STATUS_INDEX_RENAMED:
		return GitStatus::IndexRenamed;
	case GIT_STATUS_INDEX_TYPECHANGE:
		return GitStatus::IndexTypechange;
	case GIT_STATUS_WT_NEW:
		return GitStatus::WtNew;
	case GIT_STATUS_WT_MODIFIED:
		return GitStatus::WtModified;
	case GIT_STATUS_WT_DELETED:
		return GitStatus::WtDeleted;
	case GIT_STATUS_WT_TYPECHANGE:
		return GitStatus::WtTypechange;
	case GIT_STATUS_WT_RENAMED:
		return GitStatus::WtTypechange;
	case GIT_STATUS_WT_UNREADABLE:
		return GitStatus::WtUnreadable;
	case GIT_STATUS_IGNORED:
		return GitStatus::Ignored;
	case GIT_STATUS_CONFLICTED:
		return GitStatus::Conflicted;
	case GIT_ENOTFOUND:
		return GitStatus::NotFound;
	default:
		return GitStatus::Unknown;
	}
}

std::string GtaGitController::GetError(int errorCode, const std::string& action) {
	const git_error* error = git_error_last();
	if (!errorCode)
		return "";
	std::string ret = ("Error %d %s - %s\n", errorCode, action, (error && error->message) ? error->message : "???");

	return ret;
}

GtaGitController* GtaGitController::getGtaGitController() {
	if (_gitController == 0)
		_gitController = new GtaGitController();
	return _gitController;
}

/**
 * @brief Removes given file from index, meaning the file will be removed from git repository but not removed locally 
*/
std::optional<std::string> GtaGitController::removeFileFromIndex(const std::string& path) {
	git_index* idx = NULL;
	int errorCode = git_repository_index(&idx, _repo);
	if (errorCode) {
		return GetError(errorCode, "Get repo index");
	}

	errorCode = git_index_remove_bypath(idx, path.c_str());
	if (errorCode) {
		return GetError(errorCode, "Remove file from index");
	}

	git_index_write(idx);
	git_index_free(idx);

	return std::nullopt;
}