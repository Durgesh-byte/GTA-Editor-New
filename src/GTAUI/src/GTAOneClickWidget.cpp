#include "GTAOneClickWidget.h"
#include "ui_GTAOneClickWidget.h"
#include "GTAAppController.h"
#include "GTAOneClickOptions.h"
#include "GTAOneClickConfiguration.h"
#include "GTAOneClickApplications.h"
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidgetItem>
#include "GTAParamValidator.h"
#include "GTAOneClickDynamicDirOption.h"
#include "GTAGenSearchWidget.h"


GTAOneClickWidget::GTAOneClickWidget(QWidget *parent) :
    GTAActionWidgetInterface(parent),
    ui(new Ui::GTAOneClickWidget)
{
    _CurrentOptionHash.clear();
    _CurrentOptionLabelList.clear();
    ui->setupUi(this);
    DynamicOptionFL= new QGridLayout();

    connect(ui->SearchPB,SIGNAL(clicked()),this,SLOT(onSearchPBClicked()));
    connect(ui->ConfFileLE,SIGNAL(textChanged(QString)),this,SLOT(onConfFileChanged(QString)));
    connect(ui->ConfigurationCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onConfigNameChanged(QString)));
    connect(ui->ApplicationLB,SIGNAL(currentTextChanged(QString)),this,SLOT(onApplicationSelected(QString)));
    connect(ui->StartEnvCB,SIGNAL(toggled(bool)),this,SLOT(onStartEnvClicked(bool)));
    connect(ui->StopAltoCB,SIGNAL(toggled(bool)),this,SLOT(onStopAltoClicked(bool)));
    connect(ui->StopTestConfigCB,SIGNAL(toggled(bool)),this,SLOT(onStopTestConfig(bool)));
}

GTAOneClickWidget::~GTAOneClickWidget()
{
    ClearDynamicOptionWidgetItems();
    delete ui;
}

void GTAOneClickWidget::onApplicationSelected(QString iAppName)
{
    ClearDynamicOptionWidgetItems();
    QString selectedAppName;
    QString selectedAppVersion;
    if(iAppName.contains(":"))
    {

        QStringList appDet = iAppName.split(":");
        if(appDet.count() > 1)
        {
            selectedAppName = appDet.at(1);
        }
    }
    selectedAppName = selectedAppName.trimmed();
    selectedAppVersion = selectedAppName.trimmed();


    QList<GTAOneClickConfiguration> configs = _OneClickTestRig->getConfigurations();
    GTAOneClickConfiguration CurrConfig;
    GTAOneClickApplications CurrApp;
    foreach(GTAOneClickConfiguration config,configs)
    {
        QString ConfigName = config.getConfigurationName();
        QString selectedConfigName = getConfigName();
        if(ConfigName == selectedConfigName)
        {
            CurrConfig = config;
            break;
        }
    }

    QList<GTAOneClickApplications> apps = CurrConfig.getApplications();
    foreach(GTAOneClickApplications app,apps)
    {
        QString appName = app.getAppName();
        QString appVersion = app.getAppVersion();
        if(appVersion.isEmpty() || appVersion.isNull())
        {
            appVersion = "nil";
        }
        if(appVersion == selectedAppVersion)
        {
            CurrApp = app;
            break;
        }
    }
    
    _Options  = CurrApp.getAppOptions();
    createDymamicUI(_Options);
    setInitialValues();
    QString des = CurrApp.getDescription();
    setDescription(des);
    QLabel *lbl = new QLabel("Description ");
    QTextEdit *pTE =  new QTextEdit();
    pTE->setText(des);
    pTE->setReadOnly(true);
    pTE->setObjectName("Description_TEWid");
    _CurrentOptionLabelList.append(lbl);
    _CurrentOptionHash.insert("Description",pTE);

    DynamicOptionFL->addWidget(lbl);
    DynamicOptionFL->addWidget(pTE);
    pTE->setMaximumHeight(300);

    DynamicOptionFL->setSizeConstraint(QLayout::SetMaximumSize);
    DynamicOptionFL->setVerticalSpacing(10);
    DynamicOptionFL->setHorizontalSpacing(10);
    DynamicOptionFL->setSpacing(10);

    QWidget *pWidget = new QWidget();
    pWidget->setLayout(DynamicOptionFL);
    ui->DynamicOptionSA->setWidget(pWidget);
}

bool GTAOneClickWidget::decodeCondition(const QString &iCond, QString &oCondOption, QString &oCondValue)
{
    bool rc = false;
    QStringList items;
    if(iCond.contains("=="))
    {
        items = iCond.split("==",QString::SkipEmptyParts);
        if(items.count()>1)
        {
            oCondOption = items.at(0).trimmed();
            oCondValue = items.at(1).trimmed();
            rc = true;
        }
    }
    else
    {
        return rc;
    }
    return rc;
}

bool GTAOneClickWidget::createDymamicUI(GTAOneClickOptions iOption)
{
    bool rc = false;
    QStringList optionList = iOption.getAllOptions();
    int rowCounter = 0;
    foreach(QString optionName, optionList)
    {
        _CurrentOptionList.append(optionName);
        QString optionType = iOption.getOptionType(optionName);
        GTAOneClickOption opt = iOption.getDefOption(optionName);
        QString defVal = opt.getOptionDefaultValue();
        QStringList values = opt.getOptionValues();
        QString configPath = opt.getOptionConfigPath();
        QString defIniPath = opt.getOptionDefIniPath();
        if(optionType.toLower() == "yesno")
        {
            QCheckBox *pChkB = new QCheckBox(optionName);
            pChkB->setObjectName(optionName+"_ChkB");
            if(defVal.toLower() == "yes")
            {
                pChkB->setChecked(true);
            }
            else
            {
                pChkB->setChecked(false);
            }
            pChkB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            connect(pChkB,SIGNAL(toggled(bool)),this,SLOT(onOptionValueChanged(bool)));
            DynamicOptionFL->addWidget(pChkB,rowCounter++,0);
            if(!_CurrentOptionHash.contains(optionName)){
                _CurrentOptionHash.insert(optionName,pChkB);
            }
        }
        else if(optionType.toLower() == "dropdown")
        {

            QLabel *lblComB = new QLabel(optionName);
            lblComB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            DynamicOptionFL->addWidget(lblComB,rowCounter++,0);
            _CurrentOptionLabelList.append(lblComB);

            QComboBox *pComB = new QComboBox();
            pComB->setObjectName(optionName+"_ComB");
            pComB->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            if(!values.isEmpty())
            {
                foreach(QString value,values)
                {
                    if(!(value.isNull() || value.isEmpty()))
                    {
                        pComB->addItem(value.trimmed());
                    }
                }
            }
            DynamicOptionFL->addWidget(pComB,rowCounter++,0);
            connect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
            if(!_CurrentOptionHash.contains(optionName))
            {
                _CurrentOptionHash.insert(optionName,pComB);
            }
        }
        else if(optionType.toLower() == "selectfile")
        {
            //read the xml file to get the path for directory list.

            QString path = QDir::cleanPath(configPath);
            QDir dir(path);
            QStringList dirNames;
            QFileInfoList infoLists = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
            foreach(QFileInfo info,infoLists)
            {
                QString path1 = info.absoluteFilePath().trimmed();
                path1 = path1.remove(path);
                if(!path1.isEmpty() && info.isDir() && !path1.contains(QDir::separator()))
                {
                    dirNames.append(info.baseName());
                }
            }
            if(!dirNames.isEmpty())
            {
                GTAOneClickDynamicDirOption *pDirOpt = new GTAOneClickDynamicDirOption();
                pDirOpt->insertDirectories(dirNames,defIniPath);
                DynamicOptionFL->addWidget(pDirOpt,rowCounter++,0);
                if(!_CurrentOptionHash.contains(optionName))
                {
                    _CurrentOptionHash.insert(optionName,pDirOpt);
                }
            }
        }
        else if(optionType.toLower() == "freetext")
        {
            QLabel *lblTE = new QLabel(optionName);
            lblTE->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            DynamicOptionFL->addWidget(lblTE,rowCounter++,0);
            _CurrentOptionLabelList.append(lblTE);


            QTextEdit *pTE = new QTextEdit();
            pTE->setText(defVal);
            pTE->setObjectName(optionName+"_TE");
            pTE->setMaximumHeight(60);
            DynamicOptionFL->addWidget(pTE,rowCounter++,0);
            connect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            if(!_CurrentOptionHash.contains(optionName))
            {
                _CurrentOptionHash.insert(optionName,pTE);
            }
        }
    }
    foreach(QString optName, _CurrentOptionHash.keys())
    {
        _OptUpdatedFlag.insert(optName,false);
    }

    return rc;
}

void GTAOneClickWidget::onOptionValueChanged()
{
    QTextEdit *pUiItem = dynamic_cast<QTextEdit *>(sender());
    QString widObjName = pUiItem->objectName();
    QString optionName = widObjName.remove("_TE",Qt::CaseSensitive);
    QString val;
    if(pUiItem)
    {
        val= pUiItem->toPlainText();
        ////QRegExp rx("([*/^&~%!|(){}[]~,]+)");
        QRegExp rx("([&~!*^/%|,(){}[\\]]+)");
        val.remove(rx);
        disconnect(pUiItem,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
        pUiItem->setText(val);
        pUiItem->moveCursor(QTextCursor::End);
        connect(pUiItem,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
    }
    setOptionValues(optionName,val);

}

void GTAOneClickWidget::onOptionValueChanged(bool iVal)
{
    QCheckBox *pUiItem = dynamic_cast<QCheckBox *>(sender());
    QString widObjName = pUiItem->objectName();
    QString optionName = widObjName.remove("_ChkB",Qt::CaseSensitive);
    QString val = "";
    if(iVal)
    {
        val ="yes";
    }
    else
    {
        val ="no";
    }
    setOptionValues(optionName,val);

}

void GTAOneClickWidget::onOptionValueChanged(QString iVal)
{
    QComboBox *pUiItem = dynamic_cast<QComboBox *>(sender());
    QString widObjName = pUiItem->objectName();
    QString optionName = widObjName.remove("_ComB",Qt::CaseSensitive);
    setOptionValues(optionName,iVal);
}
bool GTAOneClickWidget::isRelatedtoOtherOptions(QString &iOptionName,QString &iOptionVal)
{
    QStringList options = _Options.getAllOptions();
    bool rc = false;
    bool start = false;
    GTAOneClickOption CondOpts;
    foreach(QString optName,options)
    {
        if(optName == iOptionName)
        {
            start = true;
            continue;
        }
        if(start)
        {
            CondOpts = (_Options.getOptionforCond(optName,iOptionName,iOptionVal));
            if(!CondOpts.getOptionName().isEmpty() && !CondOpts.getOptionName().isNull())
            {
                rc = true;
                break;
            }
        }
    }
    return rc;
}

void GTAOneClickWidget::setOptionValues(QString &iOptionName, QString &iVal)
{
    QString optionName = iOptionName;
    QStringList optNames = _Options.getAllOptions();
    QList<GTAOneClickOption> CondOptionNames;
    QList<GTAOneClickOption> DefOptionNames;
    QList<QString> CondDefOptions;
    QList<QString> CondCondOptions;
    QList<QString> NAOptionNames;
    bool start  = false;
    _OptUpdatedFlag[iOptionName]  =true;
    QStringList PrevOptionsList;
    QStringList ignoreList;
    foreach(QString optName,optNames)
    {
        if(optName == optionName)
        {
            start = true;
            continue;
        }
        else
        {
            if(start == true)
            {
                GTAOneClickOption condOpt = _Options.getOptionforCond(optName,optionName,iVal);
                if(!(condOpt.getOptionName().isEmpty() ||condOpt.getOptionName().isNull()))
                {
                    CondOptionNames.append(condOpt);
                }
                else
                {
                    foreach(QString prevOpt,PrevOptionsList)
                    {
                        QWidget *pItem1 = _CurrentOptionHash.value(prevOpt);
                        QString optType = _Options.getOptionType(prevOpt);
                        QString tempVal = iVal;
                        if(optType.toLower() == "dropdown")
                        {
                            QComboBox *pComB11 = dynamic_cast<QComboBox *>(pItem1);
                            if(pComB11 != NULL)
                            {
                                tempVal = pComB11->currentText();
                            }
                        }
                        else if(optType.toLower() == "yesno")
                        {
                            QCheckBox *pChkB11 = dynamic_cast<QCheckBox *>(pItem1);
                            if(pChkB11 != NULL)
                            {
                                if(pChkB11->isChecked())
                                {
                                    tempVal = "yes";
                                }
                                else
                                {
                                    tempVal = "no";
                                }

                            }
                        }
                        else if(optType.toLower() == "freetext")
                        {
                            QTextEdit *pTE11 = dynamic_cast<QTextEdit *>(pItem1);
                            if(pTE11 != NULL)
                            {
                                tempVal = pTE11->toPlainText();
                            }
                        }
                        GTAOneClickOption condOpt = _Options.getOptionforCond(prevOpt,optName,iVal);
                        if(!(condOpt.getOptionName().isEmpty() ||condOpt.getOptionName().isNull()))
                        {
                            CondOptionNames.append(condOpt);
                            ignoreList.append(condOpt.getOptionName());
                        }
                        else
                        {
                            GTAOneClickOption condOpt22 = _Options.getOptionforCond(optName,prevOpt,tempVal);
                            if(!(condOpt22.getOptionName().isEmpty() ||condOpt22.getOptionName().isNull()))
                            {
                                CondOptionNames.append(condOpt22);
                                ignoreList.append(condOpt22.getOptionName());
                            }
                        }
                    }

                    foreach(QString prevOpt,PrevOptionsList)
                    {
                        QWidget *pItem = _CurrentOptionHash.value(prevOpt);
                        QString optType = _Options.getOptionType(prevOpt);
                        QString tempVal = iVal;
                        if(optType.toLower() == "dropdown")
                        {
                            QComboBox *pComB1 = dynamic_cast<QComboBox *>(pItem);
                            if(pComB1 != NULL)
                            {
                                tempVal = pComB1->currentText();
                            }
                        }
                        else if(optType.toLower() == "yesno")
                        {
                            QCheckBox *pChkB1 = dynamic_cast<QCheckBox *>(pItem);
                            if(pChkB1 != NULL)
                            {
                                if(pChkB1->isChecked())
                                {
                                    tempVal = "yes";
                                }
                                else
                                {
                                    tempVal = "no";
                                }

                            }
                        }
                        else if(optType.toLower() == "freetext")
                        {
                            QTextEdit *pTE1 = dynamic_cast<QTextEdit *>(pItem);
                            if(pTE1 != NULL)
                            {
                                tempVal = pTE1->toPlainText();
                            }
                        }

                        GTAOneClickOption condOpt1 = _Options.getOptionforCond(optName,prevOpt,tempVal);
                        if(!(condOpt1.getOptionName().isEmpty() ||condOpt1.getOptionName().isNull()))
                        {
                            ignoreList.append(condOpt1.getOptionName());
                        }
                    }
                    if(ignoreList.contains(optName))
                    {
                        continue;
                    }
                    GTAOneClickOption defOpt = _Options.getDefOption(optName);
                    if(!(defOpt.getOptionName().isEmpty() ||defOpt.getOptionName().isNull()))
                    {
                        DefOptionNames.append(defOpt);
                    }
                    else
                    {
                        NAOptionNames.append(optName);
                    }
                }
            }
            else
            {
                PrevOptionsList.append(optName);
            }
        }
    }
    start =false;


    foreach(GTAOneClickOption opt2,DefOptionNames)
    {
        QString optName2 = opt2.getOptionName();
        QString optType2 = opt2.getOptionType();
        QString optDefVal2 = opt2.getOptionDefaultValue().trimmed();
        QStringList optValues2 = opt2.getOptionValues();
        if(optType2 =="dropdown")
        {
            QComboBox *pComB = dynamic_cast<QComboBox*>(_CurrentOptionHash.value(optName2));
            disconnect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
            if(pComB->count()>0)
            {
                pComB->clear();
            }
            if(optDefVal2.isEmpty() || optDefVal2.isNull())
            {
                if(optValues2.count()>0)
                    optDefVal2 = optValues2.at(0);
            }
            if(isRelatedtoOtherOptions(optName2,optDefVal2))
            {
                CondDefOptions.append(optName2);
                continue;
            }
            connect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
            if(optValues2.count()>0)
            {
                pComB->addItems(optValues2);
            }
            if(optDefVal2.isEmpty()||optDefVal2.isNull())
            {
                if(pComB->count()>0)
                    pComB->setCurrentIndex(0);
            }
            else
            {
                int idx = pComB->findText(optDefVal2);
                if(idx>=0)
                    pComB->setCurrentIndex(idx);
                else
                    if(pComB->count()>0)
                    {
                        pComB->setCurrentIndex(0);
                    }
            }

        }
        if(optType2 =="freetext")
        {
            QTextEdit *pTE = dynamic_cast<QTextEdit*>(_CurrentOptionHash.value(optName2));
            disconnect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            pTE->clear();
            if(isRelatedtoOtherOptions(optName2,optDefVal2))
            {
                CondDefOptions.append(optName2);
                continue;
            }
            connect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            if(!(optDefVal2.isEmpty()||optDefVal2.isNull()))
            {
                pTE->setText(optDefVal2);
            }
            else
            {
                pTE->setText("Type Here");
                pTE->clear();
            }


        }
        if(optType2 =="yesno")
        {
            QCheckBox *pChkB = dynamic_cast<QCheckBox*>(_CurrentOptionHash.value(optName2));
            disconnect(pChkB,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            QString tempval = "no";
            if(optDefVal2.toLower() == "yes")
            {
                tempval   = "yes";
            }
            if(isRelatedtoOtherOptions(optName2,tempval))
            {
                CondDefOptions.append(optName2);
                continue;
            }
            connect(pChkB,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            pChkB->setChecked(false);
            if(!(optDefVal2.isEmpty()||optDefVal2.isNull()))
            {
                if(optDefVal2.toLower()=="yes")
                {
                    pChkB->setChecked(true);
                }
                else
                {
                    pChkB->setChecked(true);
                    pChkB->setChecked(false);
                }

            }


        }
    }
    foreach(GTAOneClickOption opt2,DefOptionNames)
    {

        QString optName21 = opt2.getOptionName();
        QString optType21 = opt2.getOptionType();
        QString optDefVal21 = opt2.getOptionDefaultValue().trimmed();
        QStringList optValues21 = opt2.getOptionValues();
        if(CondDefOptions.contains(optName21))
        {
            if(optType21 =="dropdown")
            {
                QComboBox *pComB = dynamic_cast<QComboBox*>(_CurrentOptionHash.value(optName21));
                connect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
                if(optValues21.count()>0)
                {
                    pComB->addItems(optValues21);
                }
                if(optDefVal21.isEmpty()||optDefVal21.isNull())
                {
                    if(pComB->count()>0)
                        pComB->setCurrentIndex(0);
                }
                else
                {
                    int idx = pComB->findText(optDefVal21);
                    if(idx>=0)
                        pComB->setCurrentIndex(idx);
                    else
                        if(pComB->count()>0)
                        {
                            pComB->setCurrentIndex(0);
                        }
                }
            }
            if(optType21 =="freetext")
            {
                QTextEdit *pTE = dynamic_cast<QTextEdit*>(_CurrentOptionHash.value(optName21));
                connect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
                if(!(optDefVal21.isEmpty()||optDefVal21.isNull()))
                {
                    pTE->setText(optDefVal21);
                }
                else
                {
                    pTE->setText("Type Here");
                    pTE->clear();
                }
            }
            if(optType21 =="yesno")
            {
                QCheckBox *pChkB = dynamic_cast<QCheckBox*>(_CurrentOptionHash.value(optName21));
                connect(pChkB,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
                if(!(optDefVal21.isEmpty()||optDefVal21.isNull()))
                {
                    if(optDefVal21.toLower()=="yes")
                    {
                        pChkB->setChecked(true);
                    }
                    else
                    {
                        pChkB->setChecked(true);
                        pChkB->setChecked(false);
                    }

                }
            }
        }
    }


    foreach(GTAOneClickOption opt1,CondOptionNames)
    {
        QString optName1 = opt1.getOptionName();
        QString optType1 = opt1.getOptionType();
        QString optDefVal1 = opt1.getOptionDefaultValue();
        QStringList optValues1 = opt1.getOptionValues();
        _OptUpdatedFlag[optName1] = true;
        if(optType1 =="dropdown")
        {
            QComboBox *pComB = dynamic_cast<QComboBox*>(_CurrentOptionHash.value(optName1));
            disconnect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
            if(pComB->count()>0)
            {
                pComB->clear();
            }
            if(isRelatedtoOtherOptions(optName1,optDefVal1))
            {
                CondCondOptions.append(optName1);
                continue;
            }
            connect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
            if(optValues1.count()>0)
            {
                pComB->addItems(optValues1);
            }

            if(optDefVal1.isEmpty()||optDefVal1.isNull())
            {
                if(pComB->count()>0)
                {
                    pComB->setCurrentIndex(0);
                }
            }
            else
            {
                int idx = pComB->findText(optDefVal1);
                if(idx>=0)
                    pComB->setCurrentIndex(idx);
                else
                    if(pComB->count()>0)
                    {
                        pComB->setCurrentIndex(0);
                    }
            }
        }
        if(optType1 =="freetext")
        {
            QTextEdit *pTE = dynamic_cast<QTextEdit*>(_CurrentOptionHash.value(optName1));
            disconnect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            pTE->clear();
            if(isRelatedtoOtherOptions(optName1,optDefVal1))
            {
                CondCondOptions.append(optName1);
                continue;
            }
            connect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            if(!(optDefVal1.isEmpty()||optDefVal1.isNull()))
            {
                pTE->setText(optDefVal1);
            }
            else
            {
                pTE->setText("Type Here");
                pTE->clear();
            }
        }
        if(optType1.toLower() =="yesno")
        {
            QCheckBox *pChkB = dynamic_cast<QCheckBox*>(_CurrentOptionHash.value(optName1));
            disconnect(pChkB,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            pChkB->setChecked(false);
            if(isRelatedtoOtherOptions(optName1,optDefVal1))
            {
                CondCondOptions.append(optName1);
                continue;
            }
            connect(pChkB,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            if(!(optDefVal1.isEmpty()||optDefVal1.isNull()))
            {
                if(optDefVal1.toLower()=="yes")
                {
                    pChkB->setChecked(true);
                }
                else
                {
                    pChkB->setChecked(true);
                    pChkB->setChecked(false);
                }

            }
        }
    }


    foreach(GTAOneClickOption opt1,CondOptionNames)
    {
        QString optName1 = opt1.getOptionName();
        QString optType1 = opt1.getOptionType();
        QString optDefVal1 = opt1.getOptionDefaultValue();
        QStringList optValues1 = opt1.getOptionValues();
        _OptUpdatedFlag[optName1] = true;
        if(CondCondOptions.contains(optName1))
        {
            if(optType1 =="dropdown")
            {
                QComboBox *pComB = dynamic_cast<QComboBox*>(_CurrentOptionHash.value(optName1));
                connect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
                if(optValues1.count()>0)
                {
                    pComB->addItems(optValues1);
                }

                if(optDefVal1.isEmpty()||optDefVal1.isNull())
                {
                    if(pComB->count()>0)
                    {
                        pComB->setCurrentIndex(0);
                    }
                }
                else
                {
                    int idx = pComB->findText(optDefVal1);
                    if(idx>=0)
                        pComB->setCurrentIndex(idx);
                    else
                        if(pComB->count()>0)
                        {
                            pComB->setCurrentIndex(0);
                        }
                }
            }
            if(optType1 =="freetext")
            {
                QTextEdit *pTE = dynamic_cast<QTextEdit*>(_CurrentOptionHash.value(optName1));
                connect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
                if(!(optDefVal1.isEmpty()||optDefVal1.isNull()))
                {
                    pTE->setText(optDefVal1);
                }
                else
                {
                    pTE->setText("Type Here");
                    pTE->clear();
                }
            }
            if(optType1 =="yesno")
            {
                QCheckBox *pChkB = dynamic_cast<QCheckBox*>(_CurrentOptionHash.value(optName1));
                connect(pChkB,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
                if(!(optDefVal1.isEmpty()||optDefVal1.isNull()))
                {
                    if(optDefVal1.toLower()=="yes")
                    {
                        pChkB->setChecked(true);
                    }
                    else
                    {
                        pChkB->setChecked(true);
                        pChkB->setChecked(false);
                    }

                }
            }

        }
    }



    foreach(QString naOpt,NAOptionNames)
    {

        QString optType3 = _Options.getOptionType(naOpt);
        QList<GTAOneClickOption> optNames3 = _Options.getOptionCondMap(naOpt);
        bool rc1 = false;
        foreach(GTAOneClickOption optName3,optNames3)
        {
            QString optCond3_Name;
            QString optCond3_Val;
            QString optCond3 =optName3.getOptionCondition();

            bool rc = decodeCondition(optCond3,optCond3_Name,optCond3_Val);
            QString optType4 = _Options.getOptionType(optCond3_Name);
            if(rc)
            {
                if(optType4 =="dropdown")
                {
                    QComboBox *pComB = dynamic_cast<QComboBox*>(_CurrentOptionHash.value(optCond3_Name));
                    if(pComB->count()>0)
                    {
                        if(pComB->currentText() == optCond3_Val)
                        {
                            rc1 = true;
                            break;
                        }
                    }
                }
                if(optType4 =="freetext")
                {
                    QTextEdit *pTE = dynamic_cast<QTextEdit*>(_CurrentOptionHash.value(optCond3_Name));
                    if(pTE->toPlainText() == optCond3_Val)
                    {
                        rc1 = true;
                        break;
                    }
                }
                if(optType4 =="yesno")
                {
                    QCheckBox *pChkB = dynamic_cast<QCheckBox*>(_CurrentOptionHash.value(optCond3_Name));
                    if((pChkB->isChecked() && (optCond3_Val == "yes"))||(!pChkB->isChecked() && (optCond3_Val == "no")))
                    {
                        rc1 = true;
                        break;
                    }
                }
            }
        }
        if(!rc1)
        {
            if(optType3 =="dropdown")
            {
                QComboBox *pComB = dynamic_cast<QComboBox*>(_CurrentOptionHash.value(naOpt));
                disconnect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
                if(pComB->count()>0)
                {
                    pComB->clear();
                }
                connect(pComB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
            }
            if(optType3 =="freetext")
            {
                QTextEdit *pTE = dynamic_cast<QTextEdit*>(_CurrentOptionHash.value(naOpt));
                disconnect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
                pTE->clear();
                connect(pTE,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            }
            if(optType3 =="yesno")
            {
                QCheckBox *pChkB = dynamic_cast<QCheckBox*>(_CurrentOptionHash.value(naOpt));
                disconnect(pChkB,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
                pChkB->setChecked(false);
                connect(pChkB,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            }
        }
    }

}

void GTAOneClickWidget::setInitialValues()
{
    foreach(QString optName,_CurrentOptionList)
    {
        QString optType = _Options.getOptionType(optName);
        QWidget * pItem = _CurrentOptionHash.value(optName);
        GTAOneClickOption defOpt = _Options.getDefOption(optName);

        QString val = defOpt.getOptionDefaultValue();

        if(_OptUpdatedFlag.value(optName) == true)
            continue;
        else
        {
            if(optType == "dropdown")
            {
                QComboBox *pComB = dynamic_cast<QComboBox *>(pItem);
                if((pComB!=NULL )&& (pComB->count()>0))
                {
                    if(pComB->count()>0)
                    {
                        int comIdx = pComB->findText(val);
                        if(comIdx>=0)
                            pComB->setCurrentIndex(comIdx);
                        else
                        {
                            pComB->setCurrentIndex(0);
                            val = pComB->currentText();
                        }
                    }

                }
            }
            if(optType == "yesno")
            {
                QCheckBox *pChkB = dynamic_cast<QCheckBox *>(pItem);
                if((pChkB!=NULL ))
                {
                    if(val.toLower() == "yes")
                    {
                        pChkB->setChecked(true);
                    }
                    else
                    {
                        pChkB->setChecked(false);
                    }
                }
            }
            if(optType == "freetext")
            {
                QTextEdit *pTE = dynamic_cast<QTextEdit *>(pItem);
                if(pTE!=NULL )
                    pTE->setText(val);
            }
        }
        if(!val.isEmpty() && !val.isNull())
            setOptionValues(optName,val);
    }
    foreach(QString optName,_CurrentOptionHash.keys())
    {
        _OptUpdatedFlag[optName]  =false;
    }
}

void GTAOneClickWidget::setDescription(const QString &iDescription)
{
    _Description = iDescription;
}

QString GTAOneClickWidget::getDescription()const
{
    return _Description;
}

void GTAOneClickWidget::ClearDynamicOptionWidgetItems()
{
    QWidget * pUiWid = NULL;
    foreach(QString keyItem, _CurrentOptionHash.keys())
    {
        QWidget *pItem = _CurrentOptionHash.value(keyItem);
        QString objName = pItem->objectName();
        if(objName.endsWith("_LEWid"))
        {
            disconnect(pItem,SIGNAL(textChanged()),this,SLOT(onOptionValueChanged()));
            pUiWid = DynamicOptionFL->findChild<QTextEdit *>(objName);
        }
        else if(objName.endsWith("_ComBWid"))
        {
            disconnect(pItem,SIGNAL(currentIndexChanged(QString)),this,SLOT(onOptionValueChanged(QString)));
            pUiWid = DynamicOptionFL->findChild<QComboBox *>(objName);
        }
        else if(objName.endsWith("_ChkBWid"))
        {
            disconnect(pItem,SIGNAL(clicked(bool)),this,SLOT(onOptionValueChanged(bool)));
            pUiWid = DynamicOptionFL->findChild<QCheckBox *>(objName);
        }
        else if(objName.endsWith("_TEWid"))
        {
            pUiWid = dynamic_cast<QTextEdit *>(pItem);
        }
        if(pUiWid != NULL)
        {
            DynamicOptionFL->removeWidget(pUiWid);
        }
        if(pItem != NULL)
        {
            delete pItem;
            pItem = NULL;
        }
    }
    _CurrentOptionHash.clear();
    foreach(QWidget * pLblItem, _CurrentOptionLabelList)
    {
        if(pUiWid != NULL)
        {
            DynamicOptionFL->removeWidget(pUiWid);
        }
        if(pLblItem != NULL)
        {
            delete pLblItem;
            pLblItem = NULL;
        }
    }
    _CurrentOptionLabelList.clear();
}
void GTAOneClickWidget::setAppName(const QString &iAppName)
{
    QListWidgetItem * pItem = new QListWidgetItem(iAppName);
    ui->ApplicationLB->addItem(pItem);
}
void GTAOneClickWidget::setTestRigName(const QString &iTestRigName)
{
    ui->TestRigNameLE->setText(iTestRigName);
}
void GTAOneClickWidget::setTestRigType(const QString &iTestRigType)
{
    ui->TestRigTypeValueLE->setText(iTestRigType);
}
void GTAOneClickWidget::setConfigName(const QString &iConfigName)
{
    ui->ConfigurationCB->addItem(iConfigName);
}
void GTAOneClickWidget::setConfFile(const QString &iConfFile)
{
    ui->ConfFileLE->setText(iConfFile);
}
QString GTAOneClickWidget::getConfFile()const
{
    return ui->ConfFileLE->text();
}
QString GTAOneClickWidget::getTestRigType()const
{
    return ui->TestRigTypeValueLE->text();
}
QString GTAOneClickWidget::getTestRigName()const
{
    return ui->TestRigNameLE->text();
}
QString GTAOneClickWidget::getConfigName()const
{
    return ui->ConfigurationCB->currentText();
}
QString GTAOneClickWidget::getAppName()const
{

    QListWidgetItem *pItem = ui->ApplicationLB->currentItem();
    if(pItem!=NULL)
    {
        return pItem->text();
    }
    return "";
}

GTAOneClickOptions GTAOneClickWidget::getOptions()const
{
    return _Options;
}

void GTAOneClickWidget::onSearchPBClicked()
{
    emit searchObject(ui->ConfFileLE);
}
bool GTAOneClickWidget::parseConfFile(QString &iConfFile)
{
    bool rc = false;
    GTAAppController * pAppCtrl = GTAAppController::getGTAAppController();
    _OneClickTestRig = new GTAOneClickTestRig();
    if(pAppCtrl != NULL)
    {
        rc = pAppCtrl->getOneClickConfigFileDetails(iConfFile,_OneClickTestRig);
    }
    return rc;
}
void GTAOneClickWidget::onConfFileChanged(QString iConfFile)
{
    bool rc = parseConfFile(iConfFile);
    QList<QString> configNames;
    if(rc)
    {
        if(_OneClickTestRig != NULL)
        {
            ui->ConfigurationCB->clear();
            setConfFile(iConfFile);
            QString TestRigName = _OneClickTestRig->getTestRigName();
            QString TestRigType = _OneClickTestRig->getTestRigType();
            setTestRigName(TestRigName);
            setTestRigType(TestRigType);
            QList<GTAOneClickConfiguration> configs = _OneClickTestRig->getConfigurations();
            for(int ConfIdx=0;ConfIdx<configs.count();ConfIdx++)
            {
                QString confName =configs.at(ConfIdx).getConfigurationName();
                if(!confName.isEmpty() && !confName.isNull() && !configNames.contains(confName))
                {
                    setConfigName(confName);
                    configNames.append(confName);
                }
            }
            ui->ConfigurationCB->setEnabled(true);
            ui->ApplicationLB->setEnabled(true);
        }
    }
}


void GTAOneClickWidget::onConfigNameChanged(QString iConfigName)
{
    if(_OneClickTestRig!= NULL)
    {
        QList<GTAOneClickConfiguration> configs = _OneClickTestRig->getConfigurations();
        GTAOneClickConfiguration config;
        for(int ConfIdx=0;ConfIdx<configs.count();ConfIdx++)
        {
            if(configs.at(ConfIdx).getConfigurationName() == iConfigName)
            {
                config =configs.at(ConfIdx);
                break;
            }
        }
        ui->ApplicationLB->clear();
        QList<GTAOneClickApplications> apps = config.getApplications();
        for(int appIdx=0;appIdx<apps.count();appIdx++)
        {
            QString appName = apps.at(appIdx).getAppName();
            QString Version = apps.at(appIdx).getAppVersion();
            if(!appName.isEmpty())
            {
                if(Version.isEmpty())
                {
                    Version = "nil";
                }
                QString app = QString("Version: %2").arg(Version);
                setAppName(app);
            }
        }
        if(ui->ApplicationLB->count()>=0)
        {
            ui->ApplicationLB->setCurrentRow(0);
        }
    }
}
void GTAOneClickWidget::clear()
{
    ui->ConfFileLE->clear();
    ui->ConfigurationCB->clear();
    ui->TestRigNameLE->clear();
    ui->TestRigTypeValueLE->clear();
    ui->ApplicationLB->clear();
    ClearDynamicOptionWidgetItems();
    ui->StartEnvCB->setChecked(true);
}


void GTAOneClickWidget::setActionCommand(const GTAActionBase * ipActionCmd)
{
    clear();
    GTAOneClick* pOneClickCmd = dynamic_cast<GTAOneClick*>((GTAActionBase * )ipActionCmd);
    if(pOneClickCmd!= NULL)
    {
        bool startEnv = pOneClickCmd->getIsStartEnv();
        bool stopTestConfig = pOneClickCmd->getIsStopTestConfig();
        bool stopAlto = pOneClickCmd->getIsStopAlto();
        ui->StartEnvCB->setChecked(startEnv);
        ui->StopAltoCB->setChecked(stopAlto);
        ui->StopTestConfigCB->setChecked(stopTestConfig);

        if(startEnv || stopAlto)
        {
            QString ConfigFile = pOneClickCmd->getConfFile();
            QString ConfigName = pOneClickCmd->getConfigName();
            QString TestRigName = pOneClickCmd->getTestRigName();
            QString TestRigType = pOneClickCmd->getTestRigType();
            QString AppName = pOneClickCmd->getApplicationName();
            QString description = pOneClickCmd->getDescription();
            QStringList optionNames = pOneClickCmd->getOptionNames();
            setConfFile(ConfigFile);
            int idx = ui->ConfigurationCB->findText(ConfigName);
            if(idx>=0)
            {
                ui->ConfigurationCB->setCurrentIndex(idx);
            }
            else
            {
                setConfigName(ConfigName);
                int idx1 = ui->ConfigurationCB->findText(ConfigName);
                ui->ConfigurationCB->setCurrentIndex(idx1);
            }
            setTestRigName(TestRigName);
            setTestRigType(TestRigType);
            if(ui->ApplicationLB->count()>0)
            {
                QList<QListWidgetItem*> Items = ui->ApplicationLB->findItems(AppName,Qt::MatchExactly);
                if(Items.count()>0)
                {
                    ui->ApplicationLB->setCurrentItem(Items.at(0));
                }
                else
                {
                    setAppName(AppName);
                }

            }
            else
            {
                setAppName(AppName);
            }
            QWidget * pUiItm = _CurrentOptionHash.value("Description");
            if(pUiItm!=NULL)
            {
                QTextEdit *pTEdes = dynamic_cast<QTextEdit*>(pUiItm);
                if(pTEdes!=NULL)
                {
                    pTEdes->setText(description);
                }
            }
            foreach(QString option,optionNames)
            {
                QStringList optionVal = pOneClickCmd->getValueForOption(option);
                QString optionType = pOneClickCmd->getTypeofOption(option);
                QStringList dirNames = pOneClickCmd->getDirNamesofOption(option);
                QString iniFileName = pOneClickCmd->getIniFileNameofOption(option);
                QHash<QString,bool> dirCheckStatus = pOneClickCmd->getDirChkStatusofOption(option);
                QWidget * pItem = _CurrentOptionHash.value(option);
                if(pItem !=NULL)
                {
                    if(optionType == "freetext")
                    {

                        QTextEdit *pTE = dynamic_cast<QTextEdit *>(pItem);
                        if(pTE !=NULL)
                        {
                            QString val;
                            if(optionVal.count()>0)
                            {
                                val = optionVal.at(0);
                            }
                            pTE->setText(val);
                        }
                    }
                    if(optionType == "dropdown")
                    {

                        QComboBox *pComB = dynamic_cast<QComboBox *>(pItem);
                        if(pComB !=NULL)
                        {
                            QString val;
                            if(optionVal.count()>0)
                            {
                                val = optionVal.at(0);
                            }
                            int idx = pComB->findText(val);
                            if(idx>=0)
                            {
                                pComB->setCurrentIndex(idx);
                            }
                            else
                            {

                                pComB->addItem(val);
                                int idx1 = pComB->findText(val);
                                pComB->setCurrentIndex(idx1);
                            }
                        }
                    }
                    if(optionType == "yesno")
                    {

                        QCheckBox *pCkhB = dynamic_cast<QCheckBox *>(pItem);
                        if(pCkhB !=NULL)
                        {
                            QString val;
                            if(optionVal.count()>0)
                            {
                                val = optionVal.at(0);
                            }
                            if(val == "yes")
                            {
                                pCkhB->setChecked(true);
                            }
                            else
                            {
                                pCkhB->setChecked(false);
                            }
                        }
                    }
                    if(optionType.toLower() == "selectfile")
                    {
                       
                        GTAOneClickDynamicDirOption *pDirOpt = dynamic_cast<GTAOneClickDynamicDirOption *>(pItem);
                        if(pDirOpt !=NULL)
                        {
                            pDirOpt->insertDirectories(dirNames,iniFileName);
                            pDirOpt->updateCheckedStatus(dirCheckStatus);
                        }
                    }
                }
            }
        }
    }
}

bool GTAOneClickWidget::isValid() const 
{
    return true;
}
bool GTAOneClickWidget::getActionCommand(GTAActionBase*& opCmd)const
{
    GTAOneClick * pOneClickCmd = NULL;
    opCmd=NULL;
    bool startEnv = ui->StartEnvCB->isChecked();
    bool stopTestConfig = ui->StopTestConfigCB->isChecked();
    bool stopAlto = ui->StopAltoCB->isChecked();
    if(startEnv || stopAlto)
    {
        QString ConfFile = getConfFile();
        QString ConfigName = getConfigName();
        QString AppName = getAppName();
        QString TestRigName = getTestRigName();
        QStringList Options = _Options.getAllOptions();
        QString TestRigType = getTestRigType();
        QString description = getDescription();

        if(ConfFile.isEmpty() ||  ConfigName.isEmpty())
            return false;
        if(AppName.isEmpty())
            return false;


        if(ConfFile.isEmpty()||ConfigName.isEmpty()||AppName.isEmpty())
        {
            return false;
        }
        pOneClickCmd = new GTAOneClick(QString(),QString());

        pOneClickCmd->setConfFile(ConfFile);
        pOneClickCmd->setConfigName(ConfigName);
        pOneClickCmd->setApplicationName(AppName);
        pOneClickCmd->setTestRigName(TestRigName);
        pOneClickCmd->setTestRigType(TestRigType);
        pOneClickCmd->setDescription(description);

        if(!(Options.count() >0))
        {
            delete pOneClickCmd;pOneClickCmd=NULL;
            return false;
        }
        foreach(QString OptionName, Options)
        {
            QString OptionType = _Options.getOptionType(OptionName);
            QString OptionValue;
            QWidget *pUIWid = _CurrentOptionHash.value(OptionName);
            if(OptionType=="freetext")
            {

                QTextEdit *pTE = dynamic_cast<QTextEdit *>(pUIWid);
                if(pTE != NULL)
                {
                    OptionValue =  pTE->toPlainText();
                }
            }
            else if(OptionType=="dropdown")
            {
                QComboBox *pComB = dynamic_cast<QComboBox *>(pUIWid);
                OptionValue =  pComB->currentText();

            }
            else if(OptionType=="yesno")
            {
                QCheckBox *pChkB = dynamic_cast<QCheckBox *>(pUIWid);
                if(pChkB->checkState() == Qt::Checked)
                    OptionValue =  "yes";
                else
                    OptionValue =  "no";
            }
            else if(OptionType.toLower()=="selectfile")
            {
                GTAOneClickDynamicDirOption *pDirOpt = dynamic_cast<GTAOneClickDynamicDirOption *>(pUIWid);
                QHash<QString,bool> dirChkStatus = pDirOpt->getCheckedDirectory();
                QString iniFile = pDirOpt->getIniFileName();
                pOneClickCmd->setDirChkStatusofOption(OptionName,dirChkStatus);
                pOneClickCmd->setDirNamesofOption(OptionName,dirChkStatus.keys());
                pOneClickCmd->setIniFileNameofOption(OptionName,iniFile);
            }
            if(!OptionName.isEmpty())
            {
                if((OptionValue.isEmpty() || OptionValue.isNull())&& (OptionType=="yesno"))
                {
                    OptionValue = "no";
                }
                pOneClickCmd->setOptionValueAndType(OptionName,QStringList(OptionValue),OptionType);
            }
        }
    }
    else
    {
        pOneClickCmd = new GTAOneClick(QString(),QString());
    }
    pOneClickCmd->setIsStartEnv(startEnv);
    pOneClickCmd->setIsStopTestConfig(stopTestConfig);
    pOneClickCmd->setIsStopAlto(stopAlto);

    opCmd = pOneClickCmd;
    return true;
}
void GTAOneClickWidget::validateText()
{
    QTextEdit *pUiItem = dynamic_cast<QTextEdit *>(sender());
    if(pUiItem != NULL)
    {
        GTAParamValidator txtValidatorObj;
        QString currentText = pUiItem->toPlainText();
        
        int txtSize = currentText.size();
        if (txtSize !=0 && ((txtValidatorObj.validate(currentText,txtSize) != QValidator::Acceptable) ||(currentText.endsWith("'") || currentText.endsWith("\""))))
        {
            QList<QString> invalidChars = txtValidatorObj.getInvalidChars(currentText);
            for(int i=0;i<invalidChars.count();i++)
            {
                QString invalidChar   = invalidChars.at(i);
                currentText = currentText.remove(invalidChar,Qt::CaseInsensitive);
            }
            currentText = currentText.remove("\"",Qt::CaseInsensitive);
            currentText = currentText.remove("\'",Qt::CaseInsensitive);
            currentText = currentText.replace("=",":",Qt::CaseInsensitive);

            pUiItem->setText(currentText);
            pUiItem->moveCursor(QTextCursor::End);
        }
    }
}

void GTAOneClickWidget::onStartEnvClicked(bool iVal)
{
    disconnect(ui->StartEnvCB,SIGNAL(toggled(bool)),this,SLOT(onStartEnvClicked(bool)));
    disconnect(ui->StopAltoCB,SIGNAL(toggled(bool)),this,SLOT(onStopAltoClicked(bool)));
    disconnect(ui->StopTestConfigCB,SIGNAL(toggled(bool)),this,SLOT(onStopTestConfig(bool)));
    ui->StopTestConfigCB->setChecked(!iVal);
    ui->StopAltoCB->setChecked(!iVal);
    ui->OneClickFrame->setVisible(true);
    connect(ui->StartEnvCB,SIGNAL(toggled(bool)),this,SLOT(onStartEnvClicked(bool)));
    connect(ui->StopAltoCB,SIGNAL(toggled(bool)),this,SLOT(onStopAltoClicked(bool)));
    connect(ui->StopTestConfigCB,SIGNAL(toggled(bool)),this,SLOT(onStopTestConfig(bool)));

}
void GTAOneClickWidget::onStopTestConfig(bool iVal)
{
    disconnect(ui->StartEnvCB,SIGNAL(toggled(bool)),this,SLOT(onStartEnvClicked(bool)));
    disconnect(ui->StopAltoCB,SIGNAL(toggled(bool)),this,SLOT(onStopAltoClicked(bool)));
    disconnect(ui->StopTestConfigCB,SIGNAL(toggled(bool)),this,SLOT(onStopTestConfig(bool)));
    ui->StartEnvCB->setChecked(false);
    if(!ui->StopAltoCB->isChecked())
    {
        ui->StartEnvCB->setChecked(!iVal);
        ui->OneClickFrame->setVisible(!iVal);
    }
    connect(ui->StartEnvCB,SIGNAL(toggled(bool)),this,SLOT(onStartEnvClicked(bool)));
    connect(ui->StopAltoCB,SIGNAL(toggled(bool)),this,SLOT(onStopAltoClicked(bool)));
    connect(ui->StopTestConfigCB,SIGNAL(toggled(bool)),this,SLOT(onStopTestConfig(bool)));
}
void GTAOneClickWidget::onStopAltoClicked(bool iVal)
{
    disconnect(ui->StartEnvCB,SIGNAL(toggled(bool)),this,SLOT(onStartEnvClicked(bool)));
    disconnect(ui->StopAltoCB,SIGNAL(toggled(bool)),this,SLOT(onStopAltoClicked(bool)));
    disconnect(ui->StopTestConfigCB,SIGNAL(toggled(bool)),this,SLOT(onStopTestConfig(bool)));
    ui->OneClickFrame->setVisible(iVal);
    if(!ui->StopTestConfigCB->isChecked())
    {
        ui->StartEnvCB->setChecked(!iVal);
        ui->OneClickFrame->setVisible(true);
    }
    connect(ui->StartEnvCB,SIGNAL(toggled(bool)),this,SLOT(onStartEnvClicked(bool)));
    connect(ui->StopAltoCB,SIGNAL(toggled(bool)),this,SLOT(onStopAltoClicked(bool)));
    connect(ui->StopTestConfigCB,SIGNAL(toggled(bool)),this,SLOT(onStopTestConfig(bool)));

}
int GTAOneClickWidget::getSearchType()
{
    int retVal = GTAGenSearchWidget::ONE_CLICK;
    return retVal;
}
#include "GTAWgtRegisterDef.h"
REGISTER_ACTION_WIDGET(QString("%1 %2").arg(ACT_ONECLICK,COM_ONECLICK_ENV),GTAOneClickWidget,obj)
