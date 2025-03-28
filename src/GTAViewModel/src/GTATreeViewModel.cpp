#include "GTATreeViewModel.h"
#include "GTAUtil.h"
#include "GTACommandBase.h"


GTATreeViewModel::GTATreeViewModel(QObject *parent):QAbstractItemModel(parent)
{
    _ColumnList.clear(); // V26 editor view column enhancement #322480
    _RootNode = NULL;
}

GTATreeViewModel::GTATreeViewModel(GTATreeItem *iRootNode,QObject *parent):QAbstractItemModel(parent)
{
    // V26 editor view column enhancement #322480
    _ColumnList<<"Name"<<"Description"<<"GTA Version"<<"UUID"<<"Date Created"<<"Author"<<"Last Modified"<<"Validation Status"<<"Validator"<<"Date Validated"<<"Favourite"<<"Git status";
    _RootNode = iRootNode;
    //    setupModelData();
}

GTATreeViewModel::~GTATreeViewModel()
{

}


GTATreeItem *GTATreeViewModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return static_cast<GTATreeItem*>(index.internalPointer());
    }
    else
    {
        return _RootNode;
    }
}

QString GTATreeViewModel::getAbsolutePathForIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return QString();


    GTATreeItem *item = nodeFromIndex(index);
    if(!item)
        return QString();

    if(index.column() == COLUMN_NAME)
    {
        QString extension = item->getExtension();
       /* if(extension == "folder") //TODO ALLOW MOVE FOLDER 
            return QString();
        else*/
            return item->getAbsoluteRelPath();
    }
	return QString();
}

QString GTATreeViewModel::getExtensionForIndex(const QModelIndex &index)
{
	if (!index.isValid())
		return QString();


	GTATreeItem *item = nodeFromIndex(index);
	if (!item)
		return QString();

	if (index.column() == COLUMN_NAME)
	{
		return item->getExtension();
	}
	return QString();
}

QString GTATreeViewModel::getFileUUIDForIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return QString();


    GTATreeItem *item = nodeFromIndex(index);
    if(!item)
        return QString();

    if(index.column() == COLUMN_UUID)
    {
        return item->getUUID();
    }
	return QString();
}

QVariant GTATreeViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    switch(role)
    {
    case Qt::DisplayRole:
        {
            GTATreeItem *item = nodeFromIndex(index);
            if(!item)
                return QVariant();

            if(index.column() == COLUMN_NAME)
            {
                return item->getName();
            }
            else if(index.column() == COLUMN_DESCRIPTION)
            {
                return item->getDescription();
            }
            else if(index.column() == COLUMN_FAVOURITES)
            {
                QString fav;
                if(item->isFavourite())
                    fav = "Yes";
                else
                    fav = QString("");
                return fav;
            }
            else if(index.column() == COLUMN_UUID)
            {
                return item->getUUID();
            }
            // V26 editor view column enhancement #322480 ------>
            else if(index.column() == COLUMN_AUTHOR)
            {
                return item->getAuthorName();
            }
            else if(index.column() == COLUMN_MODIFIED_DATE)
            {
                return item->getModifiedDate();
            }
            else if(index.column()== COLUMN_VALIDATION_STATUS)
            {
                return item->getValidationStatus();
            }
            else if(index.column()== COLUMN_VALIDATED_BY)
            {
                return item->getValidatedBy();
            }
            else if(index.column() == COLUMN_VALIDATION_DATE)
            {
                return item->getValidationDate();
            }
            else if (index.column() == COLUMN_CREATED_DATE)
            {
                return item->getCreatedDate();
            }
            else if (index.column() == COLUMN_GTA_VERSION)
            {
                return item->getGtaVersion();
            }
            else if (index.column() == COLUMN_GIT_STATUS) {
                return item->getGitStrStatus();
            }
            // <-------V26 editor view column enhancement #322480

        }
        break;
    case Qt::DecorationRole:
        {
            QVariant var;
            if (index.column()==COLUMN_NAME)
            {

                GTATreeItem *item = nodeFromIndex(index);
                auto strStatus = item->getGitStrStatus();
                if(item)
                {
                    QPixmap icon;
                    QString extension = item->getExtension();
                    //ObjectIconFile
                    if (item->getGitStatus() != GitStatus::Unknown) {
                        GitStatus status = item->getGitStatus();
                        icon = assignIcon(status);
                    }
                    else if (item->getSvnStatus().compare("Not SVN",Qt::CaseInsensitive))
                    {
                        //not a svn repo
                        if(extension == "folder")
                            icon = QPixmap(":/images/forms/img/Folder.png");
                        else if(extension == "obj")
                            icon = QPixmap(":/images/ObjectFile");
                        else if(extension == "fun")
                            icon = QPixmap(":/images/FunctionFile");
                        else if(extension == "pro")
                            icon = QPixmap(":/images/ProcedureFile");
                        else if(extension == "seq")
                            icon = QPixmap(":/images/SequenceFile");
                    }
                    else
                    {
                        QString svnStatus = item->getSvnStatus();
                        if(extension == "folder")
                        {
                            if(svnStatus.compare("A",Qt::CaseInsensitive) == 0)   //added
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/AddedIcon_folder.png");

                            else if (svnStatus.compare("C",Qt::CaseInsensitive) == 0)   //conflicted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ConflictIcon_folder.png");

                            else if (svnStatus.compare("D",Qt::CaseInsensitive) == 0)   //deleted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/DeletedIcon_folder.png");

                            else if (svnStatus.compare("I",Qt::CaseInsensitive) == 0)   //Ignored
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/IgnoredIcon_folder.png");

                            else if (svnStatus.compare("M",Qt::CaseInsensitive) == 0)   //modified
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ModifiedIcon_folder.png");

                            else if (svnStatus.compare("?",Qt::CaseInsensitive) == 0)   //unversioned
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/UnversionedIcon_folder.png");

                            else if ((svnStatus.compare("K",Qt::CaseInsensitive) == 0) || (svnStatus.compare("O",Qt::CaseInsensitive) == 0)
                                || (svnStatus.compare("T",Qt::CaseInsensitive) == 0) || (svnStatus.compare("B",Qt::CaseInsensitive) == 0))  //locked
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/LockedIcon_folder.png");

                            else    //up to date
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/NormalIcon_folder.png");
                        }
                        else if(extension == "obj")
                        {
                            if(svnStatus.compare("A",Qt::CaseInsensitive) == 0)   //added
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/AddedIcon_obj.png");

                            else if (svnStatus.compare("C",Qt::CaseInsensitive) == 0)   //conflicted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ConflictIcon_obj.png");

                            else if (svnStatus.compare("D",Qt::CaseInsensitive) == 0)   //deleted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/DeletedIcon_obj.png");

                            else if (svnStatus.compare("I",Qt::CaseInsensitive) == 0)   //Ignored
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/IgnoredIcon_obj.png");

                            else if (svnStatus.compare("M",Qt::CaseInsensitive) == 0)   //modified
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ModifiedIcon_obj.png");

                            else if (svnStatus.compare("?",Qt::CaseInsensitive) == 0)   //unversioned
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/UnversionedIcon_obj.png");

                            else if ((svnStatus.compare("K",Qt::CaseInsensitive) == 0) || (svnStatus.compare("O",Qt::CaseInsensitive) == 0)
                                || (svnStatus.compare("T",Qt::CaseInsensitive) == 0) || (svnStatus.compare("B",Qt::CaseInsensitive) == 0))  //locked
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/LockedIcon_obj.png");

                            else    //up to date
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/NormalIcon_obj.png");

                        }
                        else if(extension == "fun")
                        {

                            if(svnStatus.compare("A",Qt::CaseInsensitive) == 0)   //added
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/AddedIcon_fun.png");

                            else if (svnStatus.compare("C",Qt::CaseInsensitive) == 0)   //conflicted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ConflictIcon_fun.png");

                            else if (svnStatus.compare("D",Qt::CaseInsensitive) == 0)   //deleted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/DeletedIcon_fun.png");

                            else if (svnStatus.compare("I",Qt::CaseInsensitive) == 0)   //Ignored
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/IgnoredIcon_fun.png");

                            else if (svnStatus.compare("M",Qt::CaseInsensitive) == 0)   //modified
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ModifiedIcon_fun.png");

                            else if (svnStatus.compare("?",Qt::CaseInsensitive) == 0)   //unversioned
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/UnversionedIcon_fun.png");

                            else if ((svnStatus.compare("K",Qt::CaseInsensitive) == 0) || (svnStatus.compare("O",Qt::CaseInsensitive) == 0)
                                || (svnStatus.compare("T",Qt::CaseInsensitive) == 0) || (svnStatus.compare("B",Qt::CaseInsensitive) == 0))  //locked
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/LockedIcon_fun.png");

                            else    //up to date
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/NormalIcon_fun.png");

                        }
                        else if(extension == "pro")
                        {

                            if(svnStatus.compare("A",Qt::CaseInsensitive) == 0)   //added
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/AddedIcon_pro.png");

                            else if (svnStatus.compare("C",Qt::CaseInsensitive) == 0)   //conflicted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ConflictIcon_pro.png");

                            else if (svnStatus.compare("D",Qt::CaseInsensitive) == 0)   //deleted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/DeletedIcon_pro.png");

                            else if (svnStatus.compare("I",Qt::CaseInsensitive) == 0)   //Ignored
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/IgnoredIcon_pro.png");

                            else if (svnStatus.compare("M",Qt::CaseInsensitive) == 0)   //modified
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ModifiedIcon_pro.png");

                            else if (svnStatus.compare("?",Qt::CaseInsensitive) == 0)   //unversioned
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/UnversionedIcon_pro.png");

                            else if ((svnStatus.compare("K",Qt::CaseInsensitive) == 0) || (svnStatus.compare("O",Qt::CaseInsensitive) == 0)
                                || (svnStatus.compare("T",Qt::CaseInsensitive) == 0) || (svnStatus.compare("B",Qt::CaseInsensitive) == 0))  //locked
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/LockedIcon_pro.png");

                            else    //up to date
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/NormalIcon_pro.png");

                        }
                        else if(extension == "seq")
                        {

                            if(svnStatus.compare("A",Qt::CaseInsensitive) == 0)   //added
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/AddedIcon_seq.png");

                            else if (svnStatus.compare("C",Qt::CaseInsensitive) == 0)   //conflicted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ConflictIcon_seq.png");

                            else if (svnStatus.compare("D",Qt::CaseInsensitive) == 0)   //deleted
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/DeletedIcon_seq.png");

                            else if (svnStatus.compare("I",Qt::CaseInsensitive) == 0)   //Ignored
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/IgnoredIcon_seq.png");

                            else if (svnStatus.compare("M",Qt::CaseInsensitive) == 0)   //modified
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/ModifiedIcon_seq.png");

                            else if (svnStatus.compare("?",Qt::CaseInsensitive) == 0)   //unversioned
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/UnversionedIcon_seq.png");

                            else if ((svnStatus.compare("K",Qt::CaseInsensitive) == 0) || (svnStatus.compare("O",Qt::CaseInsensitive) == 0)
                                || (svnStatus.compare("T",Qt::CaseInsensitive) == 0) || (svnStatus.compare("B",Qt::CaseInsensitive) == 0))  //locked
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/LockedIcon_seq.png");

                            else    //up to date
                                icon = QPixmap(":/images/forms/img/svnIconsGTAFlat/NormalIcon_seq.png");

                        }
                    }
                    var = var.fromValue(icon);
                    return var;
                }
            }
            return QVariant();
        }
        break;
    case Qt::ForegroundRole:
        if(index.column() == COLUMN_VALIDATION_STATUS)
        {
            if(index.data() == QVariant(XNODE_VALIDATED))
            {
                return QColor(43,97,0);
            }
            else if (index.data() == XNODE_NOT_VALIDATED)
            {
                return QColor(136,0,21);
            }
            else
            {
                return QColor(64,64,109);
            }
        }
        return QVariant();
        break;
    case Qt::BackgroundRole:
        if(index.column() == COLUMN_VALIDATION_STATUS)
        {
            if(index.data() == XNODE_VALIDATED)
            {
                return QColor(198,239,206);
            }
            else if(index.data() == XNODE_NOT_VALIDATED)
            {
                return QColor(255,199,206);
            }
            else
            {
                if (index.row()%2==0)
                {
                    return QColor(255,199,206,65);
                }
                return QColor(255,199,206,125);
            }
        }

        if (index.row()%2==0)
            return QColor(0,10,0,15);
        else
            return QVariant();
    }
    return QVariant();
}


Qt::ItemFlags GTATreeViewModel::flags(const QModelIndex &index) const
{

    if (!index.isValid())
        return 0;

    Qt::ItemFlags itemFlags;
    itemFlags = QAbstractItemModel::flags(index);
    return Qt::ItemIsDragEnabled | itemFlags;
}

QVariant GTATreeViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        if(section == COLUMN_NAME)
        {
            return QVariant("Name");
        }
        else  if(section == COLUMN_DESCRIPTION)
        {
            return QVariant("Description");
        }
        // V26 editor view column enhancement #322480
        else if(section == COLUMN_FAVOURITES)
        {
            return QVariant("Favourite");
        }
        else if(section == COLUMN_UUID)
        {
            return QVariant("UUID");
        }
        else if(section == COLUMN_AUTHOR)
        {
            return QVariant("Author");
        }
        else if(section == COLUMN_MODIFIED_DATE)
        {
            return QVariant("Last Modified");
        }
        else if(section == COLUMN_VALIDATION_STATUS)
        {
            return QVariant("Validation Status");
        }
        else if(section == COLUMN_VALIDATED_BY)
        {
            return QVariant("Validator");
        }
        else if(section == COLUMN_VALIDATION_DATE)
        {
            return QVariant("Last Validation Date");
        }
        else if(section == COLUMN_CREATED_DATE)
        {
            return QVariant("Date Created");
        }
        else if(section == COLUMN_GTA_VERSION)
        {
            return QVariant("GTA Version");
        }
        else if (section == COLUMN_GIT_STATUS) {
            return QVariant("Git status");
        }
    }
    return QVariant();
}

QModelIndex GTATreeViewModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if(!_RootNode || row < 0 || column < 0)
        return QModelIndex();

    GTATreeItem *parentNode = nodeFromIndex(parentIndex);
    if(parentNode == NULL)
        return QModelIndex();

    QList<GTATreeItem*> lstChildren;
    if (parentNode->hasChildren())
        lstChildren = parentNode->getChildren();

    GTATreeItem *childNode = NULL;
    if(lstChildren.count())
    {
        childNode = parentNode->getChildren().value(row);
    }
    if(childNode == NULL)
        return QModelIndex();
    return createIndex(row,column,childNode);

}

QModelIndex GTATreeViewModel::parent(const QModelIndex &iIndex) const
{
    GTATreeItem *pNode = nodeFromIndex(iIndex);
    if (!pNode)
        return QModelIndex();

    GTATreeItem *parentNode = pNode->getParent();
    if (!parentNode)
        return QModelIndex();

    GTATreeItem *grandparentNode = parentNode->getParent();
    if (!grandparentNode)
        return QModelIndex();


    int row = grandparentNode->getChildren().indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}
int GTATreeViewModel::rowCount(const QModelIndex &parentIndex) const
{
    if (parentIndex.column() > 0)
        return 0;

    GTATreeItem *parentNode = nodeFromIndex(parentIndex);
    if(!parentNode)
        return 0;

    return parentNode->childCount();
}
int GTATreeViewModel::columnCount(const QModelIndex &) const
{
    return _ColumnList.count();
}

Qt::DropActions GTATreeViewModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

// V26 editor view column enhancement #322480
QStringList GTATreeViewModel::getColumnList() const
{
    return _ColumnList;
}

QPixmap GTATreeViewModel::assignIcon(GitStatus status) const {

    switch (status) {
    case GitStatus::Current:
        return QPixmap(":/images/forms/img/git/status_normal.png").scaled(16, 16);
    case GitStatus::IndexNew:
        return QPixmap(":/images/forms/img/git/status_added.png").scaled(16, 16);
    case GitStatus::IndexModified:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::IndexDeleted:
        return QPixmap(":/images/forms/img/git/status_deleted.png").scaled(16, 16);
    case GitStatus::IndexRenamed:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::IndexTypechange:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::WtNew:
        return QPixmap(":/images/forms/img/git/status_added.png").scaled(16, 16);
    case GitStatus::WtModified:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::WtDeleted:
        return QPixmap(":/images/forms/img/git/status_deleted.png").scaled(16, 16);
    case GitStatus::WtTypechange:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::WtRenamed:
        return QPixmap(":/images/forms/img/git/status_modified.png").scaled(16, 16);
    case GitStatus::WtUnreadable:
        return QPixmap(":/images/forms/img/git/status_nonversioned.png").scaled(16, 16);
    case GitStatus::Ignored:
        return QPixmap(":/images/forms/img/git/status_ignored.png").scaled(16, 16);
    case GitStatus::Conflicted:
        return QPixmap(":/images/forms/img/git/status_conflicted.png").scaled(16, 16);
    case GitStatus::NotFound:
        return QPixmap(":/images/forms/img/git/status_nonversioned.png").scaled(16, 16);
    case GitStatus::Unknown:
        return QPixmap("");
    default:
        return QPixmap("");
    }
}
