#ifndef GTAChannelSelectorWidget_H
#define GTAChannelSelectorWidget_H

#include <QDialog>
#include "GTAActionRelease.h"

namespace Ui {
    class GTAChannelSelectorWidget;
}

class GTAChannelSelectorWidget :public QDialog
{
    Q_OBJECT

public:
    explicit GTAChannelSelectorWidget(QDialog *parent = 0);
    ~GTAChannelSelectorWidget();
    QString getChannelSelection(const QString& iFormat);
    void setParameter(const QString& iParam);



//signals:
//      void searchObject(QLineEdit *&);
private:
    Ui::GTAChannelSelectorWidget *ui;

private slots:


};

#endif // GTAChannelSelectorWidget_H
