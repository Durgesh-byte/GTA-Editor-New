#pragma once

#include <QDialog>
#include <vector>

namespace Ui {
	class GTAControllerLauncherWidgetClass;
}

struct FileInfo
{
	QString fileName;
	QString filePath;
	QString estimatedTime;
};

class GTAControllerLauncherWidget : public QDialog
{
	Q_OBJECT

public:
	GTAControllerLauncherWidget(QWidget *parent = nullptr);
	~GTAControllerLauncherWidget();
	void AddNewFileToExecutionList(const QString fullFilePath);
	void setDataDirectoryPath(QString dataDirectoryPath);
	void UpdateTableRows();
	void InitialiseAdditionalTimeValues();

private:
	Ui::GTAControllerLauncherWidgetClass* ui;
	std::vector<FileInfo> _executionFileList;
	QString _dataDirectoryPath;
	int _appStartupDelay;
	int _waitBeforeEmotest;
	int _delayBetweenExecution;

	QString getMaxEstimatedTime(QString iDocFileFullPath);
	void createGTAControllerConfigFile(QString gtaControllerIniFilePath);
	bool isRunning(const char* process);

private slots:
	void onUpButtonClicked();
	void onDownButtonClicked();
	void onRemoveButtonClicked();
	void onNewFileAddButtonClicked();
	void onLaunchButtonClicked();
	void onStartupDelayTextChanged(const QString&);
};
