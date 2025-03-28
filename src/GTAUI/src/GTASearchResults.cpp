#include "GTASearchResults.h"
#include "ui_GTASearchResults.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QUrl>
#include <qdesktopservices.h>

#define COL_1 0
#define COL_2 1
#define COL_3 2
#define COL_CNT 3


GTASearchResults::GTASearchResults(GTAWidget *parent) :
     GTAWidget(parent),
    ui(new Ui::GTASearchResults)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(COL_CNT);
    ui->treeWidget->setColumnWidth(COL_1,500);
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeItemDoubleClicked(QTreeWidgetItem*,int)));
    //    connect(ui->treeWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onTreeItemDoubleClicked(QModelIndex)));


    QShortcut *clearShortCut = new QShortcut(QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_Delete),this);
    clearShortCut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(clearShortCut, SIGNAL(activated()),this, SLOT(onClearSearchWindow()));

    connect(ui->treeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayContextMenu(QPoint)));

    _pContextMenu = NULL;

    createContextMenu();
}

GTASearchResults::~GTASearchResults()
{
    if(_pContextMenu != NULL)
    {
        delete _pContextMenu;
        _pContextMenu = NULL;
    }
    delete ui;
}

void GTASearchResults::show()
{
    ui->treeWidget->setVisible(true);
    QWidget::show();
}


void GTASearchResults::closeEvent(QCloseEvent *event)
{
    onClearSearchWindow();
    event->accept();
    this->QWidget::closeEvent(event);
    emit widgetClosed();
}

void GTASearchResults::onClearSearchWindow()
{
    ui->treeWidget->clear();
}

/**
 * @brief Exports the contents of the tree widget to a text file.
 *
 * This function allows the user to select a filename and directory where
 * the contents of the tree widget will be exported. The export format is a plain
 * text file where each top-level item and its children are listed with their
 * respective texts. Child items are sorted by their line number before being written.
 *
 * @note If the file cannot be opened for writing or the filename is empty, the function
 *       will return early without exporting the contents.
 */
void GTASearchResults::onExportSearchWindow()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save File"),
        "",
        tr("Text Files (*.txt)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file for writing."));
        return;
    }

    QTextStream out(&file);
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* topLevelItem = ui->treeWidget->topLevelItem(i);

        // Collect all child items and sort them
        QVector<QPair<int, QString>> lines;
        for (int j = 0; j < topLevelItem->childCount(); ++j) {
            QTreeWidgetItem* childItem = topLevelItem->child(j);
            int lineNum = childItem->text(0).toInt();
            QString value = childItem->text(1);
            lines.append(qMakePair(lineNum, value));
        }

        // Sort the vector by line numbers
        std::sort(lines.begin(), lines.end(), [](const QPair<int, QString>& a, const QPair<int, QString>& b) {
            return a.first < b.first;
            });

        out << "[" << topLevelItem->text(0) << "]\n";
        out << "Occurrences = " << topLevelItem->childCount() << "\n";

        // Write the sorted lines
        for (const QPair<int, QString>& line : lines) {
            out << "Line " << line.first << " = " << line.second << "\n";
        }

        out << "\n";
    }

    file.close();
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("Export Successful"));
    msgBox.setText(tr("The file has been successfully exported."));
    msgBox.addButton(QMessageBox::Close);
    msgBox.exec();
}

/**
 * @brief Populates the tree widget with the search results.
 *
 * This function takes a hash map of search results and populates the tree widget with these results.
 * Depending on the 'isHeader' flag, it treats the results as either header search results or general
 * search results. Each entry in the hash map corresponds to a top-level item in the tree, with its children
 * representing the individual findings (tags or rows where the search string was found).
 *
 * @param searchResultMap A hash map where the key is a string representing the file path and the value is a
 *                        SearchResults object containing details of all findings within that file.
 * @param isHeader Indicates whether the search was performed within header tags. If true, the function expects
 *                 header-specific data within the SearchResults; otherwise, it expects general row data.
 *
 * @note If the searchResultMap is empty or no elements are found, the user is notified with a warning message box.
 */
void GTASearchResults::setSearchResults(const QHash<QString, SearchResults>& searchResultMap, bool isHeader)
{
    _isHeader = isHeader;
    bool elementFound = false;
    QList<QTreeWidgetItem*> itemLst;

    if (searchResultMap.size())
    {
        QStringList keys = searchResultMap.keys();
        for (const QString& key : keys)
        {
            SearchResults objSearchRes = searchResultMap.value(key);
            QString uuid = objSearchRes.uuid;

            QHash<int, QString> relevantData = isHeader ? objSearchRes.foundTags : objSearchRes.foundRows;
            int size = relevantData.size();
            if (size > 0)
            {
                QVector<QPair<int, QTreeWidgetItem*>> sortableList;
                QString sSize = QString::number(size);
                QTreeWidgetItem* pParentItem = new QTreeWidgetItem(ui->treeWidget);
                pParentItem->setText(COL_1, key);
                pParentItem->setText(COL_3, uuid);
                pParentItem->setText(COL_2, QString("(%1)").arg(sSize));

                QHashIterator<int, QString> itr(relevantData);
                while (itr.hasNext())
                {
                    itr.next();
                    int index = itr.key();
                    QString content = itr.value();
                    QTreeWidgetItem* pChildItem = new QTreeWidgetItem();
                    pChildItem->setText(COL_1, QString::number(index));
                    pChildItem->setText(COL_2, content);
                    sortableList.append(qMakePair(index, pChildItem));
                }

                std::sort(sortableList.begin(), sortableList.end(), [](const QPair<int, QTreeWidgetItem*>& a, const QPair<int, QTreeWidgetItem*>& b) {
                    return a.first < b.first;
                    });

                for (auto& pair : sortableList) {
                    pParentItem->addChild(pair.second);
                }

                itemLst.append(pParentItem);
                elementFound = true;
            }
        }

        if (!itemLst.isEmpty())
        {
            ui->treeWidget->addTopLevelItems(itemLst);
            ui->treeWidget->hideColumn(COL_3);
        }
        else if (!elementFound)
        {
            QMessageBox::warning(this, "Not found", "Search string not found", QMessageBox::Ok);
        }
    }
}

void GTASearchResults::onTreeItemDoubleClicked(QTreeWidgetItem* pItem,int)
{
    if(pItem != NULL && pItem->childCount() == 0)
    {
        QTreeWidgetItem *pParent = pItem->parent();
        if(pParent)
        {
            QString relFilePath = pParent->text(COL_1);
			QString uuid = pParent->text(COL_3);
            QString sRowNo =  pItem->text(COL_1);
            bool ok;
            int rowNo = sRowNo.toInt(&ok);
            if(ok && !_isHeader)
            {
                emit jumpToElement(uuid,rowNo,GTAAppController::SEARCH_REPLACE,true);
            }
            else if (ok && _isHeader)
            {
                emit jumpToElementHeader(uuid, rowNo, GTAAppController::SEARCH_REPLACE, true);
            }
        }

    }
}

void GTASearchResults::displayContextMenu(const QPoint &iPos)
{
    QWidget * pSourceWidget = (QWidget*)sender();
//    if(ui->treeWidget->is)
//    {
        if(pSourceWidget != NULL && _pContextMenu != NULL && pSourceWidget == ui->treeWidget)
        {
            _pContextMenu->exec(ui->treeWidget->viewport()->mapToGlobal(iPos));
        }
//    }
}

void GTASearchResults::createContextMenu()
{
    _pContextMenu = new QMenu(this);

    _pActionClearWindow = new QAction("Clear All",this);
    _pActionExportResult = new QAction("Export Result", this);
    connect(_pActionClearWindow,SIGNAL(triggered()),this,SLOT(onClearSearchWindow()));
    connect(_pActionExportResult, SIGNAL(triggered()), this, SLOT(onExportSearchWindow()));
    _pContextMenu->addAction(_pActionClearWindow);
    _pContextMenu->addAction(_pActionExportResult);
}


