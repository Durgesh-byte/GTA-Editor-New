#ifndef GTAWIDGET_H
#define GTAWIDGET_H
#include "GTACore.h"

#pragma warning(push, 0)
#include <QWidget>
#pragma warning(pop)

class GTACore_SHARED_EXPORT GTAWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GTAWidget(QWidget *parent = 0);
    virtual ~GTAWidget();

signals:
    void widgetClosed();

private slots:

public slots:

};

#endif // GTAWIDGET_H
