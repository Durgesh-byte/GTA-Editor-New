#include "GTAHeaderTableModel.h"

#pragma warning(push, 0)
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QMap>
#pragma warning(pop)

GTAHeaderTableModel:: GTAHeaderTableModel(GTAHeader* ipHeader, QObject* iParent):QAbstractTableModel(iParent),_pHeader(ipHeader)
 {
    

 }
GTAHeaderTableModel:: ~GTAHeaderTableModel()
{

}
QVariant GTAHeaderTableModel::  data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QString snoData=QString(" ");
    int row = index.row();
    int col = index.column();

    int hdrInfoCnt = _pHeader->itemSize();
    QString sCellValue;
    QString toolTip;
    //bool fetchSuccess=false;

    if (index.column() == 2 && role == Qt::CheckStateRole)
    {
		bool checked = _pHeader->getShowInLTRA(row);
		if (checked)
		{
			return QVariant(Qt::Checked);
		}
		else
		{
			return QVariant(Qt::Unchecked);
		}
    }

    switch (role)
    {
    case Qt::DisplayRole:

        if (row < hdrInfoCnt)
        {
            if (row ==0 && col==0)
                sCellValue = HEADER_NAME;
            else if (row ==0 && col ==1)
                sCellValue = _pHeader->getName();
            else
            {
                if (col==0)
                    sCellValue= _pHeader->getFieldName(row);
                if (col==1)
                    sCellValue=_pHeader->getFieldValue(row);//,index.column()-1,fetchSuccess);
                if (col == 2)
                {
                    sCellValue = "";
                }
                if (col == 0 && !showLTRAStateChangeAllowed(_pHeader->getFieldName(row)))
                {
                    sCellValue += "*";
                }
            }

        }
        else
        {
            int fileInfoIndex = row - hdrInfoCnt;

            switch (col)
            {
            case 0:   sCellValue = _pFileInfo.at(fileInfoIndex)._Equipment;
                break;
            case 1:   sCellValue = _pFileInfo.at(fileInfoIndex)._filename;
                break;
            
            default:   sCellValue = QString();
                break;
            }
        }


        
        if (sCellValue.isEmpty())
            return QVariant();
        return QVariant(sCellValue);

        break;

    case Qt::EditRole:


        if (row < hdrInfoCnt)
        {
            if (row ==0 && col==0)
                sCellValue = HEADER_NAME;
            else if (row ==0 && col ==1)
                sCellValue = _pHeader->getName();
            else
            {
                if (col==0)
                    sCellValue= _pHeader->getFieldName(row);
                if (col==1)
                    sCellValue=_pHeader->getFieldValue(row);//,index.column()-1,fetchSuccess);
                if (col == 2)
                {
                    sCellValue = "";
                }
            }

        }
        if (sCellValue.isEmpty())
            return QVariant();
        return QVariant(sCellValue);

        break;

    case Qt::TextAlignmentRole:

        return int(Qt::AlignLeft | Qt::AlignVCenter);
        break;

    case Qt::ForegroundRole:
        {            
            if (index.column()==0)
            {
                if (_pHeader->getUnfilledMandatoryFieldRows().contains(index.row()))
                    return QColor(255, 0, 0, 30); /*Qt::red;*/
                else
                    return QColor(0, 0, 255, 255); /*Qt::blue;*/ //Qt::lightGray;//QColor(128,0,0,50);
            }

            if (index.column() == 2 && (!showLTRAStateChangeAllowed(_pHeader->getFieldName(row)) || 
                _pHeader->isNodeMandatory(_pHeader->getFieldName(row))))
            {
                return QColor(0, 0, 0, 70);
            }

            break;
        }
    case Qt::FontRole:
        {
            int sub_row = index.row();
            QString sFieldName = _pHeader->getFieldName(sub_row);
            bool isMandatoryStatus = _pHeader->isNodeMandatory(sFieldName);

            QFont fDefaultFont( "SansSerif" );
            if(isMandatoryStatus) 
            {
               fDefaultFont.setBold(true);
            }

            if (col == 2)
            {
                fDefaultFont.setBold(false);
            }

            return fDefaultFont;
        }
      
    case Qt::BackgroundRole:
        {
            int sub_row = index.row();
            QString sFieldName = _pHeader->getFieldName(sub_row);
            _pHeader->isNodeMandatory(sFieldName);

            if (index.column()==0)
                return QColor(0,0,255,10);//Qt::lightGray;//QColor(128,0,0,50);
            if (index.column()==1)
            {
                int hdrItemSize =_pHeader->itemSize();
                if (row >=hdrItemSize )
                {
                    return QColor(128, 0, 0, 50); /*Qt::lightGray;*/
                }
            }
        }
       
    case Qt::ToolTipRole:
        if (index.column()==0)
        {
            int hdrItemSize =_pHeader->itemSize();
            if (row <hdrItemSize )
            {
                  toolTip =_pHeader->getFieldDescription(row);
            }
            else
            {
                int idxR = row - hdrItemSize;
                GTAFILEInfo fInfo = _pFileInfo.at(idxR);
                QString appNames = fInfo._Application.join(", ");
                toolTip = QString("ICD Path: %1\nFile Type: %2\nApplication Used: %3\n").arg(fInfo._filePath,fInfo._fileType,appNames);
            }
          
            return QVariant(toolTip);
        }
        else if (index.column() == 2 && (!showLTRAStateChangeAllowed(_pHeader->getFieldName(row)) || 
            _pHeader->isNodeMandatory(_pHeader->getFieldName(row))))
        {
            toolTip = "Mandatory Field for LTRA Export";
            return QVariant(toolTip);
        }
        else
            return QVariant();

    default:
        return QVariant();
    }

    return QVariant();
}
//QVariant GTAHeaderTableModel::  data(const QModelIndex & index, int role) const
//{
//	if (!index.isValid())
//		return QVariant();
//   
//	QString snoData=QString(" ");
//    int row = index.row();
//	int col = index.column();
//
//	 QString sToDo("Empty to be filled");
//	 QString sCellValue;
//	 QString toolTip="Test";
//	 QString testCellValue;
//
//	 bool fetchSuccess=false;
//	switch (role)
//	{
//		case Qt::DisplayRole:
//
//			if (index.column()==0)
//				sCellValue= _pHeader->getTagName(index.row());
//			else
//				sCellValue=_pHeader->getValue(index.row(),index.column()-1,fetchSuccess);
//
//			if (sCellValue.isEmpty())
//				return QVariant();
//			return QVariant(sCellValue);
//			
//			break;
//
//		case Qt::EditRole:
//			
//			if (index.column()==0)
//				sCellValue= _pHeader->getTagName(index.row());
//			else
//				sCellValue=_pHeader->getValue(index.row(),index.column()-1,fetchSuccess);
//			if (sCellValue.isEmpty())
//				return QVariant();
//			return QVariant(sCellValue);
//
//			break;
//
//		case Qt::TextAlignmentRole:
//
//			return int(Qt::AlignLeft | Qt::AlignVCenter);
//			break;
//
//		/*case Qt::CheckStateRole:
//
//			return QVariant(false);
//			break;*/
//
//		case Qt::ForegroundRole:
//			if (index.column()==0)
//				return Qt::blue;
//			/*if (_pHeader->getValue(index.row(),index.column()) == HEADER_VALUE_EMPTY)
//				return Qt::red; QColor(255,0,0,70);*/
//				break;
//		case Qt::BackgroundRole:
//			if (index.column()==0)
//				return QColor(0,0,255,30);//Qt::lightGray;//QColor(128,0,0,50);
//			_pHeader->getValue(index.row(),index.column()-1,fetchSuccess);
//			if (!fetchSuccess)
//				return Qt::lightGray;
//		case Qt::ToolTipRole:
//			if (index.column()==0)
//			{
//				toolTip="Informations:\n² designate mandatory fields for object and function redaction\n * designate mandatory fields for procedure redaction\n^ designate mandatory field for test plan creator\n° designate mandatory fields for test report redaction";
//				return QVariant(toolTip);
//			}
//			else
//				return QVariant();
//			
//		default:
//			return QVariant();
//	}
//
//return QVariant();
//}
int GTAHeaderTableModel:: columnCount(const QModelIndex & )const
{
	//return (_pHeader->getMaxChildID()+1);
    return 3;
}
int GTAHeaderTableModel:: rowCount(const QModelIndex & )const
{
    //return _pHeader->getMaxID()+1;
    int hdrItems = _pHeader->itemSize();
    int fileInfoItems = _pFileInfo.size();

    return (hdrItems + fileInfoItems);
}
QVariant GTAHeaderTableModel:: headerData ( int section, Qt::Orientation orientation, int role  ) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return tr("Header field");
		case 1:
			return tr("Field description");
		case 2:
			return tr("Export to LTRA");
		}
	}
	return QVariant();


}

bool GTAHeaderTableModel::setData ( const QModelIndex & index, const QVariant & value, int role  )
{
    bool rC=false;
	if (role==Qt::EditRole)
	{
         int row = index.row();
        if (row ==0 && index.column()==1)
        {
            _pHeader->setName(value.toString());
            return true;
        }
		if (index.column()==1)
		{
			QString fieldName = _pHeader->getFieldName(index.row());
            _pHeader->editField(fieldName,value.toString());
			return true;
		}

       
        headerNodeInfo hData = value.value<headerNodeInfo>();
       rC= _pHeader->editHeaderItem(row,
                                 hData._name,
                                 hData._Mandatory,
                                 hData._val,
                                 hData._description,
                                 hData._showInLTRA);
       // _pHeader->
	}

    if (index.column() == 2 && role == Qt::CheckStateRole)
    {
        QString fieldName = _pHeader->getFieldName(index.row());
        bool isMandatory = _pHeader->isNodeMandatory(fieldName);
        if (!isMandatory)
        {
            _pHeader->UpdateShowLTRAFlag(fieldName, value.toBool());
        }
            
        return true;
    }
	
	return rC;
}
Qt::ItemFlags GTAHeaderTableModel::flags ( const QModelIndex & index ) const
{
	Qt::ItemFlags itemFlags;
	itemFlags = QAbstractItemModel::flags(index);

	//bool fetchStatus=false;
	//_pHeader->getValue(index.row(),index.column()-1,fetchStatus);
	int hdrSize = _pHeader->itemSize();
	if (index.column()==0 || index.row()>=hdrSize)
	{		
		return itemFlags;
	}
	/*if( fetchStatus==false)
		return itemFlags;*/
	

	
    if (index.column() == 2 )
    {
        QString fieldName = _pHeader->getFieldName(index.row());
        bool isMandatory = _pHeader->isNodeMandatory(fieldName);
        if (!isMandatory)
        {
            if (showLTRAStateChangeAllowed(fieldName))
            {
                itemFlags |= Qt::ItemIsEnabled;
                itemFlags |= Qt::ItemIsUserCheckable;
            }
        }
    }
    else
    {
        itemFlags |= Qt::ItemIsEditable;
    }
    
	return itemFlags;

}
bool GTAHeaderTableModel::insertRows( int row, int count, const QModelIndex & parent)
{
    bool rc;
    if(!count)
        beginInsertRows(parent,row,row);
    else
        beginInsertRows(parent,row,row+count -1);

    rc = _pHeader->insertEmptyNodes(row,count);
    endInsertRows ();
    return rc;
}

bool GTAHeaderTableModel::removeRows( int row, int count, const QModelIndex & parent)
{
    beginRemoveRows(parent,row,row+count);
    bool rc = _pHeader->removeRows(row,count);
    endRemoveRows();
    return rc;
}

bool GTAHeaderTableModel::showICD(const bool& )
{
    return false;
}
void  GTAHeaderTableModel::setFileInfo(QList<GTAFILEInfo> iListFileInfo)
{
    int currentRows = _pHeader->itemSize();
    int newRows = iListFileInfo.size();

    if (newRows)
    {
        beginInsertRows(QModelIndex(),currentRows,currentRows+newRows-1);
        _pFileInfo=iListFileInfo;
        endInsertRows();
       
    }
    
}
void  GTAHeaderTableModel::removeFileInfo()
{
    int currentRows = _pHeader->itemSize();
    int noOfRowsToBeRemoved = _pFileInfo.size();
    if (noOfRowsToBeRemoved)
    {
        removeRows(currentRows,noOfRowsToBeRemoved-1,QModelIndex());
            _pFileInfo.clear();
    }
    //=iListFileInfo;

}

/**
 * @brief Checks If the state of ShowLTRA can be changed for the field name
 * @param strFieldName: Name of the field selected for update
 * @return bool
*/
bool GTAHeaderTableModel::showLTRAStateChangeAllowed(QString strFieldName) const
{
    bool ChangeAllowed = false;
    if (strFieldName != "Designer Name" && strFieldName != "Specification Issue/Requirement"
        && strFieldName != "Test_Number" && strFieldName != "Facility" 
        && strFieldName != "DESCRIPTION" && strFieldName != "LTRA_Reference_Number"
        && strFieldName != "LTRA_Title")
    {
        ChangeAllowed = true;
    }

    return ChangeAllowed;
}

/**
 * @brief Selects all the header fields to be exported in the LTRA report
*/
void GTAHeaderTableModel::SelectAllForExport()
{
	for (int rowIndex = 0; rowIndex < _pHeader->itemSize(); rowIndex++)
	{
		QString fieldName = _pHeader->getFieldName(rowIndex);

		_pHeader->UpdateShowLTRAFlag(fieldName, true);
	}
    
    QModelIndex topLeftIndex = this->createIndex(0, 0);
    QModelIndex bottomRightIndex = this->createIndex(_pHeader->itemSize(), 2);

    this->dataChanged(topLeftIndex, bottomRightIndex);
}

/**
 * @brief UnSelects all the non-mandatory header fields to be exported in the LTRA report
*/
void GTAHeaderTableModel::UnSelectAllForExport()
{
    for (int rowIndex = 0; rowIndex < _pHeader->itemSize(); rowIndex++)
    {
        QString fieldName = _pHeader->getFieldName(rowIndex);
        bool isMandatory = _pHeader->isNodeMandatory(fieldName);
        if (!isMandatory && showLTRAStateChangeAllowed(fieldName))
        {
            _pHeader->UpdateShowLTRAFlag(fieldName, false);
        }
    }

    QModelIndex topLeftIndex = this->createIndex(0, 0);
    QModelIndex bottomRightIndex = this->createIndex(_pHeader->itemSize(), 2);

    this->dataChanged(topLeftIndex, bottomRightIndex);
}