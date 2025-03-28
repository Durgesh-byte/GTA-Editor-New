#ifndef GTAIMAGEDISPLAY_H
#define GTAIMAGEDISPLAY_H

#include <QDialog>
#include <QHash>

namespace Ui {
    class GTAImageDisplay;
}
class GTACheckAudioAlarm;
class GTAImageDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit GTAImageDisplay(QDialog *parent = 0);
    ~GTAImageDisplay();
    void setImage(QPixmap ipImage);

      

private:
    Ui::GTAImageDisplay *ui;

private slots:
    void closeWindow();
};

#endif // GTAIMAGEDISPLAY_H
