#ifndef GTAELEMENTTREEVIEWWIDGET_H
#define GTAELEMENTTREEVIEWWIDGET_H

#pragma warning (push, 0)
#include <QWidget>
#include <QLineEdit>
#include <QAbstractItemModel.h>
#pragma warning (pop)

namespace Ui {
    class GTAElementTreeViewWidget;
}

class GTAElementTreeViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTAElementTreeViewWidget(QWidget *parent = 0);
    ~GTAElementTreeViewWidget();

    QString getName() const;
    void setName(const QString & iVal) ;
	void setModel(QAbstractItemModel* pModel);

    

private:
    QAbstractItemModel* _pModel;
    Ui::GTAElementTreeViewWidget *ui;
    

//private slots:
   
};

#endif // GTAElementTreeViewWidget_H
