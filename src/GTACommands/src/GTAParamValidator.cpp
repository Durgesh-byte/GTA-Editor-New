#include "GTAParamValidator.h"
#include "GTAUtil.h"


GTAParamValidator::GTAParamValidator(QObject * parent):QValidator(parent)
{

}
GTAParamValidator::~GTAParamValidator()
{

}

// Do not allow : - + * / ^ & ~ % ! | (  ) { } [ ] ~
// Allowed characters: A-Z a-z 0-9 . ? \ ,  @ # $ _ : ; < > \ "
QValidator::State	GTAParamValidator::validate (  QString & input, int & pos ) const
{
    QRegExp rx("([A-Za-z0-9.?\\\\@#$_:=\\-;<>\"\\s]+)");
    QRegExpValidator validator(rx, 0);
    QValidator::State rc =  validator.validate(input,pos);
    return rc;

    /*QString lstOfForbiddenChar = GTAUtil::getListOfForbiddenChars();
    if (!input.isEmpty())
      if (lstOfForbiddenChar.contains(input.at(pos-1)))
        {
            return QValidator::Invalid;
        }
    return QValidator::Acceptable;*/
}


QList<QString> GTAParamValidator::getInvalidChars(QString &iText)const
{
    QStringList forbidChars = QString("+*/^&~%!|(){}[]~,").split("",QString::SkipEmptyParts);
    QStringList items  = iText.split("",QString::SkipEmptyParts);
    QSet<QString> intersection = forbidChars.toSet().intersect(items.toSet());
    return intersection.toList();
}


