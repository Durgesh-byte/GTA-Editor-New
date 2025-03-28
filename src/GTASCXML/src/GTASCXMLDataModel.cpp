#include "GTASCXMLDataModel.h"
#include "GTASCXMLUtils.h"

GTASCXMLDataModel::GTASCXMLDataModel()
{
}

void GTASCXMLDataModel::appendFromDataModel(GTASCXMLDataModel &iDataModel)
{
    QList<GTASCXMLData> dataItems = iDataModel.getDataItems();
    _dataItems.append(dataItems);
}

void GTASCXMLDataModel::insertDataItem(GTASCXMLData &iData)
{
    if(!_dataItems.contains(iData))
        _dataItems.append(iData);
}
QList<GTASCXMLData> GTASCXMLDataModel::getDataItems() const
{
    return _dataItems;
}

QDomElement GTASCXMLDataModel::getSCXML(QDomDocument &iDomDoc)  const
{
    QDomElement dataModelElem = iDomDoc.createElement(SCXML_DATA_MODEL);

    for(int i=0;i<_dataItems.count();i++)
    {
        QDomElement dataElem = _dataItems.at(i).getSCXML(iDomDoc);
        if(!dataElem.isNull())
            dataModelElem.appendChild(dataElem);
    }

    return dataModelElem;
}

