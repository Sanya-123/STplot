#include "chanaleitemdelegate.h"
#include <QComboBox>
#include "qcustomplot.h"
#include <QDebug>
#include <channelmodel.h>
#include <QColorDialog>
#include <QLineEdit>
#include "chanalecustomeditor.h"

ChanaleItemDelegate::ChanaleItemDelegate(bool mathChanale, QObject *parent, QVector<VarChannel *> *chanales)
    : QStyledItemDelegate{parent}, isMathChanale(mathChanale), chanalesList(chanales)
{

}

void ChanaleItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    qDebug() << "paint";
    return QStyledItemDelegate::paint(painter, option, index);
}

QWidget *ChanaleItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    qDebug() << "createEditor";
    if((index.column() == 3) || (index.column() == 4) || (index.column() == 5))
    {

        QComboBox *editor = new QComboBox(parent);
        if(index.column() == 3)
            editor->addItems(ChannelModel::getDotStyle());
        else if(index.column() == 4)
            editor->addItems(ChannelModel::getLineStyle());
        else if(index.column() == 5)
            editor->addItems(ChannelModel::getLineWidth());
        return editor;

    }
    else if(index.column() == 2)
    {
        QColorDialog *colorDialog = new QColorDialog(parent);
        return colorDialog;
    }
    else if(index.column() == 0)
    {//defoult line edit is nite set curent display name in editor
        QLineEdit *lineEdit = new QLineEdit(parent);
        return lineEdit;
    }
    else if((index.column() == 1) && isMathChanale)
    {
        QStringList empty;
        ChanaleCustomEditor *editor = new ChanaleCustomEditor(empty, "", "", parent);
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
    if(index.column() == 2)
    {
        QColorDialog *colorDialog = qobject_cast<QColorDialog *>(editor);
        if(colorDialog != nullptr)
            colorDialog->setCurrentColor(QColor(index.data().toString()));
    }
    else if((index.column() == 3) || (index.column() == 4) || (index.column() == 5))
    {
        QComboBox *dotStyleEditor = qobject_cast<QComboBox *>(editor);
        if(dotStyleEditor != nullptr)
        {
            dotStyleEditor->setCurrentText(index.data().toString());
            dotStyleEditor->showPopup();
        }
    }
    else if(index.column() == 0)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        if(lineEdit != nullptr)
            lineEdit->setText(index.data().toString());//this is main different beatwen defoult editor and this
    }
    else if((index.column() == 1) && isMathChanale)
    {
        ChanaleCustomEditor *scriptDialog = qobject_cast<ChanaleCustomEditor *>(editor);
        if(scriptDialog != nullptr)
        {
            //update editor chanale lists
            QStringList chanaleNames;
            if(chanalesList != nullptr)
            {
                for(int i = 0; i < chanalesList->size(); i++)
                {
                    chanaleNames << chanalesList->at(i)->displayName();
                }
            }
            scriptDialog->updateChanaleNames(chanaleNames);
            scriptDialog->setScript(index.data().toString());
        }
    }
    else
        return QStyledItemDelegate::setEditorData(editor, index);
}

void ChanaleItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == 2)
    {
        QColorDialog *colorDialog = qobject_cast<QColorDialog *>(editor);
        if(colorDialog != nullptr)
            model->setData(index, colorDialog->currentColor().name());
    }
    else if((index.column() == 3) || (index.column() == 4) || (index.column() == 4))
    {
        QComboBox *comboBoxEditor = qobject_cast<QComboBox *>(editor);
        if(comboBoxEditor != nullptr)
            model->setData(index, comboBoxEditor->currentIndex());
    }
    else if(index.column() == 0)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        if(lineEdit != nullptr)
            model->setData(index, lineEdit->text());
    }
    else if((index.column() == 1) && isMathChanale)
    {
        ChanaleCustomEditor *scriptDialog = qobject_cast<ChanaleCustomEditor *>(editor);
        if(scriptDialog != nullptr)
        {
            if(scriptDialog->result() == QDialog::Accepted)
                model->setData(index, scriptDialog->getScipt());
        }
    }
    else
        return QStyledItemDelegate::setModelData(editor, model, index);
}
