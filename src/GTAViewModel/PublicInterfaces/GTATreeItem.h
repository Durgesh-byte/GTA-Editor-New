#ifndef GTATREEITEM_H
#define GTATREEITEM_H

#include "GTAViewModel.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QStandardItem>
#pragma warning(pop)

//TODO: Using compile-time built structures inside .dll linked libraries is generally a bad idea, 
//      however in this project .dll libraries are not used in intented way (they are basically static linked) - so it doesn't cause problems
//      in future would propably be better to use static link or rewrite these
#pragma warning (disable : 4251)

class GTAViewModel_SHARED_EXPORT GTATreeItem
{
public:

    enum SearchType{ABS_REL_FILE_PATH,UUID};

    GTATreeItem();
    GTATreeItem(const QString &iName, const QString &iAbsRelPath, const QString &iRelpath,const QString &iExtension, const QString &iDesc);
    GTATreeItem(const GTATreeItem &Rhs);
    ~GTATreeItem();

    void setName(const QString &iName);
    QString getName()const;


    void setUUID(const QString &iUUID);
    QString getUUID() const;


    void setAbsoluteRelPath(const QString &iPath);
    QString getAbsoluteRelPath()const;

    void setRelativePath(const QString &iPath);
    QString getRelativePath()const;

    void setExtension(const QString &iExt);
    QString getExtension()const;

    void setDescription(const QString &iDesc);
    QString getDescription()const;

    // V26 editor view column enhancement #322480 ------>
    void setAuthorName(const QString &iAuthName);
    QString getAuthorName()const;

    void setModifiedDate(const QString &iModifiedDate);
    QString getModifiedDate()const;

    void setValidationStatus(const QString &iValidationStat);
    QString getValidationStatus()const;

    void setValidatedBy(const QString &iValidatedBy);
    QString getValidatedBy()const;

    void setValidationDate(const QString &iValidatedDate);
    QString getValidationDate()const;

    void setCreatedDate(const QString &iCreatedDate);
    QString getCreatedDate()const;

    void setGtaVersion(const QString &iGtaVersion);
    QString getGtaVersion()const;

    void setGitStatus(GitStatus status);
    GitStatus getGitStatus() const;
    QString getGitStrStatus() const;

    void setMaxTimeEstimated(const QString& iMaxTimeEstimated);
    QString getMaxTimeEstimated()const;

    // <-------V26 editor view column enhancement #322480

    void setSvnStatus(const QString &isvnStatus);
    QString getSvnStatus()const;

    void addChild(GTATreeItem *item);
    QList<GTATreeItem*> getChildren()const;

    /**
     * @brief Get all children
    */
    QList<GTATreeItem*> getAllChildren();

    void setChildren(const QList<GTATreeItem*> &iLstChildren, bool isUpdateList = false);

    void setParent(GTATreeItem *iParent);
    GTATreeItem *getParent()const;

    bool hasChildren()const;
    int childCount();

	void clearChildren();

    bool isFavourite()const;
    void setIsFavourite(bool iVal);

    QList<GTATreeItem*> getFolders()const;
    bool removeItem(GTATreeItem *pItem);

    void removeNonFavorite();
    GTATreeItem* findNode(const QString &iSearchStr,SearchType searchType);
    QList<GTATreeItem*> findAllNodes(const QString &iSearchStr,SearchType searchType);

    bool containsFile(const QString &iFileName);

private:
    /**
     * @brief Internal recursive method to hide getinng output from input
    */
    void internalGetAllChildren(QList<GTATreeItem*>& outChildren);

    QList<GTATreeItem*> _LstChildren;
    QString _Name;
    QString _AbsoluteRelPath;
    QString _RelativePath;
    QString _Extension;
    QString _Description;
    QString _UUID;
    // V26 editor view column enhancement #322480 ------>
    QString _AuthorName;
    QString _ModifiedDate;
    QString _ValidationStatus;
    QString _ValidatedBy;
    QString _ValidationDate;
    QString _svnStatus;
    QString _CreatedDate;
    QString _GtaVersion;
    QString _MaxTimeEstimated;
    QString _GitStatusStr;
    bool _isFavourite;
    GTATreeItem* _Parent;
    GitStatus _GitStatus = GitStatus::Unknown;
    // <-------V26 editor view column enhancement #322480

};

#endif // GTATREEITEM_H
