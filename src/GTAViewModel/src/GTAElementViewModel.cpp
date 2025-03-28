#include "GTAElementViewModel.h"
#include "GTAActionBase.h"
#include "GTACheckBase.h"
#include "GTAComment.h"
#include "GTACommandVariant.h"
#include "GTAActionElse.h"
#include "GTAActionIF.h"
#include "GTAUtil.h"
#include "GTAActionCall.h"
#include "GTACommandVariant.h"
#include <GTACommandList.h>

#pragma warning(push, 0)
#include <QColor>
#include <QFileInfo>
#include <QLabel>
#include <QSize>
#include <QUrl>
#include "qstandarditemmodel.h"
#include <QStandardItem>
#include <QDebug>
#pragma warning(pop)




bool GTAElementViewModel::_statusVar = false;
int GTAElementViewModel::_countParent = 0;



GTAElementViewModel::GTAElementViewModel(GTAElement *& pModel, QObject *parent) :
    QAbstractTableModel(parent),_pElementModel(NULL)
{
    _pElementModel = pModel;
    _currentContext=EDITING;

    _ColumnNameMap.insert(BREAKPOINT_INDEX,"");
    _ColumnNameMap.insert(ACTION_INDEX,"ACTION");
    _ColumnNameMap.insert(TYPE_INDEX,"TYPE");
    _ColumnNameMap.insert(COMMENT_INDEX,"COMMENT");
    _ColumnNameMap.insert(TIMEOUT_INDEX,"TIME OUT");
    _ColumnNameMap.insert(PRECISION_INDEX,"PRECISION");
    _ColumnNameMap.insert(ACTION_ON_FAIL_INDEX,"ACTION ON FAIL");
    _ColumnNameMap.insert(DUMP_LIST_INDEX,"DUMP LIST");
    _ColumnNameMap.insert(DUMP_LIST_INDEX, "REQUIREMENTS");
    _previousHighLightedRow = -1;

}
GTAElementViewModel::~GTAElementViewModel()
{

    _ColumnNameMap.clear();
    _lastError.clear();
    _EndHighlightsLst.clear();
}
int GTAElementViewModel::rowCount(const QModelIndex & ) const
{
    if (_pElementModel)
        return _pElementModel->getAllChildrenCount();
    else
        return 0;
}
int GTAElementViewModel::columnCount(const QModelIndex & )const
{
    int colCount = 0;
    if(_pElementModel != NULL)
    {
        colCount = _ColumnNameMap.size();
    }
    return colCount;
}
//void GTAElementViewModel::getDisplayDataMap(const QModelIndex &iIndex,QMap<int, QString>& ioColumnMapper) const
//{
//    ioColumnMapper.clear();
//    if (!iIndex.isValid())
//        return;

//    if(_pElementModel != NULL)
//    {
//        bool bOK=false;
//        int row = iIndex.row();

//        QString statement, comment;
//        QString timeOut, precision,timeOutUnit, precisionUnit, onFail;
//        QString typeOfCommand;
//        QString dumpList;
//        if( _pElementModel->hasIndex(row))
//        {
//            GTACommandBase* pCommand = NULL;
//            bOK=_pElementModel->getCommand(row,pCommand);


//            if(pCommand != NULL)
//            {
//                GTACommandBase::CommandType cmdType = pCommand->getCommandType();
//                if(cmdType == GTACommandBase::ACTION)
//                {
//                    GTAActionBase* pActionCmd = dynamic_cast<GTAActionBase*> (pCommand);
//                    if(pActionCmd != NULL)
//                    {
//                        statement =  pActionCmd->getStatement();
//                        pActionCmd->getTimeOut(timeOut,timeOutUnit);
//                        timeOut.append(QString(" %1").arg(timeOutUnit));
//                        pActionCmd->getPrecision(precision,precisionUnit);

//                        if (precisionUnit!=ACT_PRECISION_UNIT_VALUE)
//                            precision.append(QString(" %1").arg(precisionUnit));

//                        onFail = pActionCmd->getActionOnFail();
//                        typeOfCommand = pActionCmd->getCommandTypeForDisplay();
//                    }
//                }
//                else if(cmdType == GTACommandBase::CHECK)
//                {
//                    GTACheckBase* pCheckCmd = dynamic_cast<GTACheckBase*> (pCommand);
//                    if (NULL!=pCheckCmd)
//                    {
//                        statement = pCheckCmd->getStatement();

//                        pCheckCmd->getTimeOut(timeOut,timeOutUnit);
//                        timeOut.append(QString(" %1").arg(timeOutUnit));

//                        pCheckCmd->getPrecision(precision,precisionUnit);


//                        if (precisionUnit!=ACT_PRECISION_UNIT_VALUE)
//                            precision.append(QString(" %1").arg(precisionUnit));


//                        onFail = pCheckCmd->getActionOnFail();
//                        typeOfCommand = pCheckCmd->getCommandTypeForDisplay();
//                    }
//                }

//                comment = pCommand->getComment();
//                dumpList = pCommand->getDumpList().join(",\n");
//            }



//            int actionColumn = 0;

//            if (pCommand != NULL && timeOut.trimmed().isEmpty())
//                timeOut ="--";

//            if(pCommand != NULL && precision.trimmed().isEmpty())
//                precision="--";

//            ioColumnMapper.insert(actionColumn,statement);
//            ioColumnMapper.insert(++actionColumn,typeOfCommand);
//            ioColumnMapper.insert(++actionColumn,comment);
//            ioColumnMapper.insert(++actionColumn,timeOut);
//            ioColumnMapper.insert(++actionColumn,precision);
//            ioColumnMapper.insert(++actionColumn,onFail);
//            ioColumnMapper.insert(++actionColumn,dumpList);
//        }
//    }

//}


QVariant GTAElementViewModel::data(const QModelIndex &index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    bool isPrintMsgCmd=false;
    GTACommandBase* pCmd = NULL;
    _pElementModel->getCommand(index.row(),pCmd);
    //GTACheckBase* pCheck =  dynamic_cast <GTACheckBase*>(pCmd);

    GTAActionBase * pActCmd = dynamic_cast<GTAActionBase *>(pCmd);
    if((pActCmd != NULL) && (pActCmd->getAction() == ACT_PRINT) && (pActCmd->getComplement() == COM_PRINT_MSG))
    {
        isPrintMsgCmd = true;
    }

    if(_pElementModel != NULL)
    {
        QString sCellValue;
        QMap<int, QString> columnMapper;
        int row = index.row();
        int col = index.column();

        switch (role)
        {

        case Qt::DisplayRole:

            getDisplayDataMap(index,columnMapper);
            sCellValue= columnMapper.value(col);
            if((isPrintMsgCmd) && (col == ACTION_INDEX))
            {
                sCellValue.replace("print message to execution log: ","");
            }
            if (sCellValue.isEmpty())
                return QVariant();
            return QVariant(sCellValue);

            break;


        case Qt::TextAlignmentRole:


            /* if((_pElementModel->getElementType() == GTAElement::PROCEDURE)&&(row+col==0))
   return int(Qt::AlignHCenter | Qt::AlignVCenter);
   if ( (_pElementModel->getElementType() == GTAElement::PROCEDURE) && (row==_pElementModel->getAllChildrenCount()-1) && (col==0))
   return int(Qt::AlignHCenter | Qt::AlignVCenter);*/

            //if((col ==4) || (col ==3))
            //    return int(Qt::AlignCenter);
            //else
            //    return int(Qt::AlignLeft | Qt::AlignVCenter);
            //break;
            return int(Qt::AlignVCenter);


        case Qt::ForegroundRole:

            if(pCmd != NULL)
            {
                if(getIndentedView())
                {
                    GTAActionWhile* pWhile = dynamic_cast<GTAActionWhile*>(pCmd);
                    if(pWhile)
                    {
                        return QColor(255,64,34,255);
                    }
                    GTAActionDoWhile* pDoWhile = dynamic_cast<GTAActionDoWhile*>(pCmd);
                    if(pDoWhile)
                    {
                        return QColor(255,64,34,255);
                    }
                    GTAActionIF* pIF = dynamic_cast<GTAActionIF*>(pCmd);
                    if(pIF)
                    {
                        return QColor(255,64,34,255);
                    }
                    GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCmd);
                    if(pElse)
                    {
                        return QColor(255,64,34,255);
                    }
                    GTAActionForEach *pForEach = dynamic_cast<GTAActionForEach*>(pCmd);
                    if(pForEach)
                    {
                        return QColor(255,64,34,255);
                    }
                    if(pCmd->IsEndCommand())
                    {
                        return QColor(255,64,34,255);
                        /*return QColor(127,17,9,255);*/
                    }
                }
                if (_currentContext == SEARCH_REPLACE)
                {
                    if (pCmd!=NULL && pCmd->getSearchHighlight()==true)
                        return QColor(255,255,255,255);
                }
                else  if(_currentContext == EDITING || _currentContext == DEBUG)
                {
                    if(pCmd !=NULL && pCmd->getReadOnlyState())
                    {
                        return QColor(169, 169, 169, 30); /*Qt::darkGray;*/

                    }

                    if (_EndHighlightsLst.contains(row))
                        return QColor(255,255,255,255);
                }
                else if (_currentContext == LIVE)
                {
                    if(pCmd !=NULL && pCmd->getReadOnlyState())
                    {
                        return QColor("#8395a7");
                    }
                }

                QColor color = pCmd->getForegroundColor();
                if(_currentContext == LIVE && color == Qt::black)
                    color = Qt::white;
                
                if(color != Qt::black && (index.column() != 1 &&  pCmd->isForegroundColor()))
                    color = Qt::black;

                if((col == ACTION_INDEX) && isPrintMsgCmd)
                {
                    if(_currentContext == LIVE)
                        return QColor(Qt::cyan);
                    else
                    	return QColor(Qt::blue);
                }


                return color;
            }
            return QVariant();
            break;

        case Qt::BackgroundRole:
        {
            QColor oColor;
            if (pCmd)
            {
                oColor = pCmd->getBackgroundColor();
            }

            switch (_currentContext)
            {

            case DEBUG :
                if(pCmd != NULL)
                {
                    if(!pCmd->isIgnoredInSCXML())
                    {
                        if(pCmd->hasBreakpoint() && !pCmd->hasExecutionControl())
                        {
                            return QColor(192,57,43,50);
                        }
                        else if(pCmd->hasBreakpoint() && pCmd->hasExecutionControl())
                        {
                            return QColor(243,156,18,255);
                        }
                        else if(!pCmd->hasBreakpoint() && pCmd->hasExecutionControl())
                        {
                            return QColor(243,156,18,255);
                        }
                        else if(pCmd->isTitle())
                        {
                            return oColor;
                        }
                        else
                        {
                            return QColor(0,0,0,0);
                        }
                    }
                    else
                    {
                        return QColor(189,195,199,255);
                    }
                }
                break;
            case EDITING:
                if(pCmd !=NULL && pCmd->isIgnoredInSCXML())
                {
                    return QColor(128,0,0,50);

                }
                if (_EndHighlightsLst.contains(row))
                    return QColor(0,0,0,255);
                if(getIndentedView())
                {
                    if(pCmd)
                    {
                        GTAActionIF* pIF = dynamic_cast<GTAActionIF*>(pCmd);
                        GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCmd);
                        GTAActionWhile* pWhile = dynamic_cast<GTAActionWhile*>(pCmd);
                        GTAActionDoWhile* pDoWhile = dynamic_cast<GTAActionDoWhile*>(pCmd);
                        GTAActionForEach *pForEach = dynamic_cast<GTAActionForEach*>(pCmd);
                        int sub_row = index.row();
                        int *prow = &sub_row;
                        int else_count=0;
                        int count=0;
                        else_count = blockCheck(prow);
                        pCmd->getMasterCmd(count);
                        int pcount = count- else_count;
                        if(pCmd->getParent() && !pCmd->IsEndCommand() && !pIF && !pElse && !pWhile && !pDoWhile && !pForEach) {
                            /*return QColor(3, 169, 244, 50+pcount*50);*/
                            return QColor(255, 254-pcount*15, 213, 255-pcount*15);
                        }
                    }
                }
                break;
            case COMPATIBILITY_CHK:

                if (pCmd!=NULL && pCmd->getCompatibilityHighLight() == true)
                    oColor = QColor(251,130,89);
                break;
            case SEARCH_REPLACE:
                if (pCmd!=NULL && pCmd->getSearchHighlight()==true)
                    oColor = QColor(0,195,0,150);
                break;
            case LIVE:
                oColor.setNamedColor("#232629");
                if (nullptr != pCmd && col == BREAKPOINT_INDEX)
                {
                    if (pCmd->getExecutionResult() == "OK")
                        oColor.setNamedColor("#78e08f");
                    else if (pCmd->getExecutionResult() == "KO")
                        oColor.setNamedColor("#e55039");
                    else if (pCmd->getExecutionResult() == "NA")
                        oColor.setNamedColor("#45aaf2");
                }
                break;

            default:
                return oColor;
            }

            if (oColor.isValid())
                return oColor;
            else
                return QVariant();

            //QColor(102,152,255,80);//QColor(128,0,0,50);
            break;
        }
        case Qt::ToolTipRole:

            if(getIndentedView())
            {
                getDisplayDataMap(index,columnMapper);
                QString tooltip = "<b>Type: </b>" + columnMapper.value(1) + "<br>" + "<b>Comment: </b>" + columnMapper.value(2) + " <br>" + "<b>Time Out: </b>" + columnMapper.value(3) + " <br>" + "<b>Precision: </b>" + columnMapper.value(4) + " <br>" + "<b>Action On Fail:  </b>" + columnMapper.value(5) + "    <br>" + "<b>Dump List: </b>" + columnMapper.value(6);
                return QVariant(tooltip);
            }
            else
            {
                getDisplayDataMap(index,columnMapper);
                return QVariant(columnMapper.value(col, QString()));
            }
            break;
        case Qt::FontRole:
        {
            QFont oFont;
            if(pCmd != NULL)
            {
                oFont = pCmd->getFont();
                if((col == ACTION_INDEX) && isPrintMsgCmd)
                    oFont.setBold(true);
                if(getIndentedView())
                {
                    GTAActionWhile* pWhile = dynamic_cast<GTAActionWhile*>(pCmd);
                    if(pWhile)
                    {
                        oFont.setBold(true);
                    }
                    GTAActionDoWhile* pDoWhile = dynamic_cast<GTAActionDoWhile*>(pCmd);
                    if(pDoWhile)
                    {
                        oFont.setBold(true);
                    }
                    GTAActionIF* pIF = dynamic_cast<GTAActionIF*>(pCmd);
                    if(pIF)
                    {
                        oFont.setBold(true);
                    }
                    GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCmd);
                    if(pElse)
                    {
                        oFont.setBold(true);
                    }
                    GTAActionForEach *pForEach = dynamic_cast<GTAActionForEach*>(pCmd);
                    if(pForEach)
                    {
                        oFont.setBold(true);
                    }
                    if(pCmd->IsEndCommand())
                    {
                        oFont.setBold(true);
                    }
                }
            }

            switch (_currentContext)
            {
            case DEBUG:
            case EDITING:
                if (_EndHighlightsLst.contains(row))
                {
                    oFont.setBold(true);
                    oFont.setWeight(QFont::Bold);
                }
                if (pCmd!=NULL && pCmd->getReadOnlyState())
                {
                    oFont.setBold(true);
                    oFont.setWeight(QFont::Bold);
                    oFont.setItalic(true);
                }

                if (pCmd!=NULL && pCmd->isIgnoredInSCXML())
                {
                    oFont.setStrikeOut(true);
                }
                break;
            case COMPATIBILITY_CHK:


                break;
            case SEARCH_REPLACE:

                break;
            default:
                return oFont;
            }


            return oFont;
            break;
        }

        case Qt::DecorationRole :
        {
            if(_currentContext == DEBUG)
            {
                if(index.isValid() && index.column() == BREAKPOINT_INDEX)
                {
                    QVariant var;
                    if((pCmd != NULL)&& (!pCmd->isIgnoredInSCXML()))
                    {
                        if(pCmd->hasBreakpoint()&& !pCmd->hasExecutionControl())
                        {
                            QPixmap icon = QPixmap(":/images/forms/img/Breakpoint.png");
                            var.setValue(icon);
                        }
                        else if(pCmd->hasBreakpoint() && pCmd->hasExecutionControl())
                        {
                            QPixmap icon = QPixmap(":/images/forms/img/CurrentBreakppoint.png");
                            var.setValue(icon);
                        }
                        else if(!pCmd->hasBreakpoint() && pCmd->hasExecutionControl())
                        {
                            QPixmap icon = QPixmap(":/images/forms/img/BreakpointArrow.png");
                            var.setValue(icon);
                        }
                        else
                        {
                            QPixmap icon = QPixmap(":/images/forms/img/Empty.png");
                            var.setValue(icon);
                        }
                    }
                    return var;
                }
            }
            else if(_currentContext == EDITING)
            {
                if(index.isValid() && index.column() == BREAKPOINT_INDEX)
                {
                    QVariant var;
                    if((pCmd != NULL) && (!pCmd->isIgnoredInSCXML()))
                    {
                        if(pCmd->hasBreakpoint()&& !pCmd->hasExecutionControl())
                        {
                            QPixmap icon = QPixmap(":/images/forms/img/Breakpoint.png");
                            var.setValue(icon);
                        }
                        else
                        {
                            QPixmap icon = QPixmap(":/images/forms/img/Empty.png");
                            var.setValue(icon);
                        }

                    }
                    return var;
                }
            }
            break;
        }
        //        case Qt::SizeHintRole :
        //        {

        //            if(index.isValid() && index.column() == BREAKPOINT_INDEX)
        //            {
        //                QSize size;
        //                size.setWidth(25);
        //                return size;
        //            }
        //            break;

        //         }
        default:
            return QVariant();
        }

    }
    return QVariant();
}



//QModelIndex GTAElementViewModel::index( int row, int column, const QModelIndex & parent )const
//{
//    return createIndex(row,0);
//}

QModelIndex GTAElementViewModel::parent( const QModelIndex &  )
{
    return QModelIndex();
}

QVariant GTAElementViewModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    QVariant oVal = QVariant();
    if (role==Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if(_pElementModel != NULL)
            {
                int column = 0;
                QString colName =   _ColumnNameMap.value(section+column);
                if(! colName.isEmpty())
                    oVal = QVariant(colName);
            }

        }
        if (orientation == Qt::Vertical)
            oVal=QVariant(QString::number(section));
    }

    /*if (role == Qt::SizeHintRole)
 {
  if (orientation == Qt::Horizontal)
   switch (section)
   {

    case 0:oVal = QVariant(QSize(300,20));
     break;


   }
 }*/

    return oVal;
}

bool GTAElementViewModel::setData( const QModelIndex & index, const QVariant & value, int  )
{
    bool rc = false;
    if (!_pElementModel)
        return rc;

    GTACommandVariant valCmdVariant = value.value<GTACommandVariant>();
    GTACommandBase* pNewCmd = valCmdVariant.getCommand();

    int currentRow = index.row();
    GTACommandBase* pExistingCommand = nullptr;

    if (pNewCmd != nullptr)
    {
        GTACommandBase::CommandType cmdType= pNewCmd->getCommandType();
        //TODO: Review the code once comment is not inherited from command.
        if (GTACommandBase::COMMENT!=cmdType && GTACommandBase::EQUATION!=cmdType) //comment and equation not allowed to be edited through set data
        {
            rc = _pElementModel->getCommand(currentRow,pExistingCommand);

            auto pParent = _pElementModel->getParentForIndex(currentRow);
            if (pParent) {
                rc = checkCommandValidity(pExistingCommand, *pParent, pNewCmd);
            }
            
            if (!rc)
                return false;

            int rowcntBefore = _pElementModel->getAllChildrenCount();
            GTAActionBase* pNewActionBase = dynamic_cast<GTAActionBase*>(pNewCmd);
            bool pConditionElse = pNewActionBase ? pNewActionBase->getComplement() == COM_CONDITION_ELSE : false;
            if (pExistingCommand || pConditionElse)
            {
                rc = _pElementModel->updateCommand(currentRow, pNewCmd);//TODO test this code if copy paste uses updatecommand

            }
            else
            {
                rc = _pElementModel->insertCommand(pNewCmd, currentRow, true);
                int rowcntAfter = pNewCmd->getAllChildrenCount();
                if(index.row()!=0)
                    _pElementModel->removeCommand(index.row() + rowcntAfter + 1);
                QModelIndex topLeftIndex = this->createIndex(index.row(), 0);
                QModelIndex bottomRightIndex = this->createIndex(index.row() + rowcntAfter, columnCount());

                this->dataChanged(topLeftIndex, bottomRightIndex);
            }

            if (rc)
            {
                int rowcntAfter = _pElementModel->getAllChildrenCount();
                int totalChange = rowcntAfter - rowcntBefore;
                if (totalChange > 0)
                {
                    QModelIndex tmp_index;
                    beginInsertRows(tmp_index, currentRow, currentRow + totalChange - 1);
                    endInsertRows();
                }
            }
        }
        else
            _lastError="Comment/Equation cannot be edited through editor";

    }
    else
        _lastError="use add row command to append/add empty rows";

    return rc;
}

Qt::ItemFlags GTAElementViewModel::flags( const QModelIndex & index ) const
{

    Qt::ItemFlags itemFlags;
    itemFlags = QAbstractItemModel::flags(index);
    itemFlags |=  Qt::ItemIsDropEnabled;

    if(index.isValid() && index.column() == BREAKPOINT_INDEX)
    {
        itemFlags |= Qt::ItemIsEnabled | Qt::ItemIsUserCheckable /*| Qt::ItemIsEditable*/;
    }

    return itemFlags;
}
bool GTAElementViewModel::insertRows( int row, int count, const QModelIndex & parent)
{
    //if (row==-1)
    //{
    //    beginInsertRows(parent,0,0+count);
    //    row--;
    //}
    //else
    beginInsertRows(parent,row,row+count);
    int nextRow =0;
    if (rowCount())
        nextRow=row + 1;
    _pElementModel->insertCommand(NULL,nextRow,false);

    endInsertRows ();
    return true;
}

bool GTAElementViewModel::removeRows( int row, int count, const QModelIndex & parent)
{
    bool bOk =false;
    
    if (row+count-1 >= this->rowCount()) //from row to row+count-1
        return bOk;
    QList<int> rowsMarkedForDeletion;
    int lastRowToBeDeleted = row;
    for (int i=row;i<=row+count-1;i++)
    {
        GTACommandBase* pCurrentCommand =NULL;
        bOk=_pElementModel->getCommand(i,pCurrentCommand);
        if (bOk)
        {
            if (pCurrentCommand!=NULL)
            {
                int totalChildrens = pCurrentCommand->getAllChildrenCount();
                rowsMarkedForDeletion.append(i);
                i=i+totalChildrens;
            }
            else
            {
                rowsMarkedForDeletion.append(i);
            }

            if (i>lastRowToBeDeleted)
                lastRowToBeDeleted=i;
        }

    }


    beginRemoveRows(parent, row, lastRowToBeDeleted);

    for(int j=rowsMarkedForDeletion.size()-1;j>=0;j--)
    {
        _pElementModel->getAllChildrenCount();
        int indexForRemoval = rowsMarkedForDeletion[j];
        bOk=_pElementModel->removeCommand(indexForRemoval);

    }
    endRemoveRows();
    return bOk;
}
bool GTAElementViewModel::updateChanges()
{
    QModelIndex topLeftIndex = this->createIndex(0,0);
    QModelIndex bottomRightIndex = this->createIndex(_pElementModel->getAllChildrenCount()-1,_pElementModel->getAllChildrenCount()-1);
    emit this->dataChanged(topLeftIndex,bottomRightIndex);
    return true;

}
void GTAElementViewModel::clearAllinEditor()
{
    if (_pElementModel)
    {

        //_pElementViewModel->removeRows(1,_pElement->getTotalItems()-2);
        int currentRows = _pElementModel->getAllChildrenCount();
        _pElementModel->ClearAll();
        _pElementModel->initialize(currentRows);

    }
}
bool GTAElementViewModel::checkCommandValidity(GTACommandBase* pCurrentCommand, GTACommandBase* pParentCommand, GTACommandBase* pIncomingCmd)
{
    bool rc = true;
	if (pIncomingCmd != NULL)
	{
		GTAActionBase * pActionBase = dynamic_cast<GTAActionBase*>(pIncomingCmd);
		if (pActionBase != NULL) {
			QString action = pActionBase->getAction();
			QString complement = pActionBase->getComplement();

			if (complement == COM_CONDITION_ELSE)
			{
				//GTAActionElse * pElseCmd = dynamic_cast<GTAActionElse*>(pIncomingCmd);
				if (NULL != pParentCommand && rc)
				{
					GTAActionIF* pParentIF = dynamic_cast<GTAActionIF*> (pParentCommand);
					GTAActionElse* pParentElse = dynamic_cast<GTAActionElse*> (pParentCommand);

					if (pParentIF)
					{

						GTAActionElse* pElseCurrent = dynamic_cast<GTAActionElse*> (pCurrentCommand);
						GTAActionElse* pElseNew = dynamic_cast<GTAActionElse*> (pIncomingCmd);
						if (pElseCurrent != NULL &&  pElseNew != NULL)
						{
							;//do nothing if the current command is else since editing of else with else is allowed.

						}
						else if (pParentIF->hasMutuallExclusiveStatement())//has else statement
						{
							_lastError = QString("Parent command has existing else");
							return false;
						}
					}
					else if (pParentIF == NULL)
					{
						GTAActionElse* pElseNew = dynamic_cast<GTAActionElse*> (pIncomingCmd);
						if (pElseNew != NULL)
						{
							_lastError = QString("Else command can only be inserted under IF command");
							return false;
						}
					}
					else if (pParentElse != NULL)
					{
						_lastError = QString("Else command cannot be inserted under Else command");
						return false;
					}
				}
				else
				{
					_lastError = QString("Else command should have preceding if statement");
					return false;
				}

			}
			else if (action == ACT_TITLE)
			{
				if (NULL != pParentCommand) {
					GTAActionBase * pActionBaseParent = dynamic_cast<GTAActionBase*>(pParentCommand);
					QString parent_action = pActionBaseParent->getAction();
					if (parent_action == ACT_CONDITION)
					{
						_lastError = QString("Titles cannot be inserted inside of conditions");
						return false;
					}
				}
			}

		}
	}

    return rc;

}
bool GTAElementViewModel::prependRow()
{
    bool rc = false;
    if (_pElementModel!=NULL)
    {
        beginInsertRows(QModelIndex(),0,0);
        rc = _pElementModel->prependRow();
        if (rc)
        {
            
            endInsertRows();
        }
        
    }
    return rc;
}

GTAElementViewModel::DisplayContext GTAElementViewModel::getDisplayContext()const
{
    return _currentContext;
}

void GTAElementViewModel::setDisplayContext(DisplayContext iContext)
{
    _currentContext = iContext;
    _EndHighlightsLst.clear();
}

void GTAElementViewModel::findAndHighlight(const QRegExp&  searchString,QList<int>& iSelectedRows,QHash<int,QString>& ofoundRows,bool ibSearchUp,bool ibTextSearch,bool isSingleSearch,bool isHighlight)
{
    if(_pElementModel)
    {

        _pElementModel->resetStringSearch();
        _pElementModel->findStringAndSetHighlight(searchString,iSelectedRows,ofoundRows,ibSearchUp,ibTextSearch,isSingleSearch,isHighlight);
    }

    this->updateChanges();
}
bool GTAElementViewModel::replaceString(const int& row,const QRegExp& strToReplace,const QString& sReplaceWith,bool)
{
    bool rc = false;
    if(_pElementModel)
    {

        _pElementModel->resetStringSearch();
        rc =_pElementModel->replaceString(row,strToReplace,sReplaceWith );

    }

    this->updateChanges();
    return rc;
}
bool GTAElementViewModel::replaceAllString(const QRegExp& searchString,const QString& sReplaceWith)
{
    bool rc = false;
    if(_pElementModel)
    {


        _pElementModel->resetStringSearch();
        rc =_pElementModel->replaceAllString(searchString,sReplaceWith);

    }

    this->updateChanges();
    return rc;
}
bool GTAElementViewModel::replaceAllEquipment(const QRegExp& searchString, const QString& sReplaceWith)
{
    bool rc = false;
    if (_pElementModel)
    {
        _pElementModel->resetStringSearch();
        rc = _pElementModel->replaceAllEquipment(searchString, sReplaceWith);
    }
    this->updateChanges();
    return rc;
}
bool GTAElementViewModel::highLightLastChild(const QModelIndex& ipIndex)
{
    bool rc = false;
    _EndHighlightsLst.clear();
    if (ipIndex.isValid() )
    {
        int row = ipIndex.row();
        if (row>=0)
        {
            GTACommandBase* pCmd=NULL;
            _pElementModel->getCommand(row,pCmd);
            if (pCmd!=NULL)
            {
                if (pCmd->IsEndCommand()) // end while etc.
                {
                    GTACommandBase* pParent = pCmd->getParent();
                    if (pParent!=NULL)
                    {
                        int childrenCnt = pParent->getAllChildrenCount();
                        _EndHighlightsLst.append(row-childrenCnt);
                        _EndHighlightsLst.append(row);
                        int elsePose=-1;
                        if (pParent->hasMutuallExclusiveStatement(&elsePose))
                        {
                            if (elsePose!=-1)
                                _EndHighlightsLst.append(row-childrenCnt+elsePose);
                        }
                    }


                }
                else
                {
                    int elsePos=-1;
                    int childrenCount = pCmd->getAllChildrenCount();
                    if (pCmd->canHaveChildren())
                    {
                        int elseAdjust=0;
                        GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCmd);
                        if (pElse!=NULL)
                        {
                            elseAdjust++;
                            GTACommandBase* pParent = pElse->getParent();
                            int thisElsePos=-1;
                            if (pParent !=NULL && pParent->hasMutuallExclusiveStatement(&thisElsePos))
                            {
                                if(thisElsePos>=0)
                                    _EndHighlightsLst.append(row-thisElsePos);
                            }

                        }


                        _EndHighlightsLst.append(row+childrenCount+elseAdjust);
                        _EndHighlightsLst.append(row);
                        if (pCmd->hasMutuallExclusiveStatement(&elsePos))
                        {
                            if (elsePos!=-1)
                                _EndHighlightsLst.append(row+elsePos);
                        }
                    }

                }
                
            }

        }
    }
    else
        _EndHighlightsLst.clear();

    this->updateChanges();

    return rc;
}
void GTAElementViewModel::setElement(GTAElement * pElementModel)
{
    _pElementModel = pElementModel;
}
QStringList GTAElementViewModel::mimeTypes() const
{
    QStringList types;
    types <<"text/plain" <<"text/uri-list"<<"application/x-qabstractitemmodeldatalist";
    return types;
}
Qt::DropActions GTAElementViewModel::supportedDropActions() const
{
    return Qt::CopyAction  ;
}
bool GTAElementViewModel::dropMimeData(const QMimeData *data,Qt::DropAction action,
                                          int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;


    QStringList formatsStr = data->formats();
    if (! (data->hasFormat("text/uri-list") ||  data->hasFormat("application/x-qabstractitemmodeldatalist")))
        return false;

    if (column > 0)
        return false;
    int beginRow;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        return false;

    GTACommandBase * pBase = NULL;
    _pElementModel->getCommand(beginRow,pBase);
    if (NULL!=pBase)
    {
        return false;
    }
    QString sFileName;
    QByteArray encodedData ;
    
    if (data->hasFormat("application/x-qabstractitemmodeldatalist"))
    {
        QStandardItemModel model;//; = new QStandardItemModel();
        model.dropMimeData(data,action,0,0,QModelIndex());
        QStandardItem *Param = model.item(0,0);
        if (Param!=NULL)
            sFileName = Param->text();
    }
    else if ( data->hasFormat("text/uri-list"))
    {
        encodedData= data->data("text/uri-list");
        sFileName = QString(encodedData);
        QUrl dataFile(sFileName);
        QFileInfo info (dataFile.toLocalFile());
        sFileName = info.fileName();

    }
    else
        sFileName.clear();

    sFileName = sFileName.trimmed();

    if (!sFileName.isEmpty())
    {
        if (sFileName.endsWith(".seq",Qt::CaseInsensitive))
            return false;
        else if (sFileName.endsWith(".pro",Qt::CaseInsensitive))
        {
            if (_pElementModel->getElementType() != GTAElement::SEQUENCE)
                return false;
        }
        else if (sFileName.endsWith(".obj",Qt::CaseInsensitive)||sFileName.endsWith(".fun",Qt::CaseInsensitive))
        {
            if (_pElementModel->getElementType() == GTAElement::OBJECT)
                return false;

        }

        {
            QString complement = COM_CALL_OBJ;

            if (sFileName.endsWith(".pro",Qt::CaseInsensitive))
                complement = COM_CALL_PROC;
            else if (sFileName.endsWith(".fun",Qt::CaseInsensitive))
            {
                complement = COM_CALL_FUNC;
            }

            emit callActionInserted(beginRow, sFileName);

            //GTAActionCall* pCall = new GTAActionCall(ACT_CALL,complement,sFileName);
            //QModelIndex inx = createIndex(beginRow,0);

            //GTACommandVariant variant;
            //variant.setCommand(pCall);
            //QVariant actionCmdVariant;
            //actionCmdVariant.setValue(variant);
            //setData(inx,actionCmdVariant);
        }

        
    }

    return true;
}

QMimeData *GTAElementViewModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QString text = data(index, Qt::DisplayRole).toString();
            stream << index.internalId() << index.row() << index.column() << text;
        }
    }
    mimeData->setData("text/uri-list", encodedData);
    return mimeData;
}

void GTAElementViewModel::highlightRow(const int &iRowId, QModelIndex &oHighlightedRow, GTAElementViewModel::DisplayContext iDisplayContext)
{
    if(_pElementModel != NULL)
    {
        setDisplayContext(iDisplayContext);
        GTACommandBase * pCmd = NULL;
        GTACommandBase * pPreviousHighlightedCmd = NULL;
        _pElementModel->getCommand(iRowId,pCmd);
        if(pCmd)
        {

            _pElementModel->getCommand(_previousHighLightedRow,pPreviousHighlightedCmd);
            if(pPreviousHighlightedCmd)
            {
                pPreviousHighlightedCmd->setCompatibilityHighLight(false);
                pPreviousHighlightedCmd->setSearchHighlight(false);
            }

            switch(iDisplayContext)
            {
            case GTAElementViewModel::SEARCH_REPLACE :
                pCmd->setSearchHighlight(true);
                break;

            case GTAElementViewModel::COMPATIBILITY_CHK :
                pCmd->setCompatibilityHighLight(true);
                break;
            }


            oHighlightedRow = index(iRowId,0);
            _previousHighLightedRow = iRowId;
        }

    }
    this->updateChanges();
}


//bool GTAElementViewModel::dropMimeData(const QMimeData *idata,Qt::DropAction action, int row, int column, const QModelIndex &parent)
//{
//    if (action == Qt::IgnoreAction)
//        return true;
//
//    QStringList iDataFormats =  idata->formats ();
//
//    QAbstractTableModel *model = new QAbstractTableModel();
//    model->dropMimeData(idata,action,row,column,parent);
//
//    QStandardItem *elemData = model->item(0,0);
//    QString itemText;
//    if(elemData != NULL)
//    {
//        itemText = elemData->text();
//    }
//
//    return true;
//    
//    // if (idata->hasUrls())
//    //    { 
//    //        QList<QUrl> sFileName = idata->urls();
//    //        int urlSize = sFileName.size();
//    //        for (int i=0;i<urlSize;i++)
//    //        {
//    //            QString hostName = sFileName.at(i).host();
//    //            QString test;
//    //        }
//    //    }
//
//    //
//    //QString sFileName = idata->text();//"text/plain");
//}


//------------------------------- Indentation implementation --------------------------------

void GTAElementViewModel::getDisplayDataMap(const QModelIndex &iIndex, QMap<int,QString> &ioColumnMapper) const
{
    ioColumnMapper.clear();
    int else_count=0;
    if (!iIndex.isValid())
        return;

    if(_pElementModel != NULL)
    {
        bool bOK = false;
        int row = iIndex.row();
        int* prow = &row;

        QString statement, comment;
        QStringList requirements;
        QString timeOut, precision,timeOutUnit, precisionUnit, onFail;
        QString typeOfCommand;
        QString dumpList;
        GTACommandBase * pCmd = NULL;
        _countParent = 0;
        if( _pElementModel->hasIndex(row))
        {
            GTACommandBase* pCommand = NULL;
            bOK = _pElementModel->getCommand(row,pCommand);


            if(pCommand != NULL)
            {
                GTACommandBase::CommandType cmdType = pCommand->getCommandType();
                if(cmdType == GTACommandBase::ACTION)
                {
                    GTAActionBase* pActionCmd = dynamic_cast<GTAActionBase*> (pCommand);
                    if(pActionCmd != NULL)
                    {
                        statement =  pActionCmd->getStatement();
                        _pElementModel->getCommand(row,pCmd);
                        pCmd->getMasterCmd(_countParent);
                        pActionCmd->getTimeOut(timeOut,timeOutUnit);
                        timeOut.append(QString(" %1").arg(timeOutUnit));
                        pActionCmd->getPrecision(precision,precisionUnit);

                        if (precisionUnit != ACT_PRECISION_UNIT_VALUE)
                            precision.append(QString(" %1").arg(precisionUnit));

                        onFail = pActionCmd->getActionOnFail();
                        typeOfCommand = pActionCmd->getCommandTypeForDisplay();
                    }
                }
                else if(cmdType == GTACommandBase::CHECK)
                {
                    GTACheckBase* pCheckCmd = dynamic_cast<GTACheckBase*> (pCommand);
                    if (NULL != pCheckCmd)
                    {
                        statement = pCheckCmd->getStatement();
                        _pElementModel->getCommand(row,pCmd);
                        pCmd->getMasterCmd(_countParent);
                        pCheckCmd->getTimeOut(timeOut,timeOutUnit);
                        timeOut.append(QString(" %1").arg(timeOutUnit));

                        pCheckCmd->getPrecision(precision,precisionUnit);


                        if (precisionUnit != ACT_PRECISION_UNIT_VALUE)
                            precision.append(QString(" %1").arg(precisionUnit));


                        onFail = pCheckCmd->getActionOnFail();
                        typeOfCommand = pCheckCmd->getCommandTypeForDisplay();
                    }
                }

                comment = pCommand->getComment();
                requirements = pCommand->getRequirements();
                dumpList = pCommand->getDumpList().join(",\n");
            }



            //            int actionColumn = 0;
            int actionColumn = ACTION_INDEX;
            //GTACommandBase* temp;
            if (pCommand != NULL && timeOut.trimmed().isEmpty())
                timeOut = "--";

            if(pCommand != NULL && precision.trimmed().isEmpty())
                precision = "--";
            QString edit = "";
            QString statementNew = "";

            if(pCmd)
            {
                if(getIndentedView())
                {
                    else_count = blockCheck(prow);

                    GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(pCmd);
                    if (pElse != NULL ) // end while etc.
                    {
                        else_count++;
                    }
                    if(pCmd->IsEndCommand())
                    {
                        _countParent = _countParent - 1;
                    }


                    /* qDebug()<<"countParent"<<countParent;*/
                    _countParent = _countParent - else_count;
                    if(_countParent > 0)
                    {
                        for(int i=0; i<_countParent; i++)
                        {
                            /*edit = "\t"+edit;*/
                            //क्योंकि इंडेंटेशन सामुदायिक मानकों के अनुसार नहीं था
                            edit = "            "+edit;
                        }
                    }
                }
            }

            statementNew = edit+statement;

            ioColumnMapper.insert(actionColumn,statementNew);
            const_cast<GTAElementViewModel*>( this )->updateChanges();
            //            updateChanges();
            ioColumnMapper.insert(++actionColumn,typeOfCommand);
            ioColumnMapper.insert(++actionColumn,comment);
            ioColumnMapper.insert(++actionColumn,timeOut);
            ioColumnMapper.insert(++actionColumn,precision);
            ioColumnMapper.insert(++actionColumn,onFail);
            ioColumnMapper.insert(++actionColumn,dumpList);
            ioColumnMapper.insert(actionColumn, requirements.join(" "));
        }
    }

}

void GTAElementViewModel::setIndentedView(bool indentedView)
{
    _statusVar = indentedView;
}

bool GTAElementViewModel::getIndentedView() const
{
    return _statusVar;
}

int GTAElementViewModel::blockCheck(int *prow)const
{

    int row = *prow;
    //    int else_position = 0;
    //    int endIF_position=0;

    bool bOK = false;
    int else_count = 0;

    if( _pElementModel->hasIndex(row))
    {
        GTACommandBase* pCommand = NULL;
        bOK=_pElementModel->getCommand(row,pCommand);

        if(pCommand != NULL)
        {
            GTACommandBase* parentCmd = pCommand->getParent();
            GTACommandBase* masterCmd = pCommand;

            if (parentCmd == NULL)
            {
                else_count = 0;
                //                return else_count;
            }
            else
            {
                while(parentCmd != NULL)
                {
                    GTAActionElse* pElse = dynamic_cast<GTAActionElse*>(parentCmd);
                    if(pElse != NULL)
                    {
                        else_count++;
                    }
                    masterCmd = parentCmd;
                    parentCmd = parentCmd->getParent();

                }
            }
        }
    }
    return else_count;
}



//------------------------------- Indentation implementation --------------------------------

