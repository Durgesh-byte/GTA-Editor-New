#ifndef GTASVNResolveOptionDialog_H
#define GTASVNResolveOptionDialog_H

#include <QDialog>
#include "GTAActionOneClickPPC.h"

namespace Ui {
class GTASVNResolveOptionDialog;
}

class GTASVNResolveOptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GTASVNResolveOptionDialog(QWidget *parent = 0);
    ~GTASVNResolveOptionDialog();

    QString getUserSelection();
    bool startResolving();
    

private:
    Ui::GTASVNResolveOptionDialog *ui;
    bool _bResolve;
    
    
private slots:
    
    
    void onCancelPBClicked();
    void onOkPBClicked();


};

#endif // GTASVNResolveOptionDialog_H
