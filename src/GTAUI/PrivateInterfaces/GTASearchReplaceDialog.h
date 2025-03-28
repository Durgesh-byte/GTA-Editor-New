#ifndef GTASEARCHREPLACEDIALOG_H
#define GTASEARCHREPLACEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QKeyEvent>
#include <QList>
namespace Ui {
    class GTASearchReplaceDialog;
}

class GTAInitConfiguration;
class GTASearchReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GTASearchReplaceDialog(QWidget *parent = 0);
    ~GTASearchReplaceDialog();
    void setDefaultTab();

    
    
protected:
    void keyPressEvent ( QKeyEvent * event );
    void closeEvent ( QCloseEvent * e );
    void showEvent ( QShowEvent * event );
    void hideEvent ( QHideEvent * event);


signals:
    void searchNext(QRegExp,bool,bool);
    void searchHighlightAllString(QRegExp,bool ibTextSearch);
    void searchInAllDocuments(QRegExp,bool ibTextSearch);
    void replaceSearchNext(QRegExp,QString,bool);
    void replaceAll(QRegExp,QString);
    void replaceInAllDocuments(QRegExp,QString);
    void replaceAllEquipment(QRegExp, QString);
    void dlgCloseEvent();
    void searchRequirement(QRegExp);
    void searchTag(QRegExp);
    void searchComment(QRegExp);
private:
    Ui::GTASearchReplaceDialog *ui;
    QList<int> _lastSelectedRows;

    
private slots:
    void searchNext();   
    void replaceSearchNext();   
    void enableSearch ( const QString& iString ); 
    void enableSearchReplace ( const QString& iString );
    void replaceAll();
    void replaceAllEquipment();
    void searchRequirement();
    void searchTag();
    void searchComment();

};

#endif // GTASEARCHREPLACEDIALOG_H
