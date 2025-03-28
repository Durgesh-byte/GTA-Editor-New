#ifndef GTACOMBOBOXDELEGATORLOGRESULTV_H
#define GTACOMBOBOXDELEGATORLOGRESULTV_H

#include <QItemDelegate>
#include <QWidget>

class GTAComboBoxDelegatorLogResulTV : public QItemDelegate
{

    Q_OBJECT
public:
    GTAComboBoxDelegatorLogResulTV(QObject *parent);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QStringList _ComboItems;

};



#endif // GTACOMBOBOXDELEGATORLOGRESULTV_H
