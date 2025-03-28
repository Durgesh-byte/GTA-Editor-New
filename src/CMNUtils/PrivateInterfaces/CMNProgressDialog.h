/*
* Overload of QProgressDialog class
*/
#ifndef AINCMNPROGRESSDIALOG_H_
#define AINCMNPROGRESSDIALOG_H_

#include <QWidget>
#include <QProgressDialog>
#include <QProgressBar>
#include <QLabel>
#include <QIcon>

#include "AINCMNUtils.h"

namespace AINCMN
{
	namespace Dialogs
	{	
		class AINCMNUtils_SHARED_EXPORT AINCMNProgressDialog : public QProgressDialog
		{
			Q_OBJECT
		public:
			AINCMNProgressDialog(QString title, int minRange, int maxRange, bool allowedToAbort);
			virtual ~AINCMNProgressDialog();
			
			void setProBarLabel(const QString& text);
		};
	}
}

#endif // AINCMNProgressDialog_H_
