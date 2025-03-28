#include "GTAAppController.h"
#include <QApplication>
//#include "GTAMainWindow.h"
//#include "GTAGenSearchWidget.h"
#include <iostream>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include "GTAICDParser.h"
#include "GTAICDParameter.h"
#include <QDomNode>
#include <QTime>
#include <QList>
#include <QSqlQuery>
#include "GTADataController.h"
#include "GTADocumentController.h"
#include "GTAElement.h"
#include <QFuture>
#include <QtConcurrentMap>
#include <QtConcurrentRun>
#include <QDir>
#include <QXmlStreamReader>
#include "GTALocalDbUtil.h"
#define A429_OUTPUT_LABEL "A429_OUTPUT_LABEL"
#define A429_INPUT_LABEL "A429_INPUT_LABEL"
#define ANALOGUE_OUTPUT_SIGNAL "ANALOGUE_OUTPUT_SIGNAL"
#define ANALOGUE_INPUT_SIGNAL "ANALOGUE_INPUT_SIGNAL"
#define DISCRETE_OUTPUT_SIGNAL "DISCRETE_OUTPUT_SIGNAL"
#define DISCRETE_INPUT_SIGNAL "DISCRETE_INPUT_SIGNAL"
#define CAN_OUTPUT_MESSAGE "CAN_OUTPUT_MESSAGE"
#define AFDX_INPUT_MESSAGE "AFDX_INPUT_MESSAGE"
#define AFDX_OUTPUT_MESSAGE "AFDX_OUTPUT_MESSAGE"
#define BOOLEAN_TYPE  "Boolean"
#define FLOAT_TYPE  "Float"
#define INTERGER_TYPE  "Integer"

using namespace std;

enum ParseFlag{A429OUT,A429IN, ANAOUT,ANAIN,DESCOUT,DESCIN,CANOUT,AFDXOUT,AFDXIN,PMRF,MODELF,VCOMSTATIC,PIRF};
void ParseXMLUsingStreamReader();
void WriteParameterParallel();
static QList<GTAICDParameter*> getParameterListFromFile(const QString & iFile);
static void getParameterListFromLine(const QString & iLine , QList<GTAICDParameter*> & oList);
void TestParser();
extern void writeDataToDB(const QList<GTAICDParameter*>  & iParamList);
void TestDataController();
void TestICDDirectDB(const QString & iFile, QSqlQuery & iSqlQuery);
void updateQueryParameterFromICD(const QString & iICDDataLine,const QString & iEquipName, const QHash<ParseFlag,bool> & _ParsingFalgeData,QSqlQuery & iSqlQuery);
void TestLocalDbUtil();
void TestParserFile();


#include <QtTest/QtTest>

class TestQString: public QObject
{
    Q_OBJECT
        private slots:
            void toUpper();
};

void TestQString::toUpper()
{
    QString str = "Hello";
    QVERIFY(str.toUpper() == "HELLO");
}

//QTEST_MAIN(TestQString)
//#include "../GeneratedFiles/Debug/main.moc"

int main(int argc, char *argv[])
{


	

	QApplication gtaApp(argc, argv);
        //gtaApp.setStyle("cleanlooks");

        //GTADocumentController docCtrl;
        //GTAElement * pElem = NULL;
//	docCtrl.openDocument("C:/Users/nakushwac474/Projects/GenericToolAutomation/Test/SA/00_PW/V7.1/DATA/test123.fun",pElem);

	//TestLocalDbUtil();
	//TestDataController();
	//WriteParameterParallel();
	//TestParser();
	//QList<GTAICDParameter>   iList;
	//writeDataToDB(iList);

    TestParserFile();

	return gtaApp.exec();
}
#include "GTAElementViewModel.h"
#include "GTAHeaderTableModel.h"
#include <QTableView>
#include <QHeaderView>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QFileDialog>
void TestLocalDbUtil()
{
	//QString file = "C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS1_V4411SP0808222ICD1_T036 (2).csv";
	//QString file = "C:/Users/nakushwac474/Desktop/GENESTA_2.5_Latest/GENESTA_2.5/DataBases/SA/00_PW/V7.1/03_PIR/PRIM1A_ABF3FA7P0AL2P65.pir";
	QString file = "C:/Users/nakushwac474/Desktop/GENESTA_2.5_Latest/GENESTA_2.5/DataBases/SA/00_PW/V7.1/02_PMR/ICD_IHM_TEST_AUTO.xml";
	//QString file = "C:/Users/nakushwac474/Desktop/GENESTA_2.5_Latest/GENESTA_2.5/DataBases/SA/00_PW/V7.1/02_PMR/PMR_EB1_conf_PW_250_2014_11_03.xml";
	//QString file = "C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/PMR_1000000/PMR_7.0.xml";
	
	QStringList fileList;
	QString dirPath = "C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/PMR_1000000/";
	QDir Dir(dirPath);
	QStringList listOfXmlFile = Dir.entryList(QStringList("*.xml"));
	foreach(QString file,listOfXmlFile)
	{
		fileList.append(dirPath+file);
	}

	QString startTime = QDateTime().currentDateTime().toString();
	
	bool rc = GTALocalDbUtil::openDatabase("C:/temp/db.csv");
	if(rc)
	{
		GTALocalDbUtil::setDefaultParseFlag();
		GTALocalDbUtil::setICDValidFields();
		/*foreach(QString  file, fileList)
		{*/
			QString toto;
            rc = GTALocalDbUtil::updateParameterInFile(file,"","", toto);
			if(!rc)
			{
				qDebug()<<GTALocalDbUtil::getLastError();
			}
		/*}*/
		GTALocalDbUtil::closeDatabase();
	}
	QString endTime = QDateTime().currentDateTime().toString();
	qDebug()<<startTime<<endTime;
	//cout<<startTime.toStdString()<<endTime.toStdString();

}
void TestParser()
{
	QTime time;
	qDebug()<<"Start Parsing"<<time.currentTime();
	//GTAICDParser parser("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/PMR_RSG_06.01.00.xml");
	GTAICDParser parser("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS1_V4411SP0808222ICD1_T036 (2).csv");
	parser.parse();
	qDebug()<<"End Parsing"<<time.currentTime();

	QList<GTAICDParameter*> oList;
	parser.getParameterList(oList);
	qDebug()<<"Start DB Writing"<<time.currentTime();
	writeDataToDB(oList);
	qDebug()<<"End DB Writing"<<time.currentTime();


}

void writeDataToDB(const QList<GTAICDParameter*>  & iParamList)
{

	QSqlDatabase SqlDb = QSqlDatabase::addDatabase("QSQLITE");
	SqlDb.setDatabaseName("C://Temp//GTA_PARAMETER.db");
	bool rc = SqlDb.open();
	qDebug()<<SqlDb.lastError().text();
	if(rc)
	{
		QSqlQuery query(SqlDb);

		QString queryStr = "INSERT INTO PARAMETER(NAME,BIT,FILE,IDENTIFIER,LABEL,MNVAL,MXVAL,PARAMTYPE,SIGNALTYPE,UNIT,VALUETYPE) ";
		//queryStr += "VALUES(:name,:bit,:file,:ident,:label,:min,:max,:paramtype,:signtype,:unit,:valuetype)";
		queryStr += "VALUES(?,?,?,?,?,?,?,?,?,?,?)";
		rc = query.prepare(queryStr);

		if(rc)
		{
			SqlDb.transaction();
			foreach(GTAICDParameter *Param,iParamList)
			{
				query.addBindValue(Param->getName());
				query.addBindValue(Param->getBit());
				query.addBindValue(Param->getSourceFile());
				query.addBindValue(Param->getIdentifier());
				query.addBindValue(Param->getLabel());
				query.addBindValue(Param->getMinValue());
				query.addBindValue(Param->getMaxValue());
				query.addBindValue(Param->getParameterType());
				query.addBindValue(Param->getSignalType());
				query.addBindValue(Param->getUnit());
				query.addBindValue(Param->getValueType());

				rc = query.exec();
			}
			rc = SqlDb.commit();
			if(!rc)
			{
				qDebug()<<"Fail to save "<<SqlDb.lastError().text();

			}
			SqlDb.close();
		}
		else
			qDebug()<<query.lastError().text();
	}

}


void TestDataController()
{
	/*QString dirPath = "C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/";
	QString extension = "*.csv";
	QDir Dir(dirPath);
	QStringList fileList = Dir.entryList(QStringList(extension));
	foreach(QString file,fileList)
	{
		fileList.append(dirPath+file);
	}
	
	QString startTime = QDateTime().currentDateTime().toString();
	GTADataController DataCtrl;
	DataCtrl.setLocalDatabaseFile("C:/TEMP/GTA_PARAMETER.csv");
     QStringList fileToRemoveList, fileToUpdateList;
     bool rc = DataCtrl.getFileListToUpdateParameter(fileList,fileToUpdateList,fileToRemoveList);
	 if(rc)
	 {
		DataCtrl.removeParameterInFiles(fileToRemoveList);
		rc = DataCtrl.updateParameterFromFiles(fileToUpdateList);
		if(!rc)
			qDebug()<<DataCtrl.getLastError();
	 }
	QString endTime = QDateTime().currentDateTime().toString();
	qDebug()<<startTime<<endTime;*/
	//GTADataController * pDataCtrl = new GTADataController("C:/TEMP/GTA_PARAMETER.db");

	//QStringList fileList;
	/*fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS1_V4411SP0808222ICD1_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS1_V4411SP0808222ICD1_T036 (3).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS1_V4411SP0808222ICD1_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS2_V4411SP0808222ICD2_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS2_V4411SP0808222ICD2_T036 (3).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS2_V4411SP0808222ICD2_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC1A_V2471SP0900505ICD1_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC1A_V2471SP0900505ICD1_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC1B_V2471SP0900505ICD2_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC1B_V2471SP0900505ICD2_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC2A_V2471SP0900506ICD1_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC2A_V2471SP0900506ICD1_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC2B_V2471SP0900506ICD2_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC2B_V2471SP0900506ICD2_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/PDMMF_V2471SP0900504ICD1_T032 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/PDMMF_V2471SP0900504ICD1_T032.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/SCI_ADIS_V8900SP0907249ICD1_T035 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/SCI_ADIS_V8900SP0907249ICD1_T035.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/vecteurAvion28.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/vecteurAvion34.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/vecteurAvion52.csv");*/

	/*QString dirPath = "C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/PMR_1000000/";
	QDir Dir(dirPath);
	QStringList listOfXmlFile = Dir.entryList(QStringList("*.xml"));
	foreach(QString file,listOfXmlFile)
	{
	fileList.append(dirPath+file);
	}*/
// 	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/PMR_1000000/PMR_1869735.xml");
// 	QString startTime = QDateTime().currentDateTime().toString();
// 	GTADataController DataCtrl;
// 	DataCtrl.setLocalDatabaseFile("C:/TEMP/GTA_PARAMETER.db");
// 	DataCtrl.updateParameterFromFiles(fileList);
// 	QString endTime = QDateTime().currentDateTime().toString();
// 	qDebug()<<startTime<<endTime;
}
void WriteParameterParallel()
{
	QStringList fileList;
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS1_V4411SP0808222ICD1_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS1_V4411SP0808222ICD1_T036 (3).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS1_V4411SP0808222ICD1_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS2_V4411SP0808222ICD2_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS2_V4411SP0808222ICD2_T036 (3).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/CIDS2_V4411SP0808222ICD2_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC1A_V2471SP0900505ICD1_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC1A_V2471SP0900505ICD1_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC1B_V2471SP0900505ICD2_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC1B_V2471SP0900505ICD2_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC2A_V2471SP0900506ICD1_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC2A_V2471SP0900506ICD1_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC2B_V2471SP0900506ICD2_T036 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/EPDC2B_V2471SP0900506ICD2_T036.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/PDMMF_V2471SP0900504ICD1_T032 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/PDMMF_V2471SP0900504ICD1_T032.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/SCI_ADIS_V8900SP0907249ICD1_T035 (2).csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/SCI_ADIS_V8900SP0907249ICD1_T035.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/vecteurAvion28.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/vecteurAvion34.csv");
	fileList.append("C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/BigICDs/BigICDs/vecteurAvion52.csv");
	//  QList<QFuture<QList<GTAICDParameter*>>> FutureList;
	//  foreach(QString file, fileList)
	//  {
	//      QtConcurrent::run()
	//      QFuture<QList<GTAICDParameter*>> Future = QtConcurrent::run(getParameterListFromFile,file);
	//QFutureWatcher<QList<GTAICDParameter*>> * pWatcher = new QFutureWatcher<QList<GTAICDParameter*>>();
	//pWatcher->setFuture(Future);
	//        //connect(pWatcher,SIGNAL())
	//  }



	//    QFutureWatcher<void> * pWatcher = new QFutureWatcher<void>();
	//     pWatcher->setFuture(QtConcurrent::mappedReduced(fileList,&getParameterListFromFile,writeDataToDB,QtConcurrent::UnorderedReduce));
	//	pWatcher->waitForFinished();
}

static QList<GTAICDParameter*> getParameterListFromFile(const QString & iFile)
{
	QList<GTAICDParameter*>  oList;
	QHash<ParseFlag,bool> _ParsingFalgeData;
	_ParsingFalgeData[A429OUT] = true;
	_ParsingFalgeData[A429IN] = true;
	_ParsingFalgeData[ANAOUT] = true;
	_ParsingFalgeData[ANAIN] = true;
	_ParsingFalgeData[DESCOUT] = true;
	_ParsingFalgeData[DESCIN] = true;
	_ParsingFalgeData[CANOUT] = true;
	_ParsingFalgeData[AFDXOUT] = true;
	_ParsingFalgeData[AFDXIN] = true;
	_ParsingFalgeData[PMRF] = true;
	_ParsingFalgeData[MODELF] = true;
	_ParsingFalgeData[VCOMSTATIC] = true;
	_ParsingFalgeData[PIRF] = true;

	QStringList validFields;

	validFields<<A429_OUTPUT_LABEL<<A429_INPUT_LABEL<<ANALOGUE_OUTPUT_SIGNAL;
	validFields<<ANALOGUE_INPUT_SIGNAL<<DISCRETE_OUTPUT_SIGNAL<<DISCRETE_INPUT_SIGNAL;
	validFields<<CAN_OUTPUT_MESSAGE<<AFDX_INPUT_MESSAGE<<AFDX_OUTPUT_MESSAGE;
	bool rc = false;
	QFile fileObj(iFile);
	rc = fileObj.open(QFile::ReadOnly | QFile::Text);

	if (! rc)
	{
		qDebug()<<"Error: Fail to open file " + QString(iFile);
		return oList;
	}
	QString EquipmentName ;
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
					EquipmentName = equipData.at(1);

				continue;
			}

			foreach(QString key, validFields)
			{
				if( line.startsWith(key))
				{
					//listOfDataLine.append(line);
					getParameterListFromLine(line,oList);
					break;
				}
			}
		}
	}
	fileObj.close();
	return oList;
}
void getParameterListFromLine(const QString & iLine , QList<GTAICDParameter*> & oList)
{
	QHash<ParseFlag,bool> _ParsingFalgeData;
	_ParsingFalgeData[A429OUT] = true;
	_ParsingFalgeData[A429IN] = true;
	_ParsingFalgeData[ANAOUT] = true;
	_ParsingFalgeData[ANAIN] = true;
	_ParsingFalgeData[DESCOUT] = true;
	_ParsingFalgeData[DESCIN] = true;
	_ParsingFalgeData[CANOUT] = true;
	_ParsingFalgeData[AFDXOUT] = true;
	_ParsingFalgeData[AFDXIN] = true;
	_ParsingFalgeData[PMRF] = true;
	_ParsingFalgeData[MODELF] = true;
	_ParsingFalgeData[VCOMSTATIC] = true;
	_ParsingFalgeData[PIRF] = true;
	QString iICDDataLine = iLine;
	if(! iICDDataLine.isEmpty())
	{
		QString paramName;

		QStringList icdParamData = iICDDataLine.split(";");
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
		int fieldCount = icdParamData.count();
		if (_ParsingFalgeData[A429OUT] && icdType == A429_OUTPUT_LABEL && fieldCount > 66)
		{
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
			identifier = icdParamData.at(5);
			appName = icdParamData.at(1).simplified();
			associatedBus = icdParamData.at(4).simplified();
			signalName = icdParamData.at(16).simplified();


			label = "NA";
			signalType = "Discret";
			valueType = "Boolean";
			unit = "NA";
			minVal = icdParamData.at(15);
			maxVal = icdParamData.at(14);
		}
		else if((_ParsingFalgeData[CANOUT] && icdType == CAN_OUTPUT_MESSAGE) && fieldCount > 48)
		{
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
			identifier = icdParamData.at(24);
			appName = icdParamData.at(1).simplified();
			associatedBus = icdParamData.at(4).simplified();
			signalName = icdParamData.at(33).simplified();

			//            hasGenerationTag = false;

		}

		paramName = QString("%1.%2.%3").arg(appName,associatedBus,signalName);
		//        iSqlQuery.addBindValue(paramName);
		//        iSqlQuery.addBindValue(bit);
		//        iSqlQuery.addBindValue("XFile");
		//        iSqlQuery.addBindValue(identifier);
		//        iSqlQuery.addBindValue(label);
		//        iSqlQuery.addBindValue(minVal);
		//        iSqlQuery.addBindValue(maxVal);
		//        iSqlQuery.addBindValue(paramType);
		//        iSqlQuery.addBindValue(signalType);
		//        iSqlQuery.addBindValue(unit);
		//        iSqlQuery.addBindValue(valueType);
		//        iSqlQuery.addBindValue(iEquipName);
		//        //QFileInfo fileInfo("XFile");
		//        QDateTime lastModifiedDate = QDateTime().currentDateTime();//fileInfo.lastModified();
		//        iSqlQuery.addBindValue(lastModifiedDate);

		//        bool rc = iSqlQuery.exec();


		//        if(hasGenerationTag)
		//            paramName = QString("%1_G.%2_G.%3").arg(appName,associatedBus,signalName);
		//        else
		if(! appName.isEmpty() && (! associatedBus.isEmpty()) && (! signalName.isEmpty()))
		{
			GTAICDParameter *  pParameter = new GTAICDParameter();
			paramName = QString("%1.%2.%3").arg(appName,associatedBus,signalName);
			pParameter->setName(paramName);

            QString tempName = paramName;
            pParameter->setTempParamName(tempName);
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
			oList.append(pParameter);
			//qDebug()<<pParameter->getName();
		}

	}
}


void TestICDDirectDB(const QString & iFile, QSqlQuery & iSqlQuery)
{

	QHash<ParseFlag,bool> _ParsingFalgeData;
	_ParsingFalgeData[A429OUT] = true;
	_ParsingFalgeData[A429IN] = true;
	_ParsingFalgeData[ANAOUT] = true;
	_ParsingFalgeData[ANAIN] = true;
	_ParsingFalgeData[DESCOUT] = true;
	_ParsingFalgeData[DESCIN] = true;
	_ParsingFalgeData[CANOUT] = true;
	_ParsingFalgeData[AFDXOUT] = true;
	_ParsingFalgeData[AFDXIN] = true;
	_ParsingFalgeData[PMRF] = true;
	_ParsingFalgeData[MODELF] = true;
	_ParsingFalgeData[VCOMSTATIC] = true;
	_ParsingFalgeData[PIRF] = true;

	QStringList validFields;

	validFields<<A429_OUTPUT_LABEL<<A429_INPUT_LABEL<<ANALOGUE_OUTPUT_SIGNAL;
	validFields<<ANALOGUE_INPUT_SIGNAL<<DISCRETE_OUTPUT_SIGNAL<<DISCRETE_INPUT_SIGNAL;
	validFields<<CAN_OUTPUT_MESSAGE<<AFDX_INPUT_MESSAGE<<AFDX_OUTPUT_MESSAGE;
	bool rc = false;
	QFile fileObj(iFile);
	rc = fileObj.open(QFile::ReadOnly | QFile::Text);

	if (! rc)
	{
		qDebug()<<"Error: Fail to open file " + QString(iFile);
		return;
	}
	QString EquipmentName ;
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
					EquipmentName = equipData.at(1);

				continue;
			}

			foreach(QString key, validFields)
			{
				if( line.startsWith(key))
				{
					listOfDataLine.append(line);
					updateQueryParameterFromICD(line,EquipmentName,_ParsingFalgeData,iSqlQuery);
					break;
				}
			}
		}
	}
	fileObj.close();

}
void updateQueryParameterFromICD(const QString & iICDDataLine,const QString & iEquipName, const QHash<ParseFlag,bool> & _ParsingFalgeData,QSqlQuery & iSqlQuery)
{

	if(! iICDDataLine.isEmpty())
	{
		QString paramName;

		QStringList icdParamData = iICDDataLine.split(";");
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
		int fieldCount = icdParamData.count();
		if (_ParsingFalgeData[A429OUT] && icdType == A429_OUTPUT_LABEL && fieldCount > 66)
		{
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
			identifier = icdParamData.at(5);
			appName = icdParamData.at(1).simplified();
			associatedBus = icdParamData.at(4).simplified();
			signalName = icdParamData.at(16).simplified();


			label = "NA";
			signalType = "Discret";
			valueType = "Boolean";
			unit = "NA";
			minVal = icdParamData.at(15);
			maxVal = icdParamData.at(14);
		}
		else if((_ParsingFalgeData[CANOUT] && icdType == CAN_OUTPUT_MESSAGE) && fieldCount > 48)
		{
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
			identifier = icdParamData.at(24);
			appName = icdParamData.at(1).simplified();
			associatedBus = icdParamData.at(4).simplified();
			signalName = icdParamData.at(33).simplified();

			//            hasGenerationTag = false;

		}

		paramName = QString("%1.%2.%3").arg(appName,associatedBus,signalName);
		iSqlQuery.addBindValue(paramName);
		iSqlQuery.addBindValue(bit);
		iSqlQuery.addBindValue("XFile");
		iSqlQuery.addBindValue(identifier);
		iSqlQuery.addBindValue(label);
		iSqlQuery.addBindValue(minVal);
		iSqlQuery.addBindValue(maxVal);
		iSqlQuery.addBindValue(paramType);
		iSqlQuery.addBindValue(signalType);
		iSqlQuery.addBindValue(unit);
		iSqlQuery.addBindValue(valueType);
		iSqlQuery.addBindValue(iEquipName);
		//QFileInfo fileInfo("XFile");
		QDateTime lastModifiedDate = QDateTime().currentDateTime();//fileInfo.lastModified();
		iSqlQuery.addBindValue(lastModifiedDate);

		/*bool rc =*/ iSqlQuery.exec();


		//        //        if(hasGenerationTag)
		//        //            paramName = QString("%1_G.%2_G.%3").arg(appName,associatedBus,signalName);
		//        //        else
		//        if(! appName.isEmpty() && (! associatedBus.isEmpty()) && (! signalName.isEmpty()))
		//        {
		//            pParameter = new GTAICDParameter();
		//            paramName = QString("%1.%2.%3").arg(appName,associatedBus,signalName);

		//            pParameter->setName(paramName);
		//            pParameter->setParameterType(paramType);
		//            pParameter->setBit(bit);
		//            //        oParam.setEquipmentName(equipmentName);
		//            pParameter->setIdentifier(identifier);
		//            pParameter->setLabel(label);
		//            pParameter->setMaxValue(maxVal);
		//            pParameter->setMinValue(minVal);
		//            pParameter->setSignalType(signalType);
		//            //oParam.setSourceFile(_sFilePath);
		//            pParameter->setSourceType("ICD");
		//            pParameter->setUnit(unit);
		//            pParameter->setValueType(valueType);
		//            qDebug()<<pParameter->getName();
		//        }

	}

}
void ParseXMLUsingStreamReader()
{
	//QString xmlFile = "C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/PMR_1000000/PMR_5.xml";
	QString xmlFile = "C:/Users/nakushwac474/Projects/FlightOperation/GenericToolAutomation/Test/PMR_1000000/PMR_1869735.xml";

	QXmlStreamReader StreamReader;
	QFile File(xmlFile);
	if(File.open(QFile::Text | QFile::ReadOnly))
	{
		StreamReader.setDevice(&File);
		if(StreamReader.hasError())
		{
			qDebug()<<StreamReader.errorString();
			return;
		}
		StreamReader.readNextStartElement();
		while(!StreamReader.atEnd() && !StreamReader.hasError())
		{
			StreamReader.readNext();
			QString nodeName = StreamReader.name().toString();
			//qDebug()<<"node name: "<<nodeName;
			if(StreamReader.isStartElement() && StreamReader.name().toString() == "Variable")
			{
				StreamReader.readNextStartElement();
				QString bisgNode, typeNode;
				while(bisgNode.isEmpty() || typeNode.isEmpty())
				{
					if(StreamReader.readNext() == QXmlStreamReader::StartElement)
					{
						if( StreamReader.name().toString() == "Bisg")
						{
							bisgNode =StreamReader.attributes().value("mapping").toString();
						}
						else if( StreamReader.name().toString() == "Type")
						{
							typeNode = StreamReader.readElementText();
						}
					}


				}
				//qDebug()<<bisgNode<<typeNode;

			}
			//qDebug()<<"Error: "<<StreamReader.errorString();

			//             if(StreamReader.isEndElement() && StreamReader.name().toString() == "Variables")
			//                 break;
		}
	}

}
