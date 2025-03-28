#include "GTATitleBasedExecutionModel.h"

GTATitleBasedExecutionModel::GTATitleBasedExecutionModel(GTAElement *pElement, QObject *pParent):
    GTAEditorTreeModel(pElement,pParent)
{
    _ColumnList.clear();
    _ColumnList << "Action";
}


void GTATitleBasedExecutionModel::LayoutAboutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void GTATitleBasedExecutionModel::LayoutChanged()
{
    emit layoutChanged();
}

bool GTATitleBasedExecutionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool rc = false;
    GTACommandBase *pCmd = nodeFromIndex(index);
    switch(role)
    {

    case Qt::CheckStateRole :

        if(pCmd)
        {
            //            if(pCmd->IsUserEditable())
            //            {
            emit layoutAboutToBeChanged();

            int val = value.toInt();

            if((Qt::CheckState)val == Qt::Checked)
            {
                // isChecked = true;
                pCmd->setIgnoreInSCXML(false);


                GTACommandBase * pParent = NULL;
                pParent = pCmd->getParent();
                if(pParent && pParent->isIgnoredInSCXML())
                {
                    pParent->setIgnoreInSCXML(false,GTACommandBase::NON_RECURSIVE);
                }

            }
            else if((Qt::CheckState)val == Qt::Unchecked)
            {
                pCmd->setIgnoreInSCXML(true);

                GTACommandBase * pParent = NULL;
                pParent = pCmd->getParent();
                if(pParent)
                {
                    if(pParent->areAllChilrenIgnored())
                        pParent->setIgnoreInSCXML(true);
                }
            }

            rc = true;
            emit layoutChanged();
            //            }

        }

        break;
    }
    return rc;
}

QVariant GTATitleBasedExecutionModel::data(const QModelIndex &index, int role) const
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

        return QVariant(data);
    }
    break;

    case Qt::CheckStateRole:
    {
        if (index.isValid() && index.column()==0)
        {
            QVariant val = Qt::Checked;
            if(pCmd->isIgnoredInSCXML())
            {
                val =  Qt::Unchecked;
            }
            return val;
        }
        else
            return QVariant();
    }


    case Qt::ForegroundRole:
    {
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
    }

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

Qt::ItemFlags GTATitleBasedExecutionModel::flags ( const QModelIndex & index ) const
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


GTACommandBase * GTATitleBasedExecutionModel::nodeFromIndex(const QModelIndex &index) const
{
    GTACommandBase *pCmd = NULL;
    if(index.isValid())
        pCmd = static_cast<GTACommandBase*>(index.internalPointer());

    return pCmd;
}


GTAElement * GTATitleBasedExecutionModel::getElement()const
{
    return _pElement;
}
