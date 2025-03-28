#ifndef GTAPROGRESS_H
#define GTAPROGRESS_H

#pragma warning (push, 0)
#include <QWidget>
#pragma warning (pop)

namespace Ui {
    class GTAProgress;
}

class GTAProgress : public QWidget
{
    Q_OBJECT

public:
    explicit GTAProgress(QWidget* parent = 0);
    ~GTAProgress();
    void setPosition(const QRect& iRect);
    void setLabelText(const QString& iText);
    void onStopProgressBar();

signals:
    void stopProgressBar();

public slots:
    void startProgressBar();

private:
    Ui::GTAProgress *ui;

};

#endif // GTAPROGRESS_H

