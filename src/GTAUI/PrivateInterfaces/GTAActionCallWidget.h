#ifndef GTAACTIONCALLWIDGET_H
#define GTAACTIONCALLWIDGET_H

#pragma warning (push, 0)
#include <QWidget>
#include <QLineEdit>
#include "GTAActionWidgetInterface.h"
#include <QTableWidgetItem>
#pragma (pop) //QT libs not separated because of circular dependency issues

#include <GTAGenSearchWidget.h>
class GTAActionCall;
namespace Ui {
class GTAActionCallWidget;
}

class GTAActionCallWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionCallWidget(QWidget *parent = 0);
    ~GTAActionCallWidget();


    void setActionCommand(const GTAActionBase* ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const;
    bool hasSearchItem(){return true;}
    //bool hasTimeOut(int& oValidation){oValidation = NUMBER|NON_NEGATIVE;return false;}
    int getSearchType();

    void setValue(const QString & iVal) ;
    QString getValue() const;
    void addTag(const TagVariablesDesc& iTagVar,const QString& iTagVal=QString());
    
    void setParamLabel(const QString& iComplementValue);
    void setComplement(const QString& iComplement);

    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;

signals:
    void searchObject(QLineEdit *&);
    void searchCallParam(QTableWidgetItem *&);
private:
    void setTagTableVisibility(bool iVal = true);

    QString _complement;
    QString _RefFileUUID;
    Ui::GTAActionCallWidget* ui;
    GTAGenSearchWidget * _pSearchWidget;
    GTAGenSearchWidget::ElementType _CurrSearchType;
    QLineEdit * _SearchTagLineEdit;

public slots:
        void onSearchOKPBClick();
private slots:
    void onSearchPBClicked();
    void setTagTable(QString);
    void resetTableWidget();
    void onTagTableSearchClicked(int row,int col);
    void setComplementFields(const QString& name);
    
    //void set
};

#endif // GTAACTIONCALLWIDGET_H


