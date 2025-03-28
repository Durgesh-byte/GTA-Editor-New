#include "GTAValidationWidgetModel.h"

#pragma warning(push, 0)
#include <QMessageBox>
#pragma warning(pop)

GTAValidationWidgetModel::GTAValidationWidgetModel(GTAElement *pElement, QObject *pParent):
    GTAEditorTreeModel(pElement,pParent)
{}

void GTAValidationWidgetModel::LayoutAboutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void GTAValidationWidgetModel::LayoutChanged()
{
    emit layoutChanged();
}

bool GTAValidationWidgetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool rc = false;
    GTACommandBase *pCmd = nodeFromIndex(index);
    switch(role)
    {

    case Qt::CheckStateRole :

        if(pCmd)
        {
            emit layoutAboutToBeChanged();

            int val = value.toInt();

            if((Qt::CheckState)val == Qt::Checked)
            {
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
                if (pParent)
                {
                    if (pParent->areAllChilrenIgnored())
                        pParent->setIgnoreInSCXML(true);
                }
            }

            rc = true;
            emit layoutChanged();
        }

        break;
    }
    return rc;
}

QVariant GTAValidationWidgetModel::data(const QModelIndex &index, int role) const
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

Qt::ItemFlags GTAValidationWidgetModel::flags ( const QModelIndex & index ) const
{

    Qt::ItemFlags itemFlags;
    itemFlags = QAbstractItemModel::flags(index);

    itemFlags |= Qt::ItemIsEnabled | Qt::ItemIsUserCheckable|itemFlags;
    return itemFlags;
}

GTACommandBase * GTAValidationWidgetModel::nodeFromIndex(const QModelIndex &index) const
{
    GTACommandBase *pCmd = NULL;
    if(index.isValid())
        pCmd = static_cast<GTACommandBase*>(index.internalPointer());

    return pCmd;
}

QStringList  GTAValidationWidgetModel::getElementUUID()const
{
    QStringList selectedUuidList;

    //preventing crash
    if (_pElement == NULL)
        return selectedUuidList;

    int count = _pElement->getAllChildrenCount();
    for(int i = 0; i < count; i++)
    {
        GTACommandBase *pCmd = NULL;
        _pElement->getCommand(i,pCmd);
        if(pCmd != NULL && !pCmd->isIgnoredInSCXML())
        {
            GTAActionExpandedCall *pExpCall = dynamic_cast<GTAActionExpandedCall *>(pCmd);
            if (pExpCall)
            {
                QString UUID = pExpCall->getRefrenceFileUUID();
                qDebug()<<pExpCall->getRefrenceFileUUID();
                qDebug()<<pExpCall->getObjId();
                selectedUuidList.append(UUID);
            }
        }
    }

    QMessageBox msgBox;
    msgBox.setText(QString("Do you want to mark %1.%2 as validated?").arg(_pElement->getName(),getExtensionForElmntType(_pElement->getElementType())));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle("Validation confirmation");
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    //if(ret==QMessageBox::Yes)
    /*int ret;
    ret = QMessageBox::question(this, "Validation confirmation",
                                    QString("Do you want to mark %1.%2 as validated?").arg(_pElement->getName(),getExtensionForElmntType(_pElement->getElementType())),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);*/
    if(ret == QMessageBox::Yes)
    {
        selectedUuidList.append(_pElement->getUuid());
    }
    selectedUuidList.removeDuplicates();
    return selectedUuidList;
}

QString  GTAValidationWidgetModel::getExtensionForElmntType(GTAElement::ElemType elemType)
{
    QString sExt;

    switch(elemType)
    {
    case GTAElement::OBJECT : sExt="obj";
        break;
    case GTAElement::FUNCTION : sExt="fun";
        break;
    case GTAElement::PROCEDURE : sExt="pro";
        break;
    case GTAElement::SEQUENCE : sExt="seq";
        break;
    case GTAElement::TEMPLATE : sExt="tmpl";
        break;
    default:
        ;
        break;
    }
    return sExt;
}

GTAElement * GTAValidationWidgetModel::getElement()const
{
    return _pElement;
}
