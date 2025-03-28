#include "GTAActionCallWidget.h"
#pragma (push, 0)
#include "ui_GTAActionCallWidget.h"
#include <QTableWidgetItem>
#include <QMessageBox>
#pragma (pop) //QT libs not separated because of circular dependency issues
#include "GTACommandBase.h"
#include "GTAAppController.h"
#include "GTAActionCall.h"
#include "GTAParamValidator.h"
#include "GTAGenSearchWidget.h"
#include "GTAParamValueValidator.h"
#include "GTAICDParameter.h"

#define COL_CNT 2
#define STR_SEARCH_TXT "Enter value or double click for more options"

GTAActionCallWidget::GTAActionCallWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAActionCallWidget)
{
    ui->setupUi(this);
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    //connect(ui->tagTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT());
    connect(ui->ParamNameLE,SIGNAL(textChanged(QString)),this,SLOT(setTagTable(QString)));
    //dataValidatorForCommand validator;
    //QString inputMask = GTAUtil::getListOfForbiddenChars();
    //ui->ParamNameLE->setValidator(&validator);

    _pSearchWidget = new GTAGenSearchWidget();
    if(_pSearchWidget)
    {
        _pSearchWidget->hide();
        connect(_pSearchWidget,SIGNAL(okPressed()),this,SLOT(onSearchOKPBClick()));
    }
    _SearchTagLineEdit = new QLineEdit();
    ui->MonitorNameLE->setVisible(false);
    ui->MonitorNameLB->setVisible(false);
//    ui->MonitorTypeCB->setVisible(false);

    ui->ContinuousLoopTimeLE->setVisible(false);
    ui->ContinuousUnitCB->setVisible(false);
    ui->ChkContinuousLoop->setVisible(false);

    QStringList unit;
    unit << "sec" << "msec";
    ui->ContinuousUnitCB->addItems(unit);

    QStringList monTypes;
    monTypes<<ELEM_PROC<<ELEM_FUNC;
    ui->MonitorTypeCB->addItems(monTypes);
}

GTAActionCallWidget::~GTAActionCallWidget()
{
    if(_pSearchWidget != NULL)
    {
        delete _pSearchWidget;
        _pSearchWidget = NULL;
    }

    if(_SearchTagLineEdit != NULL)
    {
        delete _SearchTagLineEdit;
        _SearchTagLineEdit = NULL;
    }
    delete ui;
}
QString GTAActionCallWidget::getValue() const
{
    return ui->ParamNameLE->text();
}
void GTAActionCallWidget::setValue(const QString & iVal)
{
    ui->ParamNameLE->setText(iVal);
}
void GTAActionCallWidget::setActionCommand(const GTAActionBase * ipActCmd)
{
    clear();

    GTAActionCall* pCallCmd = dynamic_cast<GTAActionCall*> ((GTAActionBase *)ipActCmd);
    if (NULL!=pCallCmd) { 
        setComplement(ipActCmd->getComplement());
        QString elementName = pCallCmd->getElement();
        QString monName = pCallCmd->getMonitorName();
        _RefFileUUID = pCallCmd->getRefrenceFileUUID();
        setValue(elementName);
        ui->MonitorNameLE->setText(monName);
        ui->ChkContinuousLoop->setChecked(pCallCmd->isContinuousLoop());
        ui->ContinuousLoopTimeLE->setText(pCallCmd->getContinuousLoopTime());
        if (pCallCmd->getElement().contains(".pro")) {
            ui->MonitorTypeCB->setCurrentIndex(0);
        }
        else {
            ui->MonitorTypeCB->setCurrentIndex(1);
        }

        QString unit = pCallCmd->getContinuousLoopTimeUnit();
        int index = ui->ContinuousUnitCB->findText(unit);
        if(index >= 0 && index < ui->ContinuousUnitCB->count()) {
            ui->ContinuousUnitCB->setCurrentIndex(index);
        }
        if (!elementName.isEmpty()) {
            this->setTagTable(elementName);

            int noOfRows = ui->tagTableWidget->rowCount();
            QMap<QString, QString> tagValues = pCallCmd->getTagValue();
            for (int i=0; i< ui->tagTableWidget->rowCount(); ++i) {
                QTableWidgetItem *pVarItem =NULL;
                pVarItem = ui->tagTableWidget->item(i,0);

                if (pVarItem!=NULL && tagValues.contains(pVarItem->text())) {
                    QTableWidgetItem *pValueItem =NULL;
                    pValueItem = ui->tagTableWidget->item(i,1);
                    if (NULL!=pValueItem) {
                        pValueItem->setText(tagValues.value(pVarItem->text()));
                    }
                }
            }
        }
    }
}
void GTAActionCallWidget::onSearchPBClicked()
{
    //    emit searchObject(ui->ParamNameLE);
    if(_SearchTagLineEdit)
    {
        _CurrSearchType = (GTAGenSearchWidget::ElementType)getSearchType();
        _pSearchWidget->setSearchType(_CurrSearchType);
        _pSearchWidget->show();
    }
}


void GTAActionCallWidget::clear() 
{
    ui->ParamNameLE->clear();
    ui->MonitorNameLE->clear();
    resetTableWidget();
    ui->ChkContinuousLoop->setChecked(false);
    ui->ContinuousLoopTimeLE->clear();
    ui->ContinuousUnitCB->setCurrentIndex(0);
}
bool GTAActionCallWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    bool rc = false;
    GTAActionCall* pCall = NULL;

    if (isValid())
    {
        pCall= new GTAActionCall;
        pCall->setElement(ui->ParamNameLE->text());

        QString monName = ui->MonitorNameLE->text();
        pCall->setMonitorName(monName);

        bool bIsContinuous = ui->ChkContinuousLoop->isChecked();
        pCall->setIsContinuousLoop(bIsContinuous);

        pCall->setContinuousLoopTime(ui->ContinuousLoopTimeLE->text());
        pCall->setContinuousLoopTimeUnit(ui->ContinuousUnitCB->currentText());

        QMap<QString,QString> varValMap;
        int noOfRows = ui->tagTableWidget->rowCount();
        for (int i=0;i<noOfRows;i++)
        {
            QTableWidgetItem *pVarItem =NULL;
            pVarItem = ui->tagTableWidget->item(i,0);


            int noOfRows = ui->tagTableWidget->rowCount();
            for (int i=0;i<noOfRows;i++)
            {
                QTableWidgetItem *pVarItem =NULL;
                pVarItem = ui->tagTableWidget->item(i,0);


                QTableWidgetItem *pValueItem =NULL;
                pValueItem = ui->tagTableWidget->item(i,1);


                if (NULL!=pValueItem && NULL!=pVarItem)
                {
                    QString sVar = pVarItem->text();
                    QString sVal = pValueItem->text();

                    if(sVal == QString(STR_SEARCH_TXT))
                    {
                        sVal = QString();
                    }
                    if (!sVal.isEmpty() && !sVar.isEmpty())
                    {
                        /* sVal.remove("\"");
                     sVal.remove("'");
                     sVal.remove("`");*/

                        varValMap[sVar] = sVal;
                    }
                }

            }
        }
        pCall->setTagValue(varValMap);
        pCall->setRefrenceFileUUID(_RefFileUUID);
        rc = true;

    }
    opCmd=pCall;
    return rc;
    
}
void GTAActionCallWidget::resetTableWidget()
{
    ui->tagTableWidget->clear();
    ui->tagTableWidget->setColumnCount(COL_CNT);
    ui->tagTableWidget->setRowCount(0);
    QStringList headerLabels;
    headerLabels<<"Variable"<<"Value"<<"Search";
    ui->tagTableWidget->setHorizontalHeaderLabels(headerLabels);
    //ui->tagTableWidget->setRes

}
bool GTAActionCallWidget::isValid()const
{
    QString sCalledItem = ui->ParamNameLE->text();
    if (sCalledItem.isEmpty())
    {        
        _LastError = QString("%1 is empty").arg(ui->paramLB->text());
        return false;
    }


    int noOfRows = ui->tagTableWidget->rowCount();

    for (int i=0;i<noOfRows;i++)
    {
        QTableWidgetItem *pValueItem =NULL;
        pValueItem = ui->tagTableWidget->item(i,1);
        if (pValueItem!=NULL)
        {
            QString sVal = pValueItem->text(); //TODO:are empty spaces allowed??
            if(sVal == QString(STR_SEARCH_TXT))
            {
                sVal = QString();
            }
            sVal.remove(QRegExp("\\s+"));
            GTAParamValueValidator validatorObj;
            int lastChar = 0;
            lastChar = sVal.size()-1;

            if (sVal.isEmpty() )
            {
                _LastError = QString("Value against tag : %1 is empty").arg(ui->tagTableWidget->item(i,0)->text());
                return false;
            }
            //else if (  validatorObj.validate(sVal,lastChar) == QValidator::Invalid )
            //{
            //	_LastError = QString("Value against tag : %1 contains forbidden charater(s)").arg(ui->tagTableWidget->item(i,0)->text());
            //	return false;
            //}
        }
    }
    _LastError = "";
    return true;
}
void GTAActionCallWidget::addTag( const TagVariablesDesc& iTagVar , const QString& iTagVal)
{
    int noOfRows = ui->tagTableWidget->rowCount();
    ui->tagTableWidget->setColumnCount(COL_CNT);
    ui->tagTableWidget->setRowCount(noOfRows+1);
    // Display the header (QTableView) and make possible to resize column's size
    ui->tagTableWidget->horizontalHeader()->setVisible(true);

    QTableWidgetItem *pNewItem = new QTableWidgetItem();
    pNewItem->setText(iTagVar.name);
    pNewItem->setToolTip(QString("Type : %1 \nDescription : %6 \nUnit : %2 \nSupported Value : %3 \nMax : %4 \nMin : %5").arg(iTagVar.type,iTagVar.unit,iTagVar.supportedValue,iTagVar.max,iTagVar.min,iTagVar.Desc));
    ui->tagTableWidget->setItem(noOfRows,0,pNewItem);
    pNewItem->setFlags(Qt::ItemIsSelectable); // make it selectable only

    QTableWidgetItem *pNewItemValue = new QTableWidgetItem();
    connect(ui->tagTableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onTagTableSearchClicked(int,int)));
    pNewItemValue->setText(STR_SEARCH_TXT);

    QFont font;
    font.setWeight(QFont::Light);
    font.setStyle(QFont::StyleItalic);
    pNewItemValue->setFont(font);
    ui->tagTableWidget->setItem(noOfRows,1,pNewItemValue);
    ui->tagTableWidget->show();

    if (!iTagVal.isEmpty())
    {
        pNewItem->setText(iTagVal);
    }
}


void GTAActionCallWidget::onTagTableSearchClicked(int row,int col)
{
    //    QObject * obj = sender();
    //    QString rowNo = obj->objectName().replace("SearchButton_","").trimmed();

    if(col == 1)
    {
        QString rowNo = QString::number(row);
        if(_SearchTagLineEdit)
        {
            _CurrSearchType = GTAGenSearchWidget::PARAMETER;
            _SearchTagLineEdit->setObjectName(rowNo);
            _pSearchWidget->setSearchType(_CurrSearchType);
            _pSearchWidget->show();
        }
    }
}
void GTAActionCallWidget::onSearchOKPBClick()
{
    GTAICDParameter items = _pSearchWidget->getSelectedItems();

    if(_CurrSearchType == GTAGenSearchWidget::PARAMETER)
    {
        QString rowNo = _SearchTagLineEdit->objectName();
        int row = rowNo.toInt();
        QTableWidgetItem *pItem = ui->tagTableWidget->item(row,1);
        pItem->setText(items.getName());
    }
    else
    {
		_RefFileUUID = items.getUUID();
        ui->ParamNameLE->setText(items.getName());
    }
    _pSearchWidget->hide();
}
void GTAActionCallWidget::setTagTable(QString isFileName)
{
    
    //TODO: This function to be moved to appcontroller;
    GTAParamValueValidator paramValidator;
    //    int posToCheck= isFileName.size()-1;
    //    if ( paramValidator.validate(isFileName,posToCheck) == QValidator::Invalid)
    //    {
    //        ui->ParamNameLE->clear();
    //        QMessageBox::information(this,"Value Error","File name contains forbidden charachters");
    //        return;
    //    }


    GTAAppController* pController = GTAAppController::getGTAAppController();
    GTAElement* pElement =NULL;
    bool rC = pController->getElementFromDocument(_RefFileUUID,pElement,true);
    resetTableWidget();

    if (rC && NULL!=pElement) {
        QList<TagVariablesDesc> tagList = pElement->getTagList();
        if (!tagList.isEmpty()) {
            ui->tagTableWidget->show();
            foreach(TagVariablesDesc sTagName, tagList) {
                if (!sTagName.name.isEmpty()) {
                    addTag(sTagName);
                }
            }
        }
        else {
            QString sText = QString("No variables found in %1").arg(isFileName);
        }
        delete pElement;
        pElement=NULL;
    }
    else
    {
        _LastError=pController->getLastError();
    }
    
}
void GTAActionCallWidget::setTagTableVisibility(bool iVal)
{
    ui->tagTableWidget->setVisible(iVal);
}
//void GTAActionCallWidget::keyPressEvent ( QKeyEvent * event )
//{
//    QWidget::keyPressEvent(event);
//
//}
void GTAActionCallWidget::setParamLabel(const QString& iComplementValue)
{
    ui->paramLB->setText(iComplementValue);
}
void GTAActionCallWidget::setComplement(const QString& iComplement)
{
    _complement=iComplement;
    ui->MonitorNameLB->setVisible(false);
    ui->MonitorNameLE->setVisible(false);
    ui->MonitorTypeCB->setVisible(false);
    ui->ContinuousLoopTimeLE->setVisible(false);
    ui->ContinuousUnitCB->setVisible(false);
    ui->ChkContinuousLoop->setVisible(false);
    if( iComplement == COM_CALL_OBJ)
    {

        setTagTableVisibility(true);
        setParamLabel(iComplement);
    }
    else if(iComplement == COM_CALL_PROC)
    {
        
        setTagTableVisibility(false);
        setParamLabel(iComplement);
    }
    else if(iComplement == COM_CALL_FUNC)
    {
        
        setTagTableVisibility(true);
        setParamLabel(iComplement);
    }
    else if(iComplement == COM_CALL_MON)
    {
//        ui->MonitorNameLB->setVisible(true);
//        ui->MonitorNameLE->setVisible(true);
        ui->MonitorTypeCB->setVisible(true);
//        ui->ContinuousLoopTimeLE->setVisible(true);
//        ui->ContinuousUnitCB->setVisible(true);
//        ui->ChkContinuousLoop->setVisible(true);
    }
}

int GTAActionCallWidget::getSearchType()
{
    int retVal = -1;
    if(_complement == COM_CALL_OBJ)
        retVal = GTAGenSearchWidget::OBJECT;
    else if(_complement == COM_CALL_FUNC)
        retVal = GTAGenSearchWidget::FUNCTION;
    else if(_complement == COM_CALL_PROC)
        retVal = GTAGenSearchWidget::PROCEDURE;
    else if(_complement == COM_CALL_MON)
    {
        QString MonType = ui->MonitorTypeCB->currentText();
        if(MonType == ELEM_PROC)
            retVal = GTAGenSearchWidget::PROCEDURE;
        else if(MonType == ELEM_FUNC)
            retVal = GTAGenSearchWidget::FUNCTION;
        else
            retVal = GTAGenSearchWidget::FUNCTION;
    }

    return retVal;
}

bool GTAActionCallWidget::setWidgetFields(const QList<QPair<QString, QString>>& params, ErrorMessageList& errorLogs)
{
    for (const auto& param : params) {
        if (param.first == COM_CALL_OBJ && param.second != "") {
            setComplement(COM_CALL_OBJ);
            setComplementFields(param.second);
            break;
        }
        else if (param.first == COM_CALL_FUNC && param.second != "") {
            setComplement(COM_CALL_FUNC);
            setComplementFields(param.second);
            break;
        }
    }

    //Not sure if complement has to be set first
    QMap<QString, QString> automaticArguments;
    for (const auto& param : params) {
        if (param.first != COM_CALL_OBJ && param.first != COM_CALL_FUNC) {
            automaticArguments[param.first] = param.second;
        }
    }

    int noOfRows = ui->tagTableWidget->rowCount();
    for (int i = 0; i < ui->tagTableWidget->rowCount(); ++i) {
        QTableWidgetItem* pVarItem = NULL;
        pVarItem = ui->tagTableWidget->item(i, 0);
        auto text = pVarItem->text();
        if (pVarItem != NULL && automaticArguments.contains(pVarItem->text())) {
            QTableWidgetItem* pValueItem = NULL;
            pValueItem = ui->tagTableWidget->item(i, 1);
            if (NULL != pValueItem) {
                pValueItem->setText(automaticArguments.value(pVarItem->text()));
            }
        }
    }

    return true;
}

void GTAActionCallWidget::setComplementFields(const QString& name) {
    _CurrSearchType = (GTAGenSearchWidget::ElementType)getSearchType();
    _pSearchWidget->searchFile(name, _CurrSearchType);
    GTAICDParameter items = _pSearchWidget->getSelectedItems();
    _RefFileUUID = items.getUUID();
    ui->ParamNameLE->setText(items.getName());
    _pSearchWidget->hide();
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CALL,COM_CALL_FUNC),GTAActionCallWidget,obj)

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CALL,COM_CALL_OBJ),GTAActionCallWidget,obj1)

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CALL,COM_CALL_PROC),GTAActionCallWidget,obj2)
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_CALL,COM_CALL_MON),GTAActionCallWidget,obj3)
