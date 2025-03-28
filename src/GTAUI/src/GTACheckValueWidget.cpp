#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QCompleter>
#include "GTAAppController.h"
#include "GTACheckValueWidget.h"
#include "ui_GTACheckValueWidget.h"
#include "GTACheckValue.h"
#include "GTAUtil.h"
#include "GTACheckValue.h"
#include "GTAParamValidator.h"
#include "GTAParamValueValidator.h"
#include "GTAAppController.h"
#include "GTAICDParameter.h"




GTACheckValueWidget::GTACheckValueWidget(QWidget *parent) :
	GTACheckWidgetInterface(parent),
	ui(new Ui::GTACheckValueWidget)
{
	ui->setupUi(this);
	QStringList operatorList;
	operatorList << ARITH_EQ << ARITH_NOTEQ << ARITH_GT << ARITH_LTEQ << ARITH_LT << ARITH_GTEQ;
	//do not change the order , it requires for validation
	_OperList << ARITH_NOTEQ << ARITH_LTEQ << ARITH_GTEQ << ARITH_GT << ARITH_LT << ARITH_EQ;
	ui->Operator1CB->addItems(operatorList);

	_sFSList = QStringList() << ACT_FSSSM_NOT_LOST << ACT_FSSSM_NO << ACT_FSSSM_ND << ACT_FSSSM_FW << ACT_FSSSM_NCD << ACT_FSSSM_FT << ACT_FSSSM_MINUS << ACT_FSSSM_PLUS << ACT_FSSSM_LOST;
	ui->functionalStatusCB->addItems(_sFSList);

	ui->precisionTypeCB->addItems(QStringList() << ACT_PRECISION_UNIT_VALUE << ACT_PRECISION_UNIT_PERCENT);

	ui->precisionTypeCB->setEnabled(true);
	ui->editPrecisionLE->setEnabled(true);

	ui->SearchParityPB->setDisabled(true);
	ui->EditParityLE->setDisabled(true);
	ui->SearchSDIPB->setDisabled(true);
	ui->EditSDILE->setDisabled(true);

	_SearchButtonEditorMap.insert(ui->SearchParam1PB, ui->Param1LE);
	_SearchButtonEditorMap.insert(ui->SearchParam2PB, ui->Param2LE);
	_SearchButtonEditorMap.insert(ui->SearchPrecisionPB, ui->editPrecisionLE);
	_SearchButtonEditorMap.insert(ui->SearchSignPB, ui->SignLE);
	_SearchButtonEditorMap.insert(ui->SearchFSPB, ui->EditFSLE);
	_SearchButtonEditorMap.insert(ui->SearchSDIPB, ui->EditSDILE);
	_SearchButtonEditorMap.insert(ui->SearchParityPB, ui->EditParityLE);

	foreach(QPushButton * pSearchPB, _SearchButtonEditorMap.keys())
	{
		connect(pSearchPB, SIGNAL(clicked()), this, SLOT(onSearchPBClicked()));
	}

	connect(ui->Param1LE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateParamMedia(const QString &)));
	connect(ui->addAndExpPB, SIGNAL(clicked()), this, SLOT(addAndListExpression()));
	connect(ui->addOrExpPB, SIGNAL(clicked()), this, SLOT(addOrListExpression()));
	connect(this, SIGNAL(evaluateExpression()), this, SLOT(valuateExpression()));
	connect(ui->deleteExpPB, SIGNAL(clicked()), this, SLOT(deleteExpression()));
	connect(ui->andExpLW, SIGNAL(itemSelectionChanged()), ui->orExpLW, SLOT(clearSelection()));
	connect(ui->orExpLW, SIGNAL(itemSelectionChanged()), ui->andExpLW, SLOT(clearSelection()));
	connect(ui->orExpLW, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameterLineEdits()));
	connect(ui->andExpLW, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameterLineEdits()));

	connect(ui->andExpLW, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListItemClicked(QListWidgetItem*)));
	connect(ui->orExpLW, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListItemClicked(QListWidgetItem*)));
	connect(ui->editExpPB, SIGNAL(clicked()), this, SLOT(updateParameterValue()));
	connect(ui->importFromCSVPB, SIGNAL(clicked()), this, SLOT(importParametersFromCSV()));
	connect(ui->Operator1CB, SIGNAL(currentIndexChanged(QString)), this, SLOT(togglePrecision(QString)));
	connect(ui->precisionTypeCB, SIGNAL(currentIndexChanged(QString)), this, SLOT(resetPrecision(QString)));

	connect(ui->checkFSCB, SIGNAL(toggled(bool)), this, SLOT(disableValueEdits(bool)));
	connect(ui->checkFSCB, SIGNAL(toggled(bool)), this, SLOT(hideNote(bool)));
	connect(ui->checkFSCB, SIGNAL(toggled(bool)), this, SLOT(resetCheckOnlyValue(bool)));
	connect(ui->checkFSCB, SIGNAL(toggled(bool)), this, SLOT(resetSDI(bool)));
	connect(ui->checkFSCB, SIGNAL(toggled(bool)), this, SLOT(resetParity(bool)));
	connect(ui->checkFSCB, SIGNAL(toggled(bool)), this, SLOT(resetRefreshRate(bool)));

	connect(ui->checkOnlyValueChB, SIGNAL(toggled(bool)), this, SLOT(resetCheckFS(bool)));
	connect(ui->checkOnlyValueChB, SIGNAL(toggled(bool)), this, SLOT(enableValueEdits(bool)));
	connect(ui->checkOnlyValueChB, SIGNAL(toggled(bool)), this, SLOT(hideNote(bool)));
	connect(ui->checkOnlyValueChB, SIGNAL(toggled(bool)), this, SLOT(resetSDI(bool)));
	connect(ui->checkOnlyValueChB, SIGNAL(toggled(bool)), this, SLOT(resetParity(bool)));
	connect(ui->checkOnlyValueChB, SIGNAL(toggled(bool)), this, SLOT(resetRefreshRate(bool)));

	connect(ui->chkOnlySDI, SIGNAL(toggled(bool)), this, SLOT(disableValueEdits(bool)));
	connect(ui->chkOnlySDI, SIGNAL(toggled(bool)), this, SLOT(hideNote(bool)));
	connect(ui->chkOnlySDI, SIGNAL(toggled(bool)), this, SLOT(resetCheckFS(bool)));
	connect(ui->chkOnlySDI, SIGNAL(toggled(bool)), this, SLOT(resetCheckOnlyValue(bool)));
	connect(ui->chkOnlySDI, SIGNAL(toggled(bool)), this, SLOT(resetParity(bool)));
	connect(ui->chkOnlySDI, SIGNAL(toggled(bool)), this, SLOT(resetRefreshRate(bool)));

	connect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(disableValueEdits(bool)));
	connect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(hideNote(bool)));
	connect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(resetCheckFS(bool)));
	connect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(resetCheckOnlyValue(bool)));
	connect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(resetSDI(bool)));
	connect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(resetRefreshRate(bool)));

	connect(ui->chkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(disableValueEditsOnRefresh(bool)));
	connect(ui->chkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(hideNote(bool)));
	connect(ui->chkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(resetCheckFS(bool)));
	connect(ui->chkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(resetCheckOnlyValue(bool)));
	connect(ui->chkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(resetSDI(bool)));
	connect(ui->chkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(resetParity(bool)));
	
	connect(ui->WaitUntilCB, SIGNAL(toggled(bool)), this, SLOT(waitUntilCheckStatusSlot(bool)));

	GTAParamValidator* pValidator1 = new GTAParamValidator(ui->Param1LE);
	ui->Param1LE->setValidator(pValidator1);

	GTAParamValueValidator* pValidator2 = new GTAParamValueValidator(ui->Param2LE);
	ui->Param2LE->setValidator(pValidator2);

	ui->editPrecisionLE->setValidator(new QDoubleValidator(0, 100, 4, this));

	_isParameterLE = false;
	hideNote(false);

	bool getGenerateGenSCXML = TestConfig::getInstance()->getGenericSCXML();
	if (getGenerateGenSCXML)
	{
		ui->chkRefreshRate->setDisabled(true);
		ui->chkOnlySDI->setDisabled(true);
		ui->chkOnlyParity->setDisabled(true);
		ui->CheckSDICB->setDisabled(true);
		ui->CheckParityCB->setDisabled(true);
	}
  

}

void GTACheckValueWidget::hideNote(bool)
{
	bool isBenchParamFnsChecked = (ui->checkFSCB->isChecked() || ui->chkRefreshRate->isChecked() || ui->chkOnlyParity->isChecked() || ui->chkOnlySDI->isChecked());
	bool isValueChecked = ui->checkOnlyValueChB->isChecked();

	ui->checkValnFS->hide();
	ui->checkFsWarLB->hide();

	if (!isBenchParamFnsChecked && !isValueChecked)
	{
		ui->checkValnFS->show();
		ui->checkFsWarLB->hide();
	}
	else if (isValueChecked)
	{
		ui->checkValnFS->hide();
		ui->checkFsWarLB->hide();
	}
	else if (isBenchParamFnsChecked)
	{
		ui->checkValnFS->hide();
		ui->checkFsWarLB->show();
	}
}

void GTACheckValueWidget::disableValueEditsOnRefresh(bool iStatus)
{
	disableValueEdits(iStatus);
	ui->editPrecisionLE->setDisabled(false);
	ui->precisionTypeCB->setDisabled(false);
}

void GTACheckValueWidget::disableValueEdits(bool iStatus)
{
	ui->Param2LE->setDisabled(iStatus);
	ui->SearchParam2PB->setDisabled(iStatus);



	hideNote(false);


	if (ui->checkFSCB->isChecked() || ui->chkOnlyParity->isChecked() || ui->chkOnlySDI->isChecked())
	{
		QStringList operatorList;
		operatorList << ARITH_EQ << ARITH_NOTEQ;
		ui->Operator1CB->clear();
		ui->Operator1CB->addItems(operatorList);
	}
	else
	{
		QStringList operatorList;
		operatorList << ARITH_EQ << ARITH_NOTEQ << ARITH_GT << ARITH_LTEQ << ARITH_LT << ARITH_GTEQ;
		ui->Operator1CB->clear();
		ui->Operator1CB->addItems(operatorList);
	}
	ui->Operator1CB->setCurrentIndex(0);
	if (iStatus == true)
	{
		hideNote(false);
		
		ui->editPrecisionLE->setDisabled(true);
		ui->precisionTypeCB->setDisabled(true);
	}
	else
	{
		ui->editPrecisionLE->setDisabled(false);
		ui->precisionTypeCB->setDisabled(false);
	}
}

void GTACheckValueWidget::enableValueEdits(bool)
{
	if (ui->checkOnlyValueChB->isChecked())
		disableValueEdits(false);
}

void GTACheckValueWidget::valuateExpression()
{
	int andExpListCount = ui->andExpLW->count();
	int orExpListCount = ui->orExpLW->count();

	ui->resultExpTE->clear();

	QString sFinalExpression;
	for (int i = 0; i<andExpListCount; i++)
	{
		QListWidgetItem* pItem = ui->andExpLW->item(i);
		QString sExp = pItem->text();
		sFinalExpression.append(sExp);

		if (andExpListCount>1 && i<andExpListCount - 1)
			sFinalExpression.append(" AND ");
	}


	if (orExpListCount)
	{

		sFinalExpression.prepend("{");
		sFinalExpression.append("} OR {");

		for (int i = 0; i<orExpListCount; i++)
		{
			QListWidgetItem* pItem = ui->orExpLW->item(i);
			QString sExp = pItem->text();
			sFinalExpression.append(sExp);
			if (orExpListCount>1 && i<orExpListCount - 1)
				sFinalExpression.append(" AND ");
		}
		sFinalExpression.append("}");
	}

	convertToHtml(sFinalExpression);
	ui->resultExpTE->setHtml(sFinalExpression);
}

GTACheckValueWidget::~GTACheckValueWidget()
{
	delete ui;
}

void GTACheckValueWidget::onSearchPBClicked()
{
	QPushButton * pSearchPBClicked = dynamic_cast<QPushButton*>(sender());
	
	if (sender() == ui->SearchParam1PB)
		_isParameterLE = true;
	else
		_isParameterLE = false;
	if (sender() == ui->SearchParam1PB)
		ui->editPrecisionLE->setValidator(0);
    
	if (pSearchPBClicked != NULL && _SearchButtonEditorMap.contains(pSearchPBClicked))
	{
		QLineEdit *pEdit = _SearchButtonEditorMap.value(pSearchPBClicked);
		pEdit->setEnabled(true);
		emit searchParameter(pEdit);
	}
}

void GTACheckValueWidget::setCheckCommand(GTACheckValue * ipCheckCmd)
{
	clear();
	if (ipCheckCmd != NULL)
	{
		bool insertInAnd = true;
		bool waitUntil = ipCheckCmd->getWaitUntil();
		ui->WaitUntilCB->setChecked(waitUntil);

		QStringList LeftExprList, RightExprList;
		int noOfItems = ipCheckCmd->getCount();
		for (int i = 0; i<noOfItems; i++)
		{
			QString sParam = ipCheckCmd->getParameter(i);
			QString sFStatus = ipCheckCmd->getFunctionalStatus(i);
			QString sValue = ipCheckCmd->getValue(i);
			QString sCondition = ipCheckCmd->getCondition(i);
			double sPrecisionVal = ipCheckCmd->getPrecisionValue(i);
			QString sPrecisionFE = ipCheckCmd->getPrecisionValueFE(i);
			QString sPrecisionType = ipCheckCmd->getPrecisionType(i);
			bool isCheckFS = ipCheckCmd->getISCheckFS(i);
			bool isCheckOnlyValue = ipCheckCmd->getISOnlyCheckValue(i);
			bool isCheckSDI = ipCheckCmd->getIsSDIOnly(i);
			bool isCheckParity = ipCheckCmd->getIsParityOnly(i);
			bool isCheckRefresh = ipCheckCmd->getIsRefreshRateOnly(i);
			QString sSDI = ipCheckCmd->getSDI(i);
			QString sParity = ipCheckCmd->getParity(i);

			QString expr;
			sPrecisionType = sPrecisionType == ACT_PRECISION_UNIT_PERCENT ? QString(ACT_PRECISION_UNIT_PERCENT) : QString();
			if (isCheckFS)
			{
				expr = QString("%1(FS:%2)%3").arg(sParam, sFStatus, CHK_FS_STR);
			}
			else if (isCheckSDI)
			{
				expr = QString("%1(SDI:%2)%3").arg(sParam, sSDI, CHK_SDI_STR);
			}
			else if (isCheckRefresh)
			{
				expr = QString("%1%2").arg(sParam, CHK_REFRESH_RATE_STR);
			}
			else if (isCheckParity)
			{
				expr = QString("%1(Parity:%2)%3").arg(sParam, sParity, CHK_PARITY_STR);
			}
			else
			{
				if (!_OperList.contains(sCondition))
				{
					_OperList << sCondition;
				}

				if (sPrecisionFE == "") 
				{
					expr = QString("%1(FS:%2) %3 %4 (+/- %5%6)").arg(sParam, sFStatus, sCondition, sValue, QString::number(sPrecisionVal, 'f', 4), sPrecisionType);
					if (isCheckOnlyValue)
						expr.append(CHK_VALUE_ONLY_STR);
				}
				else
				{
					expr = QString("%1(FS:%2) %3 %4 (+/- %5%6)").arg(sParam, sFStatus, sCondition, sValue, sPrecisionFE, sPrecisionType);
					if (isCheckOnlyValue)
						expr.append(CHK_VALUE_ONLY_STR);
				}
			}
			if (insertInAnd)
			{
				LeftExprList.append(expr);
			}
			else
			{
				RightExprList.append(expr);
			}

			if (i<noOfItems - 1)
			{
				QString sOperator = ipCheckCmd->getBinaryOperator(i);
				if (sOperator.compare(QString("OR"), Qt::CaseInsensitive) == 0)
				{
					insertInAnd = false;
				}
			}
		}
		ui->andExpLW->addItems(LeftExprList);
		ui->orExpLW->addItems(RightExprList);

		silentReSetChkOnlyValueCB();
		selectLastEditedRow();

		emit evaluateExpression();
	}
}

GTACheckValue * GTACheckValueWidget::getCheckCommand()
{
	int andExpListCount = ui->andExpLW->count();
	int orExpListCount = ui->orExpLW->count();
	QString andExpression = "AND";
	if (orExpListCount == 0 && andExpListCount == 0)
	{
		return NULL;
	}
	GTACheckValue * pCheck = new GTACheckValue();

	for (int i = 0; i<andExpListCount; i++)
	{
		QListWidgetItem* pItem = (ui->andExpLW->item(i));
		if (NULL != pItem)
		{
			QString currentExpression = pItem->text();
			QString sParameter, sConditionalExp, sValue, sFuncStatus, sPrecisionType;
			double dPrecisionVal;
			bool isCheckValueOnly, isFSCheck, isSDI, isParity, isRefreshRate;
			QString sSDI,sParity,precisionValFE;
			extractCheckItemsFromCheckExpression(currentExpression,
				sParameter,
				sConditionalExp,
				sValue,
				sFuncStatus,
				dPrecisionVal,
				precisionValFE,
				sPrecisionType,
				isCheckValueOnly,
				isFSCheck, isSDI, isParity,
				isRefreshRate,
				sSDI, sParity);

			pCheck->insertParamenter(sParameter,
				sConditionalExp,
				sValue,
				sFuncStatus,
				dPrecisionVal,
				precisionValFE,
				sPrecisionType,
				isCheckValueOnly,
				isFSCheck,
				isSDI,
				isParity, isRefreshRate,
				sSDI,
				sParity);

		}
		if (andExpListCount>1 && i<andExpListCount - 1)
		{
			pCheck->insertBinaryOperator(andExpression);
		}
	}

	QString orExpression = "OR";
	if (orExpListCount)
		pCheck->insertBinaryOperator(orExpression);

	for (int i = 0; i<orExpListCount; i++)
	{
		QListWidgetItem* pItem = ui->orExpLW->item(i);
		if (NULL != pItem)
		{
			QString currentExpression = pItem->text();
			QString sParameter, sConditionalExp, sValue, sFuncStatus, sPrecisionType;
			double dPrecisionVal;
			bool isCheckValueOnly;
			bool isFSCheck;
			bool isSDI, isParity, isRefreshRate;
			QString sSDI, sParity, precisionValFE;
			extractCheckItemsFromCheckExpression(currentExpression,
				sParameter,
				sConditionalExp,
				sValue,
				sFuncStatus,
				dPrecisionVal,
				precisionValFE,
				sPrecisionType,
				isCheckValueOnly,
				isFSCheck, isSDI, isParity,
				isRefreshRate,
				sSDI, sParity);

			pCheck->insertParamenter(sParameter,
				sConditionalExp,
				sValue,
				sFuncStatus,
				dPrecisionVal,
				precisionValFE,
				sPrecisionType,
				isCheckValueOnly,
				isFSCheck, isSDI, isParity,
				isRefreshRate, sSDI, sParity);
		}
		if (orExpListCount>1 && i<orExpListCount - 1)
		{
			pCheck->insertBinaryOperator(andExpression);
		}
	}
	pCheck->setWaitUntil(ui->WaitUntilCB->isChecked());
	pCheck->setLastEditedRow(_lastEditedRow);
	pCheck->setLastEditedCol(_lastEditedCol);
	//pCheck->setConfCheckTime(ui->confirmationCheckLE->text().toDouble());
	return pCheck;
}

QString GTACheckValueWidget::getFunctionalStatus() {
	QString sFs;
	if (!ui->EditFSLE->text().isEmpty()) {
		sFs = ui->EditFSLE->text();
	}
	else {
		sFs = ui->functionalStatusCB->currentText();
	}
	return sFs;
}

QString GTACheckValueWidget::getOperator() {
	QString op;
	if (!ui->SignLE->text().isEmpty()) {
		op = ui->SignLE->text();
	}
	else {
		op = ui->Operator1CB->currentText();
		
	}
	return op;
}

void GTACheckValueWidget::addAndListExpression()
{
	QString errorMsg;
	if (!isValidParameter(errorMsg))
	{
		if(this->isVisible())
			QMessageBox::critical(this, "Field value error", errorMsg);
		return;
	}
	
	QString sParameter = ui->Param1LE->text();
	QString sValue = ui->Param2LE->text();
	QList<QPair<QString, QString>> lstOfParamValuePair;
	bool rc = getParamValuePairForChannel(sParameter, sValue, lstOfParamValuePair);
	if (!rc)
	{
		if (this->isVisible())
			QMessageBox::critical(this, "Error", "combination of channels is invalid");
		return;
	}

	for (int i = 0; i<lstOfParamValuePair.count(); i++)
	{
		QPair <QString, QString> paramValPair = lstOfParamValuePair.at(i);
		QString parameterName = paramValPair.first;
		QString parameterVal = paramValPair.second;
		QString sFs = getFunctionalStatus();
		QString pPrecisionLEValue = ui->editPrecisionLE->text();
		double sPrecisonVal = ui->editPrecisionLE->text().toDouble();
		QString sPrecisonTyp = ui->precisionTypeCB->currentText() == ACT_PRECISION_UNIT_PERCENT ? QString(ACT_PRECISION_UNIT_PERCENT) : QString();
		QString sSDI = getSDI();
		QString sParity = getParity();
		int currentOperatorIndex = ui->Operator1CB->currentIndex();
		QString sCondition;
		QString sOperator = ui->SignLE->text();

		if (sOperator.contains("line."))
		{
			sPrecisonVal = 0.0;
		}

		if (ui->checkFSCB->isChecked() == false &&
			ui->chkOnlySDI->isChecked() == false &&
			ui->chkOnlyParity->isChecked() == false &&
			ui->chkRefreshRate->isChecked() == false)
		{
			sCondition = getOperator();
			sCondition.remove(" ");
			_OperList << sCondition;
			sValue.remove(" ");


		}
		else
		{
			if (!ui->chkRefreshRate->isChecked())
			{
				sValue.clear();
				sPrecisonVal = 0;
				sPrecisonTyp.clear();
			}
		}
		QString sPrecisonValString = QString::number(sPrecisonVal, 'f', 4);
		if (pPrecisionLEValue.contains("line.") && !sOperator.contains("line."))
		{
			sPrecisonValString = pPrecisionLEValue;
		}
		QString lhs = parameterName.trimmed();
		if (!(lhs.startsWith("\"") && lhs.endsWith("\"")))
			lhs = lhs.remove(" ").trimmed();
		QListWidgetItem* pItem = new QListWidgetItem(ui->andExpLW);
		if (ui->checkFSCB->isChecked())
			pItem->setText(QString("%1 %4(FS:%2) %3").arg(lhs, sFs, CHK_FS_STR, sCondition));
		else if (ui->chkOnlySDI->isChecked())
		{
			pItem->setText(QString("%1(SDI:%2) %3").arg(lhs, sSDI, CHK_SDI_STR));
		}
		else if (ui->chkOnlyParity->isChecked())
		{
			pItem->setText(QString("%1(Parity:%2) %3").arg(lhs, sParity, CHK_PARITY_STR));
		}
		else if (ui->chkRefreshRate->isChecked())
		{
			pItem->setText(QString("%1 (Refresh Rate:%2)(+/- %4%5)%3").arg(lhs, "250", CHK_REFRESH_RATE_STR, sPrecisonValString, sPrecisonTyp));
		}
		else if (ui->checkOnlyValueChB->isChecked())
			pItem->setText(QString("%1(FS:%2) %3 %4 (+/- %5%6)%7").arg(lhs, sFs, sCondition, parameterVal, sPrecisonValString, sPrecisonTyp, CHK_VALUE_ONLY_STR));
		else
			pItem->setText(QString("%1(FS:%2) %3 %4 (+/- %5%6)").arg(lhs, sFs, sCondition, parameterVal, sPrecisonValString, sPrecisonTyp));

		ui->andExpLW->addItem(pItem);
		clearParameterListSelection();
	}

	_lastEditedCol = 0;
	_lastEditedRow = ui->andExpLW->count() - 1;
	selectLastEditedRow();

	emit evaluateExpression();
}
void GTACheckValueWidget::addOrListExpression()
{
	QString errorMsg;
	if (!isValidParameter(errorMsg))
	{
		if (this->isVisible())
			QMessageBox::critical(this, "Field value error", errorMsg);
		return;
	}

	if (!ui->andExpLW->count())
	{
		if (this->isVisible())
			QMessageBox::critical(this, "And list empty", "Add to AND list before using OR list");
		return;
	}
	
	QString sParameter = ui->Param1LE->text();
	QString sValue = ui->Param2LE->text();
	QList<QPair<QString, QString>> lstOfParamValuePair;
	bool rc = getParamValuePairForChannel(sParameter, sValue, lstOfParamValuePair);
	if (!rc)
	{
		if (this->isVisible())
			QMessageBox::critical(this, "Error", "combination of channels is invalid");
		return;
	}

	for (int i = 0; i<lstOfParamValuePair.size(); i++)
	{
		QPair <QString, QString> paramValPair = lstOfParamValuePair.at(i);
		QString parameterName = paramValPair.first;
		QString parameterVal = paramValPair.second;
		QString sFs = getFunctionalStatus();
		QString sPrecisonVal = ui->editPrecisionLE->text();
		QString sPrecisonTyp = ui->precisionTypeCB->currentText() == ACT_PRECISION_UNIT_PERCENT ? QString(ACT_PRECISION_UNIT_PERCENT) : QString();
		QString sSDI = ui->CheckSDICB->currentText();
		QString sParity = ui->CheckParityCB->currentText();
		QString sOperator = ui->SignLE->text();

		if (sOperator.contains("line."))
		{
			sPrecisonVal = QString::number(0.0, 'f', 4);
		}

		if (!sPrecisonVal.contains("line."))
		{
			sPrecisonVal = QString::number(sPrecisonVal.toDouble(), 'f', 4);
		}

		int currentOperatorIndex = ui->Operator1CB->currentIndex();
		QString sCondition;
		if (ui->checkFSCB->isChecked() == false &&
			ui->chkOnlySDI->isChecked() == false &&
			ui->chkOnlyParity->isChecked() == false &&
			ui->chkRefreshRate->isChecked() == false)
		{
			sCondition = getOperator();
			sCondition.remove(" ");
			if (!_OperList.contains(sCondition))
			{
				_OperList << sCondition;
			}
			sValue.remove(" ");
		}
		else
		{
			if (!ui->chkRefreshRate->isChecked())
			{
				sValue.clear();
				sPrecisonVal.clear();
				sPrecisonTyp.clear();
			}
		}

		QListWidgetItem* pItem = new QListWidgetItem(ui->orExpLW);
		QString lhs = parameterName.trimmed();
		if (!(lhs.startsWith("\"") && lhs.endsWith("\"")))
			lhs = lhs.remove(" ").trimmed();

		if (ui->checkFSCB->isChecked())
			pItem->setText(QString("%1 %4(FS:%2) %3").arg(lhs, sFs, CHK_FS_STR, sCondition));
		else if (ui->chkOnlySDI->isChecked())
		{
			pItem->setText(QString("%1(SDI:%2) %3").arg(lhs, sSDI, CHK_SDI_STR));
		}
		else if (ui->chkOnlyParity->isChecked())
		{
			pItem->setText(QString("%1(Parity:%2) %3").arg(lhs, sParity, CHK_PARITY_STR));
		}
		else if (ui->chkRefreshRate->isChecked())
		{
			//            pItem->setText(QString("%2 %1").arg(lhs,CHK_REFRESH_RATE_STR));
			pItem->setText(QString("%1 (Refresh Rate:%2)(+/- %4%5)%3").arg(lhs, "250", CHK_REFRESH_RATE_STR));
		}
		else if (ui->checkOnlyValueChB->isChecked())
			pItem->setText(QString("%1(FS:%2) %3 %4 (+/- %5%6)%7").arg(lhs, sFs, sCondition, parameterVal, sPrecisonVal, sPrecisonTyp, CHK_VALUE_ONLY_STR));
		else
			pItem->setText(QString("%1(FS:%2) %3 %4 (+/- %5%6)").arg(lhs, sFs, sCondition, parameterVal, sPrecisonVal, sPrecisonTyp));

		ui->orExpLW->addItem(pItem);
		clearParameterListSelection();
	}

	_lastEditedCol = 1;
	_lastEditedRow = ui->orExpLW->count() - 1;
	selectLastEditedRow();

	emit evaluateExpression();
}

bool GTACheckValueWidget::isValidParameter(QString& ioErrorMsg)
{
	QString sParameter = ui->Param1LE->text();
	QStringList splitWithSpaces = sParameter.split(" ", QString::SkipEmptyParts);

	if (!(sParameter.startsWith("\"") && sParameter.endsWith("\"")))
	{
		if (sParameter.isEmpty() || splitWithSpaces.size() == 0)
		{
			ioErrorMsg = QString("Parameter name cannot be empty");
			return false;
		}

		if (splitWithSpaces.size()>1)
		{
			ioErrorMsg = QString("Parameter name cannot contain empty spaces");
			return false;
		}

		foreach(QString conditionalExp, _OperList)
		{
			if (sParameter.contains(conditionalExp))
			{
				ioErrorMsg = QString("Invalid Expression character [%1] found in parameter name").arg(conditionalExp);
				return false;
			}
		}
	}
	QString sValue = ui->Param2LE->text();
	splitWithSpaces.clear();
	splitWithSpaces = sValue.split(" ", QString::SkipEmptyParts);

	if (!(sValue.startsWith("\"") && sValue.endsWith("\"")))
	{
		if (sValue.isEmpty() || splitWithSpaces.size() == 0)
		{
			if (ui->checkFSCB->isChecked() == false &&
				ui->chkOnlySDI->isChecked() == false &&
				ui->chkOnlyParity->isChecked() == false &&
				ui->chkRefreshRate->isChecked() == false)
			{
				ioErrorMsg = QString("Parameter/value cannot be Empty");
				return false;
			}
		}

		if (splitWithSpaces.size()>1)
		{
			ioErrorMsg = QString("Parameter/Value cannot contain empty spaces");
			return false;
		}
		foreach(QString conditionalExp, _OperList)
		{
			if (sValue.contains(conditionalExp))
			{
				ioErrorMsg = QString("Invalid Expression charachter [%1] found in Parameter/value").arg(conditionalExp);
				return false;
			}
		}
	}
	QString sPrecisionVal = ui->editPrecisionLE->text();

	if (sPrecisionVal.isEmpty() && (!ui->chkOnlySDI->isChecked() &&
		!ui->checkFSCB->isChecked() &&
		!ui->chkOnlyParity->isChecked()))
	{
		ioErrorMsg = QString("Precision cannot be empty");
		return false;
	}
	return true;
}

void GTACheckValueWidget::deleteExpression()
{
	QList<QListWidgetItem*> lstSelectedAndExp = ui->andExpLW->selectedItems();
	QList<QListWidgetItem*> lstSelectedorExp = ui->orExpLW->selectedItems();

	if (!lstSelectedorExp.isEmpty())
	{
		QListWidgetItem* pItem = lstSelectedorExp[0];
		//QString test = pItem->text();
		ui->orExpLW->removeItemWidget(pItem);
		delete pItem;
		pItem = NULL;

		_lastEditedCol = ui->orExpLW->count() == 0 ? 0 : 1;
		QListWidget* editedListWidget = _lastEditedCol == 0 ? ui->andExpLW : ui->orExpLW;
		_lastEditedRow = editedListWidget->count() - 1;
		selectLastEditedRow();

		clearParameterListSelection();
	}
	else if (!lstSelectedAndExp.isEmpty())
	{
		if (ui->andExpLW->count() == 1 && ui->orExpLW->count()>0)
		{
			if (this->isVisible())
				QMessageBox::critical(this, "And list empty", "And list cannot be empty while Or list is not empty");
		}
		else
		{
			QListWidgetItem* pItem = lstSelectedAndExp[0];
			ui->andExpLW->removeItemWidget(pItem);
			delete pItem;
			pItem = NULL;
			
			_lastEditedCol = 0;
			_lastEditedRow = ui->andExpLW->count() - 1;
			selectLastEditedRow();

		}
		clearParameterListSelection();
	}
	else
	{
		if (this->isVisible())
			QMessageBox::critical(this, "Delete", "Select Item for deletion");
		return;
	}
	emit evaluateExpression();
}

bool GTACheckValueWidget::isValidExpression(const QString& sExpression)
{
	bool isValid = true;
	QString currentItemText = sExpression;

	QStringList splitExp;
	bool containsconditionalExp = false;
	foreach(QString conditionalExp, _OperList)
	{
		if (currentItemText.contains(conditionalExp))
		{
			containsconditionalExp = true;

			splitExp = currentItemText.split(conditionalExp, QString::SkipEmptyParts);
			if (splitExp.size() != 2)
				return false;
			else //check for spaces no spaces allowed in parameters
			{
				QString sParameter = splitExp.at(0);
				QStringList splitExpParam = sParameter.split(" ", QString::SkipEmptyParts);
				if (splitExpParam.size() != 1)
					return false;

				QString sValue = splitExp.at(1);
				QStringList splitExpValue = sValue.split(" ", QString::SkipEmptyParts);
				if (splitExpValue.size() != 1)
					return false;
			}
			break;
		}
	}
	if (containsconditionalExp == false)
		return false;
	//after spiting for first time no expression should have another condition expression
	if (splitExp.size() == 2)
		foreach(QString conditionalExp, _OperList)
	{
		if (splitExp[0].contains(conditionalExp))
			return false;

		if (splitExp[1].contains(conditionalExp))
			return false;
	}
	return isValid;
}

bool GTACheckValueWidget::convertToHtml(QString& ipString)
{
	QString headerString = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
	QString andHTML = "<span style=\" font-weight:600; \"> AND </span>";
	QString orHTML = "<span style=\" font-weight:600;\"> OR </span>";
	ipString.replace(QString("<"), QString("&lt;"));
	ipString.replace(QString(">"), QString("&gt;"));
	ipString.replace(QString(" AND "), andHTML);
	ipString.replace(QString(" OR "), orHTML);
	ipString.prepend(headerString);
	return false;
}
void GTACheckValueWidget::clearParameterListSelection()
{
	ui->orExpLW->clearSelection();
	ui->andExpLW->clearSelection();
}

void GTACheckValueWidget::onListItemClicked(QListWidgetItem *)
{
	updateParameterLineEdits();
}

void GTACheckValueWidget::updateParameterLineEdits()
{
	QListWidget* pLWidget = dynamic_cast <QListWidget*> (sender());
	
	if (pLWidget)
	{
		QList<QListWidgetItem*> lstSelectedItems = pLWidget->selectedItems();
		if (!lstSelectedItems.isEmpty())
		{
			QListWidgetItem* pItem = lstSelectedItems.at(0);
			QString currentExpression = pItem->text();

			QStringList splitExp;
			QString sParameter;
			QString sFunctionalStatus;
			QString sOperator;

			bool isCheckValueOnly = false;
			bool isCheckStatus = false;
			bool isSDIOnly = false;
			bool isParityOnly = false;
			bool isRefreshRateOnly = false;

			if (!(currentExpression.trimmed().startsWith("\"") && currentExpression.trimmed().endsWith("\"")))
			{
				if (currentExpression.contains(CHK_FS_STR))
				{
					splitExp = currentExpression.split(CHK_VALUE_ONLY_STR, QString::SkipEmptyParts);
					isCheckValueOnly = false;
					isCheckStatus = true;
					isSDIOnly = false;
					isParityOnly = false;
					isRefreshRateOnly = false;
					ui->Param2LE->clear();
				}
				else if (currentExpression.contains(CHK_SDI_STR))
				{
					splitExp = currentExpression.split(CHK_SDI_STR, QString::SkipEmptyParts);
					isCheckValueOnly = false;
					isCheckStatus = false;
					isSDIOnly = true;
					isParityOnly = false;
					isRefreshRateOnly = false;
					ui->Param2LE->clear();
				}
				else if (currentExpression.contains(CHK_PARITY_STR))
				{
					splitExp = currentExpression.split(CHK_PARITY_STR, QString::SkipEmptyParts);
					isCheckValueOnly = false;
					isCheckStatus = false;
					isSDIOnly = false;
					isParityOnly = true;
					isRefreshRateOnly = false;
					ui->Param2LE->clear();
				}
				else if (currentExpression.contains(CHK_REFRESH_RATE_STR))
				{
					splitExp = currentExpression.split(CHK_REFRESH_RATE_STR, QString::SkipEmptyParts);
					isCheckValueOnly = false;
					isCheckStatus = false;
					isSDIOnly = false;
					isParityOnly = false;
					isRefreshRateOnly = true;
					ui->Param2LE->clear();
				}
				else
				{
					if (currentExpression.contains(CHK_VALUE_ONLY_STR))
					{
						isCheckValueOnly = true;
						isCheckStatus = false;
						isSDIOnly = false;
						isParityOnly = false;
						isRefreshRateOnly = false;
						silentSetChkValueOnlyCB();
						currentExpression.remove(CHK_VALUE_ONLY_STR);
					}
					else
						silentReSetChkOnlyValueCB();

					ui->checkFSCB->setChecked(false);
					ui->chkOnlySDI->setChecked(false);
					ui->chkOnlyParity->setChecked(false);
					ui->chkRefreshRate->setChecked(false);

					for (int i = 0; i<_OperList.size(); i++)
					{
						QString conditionalExp = _OperList.at(i);
						if (currentExpression.contains(conditionalExp))
						{
							splitExp = currentExpression.split(conditionalExp, QString::SkipEmptyParts);
							sOperator = conditionalExp;
							break;
						}
					}
				}
			}
			for (int i = 0; i<_OperList.size(); i++)
			{
				QString conditionalExp = _OperList.at(i);
				if (currentExpression.contains(conditionalExp))
				{
					splitExp = currentExpression.split(conditionalExp, QString::SkipEmptyParts);
					sOperator = conditionalExp;
					break;
				}
			}
			if (!sOperator.isEmpty())
			{
				int opIndex = ui->Operator1CB->findText(sOperator);
				if (opIndex >= 0)
				{
					ui->Operator1CB->setCurrentIndex(opIndex);
					togglePrecision(sOperator.trimmed());
					ui->SignLE->setText("");
				}
				else
				{
					ui->SignLE->setText(sOperator);
					

				}

			}

			if (splitExp.isEmpty())
            {
				if (currentExpression.contains("line."))
				{
					int i=0;
					splitExp = currentExpression.split("line.", QString::SkipEmptyParts);
					while (i < splitExp.size())
					{
						if(splitExp[i].contains("param_"))
						    splitExp[i].prepend("line.");
						i++;
					}
					sOperator = splitExp[1];
					splitExp.removeAt(1);
					ui->SignLE->setText(sOperator);
				}
				else
				{
					return;
				}
				
			}
			
			sParameter = splitExp.at(0);

			if (isSDIOnly)
			{
				QStringList parameterInfoLst = sParameter.split("(SDI:");
				QString sSDI = QString("00");

				if (parameterInfoLst.size() == 2)
				{
					sParameter = parameterInfoLst.at(0).trimmed();
					if (!(sParameter.trimmed().startsWith("\"") && sParameter.trimmed().endsWith("\"")))
						sParameter.remove(QRegExp("\\s+"));
					sSDI = parameterInfoLst.at(1).trimmed();
					sSDI.remove(QRegExp("\\s+"));
					sSDI.remove(")");
					sSDI.remove(QString(CHK_SDI_STR).remove(QRegExp("\\s+")));
					sSDI.remove(QString(CHK_VALUE_ONLY_STR).remove(QRegExp("\\s+")));
				}
				silentUpdateParam1LE(sParameter);
				int index = ui->CheckSDICB->findText(sSDI);
				if (index >= 0 && index < ui->CheckSDICB->count())
					ui->CheckSDICB->setCurrentIndex(index);
			}
			else if (isParityOnly)
			{
				QStringList parameterInfoLst = sParameter.split("(Parity:");
				QString sParity = QString("0");

				if (parameterInfoLst.size() == 2)
				{
					sParameter = parameterInfoLst.at(0).trimmed();
					if (!(sParameter.trimmed().startsWith("\"") && sParameter.trimmed().endsWith("\"")))
						sParameter.remove(QRegExp("\\s+"));
					sParity = parameterInfoLst.at(1).trimmed();
					sParity.remove(QRegExp("\\s+"));
					sParity.remove(")");
					sParity.remove(QString(CHK_PARITY_STR).remove(QRegExp("\\s+")));
					sParity.remove(QString(CHK_VALUE_ONLY_STR).remove(QRegExp("\\s+")));
				}
				silentUpdateParam1LE(sParameter);
				int index = ui->CheckParityCB->findText(sParity);
				if (index >= 0 && index < ui->CheckParityCB->count())
					ui->CheckParityCB->setCurrentIndex(index);

			}
			else if (isRefreshRateOnly)
			{
				QStringList parameterInfoLst;
				if (sParameter.contains("Refresh Rate:"))
					parameterInfoLst = sParameter.split("Refresh Rate:");
				else if (sParameter.contains("RefreshRate:"))
					parameterInfoLst = sParameter.split("RefreshRate:");

				QString sRefreshRate = QString("250");
				QString sPrecision = QString("0");
				QString sPrecisionType = QString("");

				if (parameterInfoLst.size() == 2)
				{
					sParameter = parameterInfoLst.at(0).trimmed();
					if (!(sParameter.trimmed().startsWith("\"") && sParameter.trimmed().endsWith("\"")))
						sParameter.remove(QRegExp("\\s+"));

					if (sParameter.contains("("))
						sParameter.remove("(");

					sParameter.trimmed();
					QString refreshVal = parameterInfoLst.at(1).trimmed();
					QStringList values = refreshVal.split(")", QString::SkipEmptyParts);
					if (values.size() == 2)
					{
						// refresh rate at index 0
						QString tmp = values.at(0);
						tmp.trimmed();
						if (!tmp.isEmpty())
						{
							sRefreshRate = tmp;
						}

						// precision at index 1
						tmp = values.at(1);
						QRegExp exp(QString("[\\(\\+/-]+"));
						tmp.remove(exp);
						if (!tmp.isEmpty())
						{
							tmp.trimmed();
							if (tmp.endsWith(ACT_PRECISION_UNIT_PERCENT))
							{
								sPrecisionType = ACT_PRECISION_UNIT_PERCENT;
							}
							tmp.remove(ACT_PRECISION_UNIT_PERCENT);
							sPrecision = tmp.trimmed();
						}
					}
				}
				silentUpdateParam1LE(sParameter);
				ui->chkRefreshRate->setChecked(true);
				ui->editPrecisionLE->setEnabled(true);
				ui->editPrecisionLE->setText(sPrecision);

				if (!sPrecisionType.isEmpty())
				{
					int index = ui->precisionTypeCB->findText(ACT_PRECISION_UNIT_PERCENT);
					ui->precisionTypeCB->setCurrentIndex(index);
				}
				else
					ui->precisionTypeCB->setCurrentIndex(0);
			}
			else
			{
				QStringList parameterInfoLst = sParameter.split("(FS:");
				sFunctionalStatus = ACT_FSSSM_NO;

				if (parameterInfoLst.size() == 2)
				{
					sParameter = parameterInfoLst.at(0).trimmed();
					if (!(sParameter.trimmed().startsWith("\"") && sParameter.trimmed().endsWith("\"")))
						sParameter.remove(QRegExp("\\s+"));
					sFunctionalStatus = parameterInfoLst.at(1).trimmed();
					sFunctionalStatus.remove(QRegExp("\\s+"));
					sFunctionalStatus.remove(")");
					sFunctionalStatus.remove(QString(CHK_FS_STR).remove(QRegExp("\\s+")));
					sFunctionalStatus.remove(QString(CHK_VALUE_ONLY_STR).remove(QRegExp("\\s+")));
				}

				//set decoded items in widget
				silentUpdateParam1LE(sParameter);
				int indexForFS = ui->functionalStatusCB->findText(sFunctionalStatus);
				if (indexForFS >= 0)
				{
					ui->functionalStatusCB->setCurrentIndex(indexForFS);
				}
				else
				{
					ui->EditFSLE->setDisabled(ui->checkOnlyValueChB->isChecked() 
						|| !ui->checkFSCB->isEnabled()
						|| !ui->checkFSCB->isChecked()
					);
					ui->EditFSLE->setText(sFunctionalStatus);
				}

			}

			if (isCheckStatus)
			{
				ui->checkFSCB->setChecked(true);
				ui->editPrecisionLE->setDisabled(true);
				ui->Param2LE->setDisabled(true);
				ui->SearchParam2PB->setDisabled(true);
			}
			else if (isSDIOnly)
			{
				ui->chkOnlySDI->setChecked(true);
				ui->editPrecisionLE->setDisabled(true);
				ui->Param2LE->setDisabled(true);
				ui->SearchParam2PB->setDisabled(true);
			}
			else if (isParityOnly)
			{
				ui->chkOnlyParity->setChecked(true);
				ui->editPrecisionLE->setDisabled(true);
				ui->Param2LE->setDisabled(true);
				ui->SearchParam2PB->setDisabled(true);
			}
			else if (isRefreshRateOnly)
			{
				ui->chkRefreshRate->setChecked(true);
				ui->editPrecisionLE->setDisabled(true);
				ui->Param2LE->setDisabled(true);
				ui->SearchParam2PB->setDisabled(true);
			}
			else
			{
				QString sParameterValueToUpdate = splitExp.at(1).trimmed();

				QStringList sParamValInfoLst = sParameterValueToUpdate.split("(", QString::SkipEmptyParts);
				if (sParamValInfoLst.size() == 2)
				{
					QString parameter = sParamValInfoLst.at(0).trimmed();
					if (!(parameter.startsWith("\"") && parameter.endsWith("\"")))
						parameter.remove(QRegExp("\\s+"));
					ui->Param2LE->setText(parameter);
					QString sPrecisionInfo = sParamValInfoLst.at(1).trimmed();
					sPrecisionInfo.remove(QRegExp("\\s+"));
					sPrecisionInfo.remove("+/-");
					sPrecisionInfo.remove(")");
					sPrecisionInfo.remove(QString(CHK_FS_STR).remove(" "));
					if (sPrecisionInfo.contains(ACT_PRECISION_UNIT_PERCENT))
					{
						sPrecisionInfo.remove(ACT_PRECISION_UNIT_PERCENT);
						ui->editPrecisionLE->setText(sPrecisionInfo.trimmed());
						int index = ui->precisionTypeCB->findText(ACT_PRECISION_UNIT_PERCENT);
						ui->precisionTypeCB->setCurrentIndex(index);
					}
					else
					{
						ui->editPrecisionLE->setText(sPrecisionInfo);
						int index = ui->precisionTypeCB->findText(ACT_PRECISION_UNIT_VALUE);
						ui->precisionTypeCB->setCurrentIndex(index);
					}
				}
				ui->Param2LE->setDisabled(false);
				ui->SearchParam2PB->setDisabled(false);

				if (isCheckStatus == true)
				{
					ui->checkFSCB->setChecked(true);
				}
			}
		}
	}
}

void GTACheckValueWidget::updateParameterValue()
{
	//Check for valid conditions
	QString sParameter = ui->Param1LE->text().trimmed();
	if (!(sParameter.trimmed().startsWith("\"") && sParameter.trimmed().endsWith("\"")))
		sParameter.remove(QRegExp("\\s+"));
	bool isCheckValueOnly = ui->checkOnlyValueChB->checkState() == Qt::Checked ? true : false;
	bool isCheckStatus = ui->checkFSCB->checkState() == Qt::Checked ? true : false;
	bool isSDIOnly = ui->chkOnlySDI->isChecked();
	bool isParityOnly = ui->chkOnlyParity->isChecked();
	bool isRefreshOnly = ui->chkRefreshRate->isChecked();

	QString sOperator = ui->Operator1CB->currentText();
	sOperator.remove(QRegExp("\\s+"));

	QString sValue = ui->Param2LE->text().trimmed();
	if (!(sValue.trimmed().startsWith("\"") && sValue.trimmed().endsWith("\"")))
		sValue.remove(QRegExp("\\s+"));

	if (((!isCheckStatus) && (!isSDIOnly) && (!isParityOnly) && (!isRefreshOnly))
		&& (sParameter.isEmpty() || sValue.isEmpty() || sOperator.isEmpty()))
	{
		if (this->isVisible())
			QMessageBox::critical(this, "Empty Field", "Incomplete field");
		return;
	}

	QListWidgetItem* pItem = NULL;
	QList<QListWidgetItem*> lstSelectedAndExp = ui->andExpLW->selectedItems();
	QList<QListWidgetItem*> lstSelectedorExp = ui->orExpLW->selectedItems();

	if (!lstSelectedorExp.isEmpty())
	{
		pItem = lstSelectedorExp.at(0);
		_lastEditedCol = 1;
	}
	else if (!lstSelectedAndExp.isEmpty())
	{
		pItem = lstSelectedAndExp.at(0);
		_lastEditedCol = 0;
	}
	else if (lstSelectedAndExp.isEmpty() && lstSelectedorExp.isEmpty())
	{
		if (this->isVisible())
			QMessageBox::critical(this, "Selection error", "Nothing selected for editing");
		return;
	}

	if (!pItem)
		return; //this should not happen

	QList<QPair<QString, QString>> lstOfParamValuePair;
	bool rc = getParamValuePairForChannel(sParameter, sValue, lstOfParamValuePair);
	if (!rc)
	{
		if (this->isVisible())
			QMessageBox::critical(this, "Error", "Error");
		return;
	}

	for (int i = 0; i<lstOfParamValuePair.count(); i++)
	{
		QPair <QString, QString> paramValPair = lstOfParamValuePair.at(i);
		QString parameterName = paramValPair.first;
		QString parameterVal = paramValPair.second;
		QString sFunctionStatus = getFunctionalStatus();
		QString sPrecisionValue = "0.0";
		QString sPrecisionType;
		QString sSDI = ui->CheckSDICB->currentText();
		QString sParity = ui->CheckParityCB->currentText();

		if (isCheckStatus == true)
		{
			QString expr = QString("%1(FS:%2)").arg(parameterName, sFunctionStatus);
			pItem->setText(expr.append(CHK_FS_STR));
		}
		else if (isSDIOnly)
		{
			QString expr = QString("%1(SDI:%2)").arg(parameterName, sSDI);
			pItem->setText(expr.append(CHK_SDI_STR));
		}
		else if (isParityOnly)
		{
			QString expr = QString("%1(Parity:%2)").arg(parameterName, sParity);
			pItem->setText(expr.append(CHK_PARITY_STR));
		}
		else if (isRefreshOnly)
		{
			QString expr = QString("%2 %1").arg(parameterName, CHK_REFRESH_RATE_STR);
			pItem->setText(expr);
		}
		else
		{
			sPrecisionValue = ui->editPrecisionLE->text();
			QString sOperatorValue = getOperator();
			if (sOperatorValue.contains("line."))
			{
				sPrecisionValue = QString::number(0.0, 'f', 4);
				sOperator = sOperatorValue;
				if (!_OperList.contains(sOperatorValue))
				{
					_OperList << sOperatorValue;
				}
			}

			if (!sPrecisionValue.contains("line."))
			{
				sPrecisionValue = QString::number(sPrecisionValue.toDouble(), 'f', 4);
			}

			sPrecisionType = ui->precisionTypeCB->currentText() == ACT_PRECISION_UNIT_PERCENT ? QString(ACT_PRECISION_UNIT_PERCENT) : QString();

			QString expr = QString("%1(FS:%2) %3 %4 (+/- %5%6)").arg(parameterName, sFunctionStatus, sOperator, parameterVal, sPrecisionValue, sPrecisionType);

			if (isCheckValueOnly)
				expr.append(CHK_VALUE_ONLY_STR);

			pItem->setText(expr);
		}
	}

	QListWidget* editedListWidget = _lastEditedCol == 0 ? ui->andExpLW : ui->orExpLW;
	_lastEditedRow = editedListWidget->selectionModel()->selectedIndexes().at(0).row();
	
	clearParameterListSelection();
	selectLastEditedRow();
	
	emit evaluateExpression();
}

void GTACheckValueWidget::clear()
{
	ui->andExpLW->clear();
	if (ui->Operator1CB->count()>0)
		ui->Operator1CB->setCurrentIndex(0);
	ui->orExpLW->clear();
	ui->Param1LE->clear();
	ui->Param2LE->clear();
	ui->resultExpTE->clear();
	ui->SignLE->clear();
	ui->WaitUntilCB->setChecked(true);
	ui->functionalStatusCB->setCurrentIndex(0);
	ui->editPrecisionLE->setText("0");
	ui->precisionTypeCB->setCurrentIndex(0);
	silentReSetChkOnlyValueCB();
	ui->checkFSCB->setChecked(false);
	ui->SearchParam2PB->setDisabled(false);

	ui->checkOnlyValueChB->setChecked(false);
	ui->chkOnlySDI->setChecked(false);
	ui->CheckSDICB->setCurrentIndex(0);
	ui->chkOnlyParity->setChecked(false);
	ui->CheckParityCB->setCurrentIndex(0);
	ui->chkRefreshRate->setChecked(false);
}

bool GTACheckValueWidget::isValid()
{
	bool rc = true;
	if (ui->andExpLW->count() == 0 && ui->orExpLW->count() == 0)
		rc = false;
	return rc;
}

bool GTACheckValueWidget::getCommand(GTACheckBase *& pCheckCmd)
{
	bool rc = isValid();
	if (rc)
	{
		pCheckCmd = getCheckCommand();
	}
	return rc;
}

bool GTACheckValueWidget::setCommand(const GTACheckBase * pCheckCmd)
{
	bool rc = false;
	if (pCheckCmd != NULL && pCheckCmd->getCheckType() == GTACheckBase::VALUE)
	{
		GTACheckValue * pCmd = dynamic_cast<GTACheckValue*>((GTACheckBase*)pCheckCmd);
		if (pCmd != NULL)
		{
			setCheckCommand(pCmd);
			_lastEditedRow = pCmd->getLastEditedRow();
			_lastEditedCol = pCmd->getLastEditedCol();
			selectLastEditedRow();
			rc = true;
		}
	}
	return rc;
}

/**
 * @brief Select the last edited item in order to fill the Parameter and value fields
*/
void GTACheckValueWidget::selectLastEditedRow()
{
    // Determine which QListWidget to use based on the value of _lastEditedCol.
    QListWidget* editedListWidget = (_lastEditedCol == 0) ? ui->andExpLW : ui->orExpLW;

    // Select the item at _lastEditedRow in the determined QListWidget.
    if (editedListWidget->item(_lastEditedRow)) {
        editedListWidget->item(_lastEditedRow)->setSelected(true);
    } else {
        // Handle the case where _lastEditedRow is out of bounds
        qDebug() << "Error: _lastEditedRow is out of bounds.";
    }
}

void GTACheckValueWidget::waitUntilCheckStatusSlot(bool iStatus)
{
	emit waitUntilCheckStatus(iStatus);
}

void GTACheckValueWidget::importParametersFromCSV()
{
	QString sCsvFilePath = QFileDialog::getOpenFileName(this, tr("Select Database"), "", tr("Excel File (*.csv)"));
	fillExpLWFromCSV(ui->andExpLW, sCsvFilePath);
}

void GTACheckValueWidget::fillExpLWFromCSV(QListWidget*, const QString& iFilePath)
{
	QFileInfo iFileInfo(iFilePath);
	bool rC = true;

	QStringList validLines;
	if (iFileInfo.exists())
	{
		QFile csvFileObj(iFilePath);
		bool fOpenRC = csvFileObj.open(QIODevice::Text | QFile::ReadOnly);
		int lineNo = 0;
		QStringList erroLines;
		if (fOpenRC)
		{
			while (!csvFileObj.atEnd())
			{
				lineNo++;
				QString line = csvFileObj.readLine();
				line = line.trimmed();
				if (validateRowFromCSV(line))
					validLines.append(line);
				else
				{
					rC = false;
					erroLines.append(QString::number(lineNo));
				}
			}
		}

		if (rC)
		{
			foreach(QString line, validLines)
			{
				QStringList paramInfoLst = line.split(";");

				if (paramInfoLst.count() == 5) //change this count check to 8 later
				{
					QString sParameter = paramInfoLst.at(0);
					sParameter = sParameter.trimmed();

					QString sValue = paramInfoLst.at(1);
					sValue = sValue.trimmed();

					QString sFS = paramInfoLst.at(2);
					sFS = sFS.trimmed();

					QString sPrecision = paramInfoLst.at(3).trimmed();
					QString sNumericalPart = sPrecision;
					sNumericalPart.remove(ACT_PRECISION_UNIT_PERCENT);
					bool isNo;
					sNumericalPart.trimmed().toDouble(&isNo);

					QString sCondition = paramInfoLst.at(4);
					sCondition = sCondition.trimmed();

					QString sSDI;
					QString sParity;
					QString sRefresh;
					int index = 5;
					if (index < paramInfoLst.count())
					{
						sSDI = paramInfoLst.at(5);
						sSDI = sSDI.trimmed();
					}
					index++;
					if (index < paramInfoLst.count())
					{
						sParity = paramInfoLst.at(6);
						sParity = sParity.trimmed();
					}
					index++;
					if (index < paramInfoLst.count())
					{
						sRefresh = paramInfoLst.at(7);
						sRefresh = sRefresh.trimmed();
					}

					if ((sCondition.contains(ARITH_LT) || sCondition.contains(ARITH_GT)))
						sPrecision = "0";
					QString sStatement;
					if (!sValue.isEmpty())
						sStatement = QString("%1(FS:%2) %3 %4 (+/- %5)").arg(sParameter, sFS, CHK_FS_STR, sValue, "");
					else if (!sFS.isEmpty())
						sStatement = QString("%1(FS:%2) %3").arg(sParameter, sFS, CHK_FS_STR);
					else if (!sSDI.isEmpty())
						sStatement = QString("%1(SDI:%2) %3").arg(sParameter, sSDI, sCondition, CHK_SDI_STR);
					else if (!sParity.isEmpty())
					{
						sStatement = QString("%1(Parity:%2) %3").arg(sParameter, sParity, sCondition, sValue);
					}
					else if (sRefresh == "true")
					{
						sStatement = QString("%2 %1").arg(sParameter, CHK_REFRESH_RATE_STR);
					}
					ui->andExpLW->addItem(sStatement);
					emit evaluateExpression();
				}
			}
		}
		else
			if (this->isVisible())
				QMessageBox::critical(0, "Parse Error", QString("Error encountered in line(s) [%1]").arg(erroLines.join(",")));
		if (fOpenRC)
			csvFileObj.close();
	}
}

/*
* This SLOT is called after having changed the operator of the CHECK action
*/
void GTACheckValueWidget::togglePrecision(QString condition)
{
	bool val = true;
	ui->precisionTypeCB->setEnabled(val);
	ui->editPrecisionLE->setEnabled(val);

	bool isChecked = (ui->checkOnlyValueChB->isChecked() ||
		ui->checkFSCB->isChecked() ||
		ui->chkOnlySDI->isChecked() ||
		ui->chkOnlyParity->isChecked() ||
		ui->chkRefreshRate->isChecked());

	if (condition == ARITH_NOTEQ || condition == ARITH_EQ)
	{
		if (ui->checkOnlyValueChB->isChecked() || ui->chkRefreshRate->isChecked())
			val = true;
		else if (!isChecked)
			val = true;
		else
			val = false;
		ui->precisionTypeCB->setEnabled(val);
		ui->editPrecisionLE->setEnabled(val);
	}
}

/*
* This SLOT allow to reset the Precision value when the PrecisionType is changed
*/
void GTACheckValueWidget::resetPrecision(QString condition)
{
	bool val = true;
	ui->precisionTypeCB->setEnabled(val);
	ui->editPrecisionLE->setText("0");
	ui->editPrecisionLE->setEnabled(val);

	bool isChecked = (ui->checkOnlyValueChB->isChecked() ||
		ui->checkFSCB->isChecked() ||
		ui->chkOnlySDI->isChecked() ||
		ui->chkOnlyParity->isChecked() ||
		ui->chkRefreshRate->isChecked());

	if (condition == ARITH_NOTEQ || condition == ARITH_EQ)
	{
		if (ui->checkOnlyValueChB->isChecked() || ui->chkRefreshRate->isChecked())
			val = true;
		else if (!isChecked)
			val = true;
		else
			val = false;
		ui->precisionTypeCB->setEnabled(val);
		ui->editPrecisionLE->setEnabled(val);
	}
}

bool GTACheckValueWidget::validateRowFromCSV(const QString& iLine)
{
	bool rC = true;
	if (!iLine.isEmpty())
	{
		QStringList paramInfoLst = iLine.split(";");
		if (paramInfoLst.size() == 5)
		{
			QString sPrecision = paramInfoLst.at(3);
			sPrecision.remove(ACT_PRECISION_UNIT_PERCENT);
			sPrecision = sPrecision.trimmed();
			bool isNo;
			sPrecision.toDouble(&isNo);

			QString sCondition = paramInfoLst.at(4);
			sCondition = sCondition.trimmed();

			QString sParameter = paramInfoLst.at(0);
			sParameter = sParameter.trimmed();

			QString sValue = paramInfoLst.at(1);
			sValue = sValue.trimmed();

			QString sFS = paramInfoLst.at(2);
			sFS = sFS.trimmed();


			QString sSDI;
			QString sParity;
			QString sRefresh;
			int index = 5;
			if (index < paramInfoLst.count())
			{
				sSDI = paramInfoLst.at(5);
				sSDI = sSDI.trimmed();
			}
			index++;
			if (index < paramInfoLst.count())
			{
				sParity = paramInfoLst.at(6);
				sParity = sParity.trimmed();
			}
			index++;
			if (index < paramInfoLst.count())
			{
				sRefresh = paramInfoLst.at(7);
				sRefresh = sRefresh.trimmed();
			}

			if (!_OperList.contains(sCondition))//condition should belong to operator List
				rC = false;

			else if (sPrecision.isEmpty() == false && isNo == false)
			{
				rC = false;
			}
			else if (!_sFSList.contains(sFS))
			{
				rC = false;
			}

			else if (sValue.isEmpty())
			{
				if (sFS.isEmpty())
					return false;
				if (sCondition != ARITH_NOTEQ && sCondition != ARITH_EQ)
					return false;
			}
		}
		else
			rC = false;
	}
	return rC;
}

void GTACheckValueWidget::setPrecisionValue(const QString &iPrecision)
{
	if (_isParameterLE)
		ui->editPrecisionLE->setText(iPrecision);
}

void GTACheckValueWidget::extractCheckItemsFromCheckExpression(QString  iChkExp,
	QString& osParameter,
	QString& osConditionalExp,
	QString& osValue,
	QString& osFunctionalStatus,
	double&  oPrecisonVal,
	QString& oPrecisonValFE,
	QString& osPrecisonType,
	bool& isChekOnlyValue,
	bool& isCheckFs,
	bool &isSDI,
	bool &isParity,
	bool &isRefreshRate,
	QString &iSDIVal,
	QString &iParity)
{
	QString currentExpression = iChkExp;

	if (currentExpression.contains(CHK_FS_STR))
	{
		currentExpression.remove(CHK_FS_STR);
		QStringList splitExp = currentExpression.split(CHK_FS_STR, QString::SkipEmptyParts);
		QString sParameter = splitExp.at(0).trimmed();
		if (!(sParameter.startsWith("\"") && sParameter.endsWith("\"")))
			sParameter.remove(QRegExp("\\s+"));

		QStringList parameterInfoLst = sParameter.split("(FS:");
		QString sFunctionalStatus = ACT_FSSSM_NO;

		if (parameterInfoLst.size() == 2)
		{
			sParameter = parameterInfoLst.at(0);
			sFunctionalStatus = parameterInfoLst.at(1);
			sFunctionalStatus.remove(")");
		}

		QString oper = "=";

		foreach(QString conditionalExp, _OperList)
		{
			if (currentExpression.contains(conditionalExp))
			{
				oper = conditionalExp;
				sParameter.replace(oper, "");
			}
		}

		osParameter = sParameter;
		osConditionalExp = oper;
		osValue = "";
		osFunctionalStatus = sFunctionalStatus;
		oPrecisonVal = 0.0;
		osPrecisonType = ACT_PRECISION_UNIT_VALUE;
		isChekOnlyValue = false;
		isCheckFs = true;
		isSDI = false;
		isParity = false;
		isRefreshRate = false;
		iSDIVal = QString("00");
		iParity = QString("0");
		return;
	}
	else if (currentExpression.contains(CHK_SDI_STR))
	{

		currentExpression.remove(CHK_SDI_STR);
		QStringList splitExp = currentExpression.split(CHK_SDI_STR, QString::SkipEmptyParts);
		QString sParameter = splitExp.at(0).trimmed();
		if (!(sParameter.startsWith("\"") && sParameter.endsWith("\"")))
			sParameter.remove(QRegExp("\\s+"));

		QStringList parameterInfoLst = sParameter.split("(SDI:");
		QString sSDIStatus = QString("00");

		if (parameterInfoLst.size() == 2)
		{
			sParameter = parameterInfoLst.at(0);
			sSDIStatus = parameterInfoLst.at(1);
			sSDIStatus.remove(")");
		}

		QString oper = "=";

		foreach(QString conditionalExp, _OperList)
		{
			if (currentExpression.contains(conditionalExp))
			{
				oper = conditionalExp;
			}
		}

		osParameter = sParameter;
		osConditionalExp = oper;
		osValue = "";
		osFunctionalStatus = ACT_FSSSM_NO;
		oPrecisonVal = 0.0;
		osPrecisonType = ACT_PRECISION_UNIT_VALUE;
		isChekOnlyValue = false;
		isCheckFs = false;
		isSDI = true;
		isParity = false;
		isRefreshRate = false;
		iSDIVal = sSDIStatus;
		iParity = QString("0");
		return;
	}
	else if (currentExpression.contains(CHK_PARITY_STR))
	{
		currentExpression.remove(CHK_PARITY_STR);
		QStringList splitExp = currentExpression.split(CHK_PARITY_STR, QString::SkipEmptyParts);
		QString sParameter = splitExp.at(0).trimmed();
		if (!(sParameter.startsWith("\"") && sParameter.endsWith("\"")))
			sParameter.remove(QRegExp("\\s+"));

		QStringList parameterInfoLst = sParameter.split("(Parity:");
		QString sParityStatus = QString("0");

		if (parameterInfoLst.size() == 2)
		{
			sParameter = parameterInfoLst.at(0);
			sParityStatus = parameterInfoLst.at(1);
			sParityStatus.remove(")");
		}

		QString oper = "=";

		foreach(QString conditionalExp, _OperList)
		{
			if (currentExpression.contains(conditionalExp))
			{
				oper = conditionalExp;
			}
		}

		osParameter = sParameter;
		osConditionalExp = oper;
		osValue = "";
		osFunctionalStatus = ACT_FSSSM_NO;
		oPrecisonVal = 0.0;
		osPrecisonType = ACT_PRECISION_UNIT_VALUE;
		isChekOnlyValue = false;
		isCheckFs = false;
		isSDI = false;
		isParity = true;
		isRefreshRate = false;
		iSDIVal = QString("00");
		iParity = sParityStatus;
		return;
	}
	else if (currentExpression.contains(CHK_REFRESH_RATE_STR))
	{
		currentExpression.remove(CHK_REFRESH_RATE_STR);
		QStringList splitExp = currentExpression.split(CHK_REFRESH_RATE_STR, QString::SkipEmptyParts);
		QString sParameter = splitExp.at(0).trimmed();
		if (!(sParameter.startsWith("\"") && sParameter.endsWith("\"")))
			sParameter.remove(QRegExp("\\s+"));

		QString oper = "=";

		osParameter = sParameter;
		osConditionalExp = oper;
		osValue = "";
		osFunctionalStatus = ACT_FSSSM_NO;
		oPrecisonVal = 0.0;
		osPrecisonType = ACT_PRECISION_UNIT_VALUE;
		isChekOnlyValue = false;
		isCheckFs = false;
		isSDI = false;
		isParity = false;
		isRefreshRate = true;
		iSDIVal = QString("00");
		iParity = QString("0");
		return;
	}
	else
	{
		isCheckFs = false;
		isCheckFs = false;
		isSDI = false;
		isParity = false;
		isRefreshRate = false;
		bool isListBoxOper = false;

		if (currentExpression.contains(CHK_VALUE_ONLY_STR))
			isChekOnlyValue = true;
		else
			isChekOnlyValue = false;


		currentExpression.remove(CHK_VALUE_ONLY_STR);
		QRegularExpression pattern("<|<=|>|>=|=|NOT=");
		if (pattern.match(currentExpression).hasMatch())
		{
			isListBoxOper = true;
		}

		if (currentExpression.contains("line") && !isListBoxOper)
		{
			int i = 0;
		    QStringList splitExp = currentExpression.split("line.", QString::SkipEmptyParts);
			while (i < splitExp.size())
			{
				splitExp[i].prepend("line.");
				i++;
			}
			QString sOperator = splitExp[1];
			_OperList << sOperator;

		}
		
		foreach(QString conditionalExp, _OperList)
		{
			if (currentExpression.contains(conditionalExp))
			{
				QStringList splitExp = currentExpression.split(conditionalExp, QString::SkipEmptyParts);

				if (splitExp.size() == 2)
				{
					QString sParameter = splitExp.at(0).trimmed();

					QStringList parameterInfoLst = sParameter.split("(FS:");
					QString sFunctionalStatus = ACT_FSSSM_NO;

					if (parameterInfoLst.size() == 2)
					{
						sParameter = parameterInfoLst.at(0).trimmed();
						if (!(sParameter.startsWith("\"") && sParameter.endsWith("\"")))
							sParameter.remove(QRegExp("\\s+"));
						sFunctionalStatus = parameterInfoLst.at(1);
						sFunctionalStatus.remove(QRegExp("\\s+"));
						sFunctionalStatus.remove(")");
					}

					QString sValue = splitExp.at(1);
					sValue = sValue.trimmed();

					QString sPrecisonType = ACT_PRECISION_UNIT_VALUE;
					double sPrecisonVal = 0.0;


					QStringList sValueInfo = sValue.split("(");
					if (sValueInfo.size() == 2)
					{
						sValue = sValueInfo.at(0);

						QString sPrecInfo = sValueInfo.at(1);
						sPrecInfo.remove("+/-");
						sPrecInfo.remove(")");


						if (sPrecInfo.contains(ACT_PRECISION_UNIT_PERCENT))
							sPrecisonType = ACT_PRECISION_UNIT_PERCENT;

						sPrecisonVal = sPrecInfo.remove(ACT_PRECISION_UNIT_PERCENT).toDouble();
						if (sPrecInfo != QString::number(sPrecisonVal))
							oPrecisonValFE = sPrecInfo.trimmed();
					}
					osParameter = sParameter;
					osConditionalExp = conditionalExp;
					osValue = sValue.trimmed();
					osFunctionalStatus = sFunctionalStatus;
					oPrecisonVal = sPrecisonVal;
					osPrecisonType = sPrecisonType;
					return;
				}
			}
		}
	}
}

/* This function allows to check the OnlyValue CheckBox
* @return: None
*/
void GTACheckValueWidget::silentSetChkValueOnlyCB()
{
	ui->checkOnlyValueChB->setChecked(true);
}

/* This function allows to uncheck the OnlyFS CheckBox
* @return: None
*/
void GTACheckValueWidget::silentResetChkOnlyFSCB()
{
	ui->checkFSCB->setChecked(false);
}

/* This function allows to uncheck the OnlyValue CheckBox
* @return: None
*/
void GTACheckValueWidget::silentReSetChkOnlyValueCB()
{
	ui->checkOnlyValueChB->setChecked(false);
}

/* This function allow to reset all checkBox/ComboBox states 
 * @return : None
*/
void GTACheckValueWidget::resetAllCheckBoxStatus()
{
	ui->checkOnlyValueChB->setChecked(false);
	ui->checkFSCB->setChecked(false);
	ui->chkOnlySDI->setChecked(false);
	ui->chkOnlyParity->setChecked(false);
	ui->chkRefreshRate->setChecked(false);

	ui->checkOnlyValueChB->setEnabled(true);
	ui->checkFSCB->setEnabled(true);
	ui->chkOnlySDI->setEnabled(true);
	ui->chkOnlyParity->setEnabled(true);
	ui->chkRefreshRate->setEnabled(true);

	ui->functionalStatusCB->setEnabled(true);
	ui->functionalStatusCB->setCurrentIndex(0);
	ui->CheckSDICB->setEnabled(true);
	ui->CheckParityCB->setEnabled(true);
}

/* This function allows to automatically tick an option if the Parameter has a Media VMAC or DIS.
* @return : None
*/
void GTACheckValueWidget::setOnlyValueStatus()
{
	ui->checkOnlyValueChB->setChecked(true);
	ui->checkFSCB->setChecked(false);
	ui->chkOnlySDI->setChecked(false);
	ui->chkOnlyParity->setChecked(false);
	ui->chkRefreshRate->setChecked(false);

	ui->checkOnlyValueChB->setEnabled(false);
	ui->checkFSCB->setEnabled(false);
	ui->chkOnlySDI->setEnabled(false);
	ui->chkOnlyParity->setEnabled(false);
	ui->chkRefreshRate->setEnabled(false);

	ui->functionalStatusCB->setEnabled(false);
	ui->functionalStatusCB->setCurrentIndex(1);
	ui->CheckSDICB->setEnabled(false);
	ui->CheckParityCB->setEnabled(false);
}

/* This SLOT allows to evaluate the entered Parameter and to detect its media
* @sender: this - textChanged() of the Parameter field
* @input: none
* @return: void
*/
void GTACheckValueWidget::evaluateParamMedia(const QString &)
{
	ui->Param2LE->setDisabled(false);
	QString param = ui->Param1LE->text().simplified();

	if (!param.isEmpty() && !param.contains(GTA_CIC_IDENTIFIER))
	{
		GTAAppController* pCtrlr = GTAAppController::getGTAAppController();

		if (NULL == pCtrlr)
			return;

		QStringList paramList = QStringList() << param;
		QList<GTAICDParameter> icdDetailList;
		pCtrlr->getICDDetails(paramList, icdDetailList);
		bool isicdEmpty_and_islineParam = icdDetailList.isEmpty() && !((param.contains("@") && param.count('@', Qt::CaseInsensitive) % 2 == 0) || param.startsWith("line."));

		if (isicdEmpty_and_islineParam)
		{
			setOnlyValueStatus();

		}
		else
		{
			resetAllCheckBoxStatus();
		}
	}
	else
	{
		// If the Parameter field is empty, reset state of checkboxes
		resetAllCheckBoxStatus();
	}
}


void GTACheckValueWidget::silentUpdateParam1LE(const QString& iString)
{
	disconnect(ui->Param1LE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateParamMedia(const QString &)));
	ui->Param1LE->setText(iString);
	connect(ui->Param1LE, SIGNAL(textChanged(const QString &)), this, SLOT(evaluateParamMedia(const QString &)));
}
void GTACheckValueWidget::resetCheckFS(bool iStatus)
{
	if (iStatus && ui->checkFSCB->isChecked())
		ui->checkFSCB->setChecked(false);
	ui->checkFSCB->setDisabled(iStatus);
	ui->EditFSLE->setDisabled(iStatus);
	ui->SearchFSPB->setDisabled(iStatus);
}
void GTACheckValueWidget::resetCheckOnlyValue(bool iStatus)
{
	if (iStatus && ui->checkOnlyValueChB->isChecked())
		ui->checkOnlyValueChB->setChecked(false);
	ui->checkOnlyValueChB->setDisabled(iStatus);
}
bool GTACheckValueWidget::getParamValuePairForChannel(QString iParamName, QString iParamValue, QList<QPair<QString, QString> >& olstParamValPair)
{
	bool rc = true;

	QStringList channelParameters = GTAUtil::getProcessedParameterForChannel(iParamName);
	QStringList channelValues = GTAUtil::getProcessedParameterForChannel(iParamValue);

	int noOfParams = channelParameters.size();
	int noOfValues = channelValues.size();

	if (noOfValues > noOfParams)
	{
		return false;
	}

	else
	{
		for (int i = 0; i<noOfParams; i++)
		{
			QPair <QString, QString> paramValPair;
			paramValPair.first = channelParameters.at(i);

			if (noOfParams == noOfValues)
				paramValPair.second = channelValues.at(i);
			else
				paramValPair.second = channelValues.at(0);

			olstParamValPair.append(paramValPair);
		}
	}

	return rc;
}
void GTACheckValueWidget::setAutoCompleteItems(const QStringList& iWordList)
{

	if (iWordList.isEmpty() == false)
	{
		QCompleter *pCompleter = NULL;
		pCompleter = ui->Param1LE->completer();
		if (pCompleter)
		{
			delete pCompleter; pCompleter = NULL;
		}


		pCompleter = ui->Param2LE->completer();
		if (pCompleter)
		{
			delete pCompleter; pCompleter = NULL;
		}

		pCompleter = new QCompleter(iWordList, this);
		pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
		ui->Param1LE->setCompleter(pCompleter);
		ui->Param2LE->setCompleter(pCompleter);
	}

}

bool GTACheckValueWidget::hasSearchItem()const
{
	return true;
}
bool GTACheckValueWidget::hasActionOnFail()const
{
	return true;
}
bool GTACheckValueWidget::hasPrecision()const
{
	return false;
}
bool GTACheckValueWidget::hasTimeOut()const
{
	return true;
}
bool GTACheckValueWidget::hasDumpList()const
{
	return true;
}
bool GTACheckValueWidget::hasConfigTime() const
{
	GTAAppController *pAppCtrl = GTAAppController::getGTAAppController();
	if (pAppCtrl->isGenericSCXMLMode() == true)
		return false;
	return true;
}

void GTACheckValueWidget::resetSDI(bool iStatus)
{

	if (iStatus && ui->chkOnlySDI->isChecked())
	{
		disconnect(ui->chkOnlySDI, SIGNAL(toggled(bool)), this, SLOT(disableValueEdits(bool)));
		ui->chkOnlySDI->setChecked(false);
		connect(ui->chkOnlySDI, SIGNAL(toggled(bool)), this, SLOT(disableValueEdits(bool)));
	}
	ui->chkOnlySDI->setDisabled(iStatus);
}

void GTACheckValueWidget::resetParity(bool iStatus)
{
	if (iStatus && ui->chkOnlyParity->isChecked())
	{
		disconnect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(disableValueEdits(bool)));
		ui->chkOnlyParity->setChecked(false);
		connect(ui->chkOnlyParity, SIGNAL(toggled(bool)), this, SLOT(disableValueEdits(bool)));
	}

	ui->chkOnlyParity->setDisabled(iStatus);
}

void GTACheckValueWidget::resetRefreshRate(bool iStatus)
{
	if (iStatus && ui->chkRefreshRate->isChecked())
	{
		disconnect(ui->chkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(disableValueEditsOnRefresh(bool)));
		ui->chkRefreshRate->setChecked(false);
		connect(ui->chkRefreshRate, SIGNAL(toggled(bool)), this, SLOT(disableValueEditsOnRefresh(bool)));
	}

	ui->chkRefreshRate->setDisabled(iStatus);
}

QString GTACheckValueWidget::getParity() const
{
	return ui->CheckParityCB->currentText();
}

QString GTACheckValueWidget::getSDI()const
{
	return ui->CheckSDICB->currentText();
}

bool GTACheckValueWidget::getCheckOnlyRefreshRateState()const
{
	return ui->chkRefreshRate->isChecked();
}

bool GTACheckValueWidget::getCheckOnlySDIState()const
{
	return ui->chkOnlySDI->isChecked();
}

bool GTACheckValueWidget::getCheckOnlyParityState()const
{
	return ui->chkOnlyParity->isChecked();
}

bool GTACheckValueWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
	for (auto param : params)
	{
		if (param.first == "Param1LE")
		{
			if (param.second != "") ui->Param1LE->setText(param.second);
			else return false;
		}
		else if(param.first == "Operator1CB")
		{
			int currIdx = ui->Operator1CB->findText(param.second);
			if (currIdx >= 0) ui->Operator1CB->setCurrentIndex(currIdx);
			else ui->Operator1CB->setCurrentIndex(0);
		}
		else if(param.first == "Param2LE")
		{
			if(param.second != "") ui->Param2LE->setText(param.second);
		}
		else if(param.first == "ChosenList")
		{
			if (param.second == "second") ui->addOrExpPB->click();
			else if (param.second == "first") ui->addAndExpPB->click();
		}
		else if(param.first == "editPrecisionLE")
		{
			bool paramSecondIsNumber = false;
			param.second.toDouble(&paramSecondIsNumber);
			if (paramSecondIsNumber)
				ui->editPrecisionLE->setText(param.second);
			else
				ui->editPrecisionLE->setText("0");
		}
		else if(param.first == "precisionTypeCB")
		{
			int currIdx = ui->precisionTypeCB->findText(param.second);
			if (currIdx >= 0) ui->precisionTypeCB->setCurrentIndex(currIdx);
			else ui->precisionTypeCB->setCurrentIndex(0);
		}
		else if(param.first == "functionalStatusCB")
		{
			int currIdx = ui->functionalStatusCB->findText(param.second);
			if (currIdx >= 0) ui->functionalStatusCB->setCurrentIndex(currIdx);
			else ui->functionalStatusCB->setCurrentIndex(0);
		}
		else if(param.first == "CheckSDICB")
		{
			int currIdx = ui->CheckSDICB->findText(param.second);
			if (currIdx >= 0) ui->CheckSDICB->setCurrentIndex(currIdx);
			else ui->CheckSDICB->setCurrentIndex(0);
		}
		else if(param.first == "CheckParityCB")
		{
			int currIdx = ui->CheckParityCB->findText(param.second);
			if (currIdx >= 0) ui->CheckParityCB->setCurrentIndex(currIdx);
			else ui->CheckParityCB->setCurrentIndex(0);
		}
		else if(param.first == "CheckOnly")
		{
			if (param.second == "Value") ui->checkOnlyValueChB->setChecked(true);
			else if (param.second == "Rate") ui->chkRefreshRate->setChecked(true);
			else if (param.second == "FS")
			{
				ui->checkFSCB->setChecked(true);
				ui->checkOnlyValueChB->setChecked(false);
			}
			else if (param.second == "SDI") ui->chkOnlySDI->setChecked(true);
			else if (param.second == "Parity") ui->chkOnlyParity->setChecked(true);
			else {
				errorLogs.append(QString("Check value unexpected second param: %1").arg(param.second));
			}
		}
		else
		{
			errorLogs.append(QString("Check value unexpected first param: %1").arg(param.first));
		}
	}
	if (ui->Param2LE->text() == ""
		&& !ui->checkOnlyValueChB->isChecked()
		&& !ui->chkRefreshRate->isChecked()
		&& !ui->checkFSCB->isChecked()
		&& !ui->chkOnlySDI->isChecked()
		&& !ui->chkOnlyParity->isChecked()
		)
	{
		return false;
	}
	return true;
}
