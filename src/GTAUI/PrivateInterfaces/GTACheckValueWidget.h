#ifndef GTACHECKVALUEWIDGET_H
#define GTACHECKVALUEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QHash>
#include <QListWidgetItem>
#include "GTACheckWidgetInterface.h"
namespace Ui {
class GTACheckValueWidget;
}
class GTACheckValue;

class GTACheckValueWidget : public GTACheckWidgetInterface
{
    Q_OBJECT

public:
    explicit GTACheckValueWidget(QWidget *parent = 0);
    ~GTACheckValueWidget();

    virtual void clear();
    virtual bool isValid();
    virtual bool getCommand(GTACheckBase *& pCheckCmd);
    virtual bool setCommand(const GTACheckBase * pCheckCmd);
    void setPrecisionValue(const QString &iPrecision);
    void setAutoCompleteItems(const QStringList & iWordList);
    QString getParity()const;
    QString getSDI()const;
    bool getCheckOnlyRefreshRateState()const;
    bool getCheckOnlySDIState()const;
    bool getCheckOnlyParityState()const;
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;
    QString getFunctionalStatus();
    QString getOperator();
    void selectLastEditedRow();

signals:
    void searchParameter(QLineEdit *&);
    void setTextColorOfQueryEditor(const QColor & c);
    void evaluateExpression();

    void waitUntilCheckStatus(bool iStatus);


private:
    Ui::GTACheckValueWidget *ui;
    bool _isParameterLE;
    QHash<QPushButton*,QLineEdit *> _SearchButtonEditorMap;
	QHash<QPushButton*, QComboBox *> _SearchButtonCBMap;
    QStringList _OperList;
    QStringList _sFSList;

    int _lastEditedRow = 0;
    int _lastEditedCol = 0;

    void setCheckCommand( GTACheckValue * ipCheckCmd) ;
    GTACheckValue * getCheckCommand();

    //resets all editable widgets to default value
   // void resetWidget();

    /** This function checks if a query is valid. Returns false for nvalid statement

  Each line in of query editor is treated as one statement.
  Each statement is validated using following conditions
   -it can have only one conditional operator (!=,>=,<=>,<,=)
   -each condition is binary operator therefore only two operands are allowed
    they should not be seperated by spaces. [ "var1!=  var2"  => valid]
              [ "var1!= var iable"  => invalid]
   -spaces can exist between operand and condition [ "var1      =    var2" =>valid]


 */
    bool isValidExpression(const QString& sExpression);
    void fillExpLWFromCSV(QListWidget* , const QString& iFilePath);
    bool validateRowFromCSV(const QString& iLine);
    void extractCheckItemsFromCheckExpression(QString iChkExp,
                                                 QString& osParameter,
                                                 QString& osConditionalExp,
                                                 QString& osValue,
                                                 QString& osFunctionalStatus,
                                                 double& oPrecisonVal,
												 QString& oPrecisonValFE,
                                                 QString& osPrecisonType,
                                                 bool& isChekOnlyValue,
                                                 bool& isCheckFs,
                                                 bool &isSDI,
                                                 bool &isParity,
                                                 bool &isRefreshRate,
                                                 QString &iSDIVal,
                                                 QString &iParity);

    void silentSetChkValueOnlyCB();
    void silentReSetChkOnlyValueCB();
    void silentResetChkOnlyFSCB();
    void silentUpdateParam1LE(const QString& iString);
	void setOnlyValueStatus();
	void resetAllCheckBoxStatus();

    bool getParamValuePairForChannel(QString iParamName, QString iParamValue,QList<QPair<QString,QString> >& olstParamValPair);
    
    bool hasSearchItem()const ;
    bool hasActionOnFail()const ;
    bool hasPrecision()const;
    bool hasTimeOut()const ;
    bool hasDumpList()const;
    bool hasConfigTime() const ;
    
private slots:
    void onSearchPBClicked();
    void addAndListExpression();
    void addOrListExpression();
    void valuateExpression();
    void deleteExpression();
    void togglePrecision(QString);
    void resetPrecision(QString);
    /*
  this functin converts the input string into html format for display in editor
  AND/OR operators are in bold.
 */
    bool convertToHtml( QString& ipString);
    bool isValidParameter(QString& ioErrorMsg);
    void clearParameterListSelection();
    void updateParameterLineEdits();
    void updateParameterValue();
    void waitUntilCheckStatusSlot(bool iStatus);
    void importParametersFromCSV();
    void disableValueEditsOnRefresh(bool iStatus);
    void disableValueEdits(bool iStatus);
    void onListItemClicked( QListWidgetItem *);
    void evaluateParamMedia( const QString & );
    void resetCheckFS(bool iStatus);
    void resetCheckOnlyValue(bool iStatus);
    void hideNote(bool );
    void enableValueEdits(bool );
    void resetSDI(bool iStatus);
    void resetParity(bool iStatus);
    void resetRefreshRate(bool iStatus);
};

#endif // GTACHECKVALUEWIDGET_H
