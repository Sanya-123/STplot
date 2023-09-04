#include "chanaleitemdelegate.h"
#include <QComboBox>
#include "qcustomplot.h"
#include <QDebug>
#include <channelmodel.h>
#include <QColorDialog>

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
        return editor;

    }
    else if(index.column() == 2)
    {
        QColorDialog *colorDialog = new QColorDialog(parent);
        return colorDialog;
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
    if(index.column() == 2)
    {
        QColorDialog *colorDialog = qobject_cast<QColorDialog *>(editor);
        colorDialog->setCurrentColor(QColor(index.data().toString()));
    }
    else if((index.column() == 3) || (index.column() == 4))
    {
        QComboBox *dotStyleEditor = qobject_cast<QComboBox *>(editor);
        dotStyleEditor->setCurrentText(index.data().toString());
    }
    else
        return QStyledItemDelegate::setEditorData(editor, index);
}

void ChanaleItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == 2)
    {
        QColorDialog *colorDialog = qobject_cast<QColorDialog *>(editor);
        model->setData(index, colorDialog->currentColor().name());
    }
    else if((index.column() == 3) || (index.column() == 4))
    {
        QComboBox *comboBoxEditor = qobject_cast<QComboBox *>(editor);
        model->setData(index, comboBoxEditor->currentIndex());
    }
    else
        return QStyledItemDelegate::setModelData(editor, model, index);
}
