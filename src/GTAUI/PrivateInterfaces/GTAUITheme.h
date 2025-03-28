#ifndef GTAUITHEME_H
#define GTAUITHEME_H

#include <QWidget>
#include <QKeyEvent>
namespace Ui {
    class GTAUITheme;
}

class GTAUITheme : public QWidget
{
    Q_OBJECT

public:
    explicit GTAUITheme(QWidget *parent = 0);
    ~GTAUITheme();

    QColor getBackGroundColor() const;
    QColor getWindowTextColor() const;
    QColor getButtonTextColor() const;
    QColor getUserTextColor() const;

    void saveTheme();
    void loadTheme();
protected:
    void keyPressEvent ( QKeyEvent * event );

private slots:
    void onColorChange(const int & );
    void setDefualt();
    void onOKClicked();
    void onCancelClicked();




private:
    Ui::GTAUITheme *ui;

};

#endif // GTAUITHEME_H
