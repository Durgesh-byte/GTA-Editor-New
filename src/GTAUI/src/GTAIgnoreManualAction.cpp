#include "GTAIgnoreManualAction.h"
#include "ui_GTAignoremanualaction.h"

/**
 * @brief Constructor of the IgnoreManualAction widget
 * @param window Selected editor window
 * @param parent 
*/
GTAIgnoreManualAction::GTAIgnoreManualAction(GTAEditorWindow* window, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTAIgnoreManualAction)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(QPixmap(":/images/forms/img/GTAAppIcon_black.png")));
    ui->ManActTW->verticalHeader()->setVisible(false);
    ui->ManActTW->setColumnWidth(0, 50);
    ui->ManActTW->setColumnWidth(1, 400);
    ui->ManActTW->setColumnWidth(2, 100);
    connect(ui->ignoreAllCheckBox, SIGNAL(toggled(bool)), this, SLOT(onIgnoreAll(bool)));
    connect(ui->ManActTW, SIGNAL(cellChanged(int, int)), this, SLOT(onItemChanged(int, int)));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onCancelled()));

    updateTable(window);
}

/**
 * @brief Destructor
*/
GTAIgnoreManualAction::~GTAIgnoreManualAction()
{
    delete ui;
}

/**
 * @brief Update the table to be displayed according the window manAct list
 * @param window 
*/
void GTAIgnoreManualAction::updateTable(GTAEditorWindow* window)
{
    // Get the list of manAct rows
    _window = window;
    window->getManualActionRows(_manActRows);
    ui->label->setText(window->getElementName());

    if (_manActRows.isEmpty())
        return;

    // For each manAct
    for (int i = 0; i < _manActRows.count(); i++)
    {
        int index = _manActRows[i];

        // add line number
        QTableWidgetItem* line = new QTableWidgetItem(QString::number(index));
        line->setTextAlignment(Qt::AlignCenter);

        // add action
        GTACommandBase* pCurrentCommand = window->getCommandForSelectedRow(index);
        QTableWidgetItem* action = new QTableWidgetItem(pCurrentCommand->getPrintStatement());
        
        if (pCurrentCommand->isIgnoredInSCXML())
        {
            action->setCheckState(Qt::Checked);
            _initialIndexes.append(index);
        }
        else
        {
            action->setCheckState(Qt::Unchecked);
        }
            
        // add wait for
        GTAActionManual* manAct = dynamic_cast<GTAActionManual*>(pCurrentCommand);
        QString waitForText = "_";
        if (manAct->getAcknowledgetment())
        {
            waitForText = "Acknowledgment";
        }
        if (manAct->getUserFeedback())
        {
            waitForText = manAct->getParameter();
        }
        QTableWidgetItem* waitFor = new QTableWidgetItem(waitForText);
        waitFor->setTextAlignment(Qt::AlignCenter);

        // insert row
        ui->ManActTW->insertRow(i);       
        ui->ManActTW->setItem(i, 0, line);
        ui->ManActTW->setItem(i, 1, action);
        ui->ManActTW->setItem(i, 2, waitFor);
        
    }

    updateCheckBox();
}

/**
 * @brief Slot on ignoreAllCheckBox state change
 * @param state 
*/
void GTAIgnoreManualAction::onIgnoreAll(bool state)
{
    for (int i = 0; i < ui->ManActTW->rowCount(); i++)
    {
        QTableWidgetItem* action = ui->ManActTW->item(i, 1);
        action->setCheckState(state ? Qt::Checked: Qt::Unchecked);
    }
}

/**
 * @brief Slot on every item checkbox change
 * @param row is the changed row index
 * @param column is the changed column index
*/
void GTAIgnoreManualAction::onItemChanged(int row, int column)
{
    if (column == 1)
    {
        int lineId = ui->ManActTW->item(row, 0)->text().toInt();
        QTableWidgetItem* action = ui->ManActTW->item(row, 1);
        QColor bgColor;
        QFont font;
        // Ignore On
        if (action->checkState() == Qt::Checked)
        {
            bgColor = QColor(128, 0, 0, 50);
            font.setStrikeOut(true);
            if (!_finalIndexes.contains(lineId))
                _finalIndexes.append(lineId);
        }
        // Ignore Off
        else
        {
            bgColor = Qt::white;
            font.setStrikeOut(false);
            if (_finalIndexes.contains(lineId))
                _finalIndexes.removeOne(lineId);
        }
        action->setBackgroundColor(bgColor);
        action->setFont(font);
    }
    if (ui->ManActTW->rowCount() == _manActRows.count())
        updateCheckBox();
}

/**
 * @brief Get the list of checked items to be ignored
 * @return the list of indexes of the manual actions to be ignored
*/
QList<int>  GTAIgnoreManualAction::getSelection()
{
    return _finalIndexes;
}

/**
 * @brief Update the ignoreAllCheckBox if all or any of the item checkboxes are toggled
*/
void GTAIgnoreManualAction::updateCheckBox()
{
    disconnect(ui->ignoreAllCheckBox, SIGNAL(toggled(bool)), this, SLOT(onIgnoreAll(bool)));
    int nbCheckedRows = 0;
    for (int i = 0; i < ui->ManActTW->rowCount(); i++)
    {
        QTableWidgetItem* action = ui->ManActTW->item(i, 1);
        if (action == nullptr)
        {
            continue;
        }
        if (action->checkState() == Qt::Checked)
        {
            nbCheckedRows++;
        }
    }
    Qt::CheckState state = nbCheckedRows == ui->ManActTW->rowCount() ? Qt::Checked : Qt::Unchecked;
    ui->ignoreAllCheckBox->setCheckState(state);
    connect(ui->ignoreAllCheckBox, SIGNAL(toggled(bool)), this, SLOT(onIgnoreAll(bool)));
}

/**
 * @brief Slot on Cancel
*/
void GTAIgnoreManualAction::onCancelled()
{
    _finalIndexes = _initialIndexes;
}