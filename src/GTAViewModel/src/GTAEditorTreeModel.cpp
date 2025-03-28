#include "GTAEditorTreeModel.h"
#include "GTAActionBase.h"
#include "GTACheckBase.h"
#include "GTACommandBase.h"

#pragma warning(push, 0)
#include <QDebug>
#include <QPixmap>
#pragma warning(pop)

GTAEditorTreeModel::GTAEditorTreeModel(const GTACommandBase * ipCmd, QObject *parent) :
        QAbstractItemModel(parent)
{
    
    _pElement = new GTAElement(GTAElement::CUSTOM);
    _ColumnList<<"Action"<<"Action On Fail"<<"Time Out"<<"Precision"<<"Comment";
    GTACommandBase* pClone = ((GTACommandBase*)ipCmd)->getClone();
    QString statement = pClone->getStatement();
    pClone->setParent(NULL);
    _pElement->insertCommand(pClone,0,false);
    _pElement->setName(statement);
    setObjectName(statement);
}

GTAEditorTreeModel::GTAEditorTreeModel(GTAElement *ipElement, QObject *parent) : QAbstractItemModel(parent)
{

    _pElement = ipElement;
    _ColumnList<<"Action"<<"Action On Fail"<<"Time Out"<<"Precision"<<"Comment"<<"Dump List";
}
GTAEditorTreeModel::~GTAEditorTreeModel()
{
    //if(_pElement!=NULL && _pElement->getElementType()==GTAElement::CUSTOM)
	if(_pElement!=NULL)
    {
        delete _pElement;
        _pElement = NULL;
    }
}
int GTAEditorTreeModel::rowCount(const QModelIndex &parent) const
{
    //Why this is 1 here
    // Original Code : int childCount = 1;
    int childCount = 0;
    if(parent.column() > 0)
        return 0;
    if(!parent.isValid() )
    {
        if( _pElement != NULL)
            childCount = _pElement->getDirectChildrenCount();
    }
    else
    {
        GTACommandBase * pCmd = static_cast<GTACommandBase*>(parent.internalPointer());
        if(pCmd!= NULL)
        {

            QList<GTACommandBase *> lst = pCmd->getChildren();
            if(!lst.isEmpty())
            {
                childCount = lst.count();
            }

        }
    }
    //Why this is +1 here
    // Original Code : return childCount+1;
    //IKM Code
    return childCount;
}
int GTAEditorTreeModel::columnCount(const QModelIndex &) const
{
    return _ColumnList.count();
}
QModelIndex GTAEditorTreeModel::index(int row, int column, const QModelIndex &parent) const
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
        if(pCmd != NULL)
            return createIndex(row,column,pCmd);
        else
            return QModelIndex();
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
        if(pCmd != NULL)
            return createIndex(row,column,pCmd);
        else
            return QModelIndex();
    }

    
}

QModelIndex GTAEditorTreeModel::parent(const QModelIndex &index) const
{
    QModelIndex oIndex = QModelIndex();
    if(!index.isValid())
        return oIndex;

    GTACommandBase *pChildCmd = static_cast<GTACommandBase*>(index.internalPointer());


    if(pChildCmd != NULL)
    {
       	GTACommandBase * pParent = pChildCmd->getParent();
        if(pParent != NULL)
        {
            QList<GTACommandBase*> & childList = pParent->getChildren();
            GTACommandBase* pGrandParent = pParent->getParent();
            if(pGrandParent==NULL)
            {
                int row = 0;
                _pElement->getDirectChildrenIndex(pParent,row);
                oIndex =   QAbstractItemModel::createIndex(row,0,pParent);
            }
            else
            {
                int row = childList.indexOf(const_cast<GTACommandBase*>(pParent));
                if(row< 0)
                    row=0;
                oIndex =   QAbstractItemModel::createIndex(row,0,pParent);
            }
            
        }
    }
    return oIndex;
}
QVariant GTAEditorTreeModel::data(const QModelIndex &index, int role) const
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
            QString data;
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
            else if (column==5)
            {
                data = getDumpList(pCmd);
            }
            return QVariant(data);
        }
        break;
	
    case Qt::DecorationRole:
        {
            if(pCmd != NULL && index.column() == 0)
            {
                //GTACommandBase::CommandType cmdType = pCmd->getCommandType();
                //switch(cmdType)
                //{
                //case GTACommandBase::ACTION: return QVariant(QPixmap("C:/Users/nakushwac474/Projects/FlightOperation/GTA_WS02072015/SourceCode/GTAApp/forms/img/ACTION.png"));
                // //case GTACommandBase::ACTION: return QVariant(QPixmap(":/images/ACTION"));
                //case GTACommandBase::CHECK: return QVariant(QPixmap("C:/Users/nakushwac474/Projects/FlightOperation/GTA_WS02072015/SourceCode/GTAApp/forms/img/CHECK.png"));
                //default: return QVariant();
                //}
                return QVariant();
            }
        }
    case Qt::ForegroundRole:

        if(pCmd != NULL && index.column()==0 )
        {
            if( pCmd->hasReference())
                return pCmd->getForegroundColor();
            else 
            {
                GTACommandBase::CommandType cmdType = pCmd->getCommandType();
                if (cmdType==GTACommandBase::CHECK)
                    return QColor(0,128,0,200);
            }
        }
        
        return QVariant();


	 case Qt::BackgroundRole:
        {
            QColor oColor;
            if(pCmd != NULL)
            {
                oColor =  pCmd->getBackgroundColor();
            }

            if(oColor.isValid())
                return oColor;
            else  if (index.row()%2==0)
                return QColor(0,10,0,15);
            else
                return QVariant();
            break;
        }
	 default: return QVariant();
         }
}
Qt::ItemFlags GTAEditorTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
QVariant GTAEditorTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
        {
            if (orientation == Qt::Horizontal )
                return _ColumnList.at((section != -1) && (section < _ColumnList.count())  ? section : 0);

            if (orientation == Qt::Vertical)
                return QVariant(QString::number(section));
        }

    }
    return QVariant();
}
QString GTAEditorTreeModel::getActionOnFail(GTACommandBase * pCmd) const
{
    QString oVal;
    if(pCmd == NULL)
        return oVal;

    if( GTACommandBase::ACTION == pCmd->getCommandType())
    {
        GTAActionBase * pAction = dynamic_cast<GTAActionBase*>(pCmd);
        if(pAction != NULL)
            oVal = pAction->getActionOnFail();
    }
    else if(GTACommandBase::CHECK == pCmd->getCommandType())
    {
        GTACheckBase * pCheck = dynamic_cast<GTACheckBase*>(pCmd);
        if(pCheck != NULL)
            oVal = pCheck->getActionOnFail();
    }
    return oVal;
}

QString GTAEditorTreeModel::getTimeOut(GTACommandBase * pCmd) const
{
    QString oVal;
    if(pCmd == NULL)
        return oVal;

    if( GTACommandBase::ACTION == pCmd->getCommandType())
    {
        GTAActionBase * pAction = dynamic_cast<GTAActionBase*>(pCmd);
        if(pAction != NULL)
        {
            QString timeOut,unit;
            pAction->getTimeOut(timeOut,unit);
            if(!timeOut.isEmpty())
                oVal = QString("%1 %2").arg(timeOut,unit);
        }
    }
    else if(GTACommandBase::CHECK == pCmd->getCommandType())
    {
        GTACheckBase * pCheck = dynamic_cast<GTACheckBase*>(pCmd);
        if(pCheck != NULL)
        {
            QString timeOut,unit;
            pCheck->getTimeOut(timeOut,unit);
            if(!timeOut.isEmpty())
                oVal = QString("%1 %2").arg(timeOut,unit);
        }
    }
    return oVal;
}
QString GTAEditorTreeModel::getComment(GTACommandBase * pCmd) const
{
    QString oVal;
    if(pCmd != NULL)
        oVal= pCmd->getComment();
    return oVal;
}
QString GTAEditorTreeModel::getDumpList(GTACommandBase * pCmd) const
{
    QString oVal;
    if(pCmd != NULL)
        oVal= pCmd->getDumpList().join(",\n");
    return oVal;
}
QString GTAEditorTreeModel::getPrecision(GTACommandBase * pCmd) const
{
    QString oVal;
    if(pCmd == NULL)
        return oVal;

    if( GTACommandBase::ACTION == pCmd->getCommandType())
    {
        GTAActionBase * pAction = dynamic_cast<GTAActionBase*>(pCmd);
        if(pAction != NULL)
        {
            QString precision,type;
            pAction->getPrecision(precision,type);
            if(!precision.isEmpty())
                oVal = QString("%1 %2").arg(precision,type);
        }
    }
    else if(GTACommandBase::CHECK == pCmd->getCommandType())
    {
        GTACheckBase * pCheck = dynamic_cast<GTACheckBase*>(pCmd);
        if(pCheck != NULL)
        {
            QString precision,type;
            pCheck->getPrecision(precision,type);
            if(!precision.isEmpty())
                oVal = QString("%1 %2").arg(precision,type);
        }
    }
    return oVal;
}
