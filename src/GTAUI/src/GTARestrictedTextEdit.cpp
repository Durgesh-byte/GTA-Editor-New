#include "GTARestrictedTextEdit.h"
GTARestrictedTextEdit::GTARestrictedTextEdit(QWidget *parent ):QTextEdit(parent)
{
   // connect(this,SIGNAL(textChanged()),this,SLOT(replaceInvalidChar()));

}
GTARestrictedTextEdit::~GTARestrictedTextEdit()
{

}
void GTARestrictedTextEdit::replaceInvalidChar()
{
    QString sText=this->toPlainText();
    if (sText.size()>1)
    {
        QRegExp rx("([`\\+\\\\'\"]+)");
        if(sText.contains(rx))
            this->clear();
    }
}
void	GTARestrictedTextEdit::keyPressEvent ( QKeyEvent * event )
{
    QString text = event->text();
    QRegExp rx("([`\\+\\\\'\"]+)");

    if (text.contains(rx)==false)
    {
        QTextEdit::keyPressEvent(event);
    }
}