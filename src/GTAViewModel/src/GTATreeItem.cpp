#include "GTATreeItem.h"

GTATreeItem::GTATreeItem()
{
    _isFavourite = false;
    _UUID = QString();
    _CreatedDate = QString();
    _ModifiedDate = QString();
    _AuthorName= QString();
    _ValidationStatus= QString();
    _ValidatedBy= QString();
    _ValidationDate= QString();
    _GtaVersion= QString();
    _MaxTimeEstimated = QString();
    _GitStatusStr = QString();
}
GTATreeItem::GTATreeItem(const QString &iName, const QString &iAbsRelPath, const QString &iRelpath, const QString &iExtension,const QString &iDesc)
{
    _Name = iName;
    _AbsoluteRelPath = iAbsRelPath;
    _RelativePath = iRelpath;
    _Extension = iExtension;
    _Description = iDesc;
    _Parent = NULL;
    _isFavourite = false;
    _UUID = QString();
    _ModifiedDate = QString();
    _AuthorName= QString();
    _ValidationStatus= QString();
    _ValidatedBy= QString();
    _ValidationDate= QString();
    _CreatedDate = QString();
    _GtaVersion= QString();
    _MaxTimeEstimated = QString();
    _GitStatusStr = QString();
}

GTATreeItem::GTATreeItem(const GTATreeItem &Rhs)
{
    
    setName(Rhs.getName());
    setAbsoluteRelPath(Rhs.getAbsoluteRelPath());
    setRelativePath(Rhs.getRelativePath());
    setExtension(Rhs.getExtension());
    setDescription(Rhs.getDescription());
    setIsFavourite(Rhs.isFavourite());
    setParent(NULL);
    setUUID(Rhs.getUUID());
    // V26 editor view column enhancement #322480 ------>
    setCreatedDate(Rhs.getCreatedDate());
    setAuthorName(Rhs.getAuthorName());
    setModifiedDate(Rhs.getModifiedDate());
    setValidationStatus(Rhs.getValidationStatus());
    setValidatedBy(Rhs.getValidatedBy());
    setValidationDate(Rhs.getValidationDate());
    setGtaVersion(Rhs.getGtaVersion());
    // <-------V26 editor view column enhancement #322480
    setSvnStatus(Rhs.getSvnStatus());
    setMaxTimeEstimated(Rhs.getMaxTimeEstimated());
    setGitStatus(Rhs.getGitStatus());
}

GTATreeItem::~GTATreeItem()
{
    //  if(!_LstChildren.isEmpty())
    qDeleteAll(_LstChildren);
    _LstChildren.clear();
}

void GTATreeItem::setName(const QString &iName)
{
    _Name = iName;
}

QString GTATreeItem::getName()const
{
    return _Name;
}
void GTATreeItem::setUUID(const QString &iUUID)
{
    _UUID = iUUID;
}
QString GTATreeItem::getUUID()const
{
    return _UUID;
}
void GTATreeItem::setAbsoluteRelPath(const QString &iPath)
{
    _AbsoluteRelPath = iPath;
}

QString GTATreeItem::getAbsoluteRelPath()const
{
    return _AbsoluteRelPath;
}

void GTATreeItem::setRelativePath(const QString &iPath)
{
    _RelativePath = iPath;
}

QString GTATreeItem::getRelativePath()const
{
    return _RelativePath;
}

void GTATreeItem::addChild(GTATreeItem *item)
{
    if(item)
    {
        item->setParent(this);
        _LstChildren.append(item);
    }
}

QList<GTATreeItem*> GTATreeItem::getChildren()const
{
    return _LstChildren;
}

QList<GTATreeItem*> GTATreeItem::getAllChildren()
{
    QList<GTATreeItem*> outChildren;
    this->internalGetAllChildren(outChildren);
    return outChildren;
}

void GTATreeItem::internalGetAllChildren(QList<GTATreeItem*>& outChildren) {
    outChildren.append(_LstChildren);
    for (const auto& child : _LstChildren) {
        child->internalGetAllChildren(outChildren);
    }
}

void GTATreeItem::setChildren(const QList<GTATreeItem*> &iLstChildren,bool isUpdateList)
{
    if(_LstChildren.count())
    {
        if(!isUpdateList)
            qDeleteAll(_LstChildren);
        _LstChildren.clear();
    }
    _LstChildren = iLstChildren;

}

bool GTATreeItem::hasChildren()const
{
    if (!_LstChildren.isEmpty())
        if(_LstChildren.count())
            return true;
    return false;
}

int GTATreeItem::childCount()
{
    return _LstChildren.count();

}

void GTATreeItem::clearChildren()
{
	_LstChildren.clear();
}

void GTATreeItem::setExtension(const QString &iExt)
{
    _Extension = iExt;
}

QString GTATreeItem::getExtension()const
{
    return _Extension;
}

void GTATreeItem::setDescription(const QString &iDesc)
{
    _Description = iDesc;
}

QString GTATreeItem::getDescription()const
{
    return _Description;
}

void GTATreeItem::setParent(GTATreeItem *iParent)
{
    _Parent = iParent;
}

GTATreeItem *GTATreeItem::getParent()const
{
    return _Parent;
}

bool GTATreeItem::isFavourite()const
{
    return _isFavourite;
}

void GTATreeItem::setIsFavourite(bool iVal)
{
    _isFavourite = iVal;
}

// V26 editor view column enhancement #322480 ------>

void GTATreeItem::setAuthorName(const QString &iAuthName)
{
    _AuthorName = iAuthName;
}

QString GTATreeItem::getAuthorName()const
{
    return _AuthorName;
}

void GTATreeItem::setModifiedDate(const QString &iModifiedDate)
{
    _ModifiedDate = iModifiedDate;
}

QString GTATreeItem::getModifiedDate()const
{
    return _ModifiedDate;
}

void GTATreeItem::setValidationStatus(const QString &iValidationStat)
{
    _ValidationStatus = iValidationStat;
}

QString GTATreeItem::getValidationStatus()const
{
    return _ValidationStatus;
}

void GTATreeItem::setValidatedBy(const QString &iValidatedBy)
{
    _ValidatedBy = iValidatedBy;
}

QString GTATreeItem::getValidatedBy()const
{
    return _ValidatedBy;
}

void GTATreeItem::setValidationDate(const QString &iValidatedDate)
{
    _ValidationDate = iValidatedDate;
}

QString GTATreeItem::getValidationDate()const
{
    return _ValidationDate;
}

void GTATreeItem::setGtaVersion(const QString &iGtaVersion)
{
    _GtaVersion = iGtaVersion;
}

QString GTATreeItem::getGtaVersion() const
{
    return _GtaVersion;
}

void GTATreeItem::setGitStatus(GitStatus status) {
    _GitStatus = status;
}

GitStatus GTATreeItem::getGitStatus() const {
    return _GitStatus;
}

QString GTATreeItem::getGitStrStatus() const {
    switch (_GitStatus) {
        case GitStatus::Current:
            return "Current";
        case GitStatus::IndexNew:
            return "New";
        case GitStatus::IndexModified:
            return "Modified";
        case GitStatus::IndexDeleted:
            return "Deleted";
        case GitStatus::IndexRenamed:
            return "Renamed";
        case GitStatus::IndexTypechange:
            return "Typechange";
        case GitStatus::WtNew:
            return "New";
        case GitStatus::WtModified:
            return "Modified";
        case GitStatus::WtDeleted:
            return "Deleted";
        case GitStatus::WtTypechange:
            return "Typechange";
        case GitStatus::WtRenamed:
            return "Renamed";
        case GitStatus::WtUnreadable:
            return "Unreadable";
        case GitStatus::Ignored:
            return "Ignored";
        case GitStatus::Conflicted:
            return "Conflicted";
        case GitStatus::NotFound:
            return "NotFound";
        case GitStatus::Unknown:
            return "Unknown";
        default:
            return "Unknown";
    }
}

void GTATreeItem::setSvnStatus(const QString &isvnStatus)
{
    _svnStatus = isvnStatus;
}

QString GTATreeItem::getSvnStatus()const
{
    return _svnStatus;
}

void GTATreeItem::setCreatedDate(const QString &iCreatedDate)
{
    _CreatedDate = iCreatedDate;
}

QString GTATreeItem::getCreatedDate()const
{
    return _CreatedDate;
}

/**
 * @brief Set the Maximum Time Estimated value
 * @param iMaxTimeEstimated 
*/
void GTATreeItem::setMaxTimeEstimated(const QString& iMaxTimeEstimated)
{
    _MaxTimeEstimated = iMaxTimeEstimated;
}

/**
 * @brief Get the Maximum Time Estimated value
 * @return QString
*/
QString GTATreeItem::getMaxTimeEstimated()const
{
    return _MaxTimeEstimated;
}

// <-------V26 editor view column enhancement #322480

QList<GTATreeItem*> GTATreeItem::getFolders()const
{
    QList<GTATreeItem*> folderItems;
    if(this->hasChildren())
    {
        folderItems = this->getChildren();
        for(int i = 0; i < folderItems.count(); i++ )
        {
            GTATreeItem* item = folderItems.at(i);
            if(item != NULL)
            {
                if(item->getExtension() != "folder")
                {
                    folderItems.removeAt(i);
                }
            }
        }

    }
    return folderItems;
}


bool GTATreeItem::removeItem(GTATreeItem *pItem)
{
    bool rc = false;

    if(pItem)
    {
        if(this->hasChildren())
        {
            QList<GTATreeItem*> children = this->getChildren();
            if(children.contains(pItem))
            {
                int index = children.indexOf(pItem);
                if(index >=0 && index < children.count())
                {
                    children.removeAt(index);
                    this->setChildren(children,true);
                    rc = true;
                    return rc;
                }

            }
            else
            {
                QList<GTATreeItem*> folderItems = this->getFolders();
                for(int i = 0; i < folderItems.count(); i++)
                {
                    GTATreeItem * folderItem = folderItems.at(i);
                    rc = folderItem->removeItem(pItem);
                    if(rc)
                        break;
                }
            }
        }
    }
    return rc;
}

void GTATreeItem::removeNonFavorite()
{
    /*if(getName() == "root")
    {
        int a = 10;
    }*/

    QList<GTATreeItem*> children = getChildren();
    QMutableListIterator<GTATreeItem*> itr(children);
    while(itr.hasNext())
    {
        GTATreeItem * pItem = itr.next();
        if(!pItem->isFavourite() && pItem->getExtension() != "folder")
        {
            itr.remove();
        }
        if(pItem->getExtension() == "folder")
        {
            pItem->removeNonFavorite();
        }
    }

    setChildren(children,true);

}

bool GTATreeItem::containsFile(const QString &iFileName)
{

    //    if(QString("%1/%2").arg(_AbsoluteRelPath,_Name )== iFileName)
    if(_AbsoluteRelPath == iFileName)
    {
        return true;
    }
    if(hasChildren())
    {
        QList<GTATreeItem*> childItems = getChildren();
        for(int i = 0; i < childItems.count(); i++)
        {
            GTATreeItem * childItem = childItems.at(i);
            bool foundItem = childItem->containsFile(iFileName);
            if(foundItem)
            {
                return true;
            }
        }
    }
    return false;
}

GTATreeItem* GTATreeItem::findNode(const QString &iSearchStr,SearchType searchType)
{
    if(searchType == GTATreeItem::ABS_REL_FILE_PATH)
    {
        if(_AbsoluteRelPath == iSearchStr)
        {
            return this;
        }
    }
    else if(searchType == GTATreeItem::UUID)
    {
        if(_UUID == iSearchStr)
            return this;
    }
    if(hasChildren())
    {
        QList<GTATreeItem*> childItems = getChildren();
        for(int i = 0; i < childItems.count(); i++)
        {
            GTATreeItem * childItem = childItems.at(i);
            GTATreeItem * foundItem = NULL;
            foundItem = childItem->findNode(iSearchStr, searchType);
            if(foundItem != NULL)
            {
                return foundItem;
            }

        }
    }

    return NULL;
}

QList<GTATreeItem*> GTATreeItem::findAllNodes(const QString &iSearchStr,SearchType searchType)
{

    QList<GTATreeItem*> nodeLst;

    if(searchType == GTATreeItem::ABS_REL_FILE_PATH)
    {
        if(_AbsoluteRelPath == iSearchStr)
        {
            nodeLst.append(this);
        }
    }
    else if(searchType == GTATreeItem::UUID)
    {
        if(_UUID == iSearchStr)
            nodeLst.append(this);
    }
    if(hasChildren())
    {
        QList<GTATreeItem*> childItems = getChildren();
        for(int i = 0; i < childItems.count(); i++)
        {
            GTATreeItem * childItem = childItems.at(i);
            QList <GTATreeItem *> foundItemLst;
            foundItemLst = childItem->findAllNodes(iSearchStr,searchType);
            if(!foundItemLst.isEmpty())
            {
                nodeLst.append(foundItemLst);

            }

        }
    }
    return nodeLst;
}
