#ifndef GTAACTIONFTAKINEMATICMULTIOUTPUTWIDGET_H
#define GTAACTIONFTAKINEMATICMULTIOUTPUTWIDGET_H

#include "GTAActionWidgetInterface.h"
#include "GTAActionBase.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QHash>
#pragma warning(pop)

namespace Ui {
    class GTAActionFTAKinematicMultiOutputWidget;
}

class GTAActionFTAKinematicMultiOutputWidget : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionFTAKinematicMultiOutputWidget(QWidget *parent = 0);
    ~GTAActionFTAKinematicMultiOutputWidget();


    
    bool getActionCommand(GTAActionBase *& pCmd)const;
    void setActionCommand(const GTAActionBase * pCmd);
    void clear();
    bool isValid()const;
    int getSearchType();
    bool hasSearchItem(){return true;}

signals:
    void searchObject(QLineEdit *&);
private:
    Ui::GTAActionFTAKinematicMultiOutputWidget *ui;
    mutable QStringList _RefFileData;//todo: implement such that this is not required

    QHash<QToolButton*,QLineEdit*> _hshToolButtonLE;


private slots:
        void onSearchPBClicked();
        void onBrowseRefFilePBClicked();
        void onRefFileTextChanged(const QString &);
      


       // void onSearchPBClicked();
};

#endif // GTAACTIONFTAKINEMATICMULTIOUTPUTWIDGET_H
