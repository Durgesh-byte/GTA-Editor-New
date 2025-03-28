#ifndef GTACMDREQUIREMENTWIDGET_H
#define GTACMDREQUIREMENTWIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QModelIndex>

namespace Ui {
    class GTACmdRequirementWidget;
}

class GTACmdRequirementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTACmdRequirementWidget(QWidget *parent = 0);
    ~GTACmdRequirementWidget();
    QStringList getRequirements()const;
    void setRequirements(const QStringList &iList);
    void clear();

public slots:
    void show();
private slots:

    void onAddPBClicked();
    void onRequirementLWClicked(const QModelIndex &index);
    void onRemovePBClicked();
    void onOKPBClicked();
    void onEditPBClicked();

private:
    Ui::GTACmdRequirementWidget *ui;
};

#endif // GTACMDREQUIREMENTWIDGET_H
