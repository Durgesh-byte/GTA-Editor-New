#ifndef GTASEARCHWIDGETINTERFACE_H
#define GTASEARCHWIDGETINTERFACE_H

#include <QWidget>
#include "GTAGenSearchWidget.h"

class GTASearchWidgetInterface: public QWidget
{
    Q_OBJECT

    public:
        
        GTASearchWidgetInterface(QWidget* iParent);
        virtual ~GTASearchWidgetInterface();
        virtual void clearSearch()=0;
        static GTAGenSearchWidget* getSearchWidget();
        void resetSearchWidget();

    public slots:
        
        //virtual bool setSearchType()=0;
        virtual void setSearchDlgVisibility()=0;
       // virtual void onOkClicked();

private:

    static GTAGenSearchWidget* _pGenSearchWidget;

};
#endif