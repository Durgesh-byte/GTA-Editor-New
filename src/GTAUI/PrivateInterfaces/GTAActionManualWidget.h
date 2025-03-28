#ifndef GTAACTIONMANUALWIDGET_H
#define GTAACTIONMANUALWIDGET_H
#include "GTAActionWidgetInterface.h"

#pragma warning(push, 0)
#include <QTextEdit>
#include <QWidget>
#include <QLineEdit>
#pragma warning(pop)

namespace Ui {
class GTAActionManualWidget;
}

class GTAActionManual;
class GTARestrictedLineWidget:public QLineEdit
{
protected:
    void	keyPressEvent ( QKeyEvent * event );

};

class GTAActionManualWidget : public GTAActionWidgetInterface 
{
    Q_OBJECT

public:
    explicit GTAActionManualWidget(QWidget *parent = 0);
    ~GTAActionManualWidget();

    void setActionCommand(const GTAActionBase * ipActionCmd);
    bool getActionCommand(GTAActionBase*& opCmd)const;
    void clear() ;
    bool isValid() const ;

    bool hasSearchItem()  {return true;}   
    void processSearchInput(const QStringList&);
    int getSearchType();

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);
    void backgroundColorChanged(const QColor &c);
    QString ampersandEncode(QString string)const;
private slots:
    void onSearchPBClicked();
    void onSearchPBClicked1();
    void onAddExpPBClicked();
    void onAddCondition(bool iValue);
    void onUserFeedbackValueChange(bool iValue);
    void onAcknowledgementValueChange(bool iValue);
    void validateText();
    void textBold();
    void textItalic();
    void textUnderline();

    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textAlign();

    void backgroundColor();
    void textColor();
    void cursorPositionChanged();
    void currentCharFormatChanged(const QTextCharFormat &format);


private:

    Ui::GTAActionManualWidget *ui;
    GTAActionManual *    _pManualAction;
    bool _IsLocalParam;
    void setParametertype(const QString &iParamType);

};

#endif // GTAACTIONMANUALWIDGET_H
