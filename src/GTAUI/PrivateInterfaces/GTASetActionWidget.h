#ifndef GTASETACTIONWIDGET_H
#define GTASETACTIONWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include "GTAActionSetList.h"
#include "GTAActionWidgetInterface.h"
#include "GTAMcduWidget.h"

class GTAEquationBase;
class GTAEquationWidget;
namespace Ui {
    class GTASetActionWidget;
}

class GTAActionSetValueLineEdit : public QLineEdit
{
    Q_OBJECT
public:
        GTAActionSetValueLineEdit(QWidget* parent=0);
signals:
        void keyPressedForValueLE(QString);
protected:
      void keyPressEvent(QKeyEvent *);
};

class GTASetActionWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTASetActionWidget(QWidget *parent = 0);
    ~GTASetActionWidget();

    void setActionCommand(const GTAActionBase* ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const;
    bool hasSearchItem(){return true;}
    bool hasActionOnFail(){return true;}
    bool hasTimeOut(int& oValidation){oValidation = NUMBER|NON_NEGATIVE;return true;}
    bool hasCallInput() { return true; }
 
    QString getVmacForceType()const;
    QList<GTAActionSet*> _SetCommandList;
    QList<QWidget*> getWidgetsInTabOrder()const;
    QWidget* getLastWidgetForTabOrder()const;
    int getSearchType();
	QStringList _forbiddenKeywords;
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;
    void selectLastEditedRow();


signals:
	void evaluateExpression();

private:

    GTAActionSet             * _pSetActionCmd;
    Ui::GTASetActionWidget   *ui;
    GTAEquationWidget        * _pEquationWgt;
    GTAMcduWidget            * _pMcduWgt;

    GTAEquationBase *         _pEquation;
    QString                      _EquationSimu;
    QString                      _vMacForceType;
	bool						 _paramWithoutFS;
   // GTAActionSetList  *      _pSetCmd;

    QString _currentParameterName;
    QString _currentParameterValue;
    bool _newParameterToBeAdded;

    void fillExpLWFromCSV(QListWidget* , const QString& iFilePath);
    bool validateRowFromCSV(const QString& iLine);
    void silentSetValueText(const QString& iString);
    void silentSetFSCB();
    void silentReSetFSCB();
    void silentSetFSValue(); 
    void silentReSetFSValue();
    QStringList _sFSList;
    bool addSetCommand(QString isParam, GTAEquationBase* ipBase);
    void silentSetSDICB();
    void silentReSetSDICB();
    void silentReSetValueCB();
    void silentSetValueCB();
	void modifyContentFSCB(bool iStatus);
	// Inline function to configure if the parameter has a FunctionalStatus or not
	inline bool getParamWithoutFS() const { return _paramWithoutFS; }
	inline void setParamWithoutFS(bool iState) { _paramWithoutFS = iState; }

    int _lastEditedRow = 0;


private slots:
    void onEquationButtonClick();
    void onEquationWgtOKButtonClick();
    void onMCDUButtonClicked();
    void onMCDUWgtOKButtonClick();

    void onSearchPBClicked();
    void onParameterValueEdited(const QString & iValue);
    void onRemoveSetCommand();
    void updateParamValueLineEdits();
    void editSetCommand();
    void onAddSetCommand();
    void updateVmacForceType(int);
    void onCancel();
    void importParametersFromCSV();
    bool isParameterFromDB();
    void disableLineEdits(bool iStatus);
    void disableLineEditsTable(bool iStatus);
    void disableFSCB( const QString&);
    void onSetSDIToggled(bool iStatus);
    void setFSDisabled(bool iStatus);
    void onChangeParameterValue(QString iStatus);
	void setOnlyValueStatus();
	void resetAllCheckBoxStatus();
	void evaluateParamMedia(const QString &);
	void onTimeCBToggled(bool iStatus);

};

#endif // GTASETACTIONWIDGET_H
