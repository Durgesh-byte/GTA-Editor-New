#ifndef GTAPARAMDETAILS_H
#define GTAPARAMDETAILS_H

#include <QWidget>

namespace Ui {
    class GTAParamDetails;
}

class GTAParamDetails : public QWidget
{
    Q_OBJECT

public:
    explicit GTAParamDetails(QWidget *parent = 0);
    ~GTAParamDetails();
    void setWidgetTitle(QString iTitle);
    void setTableData(const QHash<QString,QString> &iTableData);
    void clearTableData();
private:
    Ui::GTAParamDetails *ui;
};

#endif // GTAPARAMDETAILS_H
