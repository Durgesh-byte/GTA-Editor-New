#ifndef GTACOMMENTIMAGEWIDGET_H
#define GTACOMMENTIMAGEWIDGET_H

#pragma warning(push, 0)
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

namespace Ui {
    class GTACommentImageWidget;
}

class GTACommentImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTACommentImageWidget(QWidget *parent = 0);
    ~GTACommentImageWidget();

    QString getImageFileName() const;
    QString getImageFullFilePath() const;
    void setImageFileName(const QString &);
    void setImageFullFilePath(const QString &);
    void setPreview(const QString & iImagePath);

signals:
    void searchImage(QLineEdit *&);
private:
    Ui::GTACommentImageWidget *ui;

private slots:
    void onSearchPBClicked();
};

#endif // GTACOMMENTIMAGEWIDGET_H
