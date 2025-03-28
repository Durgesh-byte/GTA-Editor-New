#ifndef AINCMNTableSchemaParser_H_
#define AINCMNTableSchemaParser_H_

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
		class AINCMNUtils_SHARED_EXPORT AINCMNTableSchemaParser : public QObject
		{
		public:
			AINCMNTableSchemaParser(const QString& iXmlSchemaFile);
			~AINCMNTableSchemaParser();

			bool parse();
			QString getTableName(void) const;
			
			QStringList getColoumns(void) const;

			QStringList AINCMNTableSchemaParser::getDataType(void) const;

			QString AINCMNTableSchemaParser::getPrimaryKey(void) const;

			QHash<QString, bool> AINCMNTableSchemaParser::getIndexedParser(void) const;
			QString getPath()const {return _xmlFile;}




		protected:
			QString _xmlFile;
			

			QString _sTableName;
			QStringList _sColoumns;
			QStringList _sDataType;
			QString _sPrimaryKey;
			QHash<QString, bool> _hshOfIndexed;
		};
	}
}

#endif
