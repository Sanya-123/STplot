#ifndef CHANALEITEMDELEGATE_H
#define CHANALEITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>
#include <QObject>

class ChanaleItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    ChanaleItemDelegate(bool mathChanale = false, QObject *parent = nullptr);
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const override;

//    using QStyledItemDelegate::QStyledItemDelegate;

//    void paint(QPainter *painter, const QStyleOptionViewItem &option,
//               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
//    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
//                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

private:
    bool isMathChanale;

};

#endif // CHANALEITEMDELEGATE_H
