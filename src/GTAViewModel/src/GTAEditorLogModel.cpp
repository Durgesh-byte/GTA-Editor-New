#include "GTAEditorLogModel.h"
#include "GTAActionExpandedCall.h"
#include "GTAActionIRT.h"
#include "GTAActionFTAKinematicMultiOutput.h"
#include "GTAActionParentTitle.h"
#include "GTACommandBase.h"
#include "GTACheckBase.h"
#include "GTAActionBase.h"

GTAEditorLogModel::GTAEditorLogModel(GTAElement *ipElement, QObject *parent) : GTAEditorTreeModel(ipElement,parent)
{
    _ColumnList.clear();
    _ColumnList<<"Action"<<"Action On Fail"<<"Time Out"<<"Precision"<<"Comment"<<"Result"<<"Current Value"<<"Expected Value"<<"Dump List"<<"Defects"<<"User Feedback"<<"Execution Time"<<"UTC";

}
int GTAEditorLogModel::columnCount(const QModelIndex &) const
{
    return  _ColumnList.count();
}

QVariant GTAEditorLogModel::data(const QModelIndex &index, int role) const
{
    if(! index.isValid())
        return QVariant();

    GTACommandBase *pCmd = static_cast<GTACommandBase*>(index.internalPointer());
    if(pCmd == NULL)
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
    {
        int column = index.column();
        QString data = QString();
        if(column == 0)
            data = pCmd->getStatement();
        else if(column == 1)
            data = getActionOnFail(pCmd);
        else if(column == 2)
            data = getTimeOut(pCmd);
        else if(column == 3)
            data = getPrecision(pCmd);
        else if (column==4)
            data= getComment(pCmd);
        else if(column == 5)
        {
            data = QString("NA");
            //            QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
            //            foreach(GTAScxmlLogMessage objLog, msgList)
            //            {
            //                if(objLog.LOD == GTAScxmlLogMessage::Main)
            //                {
            //                    data =  objLog.ResultTypeToString(objLog.Result);
            //                }
            //            }
            data = pCmd->getGlobalResultStatus();
            if(pCmd->hasReference())
            {
                GTAActionExpandedCall *pCallCmd = static_cast<GTAActionExpandedCall*>(index.internalPointer());
                if(pCallCmd != NULL)
                    data = pCallCmd->getGlobalResultStatus();
            }
            if(pCmd->createsCommadsFromTemplate())
            {
                GTAActionFTAKinematicMultiOutput *pKinematicCmd = static_cast<GTAActionFTAKinematicMultiOutput *>(index.internalPointer());
                if(pKinematicCmd!= NULL)
                    data = pKinematicCmd->getGlobalResultStatus();
            }
            if(pCmd->isTitle())
            {
                GTAActionParentTitle *pTitle = static_cast<GTAActionParentTitle*>(index.internalPointer());
                if(pTitle != NULL)
                    data = pTitle->getGlobalResultStatus();
            }

        }

        else if(column == 6)
        {
            QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
            foreach(GTAScxmlLogMessage objLog, msgList)
            {
                if(objLog.LOD == GTAScxmlLogMessage::Main)
                {
                    data =  objLog.CurrentValue;
                }
            }
        }
        else if(column == 7)
        {
            QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
            foreach(GTAScxmlLogMessage objLog, msgList)
            {
                if(objLog.LOD == GTAScxmlLogMessage::Main)
                {
                    data =  objLog.ExpectedValue;
                }
            }
        }
        else if (column == 8)
        {
            QStringList paramValList;

            QStringList dumpList=pCmd->getDumpList();
            /*if (pCmd!=NULL)
                {
                    dumpList = pCmd->getDumpList().join("\n");
                }*/



            if(!dumpList.isEmpty())
            {
                QString items;
                QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
                for(const auto& parameter : dumpList)
                {
                    QString sValue;
                    for (const auto& objLog : msgList)
                    {
                        if(objLog.LOD == GTAScxmlLogMessage::Detail)
                        {
                            //                                items.append ( QString("\nC:%1E:%2").arg(objLog.CurrentValue,objLog.ExpectedValue));
                            QString currVal = objLog.CurrentValue;
                            if(!currVal.isEmpty() && objLog.ExpectedValue.contains(parameter))
                            {
                                QStringList valLst = currVal.split("\"Value\":");
                                if (valLst.size()>1)
                                {
                                    sValue = valLst.at(1);
                                    sValue = sValue.remove("}");
                                }
                                else
                                    sValue=currVal;

                            }
                        }
                    }

                    if (!sValue.isEmpty())
                    {
                        paramValList.append(QString("%1=%2").arg(parameter,sValue));
                    }
                }

            }
            data=paramValList.join("\n");
        }
        else if(column == 9)
        {

            //            QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
            //            GTAActionIRT* pIRT = dynamic_cast<GTAActionIRT*>(pCmd);
            //            if (pIRT && !msgList.isEmpty())
            //            {
            //                //to do if only image info is there this information should be displayed
            //                // pIRT->getLogMessageList()
            //                data = QString("DoubleClick to view image");


            //}
            data = pCmd->getReferences();
        }
        else if(column == 10)
        {
            data = pCmd->getUserComment();

        }
        else if(column == 11)
        {
            data = pCmd->getExecutionTime();
        }
        else if(column == 12)
        {
            double value = pCmd->getExecutionEpochTime();
            QVariant vEpochTime(value);
            return vEpochTime;
        }
        if(data.isEmpty())
            return QVariant();

        return QVariant(data);
    }
    break;
    case Qt::CheckStateRole:
    {
        if (index.isValid() && index.column()==0)
        {
            QVariant val = Qt::Unchecked;
            if(!pCmd->isIgnoredInSCXML())
            {
                if (pCmd->getReportStatus()==GTACommandBase::INCLUDE)
                    val =  Qt::Checked;
                else if(pCmd->getReportStatus()==GTACommandBase::EXCLUDE)
                    val =  Qt::Unchecked;
                else if (pCmd->getReportStatus()==GTACommandBase::PARTIALLY_INCLUDE)
                    val =  Qt::PartiallyChecked;
                else if(!pCmd->hasChildren() && pCmd->hasReference())       //done to handle empty call commands
                {
                    val = Qt::Unchecked;
                    pCmd->setReportStatus(GTACommandBase::EXCLUDE);
                }
                else
                    val =  Qt::Checked;
            }
            else
            {
                val = Qt::Unchecked;
                pCmd->setReportStatus(GTACommandBase::EXCLUDE);
            }
            return val;
        }
        else
            return QVariant();
    }
    case Qt::ToolTipRole:
        if(index.column() == 6 || index.column() == 7)
        {
            GTAActionIRT* pIRT = dynamic_cast<GTAActionIRT*>(pCmd);
            if (pIRT)
            {
                return QVariant();
            }
            QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
            QStringList detLogs;
            foreach(GTAScxmlLogMessage objLog, msgList)
            {
                if(objLog.LOD == GTAScxmlLogMessage::Detail)
                {
                    QString curVal = objLog.CurrentValue;
                    QString expVal = objLog.ExpectedValue;
                    QString msg = QString("%1 : %2").arg(expVal, curVal);
                    detLogs.append(msg);
                }
            }
            if(!detLogs.isEmpty())
            {
                QString toolTip = "Variable : Value\n";
                toolTip += detLogs.join("\n");
                return QVariant(toolTip);
            }
        }
        break;
    case Qt::DecorationRole:
    {
        if(pCmd != NULL && index.column() == 0)
        {
            return QVariant();
        }
    }
    break;
    case Qt::FontRole:

    {
        QFont font;
        if (pCmd!=NULL && pCmd->isIgnoredInSCXML())
            font.setStrikeOut(true);
        return font;
        break;
    }

    case Qt::ForegroundRole:
        if(pCmd != NULL)
        {
            GTACommandBase::CommandType cmdType = pCmd->getCommandType();


            if(index.column()==0 && pCmd->hasReference())
            {
                return pCmd->getForegroundColor();
            }
            else if (cmdType==GTACommandBase::CHECK)
            {
                QList<GTAScxmlLogMessage> msgList = pCmd->getLogMessageList();
                foreach(GTAScxmlLogMessage objLog, msgList)
                {
                    if(objLog.LOD == GTAScxmlLogMessage::Main)
                    {
                        QString data =  objLog.ResultTypeToString(objLog.Result);
                        if(data == "OK")
                            return QColor(0,128,0,200);
                        else if(data == "KO")
                            return QColor(128,0,0,200);
                        else
                            return QColor(128,128,128,200);

                    }
                }
            }
            if(index.column() == 5)
            {
                if(index.data() == QVariant("OK"))
                {
                    return QColor(43,97,0);
                }
                else if(index.data() == QVariant("KO"))
                {
                    return QColor(136,0,21);
                }
                else if(index.data() == QVariant("NA"))
                {
                    return QColor(64,64,109);
                }

                /*
LGrey : 191,191,191
Grey : 64,64,109

LOrange : 250,191,143
Orancge , 167,54,68
                  */

            }
            if(index.column() == 8)
            {
                return QColor(0,0,200,70);
            }

           
            QColor color = pCmd->getForegroundColor();
            if (color != Qt::black && (pCmd->isForegroundColor() && index.column() != 0)){
                color = Qt::black; 
            }
            return color;
        }
        return QVariant();
        break;
    case Qt::BackgroundRole:

        if(index.column() == 5)
        {
            if(index.data() == QVariant("OK"))
            {
                return QColor(198,239,206);
            }
            else if(index.data() == QVariant("KO"))
            {
                return QColor(255,199,206);
            }
            else if(index.data() == QVariant("NA"))
            {
                return QColor(191,191,191);
            }
            else
            {
                QColor oColor =  pCmd->getBackgroundColor();
                if(oColor.isValid())
                    return oColor;
                else  if (index.row()%2==0)
                    return QColor(0,10,0,15);
                else
                    return QVariant();
                break;
            }
        }
        else if(pCmd != NULL)
        {
            QColor oColor =  pCmd->getBackgroundColor();
            if(oColor.isValid())
                return oColor;
            else  if (index.row()%2==0)
                return QColor(0,10,0,15);
            else
                return QVariant();
            break;
        }
        return QVariant();
        break;

    default: return QVariant();
    }
    return QVariant();
}

/**
 * @brief Update purpose&conclusion fields
 * @param iElement 
 */
void GTAEditorLogModel::updateElement(const GTAElement* iElement)
{
    _pElement->setPurposeForProcedure(iElement->getPurposeForProcedure());
    _pElement->setConclusionForProcedure(iElement->getConclusionForProcedure());
}

GTAElement * GTAEditorLogModel::getDataModel() const
{
    return _pElement;
}
QModelIndex GTAEditorLogModel::index(int row, int column, const QModelIndex &parent) const
{
    if(! hasIndex(row,column,parent))
        return QModelIndex();

    GTACommandBase * pCmd = NULL;
    if(! parent.isValid())
    {
        if(_pElement!= NULL)
        {
            _pElement->getDirectChildren(row,pCmd);
        }
        else
        {
            return QModelIndex();
        }
    }
    else
    {
        GTACommandBase * pParent = static_cast<GTACommandBase*>(parent.internalPointer());
        if(pParent != NULL)
        {
            QList<GTACommandBase *> & childList= pParent->getChildren();
            if(childList.count() > row)
                pCmd= childList.at(row);
            else
                return QModelIndex();
        }
        else
        {
            return QModelIndex();
        }
    }
    if(pCmd != NULL)
        return createIndex(row,column,pCmd);
    else
        return QModelIndex();
}
//TODO: implement set data with code that uses datachanged api.
bool GTAEditorLogModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
    bool rc = false;

    if (!index.isValid())
        return rc;

/*    int row = index.row();*/
    int col = index.column();

    QModelIndex topLeftIdx = index;
    QModelIndex bottomRightIdx = index;//this->index(index.row(),columnCount(index.parent()),index.parent());//createIndex(index.row(),columnCount(index));
    GTACommandBase *pCmd = static_cast<GTACommandBase*>(index.internalPointer());

    if(role == Qt::EditRole && pCmd != NULL)
    {

        if( GTACommandBase::ACTION == pCmd->getCommandType())
        {
            GTAActionBase * pAction = dynamic_cast<GTAActionBase*>(pCmd);
            if(pAction != NULL)
            {
                if(col == 5)
                {
                    pAction->setExecutionResult(value.toString());
                }
                if(col == 11)
                {
                    pAction->setExecutionTime(value.toString());
                }
                if(col == 9)
                {
                    pAction->setReferences(value.toString());
                }
                if(col == 10)
                {
                    pAction->updateUserComments(value.toString());
                }
            }
        }
        else if(GTACommandBase::CHECK == pCmd->getCommandType())
        {
            GTACheckBase * pCheck = dynamic_cast<GTACheckBase*>(pCmd);
            if(pCheck != NULL)
            {

                if(col == 5)
                {
                    pCheck->setExecutionResult(value.toString());
                }
                if(col == 11)
                {
                    pCheck->setExecutionTime(value.toString());
                }
                if(col == 9)
                {
                    pCheck->setReferences(value.toString());
                }
                if(col == 10)
                {
                    pCheck->updateUserComments(value.toString());
                }
            }
        }
    }

    if (role==Qt::CheckStateRole && pCmd!=NULL)
    {
        emit layoutAboutToBeChanged ();
        if ( (Qt::CheckState)value.toInt()==Qt::Checked )
        {

            pCmd->setReportStatus(GTACommandBase::INCLUDE);
            //emit dataChanged(topLeftIdx,bottomRightIdx);
            if (pCmd->hasChildren())
            {
                //setData(this->index(i,0,index),value,Qt::CheckStateRole);
                pCmd->setReportStatusOfChildrens(GTACommandBase::INCLUDE);
            }



            GTACommandBase* pParent = pCmd->getParent();
            //pCmd->setReportStatusOfChildrens(GTACommandBase::INCLUDE);
            if (pParent!=NULL)
            {
                pParent->updateReportStatus();
                updateAllParents(index);

            }
            if (pCmd->hasChildren())
                updateAllChildrens(index);

            rc=true;
        }
        else if( (Qt::CheckState)value.toInt()==Qt::Unchecked )
        {

            pCmd->setReportStatus(GTACommandBase::EXCLUDE);
            //emit dataChanged(topLeftIdx,bottomRightIdx);
            if (pCmd->hasChildren())
            {
                //setData(this->index(i,0,index),value,Qt::CheckStateRole);
                pCmd->setReportStatusOfChildrens(GTACommandBase::EXCLUDE);
                //}
            }

            GTACommandBase* pParent = pCmd->getParent();
            // pCmd->setReportStatusOfChildrens(GTACommandBase::EXCLUDE);

            if (pParent!=NULL)
            {
                pParent->updateReportStatus();
                updateAllParents(index);

            }
            if (pCmd->hasChildren())
                updateAllChildrens(index);


            rc=true;
        }
        else if( (Qt::CheckState)value.toInt()==Qt::PartiallyChecked )
        {
            // pCmd->setReportStatus(GTACommandBase::PARTIALLY_INCLUDE);
            updateAllParents(index);
            rc=true;
        }

        // emit dataChanged(QModelIndex(),QModelIndex());

        emit layoutChanged();

    }
    else if(role == Qt::DisplayRole && pCmd != NULL)
    {

    }

    return rc;
}

Qt::ItemFlags GTAEditorLogModel::flags ( const QModelIndex & index ) const
{

    Qt::ItemFlags itemFlags;
    itemFlags = QAbstractItemModel::flags(index);

    itemFlags |= Qt::ItemIsEnabled | Qt::ItemIsUserCheckable|itemFlags;
    if(index.isValid())
    {
        //This is to handle chekability for call commands which do not have children (would happen in case of recursive calls)
        //So for such call commands it has been unchecked by default or else crashes.
        GTACommandBase * pCmd = static_cast<GTACommandBase*>(index.internalPointer());
        if(pCmd && pCmd->hasReference())
        {
            if(pCmd->getAllChildrenCount() == 0)
            {
                itemFlags = Qt::NoItemFlags;
            }
        }
    }

    return itemFlags;
}

//TODO Remove empty methods
void GTAEditorLogModel::updateAllParents(const QModelIndex& )
{
}
void GTAEditorLogModel::updateAllChildrens(const QModelIndex& )
{
}
