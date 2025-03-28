#include "GTAActionOneClickLaunchAppWidget.h"
#include "ui_GTAActionOneClickLaunchAppWidget.h"
#include "GTAActionPrint.h"
#include "GTAParamValidator.h"
#include <QMessageBox>
#include "GTAAppController.h"
#include "GTAGenSearchWidget.h"
#include <QTextEdit>

GTAActionOneClickLaunchAppWidget::GTAActionOneClickLaunchAppWidget(QWidget *parent) :
        GTAActionWidgetInterface(parent),
        ui(new Ui::GTAActionOneClickLaunchAppWidget)
{
    ui->setupUi(this);
    connect(ui->searchAppPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->addAppPB,SIGNAL(clicked()),this,SLOT(addToApplicationList()));
    connect(ui->editAppPB,SIGNAL(clicked()),this,SLOT(editFromParamList ()));
    connect(ui->removeAppPB,SIGNAL(clicked()),this,SLOT(removeApplication()));
    connect(ui->applicatonLW,SIGNAL(itemSelectionChanged()),this,SLOT(updateParamLEFromLst()));
    connect(ui->applicationFileLE,SIGNAL(textChanged(const QString & )),this,SLOT(updateApplicatoinCB(const QString &)));
    connect(ui->AppliationNameCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateOptionList(QString)));
    connect(ui->KillAppCB,SIGNAL(stateChanged ( int)),this,SLOT(onKillModeCBToggeled(int)));
    connect(ui->StartApplicationCB,SIGNAL(stateChanged ( int)),this,SLOT(onStartApplicationCBClicked(int)));


    GTAParamValidator* pValidator1 = new GTAParamValidator(ui->applicationFileLE);
    ui->applicationFileLE->setValidator(pValidator1);
    _pDynamicGL = new QGridLayout();
    _pWidget = new QWidget();
    ui->KillAppCB->setVisible(true);
    ui->KillAppCB->setChecked(false);
    ui->StartApplicationCB->setChecked(true);

}

GTAActionOneClickLaunchAppWidget::~GTAActionOneClickLaunchAppWidget()
{
    delete ui;
}
QStringList GTAActionOneClickLaunchAppWidget::getValues() const
{
    QStringList sParameterList;
    int items = ui-> applicatonLW->count();
    for(int i=0;i<items;i++)
    {
        QListWidgetItem* pWidgetItem = ui-> applicatonLW->item(i);
        if (NULL!=pWidgetItem)
        {
            QString sParameter = pWidgetItem->text();
            if (!sParameter.isEmpty())
                sParameterList.append(sParameter);
        }
        
    }
    return sParameterList;
}
void GTAActionOneClickLaunchAppWidget::setValue(const QString & iVal)
{
    ui->applicationFileLE->setText(iVal);
}

void GTAActionOneClickLaunchAppWidget::onSearchPBClicked()
{
    emit searchObject(ui->applicationFileLE);
}

void GTAActionOneClickLaunchAppWidget::clear()
{
    _TestRig.clear();
    ui->applicationFileLE->clear();
    ui-> applicatonLW->clear();
    clearDynamicUIWidget(true);
    ui->KillAppCB->setChecked(false);
    ui->KillAppCB->setVisible(true);

}
bool GTAActionOneClickLaunchAppWidget::isValid()const
{
    return ui->applicationFileLE->text().isEmpty() ? false : true;
}
void GTAActionOneClickLaunchAppWidget::addToApplicationList()
{
    QString sApplicationFile = ui->applicationFileLE->text();
    QString sApplicationName = ui->AppliationNameCB->currentText();

    if (!sApplicationFile.isEmpty() && !sApplicationName.isEmpty())
    {
        GTAOneClickLaunchApplicationItemStruct* pOCAppItem = new GTAOneClickLaunchApplicationItemStruct();
        pOCAppItem->setConfigFile(sApplicationFile);
        pOCAppItem->setAppName(sApplicationName);
        pOCAppItem->setTestRig(_TestRig.getTestRigName());
        bool bKillApp = ui->KillAppCB->isChecked();
        pOCAppItem->setKillMode(bKillApp);
        pOCAppItem->setToolType(_Applications.getAppToolType());
        pOCAppItem->setToolId(_Applications.getAppToolId());

        //QString applicatonToAdd = QString("%1/%2").arg(sApplicationFile,sApplicationName);

        QString applicatonToAdd;
        QString sOptions;
        if (bKillApp==false)
        {
            int wgtCount = _widgetList.count();
            QStringList OtherText;
            for (int i =0;i<wgtCount;i++)
            {
                QWidget *pWid = _widgetList.at(i);
                if(pWid != NULL)
                {
                    QString objName = pWid->objectName();
                    if(objName.endsWith("_Cb"))
                    {
                        QString OptionVal;
                        QComboBox *pCb = dynamic_cast<QComboBox *>(pWid);
                        if(pCb!=  NULL)
                        {
                            OptionVal= pCb->currentText();

                            QStringList opts = objName.split("_");
                            opts.removeLast();
                            QString optionName =opts.join("_");
                            OtherText.append(QString("%1[%2]").arg(optionName,OptionVal));

                            pOCAppItem->insertOption(optionName,OptionVal);
                            foreach(GTAOneClickLaunchApplicationOptions option, _Options)
                            {
                                QList<GTAOneClickLaunchApplicationValue> valueLst = option.getValues();
                                pOCAppItem->insertOptionType(optionName,option.getOptionType());
                                foreach(GTAOneClickLaunchApplicationValue val,valueLst)
                                {
                                    if (OptionVal== val.getValueName())
                                    {
                                        QString sArguments = val.getArgs().getArgsValue();
                                        pOCAppItem->insertValArgs(optionName,sArguments);
                                    }


                                }


                            }
                        }
                    }
                }
            }
            sOptions = OtherText.join(",");
            //if(!bKillApp)
            applicatonToAdd = QString("%1/%2(%3)").arg(sApplicationFile,sApplicationName,sOptions);
         }
        else
            applicatonToAdd = QString("%1/%2_%3(%4)").arg(sApplicationFile,sApplicationName,"KillApp",sOptions);
//         else
//         {
            

/*        }*/
        

        ui->applicatonLW->addItem(applicatonToAdd);
        _LAItemStructs.append(pOCAppItem);



        // pOCAppItem->set
        //if (applicationExists(applicatonToAdd))
        //{
        //    QMessageBox::warning(this,"Provide values",QString("List already contains [%1]").arg(applicatonToAdd));

        //}
        //else
        //{
        //   /* structAppArgument newApplicaton;
        //    newApplicaton._appName = applicatonToAdd;
        //    _lstOfApplications.append(newApplicaton);
        //    ui->applicatonLW->addItem(applicatonToAdd);*/
        //    

        //}
    }
    else
        QMessageBox::warning(this,"Provide values","Provide application name and choose appropriate application name");
}
void GTAActionOneClickLaunchAppWidget::removeApplication()
{
    QList<QListWidgetItem*> lstOfSelectedItems = ui->applicatonLW->selectedItems();
    int itemSize = ui->applicatonLW->count();
    if (!lstOfSelectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedItem = lstOfSelectedItems.at(0);
        for (int i=0;i<itemSize;i++)
        {
            if (pSelectedItem == ui->applicatonLW->item(i))
            {
                GTAOneClickLaunchApplicationItemStruct* pAppItem = _LAItemStructs.takeAt(i);
                if (pAppItem)
                {
                    delete pAppItem;
                    pAppItem=NULL;
                }

                pSelectedItem = ui->applicatonLW->takeItem(i);
                delete pSelectedItem;

                return;

            }
            
        }
        /* QString seletedApplication = lstOfSelectedItems.at(0)->text();
        int index = getIndexOfApplicaton(seletedApplication);
        if (index>-1)
        {

            QString sApplicationFile = ui->applicationFileLE->text();
            QString sApplicationName = ui->AppliationNameCB->currentText();
            QString applicatonToreplace = QString("%1/%2").arg(sApplicationFile,sApplicationName);

            structAppArgument obj;
            obj._appName = applicatonToreplace;
            _lstOfApplications.removeAt(index);
            updateApplicationViewList();
        }*/



    }
}
void GTAActionOneClickLaunchAppWidget::editFromParamList()
{

    QList<QListWidgetItem*> lstOfSelectedItems = ui->applicatonLW->selectedItems();
    int itemSize = ui->applicatonLW->count();
    if (!lstOfSelectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedItem = lstOfSelectedItems.at(0);
        for (int j=0;j<itemSize;j++)
        {
            if (pSelectedItem == ui->applicatonLW->item(j))
            {

                QString sApplicationFile = ui->applicationFileLE->text();
                QString sApplicationName = ui->AppliationNameCB->currentText();

                if (!sApplicationFile.isEmpty() && !sApplicationName.isEmpty())
                {
                    GTAOneClickLaunchApplicationItemStruct* pOCAppItem = new GTAOneClickLaunchApplicationItemStruct;
                    pOCAppItem->setConfigFile(sApplicationFile);
                    pOCAppItem->setAppName(sApplicationName);
                    pOCAppItem->setTestRig(_TestRig.getTestRigName());    
                    bool bKillApp =ui->KillAppCB->isChecked();
                    pOCAppItem->setKillMode(bKillApp);
                    pOCAppItem->setToolType(_Applications.getAppToolType());
                    pOCAppItem->setToolId(_Applications.getAppToolId());

                    QString applicatonToAdd;
/*                    if (bKillApp==false)*/
                    //{
                        //QString applicatonToAdd = QString("%1/%2").arg(sApplicationFile,sApplicationName);
                        QString sOptions;
                        int wgtCount = _widgetList.count();
                        QStringList OtherText;
                        for (int i =0;i<wgtCount;i++)
                        {
                            QWidget *pWid = _widgetList.at(i);
                            if(pWid != NULL)
                            {
                                QString objName = pWid->objectName();
                                if(objName.endsWith("_Cb"))
                                {
                                    QString OptionVal;
                                    QComboBox *pCb = dynamic_cast<QComboBox *>(pWid);
                                    if(pCb!=  NULL)
                                    {
                                        OptionVal= pCb->currentText();

                                        QStringList opts = objName.split("_");
                                        opts.removeLast();
                                        QString optionName =opts.join("_");
                                        OtherText.append(QString("%1[%2]").arg(optionName,OptionVal));

                                        pOCAppItem->insertOption(optionName,OptionVal);
                                        foreach(GTAOneClickLaunchApplicationOptions option, _Options)
                                        {
                                            QList<GTAOneClickLaunchApplicationValue> valueLst = option.getValues();
                                            pOCAppItem->insertOptionType(optionName,option.getOptionType());
                                            foreach(GTAOneClickLaunchApplicationValue val,valueLst)
                                            {
                                                if (OptionVal== val.getValueName())
                                                {
                                                    QString sArguments = val.getArgs().getArgsValue();
                                                    pOCAppItem->insertValArgs(optionName,sArguments);
                                                }


                                            }


                                        }
                                    }
                                }
                            }
                        }
                        sOptions = OtherText.join(",");
                        if(!bKillApp)
                            applicatonToAdd = QString("%1/%2(%3)").arg(sApplicationFile,sApplicationName,sOptions);
                        else 
                            applicatonToAdd = QString("%1/%2_%3(%4)").arg(sApplicationFile,sApplicationName,"KillApp",sOptions);

//                     }
//                     else
                        
                    
                    //remove old item from applist and LW.

                    GTAOneClickLaunchApplicationItemStruct* pAppItem = _LAItemStructs.takeAt(j);
                    if (pAppItem)
                    {
                        delete pAppItem;
                        pAppItem=NULL;
                    }

                    QListWidgetItem* pCurrItem = ui->applicatonLW->takeItem(j);
                    delete pCurrItem;
                    
                    //add new item at removed position
                    ui->applicatonLW->insertItem(j,applicatonToAdd);
                    _LAItemStructs.insert(j,pOCAppItem);
                }
                return;
            }

        }

    }


    //QList<QListWidgetItem*> lstOfSelectedItems = ui->applicatonLW->selectedItems();
    //if (!lstOfSelectedItems.isEmpty())
    //{
    //    QString seletedApplication = lstOfSelectedItems.at(0)->text();
    //    int index = getIndexOfApplicaton(seletedApplication);
    //    if (index>-1)
    //    {

    //        QString sApplicationFile = ui->applicationFileLE->text();
    //        QString sApplicationName = ui->AppliationNameCB->currentText();
    //        QString applicatonToreplace = QString("%1/%2").arg(sApplicationFile,sApplicationName);

    //        structAppArgument obj;
    //        obj._appName = applicatonToreplace;
    //        _lstOfApplications.replace(index,obj);
    //        updateApplicationViewList();
    //    }

    //}
}

void GTAActionOneClickLaunchAppWidget::updateParamLEFromLst( )
{
    /*QList<QListWidgetItem*> items = ui-> applicatonLW->selectedItems();
    if(!items.isEmpty())
    {
        QListWidgetItem* pFirstItem = items.at(0);
        if(NULL!=pFirstItem)
        {
            ui->applicationFileLE->setText(pFirstItem->text().split("/").at(0));
            ui->AppliationNameCB->setCurrentIndex( ui->AppliationNameCB->findText (pFirstItem->text().split("/").at(1)));
        }
    }*/

    QList<QListWidgetItem*> lstOfSelectedItems = ui->applicatonLW->selectedItems();
    int itemSize = ui->applicatonLW->count();
    if (!lstOfSelectedItems.isEmpty())
    {
        QListWidgetItem* pSelectedItem = lstOfSelectedItems.at(0);
        for (int j=0;j<itemSize;j++)
        {
            if (pSelectedItem == ui->applicatonLW->item(j))
            {
                if (_LAItemStructs.size()>j)
                {
                    GTAOneClickLaunchApplicationItemStruct* pAppItem = _LAItemStructs.at(j);
                    QString appName = pAppItem->getAppName();
                    QString configFileName = pAppItem->getConfigFile();
                    bool killMode = pAppItem->getKillMode();
                    QStringList optionLst = pAppItem->getOptions();

                    ui->applicationFileLE->setText(configFileName);
                    int appNamePos = ui->AppliationNameCB->findText(appName);
                    if (appNamePos>=0)
                    {
                        ui->AppliationNameCB->setCurrentIndex(appNamePos);
                        QString sOptions;
                        int wgtCount = _widgetList.count();
                        QStringList OtherText;
                        for (int i =0;i<wgtCount;i++)
                        {
                            QWidget *pWid = _widgetList.at(i);
                            if(pWid != NULL)
                            {
                                QString objName = pWid->objectName();
                                if(objName.endsWith("_Cb"))
                                {
                                    QString OptionVal;
                                    QComboBox *pCb = dynamic_cast<QComboBox *>(pWid);
                                    objName.remove("_Cb");
                                    if(pCb!=  NULL)
                                    {
                                        int iNdex = pCb->findText(pAppItem->getValue(objName));
                                        pCb->setCurrentIndex(iNdex);


                                    }
                                }
                            }
                        }

                        QList<GTAOneClickLaunchApplicationApplications> _apps = _TestRig.getApplications();
                        for(int appIdx=0;appIdx<_apps.count();appIdx++)
                        {
                            GTAOneClickLaunchApplicationApplications app =_apps.at(appIdx);
                            QString name = app.getAppName();
                            if(appName == name)
                            {
                                _Applications = app;
                                break;
                            }
                        }
                        //bool bKillMode = _Applications.getKillMode();
                        //ui->KillAppCB->setVisible(bKillMode);
                        ui->KillAppCB->setVisible(true);
                        ui->KillAppCB->setChecked(killMode);
                    }
                }
            }
        }
    }
}
void GTAActionOneClickLaunchAppWidget::updateApplicatoinCB(const QString & iConfigName )
{
    ui->AppliationNameCB->clear();
    clearDynamicUIWidget();
    _TestRig.clear();
    if (!iConfigName.isEmpty())
    {
        GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
        bool rc = pAppCtrl->parseOneClickLaunchAppConfig(iConfigName,_TestRig);
        if(rc)
        {
            QStringList appNames;
            QList<GTAOneClickLaunchApplicationApplications> apps = _TestRig.getApplications();
            foreach(GTAOneClickLaunchApplicationApplications app,apps)
            {
                QString name = app.getAppName();
                appNames << name;
            }
            appNames.removeDuplicates();
            ui->AppliationNameCB->addItems(appNames);
            ui->AppliationNameCB->setCurrentIndex(0);
            if(!apps.isEmpty())
                _Applications = apps.first();
            //bool bKillMode = _Applications.getKillMode();
            //ui->KillAppCB->setVisible(bKillMode);
            ui->KillAppCB->setVisible(true);


        }
    }
}
QStringList GTAActionOneClickLaunchAppWidget::getApplicationNames(const QString &)
{
    return QStringList();
}
void GTAActionOneClickLaunchAppWidget:: updateApplicationViewList()
{
    ui->applicatonLW->clear();
    foreach(structAppArgument obj, _lstOfApplications)
    {
        ui->applicatonLW->addItem(obj._appName);
    }
}
bool GTAActionOneClickLaunchAppWidget::applicationExists(const QString& iAppName)
{
    foreach(structAppArgument obj, _lstOfApplications)
    {
        if (obj._appName == iAppName)
            return true;

    }
    return false;
}
int GTAActionOneClickLaunchAppWidget::getIndexOfApplicaton(const QString& iAppName)
{

    for(int i=0;i< _lstOfApplications.size();i++)
    {
        structAppArgument obj= _lstOfApplications.at(i);
        if (obj._appName == iAppName)
        {
            return i;
        }

    }
    return -1;
}

void GTAActionOneClickLaunchAppWidget::updateOptionList(QString iAppName)
{
    clearDynamicUIWidget();
    if (iAppName.isEmpty())
        return;


    QList<GTAOneClickLaunchApplicationApplications> _apps = _TestRig.getApplications();
    for(int appIdx=0;appIdx<_apps.count();appIdx++)
    {
        GTAOneClickLaunchApplicationApplications app =_apps.at(appIdx);
        QString name = app.getAppName();
        if(iAppName == name)
        {
            _Applications = app;
            break;
        }
    }
    _Options = _Applications.getOptions();
    _pWidget->setLayout(_pDynamicGL);
    ui->DynamicSA->setWidget(_pWidget);
    bool bKillMode = _Applications.getKillMode();
    QString description = _Applications.getAppDescription();
    if(!description.isEmpty())
    {

        QLabel *pDesLbl = new QLabel("Description");
        pDesLbl->setObjectName("Description_LBL");
        QTextEdit *pTE = new QTextEdit(description);
        pTE->setReadOnly(true);
        pTE->setObjectName("Description_TE");
        _widgetList.append(pDesLbl);
        _widgetList.append(pTE);
        _pDynamicGL->addWidget(pDesLbl);
        _pDynamicGL->addWidget(pTE);
    }
    if(!_Options.isEmpty())
    {
        for(int optIdx=0;optIdx<_Options.count();optIdx++)
        {
            GTAOneClickLaunchApplicationOptions opt = _Options.at(optIdx);
            QString optType = opt.getOptionType();
            QString optName = opt.getOptionName();
            QString optDef = opt.getOptionDeafault();
            if(optType == "dropdown")
            {
                QLabel *pLbl = new QLabel(optName);
                QComboBox *pCb = new QComboBox();
                pCb->setObjectName(optName+"_Cb");
                pLbl->setObjectName(optName+"_Lbl");
                _widgetList.append(pLbl);
                _widgetList.append(pCb);
                connect(pCb,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionCBValueChanged(QString)));
                QList<GTAOneClickLaunchApplicationValue> values = opt.getValues();
                for(int valIdx=0;valIdx<values.count();valIdx++)
                {
                    GTAOneClickLaunchApplicationValue val = values.at(valIdx);
                    QString valName = val.getValueName();
                    if(valName == optDef)
                    {
                        _Value = val;
                    }
                    pCb->addItem(valName);
                }

                Qt::MatchFlags flags = static_cast<Qt::MatchFlags>(Qt::MatchRegExp);
                int defIdx = pCb->findText(optDef,flags);
                pCb->setCurrentIndex(defIdx);
                _pDynamicGL->addWidget(pLbl);
                _pDynamicGL->addWidget(pCb);
                _pDynamicGL->setSizeConstraint(QLayout::SetMaximumSize);
                _pDynamicGL->setVerticalSpacing(10);
                _pDynamicGL->setHorizontalSpacing(10);
                _pDynamicGL->setSpacing(10);
            }

        }
    }
    else
    {
        QLabel *pLbl = new QLabel("No Options available.");
        _widgetList.append(pLbl);
        _pDynamicGL->addWidget(pLbl);
        _pDynamicGL->setSizeConstraint(QLayout::SetMaximumSize);
        _pDynamicGL->setVerticalSpacing(10);
        _pDynamicGL->setHorizontalSpacing(10);
        _pDynamicGL->setSpacing(10);

    }
    ui->KillAppCB->setChecked(bKillMode);
    //ui->KillAppCB->setVisible(bKillMode);
    ui->KillAppCB->setVisible(true);
}

void GTAActionOneClickLaunchAppWidget::clearDynamicUIWidget(bool bClearOneClickSturcts)
{
    for(int widIdx=0;widIdx<_widgetList.count();widIdx++)
    {
        QWidget * pWid = _widgetList.at(widIdx);
        if(pWid != NULL)
        {
            _pDynamicGL->removeWidget(pWid);
            delete pWid;
            pWid = NULL;
        }
    }
    _widgetList.clear();
    if(bClearOneClickSturcts)
    {
        /* for(int appIdx=0;appIdx<_LAItemStructs.count();appIdx++)
        {
            GTAOneClickLaunchApplicationItemStruct *pApp = _LAItemStructs.at(appIdx);
            if(pApp != NULL)
            {
                delete pApp;
                pApp = NULL;
            }
        }*/
        _LAItemStructs.clear();
    }
    
}

void GTAActionOneClickLaunchAppWidget::onOptionCBValueChanged(QString iOptionVal)
{
    QComboBox *pCb = dynamic_cast<QComboBox *>(sender());
    QString currentOptionVal = pCb->currentText();
    QString currentOptName = pCb->objectName();
    _Value = getValuebyName(currentOptionVal,currentOptName);

}

void GTAActionOneClickLaunchAppWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
    clear();
    deleteAndClearItemStructs();
    GTAOneClickLaunchApplication* pOneClickCmd = dynamic_cast<GTAOneClickLaunchApplication*>((GTAActionBase *)ipActionCmd);
    if(pOneClickCmd!= NULL)
    {
        QList<GTAOneClickLaunchApplicationItemStruct *> currentStructLst = pOneClickCmd->getLaStruct();

        foreach(GTAOneClickLaunchApplicationItemStruct * ipStruct, currentStructLst)
        {
            _LAItemStructs.append(new GTAOneClickLaunchApplicationItemStruct(*ipStruct));
        }
        
        foreach(GTAOneClickLaunchApplicationItemStruct *pItem,_LAItemStructs)
        {
            QString configFileName = pItem ->getConfigFile();
            QString appName = pItem->getAppName();
            QStringList optionList = pItem->getOptions();
            QStringList optionValues;
            bool bKillApp = pItem->getKillMode();

            //if(bKillApp==false)
            //{
                foreach (QString option, optionList)
                {
                    QString optionVal =  pItem->getValue(option);
                    optionValues.append( QString("%1[%2]").arg(option,optionVal));

                }
                if(!bKillApp)
                    ui->applicatonLW->addItem( QString("%1/%2(%3)").arg(configFileName,appName,optionValues.join(",")));
                else
                    ui->applicatonLW->addItem( QString("%1/%2_%3(%4)").arg(configFileName,appName,"KillApp",optionValues.join(",")));
            //}
            //else
                

            /* QString statement = pItem->getAppItem();
            ui->applicatonLW->addItem(statement);*/
        }

    }
}

bool GTAActionOneClickLaunchAppWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    GTAOneClickLaunchApplication* pOneClickLACmd = NULL;
    bool rc = false;

    if (!_LAItemStructs.isEmpty())
    {    pOneClickLACmd = new GTAOneClickLaunchApplication();
        pOneClickLACmd->insertLAStruct(_LAItemStructs);
        rc = true;
    }
    opCmd = pOneClickLACmd;
    return rc;


    /*GTAOneClickLaunchApplicationItemStruct * pOneClickLAStruct = NULL;
    QString configName = ui->applicationFileLE->text();
    QString appName = ui->AppliationNameCB->currentText();

    QString toolId = _Applications.getAppToolId();  
    QString toolType = _Applications.getAppToolType();*/

    //    pOneClickLAStruct->setConfigFile(configName);
    //    pOneClickLAStruct->setAppName(appName);
    //    pOneClickLAStruct->setToolId(toolId);
    //    pOneClickLAStruct->setToolType(toolType);
    //
    //    if(!_widgetList.isEmpty())
    //    {
    //        for(int widIdx=0;widIdx<_widgetList.count();widIdx++)
    //        {
    //            QWidget *pWid = _widgetList.at(widIdx);
    //            if(pWid != NULL)
    //            {
    //                QString objName = pWid->objectName();
    //                if(objName.endsWith("_Cb"))
    //                {
    //                    QString currentVal;
    //                    QComboBox *pCb = dynamic_cast<QComboBox *>(pWid);
    //                    if(pCb!=  NULL)
    //                        currentVal= pCb->currentText();
    //
    //                    QStringList opts = objName.split("_");
    //                    opts.removeLast();
    //                    QString optionName =opts.join("_");
    //                    _Value = getValuebyName(currentVal,optionName);
    //                    pOneClickLAStruct->insertOptionType(optionName,"dropdown");
    //                    pOneClickLAStruct->insertOption(optionName,_Value.getValueName());
    //                    pOneClickLAStruct->insertValArgs(optionName,_Value.getArgs().getArgsValue());
    ////                    pOneClickLAStruct->setAppItem();
    //                    /*int sizeOfList = ui->applicatonLW->count();
    //                    for (int i=0;i<sizeOfList;i++)
    //                    {
    //                        QListWidgetItem* pItem = ui->applicatonLW->itemAt(i);
    //                        if (pItem)
    //                        {
    //                            pOneClickLACmd->pItem->text();
    //                        }
    //                    }*/
    //
    //                }
    //            }
    //        }
    //    }

}

GTAOneClickLaunchApplicationValue GTAActionOneClickLaunchAppWidget::getValuebyName(const QString &,const QString &)
{
    GTAOneClickLaunchApplicationValue sVal;
    /* _Options = _Applications.getOptions();
    if(!_Options.isEmpty())
    {
        for(int optIdx=0;optIdx<_Options.count();optIdx++)
        {
            GTAOneClickLaunchApplicationOptions opt = _Options.at(optIdx);
            QString optName = opt.getOptionName();
            if(optName == iOptionName)
            {
                QList<GTAOneClickLaunchApplicationValue> values = opt.getValues();
                for(int valIdx=0;valIdx<values.count();valIdx++)
                {
                    GTAOneClickLaunchApplicationValue val = values.at(valIdx);
                    QString valName = val.getValueName();
                    if(valName == iValName)
                    {
                        sVal = val;
                        break;
                    }
                }
            }
        }
    }*/
    return sVal;
}
void GTAActionOneClickLaunchAppWidget::deleteAndClearItemStructs()
{
    foreach(GTAOneClickLaunchApplicationItemStruct* pItem,_LAItemStructs)
    {
        if (pItem)
            delete pItem;
    }
    _LAItemStructs.clear();

}


void GTAActionOneClickLaunchAppWidget::onKillModeCBToggeled(int )
{
    //bool iKillMode = (iKillModeState == Qt::Checked)?true:false;
    ui->StartApplicationCB->setChecked(!ui->KillAppCB->isChecked());
    if(!_widgetList.isEmpty())
    {
        foreach(QWidget *pItem , _widgetList)
        {
            if(pItem!=NULL)
            {
                QString objName = pItem->objectName();
                if(objName.endsWith("_Lbl") || objName.endsWith("_Cb"))
                {
                   // pItem->setVisible(!iKillMode);
                }
            }
        }
    }
}
void GTAActionOneClickLaunchAppWidget::onStartApplicationCBClicked(int cbState)
{
    if(cbState == Qt::Checked)
    {
        onKillModeCBToggeled(Qt::Unchecked);
        ui->KillAppCB->setChecked(false);
    }
    else
        ui->KillAppCB->setChecked(true);

}
int GTAActionOneClickLaunchAppWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::ONECLICK_LAUNCH_APP;
    return retVal;
}

#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_ONECLICK,COM_ONECLICK_APP),GTAActionOneClickLaunchAppWidget,obj)
