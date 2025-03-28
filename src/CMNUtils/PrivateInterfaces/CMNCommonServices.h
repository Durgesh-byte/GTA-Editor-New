#ifndef AINCMNCOMMONSERVICES_H_
#define AINCMNCOMMONSERVICES_H_

#include "AINCMNUtils.h"

#include <QObject>
#include <QLabel>

namespace AINCMN
{
        namespace CMNUtils
	{
		class AINCMNUtils_SHARED_EXPORT AINCMNCommonServices : public QObject
		{		   
		public:
			static bool fileExists(const QString& iAbsoluteFilePath, bool bMsgDisplay=true);

			static bool folderExists(const QString& iAbsoluteDirPath, bool bMsgDisplay);
			
			static bool createFolder(const QString& iDirAbsPath);
			
			static bool cleanDir(const QString& iAbsoluteDirPath);
			
			static bool fileExistsInFolder(const QString& iAbsoluteDirPath, const QString& iFileName);
			
			static void display(const QString& iTitle, const QString& iMsg);
			
			static bool copyFile(const QString& iAbsPathOfFile, const QString& iAbsPathOfNewFile);
			
			static bool removeFile(const QString& iAbsPathOfFile);

			static QStringList getFileNames(const QString& iAbsPathOfDir, const QStringList& iFilters);

			static QStringList getAllFileNames(const QString& iAbsPathOfDir, const QStringList& iFilters);
			
			static QString getBaseName(const QString& iAbsPathOfFile);
				
			static QStringList getAbsFiles(const QString& iAbsPathOfDir, const QStringList& iFilters);
			
			//if input is null value out will be "" <empty> string
			static QString treatSpace(const QString& sString, QString sSearchString= " ");
			
			static QStringList removeEmptyString(const QStringList& iList);

			static QStringList getCommonStrings(const QStringList& iList1, const QStringList& iList2);

			static void removeTrailingZeros(QString & iosNumber, const QString& sNumberSplitter);

			static int getAlphabateRank(const QString& sAlphabate);
			
			static bool getBoolValueFromString(const QString & iValue);
			
		};
	}
}
#endif // AINCMNCommonServices_H_
