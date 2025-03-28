#ifndef GTASVNAUTHENTICATIONWIDGET_H
#define GTASVNAUTHENTICATIONWIDGET_H

#include <QWidget>
#include <QDialog>

namespace Ui {
    class GTASVNAuthenticationWidget;
}

class GTASVNAuthenticationWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GTASVNAuthenticationWidget(QWidget *parent = 0);
    ~GTASVNAuthenticationWidget();

    QString getUserName();
    QString getPassword();
    bool getSaveAuthenticationState();
    void clear();

private:
    Ui::GTASVNAuthenticationWidget *ui;

private slots:

    void clearCredentials();
    void onLoginTBClicked();
    void onCancelTBClicked();

};

#endif // GTASVNAuthenticationWidget_H
