#ifndef GTAACTIONONECLICKSETWIDGET_H
#define GTAACTIONONECLICKSETWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include "GTAActionOneClickSetList.h"
#include "GTAActionOneClickSet.h"
#include "GTAActionWidgetInterface.h"
#include <QListWidget>

class GTAEquationBase;
class GTAEquationWidget;
namespace Ui {
    class GTAActionOneClickSetWidget;
}

class GTAActionOneClickSetValueLineEdit : public QLineEdit
{
    Q_OBJECT
public:
        GTAActionOneClickSetValueLineEdit(QWidget* parent=0);
signals:
        void keyPressedForValueLE(QString);
protected:
      void keyPressEvent(QKeyEvent *);
};

class GTAActionOneClickSetWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionOneClickSetWidget(QWidget *parent = 0);
    ~GTAActionOneClickSetWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;
     int getSearchType();

    bool hasSearchItem(){return true;}
    bool hasTimeOut(int& oValidation){oValidation = NUMBER|NON_NEGATIVE;return true;}
    bool hasActionOnFail(){return true;}

    QString getVmacForceType()const;
    QList<GTAActionOneClickSet*> _SetCommandList;
    QList<QWidget*> getWidgetsInTabOrder()const;
    QWidget* getLastWidgetForTabOrder()const;




private:

    GTAActionOneClickSet             * _pSetActionCmd;
    Ui::GTAActionOneClickSetWidget   *ui;
    GTAEquationWidget        * _pEquationWgt;

    GTAEquationBase *         _pEquation;
    QString                      _EquationSimu;
    QString                      _vMacForceType;
   // GTAActionSetList  *      _pSetCmd;

    void fillExpLWFromCSV(QListWidget* , const QString& iFilePath);
    bool validateRowFromCSV(const QString& iLine);
    void silentSetValueText(const QString& iString);
    void silentSetFSCB();
    void silentReSetFSCB();
    QStringList _sFSList;

    


private slots:
    void onEquationButtonClick();
    void onEquationWgtOKButtonClick();


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
    void disableFSCB( const QString & );
};

#endif // GTAActionOneClickSetWidget_H
