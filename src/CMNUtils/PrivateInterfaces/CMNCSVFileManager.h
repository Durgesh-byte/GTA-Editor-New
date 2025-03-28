#ifndef AINCMNCSVFILEMANAGER_H_
#define AINCMNCSVFILEMANAGER_H_

#include <QObject>
#include <QList.h>
#include <QStringList>

#include "AINCMNUtils.h"

#include "AINCMNErrorLogger.h"
#include <QHash.h>

namespace AINCMN
{
	namespace CMNUtils
	{
		class AINCMNUtils_SHARED_EXPORT AINCMNCSVFileManager : public QObject
		{
			Q_OBJECT
		public:
			AINCMNCSVFileManager(QString iFileSep=";", QObject *parent = 0);
			~AINCMNCSVFileManager();

			//First Line will always be header/column_name of the CSV file
			bool read(const QString& iAbsCsvFile, QList<QStringList>& ioFileContent);
			bool read(const QString& iAbsCsvFile, const QString& iFileSep,
													QList<QStringList>& ioFileContent);
			bool read(const QString& iAbsCsvFile, 
						QList<QHash<QString, QString>>& ioListOfTableRowsWithHeader);

			bool getTableColumns(const QString& iAbsCsvFile, const QString& iFileSep,
				QStringList& ioColumns);
			bool getValidDataTypesForTable(const QString& iAbsCsvFile, const QString& iFileSep,
				QList<QStringList>& iohshColumnsTypeList);
			//First Line will always be header/column_name of the CSV file
			bool write(const QString& iAbsPathCSVFile, const QList<QStringList>& iFileContent);
			bool write(const QString& iAbsPathCSVFile, const QList<QStringList>& iFileContent,
							const QString& iFileSep);
			bool write(const QString& iAbsPathCSVFile, QList<QHash<QString, QString>>& iHashOfTable);	
			bool write(const QString& iAbsPathCSVFile, QList<QHash<QString, QString>>& iHashOfTable,
							const QString& iFileSep);													
			
			bool changeFileSepOfCSVFile(const QString& iInppFileAbsPath, const QString& iInpFileSep,
				const QString& iOutFileAbsPath, const QString& iOutFileSep);

			void setErrorLogger(AINCMNErrorLogger *& pErrLogger);

		//private: 
			QStringList createListOfCSVLines(const QList<QStringList>& iFileContent);
			QString createCSVLine(const QStringList& iLineContent, const QString& iFileSep);
			QString createCSVLine(const QStringList& iLineContent);
		        
			QList<QStringList> getFileContent(const QStringList& iListOfCSVLines);
			QStringList breakCSVLine(const QString& iCSVLine);
		    
		    QStringList createCSVFileContent(const QList<QStringList>& iListOfTableContent, const QString& iFileSep);
		    
		    QStringList splitString(const QString& iInpStr, const QString& iFileSep);
		    
		private:
			bool write(const QString& iAbsPathCSVFile, const QStringList& iListOfLines);
			
		signals:

		public slots:

		private:
			QString _file_sep;
			AINCMNErrorLogger * _pErrLogger;
		};
	}
}
#endif // AINCMNCSVFileManager_H_
