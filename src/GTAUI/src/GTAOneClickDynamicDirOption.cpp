#include "GTAOneClickDynamicDirOption.h"
#include "ui_GTAOneClickDynamicDirOption.h"
#include <QFileDialog>
#include <QSettings>

GTAOneClickDynamicDirOption::GTAOneClickDynamicDirOption(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTAOneClickDynamicDirOption)
{
    ui->setupUi(this);
    connect(ui->iniBrowsePB,SIGNAL(clicked()),this,SLOT(onIniBrowsePbClicked()));
    ui->iniPathLE->setReadOnly(true);
    _IniFileName = "";
}

void GTAOneClickDynamicDirOption::insertDirectories(QStringList dirList, const QString & iIniFilePath)
{
    foreach(QString dirName,dirList)
    {
        QCheckBox *pChkBox = new QCheckBox(dirName);
        pChkBox->setObjectName(dirName+"_ChkBx");
        pChkBox->setChecked(false);
        ui->DirChkboxLayout->addWidget(pChkBox);
        _OptionChkList.append(pChkBox);
    }
    if(!iIniFilePath.isEmpty())
    {
        ui->iniPathLE->setText(iIniFilePath);
        parseINIFile(iIniFilePath);
    }
    
}

void GTAOneClickDynamicDirOption::insertDirectories(QStringList dirList,QHash<QString,bool> dirCheckState)
{
    foreach(QString dirName,dirList)
    {
        QCheckBox *pChkBox = new QCheckBox(dirName,this);
        pChkBox->setObjectName(dirName+"_ChkBx");
        bool ischecked = false;
        if(dirCheckState.contains(dirName))
            ischecked = dirCheckState.value(dirName);
        pChkBox->setChecked(ischecked);
        ui->DirChkboxLayout->addWidget(pChkBox);
        _OptionChkList.append(pChkBox);
    }
}
void GTAOneClickDynamicDirOption::updateCheckedStatus(QHash<QString,bool> dirCheckState)
{
    foreach(QWidget * child,_OptionChkList)
    {
        if(child != NULL)
        {
            if(child->objectName().endsWith("_ChkBx"))
            {
                QCheckBox *pChkBx = dynamic_cast<QCheckBox *>(child);
                if(pChkBx!= NULL)
                {
                    QString dirName = pChkBx->text();
                    if(dirCheckState.contains(dirName))
                        pChkBx->setChecked(dirCheckState.value(dirName));
                    else
                        pChkBx->setChecked(false);

                }
            }

        }
    }
}

QHash<QString,bool> GTAOneClickDynamicDirOption::getCheckedDirectory()
{
    QHash<QString,bool> checkedDirNames;
    foreach(QWidget * child,_OptionChkList)
    {
        if(child != NULL)
        {
            if(child->objectName().endsWith("_ChkBx"))
            {
                QCheckBox *pChkBx = dynamic_cast<QCheckBox *>(child);
                if(pChkBx!= NULL)
                {
                     checkedDirNames.insert(pChkBx->text(),pChkBx->isChecked());
                }
            }
        }
    }
    return checkedDirNames;
}

void GTAOneClickDynamicDirOption::onIniBrowsePbClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"Select INI file",QDir::currentPath(),"*.ini");
    ui->iniPathLE->setText(filePath);
    parseINIFile(filePath);
}

GTAOneClickDynamicDirOption::~GTAOneClickDynamicDirOption()
{
    delete ui;
}
void GTAOneClickDynamicDirOption::parseINIFile(const QString & iIniFilePath)
{
    _IniFileName = iIniFilePath;
    QHash<QString,bool> dirChkMap;
    QSettings settings(iIniFilePath,QSettings::IniFormat);
    settings.beginGroup("PATH");
    QStringList keys = settings.allKeys();
    foreach(QString key,keys)
    {
        if(!dirChkMap.contains(key))
        {
            QString val = settings.value(key).toString();
            if(val.toInt() == 0)
                dirChkMap.insert(key,false);
            else if(val.toInt() == 1)
                dirChkMap.insert(key,true);
            else
                dirChkMap.insert(key,false);
        }
    }
    settings.endGroup();
    updateCheckedStatus(dirChkMap);
}