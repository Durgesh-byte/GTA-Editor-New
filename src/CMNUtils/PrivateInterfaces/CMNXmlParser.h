#ifndef AINCMNXmlParser_H_
#define AINCMNXmlParser_H_

#include "AINCMNUtils.h"

#include <QString>
#include <QList>
#include <QHash>
#include <QObject>
#include <QSharedPointer>
#include <QDomDocument>
#include <QStandardItemModel>

class QDomElement;			    
class QDomDocument;
namespace AINCMN
{
	namespace XmlUtils
	{
		enum CaseSensitiveConversion {noConversion=0,convertToUCase,convertToLCase};

	}
}
namespace AINCMN
{
	namespace XmlUtils
	{
		class AINCMNUtils_SHARED_EXPORT AINCMNXmlParser : public QObject
		{
		public:
			AINCMNXmlParser(const QString& iXmlFile);
			~AINCMNXmlParser();

			bool parse();

            //to get the value of an attribute related to given DomElement
            QString getValue(const QDomElement& iDomElem, const QString& iAttrName);
			
            //To get values of given attributes related to the given DomElement
            QHash<QString, QString> getValues(const QDomElement& iDomElem, const QStringList& iListOfAttributes);

            //To get the value of a particular attribute
            // for all the nodes (w.r.t tag) under a DomElement
            QList<QString> getValue(const QDomElement& iParentDomElem, const QString& iTag, const QString& iAttrName, bool bDuplicate_Allowed = false);

            QDomElement findDomElement(const QDomElement& iParentDomElem, const QString& iTag,
				const QString& iAttrName, const QString& iAttrVal,bool bIgnoreExtraSpace = false);
			
			QString findValue(const QString& iTag, const QString& iAttr, const QString& iWhereAttr, 
							  const QString& iWhereAttrVal, QDomElement * ipDomElem=NULL,bool bIgnoreExtraSpace = false);
			
			QList<QString> findValues(const QString& iTag, const QString& iAttr, const QString& iWhereAttr, 
									  const QString& iWhereAttrVal, QDomElement * ipDomElem=NULL,bool bIgnoreExtraSpace = false);
			
			QList<QDomElement> getAllDomElements(const QDomElement& iParentDomElem, const QString& iTag);
			
			QList<QDomElement> getAllDomElements(const QDomElement& iParentDomElem, const QString& iTag,
												 const QString& iAttrName, const QString& iAttrVal,bool bIgnoreExtraSpace = false);

			QDomElement getRootElement();
			
			int getAllObjectCount(const QString& sTag);
			
			QStringList getAllAttributeNames(const QDomElement& iDomElem);
			QHash<QString, QString> getAllAttrValues(const QDomElement& iDomElem,const CaseSensitiveConversion& iCaseConversion=AINCMN::XmlUtils::noConversion);
			
			//XML Modifications
			//-------------------------
			void addDomElement(QDomElement& parentDomElem, QString iTag, QHash<QString,QString> iAttrHash);

			void setAttribute(QDomElement& iDomElem, QHash<QString,QString> hshOfAttrs);

			void setAttribute(QDomElement& parentDomElem, QHash<QString,QString> hshOfAttrs, QHash<QString,QString> hshOfRefAttrs);

			QDomElement getParentElement(const QString& iTag);
			
			bool reWrite();

		protected:
			QString _xmlFile;
			QSharedPointer <QDomDocument> _spDomDoc;
		};
	}
}

#endif
