#ifndef GTAPARAMVALUEVALIDATOR_H
#define GTAPARAMVALUEVALIDATOR_H
#include "GTACommands.h"

#pragma warning(push, 0)
#include <Qlist>
#include <QColor>
#include <QFont>
#include <QMap>
#include "GTAScxmlLogMessage.h"
#include <QSet>
#include <QValidator>
#pragma warning(pop)

class GTACommands_SHARED_EXPORT GTAParamValueValidator: public QValidator
{
public:
    GTAParamValueValidator(QObject * parent = 0, const QString& validationExpression = "");
    virtual ~GTAParamValueValidator();
    QList<QString> getInvalidChars(QString &iText) const;
    QValidator::State validate ( QString & input, int & pos ) const;

private:
    QString _pValidationExpression;
};
#endif // GTAParamValueValidator_H

