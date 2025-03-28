#include "GTAGenericToolWidget.h"
#include "ui_GTAGenericToolWidget.h"
#include "GTAGenericArgument.h"
#include "GTAUiUtils.h"
#include <QFileDialog>
#include "GTAICDParameter.h"
#include "GTAUtil.h"
#define COL_COUNT 3

GTAGenericToolWidget::GTAGenericToolWidget(QWidget *parent) :
        GTAActionWidgetInterface(parent),
        ui(new Ui::GTAGenericToolWidget)
{
    ui->setupUi(this);
    ui->FuncArgsTW->clear();
    ui->FuncArgsTW->setRowCount(0);
    ui->FuncArgsTW->setColumnCount(COL_COUNT);
    _isNewCmd = true;

    QStringList header;
    header <<"Argument Name"<<"Argument Type"<<"Value";
    ui->FuncArgsTW->setHorizontalHeaderLabels(header);


    ui->FunctionListWidget->clear();
    _functionList.clear();
    _searchItem.clear();

    ui->ToolConfigLabel->setDisabled(true);
    ui->ToolConfigLE->setDisabled(true);
    ui->SearchInputFilePB->setDisabled(true);

    ui->SCXMLInputModCB->setEnabled(true);
    ui->SCXMLInputModCB->setChecked(false);

    _isInputFileSelected = false;

    pAppController = GTAAppController::getGTAAppController();
    if(pAppController!= NULL)
    {
        _allToolNamesHash = pAppController->getAllExternalToolNames();
        QStringList allTools;
        QList<QString> keys = _allToolNamesHash.keys();
        for(int i=0;i<keys.count();i++)
        {
            QString value = _allToolNamesHash.value(keys.at(i));

            // Do not include MCDU as ExternalTool
            if (value.contains(GTA_MCDU_IDENTIFIER))
                continue;

            if(!value.trimmed().isEmpty())
                allTools.append(value.trimmed());
            else
                allTools.append(keys.at(i).trimmed());
        }
        allTools.removeDuplicates();
        if(!allTools.isEmpty())
        {
            ui->toolNameCB->addItems(allTools);
            ui->FunctionListWidget->clear();
            OnToolNameChanged(allTools.first());
            connect(ui->toolNameCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(OnToolNameChanged(QString)));
            connect(ui->ExistingConfigCB,SIGNAL(stateChanged(int)),this,SLOT(onExistingConfigCBStateChanged(int)));
        }

        _pSearchWidget = new GTAGenSearchWidget();
        if(_pSearchWidget)
        {
            _pSearchWidget->hide();
            connect(_pSearchWidget,SIGNAL(okPressed()),this,SLOT(onSearchOKPBClick()));
            connect(_pSearchWidget,SIGNAL(cancelPressed()),this,SLOT(onSearchCancelPBClick()));
        }
    }
	_oldFunction = QString();
}

void GTAGenericToolWidget::onExistingConfigCBStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        ui->ToolConfigLabel->setEnabled(true);
        ui->ToolConfigLE->setEnabled(true);
        ui->SearchInputFilePB->setEnabled(true);
        ui->FunctionListWidget->clear();
        ui->FuncArgsTW->clear();
        ui->FuncArgsTW->setRowCount(0);
        connect(ui->SearchInputFilePB,SIGNAL(clicked()),this,SLOT(onSearchInputFilePBClicked()));
        _MasterFunctionList.clear();
        _MasterFunctionList = _functionList;
    }
    else if (state == Qt::Unchecked)
    {
        ui->ToolConfigLabel->setDisabled(true);
        ui->ToolConfigLE->clear();
        ui->ToolConfigLE->setDisabled(true);
        ui->SearchInputFilePB->setDisabled(true);
        disconnect(ui->SearchInputFilePB,SIGNAL(clicked()),this,SLOT(onSearchInputFilePBClicked()));
        restoreDefaults();
    }
}

GTAGenericToolWidget::~GTAGenericToolWidget()
{
    if(_pSearchWidget != NULL) {delete _pSearchWidget;_pSearchWidget = NULL;}
    disconnect(ui->FunctionListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onFunctionNameSelected(QListWidgetItem*)));
    delete ui;
}


void GTAGenericToolWidget::clear()
{
    if(pAppController!= NULL)
    {
        disconnect(ui->FunctionListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onFunctionNameSelected(QListWidgetItem*)));
        ui->FuncArgsTW->disconnect();
        ui->FunctionListWidget->disconnect();

        _functionList.clear();

        _ComboBox.clear();


        if(ui->ExistingConfigCB->checkState() == Qt::Checked)
        {
            ui->ExistingConfigCB->setCheckState(Qt::Unchecked);

        }

        _allToolNamesHash.clear();
        _allToolNamesHash = pAppController->getAllExternalToolNames();
        QStringList allTools;
        QList<QString> keys = _allToolNamesHash.keys();
        for(int i=0;i<keys.count();i++)
        {
            QString value = _allToolNamesHash.value(keys.at(i));
            if(!value.trimmed().isEmpty())
                allTools.append(value.trimmed());
        }
        allTools.removeDuplicates();
        ui->toolNameCB->addItems(allTools);

    }

}
bool GTAGenericToolWidget::isValid()const
{
    bool isValid;
    bool isFunctionSelected = false;
    for (int i = 0; i < ui->FunctionListWidget->count(); i++)
    {
        QListWidgetItem *pItem = ui->FunctionListWidget->item(i);
        if(pItem)
        {
            GTAGenericFunction function = _functionList.at(i);
            QString functionName = function.getDisplayName();
            if((pItem->checkState() == Qt::Checked) && (pItem->text() == functionName))
            {
                isFunctionSelected = true;
                QList<GTAGenericArgument> argList = function.getFunctionArguments();

                //condition for a valid function and no arguments
                if (argList.isEmpty() && isFunctionSelected)
                    isValid = true;
                else
                {
                    for(int j = 0; j < argList.count(); j++)
                    {
                        GTAGenericArgument arg = argList.at(j);
                        if(arg.isArgumentMandatory())
                        {
                            if(arg.getUserSelectedValue().isEmpty())
                            {
                                QStringList vals = arg.getValues();
                                if(!vals.isEmpty())
                                {
                                    isValid = true;
                                }
                                else
                                {
                                    isValid = false;
                                    return isValid;
                                }
                            }
                            else
                                isValid = true;
                        }
                        else
                            isValid = true;
                    }
                }
            }
        }
    }
    if(!isFunctionSelected)
        isValid = false;

    return isValid;

}

bool GTAGenericToolWidget::hasActionOnFail()
{
    if(pAppController)
    {
        return pAppController->getActionOnFailInfoForGenericTool();
    }
    else
        return false;
}

bool GTAGenericToolWidget::hasTimeOut(int& )
{
    if(pAppController)
    {
        return pAppController->getTimeOutInfoForGenericTool();
    }
    else
        return false;
}
bool GTAGenericToolWidget::hasDumpList()
{
    if(pAppController)
    {
        return pAppController->getDumpListInfoForGenericTool();
    }
    else
        return false;
}

bool GTAGenericToolWidget::hasExternalComment()const
{
    if(pAppController)
    {
        return pAppController->getExternalCommentInfoForGenericTool();
    }
    else
        return false;
}


bool GTAGenericToolWidget::getActionCommand(GTAActionBase *& opCmd)const
{
    bool rc = false;
    if(isValid())
    {
        rc =true;
        GTAGenericToolCommand *pGenericToolCmd = new GTAGenericToolCommand(ACT_GEN_TOOL,"");
        QString userToolName = ui->toolNameCB->currentText();
        QString toolName = userToolName;
        

        QList<QString> keys = _allToolNamesHash.keys();
        QString value;
        for(int i=0;i<keys.count();i++)
        {
            value = _allToolNamesHash.value(keys.at(i));
            if(value.trimmed() == userToolName)
            {
                toolName = keys.at(i);
                break;
            }
        }

        QStringList statement;
        for (int i = 0; i < ui->FunctionListWidget->count(); i++)
        {
            QListWidgetItem *pItem = ui->FunctionListWidget->item(i);
            if(pItem)
            {
                QString cmdStatement;
                GTAGenericFunction function = _functionList.at(i);
                QString functionName = function.getFunctionName();


                QString functionDisplayName = function.getDisplayName();
                if((pItem->checkState() == Qt::Checked) && (pItem->text() == functionName || pItem->text() == functionDisplayName))
                {

                    QList<GTAGenericArgument> hiddenArgsList = function.getHiddenArguments();
                    QList<GTAGenericArgument> allArgs = function.getFunctionArguments();
                    for(int j = 0; j < hiddenArgsList.count(); j++)
                    {
                        GTAGenericArgument objArg = hiddenArgsList.at(j);
                        int id = objArg.getId();
                        allArgs.insert(id,objArg);
                    }
                    function.setAllArguments(allArgs);

                    pGenericToolCmd->insertFunction(function);
                    if(!value.isEmpty())
                        cmdStatement += value + " : " + functionDisplayName + "\n";
                    else
                        cmdStatement += toolName + " : " + functionDisplayName + "\n";
                    statement.append(cmdStatement);
                }
            }
        }

        pGenericToolCmd->setToolName(toolName);
        pGenericToolCmd->setToolDisplayName(value);
        pGenericToolCmd->setDefinitions(_definitions);
        pGenericToolCmd->setUserDbList(_UserDb);

        pGenericToolCmd->setSCXMLModSelected(ui->SCXMLInputModCB->isChecked());

        opCmd = pGenericToolCmd;
    }
    return rc;
}



void GTAGenericToolWidget::setActionCommand(const GTAActionBase * pCmd)
{
    if(pCmd != NULL)
    {
        _isNewCmd = false;
        ui->FuncArgsTW->clear();
        ui->FuncArgsTW->setColumnCount(COL_COUNT);
        ui->FuncArgsTW->setRowCount(0);
        QStringList header;
        header <<"Argument Name"<<"Argument Type"<<"Value";
        ui->FuncArgsTW->setHorizontalHeaderLabels(header);

        ui->FunctionListWidget->clear();

        GTAGenericToolCommand* pGenericToolCmd = dynamic_cast<GTAGenericToolCommand*>((GTAActionBase*)pCmd);
        if(pGenericToolCmd != NULL)
        {
            QString userToolName = _allToolNamesHash.value(pGenericToolCmd->getToolName());

            //enable/disable the checkbox
            ui->SCXMLInputModCB->setChecked(pGenericToolCmd->getSCXMLModSelected());

            int idx = ui->toolNameCB->findText(userToolName);
            if(idx>=0)
            {
                disconnect(ui->toolNameCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(OnToolNameChanged(QString)));
                ui->toolNameCB->setCurrentIndex(idx);
                connect(ui->toolNameCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(OnToolNameChanged(QString)));
            }
            else
                ui->toolNameCB->addItem(userToolName);
            ui->toolNameCB->setEnabled(false);
            ui->ExistingConfigCB->setEnabled(false);
            ui->ToolConfigLE->setEnabled(false);
            _functionList.clear();
            _definitions.clear();
            _UserDb.clear();

            _functionList = pGenericToolCmd->getCommand();
            _definitions = pGenericToolCmd->getDefinitions();
            _UserDb = pGenericToolCmd->getUserDbList();

            for(int i = 0; i < _functionList.size(); i++)
            {
                GTAGenericFunction func = _functionList.at(i);

                QListWidgetItem *pItem = new QListWidgetItem(func.getDisplayName());
                pItem->setCheckState(Qt::Checked);
                ui->FunctionListWidget->addItem(pItem);
            }
            connect(ui->FunctionListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onFunctionNameSelected(QListWidgetItem*)));
        }
    }
}

void GTAGenericToolWidget::connectExternalItemsWithLocalWidget(QWidget *pParentWidget)
{
    if(pParentWidget != NULL)
        connect(this,SIGNAL(onReEvaluateShowItems(bool,bool,bool,bool,bool,bool)),pParentWidget,SLOT(onReEvaluateShowItems(bool,bool,bool,bool,bool,bool)));
}

void GTAGenericToolWidget::OnToolNameChanged(const QString &iToolNameChanged)
{
    if(pAppController != NULL)
    {
        ui->FunctionListWidget->clear();
        _functionList.clear();

        ui->FuncArgsTW->clear();
        ui->FuncArgsTW->setRowCount(0);

        if(ui->ExistingConfigCB->checkState() == Qt::Checked)
        {
            ui->ExistingConfigCB->setCheckState(Qt::Unchecked);
        }
        QString toolName =iToolNameChanged;
        QList<QString> keys = _allToolNamesHash.keys();
        for(int i=0;i<keys.count();i++)
        {
            QString value = _allToolNamesHash.value(keys.at(i));
            if(value.trimmed() == iToolNameChanged)
            {
                toolName = keys.at(i);
                break;
            }
        }

        _functionList = pAppController->getAllExternalToolFunctionNames(toolName);
        _UserDb = pAppController->getUserDefinedDb();
        _definitions = pAppController->getGenericToolDefinitions(toolName);

        for(int i=0;i<_functionList.count();i++)
        {
            GTAGenericFunction func = _functionList[i];
            QString functionName = func.getDisplayName();                       //Display Name is different than name

            QListWidgetItem *item = new QListWidgetItem(functionName);
            item->setCheckState(Qt::Unchecked);
            if (functionName.isEmpty())
            {
                item->setText(func.getFunctionName());
                item->setBackgroundColor(TITLE_EDITOR_BACK_COLOR); 
                item->setTextColor(Qt::white);                
                item->setCheckState(Qt::PartiallyChecked);
            }           
            ui->FunctionListWidget->addItem(item);            
        }
        connect(ui->FunctionListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onFunctionNameSelected(QListWidgetItem*)));

        int timeout = 0;
        bool isActOnFail = hasActionOnFail();
        bool isDumpList = hasDumpList();
        bool isExternalComment = hasExternalComment();
        bool isTimeout = hasTimeOut(timeout);
        emit(onReEvaluateShowItems(isActOnFail,false,isTimeout,false,isDumpList,isExternalComment));
    }
}

/* Initialize the table widget with appropriate rows, columns and header
* @input: rowCnt - number of rows in the table
*/
void GTAGenericToolWidget::initFuncArgsTW(int rowCnt)
{
    ui->FuncArgsTW->clear();
    ui->FuncArgsTW->setRowCount(rowCnt);
    ui->FuncArgsTW->setColumnCount(COL_COUNT);

    QStringList header;
    header << "Argument Name" << "Argument Type" << "Value";
    ui->FuncArgsTW->setHorizontalHeaderLabels(header);

    _argDependencyTable.clear();
    _ComboBox.clear();
    _searchItem.clear();
}


/* Set the name or the type of a given row in the table
* @input: row - row of the cell to be updated
* @input: col - column of the cell to be updated
* @input: columnLabel - label of the current column
*/
void GTAGenericToolWidget::setNameOrTypeItem(int row, int col, const QString &columnLabel)
{
    QTableWidgetItem* item = new QTableWidgetItem(columnLabel);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->FuncArgsTW->setItem(row, col, item);
}


/* Add check box as values in the table widget
* @input: row - row to be updated
* @input: arg - GTAGenericArgument of a given row
*/
void GTAGenericToolWidget::addCheckBoxIntoFuncArgsTW(int row, GTAGenericArgument arg)
{
    QCheckBox* valCB = new QCheckBox();
    valCB->setCheckState(Qt::Unchecked);
    arg.setUserSelectedValue("false");

    QString &userSetVal = arg.getUserSelectedValue();
    if (!userSetVal.isEmpty())
    {
        if (userSetVal == "true")
        {
            valCB->setCheckState(Qt::Checked);
            arg.setUserSelectedValue("true");
        }
    }

    _argChanged = true;
    valCB->setObjectName(arg.getArgumentName());
    valCB->setDisabled(arg.isArgConst());
    ui->FuncArgsTW->setCellWidget(row, 2, valCB);
    connect(valCB, SIGNAL(stateChanged(int)), this, SLOT(onValueCheckBoxStateChanged(int)));
}


/* Add combo box as values in the table widget
* @input: row - row to be updated
* @input: arg - GTAGenericArgument of a given row
*/
void GTAGenericToolWidget::addComboBoxIntoFuncArgsTW(int row, GTAGenericArgument arg)
{
    QComboBox* comboVals = new QComboBox();
    
    QString &argName = arg.getArgumentName();
    QString &argType = arg.getArgumentType();
    QStringList &vals = arg.getValues();

    QString &userSetVal = arg.getUserSelectedValue();
    QString& condition = arg.getCondition();
    size_t valueIdx;
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
                if (argType == "string" && !defVals.isEmpty())
                {
                    addSingleQuoteForEachElement(defVals);
                }

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
                if (argType == "string" && !valueList.isEmpty())
                {
                    addSingleQuoteForEachElement(valueList);
                }

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
                    if (argType == "string")
                    {
                        addSingleQuoteForEachElement(valueList);
                    }
                    arg.setUserSelectedValue(valueList.at(valueIdx));
                    comboVals->clear();
                    comboVals->addItems(valueList);
                    comboVals->setCurrentIndex(valueIdx);
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
            if (argType == "string" && !constVals.isEmpty())
            {
                addSingleQuoteForEachElement(constVals);
            }

            QStringList defaultVals = arg.getDefaultValue();
            if (argType == "string" && !defaultVals.isEmpty())
            {
                addSingleQuoteForEachElement(defaultVals);
            }

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
            if (argType == "string" && !constVals.isEmpty())
            {
                addSingleQuoteForEachElement(constVals);
            }
            QStringList defaultVals = arg.getDefaultValue();
            if (argType == "string" && !defaultVals.isEmpty())
            {
                addSingleQuoteForEachElement(defaultVals);
            }
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
 * @brief Adds combo box for selectring if string is a text or variable
*/
void GTAGenericToolWidget::addComboBoxIntoStringType(int row, GTAGenericArgument& arg)
{
    QComboBox* comboBox = new QComboBox();

    QString& argName = arg.getArgumentName();

    comboBox->setObjectName(argName);
    comboBox->setDisabled(false);
    comboBox->addItems(kStringTypes);
    arg._isStringVariable ? comboBox->setCurrentIndex(1) : comboBox->setCurrentIndex(0);
    ui->FuncArgsTW->setCellWidget(row, 1, comboBox);
    _ComboBox.insert(argName, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboStringTypeChanged(QString)));
}

/* Add text box as values in the table widget
* @input: row - row to be updated
* @input: arg - GTAGenericArgument of a given row
*/
void GTAGenericToolWidget::addTextBoxIntoFuncArgsTW(int row, GTAGenericArgument arg)
{
    QString &userSetVal = arg.getUserSelectedValue();
    QTableWidgetItem* itemVal = new QTableWidgetItem();
    QString &searchType = arg.getSearchType();
    QString& color = arg.getColor();
    QString& backColor = arg.getBackColor();
    QString &path = arg.getPath();

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
        font.setWeight(QFont::Bold);
        font.setStyle(QFont::StyleNormal);
        itemVal->setFont(font);
        itemVal->setText(userSetVal);
        itemVal->setForeground(QBrush(GTAUtil::getTextColor(color)));
        itemVal->setBackground(QBrush(GTAUtil::getBackGroundColorCellule(backColor)));
    }

    if (arg.isArgConst())
    {
        itemVal->setFlags(itemVal->flags() ^ Qt::ItemIsEditable);
    }
    ui->FuncArgsTW->setItem(row, 2, itemVal);
}


bool GTAGenericToolWidget::isOnCheckBox(GTAGenericArgument arg)
{
    QString &argType = arg.getArgumentType();
    return argType == "bool" || argType == "boolean";
}

bool GTAGenericToolWidget::isOnComboBox(GTAGenericArgument arg)
{
    return !isOnCheckBox(arg) && !arg.getValues().isEmpty();
}

bool GTAGenericToolWidget::isOnTextBox(GTAGenericArgument arg)
{
    return !isOnCheckBox(arg) && !isOnComboBox(arg);
}

/* Setup the table widget related to the selected function
* @input: index - index of the function
* @input: functionName - selected function name
*/
void GTAGenericToolWidget::setupFuncArgsTW(int index, const QString &functionName)
{
    if (index < 0 || index >= _functionList.count())
        return;

    GTAGenericFunction func = _functionList.at(index);
    if (func.getFunctionName() != functionName && func.getDisplayName() != functionName)
        return;

    // Init the table
    QList<GTAGenericArgument> args = func.getFunctionArguments();   
    int rowCnt = args.count();
    initFuncArgsTW(rowCnt);
    _argDependencyTable = func.getArgDependencyTable();
    if (rowCnt <= 0)
        return;

    bool argListChanged = false;
    for (int i = 0; i < rowCnt; i++)
    {
        _argChanged = false;

        // Set name and type
        GTAGenericArgument arg = args.at(i);
        QString& argName = arg.getArgumentName();
        QString& argType = arg.getArgumentType();
        setNameOrTypeItem(i, 0, argName);
        setNameOrTypeItem(i, 1, argType);

        // Set values
        if (isOnCheckBox(arg))	//Remove hardcoded values
        {
            addCheckBoxIntoFuncArgsTW(i, arg);
        }
        else
        {
            QStringList vals = arg.getValues();
            if (argType == kStringType) 
                //don't show string type cb for args with predefined values
                if (vals.isEmpty()) {
                    addComboBoxIntoStringType(i, arg);
                }
                else {
                    addSingleQuoteForEachElement(vals);
                }
            }


            if (isOnComboBox(arg))
            {
                addComboBoxIntoFuncArgsTW(i, arg);
            }
            else
            {
                addTextBoxIntoFuncArgsTW(i, arg);
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
    connectComboBox();
    connect(ui->FuncArgsTW, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onValueChanged(QTableWidgetItem*)));
}



/* This SLOT allow to update the functionName status and its arguments in ExternalTool action
* @input: item - QListWidgetItem
*/
void GTAGenericToolWidget::onFunctionNameSelected(QListWidgetItem *item)
{
    disconnect(ui->FuncArgsTW,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(onValueChanged(QTableWidgetItem *)));
	
    if (_functionList.isEmpty())
        return;
    
	// This condition is used to manage an exclusive checkBox state
	if (!_oldFunction.isEmpty())
	{
		QList<QListWidgetItem *> itemList;
		QListWidget* widgetList = item->listWidget();
		itemList = widgetList->findItems(_oldFunction, Qt::MatchExactly);
		// This condition avoids crashing GTA by changing the name of the ToolName
		if (!itemList.isEmpty())
		{
			// The functionName previously used will be unchecked unless it is a title
            itemList[0]->setCheckState(itemList[0]->textColor() == Qt::white ? Qt::PartiallyChecked :  Qt::Unchecked);
		}
	}

    // Swap state if not a title
    if (item->textColor() != Qt::white)
        item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
       
		
	// We need to keep in mind the functionName previously used
    QString functionName = item->text();
	_oldFunction = functionName;

    // Display the function table widget
    QModelIndex  idx = ui->FunctionListWidget->currentIndex();
    if (idx.isValid())
        setupFuncArgsTW(idx.row(), functionName);
}

void GTAGenericToolWidget::onSearchTypeClicked(int row, int col)
{

    if(col == 2)
    {

        disconnect(ui->FuncArgsTW,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onSearchTypeClicked(int,int)));
        QTableWidgetItem *pItem = ui->FuncArgsTW->item(row,col);        //extracting value cell

        if(pItem)
        {
            QString name = pItem->toolTip();
            if(!name.isEmpty())
            {
                QStringList searchType = name.split("-",QString::SkipEmptyParts);   //0 - arg Name 1 - searchType for that arg name

                if((searchType.at(1) == XNODE_FILE_SEARCH_TYPE) && (searchType.count() == 3))
                {

                    QString dirPath = searchType.at(2);
                    _filePath = QFileDialog::getOpenFileName(this,"Select a file",dirPath);
                    setSearchParameterValue(pItem,_filePath,QString(""));
                    return;
                }
                else if((searchType.at(1) == XNODE_FOLDER_SEARCH_TYPE) && (searchType.count() == 3))
                {
                    QString dirPath = searchType.at(2);
                    _folderPath = QFileDialog::getExistingDirectory(this,"Select a folder",dirPath);
                    setSearchParameterValue(pItem,_folderPath,QString(""));
                    return;
                }
                else if((searchType.at(1) == XNODE_USER_DEF_DB_SEARCH_TYPE) && (searchType.count() == 3))
                {
                    int i;
                    bool found = false;
                    GTAGenericDB dbObj;
                    QString userDbName = searchType.at(2);
                    for(i = 0; i < _UserDb.count();i++)
                    {
                        QString name = _UserDb.at(i).getName();
                        if(userDbName == name)
                        {
                            found = true;
                            dbObj = _UserDb.at(i);
                            break;
                        }
                    }
                    
                    if(_pSearchWidget && found) 
                    {
                        _pSearchWidget->setUserDefDb(dbObj);
                    }
                }
                if(_pSearchWidget)
                {
                     _searchItem.enqueue(row);
                    GTAGenSearchWidget::ElementType SearchType = _pSearchWidget->stringToSearchType(searchType.at(1));
                    _pSearchWidget->setSearchType(SearchType);
                    _pSearchWidget->show();
                }
            }
        }
        connect(ui->FuncArgsTW,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onSearchTypeClicked(int,int)));
    }
}

void GTAGenericToolWidget::onSearchCancelPBClick()
{
    if(!_searchItem.isEmpty())
    {
        _searchItem.dequeue();
    }
}

void GTAGenericToolWidget::onSearchOKPBClick()
{
    if(_pSearchWidget)
    {
        GTAICDParameter selectedItem = _pSearchWidget->getSelectedItems();
        if(!selectedItem.getName().isEmpty())
        {
            if(!_searchItem.isEmpty())
            {
                int row = _searchItem.dequeue();

                QString type;
                QTableWidgetItem *pItemType = ui->FuncArgsTW->item(row,1); //col 1 = type
                if(pItemType)
                     type = pItemType->text();

                QTableWidgetItem *pItem = ui->FuncArgsTW->item(row,2); //2 is column for values;Note: remove hardcoding
                QString selectedItemText = selectedItem.getName();
                QString selectedItemColor = selectedItem.getColor();
                QString selectedItemBackColor = selectedItem.getBackColor();
                onColorChanged(pItem, selectedItemColor, selectedItemBackColor);
                setSearchParameterValue(pItem,selectedItemText,type, selectedItemColor, selectedItemBackColor);
                connect(ui->FuncArgsTW,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onSearchTypeClicked(int,int)));

            }

        }
		_pSearchWidget->hide();
    }
}

/**
 * @brief The goal of this function is to complete cell of external tools with arguments find in database.
 * @param pItem current cell selected in the of external tools
 * @param iSelectedItem Name of the current item selected
 * @param iType identify the type of the cell selected.
 * @param iColor value of text foreground if there is an color to add. 
 * @param iBackColor value of cell background if there is an color to add.
 */
void GTAGenericToolWidget::setSearchParameterValue(QTableWidgetItem *pItem, QString iSelectedItem, const QString &iType, const QString &iColor, const QString &iBackColor)
{
    if(pItem == NULL)
        return;

    if(iType == "array")
    {
        iSelectedItem = QString("[%1]").arg(iSelectedItem);
    }
    else if(iType == "string" || iType.isEmpty())
    {
        iSelectedItem = QString("'%1'").arg(iSelectedItem);
    }

    pItem->setText("");
    QFont font;
    font.setWeight(QFont::Bold);
    font.setStyle(QFont::StyleItalic);
    pItem->setFont(font);
    pItem->setText(iSelectedItem);
    pItem->setForeground(QBrush(GTAUtil::getTextColor(iColor)));
    pItem->setBackground(QBrush(GTAUtil::getBackGroundColorCellule(iBackColor)));
    connect(ui->FuncArgsTW,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onSearchTypeClicked(int,int)));
}

/*
 * This function allows to modify all elements of the QStringList passed by reference by adding'em single quotes
 * input : value0, value1
 * output : 'value0', 'value1'
 */
void GTAGenericToolWidget::addSingleQuoteForEachElement(QStringList &qList)
{
    for(int i = 0; i < qList.size(); i++)
    {
        if(!qList[i].startsWith("'") && !qList[i].endsWith("'"))
        {
            qList[i] = QString("'%1'").arg(qList[i]);
        }
    }
}

void GTAGenericToolWidget::onValueCheckBoxStateChanged(int state)
{
    QString userSelVal;
    if(state == Qt::Unchecked)
        userSelVal = "false";
    else
        userSelVal = "true";

    QCheckBox *pCheckBox = dynamic_cast<QCheckBox*>(sender());
    if(pCheckBox)
    {
        QModelIndex idx = ui->FunctionListWidget->currentIndex();
        if(idx.isValid())
        {
            int functionRow = idx.row();
            if(functionRow >= 0 && functionRow < _functionList.count())
            {
                GTAGenericFunction func = _functionList.at(functionRow);
                QList<GTAGenericArgument> args = func.getFunctionArguments();
                GTAGenericArgument argument;
                bool found = false;
                int i;
                for(i = 0; i < args.count(); i++)
                {
                    argument = args.at(i);
                    if(argument.getArgumentName() == pCheckBox->objectName())
                    {
                        found = true;
                        break;
                    }
                }

                if(found)
                {
                    argument.setUserSelectedValue(userSelVal);
                    args.removeAt(i);
                    args.insert(i,argument);
                    func.setFunctionArguments(args);
                    _functionList.removeAt(functionRow);
                    _functionList.insert(functionRow,func);
                }
            }
        }
    }
}

void GTAGenericToolWidget::onComboValueChanged(QString comboValue)
{
    if(!comboValue.isEmpty())
    {
        QComboBox *pItem = dynamic_cast<QComboBox*>(sender());
        if(pItem)
        {
            QModelIndex idx = ui->FunctionListWidget->currentIndex();
            if(idx.isValid())
            {
                int functionRow = idx.row();
                if(functionRow >= 0 && functionRow < _functionList.count())
                {
                    GTAGenericFunction func = _functionList.at(functionRow);
                    QList<GTAGenericArgument> args = func.getFunctionArguments();
                    GTAGenericArgument argument;
                    bool found = false;
                    int i;
                    for(i = 0; i < args.count(); i++)
                    {
                        argument = args.at(i);
                        if(argument.getArgumentName() == pItem->objectName())
                        {
                            found = true;
                            break;
                        }
                    }
                    if(found)
                    {
                        if(!argument.isConditionValListEmpty())
                        {
                            QString condition = argument.getCondition();
                            if(!condition.isEmpty() && !_ComboBox.isEmpty())
                            {
                                QHash<QString,QComboBox*>::iterator itrCombo = _ComboBox.find(condition);
                                if(itrCombo != _ComboBox.end() && itrCombo.key() == condition)
                                {
                                    QComboBox *pItemCondition = itrCombo.value();
                                    if(pItemCondition)
                                    {
                                        int index = pItemCondition->currentIndex();

                                        QHash<int, QStringList> conditionVals = argument.getConditionValList();

                                        if(index > -1)
                                        {
                                            QHash<int,QStringList>::iterator itr = conditionVals.find(index);
                                            if(itr != conditionVals.end() && itr.key() == index)
                                            {
                                                argument.setUserSelectedHashIndex(itr.key());
                                            }
                                        }

                                    }
                                }
                            }
                        }

                        argument.setUserSelectedValue(comboValue);
                        args.removeAt(i);
                        args.insert(i,argument);
                        func.setFunctionArguments(args,!_isNewCmd);
                        _functionList.removeAt(functionRow);
                        _functionList.insert(functionRow,func);


                    }

                }
            }
        }
    }
}

/**
 * @brief Reacts to user changing string type, edits the values to match the type
          string text gets single quotes and string variables remove single quotes
*/
void GTAGenericToolWidget::onComboStringTypeChanged(const QString& comboValue) {
    if (!comboValue.isEmpty()) {
        QComboBox* pItem = dynamic_cast<QComboBox*>(sender());
        auto name = pItem->objectName();
        if (pItem) {
            QModelIndex idx = ui->FunctionListWidget->currentIndex();
            if (idx.isValid()) {
                int functionRow = idx.row();
                if (functionRow >= 0 && functionRow < _functionList.count()) {
                    GTAGenericFunction func = _functionList.at(functionRow);
                    QList<GTAGenericArgument> args = func.getFunctionArguments();
                    GTAGenericArgument argument;
                    bool found = false;
                    int argRow;
                    for (argRow = 0; argRow < args.count(); argRow++) {
                        argument = args.at(argRow);
                        auto name = argument.getArgumentName();
                        auto obj = pItem->objectName();
                        if (argument.getArgumentName() == pItem->objectName()) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        QString argValue = argument.getUserSelectedValue();
                        bool isStringVariable = comboValue == kStringVarType;
                        modifyStringToMatchStringType(argValue, isStringVariable);
                        argument.setUserSelectedValue(argValue);
                        argument._isStringVariable = isStringVariable;
     
                        args.removeAt(argRow);
                        args.insert(argRow, argument);
                        func.setFunctionArguments(args, !_isNewCmd);
                        _functionList.removeAt(functionRow);
                        _functionList.insert(functionRow, func);
                        auto cell = ui->FuncArgsTW->item(argRow, 2);
                        if (cell) {
                            cell->setText(argValue);
                        }

                    }
                }
            }
        }
    }
}

void GTAGenericToolWidget::modifyStringToMatchStringType(QString& argValue, bool isStringVariable) {
    if (!isStringVariable) {
        if (!(argValue.startsWith("'") && argValue.endsWith("'") || argValue.startsWith("\"") && argValue.endsWith("\""))) {
            argValue = "'" + argValue + "'";
            argValue.replace("\"", "'");
        }
    }
    else  {
        if (argValue.startsWith("'")) {
            argValue.remove(0, 1);
        }
        if (argValue.endsWith("'")) {
            argValue.chop(1);
        }
    }
}

void GTAGenericToolWidget::onValueChanged(QTableWidgetItem * pItem)
{
	if(pItem)
    {
        // Get the content of cell
        QString val = pItem->text();
        // Get the row number of the item
        int argRow = pItem->row();
        QModelIndex idx = ui->FunctionListWidget->currentIndex();
        if(idx.isValid())
        {
            int functionRow = idx.row();
            if(functionRow >= 0 && functionRow < _functionList.count())
            {
                GTAGenericFunction func = _functionList.at(functionRow);
                QList<GTAGenericArgument> args = func.getFunctionArguments();
                if(argRow >= 0 && argRow < args.count())
                {
                    GTAGenericArgument argument = args.at(argRow);
                    if (argument.getArgumentType() == "string") {
                        modifyStringToMatchStringType(val, argument._isStringVariable);
                        pItem->setText(val);
                    }

                    argument.setUserSelectedValue(val);
                    args.removeAt(argRow);
                    args.insert(argRow,argument);
                    func.setFunctionArguments(args);
                    _functionList.removeAt(functionRow);
                    _functionList.insert(functionRow,func);
                }
            }
        }
    }
}

/**
 * @brief this function which will be called by an emit signal, the goal is to be able to modify the color variable and also to set the value of the color in the GTAGenericArgument.
 * @param pItem of the external tool cell.
 * @param nameColor, name color that we should apply to foreground text and default background color to apply at the cell. 
 * @param nameBackColor name of background color that we should apply to background cell. 
 */
void GTAGenericToolWidget::onColorChanged(QTableWidgetItem* pItem, const QString& nameColor, const QString& nameBackColor) {
    int argRow = pItem->row();
    QModelIndex idx = ui->FunctionListWidget->currentIndex();
    
    int functionRow = idx.row();
    GTAGenericFunction func = _functionList.at(functionRow);
    QList<GTAGenericArgument> args = func.getFunctionArguments();
    if (argRow >= 0 && argRow < args.count())
    {
        for (int i = 0; i < args.count(); i++) {
            if (args.at(i).getArgumentName() == "color") {
                QString type;
                QTableWidgetItem* pItemType = ui->FuncArgsTW->item(i, 1);
                if (pItemType)
                    type = pItemType->text();
                QTableWidgetItem* pItem = ui->FuncArgsTW->item(i, 2);
                setSearchParameterValue(pItem, nameColor, type);
                GTAGenericArgument argumentColor = args.at(i);
                argumentColor.setUserSelectedValue(nameColor);
                args.removeAt(i);
                args.insert(i, argumentColor);
                func.setFunctionArguments(args);
                _functionList.removeAt(functionRow);
                _functionList.insert(functionRow, func);
            }
        }
        GTAGenericArgument argument = args.at(argRow);
        argument.setColor(nameColor);
        argument.setBackColor(nameBackColor);
        args.removeAt(argRow);
        args.insert(argRow, argument);
        func.setFunctionArguments(args);
        _functionList.removeAt(functionRow);
        _functionList.insert(functionRow, func);
    }
}


void GTAGenericToolWidget::updateUserSelectedValue()
{
    QModelIndex funcIdx = ui->FunctionListWidget->currentIndex();
    if (funcIdx.isValid())
    {
        int functionRow = funcIdx.row();
        if (functionRow >= 0 && functionRow < _functionList.count())
        {
            GTAGenericFunction function = _functionList.at(functionRow);
            QList<GTAGenericArgument> arguments = function.getFunctionArguments();
            GTAGenericArgument arg;
            for (int i = 0; i < arguments.count(); i++)
            {
                arg = arguments.at(i);
                QString v = arg.getUserSelectedValue();

                QString val;
                if (isOnCheckBox(arg))
                {
                    val = arg.getUserSelectedValue().isEmpty() ? "false" : arg.getUserSelectedValue();
                }
                if (isOnComboBox(arg))
                {
                    val = _ComboBox.find(arg.getArgumentName()).value()->currentText();
                }
                if (isOnTextBox(arg))
                {
                    val = arg.getUserSelectedValue();
                }


                arg.setUserSelectedValue(val);
                arguments.removeAt(i);
                arguments.insert(i, arg);
                function.setFunctionArguments(arguments);
                _functionList.removeAt(functionRow);
                _functionList.insert(functionRow, function);
            }

        }
    }
}


void GTAGenericToolWidget::connectComboBox()
{
    
    QHashIterator<QString, QString> itr(_argDependencyTable);
    if(!_ComboBox.isEmpty())
    {
        while(itr.hasNext())
        {
            itr.next();
            QHash<QString,QComboBox*>::iterator itrSenderObj = _ComboBox.find(itr.key());
            if(itrSenderObj.key() == itr.key())
            {
                QComboBox* pItem = itrSenderObj.value();
                if(pItem)
                {
                    connect(pItem, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
                }
            }
        }
    }
}

void GTAGenericToolWidget::onCurrentIndexChanged(int index)
{
    QComboBox *pItem = dynamic_cast<QComboBox*>(sender());
    {
        QString comboName = pItem->objectName();
        if(!_argDependencyTable.isEmpty())
        {
            QHash<QString,QString>::iterator itr = _argDependencyTable.find(comboName);
            if(itr.key() == comboName)
            {
                QString recvObj = itr.value();
                if(!_ComboBox.isEmpty())
                {
                    QHash<QString, QComboBox*>::iterator comboItr = _ComboBox.find(recvObj);
                    QComboBox *pComboItem = comboItr.value();
                    if(pComboItem)
                    {
                        QString name = pComboItem->objectName();
                        QModelIndex funcIdx = ui->FunctionListWidget->currentIndex();
                        if(funcIdx.isValid())
                        {
                            int functionRow = funcIdx.row();
                            if(functionRow >= 0 && functionRow < _functionList.count())
                            {
                                GTAGenericFunction function = _functionList.at(functionRow);
                                QList<GTAGenericArgument> arguments = function.getFunctionArguments();
                                GTAGenericArgument arg ;
                                bool found = false;
                                int i;
                                for(i = 0; i < arguments.count(); i++)
                                {
                                    arg = arguments.at(i);
                                    if(arg.getArgumentName() == name)
                                    {
                                        
                                        QHash<int, QStringList> conditionVals = arg.getConditionValList();
                                        if(!conditionVals.isEmpty())
                                        {
                                            
                                            QStringList defaultVals = arg.getDefaultValue();

                                            
                                            QHash<int,QStringList>::iterator itr = conditionVals.find(index);
                                            if(itr != conditionVals.end() && itr.key() == index)
                                            {
                                                QStringList vals = itr.value();
                                                pComboItem->clear();
                                                pComboItem->addItems(vals);
                                                arg.setUserSelectedHashIndex(itr.key());
                                                if(defaultVals.isEmpty())
                                                {
                                                    
                                                    arg.setUserSelectedValue(vals.at(0));
                                                }
                                                else
                                                {
                                                    if(index > - 1 && index < defaultVals.count())
                                                    {
                                                        QString valueToSet = defaultVals.at(index);
                                                        if(vals.contains(valueToSet))
                                                        {
                                                            int valIndex = vals.indexOf(valueToSet);
                                                            if(valIndex > -1 && valIndex < vals.count())
                                                            {
                                                                arg.setUserSelectedValue(valueToSet);
                                                                arg.setArgIsDefault(true);
                                                                pComboItem->setCurrentIndex(valIndex);
                                                            }
                                                        }
                                                    }
                                                }

                                                arguments.removeAt(i);
                                                arguments.insert(i,arg);
                                                function.setFunctionArguments(arguments);
                                                _functionList.removeAt(functionRow);
                                                _functionList.insert(functionRow,function);
                                            }
                                        }
                                        else
                                        {
                                            QStringList vals = arg.getValues();
                                            pComboItem->clear();
                                            pComboItem->addItems(vals);
                                            pComboItem->setCurrentIndex(index);
                                        }
                                    }
                                    else
                                    {
                                        QString name = arg.getArgumentName();
                                        QString type = arg.getArgumentType();
                                        QStringList arglist =  arg.getValues();

                                        QString val;
                                        if (isOnCheckBox(arg))
                                        {
                                            val = arg.getUserSelectedValue(); 
                                        }
                                        if (isOnComboBox(arg)) 
                                        {
                                            val = _ComboBox.find(arg.getArgumentName()).value()->currentText();
                                        }
                                        if (isOnTextBox(arg))
                                        {
                                            val = arg.getUserSelectedValue();
                                        }

                                        arg.setUserSelectedValue(val);
                                        arguments.removeAt(i);
                                        arguments.insert(i, arg);
                                        function.setFunctionArguments(arguments);
                                        _functionList.removeAt(functionRow);
                                        _functionList.insert(functionRow, function);
                                    }
                                }                              
                            }
                        }
                    }
                }
            }
        }
    }
}


void GTAGenericToolWidget::onSearchInputFilePBClicked()
{
    if(pAppController)
    {
        QString fileName =  QFileDialog::getOpenFileName(this,"Select Tool Configuration","","XML files(*.xml)");
        if(! fileName.isEmpty())
        {
            QFile toolInputFile(fileName);
            if(toolInputFile.exists())
            {
                ui->ToolConfigLE->setText(fileName);
                ui->FunctionListWidget->clear();
                ui->FuncArgsTW->clear();
                ui->FuncArgsTW->setRowCount(0);
                _inputFileFunctions.clear();

                _inputFileFunctions = pAppController->getInputFileFunctions(fileName);

                if(!_inputFileFunctions.isEmpty())
                {

                    if(postProcessFunctionList())
                    {
                        for(int i=0;i<_functionList.count();i++)
                        {
                            GTAGenericFunction func = _functionList.at(i);
                            QString functionName = func.getFunctionName();

                            QListWidgetItem *item = new QListWidgetItem(functionName);
                            item->setCheckState(Qt::Unchecked);
                            ui->FunctionListWidget->addItem(item);
                        }
                        connect(ui->FunctionListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onFunctionNameSelected(QListWidgetItem*)));
                        _isInputFileSelected = true;
                    }
                    else
                    {
                        restoreDefaults();
                    }

                }
                else
                {
                    _isInputFileSelected = false;
                }

            }

        }
    }

}

bool GTAGenericToolWidget::postProcessFunctionList()
{
    bool changed;
    QList<GTAGenericFunction> tempFuncList;
    for(int i = 0; i < _inputFileFunctions.count(); i++)
    {
        GTAGenericFunction inputFunction = _inputFileFunctions.at(i);
        if(_functionList.contains(inputFunction))
        {
            int index = _functionList.indexOf(inputFunction);
            if(index > -1 && index < _functionList.count())
            {
                GTAGenericFunction function =  _functionList.at(index);

                QList<GTAGenericArgument> args =  function.getFunctionArguments();
                QList<GTAGenericArgument> inputArgs =  inputFunction.getFunctionArguments();
                QList<GTAGenericArgument> tempArgs;
                changed = false;
                for(int j = 0; j < inputArgs.count(); j++)
                {
                    GTAGenericArgument inputArg = inputArgs.at(j);
                    if(args.contains(inputArg))
                    {
                        int argIndex = args.indexOf(inputArg);
                        if(argIndex > -1 && argIndex < args.count())
                        {
                            GTAGenericArgument argument = args.at(argIndex);
                            QStringList inputValList = inputArg.getValues();
                            QString inputValue = inputValList.at(0);

                            QStringList argumentValList = argument.getValues();
                            if(!argumentValList.isEmpty())
                            {
                                if(! argument.isConditionValListEmpty())
                                {
                                    QHash<int,QStringList> conditionVal = argument.getConditionValList();
                                    QHashIterator<int,QStringList> Itr(conditionVal);
                                    bool found = false;
                                    while(Itr.hasNext())
                                    {
                                        Itr.next();

                                        QStringList checkList = Itr.value();
                                        if(checkList.contains(inputValue))
                                        {
                                            argument.setUserSelectedValue(inputValue);
                                            argument.setUserSelectedHashIndex(Itr.key());
                                            tempArgs.append(argument);
                                            found = true;
                                            changed = true;
                                            break;
                                        }
                                    }
                                    if(!found)
                                    {
                                        //Add compatibility code here
                                    }
                                }
                                else
                                {
                                    if(argumentValList.contains(inputValue))
                                    {
                                        argument.setUserSelectedValue(inputValue);
                                        tempArgs.append(argument);
                                        changed = true;
                                    }
                                    else
                                    {
                                        //Add compatibility code here
                                    }
                                }
                            }
                            else
                            {
                                argument.setUserSelectedValue(inputValue);
                                tempArgs.append(argument);
                                changed = true;
                            }
                        }

                    }

                }
                if(changed)
                {
                    function.setFunctionArguments(tempArgs);
                    tempFuncList.append(function);

                }
            }
        }
    }
    if(changed)
    {
        _functionList.clear();
        _functionList = tempFuncList;

    }
    return changed;
}

void GTAGenericToolWidget::restoreDefaults()
{
    ui->FunctionListWidget->clear();
    ui->FuncArgsTW->clear();
    ui->FuncArgsTW->setRowCount(0);

    _functionList.clear();
    _inputFileFunctions.clear();
    _functionList = _MasterFunctionList;

    if(!_functionList.isEmpty())
    {
        for(int i=0;i<_functionList.count();i++)
        {
            GTAGenericFunction func = _functionList.at(i);
            QString functionName = func.getFunctionName();

            QListWidgetItem *item = new QListWidgetItem(functionName);
            item->setCheckState(Qt::Unchecked);
            ui->FunctionListWidget->addItem(item);
        }
        connect(ui->FunctionListWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onFunctionNameSelected(QListWidgetItem*)));
    }

}

bool GTAGenericToolWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    for (auto& param : params)
    {
        if (param.first == "toolNameCB")
        {
            auto idx = ui->toolNameCB->findText(param.second);
            if(idx>=0) ui->toolNameCB->setCurrentIndex(idx);
            else return false;
            auto currentIdx = ui->toolNameCB->currentIndex();
        }
        else if (param.first == "FunctionListWidget")
        {
            for (int i = 0; i < ui->FunctionListWidget->count(); i++) {
                auto item = ui->FunctionListWidget->item(i);
                QString text = item->text();
                if (text == "SetRelay")
                {
                    ui->FunctionListWidget->setCurrentRow(i);
                    ui->FunctionListWidget->setItemSelected(item, true);
                    ui->FunctionListWidget->itemClicked(item);
                    break;
                }
            }
        }
        else if (param.first == "FuncArgsTWIndex")
        {
            QTableWidgetItem* itemVal = new QTableWidgetItem();
            itemVal->setText(param.second);
            ui->FuncArgsTW->setItem(1, 2, itemVal);
        }
        else if (param.first == "FuncArgsTWEtat")
        {
            QTableWidgetItem* itemVal = new QTableWidgetItem();
            itemVal->setText(param.second);
            ui->FuncArgsTW->setItem(2, 2, itemVal);
        }
    }
    
    return true;
}



#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(ACT_GEN_TOOL,GTAGenericToolWidget,obj)

