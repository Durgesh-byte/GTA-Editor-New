#include "AINCMNTableSchemaParser.h"
using namespace AINCMN::XmlUtils;

#include "AINCMNCommonServices.h"
#include "AINCMNXmlParser.h"
using namespace AINCMN::CMNUtils;



AINCMNTableSchemaParser::AINCMNTableSchemaParser(const QString &iXmlSchemaFile)
: _xmlFile(iXmlSchemaFile)
{
}

AINCMNTableSchemaParser::~AINCMNTableSchemaParser()
{
}

bool AINCMNTableSchemaParser::parse()
{
	bool bOk (false);
	

	AINCMNXmlParser xmlParser(_xmlFile);
	bOk=xmlParser.parse();

	if(!bOk)
		return false;

	QDomElement rootDom =xmlParser.getRootElement();
	QList<QDomElement> lstOfDomElementTable	= xmlParser.getAllDomElements(rootDom,"table");
	_sTableName=lstOfDomElementTable[0].attribute("name","");

	QList<QDomElement> lstOfDomElemntsCol	= xmlParser.getAllDomElements(rootDom,"column");
	
	foreach(QDomElement element,lstOfDomElemntsCol)
	{	
		QString sCurrColName=element.attribute("name","");
		_sColoumns<<sCurrColName;
		
		_sDataType<<element.attribute("data_type","");


		QString sCurColIndexingStatus=element.attribute("indexing","");
		if (sCurColIndexingStatus=="1")
			_hshOfIndexed[sCurrColName]=true;
		else
			_hshOfIndexed[sCurrColName]=false;
		

		QString sCurrColPrimaryKeyStatus = element.attribute("primary_key","");
		if (sCurrColPrimaryKeyStatus=="1")
			_sPrimaryKey=sCurrColName;
	


	}

	return bOk;
}

QString AINCMNTableSchemaParser::getTableName(void) const
{ 
	return _sTableName;
}
QStringList AINCMNTableSchemaParser::getColoumns(void) const
{ 
	return _sColoumns;
}
QStringList AINCMNTableSchemaParser::getDataType(void) const
{ 
	return _sDataType;
}
QString AINCMNTableSchemaParser::getPrimaryKey(void) const
{
	return _sPrimaryKey;
}
QHash<QString, bool> AINCMNTableSchemaParser::getIndexedParser(void) const
{ 
	return _hshOfIndexed;
}

