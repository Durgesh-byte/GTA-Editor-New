#ifndef AINGTAICUSERFEEDBACKWIDGET_H
#define AINGTAICUSERFEEDBACKWIDGET_H

#include <QWidget>

namespace Ui {
    class AINGTAICUserFeedbackWidget;
}

class AINGTAICUserFeedbackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AINGTAICUserFeedbackWidget(QWidget *parent = 0);
    ~AINGTAICUserFeedbackWidget();

    bool showWidget(QString msg, QString expectedReturnVal);
    void showPrompt(QString msg,bool iWaitAck);

private:
    Ui::AINGTAICUserFeedbackWidget *ui;

    /**
      * Author : Priyanka
      * This function adds validator(Int/Float) to line edit based on input param.
      * @expectedReturnType: type of return value
      */
    void validateText(QString expectedReturnType);
    bool _mousePressed;
    QPoint _mousePosition;

signals:
    void sendUserInput(QString reply);
    void sendAcknowledgment(QString reply);

private slots:

    /**
      * Author : Priyanka
      * This function sends user input reply for user input prompts
      */
    void onOKPBClicked();

    /**
      * Author : Priyanka
      * This function disables OK button on user input prompts if input line edit is empty.
      */
    void okPBEnabled(QString text = QString());

    /**
      * Author : Priyanka
      * This function sends OK reply for Display message
      */
    void onOkPromptPBClicked();

    /**
      * Author : Priyanka
      * This function sends KO reply for Display message
      */
    void onKoPromptPBClicked();

protected:
    /**
      * Author : Priyanka
      * This function handles Enter key to OK button
      */
    void keyPressEvent ( QKeyEvent * event );

    /**
     * Mouse Press,Move,Release events are overridded to handling moving of frameless window
     */
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // AINGTAICUSERFEEDBACKWIDGET_H
