#ifndef GTAElementTableView_H
#define GTAElementTableView_H
#include <QObject>
#include <QTableView>
#include <QModelIndex>
#include <QKeyEvent>
class GTAElementTableView: public QTableView//,public QObject
{
    Q_OBJECT
    public:
        
        GTAElementTableView(QWidget * parent = 0 );
        virtual ~GTAElementTableView();
    

   protected:
        void	selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
        //void keyPressEvent ( QKeyEvent * event );
        
signals:
        void selectionChangedForEdior();
};
#endif~