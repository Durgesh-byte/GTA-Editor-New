#include "AINCMNCSVFileManager.h"

#include <QFile>
#include <QTextStream>
#include <QHash>
#include <QVariant>


#include "AINCMNCommonServices.h"
using namespace AINCMN::CMNUtils;
#include "AINCMNProgressDialog.h"
using namespace AINCMN::Dialogs;

#define NEW_LINE "\n"
#define DOUBLE_QUOTE "\""

AINCMNCSVFileManager::AINCMNCSVFileManager(QString iFileSep, QObject *parent) :
    QObject(parent), _pErrLogger(0)
{
    _file_sep = iFileSep;
}

AINCMNCSVFileManager::~AINCMNCSVFileManager()
{
}


void AINCMNCSVFileManager::setErrorLogger(AINCMNErrorLogger *& pErrLogger)
{
	_pErrLogger = pErrLogger;
}

//First Line will always be header/column_name of the CSV file
bool AINCMNCSVFileManager::read(const QString& iAbsCsvFile, 
													QList<QStringList>& ioFileContent)
{
	bool bOk = read(iAbsCsvFile, _file_sep, ioFileContent);
	
	return bOk;
}

bool AINCMNCSVFileManager::read(const QString& iAbsCsvFile, const QString& iFileSep,
													QList<QStringList>& ioFileContent)
{
	bool bOk(true);

	QFile file(iAbsCsvFile);
	//bOk = file.setPermissions(QFile::ReadOther);
	bOk = file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (! bOk)
	{
		AINCMNCommonServices::display("Error: File Opening", iAbsCsvFile);
		return bOk;	
	}

	QTextStream in(&file);
	int iRow = 0;
	QStringList listOfColumns;

	while(! in.atEnd()) 
	{
		iRow++;		
		QString line = in.readLine();				
		if (1 == iRow)
		{
			listOfColumns = line.split(iFileSep);
			ioFileContent.append(listOfColumns);
		}
		else {
			QStringList rowContent = splitString(line, iFileSep);
			if (rowContent.count() == listOfColumns.count())
			{					
				ioFileContent.append(rowContent);
			}
			else {
				QVariant vRow(iRow);
				QString sMsg(" Err:CSV Parsing: Header & RowContents have mismatch #Header=" + QString::number(listOfColumns.count()) + " #RowCols=" + QString::number(rowContent.count()));
				sMsg += QString((" FileName:") + iAbsCsvFile + QString("\nRowNumber=") + vRow.toString());
				sMsg += QString(" OR \nInput FileSeparator is not correct \n CorrectFileSep=" + iFileSep);

				if (_pErrLogger)
				{
					_pErrLogger->writeLine(sMsg);
				}
				else {					
					AINCMNCommonServices::display("Error:File Reading", sMsg);
				}
				
				continue;
			}
		}
	}

	/* Close the file */
	file.close();

	return bOk;
}

//First Line will always be header/column_name of the CSV file
bool AINCMNCSVFileManager::read(const QString& iAbsCsvFile, 
									QList<QHash<QString, QString>>& ioListOfTableRowsWithHeader)
{
	bool bOk(true);
	
	QFile file(iAbsCsvFile);
	bOk = file.setPermissions(QFile::ReadOther);
	bOk = file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (! bOk)
	{
		AINCMNCommonServices::display("Error: File Opening", iAbsCsvFile);
		return bOk;	
	}
		
	QTextStream in(&file);
	int iRow = 0;
	QStringList listOfColumns;
	
	while(! in.atEnd()) 
	{	
		iRow++;
		QString line = in.readLine();
			
		if (1 == iRow)
		{
			listOfColumns = line.split(_file_sep);
		}
		else {
			QHash<QString, QString> hashOfRowContent;
			QStringList rowContent = splitString(line, _file_sep);
			if (rowContent.count() == listOfColumns.count())
			{					
				for (int i=0; i<listOfColumns.count(); i++)
				{
					hashOfRowContent.insert(listOfColumns.at(i), rowContent.at(i));
				}
				
				ioListOfTableRowsWithHeader.append(hashOfRowContent);
			}
			else {
				QVariant vRow(iRow);
				QString sMsg(" Err:CSV Parsing: Header & RowContents have mismatch #Header=" + QString::number(listOfColumns.count()) + " #RowCols=" + QString::number(rowContent.count()));
				sMsg += QString((" FileName:") + iAbsCsvFile + QString("\nRowNumber=") + vRow.toString());
				sMsg += QString(" OR \nInput FileSeparator is not correct \n CorrectFileSep=" + _file_sep);

				if (_pErrLogger)
				{
					_pErrLogger->writeLine(sMsg);
				}
				else {					
					AINCMNCommonServices::display("Error:File Reading", sMsg);
				}

				continue;		
			}
		}
	}
	
	/* Close the file */
	file.close();
	
	return bOk;
}

//First Line will always be header/column_name of the CSV file
bool AINCMNCSVFileManager::write(const QString& iAbsPathCSVFile, 
								const QList<QStringList>& iFileContent)
{
	QStringList listOfLines = createCSVFileContent(iFileContent, _file_sep);
	bool bOk = write(iAbsPathCSVFile, listOfLines);
	
	return bOk;
}


bool AINCMNCSVFileManager::write(const QString& iAbsPathCSVFile,
								const QList<QStringList>& iFileContent, const QString& iFileSep)
{
	QStringList listOfLines = createCSVFileContent(iFileContent, iFileSep);
	bool bOk = write(iAbsPathCSVFile, listOfLines);
	
	return bOk;
}

bool AINCMNCSVFileManager::write(const QString& iAbsPathCSVFile, const QStringList& iListOfLines)
{
	/* Try and open a file for output */
	QFile outputFile(iAbsPathCSVFile);
	outputFile.open(QIODevice::WriteOnly | QIODevice::Text);

	/* Check it opened OK */
	if(!outputFile.isOpen())
	{
		AINCMNCommonServices::display("Error: File Writing", "Unable to create the file for output");
		return false;
	}

	/* Point a QTextStream object at the file */
	QTextStream outStream(&outputFile);

	/* Write the line to the file */
	for (int i=0; i<iListOfLines.count(); i++)
		outStream << iListOfLines.at(i) << NEW_LINE;
	
	/* Close the file */
	outputFile.close();
	
	return true;
}

bool AINCMNCSVFileManager::write(const QString& iAbsPathCSVFile, 
								QList<QHash<QString, QString>>& iHashOfTable)
{
	bool bOk = write(iAbsPathCSVFile, iHashOfTable, _file_sep);

	return bOk;
}

bool AINCMNCSVFileManager::write(const QString& iAbsPathCSVFile, 
				QList<QHash<QString, QString>>& iHashOfTable, const QString& iFileSep)
{
	QString sDQuote(DOUBLE_QUOTE);
	QList<QStringList> lstOfTableRowsWithHeader;
	for (int i=0; i<iHashOfTable.count(); i++)
	{
		QStringList lstKeyValues;
		QStringList lstKeys = iHashOfTable.at(i).keys();
		if (0==i)
		{
			//lstOfTableRowsWithHeader << lstKeys; //Major change after long time
			QStringList lstOfKeysNew;
			for(int j=0; j<lstKeys.count(); j++)
			{
				QString str = sDQuote + lstKeys.at(j) + sDQuote;
				lstOfKeysNew << str;
			}
			lstOfTableRowsWithHeader << lstOfKeysNew;
		}
		
		for (int j=0; j<lstKeys.count(); j++)
		{
			QString sVal = iHashOfTable.at(i).value(lstKeys.at(j));
			//lstKeyValues << sVal; //Major change after long time

			QString str = sDQuote + sVal + sDQuote;
			lstKeyValues << str;
		}
		lstOfTableRowsWithHeader << lstKeyValues;
	}
	
	bool bOk = write(iAbsPathCSVFile, lstOfTableRowsWithHeader, iFileSep);
	
	return bOk;
}

bool AINCMNCSVFileManager::changeFileSepOfCSVFile(const QString& iInpFileAbsPath, const QString& iInpFileSep,
						   const QString& iOutFileAbsPath, const QString& iOutFileSep)
{
	QList<QStringList> fileContent;
	bool bOk = read(iInpFileAbsPath, iInpFileSep, fileContent);
	if (bOk)
	{
		bOk = write(iOutFileAbsPath, fileContent, iOutFileSep);
	}
	
	return bOk;
}

QStringList AINCMNCSVFileManager::createCSVFileContent(
				const QList<QStringList>& iListOfTableContent, const QString& iFileSep)
{
	QStringList listCSVFileLines;

	//QString sFileHeader = createCSVLine(iListOfColumns);
	//listCSVFileLines.append(sFileHeader);

	for (int i=0; i<iListOfTableContent.count(); i++)
	{
		QString sCSVLine = createCSVLine(iListOfTableContent.at(i),iFileSep);
		listCSVFileLines.append(sCSVLine);
	}

	return listCSVFileLines;
}

QString AINCMNCSVFileManager::createCSVLine(const QStringList& iLineContent, const QString& iFileSep)
{
	QString doubleQuote(DOUBLE_QUOTE);
	QString sLine("");
	for (int i=0; i<iLineContent.count(); i++)
	{
		if ( i > 0 )
			sLine += iFileSep;
		
		if (iLineContent.at(i).contains(iFileSep))
			sLine += (doubleQuote + iLineContent.at(i) + doubleQuote);
		else
			sLine += iLineContent.at(i);
	}

	return sLine;
}

QString AINCMNCSVFileManager::createCSVLine(const QStringList& iLineContent)
{	
	return createCSVLine(iLineContent, _file_sep);
}

QStringList AINCMNCSVFileManager::createListOfCSVLines(const QList<QStringList>& iFileContent)
{
	QStringList listOfLines;
	for (int i=0; i<iFileContent.count(); i++)
		listOfLines.append(createCSVLine(iFileContent.at(i)));
	
	return listOfLines;
}

QList<QStringList> AINCMNCSVFileManager::getFileContent(const QStringList& iListOfLines)
{
	QList<QStringList> iFileContent;
	for (int i=0;i<iListOfLines.count(); i++)
		iFileContent.append(breakCSVLine(iListOfLines.at(i)));
	
	return iFileContent;
}

QStringList AINCMNCSVFileManager::breakCSVLine(const QString& iLine)
{
	QStringList splitList = iLine.split(iLine);
	int cnt = splitList.count();
	
	if (cnt > 0)
	{
		QString sLastWord = splitList.at(cnt-1);
		splitList.replace(cnt-1, sLastWord.remove(NEW_LINE));
	}
	
	return splitList;
}

QStringList AINCMNCSVFileManager::splitString(const QString& iInpStr, const QString& iFileSep)
{
	QStringList finalList;
	
	if (! iInpStr.contains(DOUBLE_QUOTE))
	{
		finalList = iInpStr.split(iFileSep);
		return finalList;
	}
	
	QStringList dcSplits = iInpStr.split(DOUBLE_QUOTE);
	//if (dcSplits.at(0).isNull())
	//{
		bool isEven = false;
		for (int i=0; i<dcSplits.count(); i++)
		{
			if (! isEven )
			{
				QStringList splits = dcSplits.at(i).split(iFileSep);
				if (i == 0)
				{
					if (splits.count() == 1)
					{
						//do nothing as first field is starting with double quote
					}
					else {
						for (int j=0; j<splits.count()-1; j++)
						{
							finalList.append(splits.at(j));
						}
					}
				}
				else if (i == dcSplits.count()-1)
				{
					for (int j=1; j<splits.count(); j++)
					{
						finalList.append(splits.at(j));
					}
				}
				else {
					for (int j=1; j<splits.count()-1; j++)
					{
						finalList.append(splits.at(j));
					}				
				}
			}
			else {
				finalList.append(dcSplits.at(i));
			}
			
			if (isEven)
				isEven = false;
			else
				isEven = true;
		}
	//}
	
	return finalList;
}
bool AINCMNCSVFileManager::getTableColumns(const QString& iAbsCsvFile, const QString& iFileSep,
								QStringList& ioColumns)
{
	bool bOk(true);

	QFile file(iAbsCsvFile);
	//bOk = file.setPermissions(QFile::ReadOther);
	bOk = file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (! bOk)
	{
		AINCMNCommonServices::display("Error: File Opening", iAbsCsvFile);
		return bOk;	
	}

	QTextStream in(&file);
	int iRow = 0;
	

	while(! in.atEnd()) 
	{
		iRow++;		
		QString line = in.readLine();				
		if (1 == iRow)
			ioColumns = line.split(iFileSep);
		break;
	}

	/* Close the file */
	file.close();

	return bOk;
}
 

bool AINCMNCSVFileManager::getValidDataTypesForTable(const QString& iAbsCsvFile, const QString& iFileSep,
							  QList<QStringList>& iohshColumnsTypeList)
{
	bool bOk(true);
	QList<QStringList> fileContent;
	read(iAbsCsvFile,iFileSep,fileContent);
	iohshColumnsTypeList.clear();

	QStringList lstofCols;
	getTableColumns(iAbsCsvFile,iFileSep,lstofCols);
	QStringList strListAllDataTypes;
	strListAllDataTypes<< "bool"<<"double" <<"int" <<"varchar(255)";

	foreach(QString sColName,lstofCols)
	{		
		iohshColumnsTypeList.append(strListAllDataTypes);
	}

	
	AINCMNProgressDialog * pProgressBar=0;
	int dFileContentSize=fileContent.size();


	if (dFileContentSize>=100)
	{
		pProgressBar = new AINCMNProgressDialog("Collecting Data Types", 0, dFileContentSize, false);
		pProgressBar->show();
	}
	

	for(int r=1;r<dFileContentSize;r++)
	{
		if (pProgressBar)
		{
			pProgressBar->setValue(r);
		}
		QStringList rowVals=fileContent.at(r);
		int cols=rowVals.size();
		for (int i=0;i<cols;i++)
		{
			//check for bool.
			QString sCurrCellVal = rowVals.at(i);
			QString currentColName = lstofCols.at(i);
			bool canBeBool=false;
			canBeBool = (sCurrCellVal=="1") | 
						(sCurrCellVal=="0") | 
						(sCurrCellVal.compare(QString("TRUE"),Qt::CaseInsensitive)==0)|
						(sCurrCellVal.compare(QString("FALSE"),Qt::CaseInsensitive)==0)|
						(sCurrCellVal=="");


			if (!canBeBool)
			{
	
					(iohshColumnsTypeList[i]).removeOne(QString("bool"));//.removeOne(QString("bool"));
				

				
			}

			bool bIntConvStatus;
			bool bDoubleConvStatus;
			
			sCurrCellVal.toDouble(&bDoubleConvStatus);
			sCurrCellVal.toInt(&bIntConvStatus);

			if (!bIntConvStatus  && sCurrCellVal!="")
			{

					iohshColumnsTypeList[i].removeOne(QString("int"));
				
			}

			if (!bDoubleConvStatus && sCurrCellVal!="")
			{

					iohshColumnsTypeList[i].removeOne(QString("double"));
				
			}



		}

	}

	if (pProgressBar)
	{
		pProgressBar->close();
		delete pProgressBar;
	}
	return bOk;

}