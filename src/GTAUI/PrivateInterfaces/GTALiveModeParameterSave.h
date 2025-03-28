#ifndef AINGTALIVEMODEPARAMETERSAVE_H
#define AINGTALIVEMODEPARAMETERSAVE_H

#include <QWidget>

namespace Ui {
class AINGTALiveModeParameterSave;
}

class AINGTALiveModeParameterSave : public QWidget
{
    Q_OBJECT

public:
    explicit AINGTALiveModeParameterSave(QWidget *parent = 0);
    ~AINGTALiveModeParameterSave();

    void showWidget(const bool &iIsImport, const QStringList &iFiles = QStringList());

private:
    Ui::AINGTALiveModeParameterSave *ui;



private slots:
    void onOKTBClicked();
    void onSavePBClicked();
    void onFileNameTextChanged(const QString &text);

signals:
    void saveParameterFile(QString fileName);
    void importFiles(QStringList fileNames);

};

#endif // AINGTALIVEMODEPARAMETERSAVE_H
