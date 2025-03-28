#ifndef GTAActionReleaseWidget_H
#define GTAActionReleaseWidget_H

#include <QWidget>
#include <QLineEdit>
#include "GTAActionWidgetInterface.h"
#include "GTAGenSearchWidget.h"
#include <QModelIndex>
#include "GTAActionRelease.h"

namespace Ui {
    class GTAActionReleaseWidget;
}

class GTAActionReleaseWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionReleaseWidget(QWidget *parent = 0);
    ~GTAActionReleaseWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}


//signals:
//      void searchObject(QLineEdit *&);
private:
    Ui::GTAActionReleaseWidget *ui;
    GTAGenSearchWidget* _pSearchParameter;
    QStringList getParameterList() const;
private slots:
    void onAddPBClicked();
    void onRemovePBClicked();
    void onParamSelectionChanged(const QModelIndex & iIndex);
    void onSearchPBClicked();
    void onSearchWidgetOKClicked();
};

#endif // GTAActionReleaseWidget_H
