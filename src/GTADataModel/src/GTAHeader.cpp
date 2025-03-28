#include "GTAHeader.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QDomNode>
#include <QDomElement>
#include <QTextStream>
#include <QDebug>
#pragma warning(pop)

GTAHeader::GTAHeader(QDomDocument* ipDomDoc, QObject*)
{
    _pHeaderDom= ipDomDoc;
    _editMode=false;
	_GTAVersion.clear();

}

GTAHeader::GTAHeader()
{
    _pHeaderDom = NULL;
	_GTAVersion.clear();
}

GTAHeader::GTAHeader(const GTAHeader &iRhs)
{

	setName(iRhs.getName());
	setDescription(iRhs.getDescription());
	setElemName(iRhs.getElemName());

	QDomDocument *rhsHeaderDom = iRhs.getHeaderDom();
	if (rhsHeaderDom != NULL)
	{
		QDomNode node = rhsHeaderDom->cloneNode(true);
		QDomDocument lhsHeaderDom = node.toDocument();
		_pHeaderDom = &lhsHeaderDom;
	}
	else
	{
		_pHeaderDom = NULL;
	}

	_lstOfFields = iRhs.getLstOfFields();
	setMaxID(iRhs.getMaxID());
	setMaxChildrenID(iRhs.getMaxChildID());
	_editMode = iRhs.inEditMode();
	//    setEditMode(iRhs.inEditMode());
	setLastError(iRhs.getLastError());
	setUnfilledMandatoryFieldRows(iRhs.getUnfilledMandatoryFieldRows());
	setGTAVersion(iRhs.getGTAVersion());
}

GTAHeader::~GTAHeader()
{

}
void GTAHeader::setName(const QString & iName)
{
    _Name = iName;
}

void GTAHeader::setDescription(const QString & iDescription)
{
    _Description = iDescription;
}

QString GTAHeader::getValue(const int& idParentId, const int& idChildId , bool& ioSuccess) const
{
    ioSuccess=false;
    QString osRetVal ="";
    if(!_pHeaderDom)
        return osRetVal;

    QDomNodeList headerNodeList=_pHeaderDom->elementsByTagName(HEADER_TAG);
    int headernodeListSize = headerNodeList.size();

    if(headernodeListSize <= 0)
        return osRetVal;

    QDomNode headerDome = headerNodeList.at(0);//Only one "HEADER" node.
    QDomNodeList headerNodes = headerDome.childNodes();

    QString sParetnIdVal = QString::number(idParentId);
    QString sChildIdVal  =QString::number(idChildId);
    for(int i=0;i<headerNodes.size();++i)
    {
        QDomElement currElement = headerNodes.at(i).toElement();
        QString testDbg1= currElement.attribute(NAME_ATTRIBUTE);//TODO REMOVE AFTER TEST
        QString paretnID=currElement.attribute(ID_ATTRIBUTE);
        if(sParetnIdVal==paretnID)
        {
            QDomNodeList childNodes = currElement.childNodes();
            if (childNodes.isEmpty())
            {
                if (idChildId==0)
                {
                    ioSuccess=true;
                    return currElement.attribute(VAL_ATTRIBUTE);
                }

                return osRetVal;
            }
            else
            {
                for(int j=0;j<childNodes.size();++j)
                {
                    QDomElement currElement1 = childNodes.at(j).toElement();
                    QString testDbg2 = currElement1.tagName(); // todo remove after debug
                    QString childID=currElement1.attribute(ID_ATTRIBUTE);
                    if (childID==sChildIdVal)
                    {
                        osRetVal= currElement1.attribute(VAL_ATTRIBUTE);
                        ioSuccess=true;
                        return osRetVal;
                    }
                }
            }
        }

    }


    return osRetVal;

}
//to do the tag name formation should be done by view , this function should return only the tag name based same
QString GTAHeader::getTagName(const int& dId)
{
    QString osRetVal ="";
    if(!_pHeaderDom)
        return osRetVal;

    QDomNodeList headerNodeList=_pHeaderDom->elementsByTagName(HEADER_TAG);
    int headernodeListSize = headerNodeList.size();
    if(headernodeListSize <= 0)
        return osRetVal;

    QDomNode headerDome = headerNodeList.at(0);//Only one "HEADER" node.
    QDomNodeList headerNodes = headerDome.childNodes();

    QString sIdVal = QString::number(dId);
    for(int i=0;i<headerNodes.size();++i)
    {
        QDomElement currElement = headerNodes.at(i).toElement();
        QString testDbg1= currElement.attribute(NAME_ATTRIBUTE);//TODO REMOVE AFTER TEST
        QString currId=currElement.attribute(ID_ATTRIBUTE);
        if(currId==sIdVal)
        {
            QDomNodeList childNodes = currElement.childNodes();
            if (childNodes.isEmpty())
            {
                osRetVal = currElement.attribute(NAME_ATTRIBUTE);
                QString mandatoryField = currElement.attribute(MANDATORY_ATTR);
                int dManField = mandatoryField.toInt();
                if(dManField&1)
                    osRetVal.append(SUPERSCRIPT1);
                if(dManField&2)
                    osRetVal.append(SUPERSCRIPT2);
                if (dManField&4)
                    osRetVal.append(SUPERSCRIPT3);
                if(dManField&8)
                    osRetVal.append(SUPERSCRIPT4);


                return osRetVal;

            }
            else
            {
                for (int cnt = 0;cnt<childNodes.size();++cnt)
                {
                    for(int j=0;j<childNodes.size();++j)
                    {
                        QDomElement currElement1 = childNodes.at(j).toElement();
                        QString testDbg2 = currElement1.tagName(); // todo remove after debug
                        QString childID=currElement1.attribute(ID_ATTRIBUTE);
                        QString mandatoryField = currElement1.attribute(MANDATORY_ATTR);
                        if (childID==QString::number(cnt))
                        {
                            osRetVal.append(currElement1.attribute(NAME_ATTRIBUTE));
                            int dManField = mandatoryField.toInt();
                            if(dManField&1)
                                osRetVal.append(SUPERSCRIPT1);
                            if(dManField&2)
                                osRetVal.append(SUPERSCRIPT2);
                            if (dManField&4)
                                osRetVal.append(SUPERSCRIPT3);
                            if(dManField&8)
                                osRetVal.append(SUPERSCRIPT4);

                            if (cnt<childNodes.size()-1)
                            {
                                osRetVal.append("/");
                            }

                        }
                    }
                }

            }
        }

    }


    return osRetVal;

}
void GTAHeader::setMaxID(const int& isID)
{
    _maxID=isID;
}
void GTAHeader::setMaxChildrenID(const int& isID)
{
    _maxChildID=isID;
}

int GTAHeader::getMaxID()const
{
    return _maxID;
}
int GTAHeader::getMaxChildID()const
{
    return _maxChildID;
}
bool GTAHeader::setValue(const int& idParentId, const int& idChildId,const QString& isValue)
{
    bool bOK = false;

    if (NULL==_pHeaderDom)
        return bOK;

    QDomNodeList headerNodeList=_pHeaderDom->elementsByTagName(HEADER_TAG);
    int headernodeListSize = headerNodeList.size();
    if(headernodeListSize <= 0)
        return bOK;

    QDomNode headerDome = headerNodeList.at(0);//Only one "HEADER" node.
    QDomNodeList headerNodes = headerDome.childNodes();

    QString sParetnIdVal = QString::number(idParentId);
    QString sChildIdVal  =QString::number(idChildId);
    for(int i=0;i<headerNodes.size();++i)
    {
        QDomElement currElement = headerNodes.at(i).toElement();
        //QString testDbg1= currElement.attribute(NAME_ATTRIBUTE);//TODO REMOVE AFTER TEST
        QString paretnID=currElement.attribute(ID_ATTRIBUTE);
        if(sParetnIdVal==paretnID)
        {
            QDomNodeList childNodes = currElement.childNodes();
            if (childNodes.isEmpty())
            {
                //Header info
                if (idChildId==0)
                {
                    QString sName = currElement.attribute(NAME_ATTRIBUTE);
                    QString tagName = currElement.tagName();
                    if (tagName=="HEADERDETAILS")
                        _Name = isValue;

                    if (sName=="DESIGNER")
                        _DesignerName=isValue;


                    currElement.setAttribute(VAL_ATTRIBUTE,isValue);
                    _editMode=true;
                    return true;
                }

            }
            else
            {
                for(int j=0;j<childNodes.size();++j)
                {
                    QDomElement currElement1 = childNodes.at(j).toElement();
                    //QString testDbg2 = currElement1.tagName(); // todo remove after debug
                    QString childID=currElement1.attribute(ID_ATTRIBUTE);
                    if (childID==sChildIdVal)
                    {
                        currElement1.setAttribute(VAL_ATTRIBUTE,isValue);
                        QString valueAfterSetting = currElement1.attribute(VAL_ATTRIBUTE);
                        _editMode=true;
                        return true;
                    }
                }
            }
        }

    }

    if (bOK)
    {
        _editMode=true;
    }
    return bOK;
}


bool GTAHeader::inEditMode()const
{
    return _editMode;
}
void GTAHeader::dumpDomNOde()
{
    QString s2;
    QTextStream os2(&s2);
    os2 << "\nStream *****\n" << *_pHeaderDom;
    qDebug(qPrintable(s2));
}

bool GTAHeader::editHeaderItem(const int& iRow,const QString& iFieldName, bool isMandatory,const QString& iFieldValue,const QString& iDescription, bool showInLTRA )
{

    bool rc = false;
    int existingIndex = getIndex(iFieldName);
    if(-1==existingIndex || iRow == existingIndex )
    {
        GTAheaderNode node;
        node._isMandatory=isMandatory;
        node._Value=iFieldValue;
        node._Name = iFieldName;
        node._Description = iDescription;
        node._showInLTRA = showInLTRA;
        _lstOfFields.replace(iRow,node);
        if (!iFieldValue.isEmpty())
            _mandatoryFieldUnfilled.removeAll(iRow);
        rc=true;
        
    }
    else
        _LastError = QString("Header already contains tag named%1").arg(iFieldName);

    return rc;
}
bool GTAHeader::addHeaderItem(const QString& iFieldName, bool isMandatory,QString iFieldValue,QString iDescription, bool showInLTRA /*,QHash<QString , QString>& otherDataMap*/)
{

    bool rc = false;
    if(-1==getIndex(iFieldName))
    {
        GTAheaderNode node;
        node._isMandatory=isMandatory;
        node._Value=iFieldValue;
        node._Name = iFieldName;
        node._Description = iDescription;
        node._showInLTRA = isMandatory || showInLTRA || isDefaultMandatoryField(iFieldName);

        _lstOfFields.append(node);
        rc=true;
        //if (!otherDataMap.isEmpty())
        //{
        //    QHashIterator<QString, QString> i(otherDataMap);
        //    while (i.hasNext()) {
        //        i.next();
        //        _hshOtherData.insert(i.key(),i.value());
        //    }
        //}
    }
    else
        _LastError = QString("Header already contains tag named%1").arg(iFieldName);

    return rc;
}
int GTAHeader::getIndex(const QString& iFieldName )const
{
    int index=-1;
    for (int i=0;i<_lstOfFields.count();i++)
    {
        GTAheaderNode node = _lstOfFields.at(i);
        index++;
        if(node._Name == iFieldName)
        {
            return index;
        }
    }
    return -1;
}
bool GTAHeader::isNodeMandatory(const QString& iFieldName)const
{
    int dIndex = getIndex(iFieldName);
    if(-1!=dIndex)
    {
        return (_lstOfFields.at(dIndex))._isMandatory;
    }

    return false;
}

/**
 * @brief Checks If the selected header Field is default mandatory field required to print LTRA
 * @param strFieldName: name of the header field
 * @return bool : true if field is default mandatory
*/
bool GTAHeader::isDefaultMandatoryField(QString strFieldName) const
{
    return (strFieldName == "Designer Name") || (strFieldName == "Specification Issue/Requirement")
        || (strFieldName == "Test_Number") || (strFieldName == "Facility")
        || (strFieldName == "DESCRIPTION") || (strFieldName == "LTRA_Reference_Number")
        || (strFieldName == "LTRA_Title");
}

bool GTAHeader::getDomElement(QDomDocument iContextDoc, QDomElement& oHdrRootNode)const

{
    bool rC = false;
    if(!iContextDoc.isNull())
    {
        foreach(GTAheaderNode node, _lstOfFields)
        {
            QDomElement itemElement = iContextDoc.createElement(XNODE_ITEM);
            itemElement.setAttribute(XATTR_NAME,node._Name);
            itemElement.setAttribute(XATTR_VAL,node._Value);
            itemElement.setAttribute(XATTR_DESCRIPTION,node._Description);
            itemElement.setAttribute(XATTR_MANDATORY,node._isMandatory);
            itemElement.setAttribute(XATTR_SHOW_IN_LTRA, node._showInLTRA);
            oHdrRootNode.appendChild(itemElement);


        }
        rC = true;

    }
    
    return rC;
}

bool GTAHeader::editField(const QString& iFieldName,const QString& iFieldValue)
{
    int dIndex = getIndex(iFieldName);
    if(dIndex>-1)
    {
        GTAheaderNode node =_lstOfFields.takeAt(dIndex);
        node._Value=iFieldValue;
        _lstOfFields.insert(dIndex,node);
        if (!iFieldValue.isEmpty())
            _mandatoryFieldUnfilled.removeAll(getIndex(iFieldName));
        return true;
    }

    return false;
}

/**
 * @brief Update value of showLTRA flag for given field name
 * @param iFieldName: name of the header field
 * @param value : new value of showLTRA flag
 * @return bool : true if updated successfully
*/
bool GTAHeader::UpdateShowLTRAFlag(const QString& iFieldName, const bool& value)
{
    int dIndex = getIndex(iFieldName);
    if (dIndex > -1)
    {
        GTAheaderNode node = _lstOfFields.takeAt(dIndex);
        node._showInLTRA = value;
        _lstOfFields.insert(dIndex, node);
        
        return true;
    }

    return false;
}

QString GTAHeader::getFieldName(const int& iOrder)
{
    int itemSize = _lstOfFields.size();
    if (iOrder<itemSize)
    {
        GTAheaderNode node = _lstOfFields.at(iOrder);
        return node._Name;
    }
    else
        return QString();

}
QString GTAHeader::getFieldValue(const int& iOrder)
{
    int itemSize = _lstOfFields.size();
    if (iOrder<itemSize)
    {
        GTAheaderNode node = _lstOfFields.at(iOrder);
        return node._Value;
    }
    else
        return QString();

}

/**
 * @brief get the showLTRA value for the given row index
 * @param iOrder : index of the row in header dialog list
 * @return bool : true if field will be displayed in LTRA
*/
bool GTAHeader::getShowInLTRA(const int& iOrder)
{
    int itemSize = _lstOfFields.size();
    if (iOrder < itemSize)
    {
        GTAheaderNode node = _lstOfFields.at(iOrder);
        return node._showInLTRA;
    }
    else
    {
        return false;
    }
}


QString GTAHeader::getFieldDescription(const int& iOrder)
{
    int itemSize = _lstOfFields.size();
    if (iOrder<itemSize)
    {
        GTAheaderNode node = _lstOfFields.at(iOrder);
        return node._Description;
    }
    else
        return QString();

}
int GTAHeader::itemSize()const
{
    int size = _lstOfFields.size();
    return size;
}

bool GTAHeader::mandatoryAttributeFilled(QStringList& oEmptyManField ) 
{
    int size = _lstOfFields.size();
    bool rc = true;
    oEmptyManField.clear();
    for(int i=0;i<size;i++)
    {
        GTAheaderNode node = _lstOfFields.at(i);
        if (node._Name == "GTA_VER")
        {
            node._isMandatory=false;
            _lstOfFields.replace(i,node);
        }
        if(node._isMandatory==true && node._Value.isEmpty())
        {

            _mandatoryFieldUnfilled.append(i);
            oEmptyManField.append(node._Name);
            rc=false;
        }
    }
    return rc;
}
bool GTAHeader::insertEmptyNodes(const int& row, const int& noOfRows)
{
    bool rc = true;
    int size = _lstOfFields.size();
    if (row <= size)
    {
        for (int i=0;i<noOfRows;i++)
        {

            GTAheaderNode node;
            node._Description="";
            node._Value="";
            node._Name="";
            node._isMandatory=false;
            _lstOfFields.insert(i+row,node);
        }
        rc=true;

    }

    return rc;
}
bool GTAHeader::removeRows(const int& row, const int& noOfRows)
{
    bool rc = true;
    int size = _lstOfFields.size();
    if (row + noOfRows <= size)
    {
        for (int i=noOfRows + row ;i>=row;i--)
        {
            _lstOfFields.removeAt(i);
        }
        rc=true;

    }

    return rc;
}


void GTAHeader::insertDefaultHeaderNodes()
{
    QStringList nameList;
    nameList << "Tester Name" << "Designer Name" <<"Keywords" << "DESCRIPTION"
            << "Aircraft" ;
    //             << "Equipment" << "Supplier" << " Facility"
    //             << "Specification Issue/Requirement" << "Facility Applicability";


    foreach(QString fieldName , nameList)
    {
        addHeaderItem(fieldName,false);
    }
}

void GTAHeader::setLstOfFields(const QList<GTAheaderNode> &iList)
{
    _lstOfFields.clear();
    for(int i = 0; i < iList.count(); i++)
    {
        GTAheaderNode node = iList.at(i);
        _lstOfFields.append(node);
    }
}

/**
	* This function sets the version of GTA provided by user. Added for Requirement #357031 (Marine)
	* @iVersion: the version to be updated in QString format
	*/
void GTAHeader::setGTAVersion(const QString & iVersion,bool iOverwriteExisting)
{
    int idx = getIndex(XNODE_GTA_VER);
    //in case the version item is not present in header, always input one
    if (idx == -1)
    {
        _GTAVersion = iVersion;
        //addHeaderItem(XNODE_GTA_VER,true,iVersion,"GTA Version in which the elements was last modified");
    }
    //based on the input to overwrite or not, edit the version
    else
    {
        if (iOverwriteExisting)
            editField(XNODE_GTA_VER,iVersion);
        _GTAVersion = getFieldValue(idx);
    }
}
