#ifndef GTAPARAMVALIDATOR_H
#define GTAPARAMVALIDATOR_H
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

class GTACommands_SHARED_EXPORT GTAParamValidator: public QValidator
{
public:
    GTAParamValidator(QObject * parent = 0);
    virtual ~GTAParamValidator();
    QList<QString> getInvalidChars(QString &iText) const;
    State	validate ( QString & input, int & pos ) const;
};
#endif // GTAPARAMVALIDATOR_H

