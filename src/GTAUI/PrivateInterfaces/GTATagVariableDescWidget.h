#ifndef GTATAGVARIABLEDESCWIDGET_H
#define GTATAGVARIABLEDESCWIDGET_H

#include <QWidget>
#include "GTAWidget.h"
#include "GTAElement.h"

namespace Ui {
class GTATagVariableDescWidget;
}

class GTATagVariableDescWidget : public GTAWidget
{
    Q_OBJECT

public:
    explicit GTATagVariableDescWidget(QString ielementPath,GTAWidget *parent = 0);
    ~GTATagVariableDescWidget();

    void showWidget(QList<TagVariablesDesc> tagList);

private:
    Ui::GTATagVariableDescWidget *ui;
    QString _elementPath;

private slots:

    /**
      * This function saves the updated tag list with user input description details.
      */
    void onOKPBClicked();

signals:
    void sendUserInput(QString iFileName, QList<TagVariablesDesc> tagList);

};

#endif // GTATAGVARIABLEDESCWIDGET_H
