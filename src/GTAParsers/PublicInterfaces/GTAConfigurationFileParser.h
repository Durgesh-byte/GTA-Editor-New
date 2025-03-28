#ifndef GTACONFIGURATIONFILEPARSER_H
#define GTACONFIGURATIONFILEPARSER_H

#pragma warning(push, 0)
#include <QtXml/QDomDocument>
#include <QHash>
#include <QString>
#include <QStringList>
#pragma warning(pop)

#include "GTAParsers.h"

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTAParsers_SHARED_EXPORT GTAConfigurationFileParser
{
public:
    GTAConfigurationFileParser(const QString & iConfigXmlFile);
    ~GTAConfigurationFileParser();

    QHash<QString,QStringList> getActionCommandList(const QString & iCommandSetTag);
    QStringList getActionOrderList(){return _orderList;}

	QList<QString> getCheckCommandList(const QString & iCommandSetTag);
   
private:
    QDomDocument * _pDomDoc;
    QStringList _orderList;
    QDomDocument * openXml(const QString & iConfigXmlFile);
};

#endif // GTACONFIGURATIONFILEPARSER_H

