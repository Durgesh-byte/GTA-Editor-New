#ifndef GTACOMMENTWIDGET_H
#define GTACOMMENTWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QVariant>
class GTAGenSearchWidget;
namespace Ui {
class GTACommentWidget;
}

class GTAComment;
class QTextEdit;
class GTACommentImageWidget;
class GTACommentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTACommentWidget(QWidget *parent = 0);
    ~GTACommentWidget();

    void setCommentCommand(const GTAComment * ipCommentCmd);
    GTAComment * getCommentCommand();
    QVariant getCommentVariant();



signals:
    void okPressed();

private:
    Ui::GTACommentWidget *ui;

    GTACommentImageWidget * _pImageWidget;
    QTextEdit                * _pCommentValueTE;

    //  QString                    _CommentCommand;

    GTAGenSearchWidget   * _pSearchWidget;
    QLineEdit               * _pCurrentSearchResult;

    GTAComment           * _pCommentCommand;
    QVariant				_commentCmdVariant;

private slots:
    void onCommentTypeChange(const QString & iCommentType);
    void onOKPBClick();

    void onSearchPBClick(QLineEdit *& ipItemToUpdate);
    void onSearchOKPBClick();
};

#endif // GTACOMMENTWIDGET_H
