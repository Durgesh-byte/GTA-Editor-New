#include "GTAElementDependenciesWidget.h"
#include "ui_GTAElementDependenciesWidget.h"
#include "GTAGitController.h"

/**
 * @brief Constructor
 * @param fileName the selected file
 * @param appController GTA application controller
 * @param parent 
*/
GTAElementDependenciesWidget::GTAElementDependenciesWidget(QString fileName, GTAAppController* appController, QWidget *parent) :
    _fileName(fileName),
    _pAppController(appController),
    _pGTADataDirectory(appController->getGTADataDirectory()),
    _pGTATableDirectory(appController->getGTATableDirectory()),
    QWidget(parent),
    ui(new Ui::GTAElementDependenciesWidget)
{
    setWindowFlags(Qt::Window);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    _elementType["obj"] = ELEM_OBJ;
    _elementType["fun"] = ELEM_FUNC;
    _elementType["pro"] = ELEM_PROC;
    _elementType["seq"] = ELEM_SEQ;
        
    displayName();
    displayUsedDocTree();
    displayUsedTool();
    displayUsedCsv();  

    ui->exportBtn->setToolTip("Export the list of dependencies into CSV");
    connect(ui->exportBtn, SIGNAL(clicked()), this, SLOT(onExportClicked()));
 }

/**
 * @brief Destructor
*/
GTAElementDependenciesWidget::~GTAElementDependenciesWidget()
{
    delete ui;
}


//============================================================================================== DISPLAY FUNCTIONS 

/**
 * @brief Display the name of selected element which dependencies are shown
*/
void GTAElementDependenciesWidget::displayName()
{
    ui->elementNameLE ->setText(_fileName);
}

/**
 * @brief  Display the list of documents used by the selected element
*/
void GTAElementDependenciesWidget::displayUsedDocTree()
{
    std::vector<FileInfo> parentDocList = getUsedDoc(_fileName);
    _allUsedDoc = parentDocList;
    ui->contentDoc->hideColumn(1);
    if (parentDocList.size() == 0)
    {
        ui->contentDoc->setDisabled(true);
        ui->titleDoc->setText(ui->titleDoc->text().replace("(X)", "(" + QString::number(_allUsedDoc.size()) + ")"));
        return;
    }
    for (FileInfo pDoc : parentDocList)
    {
        QTreeWidgetItem* pItem = addRoot(pDoc.fileName);
     
        if (pDoc.isIgnored)
        {
            QFont itemFont = pItem->font(0);
            itemFont.setStrikeOut(true);
            pItem->setFont(0, itemFont);
        }
            
        insertChildren(pItem, pDoc.fileName, pDoc.isIgnored);
    }

    int usedFilesCount = 0, ignoredFilesCount = 0;
    getUsedAndIgnoredFileCount(usedFilesCount, ignoredFilesCount);

    QString titleText = ui->titleDoc->text().replace("(X)", "(" + QString::number(usedFilesCount) + ")");
    titleText.replace("(Y)", "(" + QString::number(ignoredFilesCount) + ")");
    
    ui->titleDoc->setText(titleText);
    ui->contentDoc->sortByColumn(1);
    ui->contentDoc->resizeColumnToContents(0);
    ui->contentDoc->resizeColumnToContents(1);
}

/**
 * @brief Display the list of external tools used by the selected element
*/
void GTAElementDependenciesWidget::displayUsedTool()
{
    int nbTool;
    QStringList list = getUsedTool(nbTool);
    ui->titleTool->setText(ui->titleTool->text().replace("(X)", "(" + QString::number(nbTool) + ")"));
    if (list.isEmpty())
    {
        ui->contentTool->setDisabled(true);
        return;
    }    
    for (int i = 0; i < list.count(); i++)
    {
        QStringList newList  = list[i].split("|");
        QString toolName     = newList[0];
        QString toolFunction = newList[1];
        QString toolUuid     = newList[2];
        QFileInfo f(_pAppController->getAbsoluteFilePathFromUUID(toolUuid));
        QString relPath = f.filePath().remove(_pGTADataDirectory);
        QTableWidgetItem* name = new QTableWidgetItem(toolName + "\t");
        QTableWidgetItem* function = new QTableWidgetItem(toolFunction + "\t");
        QTableWidgetItem* usedIn = new QTableWidgetItem(iconOf(relPath), relPath);
        usedIn->setTextColor(colorOf(relPath));

        ui->contentTool->insertRow(i);
        ui->contentTool->setItem(i, 0, name);
        ui->contentTool->setItem(i, 1, function);
        ui->contentTool->setItem(i, 2, usedIn);

        _allUsedTool.append(toolName + sep + toolFunction + sep + relPath);
        
        for (int c = 0; c < ui->contentTool->columnCount(); c++)
        {
            ui->contentTool->item(i, c)->setFlags(ui->contentTool->item(i, c)->flags() & ~Qt::ItemIsEditable);
        }            
    }
    ui->contentTool->sortByColumn(0);
    ui->contentTool->resizeColumnsToContents();
    ui->contentTool->resizeRowsToContents();
}

/**
 * @brief Display the list of CSV files used by the selected element
*/
void GTAElementDependenciesWidget::displayUsedCsv()
{
    int nbCsv;
    QStringList list = getUsedCsv(nbCsv);
    ui->titleCsv->setText(ui->titleCsv->text().replace("(X)", "(" + QString::number(nbCsv) + ")"));
    if (list.isEmpty())
    {
        ui->contentCsv->setDisabled(true);
        return;
    }    
    for (int i = 0; i < list.count(); i++)
    {
        QStringList newList = list[i].split("|");
        QString csvName     = newList[0];
        QString csvCommand  = newList[1];
        QString csvUsedIn   = newList[2];
        QTableWidgetItem* name = new QTableWidgetItem(csvName + "\t");
        QTableWidgetItem* cmd = new QTableWidgetItem(csvCommand + "\t");
        QTableWidgetItem* usedIn = new QTableWidgetItem(iconOf(csvUsedIn), csvUsedIn);
        usedIn->setTextColor(colorOf(csvUsedIn));

        ui->contentCsv->insertRow(i);
        ui->contentCsv->setItem(i, 0, name);
        ui->contentCsv->setItem(i, 1, cmd);
        ui->contentCsv->setItem(i, 2, usedIn);

        _allUsedCsv.append(csvName + sep + csvCommand + sep + csvUsedIn);
        for (int c = 0; c < ui->contentCsv->columnCount(); c++)
        {
            ui->contentCsv->item(i, c)->setFlags(ui->contentCsv->item(i, c)->flags() & ~Qt::ItemIsEditable);
        }
    }
    ui->contentCsv->sortByColumn(0);
    ui->contentCsv->resizeColumnsToContents();
    ui->contentCsv->resizeRowsToContents();
}


//================================================================================================== GET FUNCTIONS 

/**
 * @brief Get the list of called document inside of a given file
 * @param file the given file path
 * @return list 
*/
std::vector<GTAElementDependenciesWidget::FileInfo> GTAElementDependenciesWidget::getUsedDoc(QString file)
{
    std::vector<FileInfo> usedDocs;
    GTAElement* pElement = NULL;
    _pAppController->getElementFromDocument(file, pElement, false);
    if (pElement)
    {
        QList<GTACommandBase*> allCmds = pElement->GetAllCommands();
        for (auto* cmd : allCmds)
        {
            GTAActionCall* call = dynamic_cast<GTAActionCall*>(cmd);
            if (call != NULL)
            {
                QString uuid = call->getRefrenceFileUUID();
                QString elementName = _pAppController->getAbsoluteFilePathFromUUID(uuid);
                elementName.replace(_pAppController->getGTADataDirectory(), "").trimmed();

                FileInfo file;
                file.fileName = elementName;
                if (((GTACommandBase*)cmd)->isIgnoredInSCXML())
                {
                    file.isIgnored = true;
                }

                QString fileName = file.fileName;
                bool isIgnored = file.isIgnored;
				auto it = std::find_if(usedDocs.begin(), usedDocs.end(),
					[&fileName, &isIgnored](const FileInfo& obj)
					{
						return (obj.fileName == fileName && obj.isIgnored == isIgnored);
					});

                if (it == usedDocs.end())
                {
					if (!elementName.isEmpty())
					{
						usedDocs.push_back(file);
					}
					else
					{
                        file.fileName = call->getElement();
						usedDocs.push_back(file);
					}
                }
            }   
        }
    }
    return usedDocs;
}

/**
 * @brief Get the list of external tools used by the selected element
 * @param nbTool number of detected tools
 * @return list
*/
QStringList GTAElementDependenciesWidget::getUsedTool(int& nbTool)
{
    QStringList usedTool, usedDoc;
    QStringList detectedTools;
    usedDoc.append(_fileName);
    _pAppController->getDocumentsUsed(_fileName, usedDoc, QStringList());
    foreach(QString doc, usedDoc)
    {
        GTAElement* pElement = NULL;
        _pAppController->getElementFromDocument(doc, pElement, false);
        if (pElement)
        {
            QList<GTACommandBase*> allCmds = pElement->GetAllCommands();
            for (GTACommandBase* cmd : allCmds)
            {
                GTAGenericToolCommand* tool = dynamic_cast<GTAGenericToolCommand*>(cmd);
                if (tool != NULL) {
                    QString toolName = tool->getToolDisplayName();
                    QString toolFunction = tool->getCommand().at(0).getDisplayName();
                    QString toolUuid = pElement->getUuid();
                    QString toolInfo = toolName + "|" + toolFunction + "|" + toolUuid;
                    usedTool.append(toolInfo);

                    if (!detectedTools.contains(toolName))
                        detectedTools.append(toolName);
                }
            }
        }     
    }
    nbTool = detectedTools.count();
    usedTool.removeDuplicates();
    return usedTool;
}

/**
 * @brief  Get the list of CSV files used by the selected element
 * @param nbCsv number of detected tools
 * @return list
*/
QStringList GTAElementDependenciesWidget::getUsedCsv(int& nbCsv)
{
    QStringList usedCsv;
    QStringList detectedCsvs;
    QStringList docsToCheck;
    
    for each (FileInfo file in _allUsedDoc)
    {
        docsToCheck.append(file.fileName);
    }

    docsToCheck.append(_fileName);
     
    for (QString doc : docsToCheck)
    {
        GTAElement* pElement = NULL;
        _pAppController->getElementFromDocument(doc, pElement, false);
        if (pElement)
        {
            QList<GTACommandBase*> allCmds = pElement->GetAllCommands();
            for (auto cmd : allCmds)
            {
                QString csv, cmdType;
                GTAActionPrintTable* printTable = dynamic_cast<GTAActionPrintTable*>(cmd);
                if (printTable)
                {                
                    csv = printTable->getTableName().remove(_pGTATableDirectory);
                }
                GTAActionForEach* forEachCmd = dynamic_cast<GTAActionForEach*>(cmd);
                if (forEachCmd)
                {
                    csv = forEachCmd->getRelativePathChkB() ? forEachCmd->getRelativePath().remove("/TABLES/") : forEachCmd->getPath().remove(_pGTATableDirectory);
                }
                if (csv.endsWith(".csv"))
                {
                    GTAActionBase* actionBase = dynamic_cast<GTAActionBase*>(cmd);
                    if (actionBase)
                    {
                        QString action = actionBase->getAction();
                        QString complement = actionBase->getComplement();
                        if (csv.startsWith('/'))
                            csv = csv.mid(1);
                        usedCsv.append(csv + "|" + action + " " + complement + "|" + doc);

                        if (!detectedCsvs.contains(csv))
                            detectedCsvs.append(csv);
                    }
                }
            }
        }
    }   
    nbCsv = detectedCsvs.count();
    usedCsv.removeDuplicates();
    return usedCsv;
}


//================================================================================================= TREE FUNCTIONS 

/**
 * @brief Add root item in dedicated treewidget
 * @param path path of the item to be added
 * @return QTreeWidgetItem* root item
*/
QTreeWidgetItem* GTAElementDependenciesWidget::addRoot(QString path)
{
    QString type = path.split('.').last();
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->contentDoc);
    item->setIcon(0, iconOf(path));
    item->setTextColor(0, colorOf(path));
    item->setText(0, path);
    item->setText(1, _elementType[type]);
    item->setExpanded(true);
    ui->contentDoc->addTopLevelItem(item);
    return item;
}

/**
 * @brief Add children item in dedicated treewidget
 * @param parent parent item of the children item to be added
 * @param path path of the item to be added
 * @return QTreeWidgetItem* child item
*/
QTreeWidgetItem* GTAElementDependenciesWidget::addChild(QTreeWidgetItem* parent, QString path)
{
    QString type = path.split('.').last();
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setIcon(0, iconOf(path));
    item->setTextColor(0, colorOf(path));
    item->setText(0, path);
    item->setText(1, _elementType[type]);
    item->setExpanded(true);
    parent->addChild(item);
    return item;
}

/**
 * @brief Insert children of a given item
 * @param item treeitem of the given item where to insert the children
 * @param file filepath of the given item
*/
void GTAElementDependenciesWidget::insertChildren(QTreeWidgetItem* item, QString file, bool isIgnored)
{
    std::vector<FileInfo> docList = getUsedDoc(file);
    for (FileInfo doc : docList)
    {
        QString filename = doc.fileName;
        doc.isIgnored = isIgnored;

        auto it = std::find_if(_allUsedDoc.begin(), _allUsedDoc.end(),
            [&filename, &isIgnored](const FileInfo& obj)
            {
                return (obj.fileName == filename && obj.isIgnored == isIgnored);
            });
   
        if (it == _allUsedDoc.end())
        {
            _allUsedDoc.push_back(doc);
        }

        QTreeWidgetItem* itemChild = addChild(item, doc.fileName);
        if (isIgnored)
        {
            QFont itemFont = itemChild->font(0);
            itemFont.setStrikeOut(true);
            itemChild->setFont(0, itemFont);
        }
        insertChildren(itemChild, doc.fileName, isIgnored);
    }
}


//================================================================================================= OTHER FUNCTIONS 

/**
 * @brief Check if a file exists
 * @param file selected file
 * @return bool
*/
bool GTAElementDependenciesWidget::isValid(QString file)
{    
    QString fullPath = _pGTADataDirectory + file;
    QFile f(fullPath);   
    return f.exists();
}

/**
 * @brief Return the appropriate icon of a given file
 * @param file the given file
 * @return QIcon
*/
QIcon GTAElementDependenciesWidget::iconOf(QString file)
{   
    QIcon icon;
    auto gitController = GtaGitController::getGtaGitController();
    const QString repoDirectory = QString().fromStdString(gitController->getRepoPath());
	if (!repoDirectory.isEmpty())
	{
		const QString gtaDataDirectory = _pAppController->getGTADataDirectory();
		QString fullFilePath = gtaDataDirectory + file;
		fullFilePath.replace(repoDirectory + "/", "");
		auto status = gitController->CheckFileStatus(fullFilePath.toStdString());

        icon = assignIcon(status);
	}
    else
    { 
        QString type = file.split('.').last();
        QString state = isValid(file) ? "Normal" : "Deleted";
        icon = QIcon(":/images/forms/img/svnIconsGTAFlat/" + state + "Icon_" + type + ".png");
    }

    return icon;
}

/**
 * @brief Return the appropriate icon of a given file
 * @param file the given file
 * @return QColor
*/
QColor GTAElementDependenciesWidget::colorOf(QString file)
{
    return isValid(file) ? Qt::black : Qt::red;
}

/**
 * @brief Return the appropriate icon for given Git status
 * @param Git status of the file
 * @return QPixel
*/
QPixmap GTAElementDependenciesWidget::assignIcon(GitStatus status) const {

    switch (status) {
    case GitStatus::Current:
        return QPixmap(":/images/forms/img/git/status_normal.png").scaled(16, 16);
    case GitStatus::IndexNew:
        return QPixmap(":/images/forms/img/git/status_added.png").scaled(16, 16);
    case GitStatus::IndexModified:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::IndexDeleted:
        return QPixmap(":/images/forms/img/git/status_deleted.png").scaled(16, 16);
    case GitStatus::IndexRenamed:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::IndexTypechange:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::WtNew:
        return QPixmap(":/images/forms/img/git/status_added.png").scaled(16, 16);
    case GitStatus::WtModified:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::WtDeleted:
        return QPixmap(":/images/forms/img/git/status_deleted.png").scaled(16, 16);
    case GitStatus::WtTypechange:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::WtRenamed:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::WtUnreadable:
        return QPixmap(":/images/forms/img/git/status_nonversioned.png").scaled(16, 16);
    case GitStatus::Ignored:
        return QPixmap(":/images/forms/img/git/status_ignored.png").scaled(16, 16);
    case GitStatus::Conflicted:
        return QPixmap(":/images/forms/img/git/status_conflicted.png").scaled(16, 16);
    case GitStatus::NotFound:
        return QPixmap(":/images/forms/img/git/status_nonversioned.png").scaled(16, 16);
    case GitStatus::Unknown:
        return QPixmap("");
    default:
        return QPixmap("");
    }
}

/**
 * @brief SLOT on Export button clicked
 * By default, the CSV output file is named with the procedure filename followed by "_dependencies"
 * Its default folder is the same as where the procedure is located but the user can select another path
 */
void GTAElementDependenciesWidget::onExportClicked()
{    
    QString exportedFile = _fileName.left(_fileName.lastIndexOf(".")) + "_dependencies";
    QString exportedFullPath = QFileDialog::getSaveFileName(this, "Export as csv file", _pGTADataDirectory + exportedFile, "csv file (*.csv)");
    QFile data(exportedFullPath);
    if (data.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream output(&data);
        int usedFilesCount = 0, ignoredFilesCount = 0;
        getUsedAndIgnoredFileCount(usedFilesCount, ignoredFilesCount);
        
        // Filename
        output << "Name;" + _fileName;
        
        // Documents
        output << "\n;";
        output << "\nUsing Documents;" + QString::number(usedFilesCount);
        output << "\nIgnored Documents;" + QString::number(ignoredFilesCount);
        output << "\nPath";
        foreach (FileInfo item, _allUsedDoc)
        {
            if (item.isIgnored)
            {
                output << "\n[Ignored] " + item.fileName;
            }
            else
            {
                output << "\n" + item.fileName;
            }
        }

        // External Tools
        output << "\n;";
        output << "\nUsing External Tools;" + QString::number(_allUsedTool.count());
        output << "\nName;Function;Used in";
        foreach(QString item, _allUsedTool)
            output << "\n" + item;

        // CSV Files
        output << "\n;";
        output << "\nUsing CSV Files;" + QString::number(_allUsedCsv.count());
        output << "\nName;Command;Used in";
        foreach(QString item, _allUsedCsv)
            output << "\n" + item;
        
        data.close();
        if (!data.exists())
            return;

        QMessageBox::information(this, "Export Dependencies", "Dependencies of " + _fileName.split("/").last() + " exported in: \n\n" + data.fileName());
    }
}

/**
 * @brief Gets the count value for used and ignore files
 * @param: usedFilesCount - Count of the used Files dependencies passed back as reference
 * @param: ignoredFilesCount - Count of ignored file dependencies passed back as reference
*/
void GTAElementDependenciesWidget::getUsedAndIgnoredFileCount(int& usedFilesCount, int& ignoredFilesCount)
{
    usedFilesCount = 0;
    ignoredFilesCount = 0;

    for each (FileInfo file in _allUsedDoc)
    {
        if (file.isIgnored)
        {
            ignoredFilesCount++;
        }
        else
        {
            usedFilesCount++;
        }
    }
}
