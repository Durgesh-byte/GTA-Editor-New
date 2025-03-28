#ifndef GTAACTIONTITLEWIDGET_H
#define GTAACTIONTITLEWIDGET_H
#include "GTAActionTitle.h"
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

namespace Ui {
class GTAActionTitleWidget;
}

class GTAActionTitleWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionTitleWidget(QWidget *parent = 0);
    ~GTAActionTitleWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid()const ;
    bool hasSearchItem(){return true;}
    bool hasExternalComment()const{return false;}
    int getSearchType();
    bool setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs) override final;



private slots:
    void onSearchPBClicked();
    void validateText();
private:

    Ui::GTAActionTitleWidget *ui;
};

#endif // GTAACTIONTITLEWIDGET_H
