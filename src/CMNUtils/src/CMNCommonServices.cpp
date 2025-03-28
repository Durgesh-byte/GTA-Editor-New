#include "AINCMNCommonServices.h"
using namespace AINCMN::CMNUtils;

#include <QDir>
#include <QMessageBox>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDirIterator>

bool AINCMNCommonServices::fileExists(const QString& iAbsoluteFilePath, bool bMsgDisplay)
{
	QFileInfo iFileInfo(iAbsoluteFilePath);
	
	if (iFileInfo.exists())
		return true;
	else if (bMsgDisplay && !iAbsoluteFilePath.isNull())
	{
		QMessageBox MsgBox;
		MsgBox.setWindowTitle("Error");
		MsgBox.setText("FileInfo: "+ iAbsoluteFilePath + " doesn't exists");
		MsgBox.exec();
	}
	
	return false;
}

bool AINCMNCommonServices::folderExists(const QString& iAbsoluteDirPath, bool bMsgDisplay)
{
	return fileExists(iAbsoluteDirPath, bMsgDisplay);
}

bool AINCMNCommonServices::createFolder(const QString& iDirAbsPath)
{
	QDir dir;
	bool bOk = dir.mkpath(iDirAbsPath);
	
	return bOk;
}

bool AINCMNCommonServices::cleanDir(const QString& iAbsoluteDirPath)
{
	bool bOk = true;
	QStringList filters = QStringList() << "*.*";
	QStringList files = getFileNames(iAbsoluteDirPath, filters);
	for (int i=0; i<files.count(); i++)
	{
		bool isIt = removeFile(files.at(i));
		if (! isIt)
			bOk = false;
	}
	
	return bOk;
}

bool AINCMNCommonServices::fileExistsInFolder(const QString& iAbsoluteDirPath, const QString& iFileName)
{
	QString iAbsoluteFilePath;
	iAbsoluteFilePath.append(iAbsoluteDirPath + QDir::separator() + iFileName);
	
	return fileExists(iAbsoluteFilePath);
}

void AINCMNCommonServices::display(const QString& iTitle, const QString& iMsg)
{
	QMessageBox MsgBox;
	MsgBox.setWindowTitle(iTitle);
	MsgBox.setText(iMsg);
	MsgBox.exec();
}

bool AINCMNCommonServices::copyFile(const QString& iAbsPathOfFile, const QString& iAbsPathOfNewFile)
{
	bool bOk(false);
	QFile file(iAbsPathOfFile);
	
	if (file.exists())	
	{
		bOk = file.copy(iAbsPathOfNewFile);	
		if (! bOk)
			display("FileSystem", iAbsPathOfFile + QString(" couldn't be copied at \n") + iAbsPathOfNewFile );
	}
			
	return bOk; 
}

bool AINCMNCommonServices::removeFile(const QString& iAbsPathOfFile)
{
	bool bOk(true);
	QFile file(iAbsPathOfFile);
	if (file.exists())	
	{
		bOk = file.remove();	
		if (! bOk)
			display("FileSystem", iAbsPathOfFile + QString(" couldn't be removed!"));
	}

	return bOk;
}

QStringList AINCMNCommonServices::getFileNames(const QString& iAbsPathOfDir, 
										  const QStringList& iFilters)
{
	QStringList listOfFiles;

	QDir dir(iAbsPathOfDir);
	if (dir.exists())
	{
		dir.setNameFilters(iFilters);

		QFileInfoList infoList = dir.entryInfoList();
		for (int i=0; i<infoList.count(); i++)
		{
			QString filename = infoList.at(i).baseName();
			listOfFiles.append(filename);
		}
	}	
	return listOfFiles;
}

QStringList AINCMNCommonServices::getAllFileNames(const QString& iAbsPathOfDir, 
											   const QStringList& iFilters)
{
	QStringList listOfFiles;

	QDirIterator it(iAbsPathOfDir, iFilters, QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		listOfFiles << it.next();
	}
	return listOfFiles;
}

QString AINCMNCommonServices::getBaseName(const QString& iAbsPathOfFile)
{
	QFileInfo fileInfo(iAbsPathOfFile);
	
	return fileInfo.baseName();
}

QStringList AINCMNCommonServices::getAbsFiles(const QString& iAbsPathOfDir, 
										  const QStringList& iFilters)
{
	QStringList listOfFiles;

	QDir dir(iAbsPathOfDir);
	if (dir.exists())
	{
		dir.setNameFilters(iFilters);

		QFileInfoList infoList = dir.entryInfoList();
		for (int i=0; i<infoList.count(); i++)
		{
			QString filename = infoList.at(i).absoluteFilePath();
			listOfFiles.append(filename);
		}
	}	
	return listOfFiles;
}

QString AINCMNCommonServices::treatSpace(const QString& sString, QString sSearchString)
{
	QString sOutput("");
	QStringList sStringSplit = sString.split(sSearchString, QString::SkipEmptyParts);	
	for (int i=0; i<sStringSplit.count(); i++)
	{
		if ( i )
			sOutput += sSearchString;
		sOutput += sStringSplit.at(i);
	}
	
	return sOutput;
}

QStringList AINCMNCommonServices::removeEmptyString(const QStringList& iList)
{
	QStringList lstOfNonEmptyStr;
	for (int i=0; i<iList.count(); i++)
	{
		QString sVal = treatSpace(iList.at(i));
		if (sVal != "")
		{
			lstOfNonEmptyStr.append(sVal);
		}
	}
	
	return lstOfNonEmptyStr;
}

QStringList AINCMNCommonServices::getCommonStrings(const QStringList& iList1, 
															const QStringList& iList2)
{
	QStringList lstOfCmnStr;
	QStringList lst1; QStringList lst2;
	if (iList1.count() > iList2.count())
	{
		lst1 = iList1;
		lst2 = iList2;
	}
	else {
		lst1 = iList2;
		lst2 = iList1;
	}
	for (int i=0; i<lst2.count(); i++)
	{
		if (lst1.contains(lst2.at(i)))
			lstOfCmnStr.append(lst2.at(i));
	}

	return lstOfCmnStr;
}

void AINCMNCommonServices::removeTrailingZeros(QString & iosNumber, const QString& sNumberSplitter)
{
	iosNumber = treatSpace(iosNumber);
	QStringList lstFields = iosNumber.split(sNumberSplitter);

	if (lstFields.count() > 1)
	{
		QString sPart1 = lstFields.at(0);
		QString sPart2 = lstFields.at(1);
		int iLenPart2 = sPart2.length();
		//int loopIdx = 0;
		while (iLenPart2)
		{
			QString lastChar = sPart2.right(1);
			if ("0" == lastChar)
			{
				if (1 == iLenPart2)
				{
					iosNumber = sPart1;
					break;
				}
				else {
					sPart2 = sPart2.left(iLenPart2-1);
					iLenPart2 = sPart2.length();
				}
			}
			else {
				iosNumber = sPart1 + sNumberSplitter + sPart2;
				break;
			}

			//loopIdx++;
		}
	}
}

int AINCMNCommonServices::getAlphabateRank(const QString& sAlphabate)
{
	QStringList lstAplpha = QStringList() << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I";
	lstAplpha << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" << "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z";

	int rank = lstAplpha.indexOf(sAlphabate);

	return rank;
}

bool AINCMNCommonServices::getBoolValueFromString(const QString & iValue)
{
	bool bValue(false);
	if (iValue == "TRUE" || iValue == "true")
	{
		bValue = true;
	}
	return bValue;
}
