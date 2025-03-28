#ifndef GTAINITCONFIGWIDGET_H
#define GTAINITCONFIGWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QKeyEvent>
namespace Ui {
    class GTAInitConfigWidget;
}

class GTAInitConfiguration;
class GTAInitConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTAInitConfigWidget(QWidget *parent = 0);
    ~GTAInitConfigWidget();

    GTAInitConfiguration * getConfiguration();
    void setConfiguration(const GTAInitConfiguration * ipConfigCmd);

protected:
    void keyPressEvent ( QKeyEvent * event );


signals:
    void okPressed();
private:
    Ui::GTAInitConfigWidget *ui;

    QList<QWidget*> _ParameterList;

    GTAInitConfiguration * _pInitConfigCmd;

    void setComboValue(QComboBox *&ipCombo, const QString & iValue);

public:
    QString getInitConfiguration() const;
    QString getEndConfiguration() const;
private slots:
    void onConfigurationChange(const QString & iConfiguration);
    void clearConfigParameters();
    void onOKPBClick();
    void enableConfigParameters(bool iState);
};

#endif // GTAINITCONFIGWIDGET_H
