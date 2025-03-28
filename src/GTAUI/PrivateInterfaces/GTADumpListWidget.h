#ifndef GTADUMPLISTWIDGET_H
#define GTADUMPLISTWIDGET_H

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

class GTAActionPrint;
namespace Ui {
    class GTADumpListWidget;
}

class GTADumpListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTADumpListWidget(QWidget *parent = 0);
    ~GTADumpListWidget();
    void setDumpList(QStringList iDumpList);
    void  clearDumpList(bool bClearData = false);
    
    QStringList getDumpList() const;
   // void setValue(const QString & iVal) ;
   // virtual void clear();
   // virtual bool isValid();
    //void setActionCommand(GTAActionPrint *ipPrintActionCmd );

signals:
    void searchObject(QLineEdit *&);

private:
    Ui::GTADumpListWidget *ui;
    QStringList _currentDumpParameterLst;
    QStringList _dumpParameterLst;


    void hideInformation();
    void showInformation(const QString& isInformation);


    
private slots:
    void onSearchPBClicked();
    void addToParamList();
    void removeFromParamList();
    void onEditingCancel();
    void editFromParamList(); 
    void editFinished();
    void updateParamLEFromLst( );
};

#endif // GTADumpListWidget_H
