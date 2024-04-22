#include "varfilter.h"
#include <QtDebug>
extern "C" {
#include "varcommon.h"
}

VarFilter::VarFilter(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool VarFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // accept if node is accepted
    if (filterAcceptsRowItself(source_row, source_parent)){
        return true;
    }
    //accept if any of the parents is accepted
    QModelIndex parent = source_parent;
    while (parent.isValid()) {
        QModelIndex grandparent = parent.parent();
        if (filterAcceptsRowItself(parent.row(), grandparent)){
            return true;
        }
        if (filterAcceptsRowItself(grandparent.row(), grandparent)){    // if parent is top level node there is some mess
            return true;
        }

        parent = parent.parent();
    }

    //accept if any of the children is accepted
    if (hasAcceptedChildren(source_row, source_parent)) {
        return true;
    }

    return false;

}

bool VarFilter::filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool VarFilter::hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex item = sourceModel()->index(source_row,0,source_parent);
    if (!item.isValid()) {
        //qDebug() << "item invalid" << source_parent << source_row;
        return false;
    }

    //check if there are children
    int childCount = item.model()->rowCount(item);
    if (childCount == 0)
        return false;

    for (int i = 0; i < childCount; ++i) {
        if (filterAcceptsRowItself(i, item))
            return true;
        if (hasAcceptedChildren(i, item))
            return true;
    }

    return false;
}
