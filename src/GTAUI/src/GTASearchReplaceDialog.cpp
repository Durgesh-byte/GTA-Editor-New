#include "GTASearchReplaceDialog.h"
#include "ui_GTASearchReplaceDialog.h"
#include "GTACommandBuilder.h"
#include <QStringList>
#include <QMessageBox>
#include "GTAInitConfiguration.h"
#include "GTAUtil.h"

GTASearchReplaceDialog::GTASearchReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GTASearchReplaceDialog)
{

    ui->setupUi(this);
    connect(ui->FindNextPB,SIGNAL(clicked()),this,SLOT   (searchNext()));
    connect(ui->SearchNextPB2,SIGNAL(clicked()),this,SLOT   (searchNext()));
    connect(ui->FindAllPB1,SIGNAL(clicked()),this,SLOT   (searchNext()));
    connect(ui->FindInAllDocPB,SIGNAL(clicked()),this,SLOT(searchNext()));
    //    connect(ui->FindAllPB2,SIGNAL(clicked()),this,SLOT   (searchNext()));
    connect(ui->FindLE1,SIGNAL(textChanged(const QString&)),this,SLOT   (enableSearch(const QString&)));
    connect(ui->FindLE2,SIGNAL(textChanged(const QString&)),this,SLOT   (enableSearchReplace(const QString&)));
    connect(ui->ReplaceLE2,SIGNAL(textChanged(const QString&)),this,SLOT   (enableSearchReplace(const QString&)));
    connect(ui->ReplacePB,SIGNAL(clicked()),this,SLOT   (replaceSearchNext()));
    connect(ui->ReplaceAllPB,SIGNAL(clicked()),this,SLOT (replaceAll()));
    connect(ui->ReplaceInAllDocsPB,SIGNAL(clicked()),this,SLOT(replaceAll()));
    //    ui->SearchPB1->hide();
    //    ui->searchPB2->hide();
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(replaceAllEquipment()));
    connect(ui->Search_requirement, SIGNAL(clicked()), this, SLOT(searchRequirement()));
    connect(ui->Search_tag, SIGNAL(clicked()), this, SLOT(searchTag()));
    connect(ui->Search_comment, SIGNAL(clicked()), this, SLOT(searchComment()));
}

GTASearchReplaceDialog::~GTASearchReplaceDialog()
{
    delete ui;
}
void GTASearchReplaceDialog::keyPressEvent ( QKeyEvent * event )
{
    if (event->key()==Qt::Key_Escape )
    {
        this->hide();
    }
    else if (event->key() == Qt::Key_F3)
    {
        searchNext();
    }
    else if (event->key() == Qt::Key_Return)
    {
        searchNext();
    }
    else if (event->key() == Qt::Key_Enter)
    {
        searchNext();
    }
    QWidget::keyPressEvent(event);
}
void GTASearchReplaceDialog::searchNext()
{
    QString searchString = ui->FindLE1->text();
    
    bool bSearchUp= false;
    bool bMatchCase= false;
    bool bUseRegExp= false;
    bool bTextSearch = false;

    int pageIndex = ui->tabWidget->currentIndex();

    switch(pageIndex)
    {
    case 0 :

        if (ui->SearchUpCB1->isChecked())
            bSearchUp = true;
        if(ui->MatchCaseCB1->isChecked())
            bMatchCase = true;
        if(ui->RegExCB1->isChecked())
            bUseRegExp = true;
        if (ui->textSearchRB->isChecked()==true)
            bTextSearch = true;
        break;

    case 1:
        searchString = ui->FindLE2->text();
        if (ui->SearchUpCB2->isChecked())
            bSearchUp = true;
        if(ui->MatchCaseCB2->isChecked())
            bMatchCase = true;
        if(ui->RegExpCB2->isChecked())
            bUseRegExp = true;
        bTextSearch=false;
    }

    if (!bUseRegExp)
    {
        searchString.replace("^","\\^");
        searchString.replace("*","\\*");
        searchString.replace("$","\\$");

    }

    Qt::CaseSensitivity caseSensitivityV = Qt::CaseInsensitive;
    if(bMatchCase)
        caseSensitivityV = Qt::CaseSensitive;
    QRegExp strExp(searchString,caseSensitivityV);


    if (!searchString.isEmpty())
    {
        QObject* pSender = sender();
        if(pSender == ui->FindNextPB )
            emit searchNext(strExp,bSearchUp,bTextSearch);
        else if (pSender == ui->SearchNextPB2)
            emit searchNext(strExp,bSearchUp,false);
        else if(pSender == ui->FindAllPB1 )
            emit searchHighlightAllString(strExp,bTextSearch);
        else if(pSender == ui->FindInAllDocPB)
            emit searchInAllDocuments(strExp,bTextSearch);
        //        else if(pSender == ui->FindAllPB2)
        //           emit searchHighlightAllString(strExp,false);
        else
            emit searchNext(strExp,bSearchUp,bTextSearch);

    }
    if(pageIndex)
        ui->FindLE2->setFocus();
    else
        ui->FindLE1->setFocus();
}
void GTASearchReplaceDialog::closeEvent ( QCloseEvent * e )
{   
    _lastSelectedRows.clear();
    emit dlgCloseEvent();
    QDialog::closeEvent(e);
}
void GTASearchReplaceDialog::hideEvent ( QHideEvent * event)
{   
    _lastSelectedRows.clear();
    emit dlgCloseEvent();
    QDialog::hideEvent(event);
}
void GTASearchReplaceDialog::enableSearchReplace ( const QString& iString )
{
    QString ipStr = iString;
    ipStr = ipStr.trimmed();
    if (ipStr.isEmpty())
    {

        QObject* pSender = sender();
        if(pSender == ui->FindLE2)
        {
            ui->SearchNextPB2->setEnabled(false);
            //        ui->FindAllPB2->setEnabled(false);
        }

        ui->ReplaceInAllDocsPB->setEnabled(false);
        ui->ReplaceAllPB->setEnabled(false);
        ui->ReplacePB->setEnabled(false);
    }
    else
    {

        ui->SearchNextPB2->setEnabled(true);
        //       ui->FindAllPB2->setEnabled(true);
        QString ipReplaceString = ui->ReplaceLE2->text();
        ipReplaceString.trimmed();
        if (ipReplaceString.isEmpty())
        {
            ui->ReplaceInAllDocsPB->setEnabled(false);
            ui->ReplaceAllPB->setEnabled(false);
            ui->ReplacePB->setEnabled(false);
        }
        else
        {
            ui->ReplaceInAllDocsPB->setEnabled(true);
            ui->ReplaceAllPB->setEnabled(true);
            ui->ReplacePB->setEnabled(true);
        }

    }
}
void GTASearchReplaceDialog::enableSearch ( const QString& iString )
{
    QString ipStr = iString;
    ipStr = ipStr.trimmed();
    if (ipStr.isEmpty())
    {
        ui->FindNextPB->setEnabled(false);
        ui->FindAllPB1->setEnabled(false);
        ui->FindInAllDocPB->setEnabled(false);
        
    }
    else
    {
        ui->FindNextPB->setEnabled(true);
        ui->FindAllPB1->setEnabled(true);
        ui->FindInAllDocPB->setEnabled(true);
        
    }
}
void GTASearchReplaceDialog::showEvent ( QShowEvent * event )
{
    _lastSelectedRows.clear();
    ui->FindLE1->setFocus();
    QDialog::showEvent(event);
}
void GTASearchReplaceDialog::replaceSearchNext()
{
    QString strngToReplace = ui->FindLE2->text();
    QString sReplaceWith = ui->ReplaceLE2->text();
    if (!strngToReplace.isEmpty() && !sReplaceWith.isEmpty())
    {
        bool bSearchUp= false;
        bool bMatchCase= false;
        bool bUseRegExp= false;

        if (ui->SearchUpCB2->isChecked())
            bSearchUp = true;
        if(ui->MatchCaseCB2->isChecked())
            bMatchCase = true;
        if(ui->RegExpCB2->isChecked())
            bUseRegExp = true;

        if (!bUseRegExp)
        {
            strngToReplace.replace("^","\\^");
            strngToReplace.replace("*","\\*");
            strngToReplace.replace("$","\\$");

        }


        Qt::CaseSensitivity caseSensitivityV = Qt::CaseInsensitive;
        if(bMatchCase)
            caseSensitivityV = Qt::CaseSensitive;
        QRegExp strExp(strngToReplace,caseSensitivityV);

        emit replaceSearchNext(strExp,sReplaceWith,bSearchUp);
    }
    ui->FindLE2->setFocus();

}
void GTASearchReplaceDialog::replaceAll()
{
    bool bMatchCase= false;
    bool bUseRegExp= false;

    if(ui->MatchCaseCB2->isChecked())
        bMatchCase = true;
    if(ui->RegExpCB2->isChecked())
        bUseRegExp = true;

    QString strngToReplace = ui->FindLE2->text();
    QString sReplaceWith = ui->ReplaceLE2->text();

    if (!bUseRegExp)
    {
        strngToReplace.replace("^","\\^");
        strngToReplace.replace("*","\\*");
        strngToReplace.replace("$","\\$");

    }


    Qt::CaseSensitivity caseSensitivityV = Qt::CaseInsensitive;
    if(bMatchCase)
        caseSensitivityV = Qt::CaseSensitive;

    QRegExp strExp(strngToReplace,caseSensitivityV);

    if(sender() == ui->ReplaceAllPB)
        emit replaceAll(strExp,sReplaceWith);
    else if(sender() == ui->ReplaceInAllDocsPB)
        emit replaceInAllDocuments(strExp,sReplaceWith);
    ui->FindLE2->setFocus();

}

void GTASearchReplaceDialog::replaceAllEquipment()
{

    QString strngToReplace = ui->lineEdit->text();
    QString sReplaceWith = ui->lineEdit_2->text();

    QRegExp strExp(strngToReplace, Qt::CaseInsensitive);

    if (!strngToReplace.isEmpty() && !sReplaceWith.isEmpty())
    {
        emit replaceAllEquipment(strExp, sReplaceWith);
    }
    ui->FindLE2->setFocus();

}

void GTASearchReplaceDialog::searchRequirement()
{
    QString searchString = ui->lineEdit1->text();
    Qt::CaseSensitivity caseSensitivityV = Qt::CaseInsensitive;
    QRegExp strExp(searchString, caseSensitivityV, QRegExp::FixedString);

    if (!searchString.isEmpty())
    {
        emit searchRequirement(strExp);
    }
    ui->lineEdit1->setFocus();
}

void GTASearchReplaceDialog::searchTag()
{
    QString searchString = ui->lineEdit1->text();
    Qt::CaseSensitivity caseSensitivityV = Qt::CaseInsensitive;
    QRegExp strExp(searchString, caseSensitivityV, QRegExp::FixedString);

    if (!searchString.isEmpty())
    {
        emit searchTag(strExp);
    }
    ui->lineEdit1->setFocus();

}

void GTASearchReplaceDialog::searchComment()
{
    QString searchString = ui->lineEdit1->text();
    Qt::CaseSensitivity caseSensitivityV = Qt::CaseInsensitive;
    QRegExp strExp(searchString, caseSensitivityV, QRegExp::FixedString);

    if (!searchString.isEmpty())
    {
        emit searchComment(strExp);
    }
    ui->lineEdit1->setFocus();
}

void  GTASearchReplaceDialog::setDefaultTab()
{
    ui->tabWidget->setCurrentIndex(0);
}
