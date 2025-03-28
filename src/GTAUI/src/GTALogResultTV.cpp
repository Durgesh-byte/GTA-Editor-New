#include "GTALogResultTV.h"

#include <QTextStream>
#include <QMessageBox>
#include <QLabel>
#include <QFile>
#include <QFile>
#include <QDir>

#include "GTACommandBase.h"
#include "GTAActionBase.h"
#include "GTAActionIRT.h"
#include "GTAUtil.h"
#include "GTAFilterModel.h"
#include "GTAImageDisplay.h"


#include "QHeaderView"

#define ECEX_TIME_COL_NO 11
#define RESULT_PAGE_UTC_COL_NO 12


GTALogResultTV::GTALogResultTV(QWidget * parent):QTreeView(parent)
{
    _RowEditor = new GTALogResultTVRowEditor();
    connect(_RowEditor,SIGNAL(rowDataChanged()),this,SLOT(onRowDataChanged()));
    _iModel = NULL;
    this->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->header(),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayHeaderContextMenu(QPoint)));

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayContextMenu(QPoint)));

    _pContextMenu = new QMenu;
    createHeaderContextmenu();

    _pContextMenuTV = new QMenu(this);
     populateContextMenu();
}


GTALogResultTV::~GTALogResultTV()
{
    if (_pContextMenu !=NULL)
    {
        delete _pContextMenu;
        _pContextMenu = NULL;
    }
    if (_RowEditor !=NULL)
    {
        delete _RowEditor;
        _RowEditor = NULL;
    }
}

void GTALogResultTV::createHeaderContextmenu()
{
    //changing the sequence of the actions is not recommended
    //actions are arranged as sequenced in GTAEditorLogModel
    if (_pContextMenu!=NULL)
    {
        Action = _pContextMenu->addAction(QIcon(),"Action",this,SLOT(showHideCol()));
        ActionOnFail = _pContextMenu->addAction(QIcon(),"Action On Fail",this,SLOT(showHideCol()));
        TimeOut = _pContextMenu->addAction(QIcon(),"Time Out",this,SLOT(showHideCol()));
        Precision = _pContextMenu->addAction(QIcon(),"Precision",this,SLOT(showHideCol()));
        Comment = _pContextMenu->addAction(QIcon(),"Comment",this,SLOT(showHideCol()));
        Result = _pContextMenu->addAction(QIcon(),"Result",this,SLOT(showHideCol()));
        CurrentValue = _pContextMenu->addAction(QIcon(),"Current Value",this,SLOT(showHideCol()));
        ExpectedValue = _pContextMenu->addAction(QIcon(),"Expected Value",this,SLOT(showHideCol()));
        DumpList = _pContextMenu->addAction(QIcon(),"Dump List",this,SLOT(showHideCol()));
        Defects = _pContextMenu->addAction(QIcon(),"Defects",this,SLOT(showHideCol()));
        UserFeedback = _pContextMenu->addAction(QIcon(),"User Feedback",this,SLOT(showHideCol()));
        ExecutionTime = _pContextMenu->addAction(QIcon(),"Execution Time",this,SLOT(showHideCol()));

        Action->setCheckable(true);         Action->setChecked(true);
        Action->setDisabled(true);          Action->setToolTip("Always Enabled");

        std::string resultListStr = TestConfig::getInstance()->getResultViewSavedList();
        QStringList ResultList = QString::fromStdString(resultListStr).split(":");

        //keeping everything as unselected to select appropriately from user saved value
        Precision->setCheckable(true);Comment->setCheckable(true);Result->setCheckable(true);CurrentValue->setCheckable(true);
        ExpectedValue->setCheckable(true);DumpList->setCheckable(true);Defects->setCheckable(true);UserFeedback->setCheckable(true);
        TimeOut->setCheckable(true);ExecutionTime->setCheckable(true);ActionOnFail->setCheckable(true);

        ActionOnFail->setChecked(false);TimeOut->setChecked(false);Precision->setChecked(false);Comment->setChecked(false);
        Result->setChecked(false);CurrentValue->setChecked(false);ExpectedValue->setChecked(false);DumpList->setChecked(false);        
        Defects->setChecked(false);UserFeedback->setChecked(false);ExecutionTime->setChecked(false);

		

        if (ResultList.contains("Action On Fail"))
        {
            ActionOnFail->setChecked(true);
            columns.append("Action On Fail");
        }
        if (ResultList.contains("Time Out"))
        {
            TimeOut->setChecked(true);
            columns.append("Time Out");
        }
        if (ResultList.contains("Precision"))
        {
            Precision->setChecked(true);
            columns.append("Precision");
        }
        if (ResultList.contains("Comment"))
        {
            Comment->setChecked(true);
            columns.append("Comment");
        }
        if (ResultList.contains("Result"))
        {
            Result->setChecked(true);
            columns.append("Result");
        }
        if (ResultList.contains("Current Value"))
        {
            CurrentValue->setChecked(true);
            columns.append("Current Value");
        }
        if (ResultList.contains("Expected Value"))
        {
            ExpectedValue->setChecked(true);
            columns.append("Expected Value");
        }
        if (ResultList.contains("Dump List"))
        {
            DumpList->setChecked(true);
            columns.append("Dump List");
        }
        if (ResultList.contains("Defects"))
        {
            Defects->setChecked(true);
            columns.append("Defects");
        }
        if (ResultList.contains("User Feedback"))
        {
            UserFeedback->setChecked(true);
            columns.append("User Feedback");
        }
        if (ResultList.contains("Execution Time"))
        {
            ExecutionTime->setChecked(true);
            columns.append("Execution Time");
        }

        actions = _pContextMenu->actions();
    }
}

void GTALogResultTV::setdefaults()
{
    // the actions are arranged according to their column numbers
    for (int i=0;i<actions.count();i++)
    {
        setColumnHidden(i,!(actions.at(i)->isChecked()));
        resizeColumnToContents(i);
    }
}


void GTALogResultTV::showHideCol()
{

    QObject *pSender = sender();
    QAction *pAction = dynamic_cast<QAction*>(pSender);

    QString actionText = pAction->text();
    int index = 12;

    /*if (actionText == "Action")
        index = 0;*/
    if (actionText == "Action On Fail")
        index =1;
    else if (actionText == "Time Out")
        index = 2;
    else if (actionText == "Precision")
        index = 3;
    else if (actionText == "Comment")
        index = 4;
    else if (actionText == "Result")
        index = 5;
    else if (actionText == "Current Value")
        index = 6;
    else if (actionText == "Expected Value")
        index = 7;
    else if (actionText == "Dump List")
        index = 8;
    else if (actionText == "Defects")
        index = 9;
    else if (actionText == "User Feedback")
        index = 10;
    else if (actionText == "Execution Time")
        index = 11;

    if (pAction!=NULL)
    {
        saveSelectionSettings(index,actionText,pAction->isChecked());

        if (index!=12)
        {
            setColumnHidden(index,!(pAction->isChecked()));
            resizeColumnToContents(index);
        }
    }

}

void GTALogResultTV::saveSelectionSettings(int index, QString actionText, bool isChecked)
{
    if (index != 12)
    {
        if (columns.contains(actionText) && !isChecked)
        {
            columns.removeAll(actionText);
        }
        else
        {
            columns.append(actionText);
        }

        QString columnsStr;
        for (int i = 0; i < columns.count(); i++)
            columnsStr.append(QString("%1:").arg(columns.at(i)));
        TestConfig::getInstance()->setResultViewSavedList(columnsStr.toStdString());
    }
}


void GTALogResultTV::populateContextMenu()
{
    if(_pContextMenuTV != NULL)
    {
        _pActionDisplayDelta = _pContextMenuTV->addAction(QIcon(),"Display Delta",this,SLOT(onDisplayDelta()));

    }
}

void GTALogResultTV::onDisplayDelta()
{
    QModelIndexList indexList = this->selectionModel()->selectedRows(RESULT_PAGE_UTC_COL_NO);
    if(indexList.count() == 2)
    {
        QModelIndex index1 = indexList.first();
        QModelIndex index2 = indexList.last();

        QAbstractItemModel * pAbstractModel =  this->model();
        if(pAbstractModel)
        {
            double dTime1 = pAbstractModel->data(index1,Qt::DisplayRole).toDouble();
            double dTime2 = pAbstractModel->data(index2,Qt::DisplayRole).toDouble();
            QString diff = getTimeDifferrence(dTime1,dTime2);

            ErrorMessageList msgList;
            ErrorMessage logMessage;
            logMessage.description = QString("Time difference between the two selected commands is %1").arg(diff);
            logMessage.errorType = ErrorMessage::ErrorType::kNa;
            logMessage.source = ErrorMessage::MsgSource::kResult;
            msgList.append(logMessage);
            emit UpdateErrorLog(msgList);

        }

    }
}

//This function will compute the time differnece between two UTC and return the differnece in the form of days::hrs::mins::sec::msecs
QString GTALogResultTV::getTimeDifferrence(const double &iTime1,const double &iTime2)
{
    QString timeDiff("0");
    double dTimeDiff = iTime2 - iTime1;
    if(dTimeDiff > 0)
    {
        qint64 time_spent = dTimeDiff;
        qDebug() << "time difference is \n";
        qint64 msec = time_spent%1000;
        qint64 sec = (time_spent/1000) % 60;
        qint64 min = (time_spent/(60*1000)) % 60;
        qint64 hr = (time_spent/(60*60*1000)) % 24;
        qint64 days = (time_spent/(60*60*1000*24));

        timeDiff = QString("%1day(s) %2hr(s) %3min(s) %4sec(s) %5msec(s)").
                arg(QString::number(days),QString::number(hr),QString::number(min),QString::number(sec),QString::number(msec));



    }
    return timeDiff;
}

void GTALogResultTV::displayContextMenu(const QPoint &iPos)
{
    QWidget * pSourceWidget = (QWidget*)sender();
    if(pSourceWidget != NULL && _pContextMenuTV != NULL && this->model() != NULL)
    {
        QModelIndexList indexList = this->selectionModel()->selectedRows(0);
        if(indexList.count() == 2)
        {
             _pContextMenuTV->exec(this->viewport()->mapToGlobal(iPos));

        }
    }
}

void GTALogResultTV::displayHeaderContextMenu(QPoint iPos)
{
    QWidget * pSourceWidget = (QWidget*)sender();
    if(pSourceWidget != NULL && _pContextMenu != NULL)
    {
        _pContextMenu->exec(this->header()->viewport()->mapToGlobal(iPos));
    }
}

void GTALogResultTV::onRowDataChanged()
{
    if((_iModel != NULL) && (_iIndex.isValid()))
    {
        int row = _iIndex.row();
        QString sResultStatus = _RowEditor->getResultStatus();
        QModelIndex newIdx1 = _iModel->index(row,5,_iIndex.parent());
        _iModel->setData(newIdx1,sResultStatus);

        QString sUserFeedback = _RowEditor->getUserFeedback();
        QString sDefects = _RowEditor->getDefects();
        QString sExecTime  = _RowEditor->getExecutionTime();

        QModelIndex newIdx = _iModel->index(row,10,_iIndex.parent());
        QModelIndex newIdx2 = _iModel->index(row,11,_iIndex.parent());
        QModelIndex newIdx3 = _iModel->index(row,9,_iIndex.parent());
        _iModel->setData(newIdx,sUserFeedback);
        _iModel->setData(newIdx2,sExecTime);
        _iModel->setData(newIdx3,sDefects);
    }
}

void GTALogResultTV::showDetails(QAbstractItemModel *iModel, const QModelIndex &iIndex, bool &oIsDataChanged)
{

    if((iModel == NULL) || !iIndex.isValid())
        return;


    _iModel = iModel;
    _iIndex = iIndex;

    int row = iIndex.row();
    //int col = iIndex.column();

    GTAFilterModel * pFilterModel = (GTAFilterModel *)(currentIndex().model());
    if (pFilterModel!=NULL)
    {
        QAbstractItemModel * pEditorModel = pFilterModel->sourceModel();
        if (pEditorModel!=NULL)
        {

            QModelIndex currIndex = pFilterModel->mapToSource(currentIndex());
            QModelIndex pParentIndex =currIndex.parent();
            if(!pParentIndex.isValid())
            {
                pParentIndex = QModelIndex();
            }
            QModelIndex pIndex = pEditorModel->index(currIndex.row(),currIndex.column(),pParentIndex);
            if(pIndex .isValid())
            {
                GTACommandBase * pCmd = static_cast<GTACommandBase *>(pIndex.internalPointer());
                if(pCmd)
                {
                    if(pCmd->isTitle() || pCmd->hasReference())
                        return;
                }
            }
        }
    }
    int colCnt = iModel->columnCount();


    if(_RowEditor != NULL)
    {
        for(int i = 0; i < colCnt; i++)
        {
            _RowEditor->setData(iModel->index(row, i,iIndex.parent()).data().toString(),i);
        }
        _RowEditor->show();

        oIsDataChanged = true;
    }
}

//void GTALogResultTV::doubleClicked()
//{

//    QModelIndex firstIndex = lstOfIndex.at(0) ;
//QPoint currentPoint = event->x();

//    QModelIndex currIndex = currentIndex();

/* if (currentIndex().column() == 8)
    {

        GTAFilterModel * pFilterModel = (GTAFilterModel *)(currentIndex().model());
        if (pFilterModel!=NULL)
        {
            QAbstractItemModel * pEditorModel = pFilterModel->sourceModel();
            if (pEditorModel!=NULL)
            {
                QModelIndex pIndex = pEditorModel->index(currIndex.row(),currIndex.column(),currIndex.parent());
                if(pIndex .isValid())
                {
                    GTACommandBase * pCmd = static_cast<GTACommandBase *>(pIndex.internalPointer());
                    if (pCmd->getCommandType()==GTACommandBase::ACTION)
                    {
                        GTAActionBase* pAction = dynamic_cast<GTAActionBase*>(pCmd);
                        if (pAction->getAction()==ACT_IRT)
                        {

                            QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
                            QStringList detLogs;
                            foreach(GTAScxmlLogMessage objLog, msgList)
                            {
                                if(objLog.LOD == GTAScxmlLogMessage::Detail)
                                {
                                    QString curVal = objLog.CurrentValue;
                                    detLogs.append(curVal);
                                }
                            }
                            if(!detLogs.isEmpty())
                            {

                                QString imageInfo = detLogs.at(0);
                                GTAAppController* pCtrler = GTAAppController::getGTAAppController();
                                if (pCtrler)
                                {
                                    QString tmpDir = pCtrler->getTempDataDirectory();
                                    QString tempLogImageFile = "LogImage.png";

                                    QString tmpImagePath = QDir::cleanPath(QString("%1%2%3").arg(tmpDir,QDir::separator(),tempLogImageFile));
                                    GTAUtil::writeImage(imageInfo,tmpImagePath);
                                    //GTAUtil::writeImage(imageInfo,"temp.png");

                                    QPixmap pix(tmpImagePath);
                                    GTAImageDisplay* pDisplay = new GTAImageDisplay(0);
                                    pDisplay->setImage(pix);
                                    pDisplay->setVisible(true);

                                }

                            }

                        }
                    }

                }
            }
        }
    }*/
//if(currentIndex().column() == 5 || currentIndex().column() == 10)
//{


//    GTAFilterModel * pFilterModel = (GTAFilterModel *)(currIndex.model());
//    if (pFilterModel!=NULL)
//    {

//        QAbstractItemModel *pEditorModel = (QAbstractItemModel *)pFilterModel->sourceModel();

//if (pEditorModel!=NULL)
//{

//        int row = currIndex.row();
//        int col = currIndex.column();
//        QString data = pEditorModel->index(row,col).data().toString();

//        qDebug()<<data;
//            QModelIndex pIndex = pEditorModel->data(currIndex);
//            if(pIndex.isValid())
//            {
//                GTACommandBase * pCmd = static_cast<GTACommandBase *>(pIndex.internalPointer());
//                if(pCmd != NULL)
//                {
//                    if (pCmd->getCommandType()==GTACommandBase::ACTION)
//                    {
//                        GTAActionBase* pAction = dynamic_cast<GTAActionBase*>(pCmd);


//                        if(pAction->getAction() == ACT_CALL || pAction->getAction() == ACT_TITLE || pAction->getAction() == ACT_TITLE_END)
//                            return;
//                    }

//                    GTALogResultTVRowEditor *rowEditor = new GTALogResultTVRowEditor(this);

//                    rowEditor->show();
//                    if(rowEditor->exec() == QDialog::Accepted)
//                    {
//                        QString sResultStatus = rowEditor->getResultStatus();
//                        QString sUserFeedback = rowEditor->getUserFeedback();

//                        pCmd->updateUserComments(sUserFeedback);
//                        //
//                    }

//                }


//            }

//}
//}
//}

//    else
//        return;


//if (firstIndex.isValid())
//{
//
//    if(currIndex.column()==8)
//    {

//
//        GTACommandBase * pCmd = static_cast<GTACommandBase*>(firstIndex.internalPointer());
//        //if(pCmd)
//        //{
//        //    if (pCmd->getCommandType()==GTACommandBase::ACTION)
//        //    {
//        //        GTAActionBase* pAction = static_cast<GTAActionBase*>(firstIndex.internalPointer());
//        //        if (pAction->getAction()==ACT_IRT)
//        //        {

//        //            QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
//        //            QStringList detLogs;
//        //            foreach(GTAScxmlLogMessage objLog, msgList)
//        //            {
//        //                if(objLog.LOD == GTAScxmlLogMessage::Detail)
//        //                {
//        //                    QString curVal = objLog.CurrentValue;
//        //                    QString expVal = objLog.ExpectedValue;
//        //                    QString msg = QString("%1 : %2").arg(expVal, curVal);
//        //                    detLogs.append(msg);
//        //                }
//        //            }
//        //            if(!detLogs.isEmpty())
//        //            {
//        //                QString toolTip = "Variable : Value\n";
//        //                toolTip += detLogs.join("\n");

//        //            }

//        //        }
//        //    }

//        //   /* GTAActionIRT* pIRT = dynamic_cast<GTAActionIRT*>(pCmd);
//        //    if (pIRT)
//        //    {*/
//        //
//        //    //}
//        //
//        }


//        QDialog * pWidget = new QDialog(this);
//        QLabel* pLabel = new QLabel("",pWidget);
//        QPixmap pix("C:/Users/NG503F3/Desktop/Airbus_LTRA_LOGO.png");
//        pLabel->setPixmap(pix);
//        int ht = pix.height();
//        int wt = pix.width();
//        pWidget->setMinimumSize(QSize(wt,ht));
//        pWidget->setModal(true);
//        pLabel->setParent(pWidget);
//        pWidget->show();
//

//        //QMessageBox::information(NULL,"row 8","this will show a window with image");
//    }
//}
//}




/*






*/

