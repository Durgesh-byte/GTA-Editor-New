#ifndef GTAActionOneClickLaunchAppWidget_H
#define GTAActionOneClickLaunchAppWidget_H

#include <QWidget>
#include "GTAActionWidgetInterface.h"
#include <QLineEdit>
#include <QHash>
#include "GTAOneClickLaunchApplicationApplications.h"
#include "GTAOneClickLaunchApplicationOptions.h"
#include "GTAOneClickLaunchApplicationTestRig.h"
#include <QGridLayout>
#include "GTAOneClickLaunchApplication.h"


class GTAActionPrint;
namespace Ui {
    class GTAActionOneClickLaunchAppWidget;
}

struct structAppArgument
{
    QString _appName;
    QStringList _lstOfArguments;
};
class GTAActionOneClickLaunchAppWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionOneClickLaunchAppWidget(QWidget *parent = 0);
    ~GTAActionOneClickLaunchAppWidget();

    QStringList getValues() const;
    void setValue(const QString & iVal) ;
   
    


    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;
    bool hasSearchItem(){return true;}
    bool hasActionOnFail(){return true;}
    int getSearchType();

signals:
    void searchObject(QLineEdit *&);
private:
    GTAOneClickLaunchApplicationValue getValuebyName(const QString &,const QString &);
    void clearDynamicUIWidget(bool bClearOneClickSturcts=false);
    bool applicationExists(const QString&);

    QStringList getApplicationNames(const QString &);
    int getIndexOfApplicaton(const QString& iAppName);

    void deleteAndClearItemStructs();
    Ui::GTAActionOneClickLaunchAppWidget *ui;

    // QStringList _FileAppList;
    //QHash<QString , QStringList> _hshOfAppArgument;
    QList<structAppArgument> _lstOfApplications;

   GTAOneClickLaunchApplicationApplications _Applications;
    GTAOneClickLaunchApplicationTestRig _TestRig;
    GTAOneClickLaunchApplicationArgs _Arguments;
    GTAOneClickLaunchApplicationValue _Value; /**/
    QList<GTAOneClickLaunchApplicationOptions> _Options;


    QList<GTAOneClickLaunchApplicationItemStruct *> _LAItemStructs;


    QGridLayout * _pDynamicGL;
    QWidget *_pWidget;
    QList<QWidget *> _widgetList;

private slots:
    void onKillModeCBToggeled(int );
    void onSearchPBClicked();
    void addToApplicationList();
    void removeApplication();
    void editFromParamList();
    void updateParamLEFromLst( );
    void updateApplicatoinCB(const QString & iConfigName);
    void updateApplicationViewList();
    void onStartApplicationCBClicked(int cbState);
    void updateOptionList(QString iAppName);
    void onOptionCBValueChanged(QString iOptionVal);
};

#endif // GTAActionOneClickLaunchAppWidget_H
