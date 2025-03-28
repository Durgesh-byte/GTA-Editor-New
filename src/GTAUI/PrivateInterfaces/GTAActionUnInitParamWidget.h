#ifndef GTAActionInitParamWidget_H
#define GTAActionInitParamWidget_H

#include <QWidget>
#include "GTAActionWidgetInterface.h"
#include <QLineEdit>
#include <QStringList>

class GTAActionUnSubscribe;
namespace Ui {
    class GTAActionUnInitParamWidget ;
}

class GTAActionUnInitParamWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionUnInitParamWidget(QWidget *parent = 0);
    ~GTAActionUnInitParamWidget();

    void setValue(const QString & iVal) ;

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}
    int getSearchType();
    void processSearchInput(const QStringList &iSearchItemList);
    void processSearchInput(const QList<GTAICDParameter> &iSearchItemList);
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs);

signals:
    void searchObject(QLineEdit *&);

private:
    Ui::GTAActionUnInitParamWidget *ui;
    QStringList _paramList;
    QList<GTAICDParameter> _benchParamList;

private:

    QStringList getValues() const;

private slots:
    void onSearchPBClicked();
    void addToParamList();
    void removeFromParamList();
    void editFromParamList();
    void updateParamLEFromLst();
    void onReleaseAllClicked(bool iVal);
};

#endif // GTAActionInitParamWidget_H
