#ifndef GTASCXMLDATA_H
#define GTASCXMLDATA_H
#include "GTASCXML.h"

#pragma warning(push, 0)
#include <QString>
#include <QDomElement>
#pragma warning(pop)

class GTASCXML_SHARED_EXPORT GTASCXMLData
{
    QString _expr;
    QString _id;
public:
    GTASCXMLData();
    void setDataId(const QString &iId);
    void setDataExpr(const QString &iExpr);
    QString getId() const {return _id;}
	QString getExpr() const { return _expr; }
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;
    bool operator ==(const GTASCXMLData &iObj )const
    {
        if(iObj.getId() == this->_id)
            return true;
        else
            return false;
    }
};

#endif // GTASCXMLDATA_H
