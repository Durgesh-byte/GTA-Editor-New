#include "GTAGenSearchWidget.h"
#include "ui_GTAGenSearchWidget.h"
#include "GTAICDParameter.h"
#include "GTAChannelSelectorWidget.h"
#include "GTAFWCData.h"
#include "GTAUtil.h"
#include <qsqlrecord.h>
#include <QMessageBox>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include "QSqlDatabase"
#include "qdebug.h"
#include "QSqlError"
#include "QSqlQuery"
#include <QShortcut>
#include <QKeySequence>
#include <QTime>

GTAGenSearchWidget::GTAGenSearchWidget(QWidget *parent) :


    QWidget(parent),
    ui(new Ui::GTAGenSearchWidget)

{

    _pAppCtrl = GTAAppController::getGTAAppController();
    ui->setupUi(this);
    initMembers();
    _FilterModel = NULL;
    _SqlQueryModel = NULL;


    QShortcut *SelectAllShortcut = new QShortcut(QKeySequence::SelectAll,this); //check if QKeySequence::SelectAll works if not use next line of code
    //    QShortcut *SelectAllShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A),this);
    connect(SelectAllShortcut, SIGNAL(activated()),this, SLOT(onSelectAllPressed()));

    //    ui->SearchResultTW->setSelectionMode(QTableView::SingleSelection);
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKPBClicked()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(onCancelClicked()));
    //    connect(ui->AircraftCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onProgramChange(QString)));
    //    connect(ui->EquipmentCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onEquipmentChange(QString)));
    //    connect(ui->StandardCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onStandardChange(QString)));
    connect(ui->SearchResultTW,SIGNAL(itemSelectionChanged()),this,SLOT(onSelectionTableSelectionChange()));
    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(searchFunction()));
    connect(ui->DBnextPage,SIGNAL(clicked()),this,SLOT(searchFunction()));
    connect(ui->DBprevPage,SIGNAL(clicked()),this,SLOT(searchFunction()));
    connect(ui->DBSearchLimit,SIGNAL(valueChanged(int)),this,SLOT(showResultPages(int)));
    connect(this,SIGNAL(queryChanged(int)),this,SLOT(showResultPages(int)));
    connect(ui->FilterStr1LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
    connect(ui->FilterStr1LE,SIGNAL(returnPressed()),this,SLOT(searchFunction()));
    connect(ui->FilterStr2LE,SIGNAL(returnPressed()),this,SLOT(searchFunction()));
    connect(ui->FilterStr2LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
    connect(ui->SearchOpt5Chk,SIGNAL(clicked(bool)),this,SLOT(getParamModelForType(bool)));
    connect(ui->CommandParamCB,SIGNAL(clicked(bool)),this,SLOT(showHideActionSelector(bool)));
    connect(ui->CommandParamCoB,SIGNAL(currentIndexChanged(int)),this,SLOT(onSearchInternalVarEnabled(int)));
    connect(ui->SearchResultTW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onOKPBClicked(QModelIndex)));
    connect(ui->SearchOpt3Chk,SIGNAL(toggled(bool)),this,SLOT(disableOtherChecks(bool)));
    connect(ui->chkUserDB,SIGNAL(toggled(bool)),this,SLOT(onUserDefinedDBClicked(bool)));
    connect(ui->userDbCB,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(onUserDBValChanged(const QString &)));
    connect(ui->SearchOpt1Chk,SIGNAL(toggled(bool)),this,SLOT(showDBSearchLimiters(bool)));

    connect(ui->CancelPB,SIGNAL(clicked()),this,SIGNAL(cancelPressed()));

    _lastindex = 0;
    ui->DBPageNumber->setText("0");
    ui->DBprevPage->setDisabled(true);
    ui->DBnextPage->setDisabled(true);

}

GTAGenSearchWidget::~GTAGenSearchWidget()
{
    initializeModels();
    delete ui;
}
void GTAGenSearchWidget::show()
{
    QWidget::show();
}
void GTAGenSearchWidget::initializeModels()
{
    if(_FilterModel != NULL)
    {
        delete _FilterModel;
        _FilterModel = NULL;
    }
}
void GTAGenSearchWidget::excludeItemsFromSearch(QStringList iExcludeItems)
{
    _ExcludedItems.clear();
    _ExcludedItems = iExcludeItems;
}

void GTAGenSearchWidget::getCommandList(ElementType iType)
{
    ui->CommandParamCoB->setVisible(false);
    if (NULL!=_pAppCtrl)
    {
#pragma warning( disable : 4482 )
        GTAUtil::ElementType elemType;
        if(iType == ElementType::PROCEDURE)
            elemType = GTAUtil::PROCEDURE;
        else if(iType == ElementType::FUNCTION)
            elemType = GTAUtil::FUNCTION;
        else if(iType == ElementType::OBJECT)
            elemType = GTAUtil::OBJECT;
        else if(iType == ElementType::SEQUENCE)
            elemType = GTAUtil::SEQUENCE;
        else
            elemType = GTAUtil::PROCEDURE;


        //adding actions
        QHash<QString,QStringList> oCommandList = _pAppCtrl->getActionCommandList(elemType);
        QList<QString> actions = oCommandList.keys();
        QStringList complements;
        QString actioninsert;
        QStringList comboBoxItems;
        comboBoxItems << "--Select--";
        for (int i = 0; i < actions.count(); i++) {
            complements = oCommandList.value(actions.at(i));
            for (int j = 0; j < complements.count(); j++) {
                actioninsert = QString("%1_%2").arg(actions.at(i), complements.at(j)).trimmed().replace(" ", "_");
                if (!actioninsert.isEmpty())
                    comboBoxItems << actioninsert;
            }
            if (complements.isEmpty()) {
                actioninsert = QString("%1").arg(actions.at(i)).trimmed().replace(" ", "_");
                if (!actioninsert.isEmpty())
                    comboBoxItems << actioninsert;
            }
        }

        actions.clear();complements.clear();

        //adding checks
        QList<QString> checkCommandlist;
        checkCommandlist = _pAppCtrl->getCheckCommandList(elemType);
        QString checkinsert;

        for (int i = 0; i < checkCommandlist.count(); i++) {
            checkinsert = QString("check %1").arg(checkCommandlist.at(i)).trimmed().replace(" ", "_");
            if (!checkinsert.isEmpty())
                comboBoxItems << checkinsert;
        }

        comboBoxItems.sort(Qt::CaseInsensitive);
        ui->CommandParamCoB->clear();
        ui->CommandParamCoB->addItems(comboBoxItems);
    }
}

void GTAGenSearchWidget::initMembers()
{

    //    GTAAppController * pController = GTAAppController::getGTAAppController();

    //    if(pController != NULL)
    //    {
    //        pController->getProgramList(_ProgramList);
    //        foreach(QString program, _ProgramList)
    //        {
    //            QStringList equipmentList;
    //            pController->getEquipmentList(equipmentList,program);
    //            _EquipmentList.insert(program,equipmentList);
    //            foreach(QString equipment, equipmentList)
    //            {
    //                QStringList standardList;
    //                pController->getStandardList(standardList,program,equipment);
    //                _StandardList.insert(equipment,standardList);
    //            }
    //        }
    //    }

    //    ui->AircraftCB->addItems(_ProgramList);
    //    if(_ProgramList.count()>0)
    //    {
    //        ui->AircraftCB->setCurrentIndex(0);
    //        QString currentProg = ui->AircraftCB->currentText();
    //        //onProgramChange(currentProg);
    //    };
}
void GTAGenSearchWidget::onProgramChange(const QString & /*iSelectedProgram*/)
{
    //    ui->EquipmentCB->clear();
    //    ui->StandardCB->clear();

    //    if(_EquipmentList.contains(iSelectedProgram))
    //    {
    //        QStringList equipList = _EquipmentList.value(iSelectedProgram);
    //        ui->EquipmentCB->addItems(equipList);
    //        //ui->EquipmentCB->setCurrentIndex(0);
    //        QString currentEquip = ui->EquipmentCB->currentText();
    //        //onEquipmentChange(currentEquip);
    //    }
}

void GTAGenSearchWidget::onEquipmentChange(const QString & /*iSelectedEquip*/)
{
    //    ui->StandardCB->clear();
    //    if(_StandardList.contains(iSelectedEquip))
    //    {
    //        QStringList stdList = _StandardList.value(iSelectedEquip);
    //        ui->StandardCB->addItems(stdList);
    //        //ui->StandardCB->setCurrentIndex(0);
    //        QString currentStd = ui->StandardCB->currentText();
    //        // onStandardChange(currentStd);
    //    }
}

void GTAGenSearchWidget::onStandardChange(const QString & /*iSelectedStd*/)
{
    //    QString program = ui->AircraftCB->currentText();
    //    QString equipment= ui->EquipmentCB->currentText();
    //    QString standard = iSelectedStd;
    //    if(! program.isEmpty() && (! equipment.isEmpty()) && (! standard.isEmpty()))
    //    {
    //        GTAAppController * pController = GTAAppController::getGTAAppController();
    //        switch(_CurrentSearchType)
    //        {
    //        case OBJECT:
    //            break;
    //        case FUNCTION: break;
    //        case PROCEDURE: break;
    //        case PARAMETER :
    //            break;
    //        case WARNING:
    //            break;
    //        case FWC:
    //            //setSource(pController->getMessageList(GTAMessage::DISPLAY),FWC);
    //            break;
    //        case BITE:
    //            //setSource(pController->getMessageList(GTAMessage::BITEMSG),BITE);
    //            break;
    //        case IMAGE:
    //            QHash<QString,QString> imageList = pController->getImageList(program,equipment,standard);
    //            setSource(imageList);
    //            break;
    //        }
    //    }
}

//********************************************************************************************
// Search Functionality -- START
//********************************************************************************************

GTAFilterModel * GTAGenSearchWidget::getSearchItems(const QString iExtn ,bool ignoreUsedByDocs,bool isResultView)
{
    QStringList items;
    QHash<QString, QString> docMap;
    if(_pAppCtrl != NULL)
    {
        /*QString sEquipment = _pAppCtrl->getEquipment();
        QString sProgram = _pAppCtrl->getProgram();
        QString sStandard = _pAppCtrl->getStandard();*/
        items = _pAppCtrl->getItemsToDisplay(iExtn,ignoreUsedByDocs,isResultView,docMap);
    }
    return getSearchModel(docMap);
}

GTAFilterModel * GTAGenSearchWidget::getIRTSearchItems()
{
    QStringList items;
    if(_pAppCtrl != NULL)
    {
        /* QString sEquipment = _pAppCtrl->getEquipment();
        QString sProgram = _pAppCtrl->getProgram();
        QString sStandard = _pAppCtrl->getStandard();*/
        items = _pAppCtrl->getIRTItemsToDisplay(/*sProgram,sEquipment,sStandard*/);
    }
    return getSearchModel(items);
}
GTAFilterModel * GTAGenSearchWidget::getRoboArmSearchItems()
{
    QStringList items;
    if(_pAppCtrl != NULL)
    {
        items = _pAppCtrl->getRoboticArmItemsToDisplay();
    }
    return getSearchModel(items);
}
GTAFilterModel * GTAGenSearchWidget::getFwcItems()
{
    QList<GTAFWCData> fwcData;
    if(_pAppCtrl != NULL)
    {
        /*QString sEquipment = _pAppCtrl->getEquipment();
        QString sProgram = _pAppCtrl->getProgram();
        QString sStandard = _pAppCtrl->getStandard();*/
        _pAppCtrl->getFwcData(fwcData/*,sProgram,sEquipment,sStandard*/);
        _StandardItemModel.setRowCount(fwcData.count());
        _StandardItemModel.setColumnCount(3);
        for(int itemIdx=0;itemIdx<fwcData.count();itemIdx++)
        {
            QString header = fwcData.at(itemIdx).getHeader();
            QString message = fwcData.at(itemIdx).getMessage();
            QString color = fwcData.at(itemIdx).getFWCColor();
            _StandardItemModel.setItem(itemIdx,0,new QStandardItem(message));
            _StandardItemModel.setItem(itemIdx,1,new QStandardItem(header));
            _StandardItemModel.setItem(itemIdx,2,new QStandardItem(color));
        }
        if(_FilterModel != NULL)
        {
            delete _FilterModel;
            _FilterModel = NULL;
        }
        _FilterModel = new GTAFilterModel(&_StandardItemModel);
    }
    return _FilterModel;
}


GTAFilterModel * GTAGenSearchWidget::getAudioRecogSearchItems()
{
    QStringList items = _pAppCtrl->getAudioRecogItemsToDisplay();
    return getSearchModel(items);
}

GTAFilterModel* GTAGenSearchWidget::getCmdInternalVarSearchItems()
{
    QList<CommandInternalsReturnInfo> retInfoList;

    QString cmdName = ui->CommandParamCoB->currentText();
    _pAppCtrl->getCommandList(retInfoList,cmdName);

    if(!retInfoList.isEmpty())
    {
        _StandardItemModel.setRowCount(retInfoList.count());
        _StandardItemModel.setColumnCount(4);
        for(int i=0;i<retInfoList.count();i++)
        {
            CommandInternalsReturnInfo retInfo = retInfoList.at(i);
            QString paramName = retInfo.retName;
            QString paramType = retInfo.retType;
            //            QString location = QString("%1:%2:%3:%4").arg("GEN_TOOL",retInfo.toolName.trimmed(),retInfo.funcName.trimmed(),retInfo.location.trimmed());
            //            location = location.replace(" ","_");

            QString location = QString("%1:%2").arg(retInfo.cmdName,retInfo.refLoc.trimmed());
            location.replace(" ","_");

            QString description = retInfo.desc;
            _StandardItemModel.setItem(i,0,new QStandardItem(paramName));
            _StandardItemModel.setItem(i,1,new QStandardItem(paramType));
            _StandardItemModel.setItem(i,2,new QStandardItem(location));
            _StandardItemModel.setItem(i,3,new QStandardItem(retInfo.location));
            _StandardItemModel.setItem(i,4,new QStandardItem(retInfo.funcName));
            _StandardItemModel.setItem(i,5,new QStandardItem(description));
        }

        if(_FilterModel != NULL)
        {
            delete _FilterModel;
            _FilterModel = NULL;
        }
        _FilterModel = new GTAFilterModel(&_StandardItemModel);
        _FilterModel->setHeaderData(0,Qt::Horizontal,"Parameter Name",Qt::DisplayRole);
        _FilterModel->setHeaderData(1,Qt::Horizontal,"Parameter Type",Qt::DisplayRole);
        _FilterModel->setHeaderData(2,Qt::Horizontal,"Parameter Location",Qt::DisplayRole);
        _FilterModel->setHeaderData(3,Qt::Horizontal,"UUID",Qt::DisplayRole);
        _FilterModel->setHeaderData(4,Qt::Horizontal,"Function Name",Qt::DisplayRole);
        _FilterModel->setHeaderData(5,Qt::Horizontal,"Parameter Description",Qt::DisplayRole);
    }
    else
    {
        _StandardItemModel.setRowCount(retInfoList.count());
        _StandardItemModel.setColumnCount(0);
        if(_FilterModel != NULL)
        {
            delete _FilterModel;
            _FilterModel = NULL;
        }
        _FilterModel = new GTAFilterModel(&_StandardItemModel);
        //return _FilterModel;
    }
    return _FilterModel;
}


GTAFilterModel * GTAGenSearchWidget::getReleaseVariables()
{
    QStringList items = _pAppCtrl->getVariablesForRelease();

    items.removeDuplicates();
    foreach(QString itemVal ,_ExcludedItems)
    {
        if(items.contains(itemVal))
        {
            int idx = items.indexOf(itemVal,0);
            if(idx>-1)
                items.removeAt(idx);
        }
    }
    return getSearchModel(items);
}

GTAFilterModel * GTAGenSearchWidget::getOneClickSearchItems()
{
    QStringList items;
    if(_pAppCtrl!=NULL)
    {
        /* QString sEquipment = _pAppCtrl->getEquipment();
        QString sProgram = _pAppCtrl->getProgram();
        QString sStandard = _pAppCtrl->getStandard();*/
        items = _pAppCtrl->getOneClickItemsToDisplay(/*sProgram,sEquipment,sStandard*/);
    }
    return getSearchModel(items);
}

GTAFilterModel * GTAGenSearchWidget::getOneClickLaunchAppSearchItems()
{
    QStringList items;
    if(_pAppCtrl!=NULL)
    {
        /* QString sEquipment = _pAppCtrl->getEquipment();
        QString sProgram = _pAppCtrl->getProgram();
        QString sStandard = _pAppCtrl->getStandard();*/
        items = _pAppCtrl->getOneClickLauncAppItemsToDisplay(/*sProgram,sEquipment,sStandard*/);
    }
    return getSearchModel(items);
}
GTAFilterModel * GTAGenSearchWidget::getPrintTables()
{
    QStringList items;
    if(_pAppCtrl!=NULL)
    {

        items = _pAppCtrl->getPrintTables();
    }
    return getSearchModel(items);
}
GTAFilterModel * GTAGenSearchWidget::getFailureCommands()
{
    QStringList items;
    if(_pAppCtrl!=NULL)
    {

        items = _pAppCtrl->getFailureCommandNames();
    }
    return getSearchModel(items);
}
GTAFilterModel * GTAGenSearchWidget::getLocalVarModel(bool isSubscribeLocal)
{
    QStringList localVars = _pAppCtrl->getLocalVariables(true,isSubscribeLocal, _knownVars);
    return getSearchModel(localVars);
}
GTAFilterModel * GTAGenSearchWidget::getParamModel(QString Query)
{
    QTime t;
    qDebug()<<"Starting the DB fetching: "<<t.currentTime();
    t.start();

    QSqlDatabase database = _pAppCtrl->getLocalDB(/*sProgram,sEquipment,sStandard*/);
    if (database.isOpen())
    {
        QSqlQuery speedQuery(database);
        emit changeLabel(-1,"Executing Query",false);
        

        qDebug() << "Time = " << QTime::currentTime();
        speedQuery.setForwardOnly(true);

        bool sendquery = speedQuery.exec(Query);
        emit changeLabel(-1,"Fetching Results",false);
        qDebug()<<"Query executed? "<<QTime::currentTime();

        int i = 0;
        qDebug()<<"Rows affected: "<<speedQuery.numRowsAffected();

        //range currenlty mentioned in the spinbox
        int range;
        if (Query.contains("LIMIT"))
        {
            QString num = Query.split("LIMIT").last();
            num.trimmed();
            if (num.contains(","))
            {
                range = num.split(",").last().toInt();
            }
            else
            {
                range = num.trimmed().toInt();
            }
        }

        while(speedQuery.next() && i<500)
        {
            i++;
            emit changeLabel(i,"",false);
        }
        speedQuery.finish();
        if (i < range)
        {
            emit changeLabel(-1,"",true);
        }

        qDebug()<<"Query executed? "<<sendquery;
        qDebug() << "Number of rows returned: " << speedQuery.size();
        qDebug() << "Calculated rows: " << i;
        qDebug() << "Query is active: " << speedQuery.isActive();
        qDebug() << "Time = " << QTime::currentTime();
    }


    if(_pAppCtrl != NULL)
    {
        //QString sEquipment = _pAppCtrl->getEquipment();
        //QString sProgram = _pAppCtrl->getProgram();
        //QString sStandard = _pAppCtrl->getStandard();
        QSqlDatabase database = _pAppCtrl->getLocalDB(/*sProgram,sEquipment,sStandard*/);
        if(_SqlQueryModel != NULL)
        {
            delete _SqlQueryModel;
            _SqlQueryModel = NULL;
        }
        _SqlQueryModel = new QSqlQueryModel();
        if(database.isOpen() && (database.tables().count()>0))
        {
            _SqlQueryModel->setQuery(Query,database);
        }
        if (_SqlQueryModel->lastError().isValid())
        {
            qDebug()<< "Error in DB: "<<_SqlQueryModel->lastError()<<" at time: "<<QTime::currentTime();
            emit changeLabel(-1,QString("Error: %1").arg(_SqlQueryModel->lastError().text()),false);
        }

        qDebug()<<"Ending the DB fetching and start filling filter model: "<<QTime::currentTime();
        _FilterModel = new GTAFilterModel(_SqlQueryModel);
        qDebug()<<"End filling filter model: "<<QTime::currentTime();
        int msecsnow = t.elapsed();
        emit changeLabel(-1,QString("Results fetched in %1 min %2 sec").arg(QString::number(msecsnow/60000),QString::number(msecsnow%60000/1000)),false);
    }
    return _FilterModel;
}

GTAFilterModel * GTAGenSearchWidget::getSearchModel(QStringList items)
{
    items.removeDuplicates();
    _StandardItemModel.setRowCount(items.count());
    _StandardItemModel.setColumnCount(1);
    for(int itemIdx=0;itemIdx<items.count();itemIdx++)
    {
        QString colItem = items.at(itemIdx).trimmed();
        if(!colItem.isEmpty() && !colItem.isNull())
        {

            _StandardItemModel.setItem(itemIdx,0,new QStandardItem(items.at(itemIdx)));
        }
    }
    if(_FilterModel != NULL)
    {
        delete _FilterModel;
        _FilterModel = NULL;
    }
    _FilterModel = new GTAFilterModel(&_StandardItemModel);
    return _FilterModel;
}

GTAFilterModel * GTAGenSearchWidget::getSearchModel(QHash<QString, QString> items)
{
    _StandardItemModel.setRowCount(items.keys().count());
    _StandardItemModel.setColumnCount(2);
    QStringList keys = items.keys();
    for(int itemIdx=0;itemIdx<keys.count();itemIdx++)
    {
        QString colItem = keys.at(itemIdx).trimmed();
        if(!colItem.isEmpty() && !colItem.isNull())
        {
            _StandardItemModel.setItem(itemIdx,0,new QStandardItem(colItem));
            _StandardItemModel.setItem(itemIdx,1,new QStandardItem(items.value(colItem)));
        }
    }
    if(_FilterModel != NULL)
    {
        delete _FilterModel;
        _FilterModel = NULL;
    }
    _FilterModel = new GTAFilterModel(&_StandardItemModel);
    return _FilterModel;
}

/* This function allow to obtain the FileType from the FilePath of a Parameter
* @input: iFileType - QString from a ComboBox
* @output: oFileType - QString containing the FileType to search
* @return: none
*/
void GTAGenSearchWidget::getFileFromFileType(const QString& iFileType, QString & oFileType) const
{
	if (iFileType == "PMR")
	{
		oFileType = "PMR";
		return;
	}
	if (iFileType == "PIR")
	{
		oFileType = "PIR";
		return;
	}
	if (iFileType == "ICD")
	{
		oFileType = "AC_ICD";
		return;
	}
	if (iFileType == "MODEL_ICD")
	{
		oFileType = "MODEL_ICD";
		return;
	}
	if (iFileType == "VCOM_STATIC")
	{
		oFileType = "VCOM";
		return;
	}
}

/* This function allow to obtain the SignalType from the Media of a Parameter
* @input: iMedia - QString from a ComboBox
* @output: oSignalType - QString containing the SignalType to search
* @return: none
*/
void GTAGenSearchWidget::getSignalTypeFromMedia(const QString& iMedia, QString & oSignalType) const
{
	if (iMedia == "AFDX")
	{
		oSignalType = "NA";
		return;
	}
	if (iMedia == "A429")
	{
		oSignalType = "ARINC";
		return;
	}
	if (iMedia == "CAN")
	{
		oSignalType = "CAN";
		return;
	}
	if (iMedia == "ANA")
	{
		oSignalType = "ANALOGUE";
		return;
	}
	if (iMedia == "DIS")
	{
		oSignalType = "DISCRETE";
		return;
	}
	if (iMedia == "VMAC")
	{
		oSignalType = "";
		return;
	}
	if (iMedia == "PIR")
	{
		oSignalType = "ARINC";
		return;
	}
	if (iMedia == "VCOM")
	{
		oSignalType = "VCOM";
		return;
	}
	if (iMedia == "MODEL")
	{
		oSignalType = "MODEL";
		return;
	}
}

void GTAGenSearchWidget::searchFunction()
{
    QString filterText1 = ui->FilterStr1LE->text();
    QString filterText2 = ui->FilterStr2LE->text();
    QString filterCriteria = ui->FilterCriterionCB->currentText();
    QStringList texts;
    filterText1 = filterText1.trimmed();
    filterText2 = filterText2.trimmed();

    //failsafe to check query doesn't fail
    filterText1 = filterText1.replace("'","");
    filterText2 = filterText2.replace("'","");

    if((filterText1 != "") && (filterText2 != ""))
    {
        texts <<filterText1 << filterText2;
        if(filterCriteria.toLower().contains("not"))
        {
            filterCriteria = "NOT";
        }
        else
        {
            filterCriteria = filterCriteria.toUpper();
        }
    }
    else
    {
        if((filterText1 != "") && (filterText2 == ""))
        {
            texts<<filterText1;
        }
        else if((filterText1 == "") && (filterText2 != ""))
        {
            texts<<filterText2;
        }
        else if((filterText1 == "") && (filterText2 == ""))
        {
            return;
        }

    }
    if((_CurrentSearchType != INIT_PIR_SEARCH ) &&(_CurrentSearchType != PARAMETER ) && (_CurrentSearchType != GEN_PARAMETER) && (_CurrentSearchType != DUMP_LIST)  && (_CurrentSearchType != RELEASE))
    {
        if(_FilterModel == NULL)
            return;
        QList<int> _cols;
        _cols<<0;
        if(_CurrentSearchType == kWarning)
            _cols<<1;

        QRegExp regExp("",Qt::CaseInsensitive,QRegExp::RegExp);
        _cols<<0;
        _FilterModel->setSearchcolumns(_cols);
        _FilterModel->setSearchStrings(texts);
        _FilterModel->setSearchCriteria(filterCriteria);
        _FilterModel->setFilterRegExp(regExp);


        if(_CurrentSearchType == kWarning)
        {
            //ui->SearchResultTW->resizeColumnsToContents();
            ui->SearchResultTW->horizontalHeader()->stretchLastSection();
        }
        else
            ui->SearchResultTW->horizontalHeader()->stretchLastSection();
    }
    else
    {
        QRegExp regExp("",Qt::CaseInsensitive,QRegExp::RegExp);
        QList<int> _cols;
        bool twoLevelSearch = false;
        if(_SubSearchType == PARAMETER_SEARCH || _SubSearchType == LIVEMODE_PARAMETER_SEARCH)
        {
            QStringList SearchFilter1;
            QStringList search;
            QStringList fileMediaSearch;
            //ui->SearchOpt1Chk->hide();
            ui->SearchOpt2Chk->hide();
            ui->SearchOpt3Chk->setText("Signal Name/Parameter Local Name");
            ui->SearchOpt4Chk->setText("Label");
            ui->SearchOpt5Chk->setText("Local Variables");
            ui->SearchOpt6Chk->setText("Application Name");
            ui->SearchOpt7Chk->setText("Bus Name");
            ui->SearchOpt8Chk->setText("File Type");
            ui->SearchOpt9Chk->setText("Media Type");
            ui->SearchOpt10Chk->setText("FS/SSM");
            ui->SearchOpt11Chk->setText("File Name");
            ui->DirectionChk->setText("Direction");

            filterText1 = filterText1.toLower();
            filterText2 = filterText2.toLower();
            if(ui->SearchOpt3Chk->isChecked())
            {
                //_cols<<0<<19;
                if (!filterText1.isEmpty())
                    SearchFilter1.append("(NAME LIKE '%" + filterText1 + "%' OR PARAMETER_LOCAL_NAME LIKE '%" + filterText1 + "%')");
                if (!filterText2.isEmpty())
                    SearchFilter1.append("(NAME LIKE '%" + filterText2 + "%' OR PARAMETER_LOCAL_NAME LIKE '%" + filterText2 + "%')");
                search.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();
            }
            if(ui->SearchOpt4Chk->isChecked())
            {
                if(!filterText1.isEmpty())
                    SearchFilter1.append("LABEL LIKE '%"+filterText1+"%'");
                if(!filterText2.isEmpty())
                    SearchFilter1.append("LABEL LIKE '%"+filterText2+"%'");
                //_cols<<7;
                search.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();
            }
            if(ui->SearchOpt6Chk->isChecked())
            {
                //_cols<<14;
                if(!filterText1.isEmpty())
                    SearchFilter1.append("NAME LIKE '%" + filterText1 + "%.%.%'"); //("APPLICATION LIKE '%"+filterText1+"%'");
                if(!filterText2.isEmpty())
                    SearchFilter1.append("NAME LIKE '%" + filterText1 + "%.%.%'"); //("APPLICATION LIKE '%"+filterText2+"%'");
                search.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();

            }
            if(ui->SearchOpt7Chk->isChecked())
            {
                //_cols<<15;
                if(!filterText1.isEmpty())
                    SearchFilter1.append("NAME LIKE '%.%" + filterText1 + "%.%'"); //("BUSNAME  LIKE '%"+filterText1+"%'");
                if(!filterText2.isEmpty())
                    SearchFilter1.append("NAME LIKE '%.%" + filterText1 + "%.%'"); //("BUSNAME LIKE '%"+filterText2+"%'");
                search.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();
            }
            if(ui->SearchOpt8Chk->isChecked())
            {
                //_cols<<13;
                twoLevelSearch = true;
                QString fileType = ui->SearchFileTypeCB->currentText();
				QString oFileType = QString();
				getFileFromFileType(fileType, oFileType);
                
                SearchFilter1.append("FILE LIKE '/"+oFileType+"%/%'");

                fileMediaSearch.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();
            }
            if(ui->SearchOpt9Chk->isChecked())
            {
                //_cols<<17;
                twoLevelSearch = true;
                QString mediaType = ui->SearchMediaTypeCB->currentText();
				QString oSignalType = QString();
				getSignalTypeFromMedia(mediaType, oSignalType);
                if (mediaType == "PIR")
                    SearchFilter1.append("PARAMTYPE LIKE 'FIB-IPR'");
                else
                    SearchFilter1.append("SIGNALTYPE LIKE '%" + oSignalType + "%'");

                fileMediaSearch.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();

            }
            if(ui->SearchOpt10Chk->isChecked())
            {
                if(!filterText1.isEmpty())
                    SearchFilter1.append("FS_NAME LIKE '%"+filterText1+"%'");
                if(!filterText2.isEmpty())
                    SearchFilter1.append("FS_NAME LIKE '%"+filterText2+"%'");
                search.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();

            }
            if(ui->SearchOpt11Chk->isChecked())
            {
                if(!filterText1.isEmpty())
                    SearchFilter1.append("FILE LIKE '%"+filterText1+"%'");
                if(!filterText2.isEmpty())
                    SearchFilter1.append("FILE LIKE '%"+filterText2+"%'");
                search.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();

            }
            if(ui->DirectionChk->isChecked())
            {

                twoLevelSearch = true;
                QString direction = ui->DirectionCB->currentText();
                if(direction != "All")
                {
                    SearchFilter1.append("DIRECTION LIKE '%"+direction+"%'");
                    fileMediaSearch.append(SearchFilter1.join(" "+filterCriteria+" "));
                    SearchFilter1.clear();
                }

            }

            if((search.count() <1))
            {
                ui->SearchOpt3Chk->setChecked(true);
                if(!filterText1.isEmpty())
					SearchFilter1.append("(NAME LIKE '%" + filterText1 + "%') OR PARAMETER_LOCAL_NAME LIKE '%" + filterText1 + "%')");
                if(!filterText2.isEmpty())
					SearchFilter1.append("(NAME LIKE '%" + filterText2 + "%') OR PARAMETER_LOCAL_NAME LIKE '%" + filterText2 + "%')");
                search.append(SearchFilter1.join(" "+filterCriteria+" "));
                SearchFilter1.clear();
            }
            QStringList fileMediaStrList;

            if(!fileMediaSearch.isEmpty())
                fileMediaStrList.append(fileMediaSearch.join(" AND "));
            if(!search.isEmpty())
                fileMediaStrList.append(QString("(%1)").arg(search.join(" OR ")));

            QString WhereClause = fileMediaStrList.join(" AND ");
            QString Query = QString("SELECT * FROM %1 WHERE %2").arg(PARAMETER_TABLE,WhereClause);
            //QString Query = QString("SELECT * FROM %1 WITH (NOLOCK) WHERE %2").arg(PARAMETER_TABLE,WhereClause);

            if (ui->SearchOpt1Chk->isVisible() && ui->SearchOpt1Chk->isEnabled() && ui->SearchOpt1Chk->isChecked())
            {
                int limit = ui->DBSearchLimit->value();
                QToolButton *pushedButton = dynamic_cast<QToolButton*>(sender());
                QString limitClause = QString(" LIMIT %1").arg(QString::number(limit));

                if (pushedButton != NULL)
                {
                    if (pushedButton == ui->DBnextPage)
                    {
                        if (_lastindex == limit)
                        {
                            //corner case: when the search button was pressed first and then next page was pressed
                            //corner case: when a search was done but search limit was changed. the _lastindex becomes zero. use the _current index as 0 then and do not increase the index
                            _lastindex = limit;
                            limitClause = QString(" LIMIT %1,%2").arg(QString::number(_lastindex),QString::number(limit));
                            _lastindex = _lastindex + limit;
                        }
                        else
                        {
                            limitClause = QString(" LIMIT %1,%2").arg(QString::number(_lastindex),QString::number(limit));
                            _lastindex = _lastindex + limit;
                        }
                        ui->DBprevPage->setEnabled(true);
                    }
                    else if (pushedButton == ui->DBprevPage)
                    {
                        _lastindex = ((_lastindex - 2*limit) <= 0) ? 0 : (_lastindex - 2*limit);
                        limitClause = QString(" LIMIT %1,%2").arg(QString::number(_lastindex),QString::number(limit));
                        if (_lastindex <= 0)
                        {
                            ui->DBprevPage->setDisabled(true);
                            ui->DBnextPage->setEnabled(true);
                        }
                        else
                            ui->DBprevPage->setEnabled(true);
                    }

                    //when new search is requested from previous or when no results are loaded previously in the Table
                    if (NULL != _FilterModel)
                    {
                        if ((_lastLikeQuery != Query) || _FilterModel->rowCount() == 0)
                        {
                            emit queryChanged(ui->DBSearchLimit->value());
                            limitClause = QString(" LIMIT %1,%2").arg(QString::number(_lastindex),QString::number(limit));
                            _lastLikeQuery = Query;
                        }
                    }
                }
                else
                {
                    //when new search is requested from previous or when no results are loaded previously in the Table
                    if (NULL != _FilterModel)
                    {
                        if ((_lastLikeQuery != Query) || (_FilterModel->rowCount() == 0))
                        {
                            emit queryChanged(ui->DBSearchLimit->value());
                            limitClause = QString(" LIMIT %1,%2").arg(QString::number(_lastindex),QString::number(limit));
                            _lastLikeQuery = Query;
                        }

                        _lastindex = limit;
                    }
                }
                ui->DBnextPage->setEnabled(true);
                Query.append(limitClause);

                if(_FilterModel != NULL)
                {
                    delete _FilterModel;
                    _FilterModel = NULL;
                }

                if (NULL != _SqlQueryModel)
                {
                    delete _SqlQueryModel;
                    _SqlQueryModel = NULL;
                }

                 enablePageSearchOptions(false);


                connect(this,SIGNAL(changeLabel(int,QString,bool)),this,SLOT(onLabelChange(int,QString,bool)));
                ui->DBprogressBar->setMaximum(ui->DBSearchLimit->value() + 1);
                ui->DBprogressBar->setMinimum(0);
                ui->OKPB->setEnabled(false);
                ui->CancelPB->setEnabled(false);
                QFutureWatcher<GTAFilterModel*> * FutureWatcher = new QFutureWatcher<GTAFilterModel*>();
                connect(FutureWatcher,SIGNAL(finished()),this,SLOT(QueryProcessing()));
                QFuture<GTAFilterModel*> Future = QtConcurrent::run(this,&GTAGenSearchWidget::getParamModel,Query);
                FutureWatcher->setFuture(Future);


                /*_FilterModel = getParamModel(Query);
                QueryProcessing();*/


            }
            else
            {
                //@SK
                //do not use a future watcher for non paged results.
                //force the ui to get stuck
                //users have to start using page based approach
                //so evil. #mojoJoJo
                //@JK - SK this is too much
                _FilterModel = getParamModel(Query);
                ui->SearchResultTW->setModel(_FilterModel);
                ui->SearchResultTW->resizeColumnsToContents();
            }


            //            _FilterModel = getParamModel(Query);
            //            enablePageSearchOptions(true);
            //            ui->SearchResultTW->setModel(_FilterModel);
            //            ui->SearchResultTW->resizeColumnsToContents();
        }
        else
        {
            _cols<<0;
            if(_FilterModel == NULL)
                return;
            _FilterModel->setSearchcolumns(_cols);
            _FilterModel->setSearchStrings(texts);
            _FilterModel->setSearchCriteria(filterCriteria);
            _FilterModel->setFilterRegExp(regExp);
            ui->SearchResultTW->horizontalHeader()->stretchLastSection();
        }
    }

}

/**
      * This slot resets the DB load time label, page number, lastindex
      */
void GTAGenSearchWidget::showResultPages(int /*iVal*/)
{
    _lastindex = 0;
    //    ui->DBPageNumber->setText("0");
    ui->DBLoadTime->setText("Result: NA");
    ui->DBprevPage->setDisabled(true);
    ui->DBnextPage->setDisabled(true);
}

/**
      * This slot toggles DB page number, progress bar, next page icon if DB page based search is enabled
      */
void GTAGenSearchWidget::onLabelChange(int iVal,QString iMessage,bool iVisibility)
{
    if (ui->SearchOpt1Chk->isChecked())
    {
        if (iVal != -1)
            ui->DBprogressBar->setValue(iVal);
        if (!iMessage.isEmpty())
            ui->DBLoadTime->setText(iMessage);
        if (iVisibility)
            ui->DBnextPage->setDisabled(iVisibility);
    }
}

/**
      * This slot fills the filter model with search results. Should be connected with a future watcher only.
      */
void GTAGenSearchWidget::QueryProcessing()
{
    QFutureWatcher<GTAFilterModel*> *pWatcher = dynamic_cast<QFutureWatcher<GTAFilterModel*> *>(sender());
    if (pWatcher == NULL)
        return;
    else
    {
        ui->DBPageNumber->setText(QString("%1").arg(QString::number(_lastindex/(ui->DBSearchLimit->value() == 0 ? 1 : ui->DBSearchLimit->value()))));
        ui->DBprogressBar->setValue(ui->DBSearchLimit->value() + 1);
        _FilterModel = pWatcher->result();
        enablePageSearchOptions(true);
        ui->SearchResultTW->setModel(_FilterModel);
        ui->SearchResultTW->resizeColumnsToContents();
        ui->OKPB->setEnabled(true);
        ui->CancelPB->setEnabled(true);
    }
}

/**
      * This function enables/disables the Page based DB search options to prevent user input during a query search being processed
      * @iVal: boolean input to toggle visibility of ui components
      */
void GTAGenSearchWidget::enablePageSearchOptions(bool iVal)
{
    ui->DBnextPage->setVisible(iVal);
    ui->DBprevPage->setVisible(iVal);
    ui->SearchPB->setEnabled(iVal);
    ui->FilterStr1LE->setEnabled(iVal);
    ui->FilterStr2LE->setEnabled(iVal);
    ui->DBLoadTime->setEnabled(iVal);
}

void GTAGenSearchWidget::clearSearch()
{
    if(_FilterModel == NULL)
        return;
    QString filterText1 = ui->FilterStr1LE->text();
    QString filterText2 = ui->FilterStr2LE->text();
    filterText1 = filterText1.trimmed();
    filterText2 = filterText2.trimmed();
    if((filterText1 == "") && (filterText2 == ""))
    {
        QRegExp regExp("",Qt::CaseInsensitive,QRegExp::RegExp);
        QList<int> _cols;
        _cols<<0;
        QStringList texts;
        texts<<"";
        _FilterModel->setSearchcolumns(_cols);
        _FilterModel->setSearchStrings(texts);
        _FilterModel->setSearchCriteria("");
        _FilterModel->setFilterRegExp(regExp);
        if(((_CurrentSearchType == PARAMETER) || (_CurrentSearchType == GEN_PARAMETER)) && (_SubSearchType == PARAMETER_SEARCH || _SubSearchType == LIVEMODE_PARAMETER_SEARCH))
        {
            ui->SearchResultTW->setModel(_FilterModel);
        }
    }
    else
    {
        //to keep query being pushed to network DB, check is made to see if query data set limit is needed
        if((ui->FilterStr1LE == sender()) && (filterText1 == "") && !ui->SearchOpt1Chk->isChecked())
        {
            searchFunction();
        }
        else if((ui->FilterStr2LE == sender()) && (filterText2 == "") && !ui->SearchOpt1Chk->isChecked())
        {
            searchFunction();
        }
    }
}

/*
 *
 */
void GTAGenSearchWidget::onActionChange()
{
	ui->SearchOptGB->setEnabled(true);
	ui->SearchOpt1Chk->setEnabled(true);
	ui->SearchOpt2Chk->setEnabled(true);
	ui->SearchOpt3Chk->setEnabled(true);
	ui->SearchOpt4Chk->setEnabled(true);
	ui->SearchOpt5Chk->setEnabled(true);
	ui->SearchOpt6Chk->setEnabled(true);
	ui->SearchOpt7Chk->setEnabled(true);
	ui->SearchOpt8Chk->setEnabled(true);
	ui->SearchOpt9Chk->setEnabled(true);
	ui->SearchOpt10Chk->setEnabled(true);
	ui->SearchOpt11Chk->setEnabled(true);
	ui->DirectionChk->setEnabled(true);
	ui->DirectionCB->setEnabled(true);
	ui->SearchFileTypeCB->setEnabled(true);
	ui->SearchMediaTypeCB->setEnabled(true);
	ui->chkUserDB->setEnabled(true);
	ui->userDbCB->setEnabled(true);

	ui->CommandParamCB->show();
	ui->CommandParamCB->setEnabled(true);
}

//********************************************************************************************
// Search Functionality -- END
//********************************************************************************************

void GTAGenSearchWidget::setSearchType(ElementType iSearchType,SubSearchType iSubSearchType)
{
    setCurrentConfiguration();

    QStringList mediatype;
    QStringList filetype;

    _CurrentSearchType = iSearchType;
    getCommandList(iSearchType);

    _SubSearchType = iSubSearchType;
    ui->SearchOptGB->setEnabled(true);
    ui->SearchOptGB->hide();
    ui->CommandParamCB->hide();

    ui->SearchOpt1Chk->setChecked(false);
    ui->SearchOpt2Chk->show();
    ui->SearchOpt3Chk->show();
    ui->SearchOpt4Chk->show();
    ui->SearchOpt5Chk->show();
    ui->SearchOpt6Chk->show();
    ui->SearchOpt7Chk->show();
    ui->SearchOpt8Chk->show();
    ui->SearchOpt9Chk->show();
    ui->SearchOpt10Chk->show();
    ui->SearchOpt11Chk->show();
    ui->DirectionChk->show();
    ui->DirectionCB->show();
    ui->SearchFileTypeCB->show();
    ui->SearchMediaTypeCB->show();
    ui->chkUserDB->show();
    ui->userDbCB->show();

    ui->SearchResultTW->setSelectionMode(QTableView::SingleSelection);

    ui->SearchOptSW->setCurrentIndex(0);
    ui->OKPB->setText("OK");
    ui->SearchResultTW->horizontalHeader()->setStretchLastSection(true);
    ui->PreviewImgPB->hide();
    ui->CommandParamCB->setCheckState(Qt::Unchecked);
    ui->SearchVMACVarChk->setChecked(false);
    ui->SearchVMACVarChk->hide();


    showDBSearchLimiters(false);

    ui->SearchOpt1Chk->setText("Description");
    ui->SearchOpt1Chk->show();

    if(_FilterModel != NULL)
    {
        delete _FilterModel;
        _FilterModel = NULL;
    }
    switch(_CurrentSearchType)
    {
    case OBJECT:
		onActionChange();
		_SubSearchType = INVALID_SEARCH;
        _FilterModel  = getSearchItems("*.obj");
        this->setWindowTitle("Object Selection");
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Object Name",Qt::DisplayRole);
            _FilterModel->setHeaderData(1,Qt::Horizontal,"Id",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case FUNCTION:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Function Selection");
        _FilterModel  = getSearchItems("*.fun");
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Function Name",Qt::DisplayRole);
            _FilterModel->setHeaderData(1,Qt::Horizontal,"Id",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case SEQUENCE:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Sequence Selection");
        _FilterModel  = getSearchItems("*.seq");
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Sequence Name",Qt::DisplayRole);
            _FilterModel->setHeaderData(1,Qt::Horizontal,"Id",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case PROCEDURE:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Procedure Selection");
        _FilterModel  = getSearchItems("*.pro");
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Procedure Name",Qt::DisplayRole);
            _FilterModel->setHeaderData(1,Qt::Horizontal,"Id",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case RESULT_SEQ:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Sequence Selection");
        _FilterModel  = getSearchItems("*.seq",false,true);
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Procedure Name",Qt::DisplayRole);
            _FilterModel->setHeaderData(1,Qt::Horizontal,"Id",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case RESULT_PRO:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Procedure Selection");
        _FilterModel  = getSearchItems("*.pro",false,true);
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Procedure Name",Qt::DisplayRole);
            _FilterModel->setHeaderData(1,Qt::Horizontal,"Id",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case TEMPLATE:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Template Selection");
        _FilterModel  = getSearchItems("*.tmpl");
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Template Name",Qt::DisplayRole);
            _FilterModel->setHeaderData(1,Qt::Horizontal,"Id",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case PRINT_TABLE:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Print table selection");
        _FilterModel  = getPrintTables();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Template Name",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case FAILURE_COMMANDS:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Print table selection");
        _FilterModel  = getFailureCommands();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Template Name",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case GEN_PARAMETER:
    case DUMP_LIST:
    case PARAMETER :
		onActionChange(); 
		if(_SubSearchType == LIVEMODE_PARAMETER_SEARCH)
        {
            ui->SearchResultTW->setSelectionMode(QTableView::ExtendedSelection);
        }
        else
        {
            _SubSearchType = PARAMETER_SEARCH;
        }
        showDBSearchLimiters(true);
        this->setWindowTitle("Parameter Selection");
        setUIForParameterSearch();
        getParamModelForType(_SubSearchType);
        break;

    case INIT_PIR_SEARCH:

        _SubSearchType = PARAMETER_SEARCH;
        this->setWindowTitle("PIR parameter Selection");
        ui->SearchOptGB->show();
        ui->SearchOpt3Chk->show();
        ui->SearchOpt3Chk ->setChecked(true);

        mediatype<< "AFDX"<<"A429"<<"CAN"<<"ANA" <<"DIS" << "VMAC" << "PIR" << "VCOM" << "MODEL";
        filetype <<"PMR" <<"PIR" <<"ICD"<<"MODEL_ICD"<<"VCOM_STATIC";

        ui->SearchFileTypeCB->addItems(filetype);
        ui->SearchFileTypeCB->setCurrentIndex(filetype.indexOf("PIR"));

        ui->SearchMediaTypeCB->addItems(mediatype);
        ui->SearchMediaTypeCB->setCurrentIndex(mediatype.indexOf("PIR"));

        ui->SearchOpt8Chk->show();
        ui->SearchOpt8Chk->setChecked(true);
        ui->SearchOpt9Chk->show();
        ui->SearchOpt9Chk->setChecked(true);

        ui->SearchOptGB->setEnabled(true);
        ui->SearchOpt1Chk->setEnabled(false);
        ui->SearchOpt2Chk->setEnabled(false);
        ui->SearchOpt3Chk->setEnabled(false);
        ui->SearchOpt4Chk->setEnabled(false);
        ui->SearchOpt5Chk->setEnabled(false);
        ui->SearchOpt6Chk->setEnabled(false);
        ui->SearchOpt7Chk->setEnabled(false);
        ui->SearchOpt8Chk->setEnabled(false);
        ui->SearchOpt9Chk->setEnabled(false);
        ui->SearchOpt10Chk->setEnabled(false);
        ui->SearchOpt11Chk->setEnabled(false);
        ui->DirectionChk->setEnabled(false);
        ui->DirectionCB->setEnabled(false);
        ui->SearchFileTypeCB->setEnabled(false);
        ui->SearchMediaTypeCB->setEnabled(false);
        ui->chkUserDB->setEnabled(false);
        ui->userDbCB->setEnabled(false);

        ui->CommandParamCB->show();
        ui->CommandParamCB->setEnabled(true);


        getParamModelForType(_SubSearchType);
        break;

    case RELEASE_PIR_SEARCH:
        _SubSearchType = SUBSCRIBE_LOCAL_SEARCH;
        ui->SearchResultTW->setSelectionMode(QTableView::ExtendedSelection);
        this->setWindowTitle("Subscribed PIR Selection");
        ui->SearchOptGB->setEnabled(false);
        getParamModelForType(_SubSearchType);
        getParamModelForType(GTAGenSearchWidget::SUBSCRIBE_LOCAL_SEARCH);
        break;

    case IRT_OBJECT:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("IRT Object Selection");
        _FilterModel  = getIRTSearchItems();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"IRT Sequence Name",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case ROBO_ARM_OBJECT:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("ROBOTIC Arm Object Selection");
        _FilterModel  = getRoboArmSearchItems();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Robotic arm sequence name",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case kWarning:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Warning Message Selection");
        ui->SearchOptGB->show();
        ui->SearchOpt1Chk->setText("Warning Message");
        ui->SearchOpt2Chk->setText("Label Bit");
        ui->SearchOpt3Chk->hide();
        ui->SearchOpt4Chk->hide();
        ui->SearchOpt5Chk->hide();
        ui->SearchOpt6Chk->hide();
        ui->SearchOpt7Chk->hide();
        ui->SearchOpt8Chk->hide();
        ui->SearchOpt9Chk->hide();
        ui->SearchOpt10Chk->hide();
        ui->SearchOpt11Chk->hide();
        ui->DirectionChk->hide();
        ui->DirectionCB->hide();
        ui->chkUserDB->show();
        ui->userDbCB->show();
        ui->SearchFileTypeCB->hide();
        ui->SearchMediaTypeCB->hide();
        _FilterModel = getFwcItems();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"FWC Message",Qt::DisplayRole);
            _FilterModel->setHeaderData(1,Qt::Horizontal,"FWC Heading",Qt::DisplayRole);
            _FilterModel->setHeaderData(2,Qt::Horizontal,"FWC Color",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case FWC:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Display Message Selection");
        ui->SearchOptGB->show();
        ui->SearchOpt1Chk->setText("Display Message");
        ui->SearchOpt2Chk->hide();
        ui->SearchOpt3Chk->hide();
        ui->SearchOpt4Chk->hide();
        ui->SearchOpt5Chk->hide();
        ui->SearchOpt6Chk->hide();
        ui->SearchOpt7Chk->hide();
        ui->SearchOpt8Chk->hide();
        ui->SearchOpt9Chk->hide();
        ui->SearchOpt10Chk->hide();
        ui->SearchOpt11Chk->hide();
        ui->DirectionChk->hide();
        ui->DirectionCB->hide();
        ui->chkUserDB->hide();
        ui->userDbCB->hide();
        ui->SearchFileTypeCB->hide();
        ui->SearchMediaTypeCB->hide();
        break;

    case BITE:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("BITE Message Selection");
        ui->SearchOptGB->show();
        ui->SearchOpt1Chk->setText("Message");
        ui->SearchOpt2Chk->setText("Label Bit");
        ui->SearchOpt3Chk->setText("Description");
        ui->SearchOpt4Chk->hide();
        ui->SearchOpt5Chk->hide();
        ui->SearchOpt6Chk->hide();
        ui->SearchOpt7Chk->hide();
        ui->SearchOpt8Chk->hide();
        ui->SearchOpt9Chk->hide();
        ui->SearchOpt10Chk->hide();
        ui->SearchOpt11Chk->hide();
        ui->DirectionChk->hide();
        ui->DirectionCB->hide();
        ui->chkUserDB->hide();
        ui->userDbCB->hide();
        ui->SearchFileTypeCB->hide();
        ui->SearchMediaTypeCB->hide();
        break;
    case RELEASE:
    {

		onActionChange(); 
		this->setWindowTitle("Select Parameters");
        setUIForParameterSearch();
        ui->CommandParamCB->hide();
        ui->SearchOpt1Chk->setChecked(false);
        ui->SearchOpt1Chk->hide();
        _SubSearchType = PARAMETER_SEARCH;
        ui->SearchVMACVarChk->show();
        ui->SearchVMACVarChk->setChecked(false);
        connect(ui->SearchVMACVarChk, SIGNAL(stateChanged(int)),this, SLOT(onSearchVMACEnabled(int)));
    }
    break;
    case IMAGE:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Picture Selection");
        ui->PreviewImgPB->show();
        ui->SearchOptGB->show();
        ui->SearchOptSW->setCurrentIndex(1);
        break;
    case ONE_CLICK:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("OneClick Config Selection");
        _FilterModel  = getOneClickSearchItems();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"One Click Config Name",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case ONECLICK_LAUNCH_APP:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("OneClick Launch Application Config Selection");
        _FilterModel  = getOneClickLaunchAppSearchItems();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"OneClick Launch Application Config Name",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;

    case AUDIO_RECOG:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        this->setWindowTitle("Select Audio Alarm");
        _FilterModel  = getAudioRecogSearchItems();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Audio Alarm Name",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;

    case USER_DEF:
		onActionChange(); 
		_SubSearchType = INVALID_SEARCH;
        QString name = _UserDb.getName();
        this->setWindowTitle(name);
        ui->SearchOpt1Chk->setChecked(false);
        QString path = _UserDb.getPath();
        QString relativePath = _UserDb.getRelativePath();

        QFile file(path);
        if (!file.exists())
        {
            if (!relativePath.isEmpty())
            {
                GTASystemServices* pSystemServices = GTASystemServices::getSystemServices();
                path = pSystemServices->getLibraryPath() + relativePath;
            }
            else
            {
                QList<GTAGenericDB> userDBSearchList;
                bool rc = _pAppCtrl->parseGenericDBFile(userDBSearchList);
                if (rc)
                {
                    foreach(GTAGenericDB userDBSearch, userDBSearchList)
                    {
                        if (userDBSearch.getName() == name)
                        {
                            GTASystemServices* pSystemServices = GTASystemServices::getSystemServices(); 
                            path = pSystemServices->getLibraryPath() + userDBSearch.getRelativePath();
                            break;
                        }
                    }
                }
            }
        }

        _FilterModel = getUserDefinedDatabaseRecords(path);
        if(_FilterModel != NULL)
        {
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;


        //    case USER_DEF_DB_SEARCH:
        //        _SubSearchType = INVALID_SEARCH;

        //        break;
    }
    ui->SearchResultTW->resizeColumnsToContents();
}

GTAFilterModel * GTAGenSearchWidget::getUserDefinedDatabaseRecords(const QString &iPath)
{
    //    QList<QStringList*> Records;
    QFile file(iPath);
    if(file.exists())
    {
        QList<QString> headings = _UserDb.getHeadingList();
        QList<int> columns = _UserDb.getColumnList();
        QString delimiter = _UserDb.getDelimiter();
        if(headings.count() == columns.count())
        {

            if(file.open(QIODevice::ReadOnly))
            {
                //            QStandardItemModel *userDataModel = new QStandardItemModel();

                int count = 0;
                _StandardItemModel.clear();
                _StandardItemModel.setColumnCount(columns.count());
                //            _StandardItemModel.setRowCount(100);
                while(!file.atEnd())
                {

                    QByteArray line = file.readLine();
                    QString Line(line);
                    QStringList values = Line.split(delimiter);
                    if(values.count() == 0)
                        break;
                    if(count == 0)
                    {
                        for(int i = 0; i < headings.count(); i++)
                        {
                            _StandardItemModel.setHeaderData(i,Qt::Horizontal,headings.at(i));
                        }
                        count++;
                    }
                    else
                    {

                        for(int j = 0; j < columns.count(); j++)
                        {
                            int index = columns.at(j);
                            if(index < values.count())
                            {
                                QString val = values.at(index);
                                _StandardItemModel.setItem((count-1),j, new QStandardItem(val));
                                if (j == columns.count() - 1)
                                    count++;

                            }
                        }                        
                    }
                }

                if(_FilterModel != NULL)
                {
                    delete _FilterModel;
                    _FilterModel = NULL;
                }
                _FilterModel = new GTAFilterModel(&_StandardItemModel);
                return _FilterModel;
            }

        }
    }
    return NULL;
}

GTAGenSearchWidget::ElementType GTAGenSearchWidget::getSearchType() const
{
    return _CurrentSearchType;
}
void GTAGenSearchWidget::setSource(const QList<GTAICDParameter> &iSource)
{
    if(iSource.count() < 1 )
        return ;

    ui->SearchResultTW = new QTableView();
    QStringList header;
    header<<"NAME";
    header<<"PARAMETER_LOCAL_NAME";
    header<<"LABEL";
    header<<"BIT";
    header<<"TYPE";
    header<<"UNIT";

    QStandardItemModel *searchResultTvModel = new QStandardItemModel();

    for(int i = 0; i< header.count();i++)
    {
        searchResultTvModel->setHeaderData(i,Qt::Horizontal,header.at(i));
    }
    for(int p = 0; p<iSource.count(); p++)
    {
        GTAICDParameter icdParam = iSource.at(p);

        searchResultTvModel->setItem(p,0,new QStandardItem(icdParam.getName()));
        searchResultTvModel->setItem(p,1,new QStandardItem(icdParam.getParameterLocalName()));
        searchResultTvModel->setItem(p,2,new QStandardItem(icdParam.getLabel()));
        searchResultTvModel->setItem(p,3,new QStandardItem(icdParam.getBit()));
        searchResultTvModel->setItem(p,4,new QStandardItem(icdParam.getParameterType()));
        searchResultTvModel->setItem(p,5,new QStandardItem(icdParam.getUnit()));
    }

    ui->SearchResultTW->setModel(searchResultTvModel);
}
void GTAGenSearchWidget::setSource(const QHash<QString, QString> &iImageList)
{

    ui->SearchResultTW = new QTableView();
    QStringList header;
    header<<"NAME";
    header<<"PATH";

    QStandardItemModel *searchResultTvModel = new QStandardItemModel(iImageList.count(),header.count());

    for(int i = 0; i< header.count();i++)
    {
        searchResultTvModel->setHeaderData(i,Qt::Horizontal,header.at(i));
    }
    int rowCounter = 0;

    foreach(QString imgName, iImageList.keys())
    {
        QString imgPath = iImageList.value(imgName);
        searchResultTvModel->setItem(rowCounter,0,new QStandardItem(imgName));
        searchResultTvModel->setItem(rowCounter,0,new QStandardItem(imgPath));
        rowCounter++;
    }
    ui->SearchResultTW->setModel(searchResultTvModel);
}
void GTAGenSearchWidget::setSource(const QList<GTAMessage> &/*iMsgList*/, ElementType iType)
{
    ui->SearchResultTW = new QTableView();
    QStringList header;
    header<<"MESSAGE";
    header<<"DESCRIPTION";
    header<<"LABEL/BIT";
    switch(iType)
    {
    case kWarning: header<<"DISPLAY COLOR"<<"DISPLAY SCREEN"; break;
    case FWC: header<<"DISPLAY COLOR"; break;
    case BITE: header<<"FWC MESSAGE"<<"FWC LABEL/BIT";
    }
}

void GTAGenSearchWidget::onSelectionTableSelectionChange()
{
    switch(_CurrentSearchType)
    {
    case IMAGE:
        QStandardItemModel *pmodel = dynamic_cast<QStandardItemModel *>(ui->SearchResultTW->model());
        QStandardItem *pItem = pmodel->itemFromIndex(ui->SearchResultTW->currentIndex());
        if(pItem != NULL)
        {
            QString imgPath = pItem->text();
            QPixmap image(imgPath);
            ui->ImgPreviewLabel->setPixmap(image);
        }
        break;
    }
}


/*void GTAGenSearchWidget::onOKPBClicked()
{
    QItemSelectionModel * pSelectModel  = ui->SearchResultTW->selectionModel();
    if(pSelectModel != NULL)
    {
        QModelIndex currIdx = pSelectModel->currentIndex();
        if(currIdx.isValid() && (currIdx.row() >= 0))
        {

            QString oItem = QString();
            int rowId = ui->SearchResultTW->currentIndex().row();

            oItem = ui->SearchResultTW->model()->index(rowId,0).data().toString();
            bool isInvalidUserInput;
            _channelSelected = getUserChannelSelection(oItem,isInvalidUserInput);
            QString paramName;

            if (isInvalidUserInput==false)
            {
                if (_FilterModel)
                {
                    int colCnt = _FilterModel->columnCount();
                    for (int i=0;i<colCnt;i++)
                    {
                        if(i)
                            _selecteItems.append(ui->SearchResultTW->model()->index(rowId,i).data().toString()) ;
                        else
                            paramName= ui->SearchResultTW->model()->index(rowId,i).data().toString() ;
                    }
                }

                if (!_channelSelected.isEmpty())
                {
                    QStringList channelIdentifiers = GTAUtil::getParamChannelIdentifiers();

                    foreach (QString sChannelIdentifier, channelIdentifiers)
                    {
      if (paramName.contains(sChannelIdentifier, Qt::CaseSensitive))
                        {
       QString str = sChannelIdentifier.pattern();
                            paramName.replace(sChannelIdentifier,QString("%1%2:%3%1").arg(GTA_CIC_IDENTIFIER,sChannelIdentifier,_channelSelected));

                            break;
                        }
                    }
                    QStringList EngIdentifiers = GTAUtil::getParamEngIdentifiers();
        foreach (QString sChannelIdentifier, EngIdentifiers)
                    {
                        if (paramName.contains(sChannelIdentifier, Qt::CaseSensitive))
                        {
                            paramName.replace(sChannelIdentifier,QString("%1%2:%3%1").arg(GTA_CIC_IDENTIFIER,sChannelIdentifier,_channelSelected));
                            break;
                        }
                    }
                }
                _selecteItems.prepend(paramName);

                emit okPressed();
                disconnect(ui->FilterStr1LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
                disconnect(ui->FilterStr2LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
                ui->FilterStr1LE->setText("");
                ui->FilterStr2LE->setText("");
                connect(ui->FilterStr1LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
                connect(ui->FilterStr2LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
                if(_FilterModel != NULL)
                {
                    delete _FilterModel;
                    _FilterModel = NULL;
                }
                if(_SubSearchType == PARAMETER_SEARCH)
                {
                    _SqlQueryModel->clear();
                    _pAppCtrl->closeDatabase();
                }

                if(pSelectModel != NULL)
                {
                    delete pSelectModel;
                    pSelectModel = NULL;
                }
            }
            else
                QMessageBox::critical(0,"Compile Error","User need to select proper channel");
        }
        else
        {
            QMessageBox::critical(0,"Compile Error","Select a valid row");
        }

    }

}*/

bool GTAGenSearchWidget::processSelItemForUserDefDb(const QModelIndex &iCurrentIndex)
{
    bool rc = false;
    if(iCurrentIndex.isValid() && (iCurrentIndex.row() >= 0))
    {
        QList<int> colConcatList = _UserDb.getColsConcatList();
        QString concatStr = _UserDb.getConcatString();
        int rowId = iCurrentIndex.row();
        QString Item;
        QString color = "";
        QString backColor = "";
        if(colConcatList.count() == 1)
        {
            Item = ui->SearchResultTW->model()->index(rowId,colConcatList.at(0)).data().toString();
            rc = true;
        }
        else if(colConcatList.count() > 1)
        {
            QStringList temp;
            for(int i = 0; i < colConcatList.count(); i++)
            {
                if (ui->SearchResultTW->model()->headerData(colConcatList.at(i), Qt::Horizontal).toString() == "Color") {
                    color = ui->SearchResultTW->model()->index(rowId, colConcatList.at(i)).data().toString();
                    color.remove("\r");
                    color.remove("\n");
                    if (!color.isEmpty() && color != "WHITE")
                        backColor = "BLACK";
                }
                else {
                    QString t = ui->SearchResultTW->model()->index(rowId, colConcatList.at(i)).data().toString();
                    temp.append(t);
                }
            }
            Item = temp.join(concatStr);
            rc = true;
        }
        else
        {
            Item = ui->SearchResultTW->model()->index(rowId,0).data().toString();
            rc = true;
        }
        _selecteItem.setName(Item);
        _selecteItem.setTempParamName(Item);
        _selecteItem.setColor(color);
        _selecteItem.setBackColor(backColor);
    }
    return rc;
}

bool GTAGenSearchWidget::processSelectedItem(const QModelIndex &iCurrentIndex)
{
    bool rc = false;
    if(iCurrentIndex.isValid() && (iCurrentIndex.row() >= 0))
    {

        QString oItem = QString();
        //        int rowId = ui->SearchResultTW->currentIndex().row();
        int rowId = iCurrentIndex.row();

        oItem = ui->SearchResultTW->model()->index(rowId,0).data().toString();
        bool isInvalidUserInput;
        _channelSelected = getUserChannelSelection(oItem,isInvalidUserInput);
        QString paramName, paramName2, paramName3;

        if (isInvalidUserInput==false)
        {
            QStringList items;
            if (_FilterModel)
            {
                int colCnt = _FilterModel->columnCount();

                for (int i=0;i<colCnt;i++)
                {
                    if(i)
                        items.append(ui->SearchResultTW->model()->index(rowId,i).data().toString());
                    else
                    {
                        //code executed only for 0th index
                        QString oItem = ui->SearchResultTW->model()->index(rowId,i).data().toString();
                        //if (oItem.contains("line."))
                        if (ui->CommandParamCB->isChecked())
                        {
                            //a dummy commandName is given to not break the SCXML generation for new SCXML
                            QString commandName = ui->SearchResultTW->model()->index(rowId,4).data().toString();
                            if (commandName.isEmpty())
                                commandName = "FunctionName";
                            oItem = QString("%1_uuid[%2]%4%3").arg(oItem,ui->SearchResultTW->model()->index(rowId,3).data().toString(),commandName,INTERNAL_PARAM_SEP);
                        }
                        paramName= oItem;
                    }
                }
            }
            if (!_channelSelected.isEmpty())
            {

                QList<QRegExp> channelIdentifiers = GTAUtil::getParameterChannelIdentifiers();
                QString replaceStr;

                foreach (QRegExp sChannelIdentifier, channelIdentifiers)
                {

                    if (paramName.contains(sChannelIdentifier))
                    {
                        QString str = sChannelIdentifier.pattern();
                        // QString paramStr = paramName;
                        if(str == "_CHA." )
                        {
                            replaceStr.clear();
                            replaceStr = QString("_%1%2:%3%1.").arg(GTA_CIC_IDENTIFIER,"CHA",_channelSelected);
                            paramName.replace(str,replaceStr);

                        }
                        if(str == "_CHB.")
                        {
                            replaceStr.clear();
                            replaceStr = QString("_%1%2:%3%1.").arg(GTA_CIC_IDENTIFIER,"CHB",_channelSelected);
                            paramName.replace(str,replaceStr);

                        }

                    }

                }

                QList<QRegExp> EngIdentifiers = GTAUtil::getParameterEngineIdentifiers();
                foreach (QRegExp sChannelIdentifier, EngIdentifiers)
                {
                    if (paramName.contains(sChannelIdentifier))
                    {
                        QString str = sChannelIdentifier.pattern();
                        if(str == "EEC1_A" || str == "EEC2_A")
                        {
                            if(_channelSelected == "CHB")
                            {
                                replaceStr.clear();
                                replaceStr = QString("%1%2:%3%1").arg(GTA_CIC_IDENTIFIER,str,"B");
                                paramName.replace(str,replaceStr);
                            }
                            else if(_channelSelected == BOTH_CHANNEL || _channelSelected == CHANNEL_IN_CONTROL || _channelSelected == CHANNEL_NOT_IN_CONTROL)
                            {
                                replaceStr.clear();
                                replaceStr = QString("%1%2:%3%1").arg(GTA_CIC_IDENTIFIER,str,_channelSelected);
                                paramName.replace(str,replaceStr);
                            }


                        }

                        if(str == "EEC1_B" || str == "EEC2_B")
                        {
                            if(_channelSelected == "CHA")
                            {
                                replaceStr.clear();
                                replaceStr = QString("%1%2:%3%1").arg(GTA_CIC_IDENTIFIER,str,"A");
                                paramName.replace(str,replaceStr);
                            }
                            else if(_channelSelected == BOTH_CHANNEL || _channelSelected == CHANNEL_IN_CONTROL || _channelSelected == CHANNEL_NOT_IN_CONTROL)
                            {
                                replaceStr.clear();
                                replaceStr = QString("%1%2:%3%1").arg(GTA_CIC_IDENTIFIER,str,_channelSelected);
                                paramName.replace(str,replaceStr);
                            }

                        }

                    }
                }

            }
            items.prepend(paramName);
            _selecteItem = updateSelectedItemStruct(items);
            if (_CurrentSearchType == RELEASE_PIR_SEARCH || (_CurrentSearchType == PARAMETER && _SubSearchType == LIVEMODE_PARAMETER_SEARCH) || _CurrentSearchType == FUNCTION)
                _selectedItems.append(_selecteItem);
            rc = true;

        }
        else
            QMessageBox::critical(0,"Error","User need to select proper channel");
    }
    else
    {
        QMessageBox::critical(0,"Error","Select a valid row");
    }

    return rc;
}
GTAICDParameter GTAGenSearchWidget::updateSelectedItemStruct(const QStringList &iItems)
{
    if(iItems.count()>1)
    {
        if(_CurrentSearchType == GTAGenSearchWidget::PARAMETER)
        {
            if(_SubSearchType == GTAGenSearchWidget::PARAMETER_SEARCH || _SubSearchType == GTAGenSearchWidget::LIVEMODE_PARAMETER_SEARCH)
            {
                _selecteItem.setName(iItems.at(0));
                _selecteItem.setParameterType(iItems.at(1));
                _selecteItem.setSignalType(iItems.at(2));
                _selecteItem.setUnit(iItems.at(3));
                _selecteItem.setValueType(iItems.at(4));
                _selecteItem.setMinValue(iItems.at(5));
                _selecteItem.setMaxValue(iItems.at(6));
                _selecteItem.setLabel(iItems.at(7));
                //_selecteItem.setIdentifier(iItems.at(8));
				_selecteItem.setParameterLocalName(iItems.at(8));
                _selecteItem.setBit(iItems.at(9));
                _selecteItem.setEquipmentName(iItems.at(10));
                _selecteItem.setSourceFile(iItems.at(11));
                _selecteItem.setLastModifiedDate(iItems.at(12));
                //_selecteItem.setSourceType(iItems.at(13));
                //_selecteItem.setApplicationName(iItems.at(14));
                //_selecteItem.setBusName(iItems.at(15));
                //_selecteItem.setSignalName(iItems.at(16));
                //_selecteItem.setMedia(iItems.at(17));
                _selecteItem.setMessageName(iItems.at(13));
                //_selecteItem.setParameterLocalName(iItems.at(19));
                _selecteItem.setFSName(iItems.at(14));
                _selecteItem.setSuffix(iItems.at(15));
                _selecteItem.setPrecision(iItems.at(16));
                //_selecteItem.setTempParamName(iItems.at(23));
                _selecteItem.setDirection(iItems.at(17));
                _selecteItem.setMediaType(iItems.at(18));
                _selecteItem.setToolName(iItems.at(19));
                _selecteItem.setToolType(iItems.at(20));
                _selecteItem.setRefreshRate(iItems.at(21));
                //_selecteItem.setPossibleValues(iItems.at(29));
            }
            else if(_SubSearchType == GTAGenSearchWidget::LOCAL_SEARCH)
            {
                if(iItems.count() ==1)
                {
                    _selecteItem.setName(iItems.at(0));
                }
            }
            else if(_SubSearchType == GTAGenSearchWidget::ACT_CHK_INTERNAL_VAR)
            {
                _selecteItem.setName(iItems.at(0));
                _selecteItem.setValueType(iItems.at(1));
                _selecteItem.setUUID(iItems.at(3));
            }
            else
            {
                if(iItems.count() ==1)
                {
                    _selecteItem.setName(iItems.at(0));
                }
            }
        }
        else if(_CurrentSearchType == GTAGenSearchWidget::kWarning)
        {
            if(iItems.count()>=3)
            {
                _selecteItem.setName(iItems.at(0));
                _selecteItem.setApplicationName(iItems.at(1));
                _selecteItem.setActorName(iItems.at(2));
            }
        }
        else if(_CurrentSearchType == GTAGenSearchWidget::USER_DEF)
        {
            if(iItems.count()>=1)
            {
                _selecteItem.setName(iItems.at(0));
            }
        }
        else if(_CurrentSearchType == GTAGenSearchWidget::BITE)
        {
            if(iItems.count()>=1)
            {
                _selecteItem.setName(iItems.at(0));
            }
        }
        else if((_CurrentSearchType == GTAGenSearchWidget::TEMPLATE) ||
                (_CurrentSearchType == GTAGenSearchWidget::PROCEDURE)||
                (_CurrentSearchType == GTAGenSearchWidget::SEQUENCE)||
                (_CurrentSearchType == GTAGenSearchWidget::OBJECT)||
                (_CurrentSearchType == GTAGenSearchWidget::FUNCTION)||
                (_CurrentSearchType == GTAGenSearchWidget::RESULT_PRO)||
                (_CurrentSearchType == GTAGenSearchWidget::RESULT_SEQ))
        {
            if(iItems.count()>=2)
            {
                _selecteItem.setName(iItems.at(0));
                _selecteItem.setUUID(iItems.at(1));
            }
        }
        else
        {
            if(iItems.count()>=1)
            {
                _selecteItem.setName(iItems.at(0));
            }
        }
    }
    else if(iItems.count() == 1)
    {
        _selecteItem.setName(iItems.at(0));
    }

    return _selecteItem;
}

void GTAGenSearchWidget::onSelectAllPressed()
{
    ui->SearchResultTW->selectAll();
}

void GTAGenSearchWidget::onOKPBClicked()
{
    QItemSelectionModel * pSelectModel  = ui->SearchResultTW->selectionModel();
    _selectedItems.clear();
    if(pSelectModel != NULL)
    {
        bool rc = false;
        QModelIndexList selectedIndices = pSelectModel->selectedRows(0);
		if (selectedIndices.count() > 0)
		{
			if (_CurrentSearchType == RELEASE_PIR_SEARCH || (_CurrentSearchType == PARAMETER && _SubSearchType == LIVEMODE_PARAMETER_SEARCH))
			{

				for (int i = 0; i < selectedIndices.count(); i++)
				{
					QModelIndex index = selectedIndices.at(i);
					rc = processSelectedItem(index);
				}

			}
			else if (_CurrentSearchType == USER_DEF)
			{
				rc = processSelItemForUserDefDb(selectedIndices.at(0));

			}
			else
			{
				QModelIndex currIdx = selectedIndices.at(0);
				if (currIdx.isValid())
					rc = processSelectedItem(currIdx);
				else
					rc = false;
			}
		}
		else
		{
			QMessageBox::warning(this, "Warning", "No item selected");
		}

        if(rc)
        {
            emit okPressed();
            disconnect(ui->FilterStr1LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
            disconnect(ui->FilterStr2LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
            ui->FilterStr1LE->setText("");
            ui->FilterStr2LE->setText("");
            connect(ui->FilterStr1LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
            connect(ui->FilterStr2LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
            if(_FilterModel != NULL)
            {
                delete _FilterModel;
                _FilterModel = NULL;
            }
            if(_SubSearchType == PARAMETER_SEARCH || _SubSearchType == LIVEMODE_PARAMETER_SEARCH)
            {
                _SqlQueryModel->clear();
                _pAppCtrl->closeDatabase();
            }

            if(pSelectModel != NULL)
            {
                delete pSelectModel;
                pSelectModel = NULL;
            }
        }
    }
	else
	{
        QMessageBox::warning(this, "WARNING", "No item from the database is available");
	}
}

GTAICDParameter GTAGenSearchWidget::getSelectedItems() const
{
    return _selecteItem;
}

QList<GTAICDParameter> GTAGenSearchWidget::getSelectedItemList() const
{
    return _selectedItems;
}

QString GTAGenSearchWidget::getUserChannelSelection(const QString& iParam, bool & oinValid) const
{

    oinValid=false;
    bool selectChannel = TestConfig::getInstance()->getChannelSelection();
    QString channelSelected=QString();
    if (selectChannel == false)
        return channelSelected;

    if ((iParam.isEmpty()==false) && (((_SubSearchType==PARAMETER_SEARCH) || _SubSearchType == LIVEMODE_PARAMETER_SEARCH) && (_CurrentSearchType==PARAMETER)))
    {

        QStringList channelIdentifiers = GTAUtil::getParamChannelIdentifiers();
        QStringList EngIdentifiers = GTAUtil::getParamEngIdentifiers();
        channelIdentifiers.append(EngIdentifiers);
        foreach (QString sChannelIdentifier, channelIdentifiers)
        {

            if (iParam.contains(sChannelIdentifier,Qt::CaseInsensitive))
            {
                GTAChannelSelectorWidget* pChannDialog = new GTAChannelSelectorWidget();
                pChannDialog->setParameter(iParam);
                int retVal = pChannDialog->exec();
                if (retVal == QDialog::Accepted)
                {
                    channelSelected = pChannDialog->getChannelSelection(sChannelIdentifier);
                }
                else if (retVal == QDialog::Rejected)
                {
                    oinValid= true;
                }
                delete pChannDialog;pChannDialog=NULL;
                break;

            }

        }

    }
    return channelSelected;

}
void GTAGenSearchWidget::onCancelClicked()
{
    hide();
    disconnect(ui->FilterStr1LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
    disconnect(ui->FilterStr2LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
    ui->FilterStr1LE->setText("");
    ui->FilterStr2LE->setText("");
    connect(ui->FilterStr1LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
    connect(ui->FilterStr2LE,SIGNAL(textChanged(QString)),this,SLOT(clearSearch()));
    if(_FilterModel != NULL)
    {
        delete _FilterModel;
        _FilterModel = NULL;
    }
    if(_SubSearchType == PARAMETER_SEARCH || _SubSearchType == LIVEMODE_PARAMETER_SEARCH)
    {
        if(_SqlQueryModel!= NULL)
        {
            _SqlQueryModel = NULL;
            delete _SqlQueryModel;
        }
        _pAppCtrl->closeDatabase();
    }
}
void GTAGenSearchWidget::setCurrentConfiguration()
{
    //    ui->AircraftCB->clear();
    //    ui->EquipmentCB->clear();
    //    ui->StandardCB->clear();
    //    if(_pAppCtrl != NULL)
    //    {
    //        QStringList programName(_pAppCtrl->getProgram());
    //        QStringList equipmentName(_pAppCtrl->getEquipment());
    //        QStringList standardName(_pAppCtrl->getStandard());
    //        ui->AircraftCB->addItems(programName);
    //        ui->EquipmentCB->addItems(equipmentName);
    //        ui->StandardCB->addItems(standardName);
    //    }
}

void GTAGenSearchWidget::onOKPBClicked(const QModelIndex &/*iModelIndex*/)
{
    onOKPBClicked();
}

void GTAGenSearchWidget::getParamModelForType(bool isLocal)
{
    ui->CommandParamCB->setChecked(false);
    if(isLocal)
    {
        ui->SearchOpt3Chk->setChecked(false);
        getParamModelForType(GTAGenSearchWidget::LOCAL_SEARCH);
    }
    else
    {
        ui->SearchOpt3Chk->setChecked(true);
        getParamModelForType(GTAGenSearchWidget::PARAMETER_SEARCH);
    }
}

void GTAGenSearchWidget::showHideActionSelector(bool showhide)
{
    ui->SearchOpt5Chk->setChecked(!showhide);
    ui->SearchOpt3Chk->setChecked(!showhide);
    ui->CommandParamCoB->setVisible(showhide);
    if (showhide)
        ui->CommandParamCoB->setCurrentIndex(0);
}

void GTAGenSearchWidget::onSearchInternalVarEnabled(int index)
{
    if(index != 0 && ui->CommandParamCB->isChecked())
    {
        getParamModelForType(GTAGenSearchWidget::ACT_CHK_INTERNAL_VAR);
    }
    else
    {
        getParamModelForType(GTAGenSearchWidget::PARAMETER_SEARCH);
    }
}

void GTAGenSearchWidget::getParamModelForType(SubSearchType subSearchType)
{
    if(subSearchType == GTAGenSearchWidget::LOCAL_SEARCH)
    {
        ui->SearchOpt3Chk->setChecked(false);
        ui->SearchOpt4Chk->setChecked(false);
        ui->CommandParamCB->setChecked(false);
        ui->PreviewImgPB->hide();
        _SubSearchType = LOCAL_SEARCH;
        if((_CurrentSearchType == PARAMETER) || (_CurrentSearchType == INIT_PIR_SEARCH) || (_CurrentSearchType == GEN_PARAMETER) )
        {
            if(_SqlQueryModel != NULL)
            {
                delete _SqlQueryModel;
                _SqlQueryModel = NULL;
            }
            _pAppCtrl->closeDatabase();
        }
        _FilterModel = getLocalVarModel();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Local Variable",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
    }
    else if(subSearchType == GTAGenSearchWidget::SUBSCRIBE_LOCAL_SEARCH)
    {
        ui->SearchOpt3Chk->setChecked(false);
        ui->SearchOpt4Chk->setChecked(false);
        ui->CommandParamCB->setChecked(false);
        ui->PreviewImgPB->hide();
        _SubSearchType = LOCAL_SEARCH;
        if((_CurrentSearchType == PARAMETER) || (_CurrentSearchType == INIT_PIR_SEARCH) || (_CurrentSearchType == GEN_PARAMETER) )
        {
            if(_SqlQueryModel != NULL)
            {
                delete _SqlQueryModel;
                _SqlQueryModel = NULL;
            }
            _pAppCtrl->closeDatabase();
        }
        _FilterModel = getLocalVarModel(true);
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"Local Variable",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
    }
    else if(subSearchType == GTAGenSearchWidget::ACT_CHK_INTERNAL_VAR)
    {
        ui->SearchOpt3Chk->setChecked(false);
        ui->SearchOpt5Chk->setChecked(false);
        ui->SearchOpt4Chk->setChecked(false);
        ui->PreviewImgPB->hide();
        _SubSearchType = ACT_CHK_INTERNAL_VAR;
        this->setWindowTitle("Select Internal Command Parameters");
        if((_CurrentSearchType == PARAMETER) || (_CurrentSearchType == GEN_PARAMETER))
        {
            if(_SqlQueryModel != NULL)
            {
                delete _SqlQueryModel;
                _SqlQueryModel = NULL;
            }
            _pAppCtrl->closeDatabase();
        }
        _FilterModel = getCmdInternalVarSearchItems();
        //_FilterModel = getForEachCmdSearchItems();
        if(_FilterModel != NULL)
        {
            //            _FilterModel->setHeaderData(0,Qt::Horizontal,"Local Variable",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
    }
    else if(subSearchType == GTAGenSearchWidget::PARAMETER_SEARCH || subSearchType == GTAGenSearchWidget::LIVEMODE_PARAMETER_SEARCH)
    {
        ui->PreviewImgPB->show();
        _SubSearchType = subSearchType;
        //        _FilterModel = NULL;
        //         ui->SearchResultTW->setModel(_FilterModel);

        if(_FilterModel != NULL)
        {
            delete _FilterModel;
            _FilterModel = NULL;
        }

    }
    else
    {

    }
    if(_FilterModel != NULL)
    {
        ui->SearchResultTW->setModel(_FilterModel);
    }
}
void GTAGenSearchWidget::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    QWidget::keyPressEvent(event);
}
void GTAGenSearchWidget::showParamDetails()
{
    QHash<QString,QString> items;
    QString oItem = QString();
    QModelIndex tableModel = ui->SearchResultTW->currentIndex();
    if(tableModel.isValid())
    {
        int rowId = tableModel.row();

        oItem = ui->SearchResultTW->model()->index(rowId,0).data().toString();

        if (_FilterModel)
        {
            int colCnt = _FilterModel->columnCount();
            for (int i=0;i<colCnt;i++)
            {
                QString hdr = ui->SearchResultTW->model()->headerData(i,Qt::Horizontal).toString();
                QString val = ui->SearchResultTW->model()->index(rowId,i).data().toString();
                items.insert(hdr,val);
            }
        }
        _detail.clearTableData();

        _detail.setTableData(items);
        _detail.show();
        _detail.setWidgetTitle("Parameter Details");
    }
}
void GTAGenSearchWidget::disableOtherChecks(bool bCheckStatus)
{
    if(bCheckStatus)
        ui->SearchOpt5Chk->setChecked(false);
}

void GTAGenSearchWidget::onSearchVMACEnabled(int state)
{
    bool enable;
    if(Qt::Checked == state)
    {
        _SubSearchType = INVALID_SEARCH;
        ui->PreviewImgPB->hide();
        enable = false;
        _FilterModel  = getReleaseVariables();
        if(_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0,Qt::Horizontal,"VMAC Parameter Name",Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }

    }
    else if(Qt::Unchecked == state)
    {
        _SubSearchType = PARAMETER_SEARCH;
        enable = true;
        ui->PreviewImgPB->show();
        ui->SearchResultTW->setModel(NULL);
    }

    ui->SearchOpt3Chk->setEnabled(enable);
    ui->SearchOpt4Chk->setEnabled(enable);
    ui->SearchOpt5Chk->setEnabled(enable);
    ui->SearchOpt6Chk->setEnabled(enable);
    ui->SearchOpt7Chk->setEnabled(enable);
    ui->SearchOpt8Chk->setEnabled(enable);
    ui->SearchOpt9Chk->setEnabled(enable);
    ui->SearchOpt10Chk->setEnabled(enable);
    ui->SearchOpt11Chk->setEnabled(enable);
    ui->DirectionChk->setEnabled(enable);
    ui->DirectionCB->setEnabled(enable);
    ui->chkUserDB->setEnabled(enable);
    ui->userDbCB->setEnabled(enable);
    ui->SearchMediaTypeCB->setEnabled(enable);
    ui->SearchFileTypeCB->setEnabled(enable);

}

void GTAGenSearchWidget::setUIForParameterSearch()
{
    QStringList mediatype;
    QStringList filetype;
    ui->SearchOptGB->show();
    ui->PreviewImgPB->show();
    ui->CommandParamCB->show();
    ui->PreviewImgPB->setText("View Parameter Details");
    connect(ui->PreviewImgPB,SIGNAL(clicked()),this,SLOT(showParamDetails()));
    mediatype << "AFDX"<<"A429"<<"CAN"<<"ANA" <<"DIS" << "VMAC" << "PIR"<<"VCOM"<<"MODEL";
    filetype <<"PMR" <<"PIR" <<"ICD"<<"MODEL_ICD"<<"VCOM_STATIC";
    ui->SearchMediaTypeCB->clear();
    ui->SearchFileTypeCB->clear();
    ui->DirectionCB->clear();

    ui->SearchOpt1Chk->setVisible(true);
    ui->DBprogressBar->reset();
    ui->DBPageNumber->setVisible(true);
    ui->DBPageNumber->setText(QString::number(_lastindex));
    ui->SearchOpt1Chk->setText("Search Result Limit");
    ui->SearchOpt2Chk->hide();
    ui->SearchOpt3Chk->setText("Signal Name/Parameter Local Name");
    ui->SearchOpt4Chk->setText("Label");
    ui->SearchOpt5Chk->setText("Local Variables");
    ui->SearchOpt6Chk->setText("Application Name");
    ui->SearchOpt7Chk->setText("Bus Name");
    ui->SearchOpt8Chk->setText("File Type");
    ui->SearchOpt9Chk->setText("Media Type");
    ui->SearchOpt10Chk->setText("FS/SSM");
    ui->SearchOpt11Chk->setText("File Name");

    ui->SearchOpt1Chk->setChecked(true);
    ui->SearchOpt3Chk->setChecked(true);
    ui->SearchOpt4Chk->setChecked(false);
    ui->SearchOpt5Chk->setChecked(false);
    ui->SearchOpt6Chk->setChecked(false);
    ui->SearchOpt7Chk->setChecked(false);
    ui->SearchOpt8Chk->setChecked(false);
    ui->SearchOpt9Chk->setChecked(false);
    ui->SearchOpt10Chk->setChecked(false);
    ui->SearchOpt11Chk->setChecked(false);
    ui->DirectionChk->setChecked(false);
    ui->chkUserDB->setChecked(false);

    QStringList directionType;
    directionType << "All" << "Input" << "Output" << "I/O";
    ui->DirectionCB->addItems(directionType);
    ui->SearchFileTypeCB->addItems(filetype);
    ui->SearchMediaTypeCB->addItems(mediatype);
}


void GTAGenSearchWidget::setUserDefDb(const GTAGenericDB &iGenDbObj)
{
    _UserDb = iGenDbObj;
}

GTAGenSearchWidget::ElementType GTAGenSearchWidget::stringToSearchType(const QString &iSearchType) // Set rules for xml and add code for other search types
{
    ElementType type;
    if(iSearchType.contains("bite", Qt::CaseInsensitive))
    {
        type = BITE;

    }
    else if(iSearchType.contains("fwc",Qt::CaseInsensitive))
    {
        type = kWarning;
    }
    else if(iSearchType.contains("parameter",Qt::CaseInsensitive))
    {
        type = GEN_PARAMETER;
    }
    else if(iSearchType.contains("irt",Qt::CaseInsensitive))
    {
        type = IRT_OBJECT;
    }
    else if(iSearchType.contains("robotic_arm",Qt::CaseInsensitive))
    {
        type = ROBO_ARM_OBJECT;
    }
    else if(iSearchType.contains("USER_database",Qt::CaseInsensitive))
    {
        type = USER_DEF;
    }
    else
        type = PARAMETER;

    return type;
}

void GTAGenSearchWidget::onUserDefinedDBClicked(bool iVal)
{
    if(iVal)
    {
        if(_pAppCtrl)
        {
            _UserDBSearchList.clear();
            bool rc = _pAppCtrl->parseGenericDBFile(_UserDBSearchList);
            if(rc)
            {
                QStringList userDbNames = getUserDBNameList();
                ui->userDbCB->clear();
                disconnect(ui->userDbCB,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(onUserDBValChanged(const QString &)));
                ui->userDbCB->addItem(QString("-- Select --"));
                ui->userDbCB->addItems(userDbNames);
                ui->userDbCB->setEnabled(true);
                connect(ui->userDbCB,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(onUserDBValChanged(const QString &)));
                onUserDBValChanged(ui->userDbCB->currentText());

            }
        }
    }
    else
    {
        ui->userDbCB->setCurrentIndex(0);
        ui->userDbCB->setDisabled(true);
    }
}

void GTAGenSearchWidget::onUserDBValChanged(const QString &iCurrentText)
{
    if(!iCurrentText.isEmpty())
    {
        GTAGenericDB userDBObj;
        bool rc = getGenDBFromList(iCurrentText,userDBObj);
        if(rc)
        {
            _UserDb = userDBObj;
            setSearchType(USER_DEF);
        }
    }
}

/**
      * This slot enables disables the Page based DB search options
      * @iVal: boolean input to toggle visibility of ui components
      */
void GTAGenSearchWidget::showDBSearchLimiters(bool iVal)
{
    ui->DBSearchLimit->setEnabled(iVal);
        ui->DBSearchLimit->setVisible(iVal);
        ui->DBnextPage->setVisible(iVal);
        ui->DBprevPage->setVisible(iVal);
        ui->DBPageNumber->setVisible(iVal);
        ui->DBprogressBar->setVisible(iVal);
        ui->DBLoadTime->setVisible(iVal);
        if (iVal)
        {
            ui->DBSearchLimit->setValue(10);
            ui->DBSearchLimit->setMaximum(50);
            ui->DBSearchLimit->setMinimum(1);
            ui->SearchOpt1Chk->setText("Search Result Limit");
        }
        if (_SubSearchType != PARAMETER_SEARCH && _SubSearchType != RELEASE && _SubSearchType != LIVEMODE_PARAMETER_SEARCH)
        {
            ui->SearchOpt1Chk->setText("Description");
        }
}

QStringList GTAGenSearchWidget::getUserDBNameList()const
{
    QStringList userDbNames;
    for(int i = 0; i < _UserDBSearchList.count(); i++)
    {
        GTAGenericDB userDbObj = _UserDBSearchList.at(i);
        QString name = userDbObj.getName();
        userDbNames.append(name);
    }
    return userDbNames;
}


bool GTAGenSearchWidget::getGenDBFromList(const QString &iGenDBName, GTAGenericDB &oUserDB)const
{
    GTAGenericDB userDbObj;
    bool found = false;
    for(int i = 0; i < _UserDBSearchList.count(); i++)
    {
        userDbObj = _UserDBSearchList.at(i);
        if(iGenDBName == userDbObj.getName())
        {
            found = true;
            oUserDB = userDbObj;
            break;
        }
    }

    return found;
}

void GTAGenSearchWidget::setKnownVars(QStringList knownVars) {
	_knownVars = knownVars;
}

QStringList GTAGenSearchWidget::getKnownVars() {
	return _knownVars;
}

void GTAGenSearchWidget::searchFile(QString FileName,ElementType iSearchType)
{
    _CurrentSearchType = iSearchType;
    QStringList texts;
    
    FileName = FileName.replace("'", "");
    texts << FileName;

    switch (_CurrentSearchType)
    {
    case OBJECT:
        onActionChange();
        _FilterModel = getSearchItems("*.obj");
        if (_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0, Qt::Horizontal, "Object Name", Qt::DisplayRole);
            _FilterModel->setHeaderData(1, Qt::Horizontal, "Id", Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    case FUNCTION:
        onActionChange();
        _FilterModel = getSearchItems("*.fun");
        if (_FilterModel != NULL)
        {
            _FilterModel->setHeaderData(0, Qt::Horizontal, "Function Name", Qt::DisplayRole);
            _FilterModel->setHeaderData(1, Qt::Horizontal, "Id", Qt::DisplayRole);
            ui->SearchResultTW->setModel(_FilterModel);
        }
        break;
    }
    if (_FilterModel == NULL)
        return;
    QList<int> _cols;
    _cols << 0;
    QRegExp regExp("", Qt::CaseInsensitive, QRegExp::RegExp);
    _FilterModel->setSearchcolumns(_cols);
    _FilterModel->setSearchStrings(texts);
    _FilterModel->setFilterRegExp(regExp);
    ui->SearchResultTW->horizontalHeader()->stretchLastSection();

    // Get a reference to the selection model
    QItemSelectionModel* selectionModel = ui->SearchResultTW->selectionModel();

    // Get the model index for the first row
    bool rc = false;
    QModelIndex firstRowIndex = _FilterModel->index(0, 0);
    if (firstRowIndex.isValid())
        rc = processSelectedItem(firstRowIndex);
    else
        rc = false;
}

