#include "AINGTALiveModeParameterSave.h"
#include "ui_AINGTALiveModeParameterSave.h"
#include <QMessageBox>

AINGTALiveModeParameterSave::AINGTALiveModeParameterSave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AINGTALiveModeParameterSave)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    ui->clearTB->setAutoRaise(true);
    ui->saveTB->setAutoRaise(true);
    ui->oKTB->setAutoRaise(true);
    connect(ui->oKTB, &QToolButton::clicked,this,&AINGTALiveModeParameterSave::onOKTBClicked);
    connect(ui->clearTB, &QToolButton::clicked,ui->fileNameLE,&QLineEdit::clear);
    connect(ui->saveTB, &QToolButton::clicked,this,&AINGTALiveModeParameterSave::onSavePBClicked);
    connect(ui->fileNameLE, &QLineEdit::textChanged,this,&AINGTALiveModeParameterSave::onFileNameTextChanged);
}

AINGTALiveModeParameterSave::~AINGTALiveModeParameterSave()
{
    delete ui;
}

void AINGTALiveModeParameterSave::showWidget(const bool &iIsImport, const QStringList &iFiles)
{
    if(iFiles.count() > 0)
    {
        ui->filesLW->addItems(iFiles);
        ui->filesLW->setVisible(true);
    }
    else
        ui->filesLW->setVisible(false);

    ui->fileNameLE->setVisible(!iIsImport);
    ui->clearTB->setVisible(!iIsImport);
    ui->saveTB->setVisible(!iIsImport);
    ui->oKTB->setVisible(iIsImport);
    ui->saveTB->setEnabled(false);
    if(iIsImport)
    {
        setWindowTitle("Import Parameters");
    }
    else
    {
        setWindowTitle("Save Parameters");
    }
    show();

}

void AINGTALiveModeParameterSave::onSavePBClicked()
{
    QString fileName = ui->fileNameLE->text();
    for(int i=0; i<ui->filesLW->count() ; i++)
    {
        if(fileName.compare(ui->filesLW->item(i)->text()) == 0)
        {
            QMessageBox msgBox (this);
            msgBox.setText("File already exists. Replace it?");
            msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("File Save Error");msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int reply = msgBox.exec();
            if (reply != QMessageBox::Yes)
            {
                return;
            }
            break;
        }
    }
    emit saveParameterFile(fileName);
    close();

}

void AINGTALiveModeParameterSave::onOKTBClicked()
{
    if(ui->filesLW->selectedItems().count()>0)
    {
        QStringList fileNames;
        QList<QListWidgetItem *> selectedItems = ui->filesLW->selectedItems();
        foreach(QListWidgetItem * item, selectedItems)
        {
            fileNames.append(item->text());
        }
        emit importFiles(fileNames);
        close();
    }
}

void AINGTALiveModeParameterSave::onFileNameTextChanged(const QString &text)
{
    if(text.length() > 0)
        ui->saveTB->setEnabled(true);
    else
        ui->saveTB->setEnabled(false);
}
