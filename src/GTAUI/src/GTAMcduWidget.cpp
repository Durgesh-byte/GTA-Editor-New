#include "GTAMcduWidget.h"
#include "ui_GTAMcduWidget.h"
#define COL_COUNT 3
#define WIDTH 650
#define HEIGHT 400
#define input_start "$"
#define sep_component "#"
#define sep_param ","
#define sep_comboItem ":"
#define zone_start "["
#define zone_end "]"
#define char_upper_gap 128

/**
 * @brief Constructor
 * @param parent 
*/
GTAMcduWidget::GTAMcduWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTAMcduWidget)
{
    ui->setupUi(this);
    setMinimumSize(WIDTH,HEIGHT);
    defineCharTable();
    connect(ui->OKPB, SIGNAL(clicked()), this, SLOT(onOKButtonClick()));
    connect(ui->CancePB, SIGNAL(clicked()), this, SLOT(onCancelButtonClick()));
}

/**
 * @brief Destructor
*/
GTAMcduWidget::~GTAMcduWidget()
{
    delete ui;
}

/**
 * @brief Display the list of available functions and their parameters
*/
void GTAMcduWidget::displayFunctions()
{
    pAppController = GTAAppController::getGTAAppController();
    if (pAppController != NULL)
    {
        ui->FunctionListWidget->clear();
        _functionList.clear();

        ui->FuncArgsTW->clear();
        ui->FuncArgsTW->setRowCount(0);

         pAppController->getAllExternalToolNames();
        _functionList = pAppController->getAllExternalToolFunctionNames(GTA_MCDU_IDENTIFIER);
        _definitions = pAppController->getGenericToolDefinitions(GTA_MCDU_IDENTIFIER);

        for (const auto &func : _functionList)
        {
            QString functionName = func.getDisplayName();                       

            QListWidgetItem* item = new QListWidgetItem(functionName);
            item->setCheckState(Qt::Unchecked);
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->FunctionListWidget->addItem(item);

            // Show only the function if on edit mode
            QString functionToEdit = _userSelection.split(sep_component).first().remove(input_start);
            item->setHidden(!_userSelection.isEmpty() && functionToEdit != functionName);
        }

        connect(ui->FunctionListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onFunctionNameSelected(QListWidgetItem*)));

        // Select last selected function if not applied
        if (!_userSelection.isEmpty() && _userSelection.startsWith(input_start))
        {
            QString functionName = _userSelection.remove(input_start).split(sep_component)[0];
            QList<QListWidgetItem*> items = ui->FunctionListWidget->findItems(functionName, Qt::MatchExactly);
            if (!items.isEmpty())
            {
                ui->FunctionListWidget->itemClicked(items.at(0));
            }
        }

        ui->FuncArgsTW->setColumnHidden(1, true);
        show();
    }
}

/**
 * @brief Get the user selection
 * @return the user selection concatenated in a string
*/
QString GTAMcduWidget::getUserSelection()
{
    return "'" +  _userSelection + "'";
}

/**
 * @brief Set the user selection
 * @param the user selection concatenated in a string 
*/
void GTAMcduWidget::setUserSelection(const QString s)
{
    _userSelection = s;
    _userSelection.remove("'");
};

/**
 * @brief Slot on OK button clicked
*/
void GTAMcduWidget::onOKButtonClick()
{
    hide();
    updateUserSelectedValue();
    emit okPressed();
}

/**
 * @brief Slot on CANCEL button clicked
*/
void GTAMcduWidget::onCancelButtonClick()
{
    hide();
}

/**
 * @brief Manage keypress event
 * @param event 
*/
void GTAMcduWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}

/**
 * @brief Slot to update the functionName status and its arguments in ExternalTool action
 * @param item QListWidgetItem
*/
void GTAMcduWidget::onFunctionNameSelected(QListWidgetItem* item)
{
    disconnect(ui->FuncArgsTW, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onValueChanged(QTableWidgetItem*)));

    if (_functionList.isEmpty())
        return;

    // This condition is used to manage an exclusive checkBox state
    if (!_oldFunction.isEmpty())
    {
        QList<QListWidgetItem*> itemList;
        QListWidget* widgetList = item->listWidget();
        itemList = widgetList->findItems(_oldFunction, Qt::MatchExactly);
        // This condition avoids crashing GTA by changing the name of the ToolName
        if (!itemList.isEmpty())
        {
            // The functionName previously used will be unchecked
            itemList[0]->setCheckState(Qt::Unchecked);
        }
    }

    // Swap state
    item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);

    // We need to keep in mind the functionName previously used
    QString functionName = item->text();
    _oldFunction = functionName;

    // Display the function table widget
    QModelIndex  idx = ui->FunctionListWidget->currentIndex();
    if (idx.isValid())
        setupFuncArgsTW(idx.row(), functionName);

    connect(ui->FuncArgsTW, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onValueChanged(QTableWidgetItem*)));
}

/**
 * @brief Setup the table widget related to the selected function
 * @param index - index of the function
 * @param functionName - selected function name
*/
void GTAMcduWidget::setupFuncArgsTW(int index, const QString& functionName)
{
    if (index < 0 || index >= _functionList.count())
        return;

    GTAGenericFunction func = _functionList.at(index);
    if (func.getFunctionName() != functionName && func.getDisplayName() != functionName)
        return;

    // Update with input values if not empty
    parseUserSelection(func);

    // Init the table
    QList<GTAGenericArgument> args = func.getFunctionArguments();
    int rowCnt = args.count();
    initFuncArgsTW(rowCnt);
    if (rowCnt <= 0)
        return;


    bool argListChanged = false;
    for (int i = 0; i < rowCnt; i++)
    {
        _argChanged = false;
        GTAGenericArgument arg = args.at(i);

        // Set name
        QString& argName = arg.getArgumentName();       
        QTableWidgetItem* itemName = new QTableWidgetItem(argName);        
        itemName->setFlags(itemName->flags() ^ Qt::ItemIsEditable);
        ui->FuncArgsTW->setItem(i, 0, itemName);

        // Set type
        QString& argType = arg.getArgumentType();
        QTableWidgetItem* itemType = new QTableWidgetItem(argType);
        itemType->setFlags(itemType->flags() ^ Qt::ItemIsEditable);
        ui->FuncArgsTW->setItem(i, 1, itemType);

        // Set values
        if (isOnCheckBox(arg))	//Remove hardcoded values
        {
            addCheckBoxIntoFuncArgsTW(i, arg);
        }
        else
        {
            QStringList vals = arg.getValues();
            if (isOnComboBox(arg))
            {
                addComboBoxIntoFuncArgsTW(i, arg);
            }
            else
            {
                addTextBoxIntoFuncArgsTW(i, arg);
            }

        }

        if (_argChanged)
        {
            args.removeAt(i);
            args.insert(i, arg);
            argListChanged = true;
        }
    }

    if (argListChanged)
    {
        func.setFunctionArguments(args);
        _functionList.removeAt(index);
        _functionList.insert(index, func);
    }

    updateUserSelectedValue();
    connect(ui->FuncArgsTW, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onValueChanged(QTableWidgetItem*)));
}

/**
 * @brief Initialize the table widget with appropriate rows, columns and header
 * @param rowCnt - number of rows in the table
*/
void GTAMcduWidget::initFuncArgsTW(int rowCnt)
{
    ui->FuncArgsTW->clear();
    ui->FuncArgsTW->setRowCount(rowCnt);
    ui->FuncArgsTW->setColumnCount(COL_COUNT);

    QStringList header;
    header << "Argument Name" << "Argument Type" << "Value";
    ui->FuncArgsTW->setHorizontalHeaderLabels(header);

    _ComboBox.clear();
    _searchItem.clear();
}

/**
 * @brief Check if the element is on a checkbox
 * @param arg - GTAGenericArgument
 * @return boolean
*/
bool GTAMcduWidget::isOnCheckBox(GTAGenericArgument arg)
{
    QString& argType = arg.getArgumentType();
    return argType == "bool" || argType == "boolean";
}

/**
 * @brief Check if the element is on a combobox
 * @param arg - GTAGenericArgument
 * @return boolean
*/
bool GTAMcduWidget::isOnComboBox(GTAGenericArgument arg)
{
    return !isOnCheckBox(arg) && !arg.getValues().isEmpty();
}

/**
 * @brief Check if the element is on a textbox
 * @param arg - GTAGenericArgument
 * @return boolean
*/
bool GTAMcduWidget::isOnTextBox(GTAGenericArgument arg)
{
    return !isOnCheckBox(arg) && !isOnComboBox(arg);
}

/**
 * @brief Add check box as values in the table widget
 * @param row - row to be updated
 * @param arg - GTAGenericArgument of a given row
*/
void GTAMcduWidget::addCheckBoxIntoFuncArgsTW(int row, GTAGenericArgument arg)
{
    QCheckBox* valCB = new QCheckBox();
    valCB->setCheckState(Qt::Unchecked);

    QString& userSetVal = arg.getUserSelectedValue();
    if (!userSetVal.isEmpty())
    {
        if (userSetVal == "true")
        {
            valCB->setCheckState(Qt::Checked);
        }
    }
    arg.setUserSelectedValue(valCB->isChecked() ? "true" : "false");

    _argChanged = true;
    valCB->setObjectName(arg.getArgumentName());
    valCB->setDisabled(arg.isArgConst());
    ui->FuncArgsTW->setCellWidget(row, 2, valCB);
    connect(valCB, SIGNAL(stateChanged(int)), this, SLOT(onValueCheckBoxStateChanged(int)));
}

/**
 * @brief Add combo box as values in the table widget
 * @param row row - row to be updated
 * @param arg arg - GTAGenericArgument of a given row
*/
void GTAMcduWidget::addComboBoxIntoFuncArgsTW(int row, GTAGenericArgument arg)
{
    QComboBox* comboVals = new QComboBox();

    QString& argName = arg.getArgumentName();
    QString& argType = arg.getArgumentType();
    QStringList& vals = arg.getValues();

    QString& userSetVal = arg.getUserSelectedValue();
    QString& condition = arg.getCondition();
    size_t valueIdx, userValueIdx;
    if (!userSetVal.isEmpty())
    {
        if (arg.isConditionValListEmpty())
        {
            valueIdx = 0;
            if (argType == "string")
                valueIdx = vals.indexOf(QString(userSetVal));
            else
                valueIdx = vals.indexOf(userSetVal);
            if (static_cast<int>(valueIdx) > -1 && valueIdx < vals.count())
            {
                comboVals->clear();
                comboVals->addItems(vals);
                comboVals->setCurrentIndex(valueIdx);
            }
            else
            {
                comboVals->clear();
                comboVals->addItems(vals);
                QStringList defVals = arg.getDefaultValue();
                if (!defVals.isEmpty())
                {
                    if (vals.contains(defVals.at(0)))
                        arg.setUserSelectedValue(defVals.at(0));
                    else
                        arg.setUserSelectedValue(vals.at(0));
                }
                else if (valueIdx < vals.count())
                    arg.setUserSelectedValue(vals.at(valueIdx));
                else
                    arg.setUserSelectedValue(vals.at(0));

                int idx = vals.indexOf(arg.getUserSelectedValue());
                if (idx >= 0)
                    comboVals->setCurrentIndex(idx);
            }
        }
        else
        {
            valueIdx = arg.getUserSelectedHashIndex();
            if (valueIdx != 999)
            {
                QHash<int, QStringList> conditionVals = arg.getConditionValList();
                QStringList valueList = conditionVals.value(valueIdx);
                if (!valueList.isEmpty())
                {
                    valueIdx = 0;
                    if (argType == "string")
                        valueIdx = valueList.indexOf(userSetVal);
                    else
                        valueIdx = vals.indexOf(userSetVal);
                    if (static_cast<int>(valueIdx) > -1 && valueIdx < valueList.count())
                    {
                        comboVals->clear();
                        comboVals->addItems(valueList);
                        comboVals->setCurrentIndex(valueIdx);
                    }
                }
            }
            else
            {
                valueIdx = 0;
                QHash<int, QStringList> conditionVals = arg.getConditionValList();
                QStringList valueList = conditionVals.value(valueIdx);
                if (!valueList.isEmpty())
                {
                    userValueIdx = valueList.indexOf(userSetVal.remove("'"));
                    bool ok;
                    auto val = userSetVal.toInt(&ok);                 
                    if (userValueIdx > valueList.size())
                    {
                        if (ok && val < valueList.size())
                        {
                            userValueIdx = val == -1 ? static_cast<size_t>(valueList.size()) - 1 : userSetVal.toInt();
                        }
                        else
                        {
                            userValueIdx = 0;
                        }                         
                    }                        

                    arg.setUserSelectedValue(userSetVal);
                    comboVals->clear();
                    comboVals->addItems(valueList);
                    comboVals->setCurrentIndex(userValueIdx);
                    _argChanged = true;
                }
            }
        }
    }
    else
    {
        if (arg.isConditionValListEmpty())
        {
            comboVals->clear();
            comboVals->addItems(vals);
            QString val = vals.at(0);
            QStringList constVals = arg.getConstantValue();
            QStringList defaultVals = arg.getDefaultValue();
            if (!defaultVals.isEmpty())
            {
                val = defaultVals.at(0);
            }
            if (!constVals.isEmpty())
            {
                val = constVals.at(0);
            }

            valueIdx = comboVals->findText(val);
            if (valueIdx > 0)
            {
                comboVals->setCurrentIndex(valueIdx);
                arg.setUserSelectedValue(val);
            }
            else
                arg.setUserSelectedValue(vals.at(0));

            _argChanged = true;
        }
        else
        {
            QStringList constVals = arg.getConstantValue();
            QStringList defaultVals = arg.getDefaultValue();
            QHash<int, QStringList> conditionVals = arg.getConditionValList();
            if (constVals.isEmpty() && defaultVals.isEmpty())
            {
                QStringList valuesList = conditionVals.value(0);
                comboVals->clear();
                comboVals->addItems(valuesList);
                arg.setUserSelectedValue(valuesList.at(0));
                arg.setUserSelectedHashIndex(0);
                _argChanged = true;
            }
            else
            {
                QStringList finalList;
                if (constVals.isEmpty())
                    finalList = defaultVals;
                else if (defaultVals.isEmpty())
                    finalList = constVals;

                QString condition = arg.getCondition();
                if (!_ComboBox.isEmpty() && !condition.isEmpty())
                {
                    auto itr = _ComboBox.find(condition);
                    if (itr.key() == condition)
                    {
                        QComboBox* pCombo = itr.value();
                        if (pCombo)
                        {
                            valueIdx = pCombo->currentIndex();
                            if (static_cast<int>(valueIdx) > -1 && valueIdx < finalList.count())
                            {
                                QStringList valuesList = conditionVals.value(valueIdx);
                                QString userSetValue = finalList.at(valueIdx);
                                int valuesListIndex = valuesList.indexOf(userSetValue);

                                if (valuesListIndex > -1 && valuesListIndex < valuesList.count())
                                {
                                    comboVals->clear();
                                    comboVals->addItems(valuesList);
                                    comboVals->setCurrentIndex(valuesListIndex);
                                    arg.setUserSelectedValue(userSetValue);
                                    arg.setUserSelectedHashIndex(valueIdx);
                                    _argChanged = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    comboVals->setObjectName(argName);
    comboVals->setDisabled(arg.isArgConst());
    ui->FuncArgsTW->setCellWidget(row, 2, comboVals);
    _ComboBox.insert(argName, comboVals);
    connect(comboVals, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboValueChanged(QString)));
}

/**
 * @brief Add text box as values in the table widget
 * @param row - row to be updated
 * @param arg - GTAGenericArgument of a given row
*/
void GTAMcduWidget::addTextBoxIntoFuncArgsTW(int row, GTAGenericArgument arg)
{
    connect(ui->FuncArgsTW, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onValueChanged(QTableWidgetItem*)));
    QString& userSetVal = arg.getUserSelectedValue();
    QTableWidgetItem* itemVal = new QTableWidgetItem();
    QString& searchType = arg.getSearchType();
    QString& path = arg.getPath();

    if (!searchType.isEmpty())
    {
        connect(ui->FuncArgsTW, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onSearchTypeClicked(int, int)));

        if ((searchType == XNODE_FILE_SEARCH_TYPE || searchType == XNODE_FOLDER_SEARCH_TYPE) && !path.isEmpty())
        {
            searchType = QString("%1-%2").arg(searchType, path);
        }
        else if (searchType == XNODE_USER_DEF_DB_SEARCH_TYPE)
        {
            searchType = QString("%1-%2").arg(searchType, arg.getUserDbName());
        }

        itemVal->setToolTip("Search Type -" + searchType);
        itemVal->setText("Search Type -" + searchType);
        QFont font;
        font.setWeight(QFont::Light);
        font.setStyle(QFont::StyleItalic);
        itemVal->setFont(font);
    }

    if (!userSetVal.isEmpty())
    {
        QFont font;
        font.setWeight(QFont::Normal);
        font.setStyle(QFont::StyleNormal);
        itemVal->setFont(font);
        itemVal->setText(userSetVal);
    }

    if (arg.isArgConst())
    {
        itemVal->setFlags(itemVal->flags() ^ Qt::ItemIsEditable);
    }
    ui->FuncArgsTW->setItem(row, 2, itemVal);
}

/**
 * @brief Update user selected value
*/
void GTAMcduWidget::updateUserSelectedValue()
{
    QModelIndex funcIdx = ui->FunctionListWidget->currentIndex();
    if (funcIdx.isValid())
    {
        int functionRow = funcIdx.row();
        if (functionRow >= 0 && functionRow < _functionList.count())
        {
            GTAGenericFunction function = _functionList.at(functionRow);
            QList<GTAGenericArgument> arguments = function.getFunctionArguments();
            _userSelection = input_start + function.getFunctionName();

            GTAGenericArgument arg;
            for (int i = 0; i < arguments.count(); i++)
            {
                arg = arguments.at(i);
                QString value = arg.getUserSelectedValue();
                QString name = arg.getArgumentName();

                QString val;
                if (isOnCheckBox(arg))
                {
                    val = value == "true" ? "true": "false";
                    
                }
                if (isOnComboBox(arg))
                {
                    val = _ComboBox.find(arg.getArgumentName()).value()->currentText();
                }
                if (isOnTextBox(arg))
                {
                    val = value;
                }

                arg.setUserSelectedValue(val);
                
                // Update value for command
                if(!_userSelection.contains("Search_Text"))
                {
                    val.remove(" ");
                    val.remove("'");     
                }
                else
                {
                    // Remove quotes
                    if (val.startsWith("'") && val.endsWith("'"))
                    {
                        val.remove(0, 1);
                        val.remove(-1, 1);
                    }                        
                    // Convert Message
                    if (name == "Message")
                    {
                        convertAlphabeticalMessageToMCDUCode(val);
                    }
                    // Convert Color or Size
                    if ( name == "Color" || name == "Size")
                    {
                        val = val.split(sep_comboItem).first();
                    }
                }
                if (name == "posX1")
                {
                    val = zone_start + val;
                }
                if (name == "posY2")
                {
                    val = val + zone_end;
                }
                if (name == "Activate")
                {                   
                    val = val == "true" ? "1" : "0";
                }             
                if (val == "None")
                    val.clear();               

                QString sep = _userSelection.contains(sep_component) ? sep_param : sep_component ;
                _userSelection += sep + val;

                arguments.removeAt(i);
                arguments.insert(i, arg);
                function.setFunctionArguments(arguments);
                _functionList.removeAt(functionRow);
                _functionList.insert(functionRow, function);
            }

            // Clean SearchText zone if all fields are not set
            if (_userSelection.contains("Search_Text"))
            {
                QString zone = _userSelection.split(zone_start).last();
                QStringList zoneList = zone.split(",");
                if (zoneList.contains("") || zoneList.last() == zone_end)
                {
                    _userSelection.remove(zone);
                    _userSelection.remove(-1, 1);
                }
            }
        }
    }
}

/**
 * @brief Slot on changed value of a parameter
 * @param pItem 
*/
void GTAMcduWidget::onValueChanged(QTableWidgetItem* pItem)
{
    if (pItem)
    {
        // Get the content of cell
        QString val = pItem->text();
        // Get the row number of the item
        int argRow = pItem->row();

        QModelIndex idx = ui->FunctionListWidget->currentIndex();
        if (idx.isValid())
        {
            int functionRow = idx.row();
            if (functionRow >= 0 && functionRow < _functionList.count())
            {
                GTAGenericFunction func = _functionList.at(functionRow);
                QList<GTAGenericArgument> args = func.getFunctionArguments();
                if (argRow >= 0 && argRow < args.count())
                {
                    GTAGenericArgument argument = args.at(argRow);
                    argument.setUserSelectedValue(val);
                    args.removeAt(argRow);
                    args.insert(argRow, argument);
                    func.setFunctionArguments(args);
                    _functionList.removeAt(functionRow);
                    _functionList.insert(functionRow, func);
                }
            }
        }
    }
}

/**
 * @brief Slot on changed state of a checkbox
 * @param state 
*/
void GTAMcduWidget::onValueCheckBoxStateChanged(int state)
{
    QString userSelVal;
    if (state == Qt::Unchecked)
        userSelVal = "false";
    else
        userSelVal = "true";

    QCheckBox* pCheckBox = dynamic_cast<QCheckBox*>(sender());
    if (pCheckBox)
    {
        QModelIndex idx = ui->FunctionListWidget->currentIndex();
        if (idx.isValid())
        {
            int functionRow = idx.row();
            if (functionRow >= 0 && functionRow < _functionList.count())
            {
                GTAGenericFunction func = _functionList.at(functionRow);
                QList<GTAGenericArgument> args = func.getFunctionArguments();
                GTAGenericArgument argument;
                bool found = false;
                int i;
                for (i = 0; i < args.count(); i++)
                {
                    argument = args.at(i);
                    if (argument.getArgumentName() == pCheckBox->objectName())
                    {
                        found = true;
                        break;
                    }
                }

                if (found)
                {
                    argument.setUserSelectedValue(userSelVal);
                    argument.setUserSelectedValue(userSelVal);
                    args.removeAt(i);
                    args.insert(i, argument);
                    func.setFunctionArguments(args);
                    _functionList.removeAt(functionRow);
                    _functionList.insert(functionRow, func);
                }
            }
        }
    }
}


/**
 * @brief Define the map between a character (string) and MCDU code (number)
*/
void GTAMcduWidget::defineCharTable()
{      
    QString alphanum_char;
    int mcdu_code;

    // 16 to 19
    QStringList list0 = { "↑","↓","←","→" };
    for (int i = 0; i < list0.count(); i++)
    {
        alphanum_char = list0[i];
        mcdu_code = i + 16;
        _keyboardToMcdu.insert(std::pair<QString, int>(alphanum_char, mcdu_code));        
        _McduToKeyboard.insert(std::pair<int, QString>(mcdu_code, alphanum_char));
    }

    // 23 to 25
    QStringList list1 = { "°","Δ","◻" };
    for (int i = 0; i < list1.count(); i++)
    {
        alphanum_char = list1[i];
        mcdu_code = i + 23;
        _keyboardToMcdu.insert(std::pair<QString, int>(alphanum_char, mcdu_code));
        _McduToKeyboard.insert(std::pair<int, QString>(mcdu_code, alphanum_char));
    }

    // 32 to 126
    QStringList list2 = { 
        " ","!","\"","#","$","%","&","'","(",")","*","+",",","-",".","/","∅",
        "1","2","3","4","5","6","7","8","9",
        ":", ";", "<", "=", ">", "?", "@",
        "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
        "[", "\\", "]", "^","_","`",
        "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
        "{","|","}","~" 
    };
    for (int i = 0; i < list2.count(); i++)
    {
        alphanum_char = list2[i];
        mcdu_code = i + 32;        
        _keyboardToMcdu.insert(std::pair<QString, int>(alphanum_char, mcdu_code));
        _McduToKeyboard.insert(std::pair<int, QString>(mcdu_code, alphanum_char));
    }
}

/**
 * @brief Convert alphabetical text in SearchText function into MCDU code
 * @param txt 
*/
void GTAMcduWidget::convertAlphabeticalMessageToMCDUCode(QString& txt)
{
    QStringList newText;
    for(auto chr: txt)
    {   
        QString sChr = QString(chr);
        auto found = _keyboardToMcdu.find(sChr);
        if (_keyboardToMcdu.find(sChr) != _keyboardToMcdu.end())
        {            
            newText.append(QString::number(_keyboardToMcdu[sChr]));
        }
    }
    txt = zone_start + newText.join(sep_param) + zone_end;
}

/**
 * @brief Convert MCDU code message into alphabetical text in SearchText funcion
 * @param txt 
*/
void GTAMcduWidget::convertMCDUCodeToAlphabeticalMessage(QString& txt)
{
    QString newTxt;
    bool ok;
    int code;
    QStringList charList = txt.split(sep_param);
    for (int i=0; i <charList.size() ; i++)
    {
        code = charList.at(i).toInt(&ok);
        if (ok && _McduToKeyboard.find(code) != _McduToKeyboard.cend())
        {
            newTxt.append(_McduToKeyboard[code]);
        }
    }
    txt = newTxt;    
}

/**
 * @brief Parse the user selection string into the corresponding table
*/
void GTAMcduWidget::parseUserSelection(GTAGenericFunction& func)
{
    QModelIndex funcIdx = ui->FunctionListWidget->currentIndex();
    if (!funcIdx.isValid() || _userSelection.isEmpty())
        return;
    
    QList<GTAGenericArgument> args = func.getFunctionArguments();
    QString funcName = _userSelection.split(sep_component).first().remove(input_start);
    QString parameters = _userSelection.split(sep_component).last();
    
    if (funcName != func.getFunctionName())
        return;

    QStringList funcParamList;
    if (funcName == "Search_Text")
    {
        auto idxMsgEnd = parameters.indexOf(zone_end);
        QString msg = parameters.mid(0,idxMsgEnd).remove(zone_start);
        convertMCDUCodeToAlphabeticalMessage(msg);
        funcParamList.append(msg);
        
        QString parameter = parameters.mid(idxMsgEnd+2,-1);
        QStringList param = parameter.split(sep_param);
        for each(auto p in param)
            funcParamList.append(p);
    }
    else
    {
        funcParamList = parameters.split(sep_param);
    }
        
    // Get the number of mandatory fields and compare to userSelection
    int nbMandatoryFields = 0;
    for each (auto arg in args)
    {
        if (arg.isArgumentMandatory())
            nbMandatoryFields++;
    }
    if (nbMandatoryFields > funcParamList.size() || funcParamList.isEmpty())
        return;

    QList<GTAGenericArgument> updatedArgs;
    for (int i = 0; i < args.size(); i++)
    {
        GTAGenericArgument arg = args.at(i);            
        QString argValueToSet;
        if (funcParamList.size() > i)
        {
            argValueToSet = funcParamList.at(i);

            // For argument not in Message of Search_Text function
            if (arg.getArgumentName() != "Message" || funcName != "Search_Text")
            {
                argValueToSet.remove("'").remove(zone_start).remove(zone_end);
            }                
        }          

        // For argument in checkbox
        if (arg.getArgumentType() == "boolean")
        {
            argValueToSet = argValueToSet == "1" ? "true" : "false";
        }
           
          
        arg.setUserSelectedValue(argValueToSet);  
        updatedArgs.append(arg);
    }
    
    func.setFunctionArguments(updatedArgs);
}