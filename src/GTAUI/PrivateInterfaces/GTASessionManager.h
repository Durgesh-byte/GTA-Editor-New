#ifndef GTASESSIONMANAGER_H
#define GTASESSIONMANAGER_H

#include <QWidget>
#include "GTAWidget.h"
#include <QListWidgetItem>
namespace Ui {
    class GTASessionManager;
}

class GTASessionManager : public GTAWidget
{
    Q_OBJECT

public:
    explicit GTASessionManager(GTAWidget *parent = 0);
    ~GTASessionManager();
    void show();
    void setSessions(const QStringList &iLstSessions);

public slots:
    void close();


private slots:
    void closeEvent(QCloseEvent *);
    void onNewPBClicked();
    void onDeletePBClicked();
    void onSavePBClicked();
    void onClearClicked();
    void onSwitchSessionClicked();
    void onSessionSelected(QListWidgetItem *item);

signals:
    void deleteSession(const QStringList &iSelectedItems);
    void saveSession(const QString &iSessionName);
    void switchSession(const QString &iSessionName);
private:
    Ui::GTASessionManager *ui;
    QStringList _sessions;    // contains names of all the sessions;
};

#endif // GTASESSIONMANAGER_H
