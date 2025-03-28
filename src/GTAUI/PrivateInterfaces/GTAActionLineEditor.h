#ifndef GTAACTIONLINEEDITOR_H
#define GTAACTIONLINEEDITOR_H
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QWidget>
#include <QValidator>
#pragma warning(pop)

namespace Ui {
    class GTAActionLineEditor;
}

class GTAActionLineEditor : public GTAActionWidgetInterface
{
    Q_OBJECT

public:
    explicit GTAActionLineEditor(QWidget *parent = 0);
    ~GTAActionLineEditor();

    virtual void clear();
    virtual bool isValid();
    void setValidator(QValidator*);
    void clearValidator();

protected:
    void setLabel(const QString & iVal);
    void setText(const QString & iVal);
    void setSuffiexLabel(const QString & iVal);

    QString getLable() const;
    QString getText() const;
    QString getPrefixLable() const;

    void showMessageTypeCB(bool iVal = true);

    void setMessageTypeList(const QStringList & iMsgTypes);
    void setCurrentMessageType(const QString & iMsg);
    QString getCurrentMessageType() const;



private:
    Ui::GTAActionLineEditor *ui;
};

#endif // GTAACTIONLINEEDITOR_H
