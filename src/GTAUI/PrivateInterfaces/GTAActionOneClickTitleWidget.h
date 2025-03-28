#ifndef GTAActionOneClickTitleWidget_H
#define GTAActionOneClickTitleWidget_H
#include "GTAActionOneClickTitle.h"
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)


namespace Ui {
class GTAActionOneClickTitleWidget;
}

class GTAActionOneClickTitleWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionOneClickTitleWidget(QWidget *parent = 0);
    ~GTAActionOneClickTitleWidget();


    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;
    int getSearchType();
    bool hasExternalComment()const{return false;}

    
signals:
    void searchImage(QLineEdit *&);
private slots:
    void onSearchPBClicked();
    void validateText();
private:

    Ui::GTAActionOneClickTitleWidget *ui;
};

#endif // GTAActionOneClickTitleWidget_H
