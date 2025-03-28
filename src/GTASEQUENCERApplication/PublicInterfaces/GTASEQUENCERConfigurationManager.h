#ifndef AINGTASEQUENCERCONFIGURATIONMANAGER_H
#define AINGTASEQUENCERCONFIGURATIONMANAGER_H

#include <QWidget>
#include <QListWidgetItem>

#include "AINGTASEQUENCERAppMainWindow.h"

namespace Ui {
    class AINGTASEQUENCERConfigurationManager;
}

class AINGTASEQUENCERConfigurationManager : public QWidget
{
    Q_OBJECT

public:
    explicit AINGTASEQUENCERConfigurationManager(QWidget *parent = 0);
    ~AINGTASEQUENCERConfigurationManager();
    void show();

    /**
      * Author: Priyanka
      * This function dispplays the list of configurtaions present in db on configurtaion manager.
      */
    void setConfigurations(const QStringList &iLstConfigurtaions);

private:
    Ui::AINGTASEQUENCERConfigurationManager *ui;
    QStringList _configurations;

public slots:
    void close();

private slots:
    void onNewPBClicked();
    void onDeletePBClicked();
    void onSavePBClicked();
    void onClearClicked();

    /**
      * Author: Priyanka
      * This function enables the delete button when a configuration is selected.
      */
    void onConfigurationSelected(QListWidgetItem *item);
    void closeEvent(QCloseEvent *event);

signals:
    void saveConfiguration(const QString &iConfigurationName);
    void deleteConfig(const QStringList &iSelectedItems);
    void widgetClosed();
};

#endif // AINGTASEQUENCERCONFIGURATIONMANAGER_H
