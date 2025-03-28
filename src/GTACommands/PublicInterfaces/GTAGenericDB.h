#ifndef GTAGENERICDB_H
#define GTAGENERICDB_H

#include "GTACommands.h"
#include "GTAGenericDbAttribute.h"

#pragma warning(push, 0)
#include <QList>
#include <QString>
#include <QStringList>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTACommands_SHARED_EXPORT GTAGenericDB
{
public:
    GTAGenericDB();

    void setName(const QString &iName);
    QString getName()const;

    void setPath(const QString &iPath);
    QString getPath()const;

    void setRelativePath(const QString& iRelativePath);
    QString getRelativePath() const;

    void addAttribute(const int &iID, const QString &iName, const int &iCol);
    QList<GTAGenericDbAttribute> getAttributes();
    GTAGenericDbAttribute getAttribute(const int &index);

    QList<QString> getHeadingList()const;
    QList<int> getColumnList()const;

    void setDelimiter(const QString &iDelimiter);
    QString getDelimiter()const;

    void setColsConcatList(const QList<int> &iVal);
    QList<int> getColsConcatList()const;

    void setConcatString(const QString &iConcatStr);
    QString getConcatString()const;

private:
    QString _Name;
    QString _Path;
    QString _RelativePath;
    QString _Delimiter;
    QList<int> _ColsToConcatList;
    QString _ConcatString;
    QList<GTAGenericDbAttribute> _Attributes;
};

#endif // GTAGENERICDB_H
