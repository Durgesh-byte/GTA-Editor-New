#include "GTAComboBoxDelegatorLogResulTV.h"
#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>
#include <iostream>


GTAComboBoxDelegatorLogResulTV::GTAComboBoxDelegatorLogResulTV(QObject *parent):QItemDelegate(parent)
{

    _ComboItems.append("OK");
    _ComboItems.append("NA");
    _ComboItems.append("KO");


}



QWidget *GTAComboBoxDelegatorLogResulTV::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QComboBox* editor = new QComboBox(parent);
    editor->addItems(_ComboItems);
    return editor;
}

void GTAComboBoxDelegatorLogResulTV::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    int value = index.model()->data(index, Qt::EditRole).toUInt();
    comboBox->setCurrentIndex(value);
}

void GTAComboBoxDelegatorLogResulTV::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

void GTAComboBoxDelegatorLogResulTV::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void GTAComboBoxDelegatorLogResulTV::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionComboBox myOption;// = option;
    if(index.row() > -1 && index.row() < _ComboItems.count())
    {
        myOption.rect = option.rect;
        QString text = _ComboItems[index.row()];
        myOption.currentText = text;
        myOption.editable = false;
     //   myOption.init();
        QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &myOption, painter);
        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &myOption, painter);


    }

}
