#include "GTACommentWidget.h"
#include "ui_GTACommentWidget.h"
#include <QTextEdit>
#include <QStringList>
#include <QMessageBox>
#include "GTACommentImageWidget.h"
#include "GTACommandBuilder.h"
#include "GTAGenSearchWidget.h"
#include "GTAComment.h"
#include "GTACommandVariant.h"
#include "GTAICDParameter.h"

#define TITLESTR "title"
#define COMMENTSTR "comment"
#define PICTURESTR "picture"

GTACommentWidget::GTACommentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GTACommentWidget)
{
    _pCommentValueTE = NULL;
    _pImageWidget = NULL;
    _pSearchWidget = NULL;
    _pCommentCommand = NULL;
    ui->setupUi(this);

    QStringList commentTypeList; commentTypeList<<TITLESTR<<COMMENTSTR<<PICTURESTR;

    ui->CommentTypeCB->addItems(commentTypeList);
    ui->CommentTypeCB->setCurrentIndex(0);

    _pImageWidget = new GTACommentImageWidget();
    _pCommentValueTE = new QTextEdit();

    ui->CommentStackSW->addWidget(_pCommentValueTE);
    ui->CommentStackSW->addWidget(_pImageWidget);

    ui->CommentStackSW->setCurrentWidget(_pCommentValueTE);

    _pSearchWidget = new GTAGenSearchWidget();
    _pSearchWidget->hide();

    connect(ui->CommentTypeCB,SIGNAL(currentIndexChanged(QString)),this,SLOT(onCommentTypeChange(QString)));
    connect(ui->OKPB,SIGNAL(clicked()),this,SLOT(onOKPBClick()));
    connect(ui->CancelPB,SIGNAL(clicked()),this,SLOT(hide()));
    connect(_pImageWidget,SIGNAL(searchImage(QLineEdit*&)),this,SLOT(onSearchPBClick(QLineEdit*&)));
    connect(_pSearchWidget,SIGNAL(okPressed()),this,SLOT(onSearchOKPBClick()));

}

GTACommentWidget::~GTACommentWidget()
{
    if(_pCommentValueTE != NULL)
        delete _pCommentValueTE;
    if(_pImageWidget != NULL)
        delete _pImageWidget;
    if(_pSearchWidget != NULL)
        delete _pSearchWidget;

    delete ui;
}

void GTACommentWidget::onCommentTypeChange(const QString & iCommentType)
{
    if(iCommentType == TITLESTR || iCommentType == COMMENTSTR)
    {
        ui->CommentStackSW->setCurrentWidget(_pCommentValueTE);
    }
    else
        ui->CommentStackSW->setCurrentWidget(_pImageWidget);

}

void GTACommentWidget::onOKPBClick()
{
    //if(_pCommentCommand != NULL)
    //{
    //    delete _pCommentCommand;
    //    _pCommentCommand = NULL;
    //}

    QString commentValue;
    GTAComment::CommentType CommType;
    bool compileError = false;
    QString iCommentType = ui->CommentTypeCB->currentText();
    if(iCommentType == TITLESTR )
    {
        CommType = GTAComment::TITLE;
        commentValue = _pCommentValueTE->toPlainText();
        if(commentValue.isEmpty())
            compileError = true;
    }
    else if(iCommentType == COMMENTSTR)
    {
        CommType = GTAComment::COMMENT;
        commentValue = _pCommentValueTE->toPlainText();
        if(commentValue.isEmpty())
            compileError = true;

    }
    else
    {
        CommType = GTAComment::PICTURE;
        commentValue = _pImageWidget->getImageFileName();
        if(commentValue.isEmpty())
            compileError = true;
    }

    if(compileError)
    {
        QString errMsg = "Provide values for all the fields";
        if (this->isVisible())
            QMessageBox::critical(0,"Compilation Error",errMsg,QMessageBox::Ok);
    }
    else
    {
        _pCommentCommand =  new GTAComment();
        _pCommentCommand->setCommentType(CommType);
        _pCommentCommand->setValue(commentValue);

        GTACommandVariant variant;
        variant.setCommand(_pCommentCommand);
        _commentCmdVariant.setValue(variant);

        hide();
        emit okPressed();
    }
}
void GTACommentWidget::onSearchPBClick(QLineEdit *&ipItemToUpdate)
{
    _pCurrentSearchResult = ipItemToUpdate;
    _pSearchWidget->setSearchType(GTAGenSearchWidget::IMAGE);
    _pSearchWidget->show();
}
void GTACommentWidget::onSearchOKPBClick()
{
    GTAICDParameter selectedItem = _pSearchWidget->getSelectedItems();
    if (!selectedItem.getName().isEmpty())
       _pCurrentSearchResult->setText(selectedItem.getName());
}
void GTACommentWidget::setCommentCommand(const GTAComment * ipCommentCmd)
{
    if(ipCommentCmd != NULL)
    {
        GTAComment * pCommentCmd = (GTAComment*)ipCommentCmd;
        _pCommentCommand=pCommentCmd;
        GTAComment::CommentType commentType =  pCommentCmd->getCommentType();
        QString commentVal = pCommentCmd->getValue();
        switch(commentType)
        {
        case GTAComment::COMMENT : _pCommentValueTE->setText(commentVal); break;
        case GTAComment::PICTURE : _pImageWidget->setImageFileName(commentVal); break;
        case GTAComment::TITLE : _pCommentValueTE->setText(commentVal); break;
        default: break;
        }


    }
    else
        _pCommentCommand=NULL;
}

GTAComment * GTACommentWidget::getCommentCommand()
{
    return _pCommentCommand;
}
QVariant GTACommentWidget::getCommentVariant()
{
    //for testing todo:remove
    /*GTACommandVariant valCmdVariant = _commentCmdVariant.value<GTACommandVariant>();
    GTACommandBase* pBase = valCmdVariant.getCommand();*/
    ///////////////////////////////////////////////////////////////////////////

    return _commentCmdVariant;
}
