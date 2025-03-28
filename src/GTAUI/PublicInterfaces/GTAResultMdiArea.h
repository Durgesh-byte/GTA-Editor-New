#ifndef GTARESULTMDIAREA_H
#define GTARESULTMDIAREA_H

#include <QMdiArea>
#include <QObject>
#include<QMdiSubWindow>

/**
 * This custom class inherited from QMdiArea to hold multiple result pages,
 * which is pushed dynamically to stacked widget for Result Analysis View
 * This is pushed to ui->MultiPerspectiveWidget as a RESULT_PAGE at index 1
*/
class GTAResultMdiArea : public QMdiArea
{
    Q_OBJECT
public:
    static GTAResultMdiArea *createResultMdiArea();
    /**
     * This getter function returns last active result page subwindow
     * @return: last active result page
    */
    QMdiSubWindow* lastActiveSubWindow();
//    ~GTAResultMdiArea();
private:
    GTAResultMdiArea();
    static GTAResultMdiArea *_resultMdiArea;
    QMdiSubWindow *_lastActiveWindow;
private slots:
    void onSubWindowActivated(QMdiSubWindow*);
};

#endif // GTARESULTMDIAREA_H
