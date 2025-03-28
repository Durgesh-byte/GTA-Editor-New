#ifndef GTAHeaderFieldEdit_H
#define GTAHeaderFieldEdit_H

#pragma warning (push, 0)
#include <QDialog>
#include <QLineEdit>
#pragma warning (pop)
//#include "GTAActionWidgetInterface.h"
class GTAActionCall;
namespace Ui {
    class GTAHeaderFieldEdit;
}

class GTAHeaderFieldEdit : public QDialog
{
    Q_OBJECT
        
public:
    enum mode{EDITMODE,ADDMODE};
    explicit GTAHeaderFieldEdit(QWidget *parent = 0);
    ~GTAHeaderFieldEdit();

   
  QString getFieldName()const;
  QString getFieldVal()const;
  QString getIsMandatory()const;
  QString getFieldDescription()const;
  bool getShowInLTRA()const;
  void clear();

  void setHeaderInfo(const QString& iFieldName,const QString& iFieldVal,QString& isMandatory,QString& iDescription, bool& iShowInLTRA);
  
  void setMode(mode iMode);
public slots:
      void show();
	
protected:
    void keyPressEvent ( QKeyEvent * event );
   

signals:
   
  void signalAddField();
  void signalEditField();

private:
    
    Ui::GTAHeaderFieldEdit *ui;
    mode _mode;
    bool _showInLTRA;
    //QString _lastError;

private slots:
   
    void accept();    
   
};

#endif // GTAHeaderFieldEdit_H
