#ifndef GTASAVEEDITORWIDGET_H
#define GTASAVEEDITORWIDGET_H

#include <QDialog>

namespace Ui {
    class GTASaveEditorWidget;
}

class GTASaveEditorWidget : public QDialog
{
    Q_OBJECT

public:
    explicit GTASaveEditorWidget(QDialog *parent = 0);
    ~GTASaveEditorWidget();
	void setSaveName(const QString& isName);
    //QString getValue() const;
signals:
	void startSaveDocument();
private slots:
	void onSaveAccepted(void);
private:
    Ui::GTASaveEditorWidget *ui;

};

#endif // GTASAVEEDITORWIDGET_H
