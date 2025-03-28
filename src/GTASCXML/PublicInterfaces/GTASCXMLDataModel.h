#ifndef GTASCXMLDATAMODEL_H
#define GTASCXMLDATAMODEL_H
#include "GTASCXML.h"
#include "GTASCXMLData.h"

#pragma warning(push, 0)
#include <QList>
#include <QDomElement>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)
class GTASCXML_SHARED_EXPORT GTASCXMLDataModel
{
    QList<GTASCXMLData> _dataItems;
public:
    GTASCXMLDataModel();
    void insertDataItem(GTASCXMLData &iData);
    void appendFromDataModel(GTASCXMLDataModel &iDataModel);
    bool isNULL() const {return _dataItems.isEmpty();}
    QList<GTASCXMLData> getDataItems() const;
    QDomElement getSCXML(QDomDocument &iDomDoc)  const;
};

#endif // GTASCXMLDATAMODEL_H
