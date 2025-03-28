#ifndef GTALOGATTACHMENTWIDGET_H
#define GTALOGATTACHMENTWIDGET_H
#include <QKeyEvent>
#include <QWidget>
#include <QStringList>
namespace Ui {
    class GTALogAttachmentWidget;
}

class GTALogAttachmentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GTALogAttachmentWidget(QWidget *parent = 0);
    ~GTALogAttachmentWidget();

    void instertItem(const QString & iVal);
    QStringList getItemList() const;
    void clear();
protected:
    void keyPressEvent ( QKeyEvent * event );

private slots:
    void onAddPBClicked();
    void onRemovePBCliecked();
private:
    Ui::GTALogAttachmentWidget *ui;
};

#endif // GTALOGATTACHMENTWIDGET_H
