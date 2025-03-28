#include "GTAConfigurationFileParser.h"

#pragma warning(push, 0)
#include <QFile>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QList>
#pragma warning(pop)

GTAConfigurationFileParser::GTAConfigurationFileParser(const QString & iConfigXmlFile)
{
	_pDomDoc = openXml(iConfigXmlFile);
}
GTAConfigurationFileParser::~GTAConfigurationFileParser()
{
	if(_pDomDoc != NULL )
	{
		delete _pDomDoc;
		_pDomDoc = NULL;
	}
}

QDomDocument * GTAConfigurationFileParser::openXml(const QString & iConfigXmlFile)
{
	QFile xmlFile(iConfigXmlFile);
	bool rc = xmlFile.open(QFile::ReadOnly | QFile::Text);
	QDomDocument * pDomDoc = NULL;
	if (rc)
	{
		pDomDoc = new QDomDocument();
		if(pDomDoc != NULL)
		{
			if( ! pDomDoc->setContent(&xmlFile))
			{
				delete pDomDoc;
				pDomDoc = NULL;
			}
		}
	}

	return pDomDoc;
}
QHash<QString,QStringList> GTAConfigurationFileParser::getActionCommandList(const QString & iCommandSetTag)
{
	QHash<QString,QStringList> oCommandList;
	if(_pDomDoc != NULL)
	{
        QDomNodeList cmdSetList = _pDomDoc->elementsByTagName(iCommandSetTag);
		QDomNode cmdSet;
		if(cmdSetList.count())
			cmdSet = cmdSetList.at(0);
		QDomNodeList domNodeList = cmdSet.childNodes();
		for(int i = 0; i < domNodeList.count();i++)
		{
			QDomNode domActionCmd =  domNodeList.at(i);
			if(domActionCmd.nodeName() != "ACTION" )
				continue;

			QDomNamedNodeMap domActionCmdAttrList =  domActionCmd.attributes();
			if(domActionCmdAttrList.contains("NAME"))
			{
				QDomNode domActionCmdAttr = domActionCmdAttrList.namedItem("NAME");
				QString cmdName =  domActionCmdAttr.nodeValue();
				if(! cmdName.isEmpty())
				{
					QStringList complementCmdList;
					QDomNodeList domComplementList = domActionCmd.childNodes();
					for(int j=0; j<domComplementList.count();j++)
					{
						QDomNode domComplemnt= domComplementList.at(j);
						if(domComplemnt.nodeName() != "COMPLEMENT")
							continue;

						QDomNamedNodeMap domCompAttr =  domComplemnt.attributes();
						if(domCompAttr.contains("NAME"))
						{
							QDomNode domCompName = domCompAttr.namedItem("NAME");
							QString complementCmdName = domCompName.nodeValue();
							complementCmdList.append(complementCmdName);
						}
					}

					oCommandList.insert(cmdName,complementCmdList);
                    _orderList.append(cmdName);
				}
			}
		}
	}
	return oCommandList;
}

QList<QString> GTAConfigurationFileParser::getCheckCommandList(const QString & iCommandSetTag)
{
	QList<QString> oCheckCommandList;

	if(_pDomDoc != NULL)
	{
        QDomNodeList cmdSetList = _pDomDoc->elementsByTagName(iCommandSetTag);
		QDomNode cmdSet;
		if(cmdSetList.count())
			cmdSet = cmdSetList.at(0);
		QDomNodeList domNodeList = cmdSet.childNodes();
		for(int i = 0; i < domNodeList.count();i++)
		{
			QDomNode domCheckCmd =  domNodeList.at(i);
			if(domCheckCmd.nodeName() != "CHECK" )
				continue;

			QDomNamedNodeMap domCheckCmdAttrList =  domCheckCmd.attributes();
			if(domCheckCmdAttrList.contains("NAME"))
			{
				QDomNode domCheckCmdAttr = domCheckCmdAttrList.namedItem("NAME");
				QString cmdName =  domCheckCmdAttr.nodeValue();
				
				oCheckCommandList.append(cmdName);
			}
		}
	}

	return oCheckCommandList;
}