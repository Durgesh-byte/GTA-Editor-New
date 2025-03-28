#ifndef GTAONECLICKDYNAMICDIROPTION_H
#define GTAONECLICKDYNAMICDIROPTION_H

#include <QWidget>
#include <QCheckBox>


namespace Ui {
    class GTAOneClickDynamicDirOption;
}

class GTAOneClickDynamicDirOption : public QWidget
{
    Q_OBJECT

public:
    explicit GTAOneClickDynamicDirOption(QWidget *parent = 0);
    ~GTAOneClickDynamicDirOption();


    void insertDirectories(QStringList dirList,QHash<QString,bool> dirCheckState);
    void insertDirectories(QStringList dirList, const QString & iIniFilePath = QString());
    QHash<QString,bool> getCheckedDirectory();
    void updateCheckedStatus(QHash<QString,bool> dirCheckState);
    QString getIniFileName() const {return _IniFileName;}

private:
    Ui::GTAOneClickDynamicDirOption *ui;
    QList<QWidget*> _OptionChkList;
    QString _IniFileName;
    
    void parseINIFile(const QString & iIniFilePath);

private slots:
    void onIniBrowsePbClicked();
};

#endif // GTAONECLICKDYNAMICDIROPTION_H
