#ifndef GTAActionMathWidget_H
#define GTAActionMathWidget_H
#include "GTAActionWidgetInterface.h"
#include "GTAActionMath.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)


namespace Ui {
    class GTAActionMathWidget;
}

class GTAActionMathWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionMathWidget(QWidget *parent = 0);
    ~GTAActionMathWidget();
    void setComplement(const QString &compVal);

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;
    bool hasSearchItem(){return true;}
    int getSearchType();


    
private slots:
        void onSearchPBClicked();
        void changeNoteText(QString);

private:
    Ui::GTAActionMathWidget *ui;
    QString _isType;
};

#endif // GTAActionMathWidget_H
