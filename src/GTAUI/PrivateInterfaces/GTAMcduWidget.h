#ifndef GTAMCDUWIDGET_H
#define GTAMCDUWIDGET_H

#include <QDialog>
#include <QKeyEvent>
#include "GTAAppController.h"
#include "GTAGenericFunction.h"
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QQueue>
#include <QComboBox>
#include <QCheckBox>

namespace Ui {
class GTAMcduWidget;
}

class GTAMcduWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GTAMcduWidget(QWidget *parent = 0);
    ~GTAMcduWidget();
    void displayFunctions();
    QString getUserSelection();
    void setUserSelection(const QString s); 

signals:
    void okPressed();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::GTAMcduWidget *ui;
    GTAAppController* pAppController;
    QList<GTAGenericFunction> _functionList;
    QList<GTAGenericParamData> _definitions;
    QHash<QString, QComboBox*> _ComboBox;
    QString _oldFunction;
    QString _userSelection;
    QQueue<int> _searchItem;
    bool _argChanged = false;
    bool _isNewCmd = true;

    std::map<QString, int> _keyboardToMcdu;
    std::map<int, QString> _McduToKeyboard;
    void defineCharTable();
    
    void setupFuncArgsTW(int index, const QString& functionName);
    void initFuncArgsTW(int rowCnt);
    void updateUserSelectedValue();
    void convertAlphabeticalMessageToMCDUCode(QString &txt);
    void convertMCDUCodeToAlphabeticalMessage(QString& txt);
    void parseUserSelection(GTAGenericFunction &func);
    
    bool isOnCheckBox(GTAGenericArgument arg);
    void addCheckBoxIntoFuncArgsTW(int row, GTAGenericArgument arg);
    bool isOnComboBox(GTAGenericArgument arg);
    void addComboBoxIntoFuncArgsTW(int row, GTAGenericArgument arg);
    bool isOnTextBox(GTAGenericArgument arg);
    void addTextBoxIntoFuncArgsTW(int row, GTAGenericArgument arg);



private slots:
    void onOKButtonClick();
    void onCancelButtonClick();
    void onFunctionNameSelected(QListWidgetItem* item);
    void onValueChanged(QTableWidgetItem* pItem);
    void onValueCheckBoxStateChanged(int state);
};

#endif // GTAMCDUWIDGET_H
