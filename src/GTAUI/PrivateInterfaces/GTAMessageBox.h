#ifndef GTAMESSAGEBOX_H
#define GTAMESSAGEBOX_H

#include <QDialog>

namespace Ui {
    class GTAMessageBox;
}

class GTAMessageBox : public QDialog
{
    Q_OBJECT

public:
    enum MessageType{Information,Warning,Critical};
    enum DialogResult {OK,Cancel,None};
    enum Button{OKPB,CANCELPB};

    explicit GTAMessageBox(MessageType iType =Information,const QString & iTitle = "",QWidget *parent = 0);
    ~GTAMessageBox();

    void setMessage(const QString & iMessage);
    void setMessageLabel(const QString & iLabel);
    void setAdditionalData(const QStringList & iDataList);
    void setAdditionalDataLabel(const QString & iLabel);
    void setButtonLabel(const QString & iLabel, Button iButton);
    void setVisibility(bool iVal, Button iButton);

    DialogResult getResult() const;
private slots:
    void onPushButtonClick();
private:
    Ui::GTAMessageBox *ui;

    DialogResult _Result;

    void setImage(MessageType iType = Information);
};

#endif // GTAMESSAGEBOX_H
