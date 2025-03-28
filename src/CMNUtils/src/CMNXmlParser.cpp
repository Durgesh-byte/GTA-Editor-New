#include "AINCMNXmlParser.h"
using namespace AINCMN::XmlUtils;

#include "AINCMNCommonServices.h"
using namespace AINCMN::CMNUtils;

#include <QFile>
#include <QtDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QStringList>

AINCMNXmlParser::AINCMNXmlParser(const QString &iXmlFile)
: _xmlFile(iXmlFile)
{
}

AINCMNXmlParser::~AINCMNXmlParser()
{
}

bool AINCMNXmlParser::parse()
{
	bool bOk (false);
	
	QFile xmlFile (_xmlFile);
	if (xmlFile.open(QIODevice::ReadOnly))
	{
		qDebug() << "XML File Parse Start";

		_spDomDoc = QSharedPointer <QDomDocument> (new QDomDocument(_xmlFile));
		if (_spDomDoc->setContent(&xmlFile))
			bOk = true;
		else {
			_spDomDoc.clear();
			bOk = false;
		}
	}

	xmlFile.close();

	return bOk;
}

QString AINCMNXmlParser::getValue(const QDomElement& iDomElem, const QString& iAttrName)
{
	QString sVal("");
		
	if (iDomElem.hasAttribute(iAttrName))
	{	
		QDomElement * pDomElem = new QDomElement(iDomElem);
		QDomAttr attr = pDomElem->attributeNode(iAttrName);
		sVal = attr.value();
		
		delete pDomElem; pDomElem=NULL;
	}
	
	return sVal;
}

QHash<QString, QString> AINCMNXmlParser::getValues(const QDomElement& iDomElem, const QStringList& iListOfAttributes)
{
	QHash<QString, QString> iHshOfValues;
	for (int i=0; i<iListOfAttributes.count(); i++)
	{
		QString sAttr = iListOfAttributes.at(i);
		QString sAttrVal = getValue(iDomElem, sAttrVal);
		if (sAttr!="" && !sAttr.isEmpty())
			iHshOfValues.insert(sAttr, sAttrVal);
	}

	return iHshOfValues;
}

QList<QString> AINCMNXmlParser::getValue(const QDomElement& iDomElem, const QString& iTag, const QString& iAttrName, bool bDuplicate_Allowed)
{
	QList <QString> listOfValues;
	if(! iDomElem.isNull())
	{
		QDomNodeList lstObjs = iDomElem.elementsByTagName(iTag);

		for (int i=0; i<lstObjs.count(); i++)
		{				
			if(lstObjs.item(i).isElement())
			{
				QDomElement iChildElem = lstObjs.item(i).toElement();
				QString sColName = getValue(iChildElem, iAttrName);

				listOfValues.append(sColName);
			}
		}
	}
	if(!bDuplicate_Allowed)
	{
		QSet <QString> set = listOfValues.toSet();
		listOfValues = set.toList();
	}

	return listOfValues;
}

int AINCMNXmlParser::getAllObjectCount(const QString& sTag)
{
	int cnt = -1;

	if(! _spDomDoc->isNull())
	{
		QDomNodeList lstOfOTableNodes = _spDomDoc->elementsByTagName(sTag);

		cnt = lstOfOTableNodes.count();
	}

	return cnt;
}

QDomElement AINCMNXmlParser::findDomElement(const QDomElement& iParentDomElem, const QString& iTag, 
						  const QString& iAttrName, const QString& iAttrVal,bool bIgnoreExtraSpace)
{
	QDomElement domElement;
	QString inputAttrVal = iAttrVal;
	if (bIgnoreExtraSpace)
	{
		inputAttrVal = AINCMNCommonServices::treatSpace(iAttrVal);
	}

	QList<QDomElement> listOfAllElements = getAllDomElements(iParentDomElem, iTag);
	for(int i=0; i<listOfAllElements.count(); i++)
	{
		QString cvalue = getValue(listOfAllElements.at(i), iAttrName);
		
		if (bIgnoreExtraSpace)
		{
			cvalue = AINCMNCommonServices::treatSpace(cvalue);
		}
		
		if (cvalue == inputAttrVal)
		{
			domElement = listOfAllElements.at(i);
			break;
		}
	} 
	
	return domElement;
}

QString AINCMNXmlParser::findValue(const QString& iTag, const QString& iAttr, const QString& iWhereAttr, 
												const QString& iWhereAttrVal, QDomElement * ipDomElem ,bool bIgnoreExtraSpace)
{	
	QString sVal("");
	
	QDomElement domElem;
	if (ipDomElem)
		domElem = findDomElement(*ipDomElem, iTag, iWhereAttr, iWhereAttrVal,bIgnoreExtraSpace);
	else
		domElem = findDomElement(getRootElement(), iTag, iWhereAttr, iWhereAttrVal,bIgnoreExtraSpace);
	
	if ( !domElem.isNull())
	{
		sVal = getValue(domElem, iAttr);
	}	
		
	return sVal;
}

QList<QString> AINCMNXmlParser::findValues(const QString& iTag, const QString& iAttr, const QString& iWhereAttr, 
												const QString& iWhereAttrVal, QDomElement * ipDomElem,bool bIgnoreExtraSpace)
{	
	QList <QString> listOfValues;
	
	QDomElement domElem;
	if ( ipDomElem )
		domElem = *ipDomElem;
	else
		domElem = getRootElement();
	
	QList<QDomElement> lstOfDomElements = getAllDomElements(domElem, iTag, iWhereAttr, iWhereAttrVal,bIgnoreExtraSpace);
	
	for(int i=0;i<lstOfDomElements.count();i++)
	{
		QDomElement domElement = lstOfDomElements.at(i);

		if ( !domElement.isNull())
		{
			listOfValues.append( getValue(domElement, iAttr));
		}
	}
		
	return listOfValues;
}


QList<QDomElement> AINCMNXmlParser::getAllDomElements(const QDomElement& iParentDomElem, 
																		const QString& iTag)
{
	QList<QDomElement> listOfAllElements;
	QDomNodeList listOfNodes = iParentDomElem.elementsByTagName(iTag);
	for (int i=0; i<listOfNodes.count(); i++)
	{
		QDomNode domNode = listOfNodes.at(i);
		if (domNode.isElement())
		{
			listOfAllElements.append(domNode.toElement());
		}
	}
	
	return listOfAllElements;
}


QList<QDomElement> AINCMNXmlParser::getAllDomElements(const QDomElement& iParentDomElem, const QString& iTag,
														const QString& iAttrName, const QString& iAttrVal,bool bIgnoreExtraSpace)
{
	QList<QDomElement> listOfAllElements;
	QList<QDomElement> listDomelements = getAllDomElements(iParentDomElem, iTag);
	QString inputAttrVal = iAttrVal;
	if (bIgnoreExtraSpace)
	{
		inputAttrVal = AINCMNCommonServices::treatSpace(iAttrVal);
	}

	for (int i=0; i<listDomelements.count(); i++)
	{
		QDomElement domElement = listDomelements.at(i);
		QString str_instance = getValue( domElement,  iAttrName);
		if (bIgnoreExtraSpace)
		{
			str_instance = AINCMNCommonServices::treatSpace(str_instance);
		}
		
		if( QString::compare(str_instance, inputAttrVal) == 0 )
		{
			listOfAllElements.append(domElement);
		}
	}

	return listOfAllElements;
}

QDomElement AINCMNXmlParser::getRootElement()
{
	QDomElement domElem;
	if (_spDomDoc)
		domElem = _spDomDoc->documentElement();
	return domElem;
}

QStringList AINCMNXmlParser::getAllAttributeNames(const QDomElement& iDomElem)
{
	QStringList lstOfAttrs;
	QDomNamedNodeMap attrNodeMap = iDomElem.attributes();
	int iCnt = attrNodeMap.length();
	for (int i=0; i<iCnt; i++)
	{
		QDomNode node = attrNodeMap.item(i);
		if (node.isAttr())
		{
			QDomAttr attr = node.toAttr();
			QString sName = attr.name();
			lstOfAttrs.append(sName);
		}
	}

	return lstOfAttrs;
}

QHash<QString, QString> AINCMNXmlParser::getAllAttrValues(const QDomElement& iDomElem,const CaseSensitiveConversion& iCaseConversion)
{
	QHash<QString, QString> hshOfAttrs;
	QDomNamedNodeMap attrNodeMap = iDomElem.attributes();
	int iCnt = attrNodeMap.length();
	for (int i=0; i<iCnt; i++)
	{
		QDomNode node = attrNodeMap.item(i);
		if (node.isAttr())
		{
			QDomAttr attr = node.toAttr();
			QString sName = attr.name();
			QString sValue = attr.value();
			if (!sName.isEmpty() || sName!="")
			{
				QString sConvertedName = sName;
				if (iCaseConversion== AINCMN::XmlUtils::convertToLCase)
					sConvertedName=sName.toLower();
				else
					if (iCaseConversion== AINCMN::XmlUtils::convertToUCase)
						sConvertedName=sName.toUpper();
				hshOfAttrs.insert(sConvertedName, sValue);
			}
		}
	}

	return hshOfAttrs;
}

void AINCMNXmlParser::addDomElement(QDomElement &parentDomElem, QString iTag, QHash<QString,QString> hshOfAttrs)
{
	QDomDocument doc;
	QDomElement newElement = doc.createElement(iTag);  // creates a QDomElement with the tag name 'iTag'

	if (!hshOfAttrs.isEmpty())
	{
		QHash<QString,QString>::iterator itr;
		for(itr = hshOfAttrs.begin(); itr!=hshOfAttrs.end(); ++itr)
		{
			newElement.setAttribute(itr.key(),itr.value());
		}
	}

	parentDomElem.appendChild(newElement);
}

void AINCMNXmlParser::setAttribute(QDomElement &iDomElem, QHash<QString,QString> hshOfAttrs)
{
	if (!hshOfAttrs.isEmpty())
	{
		QHash<QString,QString>::iterator itr;
		for (itr=hshOfAttrs.begin();itr!=hshOfAttrs.end();++itr)
		{
			iDomElem.setAttribute(itr.key(),itr.value());			
		}
	}
}

void AINCMNXmlParser::setAttribute(QDomElement &parentDomElem, QHash<QString,QString> hshOfAttrs, QHash<QString,QString> hshOfRefAttrs)
{
	if (hshOfRefAttrs.isEmpty())
	{
		for(QDomElement iElem=parentDomElem.firstChildElement(); !iElem.isNull(); iElem=iElem.nextSiblingElement())
		{
			setAttribute(iElem, hshOfAttrs);
		}
	}
	else
	{
		for(QDomElement iElem=parentDomElem.firstChildElement(); !iElem.isNull(); iElem=iElem.nextSiblingElement())
		{
			bool isItTheElement = true;
			QHash<QString,QString>::iterator hshItr;
			hshItr = hshOfRefAttrs.begin();
			while (isItTheElement && hshItr != hshOfRefAttrs.end())
			{
				if(iElem.hasAttribute(hshItr.key()))
				{
					if (iElem.attribute(hshItr.key(),"NULL")!=hshItr.value())
					{
						isItTheElement = false;
					}
				}
				else
				{
					isItTheElement = false;
				}

				hshItr++;
			}

			if(isItTheElement)
			{
				setAttribute(iElem,hshOfAttrs);
				
			}

		}
	}

}

QDomElement AINCMNXmlParser::getParentElement(const QString &iTag)
{
	QDomNodeList dList = getRootElement().elementsByTagName(iTag);

	if (!dList.isEmpty())
	{
		return dList.at(0).parentNode().toElement();
	}
	else
		return QDomElement::QDomElement();
	
	
}
bool AINCMNXmlParser::reWrite()
{
	bool bOk (false);

	QFile xmlFile (_xmlFile);
	if (xmlFile.open(QIODevice::Truncate|QIODevice::WriteOnly))
	{
		qDebug() << "XML File reWrite Start";

		xmlFile.write(_spDomDoc->toByteArray());

		bOk=true;
		xmlFile.close();
	}
	else
	{
		bOk=false;
	}
	

	return bOk;
}