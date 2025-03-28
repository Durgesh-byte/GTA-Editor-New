#ifndef GTAACTIONCMDCOMMENTWIDGET_H
#define GTAACTIONCMDCOMMENTWIDGET_H

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>
#pragma warning(pop)

namespace Ui {
    class GTAActionCmdCommentWidget;
}

class GTAActionCmdCommentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTAActionCmdCommentWidget(QWidget *parent = 0);
    ~GTAActionCmdCommentWidget();

    void setComment(const QString & iComment);
    void setImageName(const QString & iImgName);

    QString getComment() const;
    QString getImageName() const;

    void clear();
    void showImagePanel(bool iVal);
    
    
signals:
    void searchImage(QLineEdit *&);

public slots:
    void show();
private slots:
    void onSearchPBClicked();
    void okClicked();
    void validateText();
   
private:
    Ui::GTAActionCmdCommentWidget *ui;
    QString _comment;
protected:
    void keyPressEvent ( QKeyEvent * event );

};

#endif // GTAACTIONCMDCOMMENTWIDGET_H
