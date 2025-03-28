#include "GTAControllerLauncherWidget.h"
#include "ui_GTAControllerLauncherWidget.h"

#include <QFile>
#include <qdom.h>
#include "GTAUtil.h"
#include <QFileDialog>
#include <qsettings.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <GTASystemServices.h>
#include <QTime>

GTAControllerLauncherWidget::GTAControllerLauncherWidget(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::GTAControllerLauncherWidgetClass)
{
	ui->setupUi(this);
	QStringList fileHeaders = { "File Name", "File Path", "Max Time Estimated", "Full Execution Time"};

	ui->fileTableWidget->setColumnCount(4);
	ui->fileTableWidget->setHorizontalHeaderLabels(fileHeaders);

	const int width = this->width();
	const int fileNameColumnWidth = (width * 25) / 100;
	const int filePathColumnWidth = (width * 40) / 100;
	const int timeColumnWidth = (width * 15) / 100;

	ui->fileTableWidget->setColumnWidth(0, fileNameColumnWidth);
	ui->fileTableWidget->setColumnWidth(1, filePathColumnWidth);
	ui->fileTableWidget->setColumnWidth(2, timeColumnWidth);
	ui->fileTableWidget->setColumnWidth(3, timeColumnWidth);

	ui->fileTableWidget->setRowCount(0);

	ui->fileTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->fileTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

	connect(ui->upButton, SIGNAL(clicked()), this, SLOT(onUpButtonClicked()));
	connect(ui->downButton, SIGNAL(clicked()), this, SLOT(onDownButtonClicked()));
	connect(ui->removeFileButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
	connect(ui->addFileButton, SIGNAL(clicked()), this, SLOT(onNewFileAddButtonClicked()));
	connect(ui->launchButton, SIGNAL(clicked()), this, SLOT(onLaunchButtonClicked()));
	connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->appStartUpDelayEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onStartupDelayTextChanged(const QString&)));

	ui->launchButton->setEnabled(false);
	ui->appStartUpDelayEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]*"), (ui->appStartUpDelayEdit)));
	ui->appStartUpDelayEdit->setText("25");
	_waitBeforeEmotest = 0;
	_delayBetweenExecution = 0;
}

GTAControllerLauncherWidget::~GTAControllerLauncherWidget()
{}

/**
* @brief Add File to the execution list for GTAController procedure dashboard Launcher
* @param fullFilePath : Full path of the file to be added
*/
void GTAControllerLauncherWidget::AddNewFileToExecutionList(const QString fullFilePath)
{
	const int index = fullFilePath.lastIndexOf("/");
	const QString fileName = fullFilePath.mid(index + 1, fullFilePath.length() - index);

	FileInfo fileInfo;
	fileInfo.fileName = fileName;
	fileInfo.filePath = fullFilePath;

	_executionFileList.push_back(fileInfo);
	UpdateTableRows();
}

/**
* @brief : Updates the table rows after any update/add/delete operation performed from dialog
*/
void GTAControllerLauncherWidget::UpdateTableRows()
{
	int totalMaxEstimatedTime = 0;
	bool showNoData = false;
	ui->fileTableWidget->setRowCount(0);
	foreach(FileInfo item, _executionFileList)
	{
		QString maxExecutionTime = getMaxEstimatedTime(item.filePath);

		if (maxExecutionTime.endsWith(XNODE_MAX_TIME_ESTIMATED_UNIT))
		{
			maxExecutionTime = maxExecutionTime.left(maxExecutionTime.length() - 1);
		}

		bool isValidInteger;
		int maxExecutionTimeDigit = maxExecutionTime.toInt(&isValidInteger);

		if (maxExecutionTime.isEmpty() || !isValidInteger || maxExecutionTime == "0")
		{
			maxExecutionTime = "No Data";
			showNoData = true;
			isValidInteger = false;
		}
		else
		{
			maxExecutionTime += XNODE_MAX_TIME_ESTIMATED_UNIT;
		}

		ui->fileTableWidget->setRowCount(ui->fileTableWidget->rowCount() + 1);

		const int currentRow = ui->fileTableWidget->rowCount() - 1;

		ui->fileTableWidget->setItem(currentRow, 0, new QTableWidgetItem(item.fileName));
		ui->fileTableWidget->setItem(currentRow, 1, new QTableWidgetItem(item.filePath));
		ui->fileTableWidget->setItem(currentRow, 2, new QTableWidgetItem(maxExecutionTime));

		if (isValidInteger)
		{
			const int maxEstimatedTime = maxExecutionTimeDigit + _waitBeforeEmotest + _appStartupDelay + _delayBetweenExecution;
			ui->fileTableWidget->setItem(currentRow, 3, new QTableWidgetItem(QString::number(maxEstimatedTime) + XNODE_MAX_TIME_ESTIMATED_UNIT));
			totalMaxEstimatedTime += maxEstimatedTime;
		}
		else
		{
			ui->fileTableWidget->setItem(currentRow, 3, new QTableWidgetItem("No Data"));
			totalMaxEstimatedTime = 0;
		}
		
		ui->fileTableWidget->item(currentRow, 2)->setTextAlignment(Qt::AlignCenter);
		ui->fileTableWidget->item(currentRow, 3)->setTextAlignment(Qt::AlignCenter);
	}

	ui->launchButton->setEnabled(_executionFileList.size() > 0);

	if (!showNoData)
	{
		QTime baseTime(0, 0, 0), estimateTime;
		estimateTime = baseTime.addSecs(totalMaxEstimatedTime);
		QString totalTime = estimateTime.toString("hh:mm:ss");
		ui->totalMaxEstimatedTime->setText(totalTime);
	}
	else
	{
		ui->totalMaxEstimatedTime->setText("No Data");
	}

	QFont font = ui->totalMaxEstimatedTime->font();
	QFontMetrics fontMetrics(font);
	QSize textSize = fontMetrics.size(0, ui->totalMaxEstimatedTime->text());
	int boxWidth = textSize.width() + 10;
	ui->totalMaxEstimatedTime->setMaximumWidth(boxWidth);
}

/**
* @brief : Gets the Maximum estimated time to run a procedure
* @param : iDocFileFullPath : Full path of the file for which Max estimated time is needed
* @return : value of maximum estimated time in the file xml node.
*/
QString GTAControllerLauncherWidget::getMaxEstimatedTime(QString iDocFileFullPath)
{
	QFile xmlFileObj(iDocFileFullPath);
	QString oMaxTimeEstimated;
	bool rc = xmlFileObj.open(QFile::Text | QFile::ReadOnly);
	if (rc)
	{
		QDomDocument DomDoc;
		rc = DomDoc.setContent(&xmlFileObj);
		if (rc)
		{
			QDomElement rootElement = DomDoc.documentElement();
			oMaxTimeEstimated = rootElement.attribute(XNODE_MAX_TIME_ESTIMATED);
		}
		DomDoc.clear();
		xmlFileObj.close();
	}

	return oMaxTimeEstimated;
}

/**
* @brief : Create the configuration file that will be used while launching the GTAController.
* @param : gtaControllerConfigFilePath : Path at which the configuration file will be created.
*/
void GTAControllerLauncherWidget::createGTAControllerConfigFile(QString gtaControllerConfigFilePath)
{
	QString inputFiles;
	foreach(FileInfo file, _executionFileList)
	{
		const QString delimiter(",");
		inputFiles += file.filePath + delimiter;
	}
	TestConfig* testConf = TestConfig::getInstance();
	QString applicationFilePath = QCoreApplication::applicationFilePath();
	QString applicationFileDirectory = QCoreApplication::applicationDirPath();
	QString gtaControllerOutputDir = QString::fromStdString(testConf->getGTAControllerOutputLogPath());
	QString gtaControllerExe = GTASystemServices::getSystemServices()->getGTAControllerFilePath();

	const int index = gtaControllerExe.lastIndexOf("/");
	const QString gtaControllerPath = gtaControllerExe.mid(0, index);
	const QString defaultGTAControllerConfigFile = gtaControllerPath + "/FullAutomationManager.ini";
	QString autoLauncherDir = GTASystemServices::getSystemServices()->getAutoLauncherWatchDirectory();
	QString autoLauncherConfigPath = GTASystemServices::getSystemServices()->getAutoLauncherConfigPath();

	QFileInfo info(defaultGTAControllerConfigFile);
	if (info.exists())
	{
		QSettings defaultsettings(defaultGTAControllerConfigFile, QSettings::IniFormat);
		QSettings settings(gtaControllerConfigFilePath, QSettings::IniFormat);

		settings.clear();
		settings.sync();

		const QStringList childGroups = defaultsettings.childGroups();

		foreach(const QString& childGroup, childGroups)
		{
			defaultsettings.beginGroup(childGroup);
			settings.beginGroup(childGroup);
			const QStringList childKeys = defaultsettings.childKeys();

			foreach(const QString & childKey, childKeys)
			{
				settings.setValue(childKey, defaultsettings.value(childKey).toString());
			}

			if (childGroup.toUpper() == "INPUT_DATA")
			{
				settings.setValue("GTA_EDITOR_PATH", applicationFilePath);
				settings.setValue("INPUT_FILES", inputFiles);
				settings.setValue("GTA_AUTOLAUNCH_CONFIG_FILE", autoLauncherConfigPath);
				settings.setValue("STARTUP_APPLICATION_DELAY", _appStartupDelay);		
			}
			else if (childGroup.toUpper() == "OUTPUT_DATA")
			{
				settings.setValue("CONVERSION_OUTPUT_DIR", gtaControllerOutputDir);
				settings.setValue("AUTOLAUNCH_TOOL_REPOSITORY", autoLauncherDir);
			}

			defaultsettings.endGroup();
			settings.endGroup();
		}

		settings.sync();
	}
	else
	{
		QSettings settings(gtaControllerConfigFilePath, QSettings::IniFormat);

		settings.clear();
		settings.sync();

		settings.beginGroup("INPUT_DATA");
		settings.setValue("GTA_EDITOR_PATH", applicationFilePath);
		settings.setValue("CSV_DEFAULT_DIR", "");
		settings.setValue("INPUT_FILES", inputFiles);
		settings.setValue("GTA_AUTOLAUNCH_CONFIG_FILE", autoLauncherConfigPath);
		settings.setValue("STARTUP_APPLICATION_DELAY", _appStartupDelay);
		settings.endGroup();

		settings.beginGroup("OUTPUT_DATA");
		settings.setValue("CONVERSION_OUTPUT_DIR", gtaControllerOutputDir);
		settings.setValue("AUTOLAUNCH_TOOL_REPOSITORY", autoLauncherDir);
		settings.endGroup();

		settings.beginGroup("AUTOLAUNCHER_ABORTION");
		settings.setValue("CONFIDENCE_LVL", "");
		settings.endGroup();
		QString gtaControllerConfigFilePath = applicationFileDirectory + "/FullAutomationManager.ini";

		settings.sync();
	}
}

/**
* @brief : Sets the data directory path for GTAEditor to be used as default path 
		   when opening file dialog to add new file.
* @param : dataDirectoryPath : Full path of the data directory
*/
void GTAControllerLauncherWidget::setDataDirectoryPath(QString dataDirectoryPath)
{
	_dataDirectoryPath = dataDirectoryPath;
}

/**
* @brief : Moves the current selected one row up in the table list.
*/
void GTAControllerLauncherWidget::onUpButtonClicked()
{
	QItemSelectionModel* selectModel = ui->fileTableWidget->selectionModel();

	if (selectModel->hasSelection())
	{
		QModelIndexList list = selectModel->selectedRows();
		int row = list[0].row();

		if (row > 0)
		{
			FileInfo data = _executionFileList.at(row);
			_executionFileList[row] = _executionFileList[row - 1];
			_executionFileList[row - 1] = data;
			UpdateTableRows();

			ui->fileTableWidget->setCurrentIndex(ui->fileTableWidget->model()->index(row - 1, 0));
		}
	}
}

/**
* @brief : Moves the current selected one row down in the table list.
*/
void GTAControllerLauncherWidget::onDownButtonClicked()
{
	QItemSelectionModel* selectModel = ui->fileTableWidget->selectionModel();

	if (selectModel->hasSelection())
	{
		QModelIndexList list = selectModel->selectedRows();
		int row = list[0].row();

		if (row < ui->fileTableWidget->rowCount() - 1)
		{
			FileInfo data = _executionFileList.at(row);
			_executionFileList[row] = _executionFileList[row + 1];
			_executionFileList[row + 1] = data;
			UpdateTableRows();

			ui->fileTableWidget->setCurrentIndex(ui->fileTableWidget->model()->index(row + 1, 0));
		}
	}
}

/**
* @brief : removes the current selected row from the execution list.
*/
void GTAControllerLauncherWidget::onRemoveButtonClicked()
{
	QItemSelectionModel* selectModel = ui->fileTableWidget->selectionModel();

	if (selectModel->hasSelection())
	{
		QModelIndexList list = selectModel->selectedRows();
		std::sort(list.begin(), list.end(), [](QModelIndex first, QModelIndex second) {
			return first.row() > second.row();
			});

		foreach(QModelIndex indx, list)
		{
			int row = indx.row();
			int rowCount = ui->fileTableWidget->rowCount();
			if (rowCount > 0 && row <= rowCount)
			{
				_executionFileList.erase(_executionFileList.begin() + row);
				ui->fileTableWidget->setRowCount(rowCount - 1);
			}
		}

		UpdateTableRows();
	}
}

/**
* @brief : Opens a file selection dialog to add a new file in execution list
*/
void GTAControllerLauncherWidget::onNewFileAddButtonClicked()
{
	QStringList sFilePathList = QFileDialog::getOpenFileNames(this, ("AVC File Browser"),
		_dataDirectoryPath, tr("(*.pro)"));

	foreach(QString file, sFilePathList)
	{
		AddNewFileToExecutionList(file);
	}
}

/**
* @brief : Launches the GTAController procedure dashboard dialog as an independent process.
*/
void GTAControllerLauncherWidget::onLaunchButtonClicked()
{
	bool isAlreadyRunning = isRunning("Gta-Controller.exe");

	if (isAlreadyRunning)
	{
		QString errorMsg = "Gta-Controller.exe is already running.\nBefore to launch a new instance, the current running one should be closed.";
		QMessageBox::critical(this, "Error", QString(errorMsg));
		
		return;
	}

	QString gtaControllerExe = GTASystemServices::getSystemServices()->getGTAControllerFilePath();
	QString autoLauncherDir = GTASystemServices::getSystemServices()->getAutoLauncherWatchDirectory();
	QString autoLauncherConfigPath = GTASystemServices::getSystemServices()->getAutoLauncherConfigPath();

	QFile fileGtaController(gtaControllerExe);
	QFile fileAutoLauncher(autoLauncherDir);
	QFile fileAutoLauncherConfig(autoLauncherConfigPath);

	QString pathName;
	if (!fileGtaController.exists())
	{
		pathName = "GTAController exe";
	}
	else if (!fileAutoLauncher.exists())
	{
		pathName = "AutoLauncher Watch Folder";
	}
	else if (!fileAutoLauncherConfig.exists())
	{
		pathName = "AutoLauncher Config File";
	}

	if (!pathName.isEmpty())
	{
		QString errorMsg = pathName + " path for the procedure dashboard launcher is not correctly set in configuration setting.";
		QMessageBox::critical(this, "Error", QString(errorMsg));

		return;
	}

	QString applicationFileDirectory = QCoreApplication::applicationDirPath();
	QString gtaControllerConfigFilePath = applicationFileDirectory + "/FullAutomationManager.ini";

	createGTAControllerConfigFile(gtaControllerConfigFilePath);

	QApplication::setOverrideCursor(Qt::WaitCursor);

	QStringList argument;
	argument << "-f" << "-c=" + gtaControllerConfigFilePath << "-hi";

	QProcess pProcess;
	pProcess.startDetached(gtaControllerExe, argument);

	QApplication::restoreOverrideCursor();
	this->close();
}

/**
* @brief : Text changed event for app startup delay text box value changed
*/
void GTAControllerLauncherWidget::onStartupDelayTextChanged(const QString&)
{
	bool ok = false;
	QString strValue = ui->appStartUpDelayEdit->text();
	int num = strValue.toInt(&ok);
	if (ok)
	{	
		_appStartupDelay = num;
	}
	else
	{
		_appStartupDelay = 0;
	}

	UpdateTableRows();
}

/**
* @brief : Checks if the process is already running.
* @param : process - name of the process whose running instance needs to be checked.
* @return : True if instance running in background otherwise False
*/
bool GTAControllerLauncherWidget::isRunning(const char* process)
{
	QProcess tasklist;
	tasklist.start("tasklist");
	tasklist.waitForFinished();

	return tasklist.readAllStandardOutput().contains(process);
}

/**
* @brief : Initialise the values of text box for additional time delays required for launching
* the procedure dashboard
*/
void GTAControllerLauncherWidget::InitialiseAdditionalTimeValues()
{
	QString autoLauncherConfigPath = GTASystemServices::getSystemServices()->getAutoLauncherConfigPath();
	QFileInfo info(autoLauncherConfigPath);
	if (info.exists())
	{
		QSettings defaultsettings(autoLauncherConfigPath, QSettings::IniFormat);
		const QStringList childGroups = defaultsettings.childGroups();
		const QStringList childKeys = defaultsettings.childKeys();

		bool optionWaitTimeForEmoTest = defaultsettings.value("Option_WaitTimeBeforeEmotest").toBool();
		bool OptionDelayBetweenExecution = defaultsettings.value("Option_DelayBetweenExecution").toBool();

		if (optionWaitTimeForEmoTest)
		{
			_waitBeforeEmotest = defaultsettings.value("Value_WaitTimeBeforeEmotest").toInt();
		}

		if (OptionDelayBetweenExecution)
		{
			_delayBetweenExecution = defaultsettings.value("Value_DelayBetweenExecution").toInt();
		}

		ui->waitBeforeEmoTestLE->setText(QString::number(_waitBeforeEmotest));
		ui->delayBetweenExecutionLE->setText(QString::number(_delayBetweenExecution));
	}
}