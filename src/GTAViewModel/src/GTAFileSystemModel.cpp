#include "GTAFileSystemModel.h"

#pragma warning(push, 0)
#include <QFileIconProvider>
#include <QIcon>
#pragma warning(pop)

//TODO: Delete this from project (it's quite embeded with forward delcarations and such so not entirerly trivial)
GTAFileSystemModel::GTAFileSystemModel(QObject *parent /* = 0 */) : QFileSystemModel(parent)
{
}
//QVariant GTAFileSystemModel::data(const QModelIndex &index, int role) const
//{
//    if( role == Qt::DecorationRole )
//    {
//        QFileInfo info = GTAFileSystemModel::fileInfo(index);
//
//        if(info.isFile())
//        {
//            QFileIconProvider ip;
//            QIcon icon=ip.icon(info);
//
//            //if(info.suffix() == "obj")
//            //    icon.addPixmap( QPixmap(":/images/ObjectFile"),QIcon::Normal,QIcon::On);
//            //else if(info.suffix() == "fun")
//            //    icon.addPixmap (QPixmap(":/images/FunctionFile"),QIcon::Normal,QIcon::On);
//            //if(info.suffix() == "pro")
//            //    icon.addPixmap( QPixmap(":/images/ProcedureFile"),QIcon::Normal,QIcon::On);//pick the icon depending on the extension
//            //if(info.suffix() == "seq")
//            //    icon.addPixmap( QPixmap(":/images/SequenceFile"),QIcon::Normal,QIcon::On);//pick the icon depending on the extension
//            //if(info.suffix() == "tmpl")
//            //    icon.addPixmap(  QPixmap(":/images/TemplateFile"),QIcon::Normal,QIcon::On);//pick the icon depending on the extension
//
//            return icon;//setPixmapSize(QIcon::Small,QSize(16,16));
//        }
//    }
//    return QFileSystemModel::data(index, role);
//}
