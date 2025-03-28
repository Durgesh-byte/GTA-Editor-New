#ifndef GTAGENERICTOOLWIDGET_H
#define GTAGENERICTOOLWIDGET_H

#include <QWidget>
//#include "GTAGenericToolData.h"
#include "GTAActionWidgetInterface.h"
#include "GTAAppController.h"
#include "GTAGenericFunction.h"
#include <QListWidgetItem>
#include "GTAGenericToolCommand.h"
#include <QComboBox>
#include <QTableWidgetItem>
#include <QTableWidget>
#include "GTAGenSearchWidget.h"
#include <QFileDialog>
#include <QFile>

#include <QDir>
#include <QQueue>
#include <QCheckBox>
#include "GTAActionSelectorWidget.h"

namespace Ui {
    class GTAGenericToolWidget;
}

class GTAGenericToolWidget : public GTAActionWidgetInterface
{
    Q_OBJECT
public:
    explicit GTAGenericToolWidget(QWidget* parent = 0);
    ~GTAGenericToolWidget();

    void clear();
    bool isValid()const;
    void setActionCommand(const GTAActionBase* ipActionCmd);
    bool getActionCommand(GTAActionBase*& pCmd)const;
    virtual bool hasActionOnFail();
    virtual bool hasTimeOut(int&);
    virtual bool hasDumpList();
    virtual bool hasExternalComment()const;
    virtual void connectExternalItemsWithLocalWidget(QWidget* pParentWidget = NULL);

signals:
    void onReEvaluateShowItems(bool iActionOnFail, bool iPrecesion, bool iTimeOut, bool iConfTime, bool iDumpList,bool iExternalComment);
private:

    void setupFuncArgsTW(int index, const QString& functionName);
    void initFuncArgsTW(int rowCnt);
    void setNameOrTypeItem(int row, int col, const QString& argument);

    bool isOnCheckBox(GTAGenericArgument arg);
    void addCheckBoxIntoFuncArgsTW(int row, GTAGenericArgument arg);

    bool isOnComboBox(GTAGenericArgument arg);
    void addComboBoxIntoFuncArgsTW(int row, GTAGenericArgument arg);
    void addComboBoxIntoStringType(int row, GTAGenericArgument& arg);

    bool isOnTextBox(GTAGenericArgument arg);
    void addTextBoxIntoFuncArgsTW(int row, GTAGenericArgument arg);

    void modifyStringToMatchStringType(QString& argValue, bool isStringVariable);

    const QString kStringType      = "string";
    const QString kStringVarType   = "string variable";
    const QStringList kStringTypes = { kStringType , kStringVarType };

    Ui::GTAGenericToolWidget *ui;
    GTAAppController *pAppController;
    QList<GTAGenericFunction> _functionList;
    QList<GTAGenericDB> _UserDb;
    QList<GTAGenericParamData> _definitions;
    QList<GTAGenericFunction> _SelectedFunctions;
    QHash<QString,QComboBox*> _ComboBox;
    QHash<QString, QString> _argDependencyTable;
    QHash<QString, QString> _allToolNamesHash;
    GTAGenSearchWidget*_pSearchWidget;
    QList<GTAGenericFunction> _inputFileFunctions;
    QList<GTAGenericFunction> _MasterFunctionList;
    QQueue<int> _searchItem;
    QString _filePath;
    QString _folderPath;
	QString _oldFunction;
    bool _isInputFileSelected;
    bool _isMismatched;
    bool _isNewCmd;
    bool _argChanged = false;

    void connectComboBox();
    void updateUserSelectedValue();
    bool postProcessFunctionList();
    void setSearchParameterValue(QTableWidgetItem *pItem, QString iSelectedItem, const QString &iType = QString(""), const QString &iColor = QString(""), const QString &iBackColor = QString(""));
    void addSingleQuoteForEachElement(QStringList&);
	
    void restoreDefaults();
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;

    void onColorChanged(QTableWidgetItem*, const QString&, const QString&);

private slots:
    void OnToolNameChanged(const QString &iToolNameChanged);

    void onFunctionNameSelected(QListWidgetItem *item);
   // void onFunctionSelected(QListWidgetItem* item);
    void onCurrentIndexChanged(int index);
    void onValueChanged(QTableWidgetItem *pItem);
    void onValueCheckBoxStateChanged(int state);
    void onComboValueChanged(QString comboValue);
    void onComboStringTypeChanged(const QString& comboValue);// , GTAGenericArgument& arg);
    void onSearchTypeClicked(int row, int col);
    void onSearchInputFilePBClicked();
    void onSearchOKPBClick();
    void onSearchCancelPBClick();
    void onExistingConfigCBStateChanged(int state);
};

#endif // GTAGENERICTOOLWIDGET_H



