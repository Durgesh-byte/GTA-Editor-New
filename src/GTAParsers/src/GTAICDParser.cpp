#include "GTAICDParser.h"
#include "GTAUtil.h"
#include <QtConcurrentMap>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentFilter>
#include <QtConcurrentRun>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QDomDocument>
#include "GTAICDParameter.h"

#include <QMutexLocker>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <QtConcurrentFilter>
#include <QXmlStreamReader>

//#define BEGIN_STRING			 "#Begin"
//#define END_STRING				 "#End"
//#define EQUIPMENT_NAME_KEY		 "Equipment name"



//#define DATA_TYPE "Data Type"
// these are refered as DATA_TYPE in genesta's 'data' sheet
//#define EQUIPMENT_BLOCK			 "EQUIPMENT"
#define A429_OUTPUT_LABEL "A429_OUTPUT_LABEL"
#define A429_INPUT_LABEL "A429_INPUT_LABEL"
#define ANALOGUE_OUTPUT_SIGNAL "ANALOGUE_OUTPUT_SIGNAL"
#define ANALOGUE_INPUT_SIGNAL "ANALOGUE_INPUT_SIGNAL"
#define DISCRETE_OUTPUT_SIGNAL "DISCRETE_OUTPUT_SIGNAL"
#define DISCRETE_INPUT_SIGNAL "DISCRETE_INPUT_SIGNAL"
#define CAN_OUTPUT_MESSAGE "CAN_OUTPUT_MESSAGE"
#define AFDX_INPUT_MESSAGE "AFDX_INPUT_MESSAGE"
#define AFDX_OUTPUT_MESSAGE "AFDX_OUTPUT_MESSAGE"

#define PMRNODE "PMR"
//#define MODEL "MODEL"
//#define VCOM_STATIC "VCOM STATIC"
//#define PIR "PIR"
#define ICDLNODE "ICDL"

#define VARIABLES_KEY "Variables"
//#define VARIABLE_KEY "Variable"
#define INTERFACES "Interfaces"
#define BUSES "Buses"
//#define ACTOR "Actor"
#define BISG "Bisg"
#define NAME_KEY "name"
#define MAPPING "mapping"
#define TYPE_KEY "Type"
//#define TYPE_KEY_LOWER "type"
#define EQUIPMENTS_KEY "Equipments"
#define EQUIPMENT_KEY "Equipment"
#define INTERFACE_KEY "Interface"
#define BUS_KEY "Bus"
#define PARAMETERS_KEY "Parameters"
//#define PARAMETER_KEY "Parameter"

//#define SIGNAL_DESCRIPTION "Signal description"
//#define ICD "ICD"
//#define PARAMETER_NAME "Parameter name"
//#define PARAMETER_NAME_MODERN "Modern Parameter name"
//#define IDENTIFIER_PARAMETER_LOCAL_NAME "Identifier / Parameter Local Name"
//#define LABEL "Label"
//#define BIT "Bit"
//#define SIGNAL_TYPE "Signal Type"
//#define VALUE_TYPE "Value Type"
//#define MIN_VALUE_BOOL_FALSE_DEFINITION "Min Value / Bool False Definition"
//#define MAX_VALUE_BOOL_TRUE_DEFINITION "Max Value / Bool True Definition"
//#define UNIT "Unit"
//#define PARAMETER_TYPE "Parameter type"

#define BOOLEAN_TYPE  "Boolean"
#define FLOAT_TYPE  "Float"
#define INTERGER_TYPE  "Integer"
//#define STRING_TYPE  "String"


//QString GTAICDParser::_sFilePath;
//QString GTAICDParser::_sEquipment;
//QHash<GTAICDParser::ParseFlag, bool> GTAICDParser::_ParsingFalgeData;
//GTAICDParser::GTAICDParser()
//{
//	setParsingFlag
//}
GTAICDParser::GTAICDParser(const QString& iICDFilePath/*, QObject * parent*/) //: QObject(parent)
{
    _sFilePath = iICDFilePath;
    
    setDefaultParseFlag();
}
GTAICDParser::~GTAICDParser()
{
    
	QtConcurrent::run(GTAICDParser::clearParameterList,_ICDParamList);
//    for(int i = 0 ; i< _ICDParamList.count(); i++ )
//    {
//        GTAICDParameter * pICDParam = _ICDParamList.at(i);
//        if(pICDParam != NULL)
//        {
//            delete pICDParam;
//            pICDParam = NULL;
//        }
//    }
    _ICDParamList.clear();
}

void GTAICDParser::setDefaultParseFlag()
{
    _ParsingFalgeData[A429OUT] = true;
    _ParsingFalgeData[A429IN] = true;
    _ParsingFalgeData[ANAOUT] = true;
    _ParsingFalgeData[ANAIN] = true;
    _ParsingFalgeData[DESCOUT] = true;
    _ParsingFalgeData[DESCIN] = true;
    _ParsingFalgeData[CANOUT] = true;
	_ParsingFalgeData[CANIN] = true;
    _ParsingFalgeData[AFDXOUT] = true;
    _ParsingFalgeData[AFDXIN] = true;
    _ParsingFalgeData[PMRF] = true;
    _ParsingFalgeData[MODELF] = true;
    _ParsingFalgeData[VCOMSTATIC] = true;
    _ParsingFalgeData[PIRF] = true;
}
void GTAICDParser::setParseFlag(const QHash<ParseFlag, bool> &iFlags)
{
    _ParsingFalgeData = iFlags;
}

bool GTAICDParser::parse()
{
    bool rc = false;
    if (_sFilePath.endsWith(".csv"))
    {
        rc = parseICD();
    }
    else if (_sFilePath.endsWith(".xml"))
    {
        rc = parsePmrXmlFile();
    }
    else if (_sFilePath.endsWith(".pir"))
    {
        rc=  parsePIR();
    }
    else
    {
        _ErrMessage = "Error: No file present to parse";
        rc = false;
    }
    return rc;
}
QString GTAICDParser::getLastError() const
{
    return _ErrMessage;
}
bool GTAICDParser::parseICD()
{
    QStringList validFields;

    validFields<<A429_OUTPUT_LABEL<<A429_INPUT_LABEL<<ANALOGUE_OUTPUT_SIGNAL;
    validFields<<ANALOGUE_INPUT_SIGNAL<<DISCRETE_OUTPUT_SIGNAL<<DISCRETE_INPUT_SIGNAL;
    validFields<<CAN_OUTPUT_MESSAGE<<AFDX_INPUT_MESSAGE<<AFDX_OUTPUT_MESSAGE;
    bool rc = false;
    QFile fileObj(_sFilePath);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);

    if (! rc)
    {
        _ErrMessage = "Error: Fail to open file " + QString(_sFilePath);
        return rc;
    }
    QList<QString> listOfDataLine;
    while(!fileObj.atEnd())
    {
        QString line = fileObj.readLine();
        if(! line.isEmpty() && (!line.startsWith("#")))
        {
            if(line.startsWith("EQUIPMENT"))
            {
                QStringList equipData = line.split(";");
                if(equipData.count() >2)
                    _sEquipment = equipData.at(1);

                continue;
            }

            foreach(QString key, validFields)
            {
                if( line.startsWith(key))
                {
                    //listOfDataLine.append(line);

                    GTAICDParameter * pParameter = getParameterFromICD(line);
                    if(pParameter!= NULL)
                        _ICDParamList.append(pParameter);
                    break;
                }
            }
        }
    }
    fileObj.close();
    return rc;
}
GTAICDParameter * GTAICDParser::getParameterFromICD(const QString & iICDDataLine)
{

    GTAICDParameter * pParameter = NULL;
    //pParameter.setSourceType("ICD");

    if(! iICDDataLine.isEmpty())
    {
        QString paramName;

        QStringList icdParamData = iICDDataLine.split(";",QString::KeepEmptyParts);
        QString icdType = icdParamData.at(0);

        QString appName ;
        QString associatedBus;
        QString signalName;
        QString identifier;
        QString label ="NA";
        QString signalType = "NA";
        QString valueType = "NA";
        QString bit;
        QString unit = "NA";
        QString minVal = "NA";
        QString maxVal = "NA";
        QString paramType = "FIB";
        QString media = "NA";
        int fieldCount = icdParamData.count();
        if (_ParsingFalgeData[A429OUT] && icdType == A429_OUTPUT_LABEL && fieldCount > 66)
        {
            media = "A429";
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(36).simplified();
            identifier = icdParamData.at(27);
            label = icdParamData.at(6);
            valueType = icdParamData.at(37);
            signalType = QString("ARINC_%1").arg(valueType);
            if(valueType == BOOLEAN_TYPE)
            {
                bit = icdParamData.at(41);
                minVal = icdParamData.at(46);
                maxVal = icdParamData.at(45);
            }
            else if(valueType == FLOAT_TYPE)
            {
                //bit = icdParamData.at(41);
                unit = icdParamData.at(56);
                minVal = icdParamData.at(54);
                maxVal = icdParamData.at(55);
            }
            else if(valueType == INTERGER_TYPE)
            {
                unit = icdParamData.at(66);
                minVal = icdParamData.at(64);
                maxVal = icdParamData.at(65);
            }
        }
        else if(_ParsingFalgeData[A429IN] && icdType == A429_INPUT_LABEL && fieldCount > 66)
        {
            media = "A429";

            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(37).simplified();

            identifier = icdParamData.at(27);
            label = icdParamData.at(6);
            valueType = icdParamData.at(37);
            signalType = QString("ARINC_%1").arg(valueType);

            if(valueType == BOOLEAN_TYPE)
            {
                bit = icdParamData.at(41);
                unit = "NA";
                minVal = icdParamData.at(46);
                maxVal = icdParamData.at(45);

            }
            else if(valueType == FLOAT_TYPE)
            {
                unit = icdParamData.at(56);
                minVal = icdParamData.at(54);
                maxVal = icdParamData.at(55);
            }
            else if(valueType == INTERGER_TYPE)
            {
                unit = icdParamData.at(66);
                minVal = icdParamData.at(64);
                maxVal = icdParamData.at(65);

            }
        }
        else if(fieldCount > 20 && (_ParsingFalgeData[ANAOUT] || _ParsingFalgeData[ANAIN])
                && ( (icdType == ANALOGUE_OUTPUT_SIGNAL)
                     || (icdType == ANALOGUE_INPUT_SIGNAL) ) )
        {
            media = "ANA";
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(20).simplified();

            identifier = icdParamData.at(20);
            label = "NA";
            signalType = "Analogue";
            valueType = "Analogue";
            unit = icdParamData.at(18);
            minVal = icdParamData.at(16);
            maxVal = icdParamData.at(17);
            paramType = "FIB";
        }
        else if(fieldCount > 16 && (_ParsingFalgeData[DESCOUT] || _ParsingFalgeData[DESCIN]) && ((icdType == DISCRETE_OUTPUT_SIGNAL) || (icdType == DISCRETE_INPUT_SIGNAL) ))
        {
            media = "DIS";
            identifier = icdParamData.at(5);
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(16).simplified();


            label = "NA";
            signalType = "Discrete";
            valueType = "Boolean";
            unit = "NA";
            minVal = icdParamData.at(15);
            maxVal = icdParamData.at(14);
        }
        else if((_ParsingFalgeData[CANOUT] && icdType == CAN_OUTPUT_MESSAGE) && fieldCount > 48)
        {
            media = "CAN";
            identifier = icdParamData.at(18);
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(28).simplified();

            label = "NA";
            signalType = "CAN_" + icdParamData.at(29);
            valueType = icdParamData.at(29);
            if( valueType == "Float" )
            {
                unit = icdParamData.at(48);
                minVal = icdParamData.at(46);
                maxVal = icdParamData.at(47);
            }
            else if( valueType == "Boolean" )
            {
                unit = "NA";
                minVal = icdParamData.at(38);
                maxVal = icdParamData.at(37);
            }
        }
        else if(fieldCount > 33 && (_ParsingFalgeData[AFDXOUT] || _ParsingFalgeData[AFDXIN])&&((icdType == AFDX_INPUT_MESSAGE) || (icdType == AFDX_OUTPUT_MESSAGE) ))
        {
            media = "AFDX";
            identifier = icdParamData.at(24);
            appName = icdParamData.at(1).simplified();
            associatedBus = icdParamData.at(4).simplified();
            signalName = icdParamData.at(33).simplified();

            //            hasGenerationTag = false;

        }


        //        if(hasGenerationTag)
        //            paramName = QString("%1_G.%2_G.%3").arg(appName,associatedBus,signalName);
        //        else
        if(! appName.isEmpty() && (! associatedBus.isEmpty()) && (! signalName.isEmpty()))
        {
            pParameter = new GTAICDParameter();
            paramName = QString("%1.%2.%3").arg(appName,associatedBus,signalName);

            pParameter->setName(paramName);
            pParameter->setParameterType(paramType);
            pParameter->setBit(bit);
            //        oParam.setEquipmentName(equipmentName);
            pParameter->setIdentifier(identifier);
            pParameter->setLabel(label);
            pParameter->setMaxValue(maxVal);
            pParameter->setMinValue(minVal);
            pParameter->setSignalType(signalType);
            //oParam.setSourceFile(_sFilePath);
            pParameter->setSourceType("ICD");
            pParameter->setUnit(unit);
            pParameter->setValueType(valueType);
            pParameter->setMedia(media);
            //qDebug()<<pParameter->getName();
        }

    }

    return pParameter;
}

QString GTAICDParser::getEquipmentName()
{
    return _sEquipment;
}

bool GTAICDParser::parsePmrXmlFile()
{
    bool rc = false;
    QXmlStreamReader XmlReader;
    QFile File(_sFilePath);
    rc = File.open(QFile::Text | QFile::ReadOnly);
    if(rc)
    {
        XmlReader.setDevice(&File);
        rc = XmlReader.hasError();
        if(! rc)
        {
            while(!XmlReader.atEnd() && !XmlReader.hasError())
            {
                XmlReader.readNext();
                //QString nodeName = XmlReader.name().toString();
                if(XmlReader.isStartElement() && XmlReader.name().toString() == "Variable")
                {
                    XmlReader.readNextStartElement();
                    QString paramName, valueType;
                    int timeOut = 0;
                    while(paramName.isEmpty() || valueType.isEmpty())
                    {
                        if(XmlReader.readNext() == QXmlStreamReader::StartElement)
                        {
                            if( XmlReader.name().toString() == BISG)
                            {
                                paramName =XmlReader.attributes().value(MAPPING).toString();
                            }
                            else if( XmlReader.name().toString() == TYPE_KEY)
                            {
                                valueType = XmlReader.readElementText();
                            }
                        }
                        if(timeOut == 30)
                            break;

                        timeOut++;
                    }

                    if(!paramName.isEmpty())
                    {
                        paramName = paramName.replace(",",".");
                        GTAICDParameter * pParameter = new GTAICDParameter();
                        pParameter->setName(paramName);
                        pParameter->setValueType(valueType);
                        pParameter->setSourceType("PMR");
                        pParameter->setParameterType("Model");
                        pParameter->setSourceFile(_sFilePath);
						_ICDParamList.append(pParameter);
                    }

                }
            }
        }
        else
        {
            _ErrMessage= QString("Error: GTAICDParser: Fail to read xml file %1\n").arg(_sFilePath);
            _ErrMessage+=XmlReader.errorString();
        }
        File.close();
    }
    else
    {
        _ErrMessage= QString("Error: GTAICDParser: Fail to read xml file %1\n").arg(_sFilePath);
        _ErrMessage+=File.errorString();
    }
    return rc;
}

GTAICDParameter * GTAICDParser::getParameterFromICDL(const QDomNode &iParamNode)
{
    GTAICDParameter * pParameter = NULL;

    if(iParamNode.isElement())
    {

        QDomNamedNodeMap attributes = iParamNode.attributes();
        QString paramName;
        QString valueType;
        QString paramType = "FIB";
        if(attributes.contains("name"))
            paramName = attributes.namedItem("name").nodeValue();
        if(attributes.contains("type"))
            valueType = attributes.namedItem("type").nodeValue();

        pParameter = new GTAICDParameter();
        pParameter->setName(paramName);
        pParameter->setParameterType(paramType);
        pParameter->setSourceType("PMR");
    }
    return pParameter;
}

bool GTAICDParser::parsePIR()
{
    bool rc = false;
    rc = isValidPIRFile();
    if(! rc)
    {
        _ErrMessage = "Error: File " + QString(_sFilePath) + " does not exists or have invalid format";
        return rc;
    }

    QFile fileObj(_sFilePath);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);
    if(rc)
    {
        while (!fileObj.atEnd())
        {
            QString line = fileObj.readLine();
            QStringList elems = line.split(';');
            if(elems.count() >= 7)
            {

                QString paramName = QString("IPR:%1.%2.%3").arg(elems.at(1).simplified(),elems.at(3).simplified(),elems.at(0).simplified());
                QString signalType = elems.at(3).simplified();
                QString valueType = QString("%1(%2)").arg(elems.at(5),elems.at(5));
                QString paramType = "FIB";

                GTAICDParameter * pParameter = new GTAICDParameter();
                pParameter->setName(paramName);
                pParameter->setParameterType(paramType);
                pParameter->setValueType(valueType);
                pParameter->setSignalType(signalType);
                pParameter->setSourceFile(_sFilePath );
                pParameter->setSourceType(PARAM_TYPE_PIR);
                _ICDParamList.append(pParameter);
            }
        }
    }
    else
        _ErrMessage = "Error:  File " + QString(_sFilePath) + " could not open";

    return rc;
}

bool GTAICDParser::isValidPIRFile()
{
    bool rc = false;
    QFile fileObj(_sFilePath);
    rc = fileObj.open(QFile::ReadOnly | QFile::Text);
    if(rc)
    {
        QString line = fileObj.readLine();
        if(! line.isEmpty() && line.count(';') == 6)
        {
            rc = true;
        }
        else rc = false;
        fileObj.close();
    }
    return rc;
}

bool GTAICDParser::getParameterList(QList<GTAICDParameter*> &oList) const
{
    oList = _ICDParamList;
    return (_ICDParamList.count()> 0? true:false);
}
void GTAICDParser::onParseFinish()
{
    /* QFutureWatcher<GTAICDParameter*> * pWatcher = dynamic_cast<QFutureWatcher<GTAICDParameter*>*>(sender());
    if(pWatcher != NULL)
    {
        QFuture<GTAICDParameter*> Future =  pWatcher->future();
        for(int i = 0; i< Future.resultCount() ; i++)
        {
            GTAICDParameter * pParameter =  Future.resultAt(i);
            pParameter->setSourceFile(_sFilePath);
            if (_sFilePath.contains("Generation") && pParameter->getSourceType() == "ICD")
            {
                QString paramName = pParameter->getName().replace(".","_G.");
                pParameter->setName(paramName);
            }
            if(pParameter->getEquipmentName().isEmpty())
                pParameter->setEquipmentName(_sEquipment);

            _ICDParamList.append(pParameter);
        }
        pWatcher->deleteLater();
        pWatcher = NULL;
    }*/
}
void GTAICDParser::onParseFinish(QFutureWatcher<GTAICDParameter*> *& pWatcher)
{
    if(pWatcher != NULL)
    {
        QFuture<GTAICDParameter*> Future =  pWatcher->future();
        for(int i = 0; i< Future.resultCount() ; i++)
        {
            GTAICDParameter * pParameter =  Future.resultAt(i);
            if(pParameter != NULL)
            {
                pParameter->setSourceFile(_sFilePath);
                if (_sFilePath.contains("Generation") && pParameter->getSourceType() == "ICD")
                {
                    QString paramName = pParameter->getName().replace(".","_G.");
                    pParameter->setName(paramName);
                }
                if(pParameter->getEquipmentName().isEmpty())
                    pParameter->setEquipmentName(_sEquipment);

                _ICDParamList.append(pParameter);
            }
        }
        pWatcher->deleteLater();
        pWatcher = NULL;
    }
}
void GTAICDParser::clearParameterList(QList<GTAICDParameter*> & iList)
{
	qDeleteAll(iList.begin(),iList.end());
	return;
}
