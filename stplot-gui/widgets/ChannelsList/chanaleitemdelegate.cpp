#include "chanaleitemdelegate.h"
#include <QComboBox>
#include "qcustomplot.h"
#include <QDebug>
#include <channelmodel.h>

//ChanaleItemDelegate::ChanaleItemDelegate(QObject *parent)
//    : QStyledItemDelegate{parent}
//{

//}

void ChanaleItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    qDebug() << "paint";
    return QStyledItemDelegate::paint(painter, option, index);
}

QWidget *ChanaleItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    qDebug() << "createEditor";
    if((index.column() == 3) || (index.column() == 4))
    {

        QComboBox *editor = new QComboBox(parent);
        if(index.column() == 3)
            editor->addItems(ChannelModel::getDotStyle());
        else if(index.column() == 4)
            editor->addItems(ChannelModel::getLineStyle());

        //TODO if it posible get curent iteam
        return editor;

    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

QSize ChanaleItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    qDebug() << "sizeHint";
    return QStyledItemDelegate::sizeHint(option, index);
}

void ChanaleItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
//    qDebug() << "setEditorData";
    if((index.column() == 3) || (index.column() == 4))
    {
        QComboBox *dotStyleEditor = qobject_cast<QComboBox *>(editor);
        dotStyleEditor->setCurrentIndex(index.data().toInt());
    }
    else
        return QStyledItemDelegate::setEditorData(editor, index);
}

void ChanaleItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
//    qDebug() << "setModelData";
    if((index.column() == 3) || (index.column() == 4))
    {
        QComboBox *comboBoxEditor = qobject_cast<QComboBox *>(editor);
        model->setData(index, comboBoxEditor->currentIndex());
    }
    else
        return QStyledItemDelegate::setModelData(editor, model, index);
}
