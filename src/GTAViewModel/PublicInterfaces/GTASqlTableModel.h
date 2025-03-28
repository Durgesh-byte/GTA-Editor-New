#ifndef GTASQLTABLEMODEL_H
#define GTASQLTABLEMODEL_H

#include "GTAViewModel.h"
#include "GTAUtil.h"

#pragma warning(push, 0)
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QModelIndex>
#pragma warning(pop)


class GTAViewModel_SHARED_EXPORT GTASqlTableModel : public QSqlQueryModel
{
public:
    GTASqlTableModel(QObject *parent,QSqlDatabase &iDb);
    ~GTASqlTableModel();
    void updateToolName(const QString &iCriteria, const QString &iCriteriaVal, const QString &iToolName);
    void updateToolName(const QString &iToolName);
    void createEquipmentTable();
    void refresh();
    void search(const QString &iColName, const QString &iSrchStr);
private:
    QSqlDatabase _Database;

private:
    void createIndex();
    void dropIndex();

};

#endif // GTASQLTABLEMODEL_H
