#ifndef GTAFILESYSTEMMODEL_H
#define GTAFILESYSTEMMODEL_H

#pragma warning(push, 0)
#include <QFileSystemModel>
#pragma warning(pop)

#include "GTAViewModel.h"

class GTAViewModel_SHARED_EXPORT GTAFileSystemModel : public QFileSystemModel
{
public:
    GTAFileSystemModel(QObject *parent = 0);
    //QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // GTAFILESYSTEMMODEL_H
